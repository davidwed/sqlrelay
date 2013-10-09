/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2013 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: David Muse <david.muse@firstworks.com>                       |
  +----------------------------------------------------------------------+
*/

#include <config.h>
#define NEED_IS_BIT_TYPE_CHAR
#define NEED_IS_BOOL_TYPE_CHAR
#define NEED_IS_NUMBER_TYPE_CHAR
#define NEED_IS_BLOB_TYPE_CHAR
#include <datatypes.h>
#include <php.h>
#include <php_ini.h>
#include <ext/standard/info.h>
#include <pdo/php_pdo.h>
#include <pdo/php_pdo_driver.h>
#include <sqlrelay/sqlrclient.h>

extern "C" {

struct sqlrstatement {
	sqlrcursor	*sqlrcur;
	int64_t		currentrow;
};

static int sqlrcursorDestructor(pdo_stmt_t *stmt TSRMLS_DC) {
	sqlrstatement	*sqlrstmt=(sqlrstatement *)stmt->driver_data;
	delete sqlrstmt->sqlrcur;
	delete sqlrstmt;
	return 1;
}

static int sqlrcursorExecute(pdo_stmt_t *stmt TSRMLS_DC) {

	sqlrstatement	*sqlrstmt=(sqlrstatement *)stmt->driver_data;
	sqlrcursor	*sqlrcur=sqlrstmt->sqlrcur;
	if (!sqlrcur->executeQuery()) {
		return 0;
	}
	sqlrstmt->currentrow=-1;
	stmt->executed=1;
	stmt->column_count=sqlrcur->colCount();
	stmt->row_count=sqlrcur->rowCount();
	return 1;
}

static int sqlrcursorFetch(pdo_stmt_t *stmt,
				enum pdo_fetch_orientation ori,
				long offset TSRMLS_DC) {
	sqlrstatement	*sqlrstmt=(sqlrstatement *)stmt->driver_data;
	sqlrstmt->currentrow++;
	return 1;
}

static int sqlrcursorDescribe(pdo_stmt_t *stmt, int colno TSRMLS_DC) {

	sqlrstatement	*sqlrstmt=(sqlrstatement *)stmt->driver_data;
	sqlrcursor	*sqlrcur=sqlrstmt->sqlrcur;
	const char	*name=sqlrcur->getColumnName(colno);
	const char	*type=sqlrcur->getColumnType(colno);
	stmt->columns[colno].name=(char *)name;
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
	*ptr=(char *)sqlrstmt->sqlrcur->getField(sqlrstmt->currentrow,colno);
	*len=sqlrstmt->sqlrcur->getFieldLength(sqlrstmt->currentrow,colno);
	return 1;
}

static int sqlrcursorBind(pdo_stmt_t *stmt,
				struct pdo_bound_param_data *param,
				enum pdo_param_event eventtype TSRMLS_DC) {

	sqlrstatement	*sqlrstmt=(sqlrstatement *)stmt->driver_data;
	sqlrcursor	*sqlrcur=sqlrstmt->sqlrcur;
	stringbuffer	paramname;
	paramname.append((uint64_t)param->paramno+1);
	if (eventtype!=PDO_PARAM_EVT_EXEC_PRE) {
		return 1;
	}
	if (!param->is_param) {
		return 1;
	}
	switch (PDO_PARAM_TYPE(param->param_type)) {
		case PDO_PARAM_NULL:
			sqlrcur->inputBind(paramname.getString(),
						(const char *)NULL);
			return 1;
		case PDO_PARAM_INT:
		case PDO_PARAM_BOOL:
			convert_to_long(param->parameter);
			sqlrcur->inputBind(paramname.getString(),
						Z_LVAL_P(param->parameter));
			return 1;
		case PDO_PARAM_STR:
			convert_to_string(param->parameter);
			sqlrcur->inputBind(paramname.getString(),
						Z_STRVAL_P(param->parameter),
						Z_STRLEN_P(param->parameter));
			return 1;
		case PDO_PARAM_LOB:
			if (Z_TYPE_P(param->parameter)==IS_STRING) {
				convert_to_string(param->parameter);
				sqlrcur->inputBindClob(paramname.getString(),
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
				sqlrcur->inputBindBlob(paramname.getString(),
						Z_STRVAL_P(param->parameter),
						Z_STRLEN_P(param->parameter));
			}
			return 0;
		case PDO_PARAM_STMT:
			return 0;
	}
	return 0;
}

static int sqlrcursorColumnMetadata(pdo_stmt_t *stmt,
					long colno,
					zval *return_value TSRMLS_DC) {

	sqlrstatement	*sqlrstmt=(sqlrstatement *)stmt->driver_data;
	sqlrcursor	*sqlrcur=(sqlrcursor *)sqlrstmt->sqlrcur;

	array_init(return_value);
	add_assoc_string(return_value,"native_type",
				(char *)sqlrcur->getColumnType(colno),0);

	zval	*flags=NULL;
	MAKE_STD_ZVAL(flags);
	array_init(flags);
	if (sqlrcur->getColumnIsNullable(colno)) {
		add_next_index_string(return_value,"nullable",0);
	}
	if (sqlrcur->getColumnIsPrimaryKey(colno)) {
		add_next_index_string(return_value,"primary_key",0);
	}
	if (sqlrcur->getColumnIsUnique(colno)) {
		add_next_index_string(return_value,"unique",0);
	}
	if (sqlrcur->getColumnIsPartOfKey(colno)) {
		add_next_index_string(return_value,"part_of_key",0);
	}
	if (sqlrcur->getColumnIsUnsigned(colno)) {
		add_next_index_string(return_value,"unsigned",0);
	}
	if (sqlrcur->getColumnIsZeroFilled(colno)) {
		add_next_index_string(return_value,"zero_filled",0);
	}
	if (sqlrcur->getColumnIsBinary(colno)) {
		add_next_index_string(return_value,"binary",0);
	}
	if (sqlrcur->getColumnIsAutoIncrement(colno)) {
		add_next_index_string(return_value,"auto_increment",0);
	}
	add_assoc_zval(return_value,"flags",flags);
	return 1;
}

static struct pdo_stmt_methods sqlrcursorMethods={
	sqlrcursorDestructor,
	sqlrcursorExecute,
	sqlrcursorFetch,
	sqlrcursorDescribe,
	sqlrcursorGetField,
	sqlrcursorBind,
	NULL, // set attr
	NULL, // get attr
	sqlrcursorColumnMetadata,
	NULL, // next rowset
	NULL  // close
};


// what is this function for?
int _pdo_sqlrelay_error(pdo_dbh_t *dbh,
			pdo_stmt_t *stmt,
			const char *file,
			int line TSRMLS_DC) {
	return ((sqlrconnection *)dbh->driver_data)->errorNumber();
}

static int sqlrconnectionClose(pdo_dbh_t *dbh TSRMLS_DC) {
	dbh->is_closed=1;
	delete (sqlrconnection *)dbh->driver_data;
	return 0;
}

static int sqlrconnectionPrepare(pdo_dbh_t *dbh, const char *sql,
					long sqllen, pdo_stmt_t *stmt,
					zval *driveroptions TSRMLS_DC) {
	sqlrstatement	*sqlrstmt=new sqlrstatement;
	sqlrstmt->sqlrcur=new sqlrcursor((sqlrconnection *)dbh->driver_data);
	sqlrstmt->currentrow=-1;
	stmt->methods=&sqlrcursorMethods;
	stmt->driver_data=(void *)sqlrstmt;
	stmt->executed=0;
	stmt->supports_placeholders=1;
	stmt->column_count=0;
	stmt->columns=NULL;
	stmt->row_count=0;
	sqlrstmt->sqlrcur->prepareQuery(sql,sqllen);
	if (dbh->oracle_nulls) {
		sqlrstmt->sqlrcur->getNullsAsNulls();
	} else {
		sqlrstmt->sqlrcur->getNullsAsEmptyStrings();
	}
	sqlrstmt->sqlrcur=new sqlrcursor((sqlrconnection *)dbh->driver_data);
	return 1;
}

static long sqlrconnectionExecute(pdo_dbh_t *dbh,
					const char *sql,
					long sqllen TSRMLS_DC) {
	sqlrcursor	sqlrcur((sqlrconnection *)dbh->driver_data);
	long	retval=-1;
	if (sqlrcur.sendQuery(sql,sqllen)) {
		retval=sqlrcur.affectedRows();
		if (retval==-1) {
			retval=0;
		}
	}
	return retval;
}

static int sqlrconnectionBegin(pdo_dbh_t *dbh TSRMLS_DC) {
	return ((sqlrconnection *)dbh->driver_data)->begin();
}

static int sqlrconnectionCommit(pdo_dbh_t *dbh TSRMLS_DC) {
	return ((sqlrconnection *)dbh->driver_data)->commit();
}

static int sqlrconnectionRollback(pdo_dbh_t *dbh TSRMLS_DC) { 
	return ((sqlrconnection *)dbh->driver_data)->rollback();
}

static int sqlrconnectionSetAttribute(pdo_dbh_t *dbh,
					long attr, zval *val TSRMLS_DC) {

	sqlrconnection	*sqlrcon=(sqlrconnection *)dbh->driver_data;
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
		case PDO_ATTR_ERRMODE:
			// control how errors are handled
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
		case PDO_ATTR_CASE:
			// control case folding for portability
			return 1;
		case PDO_ATTR_CURSOR_NAME:
			// name a cursor for use in "WHERE CURRENT OF <name>"
			return 1;
		case PDO_ATTR_CURSOR:
			// cursor type
			return 1;
		case PDO_ATTR_ORACLE_NULLS:
			// convert empty strings to NULL
			convert_to_boolean(val);
			dbh->oracle_nulls=(Z_BVAL_P(val)==TRUE);
			return 1;
		case PDO_ATTR_PERSISTENT:
			// pconnect style connection
			return 1;
		case PDO_ATTR_STATEMENT_CLASS:
			// array(classname, array(ctor_args)) to specify
			// the class of the constructed statement
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
		case PDO_ATTR_STRINGIFY_FETCHES:
			// converts integer/float types to strings during fetch
			convert_to_boolean(val);
			dbh->stringify=(Z_BVAL_P(val)==TRUE);
			return 1;
		case PDO_ATTR_MAX_COLUMN_LEN:
			// make database calculate maximum
			// length of data found in a column
			return 1;
		case PDO_ATTR_DEFAULT_FETCH_MODE:
			// Set the default fetch mode
			return 1;
		case PDO_ATTR_EMULATE_PREPARES:
			// use query emulation rather than native
			return 1;
		default:
			return 0;
	}
}

static char *sqlrconnectionLastInsertId(pdo_dbh_t *dbh,
				const char *name, unsigned int *len TSRMLS_DC) {
	return charstring::parseNumber(
		((sqlrconnection *)dbh->driver_data)->getLastInsertId());
}

static int sqlrconnectionError(pdo_dbh_t *dbh,
					pdo_stmt_t *stmt,
					zval *info TSRMLS_DC) {

	sqlrconnection	*sqlrcon=(sqlrconnection *)dbh->driver_data;
	sqlrcon->errorNumber();
	sqlrcon->errorMessage();
	return 1;
}

static int sqlrconnectionGetAttribute(pdo_dbh_t *dbh,
				long attr, zval *retval TSRMLS_DC) {

	sqlrconnection	*sqlrcon=(sqlrconnection *)dbh->driver_data;
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
		case PDO_ATTR_ERRMODE:
			// control how errors are handled
			return 1;
		case PDO_ATTR_SERVER_VERSION:
			// database server version
			ZVAL_STRING(retval,(char *)sqlrcon->serverVersion(),1);
			return 1;
		case PDO_ATTR_CLIENT_VERSION:
			// client library version
			ZVAL_STRING(retval,(char *)sqlrcon->clientVersion(),0);
			return 1;
		case PDO_ATTR_SERVER_INFO:
			// server information
			return 1;
		case PDO_ATTR_CONNECTION_STATUS:
			// connection status
			return 1;
		case PDO_ATTR_CASE:
			// control case folding for portability
			return 1;
		case PDO_ATTR_CURSOR_NAME:
			// name a cursor for use in "WHERE CURRENT OF <name>"
			return 1;
		case PDO_ATTR_CURSOR:
			// cursor type
			return 1;
		case PDO_ATTR_ORACLE_NULLS:
			// convert empty strings to NULL
			ZVAL_LONG(retval,dbh->oracle_nulls);
			return 1;
		case PDO_ATTR_PERSISTENT:
			// pconnect style connection
			return 1;
		case PDO_ATTR_STATEMENT_CLASS:
			// array(classname, array(ctor_args)) to specify
			// the class of the constructed statement
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
			ZVAL_STRING(retval,"sqlrelay",0);
			return 1;
		case PDO_ATTR_STRINGIFY_FETCHES:
			// converts integer/float types to strings during fetch
			ZVAL_BOOL(retval,dbh->stringify);
			return 1;
		case PDO_ATTR_MAX_COLUMN_LEN:
			// make database calculate maximum
			// length of data found in a column
			return 1;
		case PDO_ATTR_DEFAULT_FETCH_MODE:
			// Set the default fetch mode
			return 1;
		case PDO_ATTR_EMULATE_PREPARES:
			// use query emulation rather than native
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
	};
	php_pdo_parse_data_source(dbh->data_source,
					dbh->data_source_len,
					options,5);
	const char	*host=options[0].optval;
	uint16_t	port=charstring::toInteger(options[1].optval);
	const char	*socket=options[2].optval;
	int32_t		tries=charstring::toInteger(options[3].optval);
	int32_t		retrytime=charstring::toInteger(options[4].optval);

	// create a sqlrconnection and attach it to the dbh
	sqlrconnection	*sqlrcon=new sqlrconnection(host,port,socket,
							dbh->username,
							dbh->password,
							tries,retrytime);
	dbh->driver_data=(void *)sqlrcon;
	dbh->methods=&sqlrconnectionMethods;

	dbh->is_persistent=0;
	dbh->auto_commit=0;
	dbh->is_closed=0;
	dbh->alloc_own_columns=1;
	dbh->in_txn=1;
	dbh->max_escaped_char_length=2;
	dbh->oracle_nulls=0; // don't convert empty strings to nulls by default
	dbh->stringify=1;

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


static const zend_function_entry sqlrelayFunctions[] = {
	PHP_FE_END
};

#if ZEND_MODULE_API_NO >= 20050922
static const zend_module_dep sqlrelayDeps[] = {
	ZEND_MOD_REQUIRED("pdo")
	ZEND_MOD_END
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
