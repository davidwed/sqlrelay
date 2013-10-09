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
#include <php.h>
#include <php_ini.h>
#include <ext/standard/info.h>
#include <pdo/php_pdo.h>
#include <pdo/php_pdo_driver.h>
#include <sqlrelay/sqlrclient.h>

extern "C" {

static int sqlrcursorDestructor(pdo_stmt_t *stmt TSRMLS_DC) {
	delete (sqlrcursor *)stmt->driver_data;
	return 1;
}

static int sqlrcursorExecute(pdo_stmt_t *stmt TSRMLS_DC) {

	sqlrcursor	*sqlrcur=(sqlrcursor *)stmt->driver_data;
	if (!sqlrcur->executeQuery()) {
		return 0;
	}
	stmt->column_count=sqlrcur->colCount();
	stmt->row_count=sqlrcur->rowCount();
	// FIXME: set anything else?
	return 1;
}

static int sqlrcursorFetch(pdo_stmt_t *stmt,
				enum pdo_fetch_orientation ori,
				long offset TSRMLS_DC) {
	sqlrcursor	*sqlrcur=(sqlrcursor *)stmt->driver_data;
	// FIXME: do something
	return 1;
}

static int sqlrcursorDescribe(pdo_stmt_t *stmt, int colno TSRMLS_DC) {

	sqlrcursor	*sqlrcur=(sqlrcursor *)stmt->driver_data;
	// FIXME: fill in stmt->columns[colno].*
	return 1;
}

static int sqlrcursorGetField(pdo_stmt_t *stmt,
				int colno,
				char **ptr,
				unsigned long *len,
				int *caller_frees TSRMLS_DC) {

	sqlrcursor	*sqlrcur=(sqlrcursor *)stmt->driver_data;
	// FIXME: set *ptr and *len to string and length of specified field
	// in current row
	return 1;
}

static int sqlrcursorBind(pdo_stmt_t *stmt,
				struct pdo_bound_param_data *param,
				enum pdo_param_event eventtype TSRMLS_DC) {

	sqlrcursor	*sqlrcur=(sqlrcursor *)stmt->driver_data;
	// FIXME: handle binds
	return 1;
}

static int sqlrcursorColumnMetadata(pdo_stmt_t *stmt,
					long colno,
					zval *return_value TSRMLS_DC) {

	sqlrcursor	*sqlrcur=(sqlrcursor *)stmt->driver_data;
	// FIXME: build associative array with info for the specified column
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
	delete (sqlrconnection *)dbh->driver_data;
	return 0;
}

static int sqlrconnectionPrepare(pdo_dbh_t *dbh, const char *sql,
					long sqllen, pdo_stmt_t *stmt,
					zval *driveroptions TSRMLS_DC) {
	sqlrcursor	*sqlrcur=
			new sqlrcursor((sqlrconnection *)dbh->driver_data);
	stmt->driver_data=(void *)sqlrcur;
	stmt->methods=&sqlrcursorMethods;
	// FIXME: handle driver options
	sqlrcur->prepareQuery(sql,sqllen);
	return 1;
}

static long sqlrconnectionExecute(pdo_dbh_t *dbh,
					const char *sql,
					long sqllen TSRMLS_DC) {
	sqlrcursor	*sqlrcur=
			new sqlrcursor((sqlrconnection *)dbh->driver_data);
	long	retval=-1;
	if (sqlrcur->sendQuery(sql,sqllen)) {
		retval=sqlrcur->affectedRows();
		if (retval==-1) {
			retval=0;
		}
	}
	delete sqlrcur;
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
		// FIXME: others?
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
		case PDO_ATTR_CLIENT_VERSION:
			ZVAL_STRING(retval,(char *)sqlrcon->clientVersion(),1);
			break;

		case PDO_ATTR_SERVER_VERSION:
			ZVAL_STRING(retval,(char *)sqlrcon->serverVersion(),1);
			break;
		case PDO_ATTR_AUTOCOMMIT:
			ZVAL_LONG(retval,dbh->auto_commit);
			break;
		// FIXME: others?
		default:
			return 0;
	}

	return 1;
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
		{"host","",0},
		{"port","",0},
		{"socket","",0},
		{"tries","0",0},
		{"retrytime","1",0},
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

	// FIXME: driver options

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
	// FIXME: replace with actual version
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
