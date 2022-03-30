// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

// Some versions of PHP 7.0 need INT64_MIN/INT64_MAX to be defined but
// these gyrations are necessary when using C++.
#include <rudiments/private/config.h>
#ifdef RUDIMENTS_HAVE_STDINT_H
	#define __STDC_LIMIT_MACROS
	#include <stdint.h>
#endif

#include <config.h>
#define NEED_IS_BIT_TYPE_CHAR 1
#define NEED_IS_BOOL_TYPE_CHAR 1
#define NEED_IS_NUMBER_TYPE_CHAR 1
#define NEED_IS_FLOAT_TYPE_CHAR 1
#define NEED_IS_BLOB_TYPE_CHAR 1
#include <datatypes.h>
#include <defines.h>
#define NEED_IS_BIND_DELIMITER 1
#define NEED_BEFORE_BIND_VARIABLE 1
#define NEED_AFTER_BIND_VARIABLE 1
#include <bindvariables.h>
#include <sqlrelay/sqlrclient.h>
#include <rudiments/stringbuffer.h>
#include <rudiments/singlylinkedlist.h>
#include <rudiments/charstring.h>
#include <rudiments/character.h>
#include <rudiments/sys.h>

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

	#define ZVAL zval*

	#define GET_PARAMETERS zend_parse_parameters
	#define PARAMS(a) a,

	#define PHP_STREAM_COPY_TO_MEM(a,b) \
		php_stream_copy_to_mem(a,PHP_STREAM_COPY_ALL,0)
	#define PHP_STREAM_TO_ZVAL(a,b) php_stream_to_zval(a,&b)
	#define PHP_STREAM_TO_ZVAL_P(a,b) php_stream_to_zval(a,b)

	#define MY_ZVAL_STRING(a,b,c) ZVAL_STRING(a,b)

	#define RET_STRING(a,b) \
		RETURN_STR(zend_string_init(a,charstring::length(a),0))

	#define ADD_ASSOC_STRING(a,b,c,d) \
		add_assoc_string(a,b,\
			zend_string_init(c,charstring::length(c),0)->val)

	#define CONVERT_TO_STRING(a) convert_to_string(&(a))
	#define CONVERT_TO_STRING_EX(a) convert_to_string_ex(&(a))
	#define CONVERT_TO_LONG(a) convert_to_long(&(a))
	#define CONVERT_TO_LONG_EX(a) convert_to_long_ex(&(a))

	#define SVAL(a) Z_STRVAL(a)
	#define SLEN(a) Z_STRLEN(a)
	#define LVAL(a) Z_LVAL(a)
	#define TYPE(a) Z_TYPE(a)
	#define TYPE_P(a) Z_TYPE_P(a)

	#define ISTRUE(a) (Z_TYPE_P(a)==IS_TRUE)

	#define MAKE_STD_ZVAL(a)

	#define ADD_NEXT_INDEX_STRING(a,b) add_next_index_string(&a,b)
	#define ADD_NEXT_INDEX_STRING_P(a,b) add_next_index_string(a,b)
	#define ADD_ASSOC_ZVAL(a,b,c) add_assoc_zval(a,b,&c)

	#define	getStmt()	(pdo_stmt_t *)Z_PDO_STMT_P(getThis())
	#define	getDbh()	(pdo_dbh_t *)Z_PDO_DBH_P(getThis())

	#if PHP_MAJOR_VERSION >= 8
		#define TSRMLS_DC
		#define TSRMLS_CC
		#define TSRMLS_FETCH()
	#endif

#else

	#define ZVAL zval**

	#define GET_PARAMETERS zend_get_parameters_ex
	#define PARAMS(a)

	#define PHP_STREAM_COPY_TO_MEM(a,b) \
		php_stream_copy_to_mem(a,b,PHP_STREAM_COPY_ALL,0)
	#define PHP_STREAM_TO_ZVAL(a,b) php_stream_to_zval(a,b)

	#define MY_ZVAL_STRING(a,b,c) ZVAL_STRING(a,b,c)

	#define RET_STRING RETURN_STRING

	#define ADD_ASSOC_STRING(a,b,c,d) add_assoc_string(a,b,c,d)

	#define CONVERT_TO_STRING(a) convert_to_string(a)
	#define CONVERT_TO_STRING_EX(a) convert_to_string_ex(a)
	#define CONVERT_TO_LONG(a) convert_to_long(a)
	#define CONVERT_TO_LONG_EX(a) convert_to_long_ex(a)

	#define SVAL(a) Z_STRVAL_P(a)
	#define SLEN(a) Z_STRLEN_P(a)
	#define LVAL(a) Z_LVAL_P(a)
	#define TYPE(a) Z_TYPE_P(a)

	#define ISTRUE(a) (Z_BVAL_P(a)==TRUE)

	#define ADD_NEXT_INDEX_STRING(a,b) add_next_index_string(a,b,1)
	#define ADD_NEXT_INDEX_STRING_P(a,b) add_next_index_string(a,b,1)
	#define ADD_ASSOC_ZVAL(a,b,c) add_assoc_zval(a,b,c)

	#define	getStmt() \
		(pdo_stmt_t *)zend_object_store_get_object(getThis() TSRMLS_CC)
	#define getDbh() \
		(pdo_dbh_t *)zend_object_store_get_object(getThis() TSRMLS_CC);
#endif

#define sqlrelayError(s) \
	_sqlrelayError(s,NULL,__FILE__,__LINE__ TSRMLS_CC)
#define sqlrelayErrorStmt(s) \
	_sqlrelayError(s->dbh,s,__FILE__,__LINE__ TSRMLS_CC)

struct sqlrstatement {
	sqlrcursor			*sqlrcur;
	int64_t				currentrow;
	long				longfield;
	zval				zvalfield;
	stringbuffer			subvarquery;
	singlylinkedlist< char * >	subvarstrings;
	bool				fwdonly;
	bool				emulatepreparesunicodestrings;
	bool				fetchlobsasstrings;
};

