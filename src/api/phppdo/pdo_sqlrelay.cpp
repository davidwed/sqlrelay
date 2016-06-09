// Copyright (c) 2013 David Muse
// See the file COPYING for more information

#include <config.h>
#define NEED_IS_BIT_TYPE_CHAR
#define NEED_IS_BOOL_TYPE_CHAR
#define NEED_IS_NUMBER_TYPE_CHAR
#define NEED_IS_FLOAT_TYPE_CHAR
#define NEED_IS_BLOB_TYPE_CHAR
#include <datatypes.h>
#include <defines.h>
#include <sqlrelay/sqlrclient.h>
#include <rudiments/stringbuffer.h>
#include <rudiments/singlylinkedlist.h>
#include <rudiments/charstring.h>
#include <rudiments/character.h>

#ifdef _WIN32
	#undef uid_t
	#undef gid_t
	#undef ssize_t
	#undef socklen_t
	#undef pid_t
	#undef mode_t
	#define PHP_WIN32
	#define ZEND_WIN32
	#define ZEND_DEBUG 0
	#define ZTS 1
#endif

extern "C" {

#include <php.h>
#include <php_ini.h>
#include <ext/standard/info.h>
#include <pdo/php_pdo.h>
#include <pdo/php_pdo_driver.h>
#include <pdo/php_pdo_driver.h>
#include <zend_exceptions.h>

#if PHP_MAJOR_VERSION >= 7

	#define PHP_STREAM_COPY_TO_MEM(a,b,c,d) php_stream_copy_to_mem(a,b,c)

	#define MY_ZVAL_STRING(a,b,c) ZVAL_STRING(a,b)

	#define RET_STRING(a,b) \
		RETURN_STR(zend_string_init(a,charstring::length(a),0))

	#define ADD_ASSOC_STRING(a,b,c,d) \
		add_assoc_string(a,b,zend_string_init(c,d,0)->val)
#else

	#define PHP_STREAM_COPY_TO_MEM(a,b,c,d) php_stream_copy_to_mem(a,b,c,d)

	#define MY_ZVAL_STRING(a,b,c) ZVAL_STRING(a,b,c)

	#define RET_STRING RETURN_STRING

	#define ADD_ASSOC_STRING(a,b,c,d) \
		add_assoc_string(a,b,c,d)
#endif

#define sqlrelayError(s) \
	_sqlrelayError(s,NULL,__FILE__,__LINE__ TSRMLS_CC)
#define sqlrelayErrorStmt(s) \
	_sqlrelayError(s->dbh,s,__FILE__,__LINE__ TSRMLS_CC)

struct sqlrstatement {
	sqlrcursor			*sqlrcur;
	int64_t				currentrow;
	long				longfield;
	stringbuffer			subvarquery;
	singlylinkedlist< char * >	subvarstrings;
	bool				fwdonly;
};

struct sqlrdbhandle {
	sqlrconnection	*sqlrcon;
	bool		translatebindsonserver;
	bool		usesubvars;
	int64_t		resultsetbuffersize;
	bool		dontgetcolumninfo;
	bool		nullsasnulls;
};

enum {
	PDO_SQLRELAY_ATTR_RESULT_SET_BUFFER_SIZE=PDO_ATTR_DRIVER_SPECIFIC,
	PDO_SQLRELAY_ATTR_DONT_GET_COLUMN_INFO,
	PDO_SQLRELAY_ATTR_GET_NULLS_AS_EMPTY_STRINGS,
	PDO_SQLRELAY_ATTR_DB_TYPE,
	PDO_SQLRELAY_ATTR_DB_VERSION,
	PDO_SQLRELAY_ATTR_DB_HOST_NAME,
	PDO_SQLRELAY_ATTR_DB_IP_ADDRESS,
	PDO_SQLRELAY_ATTR_BIND_FORMAT,
	PDO_SQLRELAY_ATTR_CURRENT_DB
};

int _sqlrelayError(pdo_dbh_t *dbh,
			pdo_stmt_t *stmt,
			const char *file,
			int line TSRMLS_DC) {

	int64_t		errornumber=0;
	const char	*errormessage=NULL;
	pdo_error_type	*pdoerr=NULL;

	if (stmt) {
		sqlrstatement	*sqlrstmt=(sqlrstatement *)stmt->driver_data;
		errornumber=sqlrstmt->sqlrcur->errorNumber();
		errormessage=sqlrstmt->sqlrcur->errorMessage();
		pdoerr=&stmt->error_code;
	} else {
		sqlrdbhandle	*sqlrdbh=(sqlrdbhandle *)dbh->driver_data;
		errornumber=sqlrdbh->sqlrcon->errorNumber();
		errormessage=sqlrdbh->sqlrcon->errorMessage();
		pdoerr=&dbh->error_code;
	}

	// FIXME: currently we're leaving this at HY000 but it really ought to
	// be set to some value.  DB2 and ODBC support this, others might too.
	charstring::copy(*pdoerr,"HY000",5);

	if (!dbh->methods) {
		TSRMLS_FETCH();
		zend_throw_exception_ex(php_pdo_get_exception(),
					errornumber TSRMLS_CC,
					"SQLSTATE[%s] [%d] %s",
					*pdoerr,errornumber,errormessage);
	}
	return errornumber;
}

void _bindFormatError() {
	TSRMLS_FETCH();
	int64_t		errornumber=
			SQLR_ERROR_INVALIDBINDVARIABLEFORMAT;
	const char	*errormessage=
			SQLR_ERROR_INVALIDBINDVARIABLEFORMAT_STRING;
	zend_throw_exception_ex(php_pdo_get_exception(),
					errornumber TSRMLS_CC,
					"SQLSTATE[HY000] [%d] %s",
					errornumber,errormessage);
}

static void clearList(singlylinkedlist< char * > *list) {
	for (singlylinkedlistnode< char * > *node=list->getFirst();
					node; node=node->getNext()) {
		delete[] node->getValue();
	}
	list->clear();
}

static int sqlrcursorDestructor(pdo_stmt_t *stmt TSRMLS_DC) {
	sqlrstatement	*sqlrstmt=(sqlrstatement *)stmt->driver_data;
	delete sqlrstmt->sqlrcur;
	clearList(&sqlrstmt->subvarstrings);
	delete sqlrstmt;
	return 1;
}

static int sqlrcursorExecute(pdo_stmt_t *stmt TSRMLS_DC) {
	sqlrstatement	*sqlrstmt=(sqlrstatement *)stmt->driver_data;
	sqlrcursor	*sqlrcur=sqlrstmt->sqlrcur;
	if (((sqlrdbhandle *)stmt->dbh->driver_data)->usesubvars) {
		if (!sqlrcur->executeQuery()) {
			sqlrelayErrorStmt(stmt);
			return 0;
		}
		clearList(&sqlrstmt->subvarstrings);
		// If we're using substitution variables, then we need
		// to re-prepare.  Arguably this is a bug in the
		// SQL Relay client API.
		sqlrcur->prepareQuery(sqlrstmt->subvarquery.getString(),
				sqlrstmt->subvarquery.getStringLength());
	} else {
		if (!sqlrcur->executeQuery()) {
			sqlrelayErrorStmt(stmt);
			return 0;
		}
	}
	sqlrstmt->currentrow=-1;
	stmt->column_count=sqlrcur->colCount();
	stmt->row_count=sqlrcur->affectedRows();
	return 1;
}

static int sqlrcursorFetch(pdo_stmt_t *stmt,
				enum pdo_fetch_orientation ori,
				long offset TSRMLS_DC) {

	sqlrstatement	*sqlrstmt=(sqlrstatement *)stmt->driver_data;
	sqlrcursor	*sqlrcur=sqlrstmt->sqlrcur;
	uint64_t	rsbs=sqlrcur->getResultSetBufferSize();

	switch (ori) {
		case PDO_FETCH_ORI_NEXT:
			sqlrstmt->currentrow++;
			break;
		case PDO_FETCH_ORI_PRIOR:
			if (sqlrstmt->fwdonly) {
				return 0;
			}
			sqlrstmt->currentrow--;
			break;
		case PDO_FETCH_ORI_FIRST:
			if (sqlrstmt->fwdonly &&
				sqlrstmt->currentrow!=-1) {
				return 0;
			}
			sqlrstmt->currentrow=0;
			break;
		case PDO_FETCH_ORI_LAST:
			// If we're using a result set buffer size, then get
			// fields until we've fetched the end of result set so
			// rowCount() will be accurate when called below.
			if (rsbs) {
				uint64_t	count=sqlrcur->rowCount()/rsbs;
				while (!sqlrcur->endOfResultSet()) {
					sqlrcur->getField(count*rsbs+1,
								(uint32_t)0);
					count++;
				}
			}
			sqlrstmt->currentrow=sqlrcur->rowCount()-1;
			break;
		case PDO_FETCH_ORI_ABS:
			if (sqlrstmt->fwdonly &&
				offset<=sqlrstmt->currentrow) {
				return 0;
			}
			sqlrstmt->currentrow=offset;
			break;
		case PDO_FETCH_ORI_REL:
			if (sqlrstmt->fwdonly && offset<1) {
				return 0;
			}
			sqlrstmt->currentrow+=offset;
			break;
	}

	if (sqlrstmt->currentrow<-1) {
		sqlrstmt->currentrow=-1;
	}

	// If we're using a result set buffer size then get the field here
	// so the row will actually be fetched and rowCount() will be accurate
	// when called below.
	if (rsbs && sqlrstmt->currentrow>-1) {
		sqlrcur->getField(sqlrstmt->currentrow,(uint32_t)0);
	}

	return (sqlrstmt->currentrow>-1 &&
		(uint64_t)sqlrstmt->currentrow>=sqlrcur->firstRowIndex() &&
		(uint64_t)sqlrstmt->currentrow<sqlrcur->rowCount());
}

static int sqlrcursorDescribe(pdo_stmt_t *stmt, int colno TSRMLS_DC) {

	sqlrstatement	*sqlrstmt=(sqlrstatement *)stmt->driver_data;
	sqlrcursor	*sqlrcur=sqlrstmt->sqlrcur;
	const char	*n=sqlrcur->getColumnName(colno);
	char		*name=estrdup((n)?n:"");
	const char	*type=sqlrcur->getColumnType(colno);
	stmt->columns[colno].name=name;
	stmt->columns[colno].namelen=charstring::length(name);
	stmt->columns[colno].maxlen=sqlrcur->getColumnLength(colno);
	if (isBitTypeChar(type) || isNumberTypeChar(type)) {
		if (isFloatTypeChar(type)) {
			stmt->columns[colno].param_type=PDO_PARAM_STR;
		} else {
			stmt->columns[colno].param_type=PDO_PARAM_INT;
		}
	} else if (isBlobTypeChar(type)) {
		stmt->columns[colno].param_type=PDO_PARAM_LOB;
	} else if (isBoolTypeChar(type)) {
		stmt->columns[colno].param_type=PDO_PARAM_BOOL;
	} else {
		stmt->columns[colno].param_type=PDO_PARAM_STR;
	}
	stmt->columns[colno].precision=sqlrcur->getColumnPrecision(colno);
	return 1;
}

static int sqlrcursorGetField(pdo_stmt_t *stmt,
				int colno,
				char **ptr,
				unsigned long *len,
				int *caller_frees TSRMLS_DC) {

	sqlrstatement	*sqlrstmt=(sqlrstatement *)stmt->driver_data;
	sqlrcursor	*sqlrcur=sqlrstmt->sqlrcur;

	*caller_frees=0;

	switch (stmt->columns[colno].param_type) {
		case PDO_PARAM_INT:
		case PDO_PARAM_BOOL:
			// handle NULLs/empty-strings
			if (!sqlrcur->getFieldLength(
					sqlrstmt->currentrow,colno)) {
				*ptr=(char *)sqlrcur->getField(
					sqlrstmt->currentrow,colno);
				*len=0;
				return 1;
			}
			sqlrstmt->longfield=(long)sqlrcur->
				getFieldAsInteger(sqlrstmt->currentrow,colno);
			*ptr=(char *)&sqlrstmt->longfield;
			*len=sizeof(long);
			return 1;
		case PDO_PARAM_STR:
			*ptr=(char *)sqlrcur->
				getField(sqlrstmt->currentrow,colno);
			*len=sqlrcur->
				getFieldLength(sqlrstmt->currentrow,colno);
			return 1;
		case PDO_PARAM_LOB:
			// lobs can be usually be returned as strings...
			*ptr=(char *)sqlrcur->
				getField(sqlrstmt->currentrow,colno);
			*len=sqlrcur->
				getFieldLength(sqlrstmt->currentrow,colno);
			// NULLs can be returned as NULLs
			if (!*ptr) {
				return 1;
			}
			// ...but empty strings must be returned
			// as empty streams
			if (!*len) {
				*ptr=(char *)php_stream_memory_create(
							TEMP_STREAM_DEFAULT);
			}
			return 1;
		default:
			return 1;
	}
	return 1;
}

static int sqlrcursorSubstitutionPreExec(sqlrstatement *sqlrstmt,
					const char *name,
					struct pdo_bound_param_data *param) {

	sqlrcursor	*sqlrcur=sqlrstmt->sqlrcur;

	if (param->param_type&PDO_PARAM_INPUT_OUTPUT) {
		return 0;
	}

	char	*nm=charstring::duplicate(name);
	sqlrstmt->subvarstrings.append(nm);

	char	*str=NULL;

	switch (PDO_PARAM_TYPE(param->param_type)) {
		case PDO_PARAM_NULL:
			sqlrcur->substitution(nm,(const char *)NULL);
			return 1;
		case PDO_PARAM_INT:
		case PDO_PARAM_BOOL:
			convert_to_long(param->parameter);
			sqlrcur->substitution(nm,Z_LVAL_P(param->parameter));
			return 1;
		case PDO_PARAM_STR:
			convert_to_string(param->parameter);
			str=new char[Z_STRLEN_P(param->parameter)+3];
			charstring::copy(str,"'");
			charstring::append(str,
					Z_STRVAL_P(param->parameter),
					Z_STRLEN_P(param->parameter));
			str[Z_STRLEN_P(param->parameter)+1]='\0';
			charstring::append(str,"'");
			sqlrstmt->subvarstrings.append(str);
			sqlrcur->substitution(nm,str);
			return 1;
		case PDO_PARAM_LOB:
			if (Z_TYPE_P(param->parameter)==IS_STRING) {
				convert_to_string(param->parameter);
				str=new char[Z_STRLEN_P(param->parameter)+3];
				charstring::copy(str,"'");
				charstring::append(str,
						Z_STRVAL_P(param->parameter),
						Z_STRLEN_P(param->parameter));
				str[Z_STRLEN_P(param->parameter)+1]='\0';
				charstring::append(str,"'");
				sqlrstmt->subvarstrings.append(str);
				sqlrcur->substitution(nm,str);
				return 1;
			} else {
				return 0;
			}
		case PDO_PARAM_STMT:
			return 0;
	}
	return 0;
}

static int sqlrcursorInputBindPreExec(sqlrcursor *sqlrcur,
					const char *name,
					struct pdo_bound_param_data *param) {

	switch (PDO_PARAM_TYPE(param->param_type)) {
		case PDO_PARAM_NULL:
			sqlrcur->inputBind(name,(const char *)NULL);
			return 1;
		case PDO_PARAM_INT:
		case PDO_PARAM_BOOL:
			// handle NULLs/empty-strings
			if (Z_TYPE_P(param->parameter)==IS_NULL) {
				sqlrcur->inputBind(name,(const char *)NULL);
				return 1;
			}
			convert_to_long(param->parameter);
			sqlrcur->inputBind(name,Z_LVAL_P(param->parameter));
			return 1;
		case PDO_PARAM_STR:
			convert_to_string(param->parameter);
			sqlrcur->inputBind(name,Z_STRVAL_P(param->parameter),
						Z_STRLEN_P(param->parameter));
			return 1;
		case PDO_PARAM_LOB:
			if (Z_TYPE_P(param->parameter)==IS_STRING) {
				convert_to_string(param->parameter);
				sqlrcur->inputBindBlob(name,
						Z_STRVAL_P(param->parameter),
						Z_STRLEN_P(param->parameter));
			} else if (Z_TYPE_P(param->parameter)==IS_RESOURCE) {
				TSRMLS_FETCH();
				php_stream	*strm=NULL;
				php_stream_from_zval_no_verify(
						strm,&param->parameter);
				if (!strm) {
					return 0;
				}
				SEPARATE_ZVAL(&param->parameter);
				Z_TYPE_P(param->parameter)=IS_STRING;
				Z_STRLEN_P(param->parameter)=
					PHP_STREAM_COPY_TO_MEM(strm,
						&Z_STRVAL_P(param->parameter),
						PHP_STREAM_COPY_ALL,0);
				sqlrcur->inputBindBlob(name,
						Z_STRVAL_P(param->parameter),
						Z_STRLEN_P(param->parameter));
			}
			return 1;
		case PDO_PARAM_STMT:
			return 0;
	}
	return 0;
}

static int sqlrcursorOutputBindPreExec(sqlrcursor *sqlrcur,
				const char *name,
				struct pdo_bound_param_data *param) {

	switch (PDO_PARAM_TYPE(param->param_type)) {
		case PDO_PARAM_NULL:
			return 1;
		case PDO_PARAM_INT:
		case PDO_PARAM_BOOL:
			sqlrcur->defineOutputBindInteger(name);
			return 1;
		case PDO_PARAM_STR:
			sqlrcur->defineOutputBindString(name,
							param->max_value_len);
			return 1;
		case PDO_PARAM_LOB:
			sqlrcur->defineOutputBindBlob(name);
			return 1;
		case PDO_PARAM_STMT:
			//sqlrcur->defineOutputBindCursor(name);
			return 0;
	}
	return 0;
}

static int sqlrcursorBindPreExec(sqlrcursor *sqlrcur,
				const char *name,
				struct pdo_bound_param_data *param) {

	if (param->param_type&PDO_PARAM_INPUT_OUTPUT) {
		return sqlrcursorOutputBindPreExec(sqlrcur,name,param);
	}
	return sqlrcursorInputBindPreExec(sqlrcur,name,param);
}

static int sqlrcursorBindPostExec(sqlrcursor *sqlrcur,
				const char *name,
				struct pdo_bound_param_data *param) {

	if (!(param->param_type&PDO_PARAM_INPUT_OUTPUT)) {
		return 1;
	}

	switch (PDO_PARAM_TYPE(param->param_type)) {
		case PDO_PARAM_NULL:
			ZVAL_NULL(param->parameter);
			return 1;
		case PDO_PARAM_INT:
			ZVAL_LONG(param->parameter,
					sqlrcur->getOutputBindInteger(name));
			return 1;
		case PDO_PARAM_BOOL:
			ZVAL_BOOL(param->parameter,
					sqlrcur->getOutputBindInteger(name));
			return 1;
		case PDO_PARAM_STR:
			MY_ZVAL_STRING(param->parameter,
				(char *)sqlrcur->getOutputBindString(name),1);
			return 1;
		case PDO_PARAM_LOB:
			{
			php_stream	*strm=NULL;
			if (Z_TYPE_P(param->parameter)==IS_STRING) {
				TSRMLS_FETCH();
				strm=php_stream_memory_create(
							TEMP_STREAM_DEFAULT);
			} else if (Z_TYPE_P(param->parameter)==IS_RESOURCE) {
				TSRMLS_FETCH();
				php_stream_from_zval_no_verify(
						strm,&param->parameter);
			}
			if (!strm) {
				return 0;
			}
			TSRMLS_FETCH();
			php_stream_write(strm,
				sqlrcur->getOutputBindBlob(name),
				sqlrcur->getOutputBindLength(name));
			php_stream_seek(strm,0,SEEK_SET);
			if (Z_TYPE_P(param->parameter)==IS_STRING) {
				php_stream_to_zval(strm,param->parameter);
			}
			}
			return 1;
		case PDO_PARAM_STMT:
			// FIXME: use pdo_stmt_construct here
			//sqlrcur->getOutputBindCursor(name);
			return 0;
	}
	return 0;
}

static int sqlrcursorBind(pdo_stmt_t *stmt,
				struct pdo_bound_param_data *param,
				enum pdo_param_event eventtype TSRMLS_DC) {

	sqlrstatement	*sqlrstmt=(sqlrstatement *)stmt->driver_data;
	sqlrcursor	*sqlrcur=sqlrstmt->sqlrcur;

	stringbuffer	paramname;
	paramname.append((uint64_t)param->paramno+1);
	const char	*name=(param->name)?param->name:paramname.getString();

	// Chop any :, @ or $'s off of the front of the name.  We have to
	// iterate because PDO itself prepends a : if the name doesn't already
	// have one.
	while (character::inSet(*name,":@$")) {
		name++;
	}

	// validate types
	bool	validtype=false;
	switch (PDO_PARAM_TYPE(param->param_type)) {
		case PDO_PARAM_NULL:
		case PDO_PARAM_BOOL:
		case PDO_PARAM_INT:
		case PDO_PARAM_STR:
		case PDO_PARAM_LOB:
		//case PDO_PARAM_STMT:
			validtype=true;
	}
	if (!validtype) {
		_bindFormatError();
		return 1;
	}

	// FIXME: what does this mean?
	if (!param->is_param) {
		return 1;
	}

	if (((sqlrdbhandle *)stmt->dbh->driver_data)->usesubvars) {
		if (eventtype==PDO_PARAM_EVT_EXEC_PRE) {
			return sqlrcursorSubstitutionPreExec(
						sqlrstmt,name,param);
		}
	} else {
		if (eventtype==PDO_PARAM_EVT_EXEC_PRE) {
			return sqlrcursorBindPreExec(sqlrcur,name,param);
		} else if (eventtype==PDO_PARAM_EVT_EXEC_POST) {
			return sqlrcursorBindPostExec(sqlrcur,name,param);
		}
	}
	return 1;
}

static int sqlrcursorSetAttribute(pdo_stmt_t *stmt,
					long attr, zval *val TSRMLS_DC) {

	sqlrstatement	*sqlrstmt=(sqlrstatement *)stmt->driver_data;
	sqlrcursor	*sqlrcur=sqlrstmt->sqlrcur;

	switch (attr) {
		case PDO_SQLRELAY_ATTR_RESULT_SET_BUFFER_SIZE:
			convert_to_long(val);
			sqlrcur->setResultSetBufferSize(Z_LVAL_P(val));
			return 1;
		case PDO_SQLRELAY_ATTR_DONT_GET_COLUMN_INFO:
			convert_to_boolean(val);
			if (Z_BVAL_P(val)==TRUE) {
				sqlrcur->dontGetColumnInfo();
			} else {
				sqlrcur->getColumnInfo();
			}
			return 1;
		case PDO_SQLRELAY_ATTR_GET_NULLS_AS_EMPTY_STRINGS:
			convert_to_boolean(val);
			if (Z_BVAL_P(val)==TRUE) {
				sqlrcur->getNullsAsEmptyStrings();
			} else {
				sqlrcur->getNullsAsNulls();
			}
			return 1;
		default:
			return 0;
	}
}

static int sqlrcursorGetAttribute(pdo_stmt_t *stmt,
					long attr, zval *retval TSRMLS_DC) {

	sqlrstatement	*sqlrstmt=(sqlrstatement *)stmt->driver_data;
	sqlrcursor	*sqlrcur=sqlrstmt->sqlrcur;

	switch (attr) {
		case PDO_SQLRELAY_ATTR_RESULT_SET_BUFFER_SIZE:
			ZVAL_LONG(retval,sqlrcur->getResultSetBufferSize());
			return 1;
		default:
			return 0;
	}
}

static int sqlrcursorColumnMetadata(pdo_stmt_t *stmt,
					long colno,
					zval *returnvalue TSRMLS_DC) {

	sqlrstatement	*sqlrstmt=(sqlrstatement *)stmt->driver_data;
	sqlrcursor	*sqlrcur=(sqlrcursor *)sqlrstmt->sqlrcur;

	array_init(returnvalue);

	// native type
	const char	*type=sqlrcur->getColumnType(colno);
	ADD_ASSOC_STRING(returnvalue,"native_type",(char *)((type)?type:""),1);


	// pdo type
	int32_t		pdotype=PDO_PARAM_STR;
	if (isBitTypeChar(type) || isNumberTypeChar(type)) {
		if (isFloatTypeChar(type)) {
			pdotype=PDO_PARAM_STR;
		} else {
			pdotype=PDO_PARAM_INT;
		}
	} else if (isBlobTypeChar(type)) {
		pdotype=PDO_PARAM_LOB;
	} else if (isBoolTypeChar(type)) {
		pdotype=PDO_PARAM_BOOL;
	}
	add_assoc_long(returnvalue,"pdo_type",pdotype);


	// flags
	zval	*flags=NULL;
	MAKE_STD_ZVAL(flags);
	array_init(flags);
	if (sqlrcur->getColumnIsNullable(colno)) {
		add_next_index_string(flags,"nullable",1);
	}
	if (sqlrcur->getColumnIsPrimaryKey(colno)) {
		add_next_index_string(flags,"primary_key",1);
	}
	if (sqlrcur->getColumnIsUnique(colno)) {
		add_next_index_string(flags,"unique",1);
	}
	if (sqlrcur->getColumnIsPartOfKey(colno)) {
		add_next_index_string(flags,"part_of_key",1);
	}
	if (sqlrcur->getColumnIsUnsigned(colno)) {
		add_next_index_string(flags,"unsigned",1);
	}
	if (sqlrcur->getColumnIsZeroFilled(colno)) {
		add_next_index_string(flags,"zero_filled",1);
	}
	if (sqlrcur->getColumnIsBinary(colno)) {
		add_next_index_string(flags,"binary",1);
	}
	if (sqlrcur->getColumnIsAutoIncrement(colno)) {
		add_next_index_string(flags,"auto_increment",1);
	}
	add_assoc_zval(returnvalue,"flags",flags);
	return 1;
}

static int sqlrcursorClose(pdo_stmt_t *stmt TSRMLS_DC) {
	sqlrstatement	*sqlrstmt=(sqlrstatement *)stmt->driver_data;
	sqlrcursor	*sqlrcur=(sqlrcursor *)sqlrstmt->sqlrcur;
	sqlrcur->closeResultSet();
	return 1;
}

static struct pdo_stmt_methods sqlrcursorMethods={
	sqlrcursorDestructor,
	sqlrcursorExecute,
	sqlrcursorFetch,
	sqlrcursorDescribe,
	sqlrcursorGetField,
	sqlrcursorBind,
	sqlrcursorSetAttribute,
	sqlrcursorGetAttribute,
	sqlrcursorColumnMetadata,
	NULL, // next rowset
	sqlrcursorClose
};


static int sqlrconnectionClose(pdo_dbh_t *dbh TSRMLS_DC) {
	sqlrdbhandle	*sqlrdbh=(sqlrdbhandle *)dbh->driver_data;
	delete (sqlrconnection *)sqlrdbh->sqlrcon;
	dbh->is_closed=1;
	return 0;
}

static void sqlrconnectionRewriteQuery(const char *query,
						uint32_t querylen,
						stringbuffer *newquery) {
	bool		inquotes=false;
	bool		inbind=false;
	uint16_t	varcounter=0;

	for (const char *c=query; *c; c++) {

		if (*c=='\'') {
			inquotes=!inquotes;
		}

		if (!inquotes) {

			if (inbind && (character::isWhitespace(*c) ||
						character::inSet(*c,",);:="))) {
				newquery->append(')');
				inbind=false;
			}

			// catch ?, :, $ and @ but not @@
			if (character::inSet(*c,"?:$") ||
					(*c=='@' && *(c+1)!='@')) {
				newquery->append("$(");
				if (*c=='?') {
					newquery->append(varcounter);
					varcounter++;
				} else {
					inbind=true;
				}
				continue;
			}
		}

		newquery->append(*c);
	}
}

static int sqlrconnectionPrepare(pdo_dbh_t *dbh, const char *sql,
					long sqllen, pdo_stmt_t *stmt,
					zval *driveroptions TSRMLS_DC) {

	sqlrdbhandle	*sqlrdbh=(sqlrdbhandle *)dbh->driver_data;
	sqlrstatement	*sqlrstmt=new sqlrstatement;
	sqlrstmt->sqlrcur=new sqlrcursor(
				(sqlrconnection *)sqlrdbh->sqlrcon,true);

	if (sqlrdbh->resultsetbuffersize>0) {
		sqlrstmt->sqlrcur->setResultSetBufferSize(
					sqlrdbh->resultsetbuffersize);
	}
	if (sqlrdbh->dontgetcolumninfo) {
		sqlrstmt->sqlrcur->dontGetColumnInfo();
	}
	if (sqlrdbh->nullsasnulls) {
		sqlrstmt->sqlrcur->getNullsAsNulls();
	}

	sqlrstmt->currentrow=-1;
	stmt->methods=&sqlrcursorMethods;
	stmt->driver_data=(void *)sqlrstmt;

	stmt->column_count=0;
	stmt->columns=NULL;
	stmt->row_count=0;

	sqlrstmt->subvarquery.clear();
	clearList(&sqlrstmt->subvarstrings);

	// FIXME:
	// To not have to set translatebindvariables on the server, we need to
	// figure out what db relay is connected to, set supports_placeholders
	// and named_rewrite_template appropriately and rewrite the
	// query using pdo_parse_params.
	//
	// Ideally we'd set a custom attribute for whether binds are translated
	// on the server or not.

	// SQL Relay actually supports named and postitional placeholders but
	// there doesn't appear to be a way to set both.  Positional is a larger
	// value in the enum, so I guess we'll use that.  The pdo code appears
	// to just check to see if it's not PDO_PLACEHOLDER_NONE anyway so
	// hopefully this is ok.
	stmt->supports_placeholders=PDO_PLACEHOLDER_POSITIONAL;

	if (sqlrdbh->usesubvars) {
		sqlrconnectionRewriteQuery(sql,sqllen,&sqlrstmt->subvarquery);
		sql=sqlrstmt->subvarquery.getString();
		sqllen=sqlrstmt->subvarquery.getStringLength();
	}

	sqlrstmt->fwdonly=pdo_attr_lval(driveroptions,
					PDO_ATTR_CURSOR,
					PDO_CURSOR_SCROLL TSRMLS_CC)==
					PDO_CURSOR_FWDONLY;
	
	if (!charstring::isNullOrEmpty(sql)) {
		sqlrstmt->sqlrcur->prepareQuery(sql,sqllen);
	}
	return 1;
}

static long sqlrconnectionExecute(pdo_dbh_t *dbh,
					const char *sql,
					long sqllen TSRMLS_DC) {
	sqlrdbhandle	*sqlrdbh=(sqlrdbhandle *)dbh->driver_data;
	sqlrcursor	sqlrcur((sqlrconnection *)sqlrdbh->sqlrcon);
	if (sqlrcur.sendQuery(sql,sqllen)) {
		return sqlrcur.affectedRows();
	}
	sqlrelayError(dbh);
	return -1;
}

static int sqlrconnectionBegin(pdo_dbh_t *dbh TSRMLS_DC) {
	sqlrdbhandle	*sqlrdbh=(sqlrdbhandle *)dbh->driver_data;
	if (((sqlrconnection *)sqlrdbh->sqlrcon)->begin()) {
		return 1;
	}
	sqlrelayError(dbh);
	return 0;
}

static int sqlrconnectionCommit(pdo_dbh_t *dbh TSRMLS_DC) {
	sqlrdbhandle	*sqlrdbh=(sqlrdbhandle *)dbh->driver_data;
	if (((sqlrconnection *)sqlrdbh->sqlrcon)->commit()) {
		return 1;
	}
	sqlrelayError(dbh);
	return 0;
}

static int sqlrconnectionRollback(pdo_dbh_t *dbh TSRMLS_DC) { 
	sqlrdbhandle	*sqlrdbh=(sqlrdbhandle *)dbh->driver_data;
	if (((sqlrconnection *)sqlrdbh->sqlrcon)->rollback()) {
		return 1;
	}
	sqlrelayError(dbh);
	return 0;
}

static int sqlrconnectionSetAttribute(pdo_dbh_t *dbh,
					long attr, zval *val TSRMLS_DC) {

	sqlrdbhandle	*sqlrdbh=(sqlrdbhandle *)dbh->driver_data;
	sqlrconnection	*sqlrcon=(sqlrconnection *)sqlrdbh->sqlrcon;

	// PDO handles several of these options itself.  These are the ones
	// it doens't handle.
	switch (attr) {
		case PDO_ATTR_AUTOCOMMIT:
			// use to turn on or off auto-commit mode
			convert_to_boolean(val);
			if (dbh->auto_commit^Z_BVAL_P(val)) {
				dbh->auto_commit=Z_BVAL_P(val);
				if (Z_BVAL_P(val)==TRUE) {
					sqlrcon->autoCommitOn();
				} else {
					sqlrcon->autoCommitOff();
				}
			}
			return 1;
		case PDO_ATTR_PREFETCH:
			// configure the prefetch size for drivers
			// that support it. Size is in KB
			return 1;
		case PDO_ATTR_TIMEOUT:
			// connection timeout in seconds
			convert_to_long(val);
			sqlrcon->setConnectTimeout(Z_LVAL_P(val),0);
			sqlrcon->setAuthenticationTimeout(Z_LVAL_P(val),0);
			sqlrcon->setResponseTimeout(Z_LVAL_P(val),0);
			return 1;
		case PDO_ATTR_SERVER_VERSION:
			// database server version
			return 1;
		case PDO_ATTR_CLIENT_VERSION:
			// client library version
			return 1;
		case PDO_ATTR_SERVER_INFO:
			// server information
			return 1;
		case PDO_ATTR_CONNECTION_STATUS:
			// connection status
			return 1;
		case PDO_ATTR_CURSOR:
			// cursor type
			return 1;
		case PDO_ATTR_PERSISTENT:
			// pconnect style connection
			return 1;
		case PDO_ATTR_FETCH_TABLE_NAMES:
			// include table names in the column names,
			// where available
			return 1;
		case PDO_ATTR_FETCH_CATALOG_NAMES:
			// include the catalog/db name names in
			// the column names, where available
			return 1;
		case PDO_ATTR_DRIVER_NAME:
			// name of the driver (as used in the constructor)
			return 1;
		case PDO_ATTR_MAX_COLUMN_LEN:
			// make database calculate maximum
			// length of data found in a column
			return 1;
		#ifdef HAVE_PHP_PDO_ATTR_EMULATE_PREPARES
		case PDO_ATTR_EMULATE_PREPARES:
			// use substititution variables rather than binds
			convert_to_boolean(val);
			sqlrdbh->usesubvars=Z_BVAL_P(val);
			return 1;
		#endif
		case PDO_SQLRELAY_ATTR_CURRENT_DB:
			convert_to_string(val);
			if (sqlrcon->selectDatabase(Z_STRVAL_P(val))) {
				return 1;
			} else {
				return 0;
			}
		default:
			return 0;
	}
}

static char *sqlrconnectionLastInsertId(pdo_dbh_t *dbh,
				const char *name, unsigned int *len TSRMLS_DC) {
	sqlrdbhandle	*sqlrdbh=(sqlrdbhandle *)dbh->driver_data;
	char	*id=php_pdo_int64_to_str(
				((sqlrconnection *)sqlrdbh->sqlrcon)->
							getLastInsertId()
				TSRMLS_CC);
	*len=charstring::length(id);
	return id;
}

static int sqlrconnectionError(pdo_dbh_t *dbh,
					pdo_stmt_t *stmt,
					zval *info TSRMLS_DC) {

	// FIXME: the first index in the info array should be the sqlstate
	// currently we're leaving it at HY000 but it really ought to be
	// set to some value.  DB2 and ODBC support this, others might too.
	if (stmt) {
		sqlrstatement	*sqlrstmt=(sqlrstatement *)stmt->driver_data;
		sqlrcursor	*sqlrcur=sqlrstmt->sqlrcur;
		add_next_index_long(info,sqlrcur->errorNumber());
		// NOTE: This is un-const'ed because add_next_index_string
		// takes a char * rather than const char * and this works
		// with both.
		char	*msg=(char *)sqlrcur->errorMessage();
		if (msg) {
			add_next_index_string(info,msg,1);
		}
	} else if (dbh) {
		sqlrdbhandle	*sqlrdbh=(sqlrdbhandle *)dbh->driver_data;
		sqlrconnection	*sqlrcon=(sqlrconnection *)sqlrdbh->sqlrcon;
		add_next_index_long(info,sqlrcon->errorNumber());
		// NOTE: This is un-const'ed because add_next_index_string
		// takes a char * rather than const char * and this works
		// with both.
		char	*msg=(char *)sqlrcon->errorMessage();
		if (msg) {
			add_next_index_string(info,msg,1);
		}
	}
	return 1;
}

static int sqlrconnectionGetAttribute(pdo_dbh_t *dbh,
				long attr, zval *retval TSRMLS_DC) {

	sqlrdbhandle	*sqlrdbh=(sqlrdbhandle *)dbh->driver_data;
	sqlrconnection	*sqlrcon=(sqlrconnection *)sqlrdbh->sqlrcon;

	// PDO handles several of these options itself.  These are the ones
	// it doens't handle.
	char	*temp;
	switch (attr) {
		case PDO_ATTR_AUTOCOMMIT:
			// use to turn on or off auto-commit mode
			ZVAL_BOOL(retval,dbh->auto_commit);
			return 1;
		case PDO_ATTR_PREFETCH:
			// configure the prefetch size for drivers
			// that support it. Size is in KB
			return 1;
		case PDO_ATTR_TIMEOUT:
			// connection timeout in seconds
			return 1;
		case PDO_ATTR_SERVER_VERSION:
			// database server version
			temp=(char *)sqlrcon->serverVersion();
			if (temp) {
				MY_ZVAL_STRING(retval,temp,1);
			}
			return 1;
		case PDO_ATTR_CLIENT_VERSION:
			// client library version
			temp=(char *)sqlrcon->clientVersion();
			if (temp) {
				MY_ZVAL_STRING(retval,temp,1);
			}
			return 1;
		case PDO_ATTR_SERVER_INFO:
			// server information
			return 1;
		case PDO_ATTR_CONNECTION_STATUS:
			// connection status
			return 1;
		case PDO_ATTR_CURSOR:
			// cursor type
			return 1;
		case PDO_ATTR_FETCH_TABLE_NAMES:
			// include table names in the column names,
			// where available
			return 1;
		case PDO_ATTR_FETCH_CATALOG_NAMES:
			// include the catalog/db name names in
			// the column names, where available
			return 1;
		case PDO_ATTR_MAX_COLUMN_LEN:
			// make database calculate maximum
			// length of data found in a column
			return 1;
		#ifdef HAVE_PHP_PDO_ATTR_EMULATE_PREPARES
		case PDO_ATTR_EMULATE_PREPARES:
			// use substititution variables rather than binds
			ZVAL_BOOL(retval,sqlrdbh->usesubvars);
			return 1;
		#endif
		case PDO_SQLRELAY_ATTR_DB_TYPE:
			temp=(char *)sqlrcon->identify();
			if (temp) {
				MY_ZVAL_STRING(retval,temp,1);
			}
			return 1;
		case PDO_SQLRELAY_ATTR_DB_VERSION:
			temp=(char *)sqlrcon->dbVersion();
			if (temp) {
				MY_ZVAL_STRING(retval,temp,1);
			}
			return 1;
		case PDO_SQLRELAY_ATTR_DB_HOST_NAME:
			temp=(char *)sqlrcon->dbHostName();
			if (temp) {
				MY_ZVAL_STRING(retval,temp,1);
			}
			return 1;
		case PDO_SQLRELAY_ATTR_DB_IP_ADDRESS:
			temp=(char *)sqlrcon->dbIpAddress();
			if (temp) {
				MY_ZVAL_STRING(retval,temp,1);
			}
			return 1;
		case PDO_SQLRELAY_ATTR_BIND_FORMAT:
			temp=(char *)sqlrcon->bindFormat();
			if (temp) {
				MY_ZVAL_STRING(retval,temp,1);
			}
			return 1;
		case PDO_SQLRELAY_ATTR_CURRENT_DB:
			temp=(char *)sqlrcon->getCurrentDatabase();
			if (temp) {
				MY_ZVAL_STRING(retval,temp,1);
			}
			return 1;
		default:
			return 0;
	}
}

static PHP_METHOD(PDO_SQLRELAY, getConnectionPort) {
	pdo_dbh_t	*dbh=
		(pdo_dbh_t *)zend_object_store_get_object(getThis() TSRMLS_CC);
	sqlrdbhandle	*sqlrdbh=(sqlrdbhandle *)dbh->driver_data;
	sqlrconnection	*sqlrcon=(sqlrconnection *)sqlrdbh->sqlrcon;
	uint16_t	port=sqlrcon->getConnectionPort();
	RETURN_LONG(port);
}

static PHP_METHOD(PDO_SQLRELAY, getConnectionSocket) {
	pdo_dbh_t	*dbh=
		(pdo_dbh_t *)zend_object_store_get_object(getThis() TSRMLS_CC);
	sqlrdbhandle	*sqlrdbh=(sqlrdbhandle *)dbh->driver_data;
	sqlrconnection	*sqlrcon=(sqlrconnection *)sqlrdbh->sqlrcon;
	const char	*socket=sqlrcon->getConnectionSocket();
	RET_STRING((char *)socket,1);
}

static PHP_METHOD(PDO_SQLRELAY, suspendSession) {
	pdo_dbh_t	*dbh=
		(pdo_dbh_t *)zend_object_store_get_object(getThis() TSRMLS_CC);
	sqlrdbhandle	*sqlrdbh=(sqlrdbhandle *)dbh->driver_data;
	sqlrconnection	*sqlrcon=(sqlrconnection *)sqlrdbh->sqlrcon;
	if (sqlrcon->suspendSession()) {
		RETURN_TRUE;
	}
	sqlrelayError(dbh);
	RETURN_FALSE;
}

static PHP_METHOD(PDO_SQLRELAY, resumeSession) {

	zval	**port;
	zval	**socket;
	if (ZEND_NUM_ARGS()!=2 ||
		zend_get_parameters_ex(2,&port,&socket)==FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(port);
	convert_to_string_ex(socket);

	pdo_dbh_t	*dbh=
		(pdo_dbh_t *)zend_object_store_get_object(getThis() TSRMLS_CC);
	sqlrdbhandle	*sqlrdbh=(sqlrdbhandle *)dbh->driver_data;
	sqlrconnection	*sqlrcon=(sqlrconnection *)sqlrdbh->sqlrcon;
	if (sqlrcon->resumeSession((*port)->value.lval,
					(*socket)->value.str.val)) {
		RETURN_TRUE;
	}
	sqlrelayError(dbh);
	RETURN_FALSE;
}

static PHP_METHOD(PDO_SQLRELAY, endSession) {
	pdo_dbh_t	*dbh=
		(pdo_dbh_t *)zend_object_store_get_object(getThis() TSRMLS_CC);
	sqlrdbhandle	*sqlrdbh=(sqlrdbhandle *)dbh->driver_data;
	sqlrconnection	*sqlrcon=(sqlrconnection *)sqlrdbh->sqlrcon;
	sqlrcon->endSession();
}

// NOTE: don't make this const or it will fail to compile with older PHP
static zend_function_entry sqlrelayConnectionFunctions[] = {
	PHP_ME(PDO_SQLRELAY,getConnectionPort,NULL,ZEND_ACC_PUBLIC)
	PHP_ME(PDO_SQLRELAY,getConnectionSocket,NULL,ZEND_ACC_PUBLIC)
	PHP_ME(PDO_SQLRELAY,suspendSession,NULL,ZEND_ACC_PUBLIC)
	PHP_ME(PDO_SQLRELAY,resumeSession,NULL,ZEND_ACC_PUBLIC)
	PHP_ME(PDO_SQLRELAY,endSession,NULL,ZEND_ACC_PUBLIC)
#ifdef PHP_FE_END
	PHP_FE_END
#else
	{NULL,NULL,NULL}
#endif
};

static PHP_METHOD(PDO_SQLRELAY, getResultSetId) {
	pdo_stmt_t	*stmt=
		(pdo_stmt_t *)zend_object_store_get_object(getThis() TSRMLS_CC);
	sqlrstatement	*sqlrstmt=(sqlrstatement *)stmt->driver_data;
	sqlrcursor	*sqlrcur=sqlrstmt->sqlrcur;
	uint16_t	id=sqlrcur->getResultSetId();
	RETURN_LONG(id);
}

static PHP_METHOD(PDO_SQLRELAY, suspendResultSet) {
	pdo_stmt_t	*stmt=
		(pdo_stmt_t *)zend_object_store_get_object(getThis() TSRMLS_CC);
	sqlrstatement	*sqlrstmt=(sqlrstatement *)stmt->driver_data;
	sqlrcursor	*sqlrcur=sqlrstmt->sqlrcur;
	sqlrcur->suspendResultSet();
	RETURN_TRUE;
}

static PHP_METHOD(PDO_SQLRELAY, resumeResultSet) {

	zval	**id;
	if (ZEND_NUM_ARGS()!=1 || zend_get_parameters_ex(1,&id)==FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(id);

	pdo_stmt_t	*stmt=
		(pdo_stmt_t *)zend_object_store_get_object(getThis() TSRMLS_CC);
	sqlrstatement	*sqlrstmt=(sqlrstatement *)stmt->driver_data;
	sqlrcursor	*sqlrcur=sqlrstmt->sqlrcur;
	if (sqlrcur->resumeResultSet((*id)->value.lval)) {
		stmt->executed=true;
		stmt->column_count=sqlrcur->colCount();
		stmt->columns=(pdo_column_data *)
				ecalloc(stmt->column_count,
					sizeof(struct pdo_column_data));
		for (int32_t i=0; i<stmt->column_count; i++) {
			if (!sqlrcursorDescribe(stmt,i TSRMLS_CC)) {
				sqlrelayErrorStmt(stmt);
				RETURN_FALSE;
			}
		}
		stmt->row_count=sqlrcur->affectedRows();
		sqlrstmt->currentrow=sqlrcur->firstRowIndex()-1;
		RETURN_TRUE;
	}
	sqlrelayErrorStmt(stmt);
	RETURN_FALSE;
}

// NOTE: don't make this const or it will fail to compile with older PHP
static zend_function_entry sqlrelayCursorFunctions[] = {
	PHP_ME(PDO_SQLRELAY,getResultSetId,NULL,ZEND_ACC_PUBLIC)
	PHP_ME(PDO_SQLRELAY,suspendResultSet,NULL,ZEND_ACC_PUBLIC)
	PHP_ME(PDO_SQLRELAY,resumeResultSet,NULL,ZEND_ACC_PUBLIC)
#ifdef PHP_FE_END
	PHP_FE_END
#else
	{NULL,NULL,NULL}
#endif
};

static
#ifdef HAVE_PHP_PDO_CONST_ZEND_FUNCTION_ENTRY
const
#endif
zend_function_entry *sqlrelayGetDriverMethods(pdo_dbh_t *dbh,
							int kind TSRMLS_DC) {
	switch (kind) {
		case PDO_DBH_DRIVER_METHOD_KIND_DBH:
			return sqlrelayConnectionFunctions;
		case PDO_DBH_DRIVER_METHOD_KIND_STMT:
			return sqlrelayCursorFunctions;
		default:
			return NULL;
	}
}

static struct pdo_dbh_methods sqlrconnectionMethods={
	sqlrconnectionClose,
	sqlrconnectionPrepare,
	sqlrconnectionExecute,
	NULL, // quote
	sqlrconnectionBegin,
	sqlrconnectionCommit,
	sqlrconnectionRollback,
	sqlrconnectionSetAttribute,
	sqlrconnectionLastInsertId,
	sqlrconnectionError,
	sqlrconnectionGetAttribute,
	NULL, // check liveness
	(pdo_dbh_get_driver_methods_func)sqlrelayGetDriverMethods
};

static int sqlrelayHandleFactory(pdo_dbh_t *dbh,
					zval *driveroptions TSRMLS_DC) {

	// parse the connect string
	pdo_data_src_parser	options[]={
		{"host",(char *)"",0},
		{"port",(char *)"",0},
		{"socket",(char *)"",0},
		{"tries",(char *)"0",0},
		{"retrytime",(char *)"1",0},
		{"debug",(char *)"0",0},
		{"resultsetbuffersize",(char *)"0",0},
		{"dontgetcolumninfo",(char *)"0",0},
		{"nullsasnulls",(char *)"0",0},
		{"lazyconnect",(char *)"1",0},
		{"krb",(char *)"no",0},
		{"krbservice",(char *)"",0},
		{"krbmech",(char *)"",0},
		{"krbflags",(char *)"",0},
		{"tls",(char *)"no",0},
		{"tlsversion",(char *)"",0},
		{"tlscert",(char *)"",0},
		{"tlspassword",(char *)"",0},
		{"tlsciphers",(char *)"",0},
		{"tlsvalidate",(char *)"",0},
		{"tlsca",(char *)"",0},
		{"tlsdepth",(char *)"0",0},
	};
	php_pdo_parse_data_source(dbh->data_source,
					dbh->data_source_len,
					options,
					sizeof(options)/sizeof(options[0]));
	const char	*host=options[0].optval;
	uint16_t	port=charstring::toInteger(options[1].optval);
	const char	*socket=options[2].optval;
	int32_t		tries=charstring::toInteger(options[3].optval);
	int32_t		retrytime=charstring::toInteger(options[4].optval);
	const char	*debug=options[5].optval;
	bool		lazyconnect=(charstring::toInteger(
						options[6].optval)>0);
	const char	*krb=options[10].optval;
	const char	*krbservice=options[11].optval;
	const char	*krbmech=options[12].optval;
	const char	*krbflags=options[13].optval;
	const char	*tls=options[14].optval;
	const char	*tlsversion=options[15].optval;
	const char	*tlscert=options[16].optval;
	const char	*tlspassword=options[17].optval;
	const char	*tlsciphers=options[18].optval;
	const char	*tlsvalidate=options[19].optval;
	const char	*tlsca=options[20].optval;
	uint16_t	tlsdepth=charstring::toInteger(options[21].optval);

	// create a sqlrconnection and attach it to the dbh
	sqlrdbhandle	*sqlrdbh=new sqlrdbhandle;
	sqlrdbh->sqlrcon=new sqlrconnection(host,port,socket,
							dbh->username,
							dbh->password,
							tries,retrytime,
							true);

	// enable kerberos or tls
	if (!charstring::compare(krb,"yes")) {
		sqlrdbh->sqlrcon->enableKerberos(krbservice,krbmech,krbflags);
	} else if (!charstring::compare(tls,"yes")) {
		sqlrdbh->sqlrcon->enableTls(tlsversion,
						tlscert,tlspassword,
						tlsciphers,tlsvalidate,
						tlsca,tlsdepth);
	}

	// enable debug
	if (!charstring::compare(debug,"1")) {
		sqlrdbh->sqlrcon->debugOn();
		sqlrdbh->sqlrcon->debugPrintFunction(zend_printf);
	} else if (!charstring::isNullOrEmpty(debug) &&
				charstring::compare(debug,"0")) {
		sqlrdbh->sqlrcon->setDebugFile(debug);
		sqlrdbh->sqlrcon->debugOn();
	}

	// if we're not doing lazy connects, then do something lightweight
	// that will verify whether SQL Relay is available or not
	if (!lazyconnect && !sqlrdbh->sqlrcon->identify()) {
		delete sqlrdbh->sqlrcon;
		sqlrdbh->sqlrcon=NULL;
		return 0;
	}

	sqlrdbh->resultsetbuffersize=charstring::toInteger(options[6].optval);
	sqlrdbh->dontgetcolumninfo=charstring::toInteger(options[7].optval);
	sqlrdbh->nullsasnulls=charstring::toInteger(options[8].optval);

	sqlrdbh->translatebindsonserver=false;
	sqlrdbh->usesubvars=false;

	dbh->driver_data=(void *)sqlrdbh;
	dbh->methods=&sqlrconnectionMethods;

	dbh->is_persistent=0;
	dbh->auto_commit=0;
	dbh->is_closed=0;
	dbh->alloc_own_columns=1;
	dbh->max_escaped_char_length=2;

	// success
	return 1;
}

static pdo_driver_t sqlrelayDriver={
	PDO_DRIVER_HEADER(sqlrelay),
	sqlrelayHandleFactory
};

static PHP_MINIT_FUNCTION(pdo_sqlrelay) {

	REGISTER_PDO_CLASS_CONST_LONG("SQLRELAY_ATTR_RESULT_SET_BUFFER_SIZE",
				(long)PDO_SQLRELAY_ATTR_RESULT_SET_BUFFER_SIZE);
	REGISTER_PDO_CLASS_CONST_LONG("SQLRELAY_ATTR_DONT_GET_COLUMN_INFO",
				(long)PDO_SQLRELAY_ATTR_DONT_GET_COLUMN_INFO);
	REGISTER_PDO_CLASS_CONST_LONG(
			"SQLRELAY_ATTR_GET_NULLS_AS_EMPTY_STRINGS",
			(long)PDO_SQLRELAY_ATTR_GET_NULLS_AS_EMPTY_STRINGS);
	REGISTER_PDO_CLASS_CONST_LONG("SQLRELAY_ATTR_DB_TYPE",
				(long)PDO_SQLRELAY_ATTR_DB_TYPE);
	REGISTER_PDO_CLASS_CONST_LONG("SQLRELAY_ATTR_DB_VERSION",
				(long)PDO_SQLRELAY_ATTR_DB_VERSION);
	REGISTER_PDO_CLASS_CONST_LONG("SQLRELAY_ATTR_DB_HOST_NAME",
				(long)PDO_SQLRELAY_ATTR_DB_HOST_NAME);
	REGISTER_PDO_CLASS_CONST_LONG("SQLRELAY_ATTR_DB_IP_ADDRESS",
				(long)PDO_SQLRELAY_ATTR_DB_IP_ADDRESS);
	REGISTER_PDO_CLASS_CONST_LONG("SQLRELAY_ATTR_BIND_FORMAT",
				(long)PDO_SQLRELAY_ATTR_BIND_FORMAT);
	REGISTER_PDO_CLASS_CONST_LONG("SQLRELAY_ATTR_CURRENT_DB",
				(long)PDO_SQLRELAY_ATTR_CURRENT_DB);

	return php_pdo_register_driver(&sqlrelayDriver);
}

static PHP_MSHUTDOWN_FUNCTION(pdo_sqlrelay) {
	php_pdo_unregister_driver(&sqlrelayDriver);
	return SUCCESS;
}

static PHP_MINFO_FUNCTION(pdo_sqlrelay) {

	stringbuffer	title;
	title.append("PDO Driver for ");
	title.append(SQL_RELAY);

	php_info_print_table_start();
	php_info_print_table_header(2,title.getString(),"enabled");
	php_info_print_table_row(2,"Client API version",SQLR_VERSION);
	php_info_print_table_end();
}

#if ZEND_MODULE_API_NO >= 20050922
// NOTE: don't make this const or it will fail to compile with older PHP
static zend_module_dep sqlrelayDeps[] = {
	ZEND_MOD_REQUIRED("pdo")
#ifdef PHP_MOD_END
	ZEND_MOD_END
#else
	{NULL,NULL,NULL}
#endif
};
#endif

// NOTE: don't make this const or it will fail to compile with older PHP
static zend_function_entry sqlrelayFunctions[] = {
#ifdef PHP_FE_END
	PHP_FE_END
#else
	{NULL,NULL,NULL}
#endif
};

zend_module_entry pdo_sqlrelay_module_entry = {
#if ZEND_MODULE_API_NO >= 20050922
	STANDARD_MODULE_HEADER_EX,
	NULL,
	sqlrelayDeps,
#else
	STANDARD_MODULE_HEADER,
#endif
	"pdo_sqlrelay",
	sqlrelayFunctions,
	PHP_MINIT(pdo_sqlrelay),
	PHP_MSHUTDOWN(pdo_sqlrelay),
	NULL,
	NULL,
	PHP_MINFO(pdo_sqlrelay),
	"1.0",
	STANDARD_MODULE_PROPERTIES
};

ZEND_GET_MODULE(pdo_sqlrelay)

}
