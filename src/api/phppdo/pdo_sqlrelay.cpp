// Copyright (c) 2013 David Muse
// See the file COPYING for more information

#include <config.h>
#define NEED_IS_BIT_TYPE_CHAR
#define NEED_IS_BOOL_TYPE_CHAR
#define NEED_IS_NUMBER_TYPE_CHAR
#define NEED_IS_BLOB_TYPE_CHAR
#include <datatypes.h>
#include <sqlrelay/sqlrclient.h>
#include <rudiments/stringbuffer.h>
#include <rudiments/charstring.h>
#include <rudiments/character.h>

extern "C" {

#include <php.h>
#include <php_ini.h>
#include <ext/standard/info.h>
#include <pdo/php_pdo.h>
#include <pdo/php_pdo_driver.h>
#include <pdo/php_pdo_driver.h>
#include <zend_exceptions.h>

#define sqlrelayError(s) \
	_sqlrelayError(s,NULL,__FILE__,__LINE__ TSRMLS_DC)
#define sqlrelayErrorStmt(s) \
	_sqlrelayError(s->dbh,s,__FILE__,__LINE__ TSRMLS_DC)

struct sqlrstatement {
	sqlrcursor	*sqlrcur;
	int64_t		rows;
	int64_t		currentrow;
	long		longfield;
	stringbuffer	subvarquery;
	linkedlist< char * >	subvarstrings;
};

struct sqlrdbhandle {
	sqlrconnection	*sqlrcon;
	bool		translatebindsonserver;
	bool		usesubvars;
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
		zend_throw_exception_ex(php_pdo_get_exception(),errornumber,
					"SQLSTATE[%s] [%d] %s",
					*pdoerr,errornumber,errormessage);
	}
	return errornumber;
}

