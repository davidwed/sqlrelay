/* Copyright (c) 2000  Adam Kropielnicki
   See the file COPYING for more information */

#include <sqlrelay/sqlrclient.h>

extern "C" {
	#ifdef __cplusplus
		#undef __cplusplus
		#define cpluspluswasdefined
	#endif
	#define HAVE_SOCKLEN_T
	#include <php.h>
	#ifdef cpluspluswasdefined
		#define __cplusplus
	#endif
}

#include <config.h>

extern "C" {

#if WIN32|WINNT
#include <windows.h>
#define DLEXPORT __declspec(dllexport)
#else
#define DLEXPORT
#endif

static int sqlrelay_connection;
static int sqlrelay_cursor;

#ifdef ZEND_MODULE_STARTUP_D
static void sqlrcon_cleanup(zend_rsrc_list_entry *rsrc TSRMLS_DC) {
	sqlrconnection	*connection=(sqlrconnection *)rsrc->ptr;
	delete connection;
}

static void sqlrcur_cleanup(zend_rsrc_list_entry *rsrc TSRMLS_DC) {
	sqlrcursor	*cursor=(sqlrcursor *)rsrc->ptr;
	delete cursor;
}

ZEND_MODULE_STARTUP_D(sqlrelay) {
	sqlrelay_connection=zend_register_list_destructors_ex(sqlrcon_cleanup,
				NULL,"sqlrelay connection",module_number);
	sqlrelay_cursor=zend_register_list_destructors_ex(sqlrcur_cleanup,
				NULL,"sqlrelay cursor",module_number);
	return SUCCESS;
}
#endif

DLEXPORT ZEND_FUNCTION(sqlrcon_alloc) {
	zval **server,**port,**socket,**user,**password,**retrytime,**tries;
	sqlrconnection *connection=NULL;
	if (ZEND_NUM_ARGS() != 7 || 
		zend_get_parameters_ex(7,&server,&port,&socket,
			&user,&password,&retrytime,&tries ) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(server);
	convert_to_long_ex(port);
	convert_to_string_ex(socket);
	convert_to_string_ex(user);
	convert_to_string_ex(password);
	convert_to_long_ex(retrytime);
	convert_to_long_ex(tries);
	connection=new sqlrconnection((*server)->value.str.val,
			(*port)->value.lval,(*socket)->value.str.val,
			(*user)->value.str.val,(*password)->value.str.val,
			(*retrytime)->value.lval,(*tries)->value.lval,true);
	connection->debugPrintFunction(zend_printf);
	ZEND_REGISTER_RESOURCE(return_value,connection,sqlrelay_connection);
}

DLEXPORT ZEND_FUNCTION(sqlrcon_free) {
	zval **sqlrcon;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcon) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrconnection *connection=NULL;
	ZEND_FETCH_RESOURCE(connection,sqlrconnection *,sqlrcon,-1,"sqlrelay connection",sqlrelay_connection);
	zend_list_delete((*sqlrcon)->value.lval);
}

DLEXPORT ZEND_FUNCTION(sqlrcon_setconnecttimeout) {
	zval **sqlrcur,**timeoutsec,**timeoutusec;
	if (ZEND_NUM_ARGS() != 3 || 
		zend_get_parameters_ex(3,&sqlrcur,&timeoutsec,&timeoutusec) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(timeoutsec);
	convert_to_long_ex(timeoutusec);
	sqlrconnection *connection=NULL;
	ZEND_FETCH_RESOURCE(connection,sqlrconnection *,sqlrcur,-1,"sqlrelay connection",sqlrelay_connection);
	if (connection) {
		connection->setConnectTimeout((*timeoutsec)->value.lval,(*timeoutusec)->value.lval);
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcon_setauthenticationtimeout) {
	zval **sqlrcur,**timeoutsec,**timeoutusec;
	if (ZEND_NUM_ARGS() != 3 || 
		zend_get_parameters_ex(3,&sqlrcur,&timeoutsec,&timeoutusec) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(timeoutsec);
	convert_to_long_ex(timeoutusec);
	sqlrconnection *connection=NULL;
	ZEND_FETCH_RESOURCE(connection,sqlrconnection *,sqlrcur,-1,"sqlrelay connection",sqlrelay_connection);
	if (connection) {
		connection->setAuthenticationTimeout((*timeoutsec)->value.lval,(*timeoutusec)->value.lval);
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcon_setresponsetimeout) {
	zval **sqlrcur,**timeoutsec,**timeoutusec;
	if (ZEND_NUM_ARGS() != 3 || 
		zend_get_parameters_ex(3,&sqlrcur,&timeoutsec,&timeoutusec) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(timeoutsec);
	convert_to_long_ex(timeoutusec);
	sqlrconnection *connection=NULL;
	ZEND_FETCH_RESOURCE(connection,sqlrconnection *,sqlrcur,-1,"sqlrelay connection",sqlrelay_connection);
	if (connection) {
		connection->setResponseTimeout((*timeoutsec)->value.lval,(*timeoutusec)->value.lval);
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcon_endsession) {
	zval **sqlrcon;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcon) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrconnection *connection=NULL;
	ZEND_FETCH_RESOURCE(connection,sqlrconnection *,sqlrcon,-1,"sqlrelay connection",sqlrelay_connection);
	if (connection) {
		connection->endSession();
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcon_suspendsession) {
	zval **sqlrcon;
	bool r;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcon) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrconnection *connection=NULL;
	ZEND_FETCH_RESOURCE(connection,sqlrconnection *,sqlrcon,-1,"sqlrelay connection",sqlrelay_connection);
	if (connection) {
		r=connection->suspendSession();
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcon_getconnectionport) {
	zval **sqlrcon;
	uint16_t r;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcon) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrconnection *connection=NULL;
	ZEND_FETCH_RESOURCE(connection,sqlrconnection *,sqlrcon,-1,"sqlrelay connection",sqlrelay_connection);
	if (connection) {
		r=connection->getConnectionPort();
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcon_getconnectionsocket) {
	zval **sqlrcon;
	const char *r;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcon) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrconnection *connection=NULL;
	ZEND_FETCH_RESOURCE(connection,sqlrconnection *,sqlrcon,-1,"sqlrelay connection",sqlrelay_connection);
	if (connection) {
		r=connection->getConnectionSocket();
		if (r) {
			RETURN_STRING(const_cast<char *>(r),1);
		}
	}
	RETURN_FALSE;
}

DLEXPORT ZEND_FUNCTION(sqlrcon_resumesession) {
	zval **sqlrcon,**port,**socket;
	bool r;
	if (ZEND_NUM_ARGS() != 3 || 
		zend_get_parameters_ex(3,&sqlrcon,&port,&socket) 
					== FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(port);
	convert_to_string_ex(socket);
	sqlrconnection *connection=NULL;
	ZEND_FETCH_RESOURCE(connection,sqlrconnection *,sqlrcon,-1,"sqlrelay connection",sqlrelay_connection);
	if (connection) {
		r=connection->resumeSession((*port)->value.lval,(*socket)->value.str.val);
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcon_errormessage) {
	zval **sqlrcon;
	const char *r;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcon) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrconnection *connection=NULL;
	ZEND_FETCH_RESOURCE(connection,sqlrconnection *,sqlrcon,-1,"sqlrelay connection",sqlrelay_connection);
	if (connection) {
		r=connection->errorMessage();
		if (r) {
			RETURN_STRING(const_cast<char *>(r),1);
		}
	}
	RETURN_NULL();
}

DLEXPORT ZEND_FUNCTION(sqlrcon_errornumber) {
	zval **sqlrcon;
	int64_t r;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcon) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrconnection *connection=NULL;
	ZEND_FETCH_RESOURCE(connection,sqlrconnection *,sqlrcon,-1,"sqlrelay connection",sqlrelay_connection);
	if (connection) {
		r=connection->errorNumber();
		if (r) {
			RETURN_LONG(r);
		}
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcon_debugon) {
	zval **sqlrcon;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcon) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrconnection *connection=NULL;
	ZEND_FETCH_RESOURCE(connection,sqlrconnection *,sqlrcon,-1,"sqlrelay connection",sqlrelay_connection);
	if (connection) {
		connection->debugOn();
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcon_debugoff) {
	zval **sqlrcon;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcon) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrconnection *connection=NULL;
	ZEND_FETCH_RESOURCE(connection,sqlrconnection *,sqlrcon,-1,"sqlrelay connection",sqlrelay_connection);
	if (connection) {
		connection->debugOff();
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcon_getdebug) {
	zval **sqlrcon;
	bool r;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcon) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrconnection *connection=NULL;
	ZEND_FETCH_RESOURCE(connection,sqlrconnection *,sqlrcon,-1,"sqlrelay connection",sqlrelay_connection);
	if (connection) {
		r=connection->getDebug();
		if (r) {
			RETURN_TRUE;
		}
	}
	RETURN_FALSE;
}

DLEXPORT ZEND_FUNCTION(sqlrcon_setdebugfile) {
	zval **sqlrcon,**filename;
	if (ZEND_NUM_ARGS() != 2 || 
		zend_get_parameters_ex(2,&sqlrcon,&filename) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(filename);
	sqlrconnection *connection=NULL;
	ZEND_FETCH_RESOURCE(connection,sqlrconnection *,sqlrcon,-1,"sqlrelay connection",sqlrelay_connection);
	if (connection) {
		connection->setDebugFile((*filename)->value.str.val);
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcon_setclientinfo) {
	zval **sqlrcon,**clientinfo;
	if (ZEND_NUM_ARGS() != 2 || 
		zend_get_parameters_ex(2,&sqlrcon,&clientinfo) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(clientinfo);
	sqlrconnection *connection=NULL;
	ZEND_FETCH_RESOURCE(connection,sqlrconnection *,sqlrcon,-1,"sqlrelay connection",sqlrelay_connection);
	if (connection) {
		connection->setClientInfo((*clientinfo)->value.str.val);
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcon_getclientinfo) {
	zval **sqlrcon;
	const char *r;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcon) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrconnection *connection=NULL;
	ZEND_FETCH_RESOURCE(connection,sqlrconnection *,sqlrcon,-1,"sqlrelay connection",sqlrelay_connection);
	if (connection) {
		r=connection->getClientInfo();
		if (r) {
			RETURN_STRING(const_cast<char *>(r),1);
		}
	}
	RETURN_FALSE;
}

DLEXPORT ZEND_FUNCTION(sqlrcur_alloc) {
	zval	**sqlrcon;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcon) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrconnection *connection=NULL;
	ZEND_FETCH_RESOURCE(connection,sqlrconnection *,sqlrcon,-1,"sqlrelay connection",sqlrelay_connection);
	if (!connection) {
		RETURN_LONG(0);
	}
	sqlrcursor	*cursor=new sqlrcursor(connection,true);
	ZEND_REGISTER_RESOURCE(return_value,cursor,sqlrelay_cursor);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_free) {
	zval **sqlrcur;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcur) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	zend_list_delete((*sqlrcur)->value.lval);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_setresultsetbuffersize) {
	zval **sqlrcur,**rows;
	if (ZEND_NUM_ARGS() != 2 || 
		zend_get_parameters_ex(2,&sqlrcur,&rows) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(rows);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (cursor) {
		cursor->setResultSetBufferSize((*rows)->value.lval);
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getresultsetbuffersize) {
	zval **sqlrcur;
	uint64_t r;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcur) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (cursor) {
		r=cursor->getResultSetBufferSize();
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_dontgetcolumninfo) {
	zval **sqlrcur;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcur) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (cursor) {
		cursor->dontGetColumnInfo();
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getcolumninfo) {
	zval **sqlrcur;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcur) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (cursor) {
		cursor->getColumnInfo();
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcur_mixedcasecolumnnames) {
	zval **sqlrcur;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcur) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (cursor) {
		cursor->mixedCaseColumnNames();
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcur_uppercasecolumnnames) {
	zval **sqlrcur;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcur) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (cursor) {
		cursor->upperCaseColumnNames();
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcur_lowercasecolumnnames) {
	zval **sqlrcur;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcur) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (cursor) {
		cursor->lowerCaseColumnNames();
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcur_cachetofile) {
	zval **sqlrcur,**filename;
	if (ZEND_NUM_ARGS() != 2 || 
		zend_get_parameters_ex(2,&sqlrcur,&filename) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(filename);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (cursor) {
		cursor->cacheToFile((*filename)->value.str.val);
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcur_setcachettl) {
	zval **sqlrcur,**ttl;
	if (ZEND_NUM_ARGS() != 2 || 
		zend_get_parameters_ex(2,&sqlrcur,&ttl) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(ttl);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (cursor) {
		cursor->setCacheTtl((*ttl)->value.lval);
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getcachefilename) {
	zval **sqlrcur;
	const char *r;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcur) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (cursor) {
		r=cursor->getCacheFileName();
		if (r) {
			RETURN_STRING(const_cast<char *>(r),1);
		}
	}
	RETURN_FALSE;
}

DLEXPORT ZEND_FUNCTION(sqlrcur_cacheoff) {
	zval **sqlrcur;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcur) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (cursor) {
		cursor->cacheOff();
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getdatabaselist) {
	zval **sqlrcur,**wild;
	bool r;
	if (ZEND_NUM_ARGS() != 2 || 
		zend_get_parameters_ex(2,&sqlrcur,&wild) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(wild);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (cursor) {
		r=cursor->getDatabaseList((*wild)->value.str.val);
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_gettablelist) {
	zval **sqlrcur,**wild;
	bool r;
	if (ZEND_NUM_ARGS() != 2 || 
		zend_get_parameters_ex(2,&sqlrcur,&wild) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(wild);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (cursor) {
		r=cursor->getTableList((*wild)->value.str.val);
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getcolumnlist) {
	zval **sqlrcur,**table,**wild;
	bool r;
	if (ZEND_NUM_ARGS() != 3 || 
		zend_get_parameters_ex(3,&sqlrcur,&table,&wild) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(table);
	convert_to_string_ex(wild);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (cursor) {
		r=cursor->getColumnList((*table)->value.str.val,(*wild)->value.str.val);
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_sendquery) {
	zval **sqlrcur,**query;
	bool r;
	if (ZEND_NUM_ARGS() != 2 || 
		zend_get_parameters_ex(2,&sqlrcur,&query) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(query);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (cursor) {
		r=cursor->sendQuery((*query)->value.str.val);
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_sendquerywithlength) {
	zval **sqlrcur,**query,**length;
	bool r;
	if (ZEND_NUM_ARGS() != 3 || 
		zend_get_parameters_ex(3,&sqlrcur,&query,&length) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(query);
	convert_to_long_ex(length);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (cursor) {
		r=cursor->sendQuery((*query)->value.str.val,(*length)->value.lval);
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_sendfilequery) {
	zval **sqlrcur,**path,**filename;
	bool r;
	if (ZEND_NUM_ARGS() != 3 || 
		zend_get_parameters_ex(3,&sqlrcur,&path,&filename) 
					== FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(path);
	convert_to_string_ex(filename);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (cursor) {
		r=((sqlrcursor *)(*sqlrcur)->value.lval)->sendFileQuery((*path)->value.str.val,(*filename)->value.str.val);
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_preparequery) {
	zval **sqlrcur,**query;
	if (ZEND_NUM_ARGS() != 2 || 
		zend_get_parameters_ex(2,&sqlrcur,&query) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(query);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (cursor) {
		cursor->prepareQuery((*query)->value.str.val);
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcur_preparequerywithlength) {
	zval **sqlrcur,**query,**length;
	if (ZEND_NUM_ARGS() != 3 || 
		zend_get_parameters_ex(3,&sqlrcur,&query,&length) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(query);
	convert_to_long_ex(length);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (cursor) {
		cursor->prepareQuery((*query)->value.str.val,(*length)->value.lval);
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcur_preparefilequery) {
	zval **sqlrcur,**path,**filename;
	bool r;
	if (ZEND_NUM_ARGS() != 3 || 
		zend_get_parameters_ex(3,&sqlrcur,&path,&filename) 
					== FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(path);
	convert_to_string_ex(filename);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (cursor) {
		r=cursor->prepareFileQuery((*path)->value.str.val,(*filename)->value.str.val);
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_substitution) {
	zval **sqlrcur,**variable,**value,**precision,**scale;
	if (ZEND_NUM_ARGS() != 3 || 
		zend_get_parameters_ex(3,&sqlrcur,&variable,&value) 
					== FAILURE) {
		if (ZEND_NUM_ARGS() != 5 || 
			zend_get_parameters_ex(5,&sqlrcur,
				&variable,&value,&precision,&scale)== FAILURE) {
			WRONG_PARAM_COUNT;
		}
	}
	convert_to_string_ex(variable);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (cursor) {
		if (Z_TYPE_PP(value)==IS_STRING) {
			convert_to_string_ex(value);
			cursor->substitution((*variable)->value.str.val,(*value)->value.str.val);
			RETURN_LONG(1);
		} else if (Z_TYPE_PP(value)==IS_LONG) {
			convert_to_long_ex(value);
			cursor->substitution((*variable)->value.str.val,(*value)->value.lval);
			RETURN_LONG(1);
		} else if (ZEND_NUM_ARGS()==5 && Z_TYPE_PP(value)==IS_DOUBLE) {
			convert_to_double_ex(value);
			cursor->substitution((*variable)->value.str.val,(*value)->value.dval,(unsigned short)(*precision)->value.lval,(unsigned short)(*scale)->value.lval);
			RETURN_LONG(1);
		} else if (Z_TYPE_PP(value)==IS_NULL) {
			cursor->substitution((*variable)->value.str.val,(const char *)NULL);
			RETURN_LONG(1);
		}
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_clearbinds) {
	zval **sqlrcur;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcur) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (cursor) {
		cursor->clearBinds();
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcur_countbindvariables) {
	zval **sqlrcur;
	uint16_t r;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcur) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (cursor) {
		r=cursor->countBindVariables();
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_inputbind) {
	zval **sqlrcur,**variable,**value,**precision,**scale,**length;
	if (ZEND_NUM_ARGS() != 3 || 
		zend_get_parameters_ex(3,&sqlrcur,
				&variable,&value) == FAILURE) {
		if (ZEND_NUM_ARGS() != 4 || 
			zend_get_parameters_ex(4,&sqlrcur,
					&variable,&value,&length)== FAILURE) {
			if (ZEND_NUM_ARGS() != 5 || 
				zend_get_parameters_ex(5,&sqlrcur,
						&variable,&value,
						&precision,&scale)== FAILURE) {
				WRONG_PARAM_COUNT;
			}
		}
	}
	convert_to_string_ex(variable);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (cursor) {
		if (Z_TYPE_PP(value)==IS_STRING) {
			convert_to_string_ex(value);
			if (ZEND_NUM_ARGS() == 4 && (*length)->value.lval>0) {
				cursor->inputBind((*variable)->value.str.val,(*value)->value.str.val,(*length)->value.lval);
			} else {
				cursor->inputBind((*variable)->value.str.val,(*value)->value.str.val);
			}
			RETURN_LONG(1);
		} else if (Z_TYPE_PP(value)==IS_LONG) {
			convert_to_long_ex(value);
			cursor->inputBind((*variable)->value.str.val,(*value)->value.lval);
			RETURN_LONG(1);
		} else if (ZEND_NUM_ARGS()==5 && Z_TYPE_PP(value)==IS_DOUBLE) {
			convert_to_double_ex(value);
			cursor->inputBind((*variable)->value.str.val,(*value)->value.dval,(unsigned short)(*precision)->value.lval,(unsigned short)(*scale)->value.lval);
			RETURN_LONG(1);
		} else if (Z_TYPE_PP(value)==IS_NULL) {
			cursor->inputBind((*variable)->value.str.val,(const char *)NULL);
			RETURN_LONG(1);
		}
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_inputbindblob) {
	zval **sqlrcur,**variable,**value,**size;
	if (ZEND_NUM_ARGS() != 4 || 
		zend_get_parameters_ex(4,&sqlrcur,&variable,&value,&size) 
					== FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(variable);
	convert_to_string_ex(value);
	convert_to_long_ex(size);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (cursor) {
		cursor->inputBindBlob((*variable)->value.str.val,(*value)->value.str.val,(*size)->value.lval);
		RETURN_LONG(1);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_inputbindclob) {
	zval **sqlrcur,**variable,**value,**size;
	if (ZEND_NUM_ARGS() != 4 || 
		zend_get_parameters_ex(4,&sqlrcur,&variable,&value,&size) 
					== FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(variable);
	convert_to_string_ex(value);
	convert_to_long_ex(size);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (cursor) {
		cursor->inputBindClob((*variable)->value.str.val,(*value)->value.str.val,(*size)->value.lval);
		RETURN_LONG(1);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_defineoutputbindstring) {
	zval **sqlrcur,**variable,**length;
	if (ZEND_NUM_ARGS() != 3 || 
		zend_get_parameters_ex(3,&sqlrcur,&variable,&length) 
					== FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(variable);
	convert_to_long_ex(length);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (cursor) {
		cursor->defineOutputBindString((*variable)->value.str.val,(*length)->value.lval);
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcur_defineoutputbindinteger) {
	zval **sqlrcur,**variable;
	if (ZEND_NUM_ARGS() != 2 || 
		zend_get_parameters_ex(2,&sqlrcur,&variable) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(variable);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (cursor) {
		cursor->defineOutputBindInteger((*variable)->value.str.val);
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcur_defineoutputbinddouble) {
	zval **sqlrcur,**variable;
	if (ZEND_NUM_ARGS() != 2 || 
		zend_get_parameters_ex(2,&sqlrcur,&variable) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(variable);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (cursor) {
		cursor->defineOutputBindDouble((*variable)->value.str.val);
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcur_defineoutputbindblob) {
	zval **sqlrcur,**variable;
	if (ZEND_NUM_ARGS() != 2 || 
		zend_get_parameters_ex(2,&sqlrcur,&variable) 
					== FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(variable);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (cursor) {
		cursor->defineOutputBindBlob((*variable)->value.str.val);
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcur_defineoutputbindclob) {
	zval **sqlrcur,**variable;
	if (ZEND_NUM_ARGS() != 2 || 
		zend_get_parameters_ex(2,&sqlrcur,&variable) 
					== FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(variable);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (cursor) {
		cursor->defineOutputBindClob((*variable)->value.str.val);
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcur_defineoutputbindcursor) {
	zval **sqlrcur,**variable;
	if (ZEND_NUM_ARGS() != 2 || 
		zend_get_parameters_ex(2,&sqlrcur,&variable) 
					== FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(variable);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (cursor) {
		cursor->defineOutputBindCursor((*variable)->value.str.val);
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcur_substitutions) {
	zval **sqlrcur,**variables,**values,**precisions,**scales,**var,**val,**precision,**scale;
	unsigned int i;
	if (ZEND_NUM_ARGS() != 3 ||
		zend_get_parameters_ex(3,&sqlrcur,&variables,&values) 
					== FAILURE) {
		if (ZEND_NUM_ARGS() != 5 || 
			zend_get_parameters_ex(5,&sqlrcur,
				&variables,&values,
				&precisions,&scales)== FAILURE) {
			WRONG_PARAM_COUNT;
		} else {
			convert_to_array_ex(precisions);
			convert_to_array_ex(scales);
		}
	}
	convert_to_array_ex(variables);
	convert_to_array_ex(values);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	long	success=1;
	if (cursor) {
		for (i=0; i<(*variables)->value.ht->nNumOfElements; i++) {
			zend_hash_index_find((*variables)->value.ht,i,(void **)&var);
			zend_hash_index_find((*values)->value.ht,i,(void **)&val);
			if (Z_TYPE_PP(val)==IS_STRING) {
				convert_to_string_ex(val);
				cursor->substitution((*var)->value.str.val,(*val)->value.str.val);
			} else if (Z_TYPE_PP(val)==IS_LONG) {
				convert_to_long_ex(val);
				cursor->substitution((*var)->value.str.val,(*val)->value.lval);
			} else if (ZEND_NUM_ARGS()==5 && Z_TYPE_PP(val)==IS_DOUBLE) {
				zend_hash_index_find((*precisions)->value.ht,i,
								(void **)&precision);
				zend_hash_index_find((*scales)->value.ht,i,
								(void **)&scale);
				convert_to_double_ex(val);
				convert_to_long_ex(precision);
				convert_to_long_ex(scale);
				cursor->substitution((*var)->value.str.val,(*val)->value.dval,(unsigned short)(*precision)->value.lval,(unsigned short)(*scale)->value.lval);
			} else if (Z_TYPE_PP(val)==IS_NULL) {
				cursor->substitution((*var)->value.str.val,(const char *)NULL);
			} else {
				success=0;
			}
		}
	}
	RETURN_LONG(success);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_inputbinds) {
	zval **sqlrcur,**variables,**values,**precisions,**scales,**var,**val,**precision,**scale;
	int i;
	if (ZEND_NUM_ARGS() != 3 ||
		zend_get_parameters_ex(3,&sqlrcur,&variables,&values) 
					== FAILURE) {
		if (ZEND_NUM_ARGS() != 5 || 
			zend_get_parameters_ex(5,&sqlrcur,
				&variables,&values,
				&precisions,&scales)== FAILURE) {
			WRONG_PARAM_COUNT;
		} else {
			convert_to_array_ex(precisions);
			convert_to_array_ex(scales);
		}
	}
	convert_to_array_ex(variables);
	convert_to_array_ex(values);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	long	success=1;
	if (cursor) {
		for (i=0; i<zend_hash_num_elements((*variables)->value.ht); i++) {
			zend_hash_index_find((*variables)->value.ht,i,(void **)&var);
			zend_hash_index_find((*values)->value.ht,i,(void **)&val);
			if (Z_TYPE_PP(val)==IS_STRING) {
				convert_to_string_ex(val);
				cursor->inputBind((*var)->value.str.val,(*val)->value.str.val);
			} else if (Z_TYPE_PP(val)==IS_LONG) {
				convert_to_long_ex(val);
				cursor->inputBind((*var)->value.str.val,(*val)->value.lval);
			} else if (ZEND_NUM_ARGS()==5 && Z_TYPE_PP(val)==IS_DOUBLE) {
				zend_hash_index_find((*precisions)->value.ht,i,
								(void **)&precision);
				zend_hash_index_find((*scales)->value.ht,i,
								(void **)&scale);
				convert_to_long_ex(precision);
				convert_to_long_ex(scale);
				convert_to_double_ex(val);
				cursor->inputBind((*var)->value.str.val,(*val)->value.dval,(unsigned short)(*precision)->value.lval,(unsigned short)(*scale)->value.lval);
			} else if (Z_TYPE_PP(val)==IS_NULL) {
				cursor->inputBind((*var)->value.str.val,(const char *)NULL);
			} else {
				success=0;
			}
		}
	}
	RETURN_LONG(success);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_validatebinds) {
	zval **sqlrcur;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcur) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (cursor) {
		cursor->validateBinds();
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcur_validbind) {
	zval **sqlrcur,**variable;
	bool r;
	if (ZEND_NUM_ARGS() != 2 || 
		zend_get_parameters_ex(2,&sqlrcur,&variable) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(variable);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (cursor) {
		r=cursor->validBind((*variable)->value.str.val);
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_executequery) {
	zval **sqlrcur;
	bool r;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcur) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (cursor) {
		r=cursor->executeQuery();
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_fetchfrombindcursor) {
	zval **sqlrcur;
	bool r;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcur) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (cursor) {
		r=cursor->fetchFromBindCursor();
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getoutputbindstring) {
	zval **sqlrcur,**variable;
	const char *r;
	uint32_t rl;
	if (ZEND_NUM_ARGS() != 2 || 
		zend_get_parameters_ex(2,&sqlrcur,&variable) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(variable);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (cursor) {
		r=cursor->getOutputBindString((*variable)->value.str.val);
		rl=cursor->getOutputBindLength((*variable)->value.str.val);
		if (r) {
			RETURN_STRINGL(const_cast<char *>(r),rl,1);
		}
	}
	RETURN_NULL();
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getoutputbindblob) {
	zval **sqlrcur,**variable;
	const char *r;
	uint32_t rl;
	if (ZEND_NUM_ARGS() != 2 || 
		zend_get_parameters_ex(2,&sqlrcur,&variable) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(variable);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (cursor) {
		r=cursor->getOutputBindBlob((*variable)->value.str.val);
		rl=cursor->getOutputBindLength((*variable)->value.str.val);
		if (r) {
			RETURN_STRINGL(const_cast<char *>(r),rl,1);
		}
	}
	RETURN_NULL();
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getoutputbindclob) {
	zval **sqlrcur,**variable;
	const char *r;
	uint32_t rl;
	if (ZEND_NUM_ARGS() != 2 || 
		zend_get_parameters_ex(2,&sqlrcur,&variable) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(variable);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (cursor) {
		r=cursor->getOutputBindClob((*variable)->value.str.val);
		rl=cursor->getOutputBindLength((*variable)->value.str.val);
		if (r) {
			RETURN_STRINGL(const_cast<char *>(r),rl,1);
		}
	}
	RETURN_NULL();
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getoutputbindinteger) {
	zval **sqlrcur,**variable;
	int64_t r;
	if (ZEND_NUM_ARGS() != 2 || 
		zend_get_parameters_ex(2,&sqlrcur,&variable) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(variable);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (cursor) {
		r=cursor->getOutputBindInteger((*variable)->value.str.val);
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getoutputbinddouble) {
	zval **sqlrcur,**variable;
	double r;
	if (ZEND_NUM_ARGS() != 2 || 
		zend_get_parameters_ex(2,&sqlrcur,&variable) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(variable);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (cursor) {
		r=cursor->getOutputBindDouble((*variable)->value.str.val);
		RETURN_DOUBLE(r);
	}
	RETURN_DOUBLE(0.0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getoutputbindlength) {
	zval **sqlrcur,**variable;
	uint32_t r;
	if (ZEND_NUM_ARGS() != 2 || 
		zend_get_parameters_ex(2,&sqlrcur,&variable) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(variable);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (cursor) {
		r=cursor->getOutputBindLength((*variable)->value.str.val);
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getoutputbindcursor) {
	zval **sqlrcur,**variable;
	if (ZEND_NUM_ARGS() != 2 || 
		zend_get_parameters_ex(2,&sqlrcur,&variable) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(variable);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (!cursor) {
		RETURN_LONG(0);
	}
	sqlrcursor	*s=cursor->getOutputBindCursor((*variable)->value.str.val,true);
	ZEND_REGISTER_RESOURCE(return_value,s,sqlrelay_cursor);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_opencachedresultset) {
	zval **sqlrcur,**filename;
	bool r;
	if (ZEND_NUM_ARGS() != 2 || 
		zend_get_parameters_ex(2,&sqlrcur,&filename) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(filename);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (cursor) {
		r=cursor->openCachedResultSet((*filename)->value.str.val);
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_colcount) {
	zval **sqlrcur;
	uint32_t r;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcur) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (cursor) {
		r=cursor->colCount();
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_rowcount) {
	zval **sqlrcur;
	uint64_t r;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcur) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (cursor) {
		r=cursor->rowCount();
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_totalrows) {
	zval **sqlrcur;
	uint64_t r;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcur) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (cursor) {
		r=cursor->totalRows();
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_affectedrows) {
	zval **sqlrcur;
	uint64_t r;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcur) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (cursor) {
		r=cursor->affectedRows();
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_firstrowindex) {
	zval **sqlrcur;
	uint64_t r;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcur) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (cursor) {
		r=cursor->firstRowIndex();
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_endofresultset) {
	zval **sqlrcur;
	bool r;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcur) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (cursor) {
		r=cursor->endOfResultSet();
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_errormessage) {
	zval **sqlrcur;
	const char *r;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcur) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (cursor) {
		r=cursor->errorMessage();
		if (r) {
			RETURN_STRING(const_cast<char *>(r),1);
		}
	}
	RETURN_NULL();
}

DLEXPORT ZEND_FUNCTION(sqlrcur_errornumber) {
	zval **sqlrcur;
	int64_t r;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcur) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (cursor) {
		r=cursor->errorNumber();
		if (r) {
			RETURN_LONG(r);
		}
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getnullsasemptystrings) {
	zval **sqlrcur;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcur) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (cursor) {
		cursor->getNullsAsEmptyStrings();
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getnullsasnulls) {
	zval **sqlrcur;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcur) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (cursor) {
		cursor->getNullsAsNulls();
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getfield) {
	zval **sqlrcur,**row,**col;
	const char *r=NULL;
	uint32_t rl;
	if (ZEND_NUM_ARGS() != 3 || 
		zend_get_parameters_ex(3,&sqlrcur,&row,&col) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(row);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (cursor) {
		if (Z_TYPE_PP(col)==IS_LONG) {
			convert_to_long_ex(col);
			r=cursor->getField((*row)->value.lval,(*col)->value.lval);
			rl=cursor->getFieldLength((*row)->value.lval,(*col)->value.lval);
		} else if (Z_TYPE_PP(col)==IS_STRING) {
			convert_to_string_ex(col);
			r=cursor->getField((*row)->value.lval,(*col)->value.str.val);
			rl=cursor->getFieldLength((*row)->value.lval,(*col)->value.str.val);
		}
		if (r) {
			RETURN_STRINGL(const_cast<char *>(r),rl,1);
		}
	}
	RETURN_NULL();
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getfieldasinteger) {
	zval **sqlrcur,**row,**col;
	int64_t r=0;
	if (ZEND_NUM_ARGS() != 3 || 
		zend_get_parameters_ex(3,&sqlrcur,&row,&col) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(row);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (cursor) {
		if (Z_TYPE_PP(col)==IS_LONG) {
			convert_to_long_ex(col);
			r=cursor->getFieldAsInteger((*row)->value.lval,(*col)->value.lval);
		} else if (Z_TYPE_PP(col)==IS_STRING) {
			convert_to_string_ex(col);
			r=cursor->getFieldAsInteger((*row)->value.lval,(*col)->value.str.val);
		}
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getfieldasdouble) {
	zval **sqlrcur,**row,**col;
	double r=0.0;
	if (ZEND_NUM_ARGS() != 3 || 
		zend_get_parameters_ex(3,&sqlrcur,&row,&col) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(row);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (cursor) {
		if (Z_TYPE_PP(col)==IS_LONG) {
			convert_to_long_ex(col);
			r=cursor->getFieldAsDouble((*row)->value.lval,(*col)->value.lval);
		} else if (Z_TYPE_PP(col)==IS_STRING) {
			convert_to_string_ex(col);
			r=cursor->getFieldAsDouble((*row)->value.lval,(*col)->value.str.val);
		}
		RETURN_DOUBLE(r);
	}
	RETURN_DOUBLE(0.0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getfieldlength) {
	zval **sqlrcur,**row,**col;
	uint32_t r=0;
	if (ZEND_NUM_ARGS() != 3 || 
		zend_get_parameters_ex(3,&sqlrcur,&row,&col) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(row);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (cursor) {
		if (Z_TYPE_PP(col)==IS_LONG) {
			convert_to_long_ex(col);
			r=cursor->getFieldLength((*row)->value.lval,(*col)->value.lval);
		} else if (Z_TYPE_PP(col)==IS_STRING) {
			convert_to_string_ex(col);
			r=cursor->getFieldLength((*row)->value.lval,(*col)->value.str.val);
		}
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getrow) {
	zval **sqlrcur,**row;
	const char * const *r;
	uint32_t *l;
	uint32_t i;
	if (ZEND_NUM_ARGS() != 2 || 
		zend_get_parameters_ex(2,&sqlrcur,&row) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(row);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (!cursor) {
		RETURN_FALSE;
	}
	r=cursor->getRow((*row)->value.lval);
	l=cursor->getRowLengths((*row)->value.lval);
	if (!r) {
		RETURN_FALSE;
	}
	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}
	for (i=0; i<cursor->colCount(); i++) {
		if (!r[i]) {
			// using add_next_index_unset because add_assoc_null
			// isn't defined in older php
			add_next_index_unset(return_value);
		} else {
			add_next_index_stringl(return_value,const_cast<char *>(r[i]),l[i],1);
		}
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getrowassoc) {
	zval **sqlrcur,**row;
	const char * const *r;
	uint32_t *l;
	const char * const *rC;
	uint32_t i;
	if (ZEND_NUM_ARGS() != 2 || 
		zend_get_parameters_ex(2,&sqlrcur,&row) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(row);

	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (!cursor) {
		RETURN_FALSE;
	}

	rC=cursor->getColumnNames();
	if (!rC) {
		RETURN_FALSE;
	}

	r=cursor->getRow((*row)->value.lval);
	l=cursor->getRowLengths((*row)->value.lval);
	if (!r) {
		RETURN_FALSE;
	}
	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}
	for (i=0; i<cursor->colCount(); i++) {
		if (!r[i]) {
			// using add_assoc_unset because add_assoc_null isn't
			// defined in older php
			add_assoc_unset(return_value,const_cast<char *>(rC[i]));
		} else {
			add_assoc_stringl(return_value,const_cast<char *>(rC[i]),const_cast<char *>(r[i]),l[i],1);
		}
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getrowlengths) {
	zval **sqlrcur,**row;
	uint32_t *r;
	uint32_t i;
	if (ZEND_NUM_ARGS() != 2 || 
		zend_get_parameters_ex(2,&sqlrcur,&row) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(row);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (!cursor) {
		RETURN_FALSE;
	}
	r=cursor->getRowLengths((*row)->value.lval);
	if (!r) {
		RETURN_FALSE;
	}
	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}
	for (i=0; i<cursor->colCount(); i++) {
		add_next_index_long(return_value,r[i]);
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getrowlengthsassoc) {
	zval **sqlrcur,**row;
	uint32_t *r;
	const char * const *rC;
	uint32_t i;
	if (ZEND_NUM_ARGS() != 2 || 
		zend_get_parameters_ex(2,&sqlrcur,&row) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(row);

	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (!cursor) {
		RETURN_FALSE;
	}

	rC=cursor->getColumnNames();
	if (!rC) {
		RETURN_FALSE;
	}

	r=cursor->getRowLengths((*row)->value.lval);
	if (!r) {
		RETURN_FALSE;
	}
	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}
	for (i=0; i<cursor->colCount(); i++) {
		add_assoc_long(return_value,const_cast<char *>(rC[i]),r[i]);
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getcolumnnames) {
	zval **sqlrcur;
	const char * const *r;
	uint32_t i;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcur) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (!cursor) {
		RETURN_FALSE;
	}
	r=cursor->getColumnNames();
	if (!r) {
		RETURN_FALSE;
	}
	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}
	for (i=0; i<cursor->colCount(); i++) {
		add_next_index_string(return_value,const_cast<char *>(r[i]),1);
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getcolumnname) {
	zval **sqlrcur,**col;
	const char *r;
	if (ZEND_NUM_ARGS() != 2 || 
		zend_get_parameters_ex(2,&sqlrcur,&col) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(col);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (cursor) {
		r=cursor->getColumnName((*col)->value.lval);
		if (r) {
			RETURN_STRING(const_cast<char *>(r),1);
		}
	}
	RETURN_FALSE;
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getcolumntype) {
	zval **sqlrcur,**col;
	const char *r=NULL;
	if (ZEND_NUM_ARGS() != 2 || 
		zend_get_parameters_ex(2,&sqlrcur,&col) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (cursor) {
		if (Z_TYPE_PP(col)==IS_LONG) {
			convert_to_long_ex(col);
			r=cursor->getColumnType((*col)->value.lval);
		} else if (Z_TYPE_PP(col)==IS_STRING) {
			convert_to_string_ex(col);
			r=cursor->getColumnType((*col)->value.str.val);
		}
		if (r) {
			RETURN_STRING(const_cast<char *>(r),1);
		}
	}
	RETURN_FALSE;
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getcolumnlength) {
	zval **sqlrcur,**col;
	uint32_t r=0;
	if (ZEND_NUM_ARGS() != 2 || 
		zend_get_parameters_ex(2,&sqlrcur,&col) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (cursor) {
		if (Z_TYPE_PP(col)==IS_LONG) {
			convert_to_long_ex(col);
			r=cursor->getColumnLength((*col)->value.lval);
		} else if (Z_TYPE_PP(col)==IS_STRING) {
			convert_to_string_ex(col);
			r=cursor->getColumnLength((*col)->value.str.val);
		}
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getcolumnprecision) {
	zval **sqlrcur,**col;
	uint32_t r=0;
	if (ZEND_NUM_ARGS() != 2 || 
		zend_get_parameters_ex(2,&sqlrcur,&col) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (cursor) {
		if (Z_TYPE_PP(col)==IS_LONG) {
			convert_to_long_ex(col);
			r=cursor->getColumnPrecision((*col)->value.lval);
		} else if (Z_TYPE_PP(col)==IS_STRING) {
			convert_to_string_ex(col);
			r=cursor->getColumnPrecision((*col)->value.str.val);
		}
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getcolumnscale) {
	zval **sqlrcur,**col;
	uint32_t r=0;
	if (ZEND_NUM_ARGS() != 2 || 
		zend_get_parameters_ex(2,&sqlrcur,&col) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (cursor) {
		if (Z_TYPE_PP(col)==IS_LONG) {
			convert_to_long_ex(col);
			r=cursor->getColumnScale((*col)->value.lval);
		} else if (Z_TYPE_PP(col)==IS_STRING) {
			convert_to_string_ex(col);
			r=cursor->getColumnScale((*col)->value.str.val);
		}
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getcolumnisnullable) {
	zval **sqlrcur,**col;
	bool r=0;
	if (ZEND_NUM_ARGS() != 2 || 
		zend_get_parameters_ex(2,&sqlrcur,&col) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (cursor) {
		if (Z_TYPE_PP(col)==IS_LONG) {
			convert_to_long_ex(col);
			r=cursor->getColumnIsNullable((*col)->value.lval);
		} else if (Z_TYPE_PP(col)==IS_STRING) {
			convert_to_string_ex(col);
			r=cursor->getColumnIsNullable((*col)->value.str.val);
		}
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getcolumnisprimarykey) {
	zval **sqlrcur,**col;
	bool r=0;
	if (ZEND_NUM_ARGS() != 2 || 
		zend_get_parameters_ex(2,&sqlrcur,&col) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (cursor) {
		if (Z_TYPE_PP(col)==IS_LONG) {
			convert_to_long_ex(col);
			r=cursor->getColumnIsPrimaryKey((*col)->value.lval);
		} else if (Z_TYPE_PP(col)==IS_STRING) {
			convert_to_string_ex(col);
			r=cursor->getColumnIsPrimaryKey((*col)->value.str.val);
		}
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getcolumnisunique) {
	zval **sqlrcur,**col;
	bool r=0;
	if (ZEND_NUM_ARGS() != 2 || 
		zend_get_parameters_ex(2,&sqlrcur,&col) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (cursor) {
		if (Z_TYPE_PP(col)==IS_LONG) {
			convert_to_long_ex(col);
			r=cursor->getColumnIsUnique((*col)->value.lval);
		} else if (Z_TYPE_PP(col)==IS_STRING) {
			convert_to_string_ex(col);
			r=cursor->getColumnIsUnique((*col)->value.str.val);
		}
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getcolumnispartofkey) {
	zval **sqlrcur,**col;
	bool r=0;
	if (ZEND_NUM_ARGS() != 2 || 
		zend_get_parameters_ex(2,&sqlrcur,&col) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (cursor) {
		if (Z_TYPE_PP(col)==IS_LONG) {
			convert_to_long_ex(col);
			r=cursor->getColumnIsPartOfKey((*col)->value.lval);
		} else if (Z_TYPE_PP(col)==IS_STRING) {
			convert_to_string_ex(col);
			r=cursor->getColumnIsPartOfKey((*col)->value.str.val);
		}
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getcolumnisunsigned) {
	zval **sqlrcur,**col;
	bool r=0;
	if (ZEND_NUM_ARGS() != 2 || 
		zend_get_parameters_ex(2,&sqlrcur,&col) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (cursor) {
		if (Z_TYPE_PP(col)==IS_LONG) {
			convert_to_long_ex(col);
			r=cursor->getColumnIsUnsigned((*col)->value.lval);
		} else if (Z_TYPE_PP(col)==IS_STRING) {
			convert_to_string_ex(col);
			r=cursor->getColumnIsUnsigned((*col)->value.str.val);
		}
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getcolumniszerofilled) {
	zval **sqlrcur,**col;
	bool r=0;
	if (ZEND_NUM_ARGS() != 2 || 
		zend_get_parameters_ex(2,&sqlrcur,&col) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (cursor) {
		if (Z_TYPE_PP(col)==IS_LONG) {
			convert_to_long_ex(col);
			r=cursor->getColumnIsZeroFilled((*col)->value.lval);
		} else if (Z_TYPE_PP(col)==IS_STRING) {
			convert_to_string_ex(col);
			r=cursor->getColumnIsZeroFilled((*col)->value.str.val);
		}
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getcolumnisbinary) {
	zval **sqlrcur,**col;
	bool r=0;
	if (ZEND_NUM_ARGS() != 2 || 
		zend_get_parameters_ex(2,&sqlrcur,&col) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (cursor) {
		if (Z_TYPE_PP(col)==IS_LONG) {
			convert_to_long_ex(col);
			r=cursor->getColumnIsBinary((*col)->value.lval);
		} else if (Z_TYPE_PP(col)==IS_STRING) {
			convert_to_string_ex(col);
			r=cursor->getColumnIsBinary((*col)->value.str.val);
		}
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getcolumnisautoincrement) {
	zval **sqlrcur,**col;
	bool r=0;
	if (ZEND_NUM_ARGS() != 2 || 
		zend_get_parameters_ex(2,&sqlrcur,&col) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (cursor) {
		if (Z_TYPE_PP(col)==IS_LONG) {
			convert_to_long_ex(col);
			r=cursor->getColumnIsAutoIncrement((*col)->value.lval);
		} else if (Z_TYPE_PP(col)==IS_STRING) {
			convert_to_string_ex(col);
			r=cursor->getColumnIsAutoIncrement((*col)->value.str.val);
		}
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getlongest) {
	zval **sqlrcur,**col;
	uint32_t r=0;
	if (ZEND_NUM_ARGS() != 2 || 
		zend_get_parameters_ex(2,&sqlrcur,&col) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (cursor) {
		if (Z_TYPE_PP(col)==IS_LONG) {
			convert_to_long_ex(col);
			r=cursor->getLongest((*col)->value.lval);
		} else if (Z_TYPE_PP(col)==IS_STRING) {
			convert_to_string_ex(col);
			r=cursor->getLongest((*col)->value.str.val);
		}
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getresultsetid) {
	zval **sqlrcur;
	uint16_t r;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcur) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (cursor) {
		r=cursor->getResultSetId();
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_suspendresultset) {
	zval **sqlrcur;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcur) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (cursor) {
		cursor->suspendResultSet();
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcur_resumeresultset) {
	zval **sqlrcur, **id;
	bool r;
	if (ZEND_NUM_ARGS() != 2 || 
		zend_get_parameters_ex(2,&sqlrcur,&id) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(id);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (cursor) {
		r=cursor->resumeResultSet((*id)->value.lval);
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_resumecachedresultset) {
	zval **sqlrcur, **id, **filename;
	bool r;
	if (ZEND_NUM_ARGS() != 3 || 
		zend_get_parameters_ex(3,&sqlrcur,&id,&filename) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(id);
	convert_to_string_ex(filename);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (cursor) {
		r=cursor->resumeCachedResultSet((*id)->value.lval,(*filename)->value.str.val);
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_closeresultset) {
	zval **sqlrcur;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcur) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,sqlrcursor *,sqlrcur,-1,"sqlrelay cursor",sqlrelay_cursor);
	if (cursor) {
		cursor->closeResultSet();
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcon_ping) {
	zval **sqlrcon;
	bool r;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcon) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrconnection *connection=NULL;
	ZEND_FETCH_RESOURCE(connection,sqlrconnection *,sqlrcon,-1,"sqlrelay connection",sqlrelay_connection);
	if (connection) {
		r=connection->ping();
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcon_selectdatabase) {
	zval **sqlrcon,**database;
	bool r;
	if (ZEND_NUM_ARGS() != 2 || 
		zend_get_parameters_ex(2,&sqlrcon,&database) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(database);
	sqlrconnection *connection=NULL;
	ZEND_FETCH_RESOURCE(connection,sqlrconnection *,sqlrcon,-1,"sqlrelay connection",sqlrelay_connection);
	if (connection) {
		r=connection->selectDatabase((*database)->value.str.val);
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcon_getcurrentdatabase) {
	zval **sqlrcon;
	const char *r;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcon) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrconnection *connection=NULL;
	ZEND_FETCH_RESOURCE(connection,sqlrconnection *,sqlrcon,-1,"sqlrelay connection",sqlrelay_connection);
	if (connection) {
		r=connection->getCurrentDatabase();
		if (r) {
			RETURN_STRING(const_cast<char *>(r),1);
		}
	}
	RETURN_FALSE;
}

DLEXPORT ZEND_FUNCTION(sqlrcon_getlastinsertid) {
	zval **sqlrcon;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcon) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrconnection *connection=NULL;
	ZEND_FETCH_RESOURCE(connection,sqlrconnection *,sqlrcon,-1,"sqlrelay connection",sqlrelay_connection);
	if (connection) {
		RETURN_LONG(connection->getLastInsertId());
	}
	RETURN_FALSE;
}

DLEXPORT ZEND_FUNCTION(sqlrcon_autocommiton) {
	zval **sqlrcon;
	bool r;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcon) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrconnection *connection=NULL;
	ZEND_FETCH_RESOURCE(connection,sqlrconnection *,sqlrcon,-1,"sqlrelay connection",sqlrelay_connection);
	if (connection) {
		r=connection->autoCommitOn();
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcon_autocommitoff) {
	zval **sqlrcon;
	bool r;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcon) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrconnection *connection=NULL;
	ZEND_FETCH_RESOURCE(connection,sqlrconnection *,sqlrcon,-1,"sqlrelay connection",sqlrelay_connection);
	if (connection) {
		r=connection->autoCommitOff();
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcon_begin) {
	zval **sqlrcon;
	bool r;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcon) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrconnection *connection=NULL;
	ZEND_FETCH_RESOURCE(connection,sqlrconnection *,sqlrcon,-1,"sqlrelay connection",sqlrelay_connection);
	if (connection) {
		r=connection->begin();
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcon_commit) {
	zval **sqlrcon;
	bool r;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcon) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrconnection *connection=NULL;
	ZEND_FETCH_RESOURCE(connection,sqlrconnection *,sqlrcon,-1,"sqlrelay connection",sqlrelay_connection);
	if (connection) {
		r=connection->commit();
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcon_rollback) {
	zval **sqlrcon;
	bool r;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcon) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrconnection *connection=NULL;
	ZEND_FETCH_RESOURCE(connection,sqlrconnection *,sqlrcon,-1,"sqlrelay connection",sqlrelay_connection);
	if (connection) {
		r=connection->rollback();
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcon_identify) {
	zval **sqlrcon;
	const char *r;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcon) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrconnection *connection=NULL;
	ZEND_FETCH_RESOURCE(connection,sqlrconnection *,sqlrcon,-1,"sqlrelay connection",sqlrelay_connection);
	if (connection) {
		r=connection->identify();
		if (r) {
			RETURN_STRING(const_cast<char *>(r),1);
		}
	}
	RETURN_FALSE;
}

DLEXPORT ZEND_FUNCTION(sqlrcon_bindformat) {
	zval **sqlrcon;
	const char *r;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcon) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrconnection *connection=NULL;
	ZEND_FETCH_RESOURCE(connection,sqlrconnection *,sqlrcon,-1,"sqlrelay connection",sqlrelay_connection);
	if (connection) {
		r=connection->bindFormat();
		if (r) {
			RETURN_STRING(const_cast<char *>(r),1);
		}
	}
	RETURN_FALSE;
}

DLEXPORT ZEND_FUNCTION(sqlrcon_dbversion) {
	zval **sqlrcon;
	const char *r;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcon) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrconnection *connection=NULL;
	ZEND_FETCH_RESOURCE(connection,sqlrconnection *,sqlrcon,-1,"sqlrelay connection",sqlrelay_connection);
	if (connection) {
		r=connection->dbVersion();
		if (r) {
			RETURN_STRING(const_cast<char *>(r),1);
		}
	}
	RETURN_FALSE;
}


DLEXPORT ZEND_FUNCTION(sqlrcon_dbhostname) {
	zval **sqlrcon;
	const char *r;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcon) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrconnection *connection=NULL;
	ZEND_FETCH_RESOURCE(connection,sqlrconnection *,sqlrcon,-1,"sqlrelay connection",sqlrelay_connection);
	if (connection) {
		r=connection->dbHostName();
		if (r) {
			RETURN_STRING(const_cast<char *>(r),1);
		}
	}
	RETURN_FALSE;
}


DLEXPORT ZEND_FUNCTION(sqlrcon_dbipaddress) {
	zval **sqlrcon;
	const char *r;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcon) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrconnection *connection=NULL;
	ZEND_FETCH_RESOURCE(connection,sqlrconnection *,sqlrcon,-1,"sqlrelay connection",sqlrelay_connection);
	if (connection) {
		r=connection->dbIpAddress();
		if (r) {
			RETURN_STRING(const_cast<char *>(r),1);
		}
	}
	RETURN_FALSE;
}

DLEXPORT ZEND_FUNCTION(sqlrcon_serverversion) {
	zval **sqlrcon;
	const char *r;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcon) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrconnection *connection=NULL;
	ZEND_FETCH_RESOURCE(connection,sqlrconnection *,sqlrcon,-1,"sqlrelay connection",sqlrelay_connection);
	if (connection) {
		r=connection->serverVersion();
		if (r) {
			RETURN_STRING(const_cast<char *>(r),1);
		}
	}
	RETURN_FALSE;
}

DLEXPORT ZEND_FUNCTION(sqlrcon_clientversion) {
	zval **sqlrcon;
	const char *r;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcon) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrconnection *connection=NULL;
	ZEND_FETCH_RESOURCE(connection,sqlrconnection *,sqlrcon,-1,"sqlrelay connection",sqlrelay_connection);
	if (connection) {
		r=connection->clientVersion();
		if (r) {
			RETURN_STRING(const_cast<char *>(r),1);
		}
	}
	RETURN_FALSE;
}

zend_function_entry sql_relay_functions[] = {
	ZEND_FE(sqlrcon_alloc,NULL)
	ZEND_FE(sqlrcon_free,NULL)
	ZEND_FE(sqlrcon_setconnecttimeout,NULL)
	ZEND_FE(sqlrcon_setauthenticationtimeout,NULL)
	ZEND_FE(sqlrcon_setresponsetimeout,NULL)
	ZEND_FE(sqlrcon_endsession,NULL)
	ZEND_FE(sqlrcon_suspendsession,NULL)
	ZEND_FE(sqlrcon_getconnectionport,NULL)
	ZEND_FE(sqlrcon_getconnectionsocket,NULL)
	ZEND_FE(sqlrcon_resumesession,NULL)
	ZEND_FE(sqlrcon_errormessage,NULL)
	ZEND_FE(sqlrcon_errornumber,NULL)
	ZEND_FE(sqlrcon_debugon,NULL)
	ZEND_FE(sqlrcon_debugoff,NULL)
	ZEND_FE(sqlrcon_getdebug,NULL)
	ZEND_FE(sqlrcon_setdebugfile,NULL)
	ZEND_FE(sqlrcon_setclientinfo,NULL)
	ZEND_FE(sqlrcon_getclientinfo,NULL)
	ZEND_FE(sqlrcur_alloc,NULL)
	ZEND_FE(sqlrcur_free,NULL)
	ZEND_FE(sqlrcur_setresultsetbuffersize,NULL)
	ZEND_FE(sqlrcur_getresultsetbuffersize,NULL)
	ZEND_FE(sqlrcur_dontgetcolumninfo,NULL)
	ZEND_FE(sqlrcur_getcolumninfo,NULL)
	ZEND_FE(sqlrcur_mixedcasecolumnnames,NULL)
	ZEND_FE(sqlrcur_uppercasecolumnnames,NULL)
	ZEND_FE(sqlrcur_lowercasecolumnnames,NULL)
	ZEND_FE(sqlrcur_cachetofile,NULL)
	ZEND_FE(sqlrcur_setcachettl,NULL)
	ZEND_FE(sqlrcur_getcachefilename,NULL)
	ZEND_FE(sqlrcur_cacheoff,NULL)
	ZEND_FE(sqlrcur_getdatabaselist,NULL)
	ZEND_FE(sqlrcur_gettablelist,NULL)
	ZEND_FE(sqlrcur_getcolumnlist,NULL)
	ZEND_FE(sqlrcur_sendquery,NULL)
	ZEND_FE(sqlrcur_sendquerywithlength,NULL)
	ZEND_FE(sqlrcur_sendfilequery,NULL)
	ZEND_FE(sqlrcur_preparequery,NULL)
	ZEND_FE(sqlrcur_preparequerywithlength,NULL)
	ZEND_FE(sqlrcur_preparefilequery,NULL)
	ZEND_FE(sqlrcur_substitution,NULL)
	ZEND_FE(sqlrcur_clearbinds,NULL)
	ZEND_FE(sqlrcur_countbindvariables,NULL)
	ZEND_FE(sqlrcur_inputbind,NULL)
	ZEND_FE(sqlrcur_inputbindblob,NULL)
	ZEND_FE(sqlrcur_inputbindclob,NULL)
	ZEND_FE(sqlrcur_defineoutputbindstring,NULL)
	ZEND_FE(sqlrcur_defineoutputbindinteger,NULL)
	ZEND_FE(sqlrcur_defineoutputbinddouble,NULL)
	ZEND_FE(sqlrcur_defineoutputbindblob,NULL)
	ZEND_FE(sqlrcur_defineoutputbindclob,NULL)
	ZEND_FE(sqlrcur_defineoutputbindcursor,NULL)
	ZEND_FE(sqlrcur_substitutions,NULL)
	ZEND_FE(sqlrcur_inputbinds,NULL)
	ZEND_FE(sqlrcur_validatebinds,NULL)
	ZEND_FE(sqlrcur_validbind,NULL)
	ZEND_FE(sqlrcur_executequery,NULL)
	ZEND_FE(sqlrcur_fetchfrombindcursor,NULL)
	ZEND_FE(sqlrcur_getoutputbindstring,NULL)
	ZEND_FE(sqlrcur_getoutputbindblob,NULL)
	ZEND_FE(sqlrcur_getoutputbindclob,NULL)
	ZEND_FE(sqlrcur_getoutputbindinteger,NULL)
	ZEND_FE(sqlrcur_getoutputbinddouble,NULL)
	ZEND_FE(sqlrcur_getoutputbindlength,NULL)
	ZEND_FE(sqlrcur_getoutputbindcursor,NULL)
	ZEND_FE(sqlrcur_opencachedresultset,NULL)
	ZEND_FE(sqlrcur_colcount,NULL)
	ZEND_FE(sqlrcur_rowcount,NULL)
	ZEND_FE(sqlrcur_totalrows,NULL)
	ZEND_FE(sqlrcur_affectedrows,NULL)
	ZEND_FE(sqlrcur_firstrowindex,NULL)
	ZEND_FE(sqlrcur_endofresultset,NULL)
	ZEND_FE(sqlrcur_errormessage,NULL)
	ZEND_FE(sqlrcur_errornumber,NULL)
	ZEND_FE(sqlrcur_getnullsasemptystrings,NULL)
	ZEND_FE(sqlrcur_getnullsasnulls,NULL)
	ZEND_FE(sqlrcur_getfield,NULL)
	ZEND_FE(sqlrcur_getfieldasinteger,NULL)
	ZEND_FE(sqlrcur_getfieldasdouble,NULL)
	ZEND_FE(sqlrcur_getfieldlength,NULL)
	ZEND_FE(sqlrcur_getrow,NULL)
	ZEND_FE(sqlrcur_getrowassoc,NULL)
	ZEND_FE(sqlrcur_getrowlengths,NULL)
	ZEND_FE(sqlrcur_getrowlengthsassoc,NULL)
	ZEND_FE(sqlrcur_getcolumnnames,NULL)
	ZEND_FE(sqlrcur_getcolumnname,NULL)
	ZEND_FE(sqlrcur_getcolumntype,NULL)
	ZEND_FE(sqlrcur_getcolumnlength,NULL)
	ZEND_FE(sqlrcur_getcolumnprecision,NULL)
	ZEND_FE(sqlrcur_getcolumnscale,NULL)
	ZEND_FE(sqlrcur_getcolumnisnullable,NULL)
	ZEND_FE(sqlrcur_getcolumnisprimarykey,NULL)
	ZEND_FE(sqlrcur_getcolumnisunique,NULL)
	ZEND_FE(sqlrcur_getcolumnispartofkey,NULL)
	ZEND_FE(sqlrcur_getcolumnisunsigned,NULL)
	ZEND_FE(sqlrcur_getcolumniszerofilled,NULL)
	ZEND_FE(sqlrcur_getcolumnisbinary,NULL)
	ZEND_FE(sqlrcur_getcolumnisautoincrement,NULL)
	ZEND_FE(sqlrcur_getlongest,NULL)
	ZEND_FE(sqlrcur_getresultsetid,NULL)
	ZEND_FE(sqlrcur_suspendresultset,NULL)
	ZEND_FE(sqlrcur_resumeresultset,NULL)
	ZEND_FE(sqlrcur_resumecachedresultset,NULL)
	ZEND_FE(sqlrcon_ping,NULL)
	ZEND_FE(sqlrcon_identify,NULL)
	ZEND_FE(sqlrcon_selectdatabase,NULL)
	ZEND_FE(sqlrcon_getcurrentdatabase,NULL)
	ZEND_FE(sqlrcon_getlastinsertid,NULL)
	ZEND_FE(sqlrcon_autocommiton,NULL)
	ZEND_FE(sqlrcon_autocommitoff,NULL)
	ZEND_FE(sqlrcon_begin,NULL)
	ZEND_FE(sqlrcon_commit,NULL)
	ZEND_FE(sqlrcon_rollback,NULL)
	ZEND_FE(sqlrcon_bindformat,NULL)
	ZEND_FE(sqlrcon_dbversion,NULL)
	ZEND_FE(sqlrcon_dbhostname,NULL)
	ZEND_FE(sqlrcon_dbipaddress,NULL)
	ZEND_FE(sqlrcon_serverversion,NULL)
	ZEND_FE(sqlrcon_clientversion,NULL)
	{NULL,NULL,NULL}
};

zend_module_entry sql_relay_module_entry = {
	#if ZEND_MODULE_API_NO >= 20010901
		STANDARD_MODULE_HEADER,
	#endif	
	"sql_relay",
	sql_relay_functions,
	// extension-wide startup function
#ifdef ZEND_MODULE_STARTUP_N
	ZEND_MODULE_STARTUP_N(sqlrelay),
#else
	NULL,
#endif
	// extension-wide shutdown function
	NULL,
	// per-request startup function
	NULL,
	// per-request shutdown function
	NULL,
	NULL,
	#if ZEND_MODULE_API_NO >= 20010901
		SQLR_VERSION,
	#endif	
	STANDARD_MODULE_PROPERTIES
};

DLEXPORT ZEND_GET_MODULE(sql_relay)

}