struct sqlrdbhandle {
	sqlrconnection	*sqlrcon;
	bool		translatebindsonserver;
	bool		usesubvars;
	bool		emulatepreparesunicodestrings;
	int64_t		resultsetbuffersize;
	bool		dontgetcolumninfo;
	bool		nullsasnulls;
	bool		fetchlobsasstrings;
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
	PDO_SQLRELAY_ATTR_CURRENT_DB,
	PDO_SQLRELAY_ATTR_CONNECTION_TIMEOUT,
	PDO_SQLRELAY_ATTR_RESPONSE_TIMEOUT,
	PDO_SQLRELAY_ATTR_SQLRELAY_VERSION,
	PDO_SQLRELAY_ATTR_RUDIMENTS_VERSION,
	PDO_SQLRELAY_ATTR_CLIENT_INFO,
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
					"SQLSTATE[%s] [%lld] %s",
					*pdoerr,(long long)errornumber,
					errormessage);
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
					"SQLSTATE[HY000] [%lld] %s",
					(long long)errornumber,errormessage);
}

static void clearList(singlylinkedlist< char * > *list) {
	for (listnode< char * > *node=list->getFirst();
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
#if PHP_MAJOR_VERSION >= 7
				zend_long offset TSRMLS_DC
#else
				long offset TSRMLS_DC
#endif
				) {

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
#if PHP_MAJOR_VERSION >= 7
	stmt->columns[colno].name=zend_string_init(n,charstring::length(n),0);
#else
	char		*name=estrdup((n)?n:"");
	stmt->columns[colno].name=name;
	stmt->columns[colno].namelen=charstring::length(name);
#endif
	stmt->columns[colno].maxlen=sqlrcur->getColumnLength(colno);
#if PHP_MAJOR_VERSION < 8 || (PHP_MAJOR_VERSION == 8 && PHP_MINOR_VERSION < 1)
	const char	*type=sqlrcur->getColumnType(colno);
	if (isBitTypeChar(type) || isNumberTypeChar(type)) {
		if (isFloatTypeChar(type)) {
			#ifdef HAVE_PHP_PDO_PARAM_ZVAL
			stmt->columns[colno].param_type=PDO_PARAM_ZVAL;
			#else
			stmt->columns[colno].param_type=PDO_PARAM_STR;
			#endif
		} else {
			stmt->columns[colno].param_type=PDO_PARAM_INT;
		}
	} else if (isBlobTypeChar(type)) {
		stmt->columns[colno].param_type=
			(sqlrstmt->fetchlobsasstrings)?
				PDO_PARAM_STR:PDO_PARAM_LOB;
	} else if (isBoolTypeChar(type)) {
		stmt->columns[colno].param_type=PDO_PARAM_BOOL;
	} else {
		stmt->columns[colno].param_type=PDO_PARAM_STR;
	}
#endif
	stmt->columns[colno].precision=sqlrcur->getColumnPrecision(colno);
	return 1;
}

static int sqlrcursorGetField(pdo_stmt_t *stmt,
				int colno,
#if PHP_MAJOR_VERSION < 8 || (PHP_MAJOR_VERSION == 8 && PHP_MINOR_VERSION < 1)
				char **ptr,
	#if PHP_MAJOR_VERSION >= 7
				size_t *len,
	#else
				unsigned long *len,
	#endif
				int *caller_frees TSRMLS_DC
#else
				zval *result,
				enum pdo_param_type *coltype TSRMLS_DC
#endif
				) {

	sqlrstatement	*sqlrstmt=(sqlrstatement *)stmt->driver_data;
	sqlrcursor	*sqlrcur=sqlrstmt->sqlrcur;

#if PHP_MAJOR_VERSION < 8 || (PHP_MAJOR_VERSION == 8 && PHP_MINOR_VERSION < 1)
	*caller_frees=0;

	switch (stmt->columns[colno].param_type) {
		// NOTE: Currently, we only use ZVAL's for doubles,
		// but we could do an additional float check here.
		#ifdef HAVE_PHP_PDO_PARAM_ZVAL
		case PDO_PARAM_ZVAL:
			// handle NULLs
			if (!sqlrcur->getFieldLength(
                                        sqlrstmt->currentrow,colno)) {
                                *ptr=(char *)sqlrcur->getField(
                                        sqlrstmt->currentrow,colno);
                                *len=0;
                                return 1;
                        }
			ZVAL_DOUBLE(&sqlrstmt->zvalfield,(double)sqlrcur->
				getFieldAsDouble(sqlrstmt->currentrow,colno));
			*ptr=(char *)&sqlrstmt->zvalfield;
			*len=sizeof(sqlrstmt->zvalfield);
			return 1;
		#endif
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
#else
	enum pdo_param_type ctype;

	const char	*ptr=
			sqlrcur->getField(sqlrstmt->currentrow,colno);
	uint64_t	 len=
			sqlrcur->getFieldLength(sqlrstmt->currentrow,colno);
	const char	*type=sqlrcur->getColumnType(colno);

	if (!len) {

		// handle NULLs and empty strings
		if (ptr) {
			if (isBlobTypeChar(type)) {
				// empty lobs must be sent as empty streams
				php_stream	*strm=
						php_stream_memory_create(
							TEMP_STREAM_DEFAULT);
				PHP_STREAM_TO_ZVAL_P(strm,result);
				ctype=PDO_PARAM_LOB;
			} else {
				ZVAL_STRINGL(result,ptr,0);
				ctype=PDO_PARAM_STR;
			}
		} else {
			ZVAL_NULL(result);
			ctype=PDO_PARAM_NULL;
		}

	} else {

		if (isBitTypeChar(type) || isNumberTypeChar(type)) {
			if (isFloatTypeChar(type)) {
				// FIXME: is this correct, there is no
				// PDO_PARAM_ZVAL or PDO_PARAM_DOUBLE,
				// or should I just make a string out of it?
				ZVAL_DOUBLE(result,
					sqlrcur->getFieldAsDouble(
						sqlrstmt->currentrow,colno));
				ctype=PDO_PARAM_STR;
			} else {
				ZVAL_LONG(result,
					sqlrcur->getFieldAsInteger(
						sqlrstmt->currentrow,colno));
				ctype=PDO_PARAM_INT;
			}
		} else if (isBlobTypeChar(type)) {
			php_stream	*strm=php_stream_memory_create(
							TEMP_STREAM_DEFAULT);
			TSRMLS_FETCH();
			php_stream_write(strm,ptr,len);
			php_stream_seek(strm,0,SEEK_SET);
			PHP_STREAM_TO_ZVAL_P(strm,result);
			ctype=PDO_PARAM_LOB;
		} else if (isBoolTypeChar(type)) {
			ZVAL_BOOL(result,
				(bool)sqlrcur->getFieldAsInteger(
						sqlrstmt->currentrow,colno));
			ctype=PDO_PARAM_BOOL;
		} else {
			ZVAL_STRINGL(result,ptr,len);
			ctype=PDO_PARAM_STR;
		}
	}
	if (coltype) {
		*coltype=ctype;
	}
	return 1;
#endif
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
			CONVERT_TO_LONG(param->parameter);
			sqlrcur->substitution(nm,LVAL(param->parameter));
			return 1;
		case PDO_PARAM_STR:
			CONVERT_TO_STRING(param->parameter);
			str=new char[SLEN(param->parameter)+3];
			if (sqlrstmt->emulatepreparesunicodestrings) {
				charstring::copy(str,"N'");
			} else {
				charstring::copy(str,"'");
			}
			charstring::append(str,
					SVAL(param->parameter),
					SLEN(param->parameter));
			str[SLEN(param->parameter)+1]='\0';
			charstring::append(str,"'");
			sqlrstmt->subvarstrings.append(str);
			sqlrcur->substitution(nm,str);
			return 1;
		case PDO_PARAM_LOB:
			if (TYPE(param->parameter)==IS_STRING) {
				CONVERT_TO_STRING(param->parameter);
				str=new char[SLEN(param->parameter)+3];
				if (sqlrstmt->emulatepreparesunicodestrings) {
					charstring::copy(str,"N'");
				} else {
					charstring::copy(str,"'");
				}
				charstring::append(str,
						SVAL(param->parameter),
						SLEN(param->parameter));
				str[SLEN(param->parameter)+1]='\0';
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

	// Bind null values as NULL, without requiring developer to specify
	// PARAM_NULL in PDOStatement::bindValue - this is the expected
	// behavior with other PDO drivers
	if (TYPE(param->parameter)==IS_NULL) {
		sqlrcur->inputBind(name,(const char *)NULL);
		return 1;
	}

	switch (PDO_PARAM_TYPE(param->param_type)) {
		case PDO_PARAM_NULL:
			sqlrcur->inputBind(name,(const char *)NULL);
			return 1;
		case PDO_PARAM_INT:
		case PDO_PARAM_BOOL:
			CONVERT_TO_LONG(param->parameter);
			sqlrcur->inputBind(name,LVAL(param->parameter));
			return 1;
		case PDO_PARAM_STR:
			CONVERT_TO_STRING(param->parameter);
			sqlrcur->inputBind(name,SVAL(param->parameter),
						SLEN(param->parameter));
			return 1;
		case PDO_PARAM_LOB:
			if (TYPE(param->parameter)==IS_STRING) {
				CONVERT_TO_STRING(param->parameter);
				sqlrcur->inputBindBlob(name,
						SVAL(param->parameter),
						SLEN(param->parameter));
			} else if (TYPE(param->parameter)==IS_RESOURCE) {
				TSRMLS_FETCH();
				php_stream	*strm=NULL;
				php_stream_from_zval_no_verify(
						strm,&param->parameter);
				if (!strm) {
					return 0;
				}
				SEPARATE_ZVAL(&param->parameter);
#if PHP_MAJOR_VERSION >= 7
				Z_TYPE_INFO(param->parameter)=IS_STRING;
				param->parameter.value.str=
#else
				Z_TYPE_P(param->parameter)=IS_STRING;
				SLEN(param->parameter)=
#endif
					PHP_STREAM_COPY_TO_MEM(strm,
						&SVAL(param->parameter));
				sqlrcur->inputBindBlob(name,
						SVAL(param->parameter),
						SLEN(param->parameter));
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

	char	*strvalue;
	#if PHP_MAJOR_VERSION >= 7
	zval	*parameter=(Z_ISREF(param->parameter))?
				Z_REFVAL(param->parameter):
				&param->parameter;
	#endif

	switch (PDO_PARAM_TYPE(param->param_type)) {
		case PDO_PARAM_NULL:
			#if PHP_MAJOR_VERSION >= 7
				ZVAL_NULL(parameter);
			#else
				ZVAL_NULL(param->parameter);
			#endif
			return 1;
		case PDO_PARAM_INT:
			#if PHP_MAJOR_VERSION >= 7
				ZVAL_LONG(parameter,
					sqlrcur->getOutputBindInteger(name));
			#else
				ZVAL_LONG(param->parameter,
					sqlrcur->getOutputBindInteger(name));
			#endif
			return 1;
		case PDO_PARAM_BOOL:
			#if PHP_MAJOR_VERSION >= 7
				ZVAL_BOOL(parameter,
					sqlrcur->getOutputBindInteger(name));
			#else
				ZVAL_BOOL(param->parameter,
					sqlrcur->getOutputBindInteger(name));
			#endif
			return 1;
		case PDO_PARAM_STR:
			strvalue=(char *)sqlrcur->getOutputBindString(name);
			if (!strvalue) {
			#if PHP_MAJOR_VERSION >= 7
				ZVAL_NULL(parameter);
			} else {
				MY_ZVAL_STRING(parameter,strvalue,1);
			#else
				ZVAL_NULL(param->parameter);
			} else {
				MY_ZVAL_STRING(param->parameter,strvalue,1);
			#endif
			}
			return 1;
		case PDO_PARAM_LOB:
			{
			php_stream	*strm=NULL;
			if (
			#if PHP_MAJOR_VERSION >= 7
				TYPE_P(parameter)
			#else
				TYPE(param->parameter)
			#endif
					==IS_STRING) {
				TSRMLS_FETCH();
				strm=php_stream_memory_create(
							TEMP_STREAM_DEFAULT);
			} else if (
			#if PHP_MAJOR_VERSION >= 7
				TYPE_P(parameter)
			#else
				TYPE(param->parameter)
			#endif
					==IS_RESOURCE) {
				TSRMLS_FETCH();
				#if PHP_MAJOR_VERSION >= 7
					php_stream_from_zval_no_verify(
							strm,parameter);
				#else
					php_stream_from_zval_no_verify(
							strm,&param->parameter);
				#endif
			}
			if (!strm) {
				return 0;
			}
			TSRMLS_FETCH();
			php_stream_write(strm,
				sqlrcur->getOutputBindBlob(name),
				sqlrcur->getOutputBindLength(name));
			php_stream_seek(strm,0,SEEK_SET);
			#if PHP_MAJOR_VERSION >= 7
				if (TYPE_P(parameter)==IS_STRING) {
					PHP_STREAM_TO_ZVAL_P(strm,parameter);
				}
			#else
				if (TYPE(param->parameter)==IS_STRING) {
					PHP_STREAM_TO_ZVAL(strm,
							param->parameter);
				}
			#endif
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
#if PHP_MAJOR_VERSION >= 7
	const char	*name=(param->name &&
				param->name->len)?
				param->name->val:paramname.getString();
#else
	const char	*name=(param->name)?
				param->name:paramname.getString();
#endif

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
#if PHP_MAJOR_VERSION >= 7
					zend_long attr,
#else
					long attr,
#endif
					zval *val TSRMLS_DC) {

	sqlrstatement	*sqlrstmt=(sqlrstatement *)stmt->driver_data;
	sqlrcursor	*sqlrcur=sqlrstmt->sqlrcur;

	switch (attr) {
		case PDO_SQLRELAY_ATTR_RESULT_SET_BUFFER_SIZE:
			convert_to_long(val);
			sqlrcur->setResultSetBufferSize(Z_LVAL_P(val));
			return 1;
		case PDO_SQLRELAY_ATTR_DONT_GET_COLUMN_INFO:
			convert_to_boolean(val);
			if (ISTRUE(val)) {
				sqlrcur->dontGetColumnInfo();
			} else {
				sqlrcur->getColumnInfo();
			}
			return 1;
		case PDO_SQLRELAY_ATTR_GET_NULLS_AS_EMPTY_STRINGS:
			convert_to_boolean(val);
			if (ISTRUE(val)) {
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
#if PHP_MAJOR_VERSION >= 7
					zend_long attr,
#else
					long attr,
#endif
					zval *retval TSRMLS_DC) {

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
#if PHP_MAJOR_VERSION >= 7
					zend_long colno,
#else
					long colno,
#endif
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
#if PHP_MAJOR_VERSION >= 7
	zval	flags;
	array_init(&flags);
#else
	zval	*flags=NULL;
	MAKE_STD_ZVAL(flags);
	array_init(flags);
#endif
	if (sqlrcur->getColumnIsNullable(colno)) {
		ADD_NEXT_INDEX_STRING(flags,"nullable");
	}
	if (sqlrcur->getColumnIsPrimaryKey(colno)) {
		ADD_NEXT_INDEX_STRING(flags,"primary_key");
	}
	if (sqlrcur->getColumnIsUnique(colno)) {
		ADD_NEXT_INDEX_STRING(flags,"unique");
	}
	if (sqlrcur->getColumnIsPartOfKey(colno)) {
		ADD_NEXT_INDEX_STRING(flags,"part_of_key");
	}
	if (sqlrcur->getColumnIsUnsigned(colno)) {
		ADD_NEXT_INDEX_STRING(flags,"unsigned");
	}
	if (sqlrcur->getColumnIsZeroFilled(colno)) {
		ADD_NEXT_INDEX_STRING(flags,"zero_filled");
	}
	if (sqlrcur->getColumnIsBinary(colno)) {
		ADD_NEXT_INDEX_STRING(flags,"binary");
	}
	if (sqlrcur->getColumnIsAutoIncrement(colno)) {
		ADD_NEXT_INDEX_STRING(flags,"auto_increment");
	}
	ADD_ASSOC_ZVAL(returnvalue,"flags",flags);
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


static
#if PHP_MAJOR_VERSION < 8 || (PHP_MAJOR_VERSION == 8 && PHP_MINOR_VERSION < 1)
int
#else
void
#endif
sqlrconnectionClose(pdo_dbh_t *dbh TSRMLS_DC) {
	sqlrdbhandle	*sqlrdbh=(sqlrdbhandle *)dbh->driver_data;
	delete sqlrdbh->sqlrcon;
	dbh->is_closed=1;
	#if PHP_MAJOR_VERSION < 8 || \
		(PHP_MAJOR_VERSION == 8 && PHP_MINOR_VERSION < 1)
	return 0;
	#endif
}

static void sqlrconnectionRewriteQuery(sqlrconnection *sqlrcon,
						const char *query,
						uint32_t querylen,
						stringbuffer *newquery) {

	queryparsestate_t	parsestate=IN_QUERY;

	uint16_t	varcounter=0;

	// run through the querybuffer...
	const char	*ptr=query;
	const char	*endptr=ptr+querylen;
	char		prev='\0';
	do {

		// if we're in the query...
		if (parsestate==IN_QUERY) {

			// if we find a quote, we're in quotes
			if (*ptr=='\'') {
				parsestate=IN_QUOTES;
			}

			// if we find whitespace or a couple of other things
			// then the next thing could be a bind variable
			if (beforeBindVariable(ptr)) {
				parsestate=BEFORE_BIND;
			}

			// append the character
			newquery->append(*ptr);

			// move on
			if (*ptr=='\\' && prev=='\\') {
				prev='\0';
			} else {
				prev=*ptr;
			}
			ptr++;
			continue;
		}

		// copy anything in quotes verbatim
		if (parsestate==IN_QUOTES) {

			// if we find a quote, but not an escaped quote,
			// then we're back in the query
			// (or we're in between one of these: '...''...'
			// which is functionally the same)
			if (*ptr=='\'' && prev!='\\') {
				parsestate=IN_QUERY;
			}

			// append the character
			newquery->append(*ptr);

			// move on
			if (*ptr=='\\' && prev=='\\') {
				prev='\0';
			} else {
				prev=*ptr;
			}
			ptr++;
			continue;
		}

		if (parsestate==BEFORE_BIND) {

			// if we find a bind variable...
			if (isBindDelimiter(ptr,
				sqlrcon->
				getBindVariableDelimiterQuestionMarkSupported(),
				sqlrcon->
				getBindVariableDelimiterColonSupported(),
				sqlrcon->
				getBindVariableDelimiterAtSignSupported(),
				sqlrcon->
				getBindVariableDelimiterDollarSignSupported())
				) {
				parsestate=IN_BIND;
				continue;
			}

			// if we didn't find a bind variable then we're just
			// back in the query
			parsestate=IN_QUERY;
			continue;
		}

		// if we're in a bind variable...
		if (parsestate==IN_BIND) {

			// If we find whitespace or a few other things
			// then we're done with the bind variable.  Process it.
			// Otherwise get the variable itself in another buffer.
			bool	endofbind=afterBindVariable(ptr);
			if (endofbind) {

				newquery->append("$(");
				newquery->append(varcounter);
				newquery->append(')');

				varcounter++;

				parsestate=IN_QUERY;

			} else {

				// move on
				if (*ptr=='\\' && prev=='\\') {
					prev='\0';
				} else {
					prev=*ptr;
				}
				ptr++;
			}
			continue;
		}

	} while (ptr<endptr);
}

static
#if PHP_MAJOR_VERSION < 8 || (PHP_MAJOR_VERSION == 8 && PHP_MINOR_VERSION < 1)
int
#else
bool
#endif
sqlrconnectionPrepare(pdo_dbh_t *dbh,
#if PHP_MAJOR_VERSION < 8 || (PHP_MAJOR_VERSION == 8 && PHP_MINOR_VERSION < 1)
				const char *sql,
	#if PHP_MAJOR_VERSION >= 7
				size_t sqllen,
	#else
				long sqllen,
	#endif
#else
				zend_string *zsql,
#endif
				pdo_stmt_t *stmt,
				zval *driveroptions TSRMLS_DC) {

	sqlrdbhandle	*sqlrdbh=(sqlrdbhandle *)dbh->driver_data;
	sqlrstatement	*sqlrstmt=new sqlrstatement;
	sqlrstmt->sqlrcur=new sqlrcursor(sqlrdbh->sqlrcon,true);

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

	sqlrstmt->emulatepreparesunicodestrings=
		sqlrdbh->emulatepreparesunicodestrings;

	sqlrstmt->fetchlobsasstrings=
		sqlrdbh->fetchlobsasstrings;

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

#if PHP_MAJOR_VERSION > 8 || (PHP_MAJOR_VERSION == 8 && PHP_MINOR_VERSION >= 1)
	const char	*sql=ZSTR_VAL(zsql);
	size_t		sqllen=ZSTR_LEN(zsql);
#endif

	if (sqlrdbh->usesubvars) {
		sqlrconnectionRewriteQuery(sqlrdbh->sqlrcon,sql,sqllen,
							&sqlrstmt->subvarquery);
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

#if PHP_MAJOR_VERSION < 8 || (PHP_MAJOR_VERSION == 8 && PHP_MINOR_VERSION < 1)
	return 1;
#else
	return true;
#endif
}

static
#if PHP_MAJOR_VERSION >= 7
zend_long
#else
long
#endif
sqlrconnectionExecute(pdo_dbh_t *dbh,
#if PHP_MAJOR_VERSION < 8 || (PHP_MAJOR_VERSION == 8 && PHP_MINOR_VERSION < 1)
					const char *sql,
	#if PHP_MAJOR_VERSION >= 7
					size_t sqllen TSRMLS_DC
	#else
					long sqllen TSRMLS_DC
	#endif
#else
					const zend_string *zsql TSRMLS_DC
#endif
					) {

#if PHP_MAJOR_VERSION > 8 || (PHP_MAJOR_VERSION == 8 && PHP_MINOR_VERSION >= 1)
	const char	*sql=ZSTR_VAL(zsql);
	size_t		sqllen=ZSTR_LEN(zsql);
#endif

	sqlrdbhandle	*sqlrdbh=(sqlrdbhandle *)dbh->driver_data;
	sqlrcursor	sqlrcur((sqlrconnection *)sqlrdbh->sqlrcon);
	if (sqlrcur.sendQuery(sql,sqllen)) {
		return sqlrcur.affectedRows();
	}
	sqlrelayError(dbh);
	return -1;
}

static
#if PHP_MAJOR_VERSION < 8 || (PHP_MAJOR_VERSION == 8 && PHP_MINOR_VERSION < 1)
int
#else
zend_string *
#endif
sqlrconnectionQuote(pdo_dbh_t *dbh,
#if PHP_MAJOR_VERSION < 8 || (PHP_MAJOR_VERSION == 8 && PHP_MINOR_VERSION < 1)
					const char *unquoted,
	#if PHP_MAJOR_VERSION >= 7
					size_t unquotedlen,
	#else
					int unquotedlen,
	#endif
					char **quoted,
	#if PHP_MAJOR_VERSION >= 7
					size_t *quotedlen,
	#else
					int *quotedlen,
	#endif
#else
					const zend_string *zunquoted,
#endif
					enum pdo_param_type paramtype TSRMLS_DC
					) {

#if PHP_MAJOR_VERSION >= 7
	size_t	i;
#else
	int	i;
#endif

#if PHP_MAJOR_VERSION < 8 || (PHP_MAJOR_VERSION == 8 && PHP_MINOR_VERSION < 1)
	// fail if quoted/quotedlen weren't provided
	if (!quoted || !quotedlen) {
		return 0;
	}
#else
	const char	*unquoted=ZSTR_VAL(zunquoted);
	size_t		unquotedlen=ZSTR_LEN(zunquoted);

	// this is awkward, but it allows us to use
	// the exisiting code below with PHP 8.1+
	char	*q=NULL;
	size_t	qlen=0;
	char	**quoted=&q;
	size_t	*quotedlen=&qlen;
#endif

	// determine size of new string
	*quotedlen=unquotedlen+2;
	for (i=0; i<unquotedlen; i++) {
		if (unquoted[i]=='\'') {
			(*quotedlen)++;
		}
	}

	// allocate new string
	*quoted=(char *)safe_emalloc((*quotedlen)+1,sizeof(char),0);

	// quote the string
	char	*ptr=*quoted;
	*ptr='\'';
	ptr++;
	for (i=0; i<unquotedlen; i++) {
		if (unquoted[i]=='\'') {
			*ptr='\'';
			ptr++;
		}
		*ptr=unquoted[i];
		ptr++;
	}
	*ptr='\'';
	ptr++;
	*ptr='\0';

#if PHP_MAJOR_VERSION < 8 || (PHP_MAJOR_VERSION == 8 && PHP_MINOR_VERSION < 1)
	return 1;
#else
	// FIXME: free q?
	return zend_string_init(q,qlen,0);
#endif
}

static
#if PHP_MAJOR_VERSION < 8 || (PHP_MAJOR_VERSION == 8 && PHP_MINOR_VERSION < 1)
int
#else
bool
#endif
sqlrconnectionBegin(pdo_dbh_t *dbh TSRMLS_DC) {
	sqlrdbhandle	*sqlrdbh=(sqlrdbhandle *)dbh->driver_data;
	if (((sqlrconnection *)sqlrdbh->sqlrcon)->begin()) {
		return 1;
	}
	sqlrelayError(dbh);
	return 0;
}

static
#if PHP_MAJOR_VERSION < 8 || (PHP_MAJOR_VERSION == 8 && PHP_MINOR_VERSION < 1)
int
#else
bool
#endif
sqlrconnectionCommit(pdo_dbh_t *dbh TSRMLS_DC) {
	sqlrdbhandle	*sqlrdbh=(sqlrdbhandle *)dbh->driver_data;
	if (((sqlrconnection *)sqlrdbh->sqlrcon)->commit()) {
		return 1;
	}
	sqlrelayError(dbh);
	return 0;
}

static
#if PHP_MAJOR_VERSION < 8 || (PHP_MAJOR_VERSION == 8 && PHP_MINOR_VERSION < 1)
int
#else
bool
#endif
sqlrconnectionRollback(pdo_dbh_t *dbh TSRMLS_DC) { 
	sqlrdbhandle	*sqlrdbh=(sqlrdbhandle *)dbh->driver_data;
	if (((sqlrconnection *)sqlrdbh->sqlrcon)->rollback()) {
		return 1;
	}
	sqlrelayError(dbh);
	return 0;
}

static
#if PHP_MAJOR_VERSION < 8 || (PHP_MAJOR_VERSION == 8 && PHP_MINOR_VERSION < 1)
int
#else
bool
#endif
sqlrconnectionSetAttribute(pdo_dbh_t *dbh,
#if PHP_MAJOR_VERSION >= 7
					zend_long attr,
#else
					long attr,
#endif
					zval *val TSRMLS_DC) {

	sqlrdbhandle	*sqlrdbh=(sqlrdbhandle *)dbh->driver_data;
	sqlrconnection	*sqlrcon=(sqlrconnection *)sqlrdbh->sqlrcon;

	// PDO handles several of these options itself.  These are the ones
	// it doens't handle.
	switch (attr) {
		case PDO_ATTR_AUTOCOMMIT:
			// use to turn on or off auto-commit mode
			convert_to_boolean(val);
			if (dbh->auto_commit!=ISTRUE(val)) {
				dbh->auto_commit=ISTRUE(val);
				if (ISTRUE(val)) {
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
			// all timeouts in seconds
			convert_to_long(val);
			sqlrcon->setConnectTimeout(Z_LVAL_P(val),0);
			sqlrcon->setResponseTimeout(Z_LVAL_P(val),0);
			return 1;
	        case PDO_SQLRELAY_ATTR_CONNECTION_TIMEOUT:
			// connection timeout in seconds
			convert_to_long(val);
			sqlrcon->setConnectTimeout(Z_LVAL_P(val),0);
			return 1;
	        case PDO_SQLRELAY_ATTR_RESPONSE_TIMEOUT:
			// cresponse timeout in seconds
			convert_to_long(val);
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
		case PDO_SQLRELAY_ATTR_CLIENT_INFO:
			// client information
			convert_to_string(val);
			sqlrcon->setClientInfo(Z_STRVAL_P(val));
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
			sqlrdbh->usesubvars=ISTRUE(val);
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

static
#if PHP_MAJOR_VERSION < 8 || (PHP_MAJOR_VERSION == 8 && PHP_MINOR_VERSION < 1)
char *
#else
zend_string *
#endif
sqlrconnectionLastInsertId(pdo_dbh_t *dbh,
#if PHP_MAJOR_VERSION < 8 || (PHP_MAJOR_VERSION == 8 && PHP_MINOR_VERSION < 1)
					const char *name,
	#if PHP_MAJOR_VERSION >= 7
					size_t *len TSRMLS_DC
	#else
					unsigned int *len TSRMLS_DC
	#endif
#else
					const zend_string *name TSRMLS_DC
#endif
					) {
	sqlrdbhandle	*sqlrdbh=(sqlrdbhandle *)dbh->driver_data;
	char	*id=charstring::parseNumber(
				((sqlrconnection *)sqlrdbh->sqlrcon)->
							getLastInsertId());
#if PHP_MAJOR_VERSION < 8 || (PHP_MAJOR_VERSION == 8 && PHP_MINOR_VERSION < 1)
	*len=charstring::length(id);
	return id;
#else
	return zend_string_init(id,charstring::length(id),0);
#endif
}

static
#if PHP_MAJOR_VERSION < 8 || (PHP_MAJOR_VERSION == 8 && PHP_MINOR_VERSION < 1)
int
#else
void
#endif
sqlrconnectionError(pdo_dbh_t *dbh, pdo_stmt_t *stmt, zval *info TSRMLS_DC) {

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
			ADD_NEXT_INDEX_STRING_P(info,msg);
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
			ADD_NEXT_INDEX_STRING_P(info,msg);
		}
	}
#if PHP_MAJOR_VERSION < 8 || (PHP_MAJOR_VERSION == 8 && PHP_MINOR_VERSION < 1)
	return 1;
#endif
}

static int sqlrconnectionGetAttribute(pdo_dbh_t *dbh,
#if PHP_MAJOR_VERSION >= 7
					zend_long attr,
#else
					long attr,
#endif
					zval *retval TSRMLS_DC) {

	sqlrdbhandle	*sqlrdbh=(sqlrdbhandle *)dbh->driver_data;
	sqlrconnection	*sqlrcon=(sqlrconnection *)sqlrdbh->sqlrcon;

	// PDO handles several of these options itself.  These are the ones
	// it doens't handle.
	char		*temp;
	int32_t		timeoutsec;
	int32_t		timeoutusec;
	long double	timeout;
	switch (attr) {
		case PDO_ATTR_AUTOCOMMIT:
			// use to turn on or off auto-commit mode
			ZVAL_BOOL(retval,dbh->auto_commit);
			return 1;
		case PDO_ATTR_PREFETCH:
			// configure the prefetch size for drivers
			// that support it. Size is in KB
			return 1;
		case PDO_SQLRELAY_ATTR_CONNECTION_TIMEOUT:
			sqlrcon->getConnectTimeout(&timeoutsec,&timeoutusec);
			timeout=timeoutsec+timeoutusec*1.0E-6;
			ZVAL_DOUBLE(retval,timeout);
			return 1;
		case PDO_ATTR_TIMEOUT:
			// Generic timeout. The closest concept would be the
			// response timeout, so just fall through to that.
		case PDO_SQLRELAY_ATTR_RESPONSE_TIMEOUT:
			sqlrcon->getResponseTimeout(&timeoutsec,&timeoutusec);
			timeout=timeoutsec+timeoutusec*1.0E-6;
			ZVAL_DOUBLE(retval,timeout);
			return 1;
		case PDO_SQLRELAY_ATTR_SQLRELAY_VERSION:
			temp=(char *)SQLR_VERSION;
			if (temp) {
				MY_ZVAL_STRING(retval,temp,1);
			}
			return 1;
	        case PDO_SQLRELAY_ATTR_RUDIMENTS_VERSION:
			temp=(char *)sys::getRudimentsVersion();
			if (temp) {
				MY_ZVAL_STRING(retval,temp,1);
			}
			return 1;
	        case PDO_SQLRELAY_ATTR_CLIENT_INFO:
			temp=(char *)((sqlrcon!=NULL)?
					sqlrcon->getClientInfo():NULL);
			if (temp) {
				MY_ZVAL_STRING(retval,temp,1);
			}
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
	pdo_dbh_t	*dbh=getDbh();
	sqlrdbhandle	*sqlrdbh=(sqlrdbhandle *)dbh->driver_data;
	sqlrconnection	*sqlrcon=(sqlrconnection *)sqlrdbh->sqlrcon;
	uint16_t	port=sqlrcon->getConnectionPort();
	RETURN_LONG(port);
}

static PHP_METHOD(PDO_SQLRELAY, getConnectionSocket) {
	pdo_dbh_t	*dbh=getDbh();
	sqlrdbhandle	*sqlrdbh=(sqlrdbhandle *)dbh->driver_data;
	sqlrconnection	*sqlrcon=(sqlrconnection *)sqlrdbh->sqlrcon;
	const char	*socket=sqlrcon->getConnectionSocket();
	RET_STRING((char *)socket,1);
}

static PHP_METHOD(PDO_SQLRELAY, suspendSession) {
	pdo_dbh_t	*dbh=getDbh();
	sqlrdbhandle	*sqlrdbh=(sqlrdbhandle *)dbh->driver_data;
	sqlrconnection	*sqlrcon=(sqlrconnection *)sqlrdbh->sqlrcon;
	if (sqlrcon->suspendSession()) {
		RETURN_TRUE;
	}
	sqlrelayError(dbh);
	RETURN_FALSE;
}

static PHP_METHOD(PDO_SQLRELAY, resumeSession) {

	ZVAL	port;
	ZVAL	socket;
	if (ZEND_NUM_ARGS()!=2 ||
		GET_PARAMETERS(ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("zz")
				&port,
				&socket)==FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(port);
	convert_to_string_ex(socket);

	pdo_dbh_t	*dbh=getDbh();
	sqlrdbhandle	*sqlrdbh=(sqlrdbhandle *)dbh->driver_data;
	sqlrconnection	*sqlrcon=(sqlrconnection *)sqlrdbh->sqlrcon;
	if (sqlrcon->resumeSession(
				#if PHP_MAJOR_VERSION >= 7
					Z_LVAL_P(port),Z_STRVAL_P(socket)
				#else
					(*port)->value.lval,
					(*socket)->value.str.val
				#endif
				)) {
		RETURN_TRUE;
	}
	sqlrelayError(dbh);
	RETURN_FALSE;
}

static PHP_METHOD(PDO_SQLRELAY, endSession) {
	pdo_dbh_t	*dbh=getDbh();
	sqlrdbhandle	*sqlrdbh=(sqlrdbhandle *)dbh->driver_data;
	sqlrconnection	*sqlrcon=(sqlrconnection *)sqlrdbh->sqlrcon;
	sqlrcon->endSession();
}

// NOTE: don't make this const or it will fail to compile with older PHP
static zend_function_entry sqlrelayConnectionFunctions[]={
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
	pdo_stmt_t	*stmt=getStmt();
	sqlrstatement	*sqlrstmt=(sqlrstatement *)stmt->driver_data;
	sqlrcursor	*sqlrcur=sqlrstmt->sqlrcur;
	uint16_t	id=sqlrcur->getResultSetId();
	RETURN_LONG(id);
}

static PHP_METHOD(PDO_SQLRELAY, suspendResultSet) {
	pdo_stmt_t	*stmt=getStmt();
	sqlrstatement	*sqlrstmt=(sqlrstatement *)stmt->driver_data;
	sqlrcursor	*sqlrcur=sqlrstmt->sqlrcur;
	sqlrcur->suspendResultSet();
	RETURN_TRUE;
}

static PHP_METHOD(PDO_SQLRELAY, resumeResultSet) {

	ZVAL	id;
	if (ZEND_NUM_ARGS()!=1 ||
		GET_PARAMETERS(ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("z")
				&id)==FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(id);

	pdo_stmt_t	*stmt=getStmt();
	sqlrstatement	*sqlrstmt=(sqlrstatement *)stmt->driver_data;
	sqlrcursor	*sqlrcur=sqlrstmt->sqlrcur;
	if (sqlrcur->resumeResultSet(
				#if PHP_MAJOR_VERSION >= 7
					Z_LVAL_P(id)
				#else
					(*id)->value.lval
				#endif
				)) {
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
static zend_function_entry sqlrelayCursorFunctions[]={
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
						int kind TSRMLS_DC
						) {
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
	sqlrconnectionQuote,
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
		{"db",(char *)"",0},
		{"connecttime",(char *)"",0},
		// FIXME: sort this out...  George Carrette suggests this to
		// make SQL Relay consistent with other PDO drivers, but the
		// change makes it inconsistent with SQL Relay drivers for
		// other languages...
		//{"autocommit",(char *)"1",0},
		{"autocommit",(char *)"0",0},
		{"bindvariabledelimiters",(char *)"?:@$",0},
		{"emulatepreparesunicodestrings",(char *)"0",0},
		{"fetchlobsasstrings",(char *)"0",0}
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
	bool		lazyconnect=!charstring::isNo(options[6].optval);
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
	const char	*db=options[22].optval;
	const char      *connecttime=options[23].optval;
	bool		autocommit=!charstring::isNo(options[24].optval);
	const char	*bindvariabledelimiters=options[25].optval;
	bool		emulatepreparesunicodestrings=
				charstring::isYes(options[26].optval);
	bool		fetchlobsasstrings=
				charstring::isYes(options[27].optval);

	// create a sqlrconnection and attach it to the dbh
	sqlrdbhandle	*sqlrdbh=new sqlrdbhandle;
	sqlrdbh->sqlrcon=new sqlrconnection(host,port,socket,
							dbh->username,
							dbh->password,
							tries,retrytime,
							true);

	// enable kerberos or tls
	if (charstring::isYes(krb)) {
		sqlrdbh->sqlrcon->enableKerberos(krbservice,krbmech,krbflags);
	} else if (charstring::isYes(tls)) {
		sqlrdbh->sqlrcon->enableTls(tlsversion,
						tlscert,tlspassword,
						tlsciphers,tlsvalidate,
						tlsca,tlsdepth);
	}

	// enable debug
	if (charstring::isYes(debug)) {
		sqlrdbh->sqlrcon->debugOn();
		sqlrdbh->sqlrcon->debugPrintFunction(
				(int (*)(const char *,...))zend_printf);
	} else if (!charstring::isNo(debug) &&
				!charstring::isNullOrEmpty(debug)) {
		sqlrdbh->sqlrcon->setDebugFile(debug);
		sqlrdbh->sqlrcon->debugOn();
	}

	// set connect timeout
	if (charstring::isNumber(connecttime)) {
		int32_t		timeoutsec=charstring::toInteger(connecttime);
		long double	dbl=charstring::toFloatC(connecttime)-
						(long double)timeoutsec;
		int32_t		timeoutusec=(int32_t)(dbl*1000000.0);
		if (timeoutsec>=0) {
			sqlrdbh->sqlrcon->setConnectTimeout(
						timeoutsec,timeoutusec);
		}
	}

	// set bind variable delimiters
	sqlrdbh->sqlrcon->setBindVariableDelimiters(bindvariabledelimiters);

	// if we're not doing lazy connects, then do something lightweight
	// that will verify whether SQL Relay is available or not
	if (!lazyconnect) {

		// Since identify() implicitly uses the ResponseTimeout make
		// sure we adjust it down to the connect timeout...

		// get connect timeout
		int32_t		connecttimeoutsec;
		int32_t		connecttimeoutusec;
		sqlrdbh->sqlrcon->getConnectTimeout(
				&connecttimeoutsec,&connecttimeoutusec);
		long double	connecttimeout=
				connecttimeoutsec+connecttimeoutusec*1.0E-6;

		// get response timeout
		int32_t		responsetimeoutsec;
		int32_t		responsetimeoutusec;
		sqlrdbh->sqlrcon->getResponseTimeout(
				&responsetimeoutsec,&responsetimeoutusec);
		long double	responsetimeout=
				responsetimeoutsec+responsetimeoutusec*1.0E-6;

		// do we need to set the response timeout?
		bool	setresponsetimeout=
				connecttimeout>=0.0 &&
				(responsetimeout<0.0 ||
					connecttimeout<responsetimeout);

		// set the response timeout to the connect timeout, if necessary
		if (setresponsetimeout) {
			sqlrdbh->sqlrcon->setResponseTimeout(
					connecttimeoutsec,connecttimeoutusec);
		}

		// call identify()
		bool	identifyok=sqlrdbh->sqlrcon->identify();
		if (!identifyok) {
			const char	*errormessage=
					sqlrdbh->sqlrcon->errorMessage();
			int64_t		errornumber=
					sqlrdbh->sqlrcon->errorNumber();
			TSRMLS_FETCH();
			zend_throw_exception_ex(php_pdo_get_exception(),
						0 TSRMLS_CC,
						"SQLRelay Connection Failed, "
						"errorNumber %ld: %s",
						errornumber,errormessage);
			delete sqlrdbh->sqlrcon;
			sqlrdbh->sqlrcon=NULL;
			return 0;
		}

		// reset the response timeout
		if (setresponsetimeout) {
			sqlrdbh->sqlrcon->setResponseTimeout(
						responsetimeoutsec,
						responsetimeoutusec);
		}
	}

	if (!charstring::isNullOrEmpty(db)) {
		sqlrdbh->sqlrcon->selectDatabase(db);
	}

	sqlrdbh->resultsetbuffersize=charstring::toInteger(options[6].optval);
	sqlrdbh->dontgetcolumninfo=charstring::isYes(options[7].optval);
	sqlrdbh->nullsasnulls=charstring::isYes(options[8].optval);

	sqlrdbh->translatebindsonserver=false;
	sqlrdbh->usesubvars=false;
	sqlrdbh->emulatepreparesunicodestrings=emulatepreparesunicodestrings;
	sqlrdbh->fetchlobsasstrings=fetchlobsasstrings;

	dbh->driver_data=(void *)sqlrdbh;
	dbh->methods=&sqlrconnectionMethods;

	dbh->is_persistent=0;
	dbh->auto_commit=autocommit;
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
	REGISTER_PDO_CLASS_CONST_LONG("SQLRELAY_ATTR_CONNECTION_TIMEOUT",
				(long)PDO_SQLRELAY_ATTR_CONNECTION_TIMEOUT);
	REGISTER_PDO_CLASS_CONST_LONG("SQLRELAY_ATTR_RESPONSE_TIMEOUT",
				(long)PDO_SQLRELAY_ATTR_RESPONSE_TIMEOUT);
	REGISTER_PDO_CLASS_CONST_LONG("SQLRELAY_ATTR_SQLRELAY_VERSION",
				(long)PDO_SQLRELAY_ATTR_SQLRELAY_VERSION);
	REGISTER_PDO_CLASS_CONST_LONG("SQLRELAY_ATTR_RUDIMENTS_VERSION",
				(long)PDO_SQLRELAY_ATTR_RUDIMENTS_VERSION);
	REGISTER_PDO_CLASS_CONST_LONG("SQLRELAY_ATTR_CLIENT_INFO",
				(long)PDO_SQLRELAY_ATTR_CLIENT_INFO);

	return
		#if PHP_MAJOR_VERSION >= 8
		(zend_result)
		#endif
		php_pdo_register_driver(&sqlrelayDriver);
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
	php_info_print_table_row(2,"Rudiments API version",
					sys::getRudimentsVersion());
#if defined(__DATE__) && defined(__TIME__)
	php_info_print_table_row(2,"Compiled",__DATE__ " " __TIME__);
#endif
	php_info_print_table_end();
}

#if ZEND_MODULE_API_NO >= 20050922
// NOTE: don't make this const or it will fail to compile with older PHP
static zend_module_dep sqlrelayDeps[]={
	ZEND_MOD_REQUIRED("pdo")
#ifdef PHP_MOD_END
	ZEND_MOD_END
#else
	{NULL,NULL,NULL}
#endif
};
#endif

// NOTE: don't make this const or it will fail to compile with older PHP
static zend_function_entry sqlrelayFunctions[]={
#ifdef PHP_FE_END
	PHP_FE_END
#else
	{NULL,NULL,NULL}
#endif
};

zend_module_entry pdo_sqlrelay_module_entry={
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
	SQLR_VERSION,
	STANDARD_MODULE_PROPERTIES
};

ZEND_GET_MODULE(pdo_sqlrelay)

}