static void clearList(linkedlist< char * > *list) {
	for (linkedlistnode< char * > *node=list->getFirstNode();
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
	sqlrstmt->rows=sqlrcur->rowCount();
	stmt->row_count=sqlrcur->affectedRows();
	return 1;
}

static int sqlrcursorFetch(pdo_stmt_t *stmt,
				enum pdo_fetch_orientation ori,
				long offset TSRMLS_DC) {
	sqlrstatement	*sqlrstmt=(sqlrstatement *)stmt->driver_data;
	switch (ori) {
		case PDO_FETCH_ORI_NEXT:
			sqlrstmt->currentrow++;
			break;
		case PDO_FETCH_ORI_PRIOR:
			sqlrstmt->currentrow--;
			break;
		case PDO_FETCH_ORI_FIRST:
			sqlrstmt->currentrow=0;
			break;
		case PDO_FETCH_ORI_LAST:
			sqlrstmt->currentrow=sqlrstmt->rows-1;
			break;
		case PDO_FETCH_ORI_ABS:
			sqlrstmt->currentrow=offset;
			break;
		case PDO_FETCH_ORI_REL:
			sqlrstmt->currentrow+=offset;
			break;
	}
	if (sqlrstmt->currentrow<-1) {
		sqlrstmt->currentrow=-1;
	}
	return (sqlrstmt->currentrow>-1 && sqlrstmt->currentrow<sqlrstmt->rows);
}

static int sqlrcursorDescribe(pdo_stmt_t *stmt, int colno TSRMLS_DC) {

	sqlrstatement	*sqlrstmt=(sqlrstatement *)stmt->driver_data;
	sqlrcursor	*sqlrcur=sqlrstmt->sqlrcur;
	char		*name=estrdup(sqlrcur->getColumnName(colno));
	const char	*type=sqlrcur->getColumnType(colno);
	stmt->columns[colno].name=name;
	stmt->columns[colno].namelen=charstring::length(name);
	stmt->columns[colno].maxlen=sqlrcur->getColumnLength(colno);
	if (isBitTypeChar(type) || isNumberTypeChar(type)) {
		stmt->columns[colno].param_type=PDO_PARAM_INT;
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
				php_stream	*strm=NULL;
				php_stream_from_zval_no_verify(
						strm,&param->parameter);
				if (!strm) {
					return 0;
				}
				SEPARATE_ZVAL(&param->parameter);
				Z_TYPE_P(param->parameter)=IS_STRING;
				Z_STRLEN_P(param->parameter)=
					php_stream_copy_to_mem(strm,
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
			// FIXME: use pdo_stmt_construct here
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
			ZVAL_STRING(param->parameter,
				(char *)sqlrcur->getOutputBindString(name),1);
			return 1;
		case PDO_PARAM_LOB:
			{
			php_stream	*strm=php_stream_memory_create(
							TEMP_STREAM_DEFAULT);
			php_stream_write(strm,
				sqlrcur->getOutputBindBlob(name),
				sqlrcur->getOutputBindLength(name));
			php_stream_seek(strm,0,SEEK_SET);
			php_stream_to_zval(strm,param->parameter);
			}
			return 1;
		case PDO_PARAM_STMT:
			// FIXME: there's no obvious way to create a PDO
			// statement object to attach this to
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
	if (character::inSet(name[0],":@$")) {
		name++;
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
	// not supported but might be supported in the
	// future so we'll let it fail silently for now
	return 0;
}

static int sqlrcursorGetAttribute(pdo_stmt_t *stmt,
					long attr, zval *retval TSRMLS_DC) {
	// not supported but might be supported in the
	// future so we'll let it fail silently for now
	return 0;
}

static int sqlrcursorColumnMetadata(pdo_stmt_t *stmt,
					long colno,
					zval *returnvalue TSRMLS_DC) {

	sqlrstatement	*sqlrstmt=(sqlrstatement *)stmt->driver_data;
	sqlrcursor	*sqlrcur=(sqlrcursor *)sqlrstmt->sqlrcur;

	array_init(returnvalue);

	// native type
	const char	*type=sqlrcur->getColumnType(colno);
	add_assoc_string(returnvalue,"native_type",(char *)type,1);


	// pdo type
	int32_t		pdotype=PDO_PARAM_STR;
	if (isBitTypeChar(type) || isNumberTypeChar(type)) {
		pdotype=PDO_PARAM_INT;
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

	sqlrstmt->rows=0;
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
	
	sqlrstmt->sqlrcur->prepareQuery(sql,sqllen);
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

	// FIXME: somehow support
	// 	setResultSetBufferSize
	// 	get/dontGetColumnInfo
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
		case PDO_ATTR_EMULATE_PREPARES:
			// use substititution variables rather than binds
			sqlrdbh->usesubvars=Z_BVAL_P(val);
			return 1;
		default:
			return 0;
	}
}

static char *sqlrconnectionLastInsertId(pdo_dbh_t *dbh,
				const char *name, unsigned int *len TSRMLS_DC) {
	sqlrdbhandle	*sqlrdbh=(sqlrdbhandle *)dbh->driver_data;
	char	*id=php_pdo_int64_to_str(
				((sqlrconnection *)sqlrdbh->sqlrcon)->
							getLastInsertId());
	*len=charstring::length(id);
	return id;
}

static int sqlrconnectionError(pdo_dbh_t *dbh,
					pdo_stmt_t *stmt,
					zval *info TSRMLS_DC) {

	// FIXME: the first index in the info array should be the sqlstate
	// currently we're leaving it at 00000 but it really ought to be
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

	// FIXME: somehow support 
	// 	getResultSetBufferSize
	// 	get/dontGetColumnInfo
	//	identify
	//	dbVersion
	//	dbHostName
	//	dbIpAddress
	//	bindFormat
	//	getCurrentDatabase
	sqlrdbhandle	*sqlrdbh=(sqlrdbhandle *)dbh->driver_data;
	sqlrconnection	*sqlrcon=(sqlrconnection *)sqlrdbh->sqlrcon;

	// PDO handles several of these options itself.  These are the ones
	// it doens't handle.
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
			ZVAL_STRING(retval,(char *)sqlrcon->serverVersion(),1);
			return 1;
		case PDO_ATTR_CLIENT_VERSION:
			// client library version
			ZVAL_STRING(retval,(char *)sqlrcon->clientVersion(),1);
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
		case PDO_ATTR_EMULATE_PREPARES:
			// use substititution variables rather than binds
			ZVAL_BOOL(retval,sqlrdbh->usesubvars);
			return 1;
		default:
			return 0;
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
	NULL // check liveness
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
	};
	php_pdo_parse_data_source(dbh->data_source,
					dbh->data_source_len,
					options,6);
	const char	*host=options[0].optval;
	uint16_t	port=charstring::toInteger(options[1].optval);
	const char	*socket=options[2].optval;
	int32_t		tries=charstring::toInteger(options[3].optval);
	int32_t		retrytime=charstring::toInteger(options[4].optval);
	bool		debug=charstring::toInteger(options[5].optval);

	// create a sqlrconnection and attach it to the dbh
	sqlrdbhandle	*sqlrdbh=new sqlrdbhandle;
	sqlrdbh->sqlrcon=new sqlrconnection(host,port,socket,
							dbh->username,
							dbh->password,
							tries,retrytime,
							true);
	if (debug) {
		sqlrdbh->sqlrcon->debugOn();
	}
	sqlrdbh->sqlrcon->debugPrintFunction(zend_printf);

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
	return php_pdo_register_driver(&sqlrelayDriver);
}

static PHP_MSHUTDOWN_FUNCTION(pdo_sqlrelay) {
	php_pdo_unregister_driver(&sqlrelayDriver);
	return SUCCESS;
}

static PHP_MINFO_FUNCTION(pdo_sqlrelay) {
	php_info_print_table_start();
	php_info_print_table_header(2, "PDO Driver for SQL Relay", "enabled");
	php_info_print_table_row(2, "Client API version", SQLR_VERSION);
	php_info_print_table_end();
}


// NOTE: don't make this const or it will fail to compile with older PHP
static zend_function_entry sqlrelayFunctions[] = {
#ifdef PHP_FE_END
	PHP_FE_END
#else
	{NULL,NULL,NULL}
#endif
};

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
