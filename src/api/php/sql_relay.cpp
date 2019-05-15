/* Copyright (c) 2000  Adam Kropielnicki
   See the file COPYING for more information */

// Some versions of PHP 7.0 need INT64_MIN/INT64_MAX to be defined but
// these gyrations are necessary when using C++.
#include <rudiments/private/config.h>
#ifdef RUDIMENTS_HAVE_STDINT_H
	#define __STDC_LIMIT_MACROS
	#include <stdint.h>
#endif

#include <sqlrelay/sqlrclient.h>

// The various define/undef games below play havoc with inttypes.h
// on some platforms (openbsd 5.7, for example).  Including it here
// prevents it from being included later after the games.
// We'll borrow a macro from rudiments to detect it's existence.
#ifdef RUDIMENTS_HAVE_INTTYPES_H
	#include <inttypes.h>
#endif

#ifdef WIN32
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
	#ifndef WIN32

		// On some platforms (solaris), stdio.h must be included prior
		// to including math.h or FILE will get redefined.
		#include <stdio.h>

		// php.h ultimately includes math.h and on some platforms,
		// __cplusplus to be defined when including it.  Manually
		// including it prior to including php.h solves this problem.
		#include <math.h>

		#ifdef __cplusplus
			#undef __cplusplus
			#define cpluspluswasdefined
		#endif
		#ifndef HAVE_SOCKLEN_T
			#define HAVE_SOCKLEN_T
		#endif
		#ifndef _WCHAR_T_DECLARED
			#define _WCHAR_T_DECLARED
		#endif
		#ifndef _WCHAR_T_DEFINED_
			#define _WCHAR_T_DEFINED_
		#endif
	#endif
	#include <php.h>
	#ifndef WIN32
		#ifdef cpluspluswasdefined
			#define __cplusplus
		#endif
	#endif
}

#include <config.h>

#if PHP_MAJOR_VERSION >= 7

	#define ZVAL zval*

	#define zend_rsrc_list_entry zend_resource
	#define ZEND_REGISTER_RESOURCE(a,b,c) \
			RETURN_RES(zend_register_resource(b,c))
	#define ZEND_FETCH_RESOURCE(a,b,c,d,e,f) \
		if ((a=(b)zend_fetch_resource( \
			Z_RES_P((zval *)c),e,f))==NULL) { \
			RETURN_FALSE; \
		}
	#define ZEND_LIST_DELETE(a) zend_list_delete(Z_RES_P(a));

	#define GET_PARAMETERS zend_parse_parameters
	#define PARAMS(a) a,

	#define SVAL(a) Z_STRVAL_P(a)
	#define LVAL(a) Z_LVAL_P(a)
	#define DVAL(a) Z_DVAL_P(a)
	#define ARRVAL(a) Z_ARRVAL_P(a)
	#define TYPE(a) Z_TYPE_P(a)

	#define RET_STRING(a,b) \
		RETURN_STR(zend_string_init(a,charstring::length(a),0))
	#define RET_STRINGL(a,b,c) \
		RETURN_STR(zend_string_init(a,b,0))


	#define ADD_ASSOC_STRINGL(a,b,c,d,e) \
		add_assoc_stringl(a,b,zend_string_init(c,d,0)->val,d)
	#define ADD_NEXT_INDEX_STRING(a,b,c) \
		add_next_index_string( \
			a,zend_string_init(b,charstring::length(b),0)->val)
	#define ADD_NEXT_INDEX_STRINGL(a,b,c,d) \
		add_next_index_stringl(a,zend_string_init(b,c,0)->val,c)

	#define HASH_INDEX_FIND(a,b,c) c=zend_hash_index_find(a,b)

	#if PHP_MAJOR_VERSION > 7 || PHP_MINOR_VERSION > 2
		#define ARRAY_INIT_CANT_FAIL 1
	#endif

#else

	#define ZVAL zval**

	#define ZEND_LIST_DELETE(a) zend_list_delete(LVAL(a));

	#define GET_PARAMETERS zend_get_parameters_ex
	#define PARAMS(a)

	// apparently, sufficiently old PHP doesn't support Z_*VAL(a)...
	#define SVAL(a) (*a)->value.str.val
	#define LVAL(a) (*a)->value.lval
	#define DVAL(a) (*a)->value.dval
	#define ARRVAL(a) (*a)->value.ht
	#define TYPE(a) Z_TYPE_PP(a)

	#define RET_STRING RETURN_STRING
	#define RET_STRINGL RETURN_STRINGL

	#define ADD_ASSOC_STRINGL(a,b,c,d,e) add_assoc_stringl(a,b,c,d,e)
	#define ADD_NEXT_INDEX_STRING(a,b,c) add_next_index_string(a,b,c)
	#define ADD_NEXT_INDEX_STRINGL(a,b,c,d) add_next_index_stringl(a,b,c,d)

	#define HASH_INDEX_FIND(a,b,c) zend_hash_index_find(a,b,(void **)&c)
#endif

// old enough versions of PHP don't support TSRMLS macros
#ifndef TSRMLS_DC
	#define TSRMLS_DC
#endif
#ifndef TSRMLS_CC
	#define TSRMLS_CC
#endif

extern "C" {

#ifdef _WIN32
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
	ZVAL server;
	ZVAL port;
	ZVAL socket;
	ZVAL user;
	ZVAL password;
	ZVAL retrytime;
	ZVAL tries;
	sqlrconnection *connection=NULL;
	if (ZEND_NUM_ARGS() != 7 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("zzzzzzz")
				&server,
				&port,
				&socket,
				&user,
				&password,
				&retrytime,
				&tries) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(server);
	convert_to_long_ex(port);
	convert_to_string_ex(socket);
	convert_to_string_ex(user);
	convert_to_string_ex(password);
	convert_to_long_ex(retrytime);
	convert_to_long_ex(tries);
	connection=new sqlrconnection(
			SVAL(server),
			LVAL(port),
			SVAL(socket),
			SVAL(user),
			SVAL(password),
			LVAL(retrytime),
			LVAL(tries),
			true);
	connection->debugPrintFunction((int (*)(const char *,...))zend_printf);
	ZEND_REGISTER_RESOURCE(return_value,connection,sqlrelay_connection);
}

DLEXPORT ZEND_FUNCTION(sqlrcon_free) {
	ZVAL sqlrcon;
	if (ZEND_NUM_ARGS() != 1 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("z")
				&sqlrcon) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrconnection *connection=NULL;
	ZEND_FETCH_RESOURCE(connection,
				sqlrconnection *,
				sqlrcon,
				-1,
				"sqlrelay connection",
				sqlrelay_connection);
	ZEND_LIST_DELETE(sqlrcon);
}

DLEXPORT ZEND_FUNCTION(sqlrcon_setconnecttimeout) {
	ZVAL sqlrcon;
	ZVAL timeoutsec;
	ZVAL timeoutusec;
	if (ZEND_NUM_ARGS() != 3 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("zzz")
				&sqlrcon,
				&timeoutsec,
				&timeoutusec) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(timeoutsec);
	convert_to_long_ex(timeoutusec);
	sqlrconnection *connection=NULL;
	ZEND_FETCH_RESOURCE(connection,
				sqlrconnection *,
				sqlrcon,
				-1,
				"sqlrelay connection",
				sqlrelay_connection);
	if (connection) {
		connection->setConnectTimeout(LVAL(timeoutsec),
						LVAL(timeoutusec));
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcon_setauthenticationtimeout) {
	ZVAL sqlrcon;
	ZVAL timeoutsec;
	ZVAL timeoutusec;
	if (ZEND_NUM_ARGS() != 3 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("zzz")
				&sqlrcon,
				&timeoutsec,
				&timeoutusec) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(timeoutsec);
	convert_to_long_ex(timeoutusec);
	sqlrconnection *connection=NULL;
	ZEND_FETCH_RESOURCE(connection,
				sqlrconnection *,
				sqlrcon,
				-1,
				"sqlrelay connection",
				sqlrelay_connection);
	if (connection) {
		connection->setAuthenticationTimeout(LVAL(timeoutsec),
							LVAL(timeoutusec));
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcon_setresponsetimeout) {
	ZVAL sqlrcon;
	ZVAL timeoutsec;
	ZVAL timeoutusec;
	if (ZEND_NUM_ARGS() != 3 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("zzz")
				&sqlrcon,
				&timeoutsec,
				&timeoutusec) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(timeoutsec);
	convert_to_long_ex(timeoutusec);
	sqlrconnection *connection=NULL;
	ZEND_FETCH_RESOURCE(connection,
				sqlrconnection *,
				sqlrcon,
				-1,
				"sqlrelay connection",
				sqlrelay_connection);
	if (connection) {
		connection->setResponseTimeout(LVAL(timeoutsec),
						LVAL(timeoutusec));
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcon_setbindvariabledelimiters) {
	ZVAL sqlrcon;
	ZVAL delimiters;
	if (ZEND_NUM_ARGS() != 2 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("zz")
				&sqlrcon,
				&delimiters) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(delimiters);
	sqlrconnection *connection=NULL;
	ZEND_FETCH_RESOURCE(connection,
				sqlrconnection *,
				sqlrcon,
				-1,
				"sqlrelay connection",
				sqlrelay_connection);
	if (connection) {
		connection->setBindVariableDelimiters(SVAL(delimiters));
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcon_getbindvariabledelimiterquestionmarksupported) {
	ZVAL sqlrcon;
	bool r;
	if (ZEND_NUM_ARGS() != 2 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("z")
				&sqlrcon) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrconnection *connection=NULL;
	ZEND_FETCH_RESOURCE(connection,
				sqlrconnection *,
				sqlrcon,
				-1,
				"sqlrelay connection",
				sqlrelay_connection);
	if (connection) {
		r=connection->getBindVariableDelimiterQuestionMarkSupported();
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcon_getbindvariabledelimitercolonsupported) {
	ZVAL sqlrcon;
	bool r;
	if (ZEND_NUM_ARGS() != 2 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("z")
				&sqlrcon) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrconnection *connection=NULL;
	ZEND_FETCH_RESOURCE(connection,
				sqlrconnection *,
				sqlrcon,
				-1,
				"sqlrelay connection",
				sqlrelay_connection);
	if (connection) {
		r=connection->getBindVariableDelimiterColonSupported();
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcon_getbindvariabledelimiteratsignsupported) {
	ZVAL sqlrcon;
	bool r;
	if (ZEND_NUM_ARGS() != 2 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("z")
				&sqlrcon) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrconnection *connection=NULL;
	ZEND_FETCH_RESOURCE(connection,
				sqlrconnection *,
				sqlrcon,
				-1,
				"sqlrelay connection",
				sqlrelay_connection);
	if (connection) {
		r=connection->getBindVariableDelimiterAtSignSupported();
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcon_getbindvariabledelimiterdollarsignsupported) {
	ZVAL sqlrcon;
	bool r;
	if (ZEND_NUM_ARGS() != 2 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("z")
				&sqlrcon) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrconnection *connection=NULL;
	ZEND_FETCH_RESOURCE(connection,
				sqlrconnection *,
				sqlrcon,
				-1,
				"sqlrelay connection",
				sqlrelay_connection);
	if (connection) {
		r=connection->getBindVariableDelimiterDollarSignSupported();
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcon_enablekerberos) {
	ZVAL sqlrcon;
	ZVAL service;
	ZVAL mech;
	ZVAL flags;
	if (ZEND_NUM_ARGS() != 4 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("zzzz")
				&sqlrcon,
				&service,
				&mech,
				&flags) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(service);
	convert_to_string_ex(mech);
	convert_to_string_ex(flags);
	sqlrconnection *connection=NULL;
	ZEND_FETCH_RESOURCE(connection,
				sqlrconnection *,
				sqlrcon,
				-1,
				"sqlrelay connection",
				sqlrelay_connection);
	if (connection) {
		connection->enableKerberos(SVAL(service),
						SVAL(mech),
						SVAL(flags));
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcon_enabletls) {
	ZVAL sqlrcon;
	ZVAL version;
	ZVAL cert;
	ZVAL password;
	ZVAL ciphers;
	ZVAL validate;
	ZVAL ca;
	ZVAL depth;
	if (ZEND_NUM_ARGS() != 8 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("zzzzzzzz")
				&sqlrcon,
				&version,
				&cert,
				&password,
				&ciphers,
				&validate,
				&ca,
				&depth) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(version);
	convert_to_string_ex(cert);
	convert_to_string_ex(password);
	convert_to_string_ex(ciphers);
	convert_to_string_ex(validate);
	convert_to_string_ex(ca);
	convert_to_long_ex(depth);
	sqlrconnection *connection=NULL;
	ZEND_FETCH_RESOURCE(connection,
				sqlrconnection *,
				sqlrcon,
				-1,
				"sqlrelay connection",
				sqlrelay_connection);
	if (connection) {
		connection->enableTls(SVAL(version),
					SVAL(cert),
					SVAL(password),
					SVAL(ciphers),
					SVAL(validate),
					SVAL(ca),
					LVAL(depth));
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcon_disableencryption) {
	ZVAL sqlrcon;
	if (ZEND_NUM_ARGS() != 1 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("z")
				&sqlrcon) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrconnection *connection=NULL;
	ZEND_FETCH_RESOURCE(connection,
				sqlrconnection *,
				sqlrcon,
				-1,
				"sqlrelay connection",
				sqlrelay_connection);
	if (connection) {
		connection->disableEncryption();
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcon_endsession) {
	ZVAL sqlrcon;
	if (ZEND_NUM_ARGS() != 1 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("z")
				&sqlrcon) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrconnection *connection=NULL;
	ZEND_FETCH_RESOURCE(connection,
				sqlrconnection *,
				sqlrcon,
				-1,
				"sqlrelay connection",
				sqlrelay_connection);
	if (connection) {
		connection->endSession();
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcon_suspendsession) {
	ZVAL sqlrcon;
	bool r;
	if (ZEND_NUM_ARGS() != 1 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("z")
				&sqlrcon) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrconnection *connection=NULL;
	ZEND_FETCH_RESOURCE(connection,
				sqlrconnection *,
				sqlrcon,
				-1,
				"sqlrelay connection",
				sqlrelay_connection);
	if (connection) {
		r=connection->suspendSession();
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcon_getconnectionport) {
	ZVAL sqlrcon;
	uint16_t r;
	if (ZEND_NUM_ARGS() != 1 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("z")
				&sqlrcon) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrconnection *connection=NULL;
	ZEND_FETCH_RESOURCE(connection,
				sqlrconnection *,
				sqlrcon,
				-1,
				"sqlrelay connection",
				sqlrelay_connection);
	if (connection) {
		r=connection->getConnectionPort();
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcon_getconnectionsocket) {
	ZVAL sqlrcon;
	const char *r;
	if (ZEND_NUM_ARGS() != 1 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("z")
				&sqlrcon) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrconnection *connection=NULL;
	ZEND_FETCH_RESOURCE(connection,
				sqlrconnection *,
				sqlrcon,
				-1,
				"sqlrelay connection",
				sqlrelay_connection);
	if (connection) {
		r=connection->getConnectionSocket();
		if (r) {
			RET_STRING(const_cast<char *>(r),1);
		}
	}
	RETURN_FALSE;
}

DLEXPORT ZEND_FUNCTION(sqlrcon_resumesession) {
	ZVAL sqlrcon;
	ZVAL port;
	ZVAL socket;
	bool r;
	if (ZEND_NUM_ARGS() != 3 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("zzz")
				&sqlrcon,
				&port,
				&socket) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(port);
	convert_to_string_ex(socket);
	sqlrconnection *connection=NULL;
	ZEND_FETCH_RESOURCE(connection,
				sqlrconnection *,
				sqlrcon,
				-1,
				"sqlrelay connection",
				sqlrelay_connection);
	if (connection) {
		r=connection->resumeSession(LVAL(port),
						SVAL(socket));
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcon_errormessage) {
	ZVAL sqlrcon;
	const char *r;
	if (ZEND_NUM_ARGS() != 1 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("z")
				&sqlrcon) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrconnection *connection=NULL;
	ZEND_FETCH_RESOURCE(connection,
				sqlrconnection *,
				sqlrcon,
				-1,
				"sqlrelay connection",
				sqlrelay_connection);
	if (connection) {
		r=connection->errorMessage();
		if (r) {
			RET_STRING(const_cast<char *>(r),1);
		}
	}
	RETURN_NULL();
}

DLEXPORT ZEND_FUNCTION(sqlrcon_errornumber) {
	ZVAL sqlrcon;
	int64_t r;
	if (ZEND_NUM_ARGS() != 1 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("z")
				&sqlrcon) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrconnection *connection=NULL;
	ZEND_FETCH_RESOURCE(connection,
				sqlrconnection *,
				sqlrcon,
				-1,
				"sqlrelay connection",
				sqlrelay_connection);
	if (connection) {
		r=connection->errorNumber();
		if (r) {
			RETURN_LONG(r);
		}
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcon_debugon) {
	ZVAL sqlrcon;
	if (ZEND_NUM_ARGS() != 1 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("z")
				&sqlrcon) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrconnection *connection=NULL;
	ZEND_FETCH_RESOURCE(connection,
				sqlrconnection *,
				sqlrcon,
				-1,
				"sqlrelay connection",
				sqlrelay_connection);
	if (connection) {
		connection->debugOn();
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcon_debugoff) {
	ZVAL sqlrcon;
	if (ZEND_NUM_ARGS() != 1 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("z")
				&sqlrcon) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrconnection *connection=NULL;
	ZEND_FETCH_RESOURCE(connection,
				sqlrconnection *,
				sqlrcon,
				-1,
				"sqlrelay connection",
				sqlrelay_connection);
	if (connection) {
		connection->debugOff();
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcon_getdebug) {
	ZVAL sqlrcon;
	bool r;
	if (ZEND_NUM_ARGS() != 1 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("z")
				&sqlrcon) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrconnection *connection=NULL;
	ZEND_FETCH_RESOURCE(connection,
				sqlrconnection *,
				sqlrcon,
				-1,
				"sqlrelay connection",
				sqlrelay_connection);
	if (connection) {
		r=connection->getDebug();
		if (r) {
			RETURN_TRUE;
		}
	}
	RETURN_FALSE;
}

DLEXPORT ZEND_FUNCTION(sqlrcon_setdebugfile) {
	ZVAL sqlrcon;
	ZVAL filename;
	if (ZEND_NUM_ARGS() != 2 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("zz")
				&sqlrcon,
				&filename) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(filename);
	sqlrconnection *connection=NULL;
	ZEND_FETCH_RESOURCE(connection,
				sqlrconnection *,
				sqlrcon,
				-1,
				"sqlrelay connection",
				sqlrelay_connection);
	if (connection) {
		connection->setDebugFile(SVAL(filename));
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcon_setclientinfo) {
	ZVAL sqlrcon;
	ZVAL clientinfo;
	if (ZEND_NUM_ARGS() != 2 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("zz")
				&sqlrcon,
				&clientinfo) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(clientinfo);
	sqlrconnection *connection=NULL;
	ZEND_FETCH_RESOURCE(connection,
				sqlrconnection *,
				sqlrcon,
				-1,
				"sqlrelay connection",
				sqlrelay_connection);
	if (connection) {
		connection->setClientInfo(SVAL(clientinfo));
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcon_getclientinfo) {
	ZVAL sqlrcon;
	const char *r;
	if (ZEND_NUM_ARGS() != 1 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("z")
				&sqlrcon) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrconnection *connection=NULL;
	ZEND_FETCH_RESOURCE(connection,
				sqlrconnection *,
				sqlrcon,
				-1,
				"sqlrelay connection",
				sqlrelay_connection);
	if (connection) {
		r=connection->getClientInfo();
		if (r) {
			RET_STRING(const_cast<char *>(r),1);
		}
	}
	RETURN_FALSE;
}

DLEXPORT ZEND_FUNCTION(sqlrcur_alloc) {
	zval	**sqlrcon;
	if (ZEND_NUM_ARGS() != 1 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("z")
				&sqlrcon) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrconnection *connection=NULL;
	ZEND_FETCH_RESOURCE(connection,
				sqlrconnection *,
				sqlrcon,
				-1,
				"sqlrelay connection",
				sqlrelay_connection);
	if (!connection) {
		RETURN_LONG(0);
	}
	sqlrcursor	*cursor=new sqlrcursor(connection,true);
	ZEND_REGISTER_RESOURCE(return_value,cursor,sqlrelay_cursor);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_free) {
	ZVAL sqlrcur;
	if (ZEND_NUM_ARGS() != 1 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("z")
				&sqlrcur) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	ZEND_LIST_DELETE(sqlrcur);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_setresultsetbuffersize) {
	ZVAL sqlrcur;
	ZVAL rows;
	if (ZEND_NUM_ARGS() != 2 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("zz")
				&sqlrcur,&rows) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(rows);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (cursor) {
		cursor->setResultSetBufferSize(LVAL(rows));
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getresultsetbuffersize) {
	ZVAL sqlrcur;
	uint64_t r;
	if (ZEND_NUM_ARGS() != 1 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("z")
				&sqlrcur) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (cursor) {
		r=cursor->getResultSetBufferSize();
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_dontgetcolumninfo) {
	ZVAL sqlrcur;
	if (ZEND_NUM_ARGS() != 1 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("z")
				&sqlrcur) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (cursor) {
		cursor->dontGetColumnInfo();
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getcolumninfo) {
	ZVAL sqlrcur;
	if (ZEND_NUM_ARGS() != 1 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("z")
				&sqlrcur) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (cursor) {
		cursor->getColumnInfo();
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcur_mixedcasecolumnnames) {
	ZVAL sqlrcur;
	if (ZEND_NUM_ARGS() != 1 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("z")
				&sqlrcur) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (cursor) {
		cursor->mixedCaseColumnNames();
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcur_uppercasecolumnnames) {
	ZVAL sqlrcur;
	if (ZEND_NUM_ARGS() != 1 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("z")
				&sqlrcur) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (cursor) {
		cursor->upperCaseColumnNames();
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcur_lowercasecolumnnames) {
	ZVAL sqlrcur;
	if (ZEND_NUM_ARGS() != 1 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("z")
				&sqlrcur) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (cursor) {
		cursor->lowerCaseColumnNames();
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcur_cachetofile) {
	ZVAL sqlrcur;
	ZVAL filename;
	if (ZEND_NUM_ARGS() != 2 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("zz")
				&sqlrcur,
				&filename) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(filename);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (cursor) {
		cursor->cacheToFile(SVAL(filename));
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcur_setcachettl) {
	ZVAL sqlrcur;
	ZVAL ttl;
	if (ZEND_NUM_ARGS() != 2 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("zz")
				&sqlrcur,
				&ttl) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(ttl);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (cursor) {
		cursor->setCacheTtl(LVAL(ttl));
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getcachefilename) {
	ZVAL sqlrcur;
	const char *r;
	if (ZEND_NUM_ARGS() != 1 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("z")
				&sqlrcur) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (cursor) {
		r=cursor->getCacheFileName();
		if (r) {
			RET_STRING(const_cast<char *>(r),1);
		}
	}
	RETURN_FALSE;
}

DLEXPORT ZEND_FUNCTION(sqlrcur_cacheoff) {
	ZVAL sqlrcur;
	if (ZEND_NUM_ARGS() != 1 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("z")
				&sqlrcur) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (cursor) {
		cursor->cacheOff();
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getdatabaselist) {
	ZVAL sqlrcur;
	ZVAL wild;
	bool r;
	if (ZEND_NUM_ARGS() != 2 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("zz")
				&sqlrcur,
				&wild) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(wild);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (cursor) {
		r=cursor->getDatabaseList(SVAL(wild));
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_gettablelist) {
	ZVAL sqlrcur;
	ZVAL wild;
	bool r;
	if (ZEND_NUM_ARGS() != 2 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("zz")
				&sqlrcur,
				&wild) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(wild);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (cursor) {
		r=cursor->getTableList(SVAL(wild));
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getcolumnlist) {
	ZVAL sqlrcur;
	ZVAL table;
	ZVAL wild;
	bool r;
	if (ZEND_NUM_ARGS() != 3 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("zzz")
				&sqlrcur,
				&table,
				&wild) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(table);
	convert_to_string_ex(wild);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (cursor) {
		r=cursor->getColumnList(SVAL(table),
					SVAL(wild));
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_sendquery) {
	ZVAL sqlrcur;
	ZVAL query;
	bool r;
	if (ZEND_NUM_ARGS() != 2 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("zz")
				&sqlrcur,
				&query) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(query);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (cursor) {
		r=cursor->sendQuery(SVAL(query));
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_sendquerywithlength) {
	ZVAL sqlrcur;
	ZVAL query;
	ZVAL length;
	bool r;
	if (ZEND_NUM_ARGS() != 3 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("zzz")
				&sqlrcur,
				&query,
				&length) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(query);
	convert_to_long_ex(length);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (cursor) {
		r=cursor->sendQuery(SVAL(query),
					LVAL(length));
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_sendfilequery) {
	ZVAL sqlrcur;
	ZVAL path;
	ZVAL filename;
	bool r;
	if (ZEND_NUM_ARGS() != 3 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("zzz")
				&sqlrcur,
				&path,
				&filename) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(path);
	convert_to_string_ex(filename);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (cursor) {
		r=((sqlrcursor *)LVAL(sqlrcur))->
				sendFileQuery(SVAL(path),
						SVAL(filename));
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_preparequery) {
	ZVAL sqlrcur;
	ZVAL query;
	if (ZEND_NUM_ARGS() != 2 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("zz")
				&sqlrcur,
				&query) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(query);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (cursor) {
		cursor->prepareQuery(SVAL(query));
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcur_preparequerywithlength) {
	ZVAL sqlrcur;
	ZVAL query;
	ZVAL length;
	if (ZEND_NUM_ARGS() != 3 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("zzz")
				&sqlrcur,
				&query,
				&length) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(query);
	convert_to_long_ex(length);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (cursor) {
		cursor->prepareQuery(SVAL(query),
					LVAL(length));
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcur_preparefilequery) {
	ZVAL sqlrcur;
	ZVAL path;
	ZVAL filename;
	bool r;
	if (ZEND_NUM_ARGS() != 3 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("zzz")
				&sqlrcur,
				&path,
				&filename) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(path);
	convert_to_string_ex(filename);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (cursor) {
		r=cursor->prepareFileQuery(SVAL(path),
						SVAL(filename));
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_substitution) {
	ZVAL sqlrcur;
	ZVAL variable;
	ZVAL value;
	ZVAL precision;
	ZVAL scale;
	if (ZEND_NUM_ARGS() != 3 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("zzz")
				&sqlrcur,
				&variable,
				&value) == FAILURE) {
		if (ZEND_NUM_ARGS() != 5 || 
			GET_PARAMETERS(
					ZEND_NUM_ARGS() TSRMLS_CC,
					PARAMS("zzzzz")
					&sqlrcur,
					&variable,
					&value,
					&precision,
					&scale)== FAILURE) {
			WRONG_PARAM_COUNT;
		}
	}
	convert_to_string_ex(variable);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (cursor) {
		if (TYPE(value)==IS_STRING) {
			convert_to_string_ex(value);
			cursor->substitution(SVAL(variable),
						SVAL(value));
			RETURN_LONG(1);
		} else if (TYPE(value)==IS_LONG) {
			convert_to_long_ex(value);
			cursor->substitution(SVAL(variable),
						LVAL(value));
			RETURN_LONG(1);
		} else if (ZEND_NUM_ARGS()==5 && TYPE(value)==IS_DOUBLE) {
			convert_to_double_ex(value);
			cursor->substitution(
				SVAL(variable),
				DVAL(value),
				(unsigned short)LVAL(precision),
				(unsigned short)LVAL(scale));
			RETURN_LONG(1);
		} else if (TYPE(value)==IS_NULL) {
			cursor->substitution(
				SVAL(variable),
				(const char *)NULL);
			RETURN_LONG(1);
		}
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_clearbinds) {
	ZVAL sqlrcur;
	if (ZEND_NUM_ARGS() != 1 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("z")
				&sqlrcur) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (cursor) {
		cursor->clearBinds();
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcur_countbindvariables) {
	ZVAL sqlrcur;
	uint16_t r;
	if (ZEND_NUM_ARGS() != 1 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("z")
				&sqlrcur) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (cursor) {
		r=cursor->countBindVariables();
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_inputbind) {
	ZVAL sqlrcur;
	ZVAL variable;
	ZVAL value;
	ZVAL precision;
	ZVAL scale;
	ZVAL length;
	if (ZEND_NUM_ARGS() != 3 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("zzz")
				&sqlrcur,
				&variable,
				&value) == FAILURE) {
		if (ZEND_NUM_ARGS() != 4 || 
			GET_PARAMETERS(
					ZEND_NUM_ARGS() TSRMLS_CC,
					PARAMS("zzzz")
					&sqlrcur,
					&variable,
					&value,
					&length)== FAILURE) {
			if (ZEND_NUM_ARGS() != 5 || 
				GET_PARAMETERS(
						ZEND_NUM_ARGS() TSRMLS_CC,
						PARAMS("zzzzz")
						&sqlrcur,
						&variable,
						&value,
						&precision,
						&scale)== FAILURE) {
				WRONG_PARAM_COUNT;
			}
		}
	}
	convert_to_string_ex(variable);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (cursor) {
		if (TYPE(value)==IS_STRING) {
			convert_to_string_ex(value);
			if (ZEND_NUM_ARGS() == 4 && LVAL(length)>0) {
				cursor->inputBind(
					SVAL(variable),
					SVAL(value),
					LVAL(length));
			} else {
				cursor->inputBind(
					SVAL(variable),
					SVAL(value));
			}
			RETURN_LONG(1);
		} else if (TYPE(value)==IS_LONG) {
			convert_to_long_ex(value);
			cursor->inputBind(SVAL(variable),
						LVAL(value));
			RETURN_LONG(1);
		} else if (ZEND_NUM_ARGS()==5 && TYPE(value)==IS_DOUBLE) {
			convert_to_double_ex(value);
			cursor->inputBind(
				SVAL(variable),
				DVAL(value),
				(unsigned short)LVAL(precision),
				(unsigned short)LVAL(scale));
			RETURN_LONG(1);
		} else if (TYPE(value)==IS_NULL) {
			cursor->inputBind(
				SVAL(variable),
				(const char *)NULL);
			RETURN_LONG(1);
		}
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_inputbindblob) {
	ZVAL sqlrcur;
	ZVAL variable;
	ZVAL value;
	ZVAL size;
	if (ZEND_NUM_ARGS() != 4 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("zzzz")
				&sqlrcur,
				&variable,
				&value,
				&size) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(variable);
	convert_to_string_ex(value);
	convert_to_long_ex(size);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (cursor) {
		cursor->inputBindBlob(SVAL(variable),
					SVAL(value),
					LVAL(size));
		RETURN_LONG(1);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_inputbindclob) {
	ZVAL sqlrcur;
	ZVAL variable;
	ZVAL value;
	ZVAL size;
	if (ZEND_NUM_ARGS() != 4 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("zzzz")
				&sqlrcur,
				&variable,
				&value,
				&size) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(variable);
	convert_to_string_ex(value);
	convert_to_long_ex(size);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (cursor) {
		cursor->inputBindClob(SVAL(variable),
					SVAL(value),
					LVAL(size));
		RETURN_LONG(1);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_defineoutputbindstring) {
	ZVAL sqlrcur;
	ZVAL variable;
	ZVAL length;
	if (ZEND_NUM_ARGS() != 3 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("zzz")
				&sqlrcur,
				&variable,
				&length) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(variable);
	convert_to_long_ex(length);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (cursor) {
		cursor->defineOutputBindString(
					SVAL(variable),
					LVAL(length));
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcur_defineoutputbindinteger) {
	ZVAL sqlrcur;
	ZVAL variable;
	if (ZEND_NUM_ARGS() != 2 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("zz")
				&sqlrcur,
				&variable) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(variable);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (cursor) {
		cursor->defineOutputBindInteger(SVAL(variable));
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcur_defineoutputbinddouble) {
	ZVAL sqlrcur;
	ZVAL variable;
	if (ZEND_NUM_ARGS() != 2 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("zz")
				&sqlrcur,
				&variable) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(variable);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (cursor) {
		cursor->defineOutputBindDouble(SVAL(variable));
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcur_defineoutputbindblob) {
	ZVAL sqlrcur;
	ZVAL variable;
	if (ZEND_NUM_ARGS() != 2 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("zz")
				&sqlrcur,
				&variable) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(variable);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (cursor) {
		cursor->defineOutputBindBlob(SVAL(variable));
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcur_defineoutputbindclob) {
	ZVAL sqlrcur;
	ZVAL variable;
	if (ZEND_NUM_ARGS() != 2 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("zz")
				&sqlrcur,
				&variable) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(variable);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (cursor) {
		cursor->defineOutputBindClob(SVAL(variable));
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcur_defineoutputbindcursor) {
	ZVAL sqlrcur;
	ZVAL variable;
	if (ZEND_NUM_ARGS() != 2 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("zz")
				&sqlrcur,
				&variable) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(variable);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (cursor) {
		cursor->defineOutputBindCursor(SVAL(variable));
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcur_substitutions) {
	ZVAL sqlrcur;
	ZVAL variables;
	ZVAL values;
	ZVAL precisions;
	ZVAL scales;
	ZVAL var;
	ZVAL val;
	ZVAL precision;
	ZVAL scale;
	unsigned int i;
	if (ZEND_NUM_ARGS() != 3 ||
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("zzz")
				&sqlrcur,
				&variables,
				&values) == FAILURE) {
		if (ZEND_NUM_ARGS() != 5 || 
			GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
					PARAMS("zzzzz")
					&sqlrcur,
					&variables,
					&values,
					&precisions,
					&scales)== FAILURE) {
			WRONG_PARAM_COUNT;
		} else {
			convert_to_array_ex(precisions);
			convert_to_array_ex(scales);
		}
	}
	convert_to_array_ex(variables);
	convert_to_array_ex(values);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	long	success=1;
	if (cursor) {
		for (i=0; i<ARRVAL(variables)->nNumOfElements; i++) {
			HASH_INDEX_FIND(ARRVAL(variables),i,var);
			HASH_INDEX_FIND(ARRVAL(values),i,val);
			if (TYPE(val)==IS_STRING) {
				convert_to_string_ex(val);
				cursor->substitution(SVAL(var),
							SVAL(val));
			} else if (TYPE(val)==IS_LONG) {
				convert_to_long_ex(val);
				cursor->substitution(SVAL(var),
							LVAL(val));
			} else if (ZEND_NUM_ARGS()==5 &&
						TYPE(val)==IS_DOUBLE) {
				HASH_INDEX_FIND(ARRVAL(precisions),i,precision);
				HASH_INDEX_FIND(ARRVAL(scales),i,scale);
				convert_to_double_ex(val);
				convert_to_long_ex(precision);
				convert_to_long_ex(scale);
				cursor->substitution(
					SVAL(var),
					DVAL(val),
					(unsigned short)LVAL(precision),
					(unsigned short)LVAL(scale));
			} else if (TYPE(val)==IS_NULL) {
				cursor->substitution(SVAL(var),
							(const char *)NULL);
			} else {
				success=0;
			}
		}
	}
	RETURN_LONG(success);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_inputbinds) {
	ZVAL sqlrcur;
	ZVAL variables;
	ZVAL values;
	ZVAL precisions;
	ZVAL scales;
	ZVAL var;
	ZVAL val;
	ZVAL precision;
	ZVAL scale;
	int i;
	if (ZEND_NUM_ARGS() != 3 ||
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("zzz")
				&sqlrcur,
				&variables,
				&values) == FAILURE) {
		if (ZEND_NUM_ARGS() != 5 || 
			GET_PARAMETERS(
					ZEND_NUM_ARGS() TSRMLS_CC,
					PARAMS("zzzzz")
					&sqlrcur,
					&variables,
					&values,
					&precisions,
					&scales)== FAILURE) {
			WRONG_PARAM_COUNT;
		} else {
			convert_to_array_ex(precisions);
			convert_to_array_ex(scales);
		}
	}
	convert_to_array_ex(variables);
	convert_to_array_ex(values);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	long	success=1;
	if (cursor) {
		for (i=0; i<zend_hash_num_elements(ARRVAL(variables)); i++) {
			HASH_INDEX_FIND(ARRVAL(variables),i,var);
			HASH_INDEX_FIND(ARRVAL(values),i,val);
			if (TYPE(val)==IS_STRING) {
				convert_to_string_ex(val);
				cursor->inputBind(SVAL(var),
							SVAL(val));
			} else if (TYPE(val)==IS_LONG) {
				convert_to_long_ex(val);
				cursor->inputBind(SVAL(var),
							LVAL(val));
			} else if (ZEND_NUM_ARGS()==5 &&
						TYPE(val)==IS_DOUBLE) {
				HASH_INDEX_FIND(ARRVAL(precisions),i,precision);
				HASH_INDEX_FIND(ARRVAL(scales),i,scale);
				convert_to_long_ex(precision);
				convert_to_long_ex(scale);
				convert_to_double_ex(val);
				cursor->inputBind(
					SVAL(var),
					DVAL(val),
					(unsigned short)LVAL(precision),
					(unsigned short)LVAL(scale));
			} else if (TYPE(val)==IS_NULL) {
				cursor->inputBind(SVAL(var),
							(const char *)NULL);
			} else {
				success=0;
			}
		}
	}
	RETURN_LONG(success);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_validatebinds) {
	ZVAL sqlrcur;
	if (ZEND_NUM_ARGS() != 1 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("z")
				&sqlrcur) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (cursor) {
		cursor->validateBinds();
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcur_validbind) {
	ZVAL sqlrcur;
	ZVAL variable;
	bool r;
	if (ZEND_NUM_ARGS() != 2 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("zz")
				&sqlrcur,
				&variable) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(variable);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (cursor) {
		r=cursor->validBind(SVAL(variable));
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_executequery) {
	ZVAL sqlrcur;
	bool r;
	if (ZEND_NUM_ARGS() != 1 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("z")
				&sqlrcur) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (cursor) {
		r=cursor->executeQuery();
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_fetchfrombindcursor) {
	ZVAL sqlrcur;
	bool r;
	if (ZEND_NUM_ARGS() != 1 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("z")
				&sqlrcur) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (cursor) {
		r=cursor->fetchFromBindCursor();
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getoutputbindstring) {
	ZVAL sqlrcur;
	ZVAL variable;
	const char *r;
	uint32_t rl;
	if (ZEND_NUM_ARGS() != 2 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("zz")
				&sqlrcur,
				&variable) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(variable);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (cursor) {
		r=cursor->getOutputBindString(SVAL(variable));
		rl=cursor->getOutputBindLength(SVAL(variable));
		if (r) {
			RET_STRINGL(const_cast<char *>(r),rl,1);
		}
	}
	RETURN_NULL();
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getoutputbindblob) {
	ZVAL sqlrcur;
	ZVAL variable;
	const char *r;
	uint32_t rl;
	if (ZEND_NUM_ARGS() != 2 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("zz")
				&sqlrcur,
				&variable) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(variable);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (cursor) {
		r=cursor->getOutputBindBlob(SVAL(variable));
		rl=cursor->getOutputBindLength(SVAL(variable));
		if (r) {
			RET_STRINGL(const_cast<char *>(r),rl,1);
		}
	}
	RETURN_NULL();
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getoutputbindclob) {
	ZVAL sqlrcur;
	ZVAL variable;
	const char *r;
	uint32_t rl;
	if (ZEND_NUM_ARGS() != 2 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("zz")
				&sqlrcur,
				&variable) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(variable);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (cursor) {
		r=cursor->getOutputBindClob(SVAL(variable));
		rl=cursor->getOutputBindLength(SVAL(variable));
		if (r) {
			RET_STRINGL(const_cast<char *>(r),rl,1);
		}
	}
	RETURN_NULL();
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getoutputbindinteger) {
	ZVAL sqlrcur;
	ZVAL variable;
	int64_t r;
	if (ZEND_NUM_ARGS() != 2 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("zz")
				&sqlrcur,
				&variable) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(variable);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (cursor) {
		r=cursor->getOutputBindInteger(SVAL(variable));
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getoutputbinddouble) {
	ZVAL sqlrcur;
	ZVAL variable;
	double r;
	if (ZEND_NUM_ARGS() != 2 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("zz")
				&sqlrcur,
				&variable) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(variable);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (cursor) {
		r=cursor->getOutputBindDouble(SVAL(variable));
		RETURN_DOUBLE(r);
	}
	RETURN_DOUBLE(0.0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getoutputbindlength) {
	ZVAL sqlrcur;
	ZVAL variable;
	uint32_t r;
	if (ZEND_NUM_ARGS() != 2 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("zz")
				&sqlrcur,
				&variable) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(variable);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (cursor) {
		r=cursor->getOutputBindLength(SVAL(variable));
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getoutputbindcursor) {
	ZVAL sqlrcur;
	ZVAL variable;
	if (ZEND_NUM_ARGS() != 2 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("zz")
				&sqlrcur,
				&variable) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(variable);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (!cursor) {
		RETURN_LONG(0);
	}
	sqlrcursor	*s=cursor->getOutputBindCursor(
					SVAL(variable),
					true);
	ZEND_REGISTER_RESOURCE(return_value,s,sqlrelay_cursor);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_opencachedresultset) {
	ZVAL sqlrcur;
	ZVAL filename;
	bool r;
	if (ZEND_NUM_ARGS() != 2 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("zz")
				&sqlrcur,
				&filename) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(filename);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (cursor) {
		r=cursor->openCachedResultSet(SVAL(filename));
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_colcount) {
	ZVAL sqlrcur;
	uint32_t r;
	if (ZEND_NUM_ARGS() != 1 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("z")
				&sqlrcur) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (cursor) {
		r=cursor->colCount();
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_rowcount) {
	ZVAL sqlrcur;
	uint64_t r;
	if (ZEND_NUM_ARGS() != 1 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("z")
				&sqlrcur) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (cursor) {
		r=cursor->rowCount();
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_totalrows) {
	ZVAL sqlrcur;
	uint64_t r;
	if (ZEND_NUM_ARGS() != 1 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("z")
				&sqlrcur) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (cursor) {
		r=cursor->totalRows();
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_affectedrows) {
	ZVAL sqlrcur;
	uint64_t r;
	if (ZEND_NUM_ARGS() != 1 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("z")
				&sqlrcur) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (cursor) {
		r=cursor->affectedRows();
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_firstrowindex) {
	ZVAL sqlrcur;
	uint64_t r;
	if (ZEND_NUM_ARGS() != 1 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("z")
				&sqlrcur) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (cursor) {
		r=cursor->firstRowIndex();
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_endofresultset) {
	ZVAL sqlrcur;
	bool r;
	if (ZEND_NUM_ARGS() != 1 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("z")
				&sqlrcur) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (cursor) {
		r=cursor->endOfResultSet();
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_errormessage) {
	ZVAL sqlrcur;
	const char *r;
	if (ZEND_NUM_ARGS() != 1 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("z")
				&sqlrcur) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (cursor) {
		r=cursor->errorMessage();
		if (r) {
			RET_STRING(const_cast<char *>(r),1);
		}
	}
	RETURN_NULL();
}

DLEXPORT ZEND_FUNCTION(sqlrcur_errornumber) {
	ZVAL sqlrcur;
	int64_t r;
	if (ZEND_NUM_ARGS() != 1 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("z")
				&sqlrcur) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (cursor) {
		r=cursor->errorNumber();
		if (r) {
			RETURN_LONG(r);
		}
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getnullsasemptystrings) {
	ZVAL sqlrcur;
	if (ZEND_NUM_ARGS() != 1 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("z")
				&sqlrcur) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (cursor) {
		cursor->getNullsAsEmptyStrings();
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getnullsasnulls) {
	ZVAL sqlrcur;
	if (ZEND_NUM_ARGS() != 1 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("z")
				&sqlrcur) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (cursor) {
		cursor->getNullsAsNulls();
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getfield) {
	ZVAL sqlrcur;
	ZVAL row;
	ZVAL col;
	const char *r=NULL;
	uint32_t rl;
	if (ZEND_NUM_ARGS() != 3 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("zzz")
				&sqlrcur,
				&row,
				&col) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(row);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (cursor) {
		if (TYPE(col)==IS_LONG) {
			convert_to_long_ex(col);
			r=cursor->getField(LVAL(row),LVAL(col));
			rl=cursor->getFieldLength(LVAL(row),LVAL(col));
		} else if (TYPE(col)==IS_STRING) {
			convert_to_string_ex(col);
			r=cursor->getField(LVAL(row),
						SVAL(col));
			rl=cursor->getFieldLength(LVAL(row),
						SVAL(col));
		}
		if (r) {
			RET_STRINGL(const_cast<char *>(r),rl,1);
		}
	}
	RETURN_NULL();
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getfieldasinteger) {
	ZVAL sqlrcur;
	ZVAL row;
	ZVAL col;
	int64_t r=0;
	if (ZEND_NUM_ARGS() != 3 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("zzz")
				&sqlrcur,
				&row,
				&col) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(row);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (cursor) {
		if (TYPE(col)==IS_LONG) {
			convert_to_long_ex(col);
			r=cursor->getFieldAsInteger(LVAL(row),
							LVAL(col));
		} else if (TYPE(col)==IS_STRING) {
			convert_to_string_ex(col);
			r=cursor->getFieldAsInteger(LVAL(row),
							SVAL(col));
		}
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getfieldasdouble) {
	ZVAL sqlrcur;
	ZVAL row;
	ZVAL col;
	double r=0.0;
	if (ZEND_NUM_ARGS() != 3 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("zzz")
				&sqlrcur,
				&row,
				&col) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(row);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (cursor) {
		if (TYPE(col)==IS_LONG) {
			convert_to_long_ex(col);
			r=cursor->getFieldAsDouble(LVAL(row),
							LVAL(col));
		} else if (TYPE(col)==IS_STRING) {
			convert_to_string_ex(col);
			r=cursor->getFieldAsDouble(LVAL(row),
							SVAL(col));
		}
		RETURN_DOUBLE(r);
	}
	RETURN_DOUBLE(0.0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getfieldlength) {
	ZVAL sqlrcur;
	ZVAL row;
	ZVAL col;
	uint32_t r=0;
	if (ZEND_NUM_ARGS() != 3 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("zzz")
				&sqlrcur,
				&row,
				&col) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(row);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (cursor) {
		if (TYPE(col)==IS_LONG) {
			convert_to_long_ex(col);
			r=cursor->getFieldLength(LVAL(row),
							LVAL(col));
		} else if (TYPE(col)==IS_STRING) {
			convert_to_string_ex(col);
			r=cursor->getFieldLength(LVAL(row),
							SVAL(col));
		}
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getrow) {
	ZVAL sqlrcur;
	ZVAL row;
	const char * const *r;
	uint32_t *l;
	uint32_t i;
	if (ZEND_NUM_ARGS() != 2 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("zz")
				&sqlrcur,
				&row) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(row);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (!cursor) {
		RETURN_FALSE;
	}
	r=cursor->getRow(LVAL(row));
	l=cursor->getRowLengths(LVAL(row));
	if (!r) {
		RETURN_FALSE;
	}
	#ifdef ARRAY_INIT_CANT_FAIL
	array_init(return_value);
	#else
	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}
	#endif
	for (i=0; i<cursor->colCount(); i++) {
		if (!r[i]) {
			// using add_next_index_unset because add_assoc_null
			// isn't defined in older php
			add_next_index_unset(return_value);
		} else {
			ADD_NEXT_INDEX_STRINGL(return_value,
						const_cast<char *>(r[i]),
						l[i],
						1);
		}
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getrowassoc) {
	ZVAL sqlrcur;
	ZVAL row;
	const char * const *r;
	uint32_t *l;
	const char * const *rC;
	uint32_t i;
	if (ZEND_NUM_ARGS() != 2 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("zz")
				&sqlrcur,
				&row) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(row);

	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (!cursor) {
		RETURN_FALSE;
	}

	rC=cursor->getColumnNames();
	if (!rC) {
		RETURN_FALSE;
	}

	r=cursor->getRow(LVAL(row));
	l=cursor->getRowLengths(LVAL(row));
	if (!r) {
		RETURN_FALSE;
	}
	#ifdef ARRAY_INIT_CANT_FAIL
	array_init(return_value);
	#else
	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}
	#endif
	for (i=0; i<cursor->colCount(); i++) {
		if (!r[i]) {
			// using add_assoc_unset because add_assoc_null isn't
			// defined in older php
			add_assoc_unset(return_value,const_cast<char *>(rC[i]));
		} else {
			ADD_ASSOC_STRINGL(return_value,
					const_cast<char *>(rC[i]),
					const_cast<char *>(r[i]),
					l[i],
					1);
		}
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getrowlengths) {
	ZVAL sqlrcur;
	ZVAL row;
	uint32_t *r;
	uint32_t i;
	if (ZEND_NUM_ARGS() != 2 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("zz")
				&sqlrcur,
				&row) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(row);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (!cursor) {
		RETURN_FALSE;
	}
	r=cursor->getRowLengths(LVAL(row));
	if (!r) {
		RETURN_FALSE;
	}
	#ifdef ARRAY_INIT_CANT_FAIL
	array_init(return_value);
	#else
	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}
	#endif
	for (i=0; i<cursor->colCount(); i++) {
		add_next_index_long(return_value,r[i]);
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getrowlengthsassoc) {
	ZVAL sqlrcur;
	ZVAL row;
	uint32_t *r;
	const char * const *rC;
	uint32_t i;
	if (ZEND_NUM_ARGS() != 2 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("zz")
				&sqlrcur,
				&row) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(row);

	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (!cursor) {
		RETURN_FALSE;
	}

	rC=cursor->getColumnNames();
	if (!rC) {
		RETURN_FALSE;
	}

	r=cursor->getRowLengths(LVAL(row));
	if (!r) {
		RETURN_FALSE;
	}
	#ifdef ARRAY_INIT_CANT_FAIL
	array_init(return_value);
	#else
	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}
	#endif
	for (i=0; i<cursor->colCount(); i++) {
		add_assoc_long(return_value,const_cast<char *>(rC[i]),r[i]);
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getcolumnnames) {
	ZVAL sqlrcur;
	const char * const *r;
	uint32_t i;
	if (ZEND_NUM_ARGS() != 1 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("z")
				&sqlrcur) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (!cursor) {
		RETURN_FALSE;
	}
	r=cursor->getColumnNames();
	if (!r) {
		RETURN_FALSE;
	}
	#ifdef ARRAY_INIT_CANT_FAIL
	array_init(return_value);
	#else
	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}
	#endif
	for (i=0; i<cursor->colCount(); i++) {
		ADD_NEXT_INDEX_STRING(return_value,const_cast<char *>(r[i]),1);
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getcolumnname) {
	ZVAL sqlrcur;
	ZVAL col;
	const char *r;
	if (ZEND_NUM_ARGS() != 2 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("zz")
				&sqlrcur,
				&col) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(col);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (cursor) {
		r=cursor->getColumnName(LVAL(col));
		if (r) {
			RET_STRING(const_cast<char *>(r),1);
		}
	}
	RETURN_FALSE;
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getcolumntype) {
	ZVAL sqlrcur;
	ZVAL col;
	const char *r=NULL;
	if (ZEND_NUM_ARGS() != 2 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("zz")
				&sqlrcur,
				&col) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (cursor) {
		if (TYPE(col)==IS_LONG) {
			convert_to_long_ex(col);
			r=cursor->getColumnType(LVAL(col));
		} else if (TYPE(col)==IS_STRING) {
			convert_to_string_ex(col);
			r=cursor->getColumnType(SVAL(col));
		}
		if (r) {
			RET_STRING(const_cast<char *>(r),1);
		}
	}
	RETURN_FALSE;
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getcolumnlength) {
	ZVAL sqlrcur;
	ZVAL col;
	uint32_t r=0;
	if (ZEND_NUM_ARGS() != 2 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("zz")
				&sqlrcur,
				&col) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (cursor) {
		if (TYPE(col)==IS_LONG) {
			convert_to_long_ex(col);
			r=cursor->getColumnLength(LVAL(col));
		} else if (TYPE(col)==IS_STRING) {
			convert_to_string_ex(col);
			r=cursor->getColumnLength(SVAL(col));
		}
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getcolumnprecision) {
	ZVAL sqlrcur;
	ZVAL col;
	uint32_t r=0;
	if (ZEND_NUM_ARGS() != 2 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("zz")
				&sqlrcur,
				&col) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (cursor) {
		if (TYPE(col)==IS_LONG) {
			convert_to_long_ex(col);
			r=cursor->getColumnPrecision(LVAL(col));
		} else if (TYPE(col)==IS_STRING) {
			convert_to_string_ex(col);
			r=cursor->getColumnPrecision(SVAL(col));
		}
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getcolumnscale) {
	ZVAL sqlrcur;
	ZVAL col;
	uint32_t r=0;
	if (ZEND_NUM_ARGS() != 2 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("zz")
				&sqlrcur,
				&col) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (cursor) {
		if (TYPE(col)==IS_LONG) {
			convert_to_long_ex(col);
			r=cursor->getColumnScale(LVAL(col));
		} else if (TYPE(col)==IS_STRING) {
			convert_to_string_ex(col);
			r=cursor->getColumnScale(SVAL(col));
		}
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getcolumnisnullable) {
	ZVAL sqlrcur;
	ZVAL col;
	bool r=0;
	if (ZEND_NUM_ARGS() != 2 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("zz")
				&sqlrcur,
				&col) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (cursor) {
		if (TYPE(col)==IS_LONG) {
			convert_to_long_ex(col);
			r=cursor->getColumnIsNullable(LVAL(col));
		} else if (TYPE(col)==IS_STRING) {
			convert_to_string_ex(col);
			r=cursor->getColumnIsNullable(SVAL(col));
		}
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getcolumnisprimarykey) {
	ZVAL sqlrcur;
	ZVAL col;
	bool r=0;
	if (ZEND_NUM_ARGS() != 2 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("zz")
				&sqlrcur,
				&col) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (cursor) {
		if (TYPE(col)==IS_LONG) {
			convert_to_long_ex(col);
			r=cursor->getColumnIsPrimaryKey(LVAL(col));
		} else if (TYPE(col)==IS_STRING) {
			convert_to_string_ex(col);
			r=cursor->getColumnIsPrimaryKey(SVAL(col));
		}
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getcolumnisunique) {
	ZVAL sqlrcur;
	ZVAL col;
	bool r=0;
	if (ZEND_NUM_ARGS() != 2 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("zz")
				&sqlrcur,
				&col) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (cursor) {
		if (TYPE(col)==IS_LONG) {
			convert_to_long_ex(col);
			r=cursor->getColumnIsUnique(LVAL(col));
		} else if (TYPE(col)==IS_STRING) {
			convert_to_string_ex(col);
			r=cursor->getColumnIsUnique(SVAL(col));
		}
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getcolumnispartofkey) {
	ZVAL sqlrcur;
	ZVAL col;
	bool r=0;
	if (ZEND_NUM_ARGS() != 2 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("zz")
				&sqlrcur,
				&col) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (cursor) {
		if (TYPE(col)==IS_LONG) {
			convert_to_long_ex(col);
			r=cursor->getColumnIsPartOfKey(LVAL(col));
		} else if (TYPE(col)==IS_STRING) {
			convert_to_string_ex(col);
			r=cursor->getColumnIsPartOfKey(SVAL(col));
		}
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getcolumnisunsigned) {
	ZVAL sqlrcur;
	ZVAL col;
	bool r=0;
	if (ZEND_NUM_ARGS() != 2 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("zz")
				&sqlrcur,
				&col) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (cursor) {
		if (TYPE(col)==IS_LONG) {
			convert_to_long_ex(col);
			r=cursor->getColumnIsUnsigned(LVAL(col));
		} else if (TYPE(col)==IS_STRING) {
			convert_to_string_ex(col);
			r=cursor->getColumnIsUnsigned(SVAL(col));
		}
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getcolumniszerofilled) {
	ZVAL sqlrcur;
	ZVAL col;
	bool r=0;
	if (ZEND_NUM_ARGS() != 2 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("zz")
				&sqlrcur,
				&col) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (cursor) {
		if (TYPE(col)==IS_LONG) {
			convert_to_long_ex(col);
			r=cursor->getColumnIsZeroFilled(LVAL(col));
		} else if (TYPE(col)==IS_STRING) {
			convert_to_string_ex(col);
			r=cursor->getColumnIsZeroFilled(SVAL(col));
		}
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getcolumnisbinary) {
	ZVAL sqlrcur;
	ZVAL col;
	bool r=0;
	if (ZEND_NUM_ARGS() != 2 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("zz")
				&sqlrcur,
				&col) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (cursor) {
		if (TYPE(col)==IS_LONG) {
			convert_to_long_ex(col);
			r=cursor->getColumnIsBinary(LVAL(col));
		} else if (TYPE(col)==IS_STRING) {
			convert_to_string_ex(col);
			r=cursor->getColumnIsBinary(SVAL(col));
		}
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getcolumnisautoincrement) {
	ZVAL sqlrcur;
	ZVAL col;
	bool r=0;
	if (ZEND_NUM_ARGS() != 2 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("zz")
				&sqlrcur,
				&col) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (cursor) {
		if (TYPE(col)==IS_LONG) {
			convert_to_long_ex(col);
			r=cursor->getColumnIsAutoIncrement(LVAL(col));
		} else if (TYPE(col)==IS_STRING) {
			convert_to_string_ex(col);
			r=cursor->getColumnIsAutoIncrement(SVAL(col));
		}
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getlongest) {
	ZVAL sqlrcur;
	ZVAL col;
	uint32_t r=0;
	if (ZEND_NUM_ARGS() != 2 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("zz")
				&sqlrcur,
				&col) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (cursor) {
		if (TYPE(col)==IS_LONG) {
			convert_to_long_ex(col);
			r=cursor->getLongest(LVAL(col));
		} else if (TYPE(col)==IS_STRING) {
			convert_to_string_ex(col);
			r=cursor->getLongest(SVAL(col));
		}
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getresultsetid) {
	ZVAL sqlrcur;
	uint16_t r;
	if (ZEND_NUM_ARGS() != 1 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("z")
				&sqlrcur) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (cursor) {
		r=cursor->getResultSetId();
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_suspendresultset) {
	ZVAL sqlrcur;
	if (ZEND_NUM_ARGS() != 1 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("z")
				&sqlrcur) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (cursor) {
		cursor->suspendResultSet();
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcur_resumeresultset) {
	ZVAL sqlrcur;
	ZVAL id;
	bool r;
	if (ZEND_NUM_ARGS() != 2 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("zz")
				&sqlrcur,
				&id) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(id);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (cursor) {
		r=cursor->resumeResultSet(LVAL(id));
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_resumecachedresultset) {
	ZVAL sqlrcur;
	ZVAL id;
	ZVAL filename;
	bool r;
	if (ZEND_NUM_ARGS() != 3 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("zzz")
				&sqlrcur,
				&id,
				&filename) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(id);
	convert_to_string_ex(filename);
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (cursor) {
		r=cursor->resumeCachedResultSet(LVAL(id),
						SVAL(filename));
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_closeresultset) {
	ZVAL sqlrcur;
	if (ZEND_NUM_ARGS() != 1 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("z")
				&sqlrcur) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrcursor *cursor=NULL;
	ZEND_FETCH_RESOURCE(cursor,
				sqlrcursor *,
				sqlrcur,
				-1,
				"sqlrelay cursor",
				sqlrelay_cursor);
	if (cursor) {
		cursor->closeResultSet();
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcon_ping) {
	ZVAL sqlrcon;
	bool r;
	if (ZEND_NUM_ARGS() != 1 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("z")
				&sqlrcon) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrconnection *connection=NULL;
	ZEND_FETCH_RESOURCE(connection,
				sqlrconnection *,
				sqlrcon,
				-1,
				"sqlrelay connection",
				sqlrelay_connection);
	if (connection) {
		r=connection->ping();
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcon_selectdatabase) {
	ZVAL sqlrcon;
	ZVAL database;
	bool r;
	if (ZEND_NUM_ARGS() != 2 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("zz")
				&sqlrcon,
				&database) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(database);
	sqlrconnection *connection=NULL;
	ZEND_FETCH_RESOURCE(connection,
				sqlrconnection *,
				sqlrcon,
				-1,
				"sqlrelay connection",
				sqlrelay_connection);
	if (connection) {
		r=connection->selectDatabase(SVAL(database));
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcon_getcurrentdatabase) {
	ZVAL sqlrcon;
	const char *r;
	if (ZEND_NUM_ARGS() != 1 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("z")
				&sqlrcon) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrconnection *connection=NULL;
	ZEND_FETCH_RESOURCE(connection,
				sqlrconnection *,
				sqlrcon,
				-1,
				"sqlrelay connection",
				sqlrelay_connection);
	if (connection) {
		r=connection->getCurrentDatabase();
		if (r) {
			RET_STRING(const_cast<char *>(r),1);
		}
	}
	RETURN_FALSE;
}

DLEXPORT ZEND_FUNCTION(sqlrcon_getlastinsertid) {
	ZVAL sqlrcon;
	if (ZEND_NUM_ARGS() != 1 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("z")
				&sqlrcon) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrconnection *connection=NULL;
	ZEND_FETCH_RESOURCE(connection,
				sqlrconnection *,
				sqlrcon,
				-1,
				"sqlrelay connection",
				sqlrelay_connection);
	if (connection) {
		RETURN_LONG(connection->getLastInsertId());
	}
	RETURN_FALSE;
}

DLEXPORT ZEND_FUNCTION(sqlrcon_autocommiton) {
	ZVAL sqlrcon;
	bool r;
	if (ZEND_NUM_ARGS() != 1 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("z")
				&sqlrcon) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrconnection *connection=NULL;
	ZEND_FETCH_RESOURCE(connection,
				sqlrconnection *,
				sqlrcon,
				-1,
				"sqlrelay connection",
				sqlrelay_connection);
	if (connection) {
		r=connection->autoCommitOn();
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcon_autocommitoff) {
	ZVAL sqlrcon;
	bool r;
	if (ZEND_NUM_ARGS() != 1 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("z")
				&sqlrcon) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrconnection *connection=NULL;
	ZEND_FETCH_RESOURCE(connection,
				sqlrconnection *,
				sqlrcon,
				-1,
				"sqlrelay connection",
				sqlrelay_connection);
	if (connection) {
		r=connection->autoCommitOff();
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcon_begin) {
	ZVAL sqlrcon;
	bool r;
	if (ZEND_NUM_ARGS() != 1 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("z")
				&sqlrcon) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrconnection *connection=NULL;
	ZEND_FETCH_RESOURCE(connection,
				sqlrconnection *,
				sqlrcon,
				-1,
				"sqlrelay connection",
				sqlrelay_connection);
	if (connection) {
		r=connection->begin();
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcon_commit) {
	ZVAL sqlrcon;
	bool r;
	if (ZEND_NUM_ARGS() != 1 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("z")
				&sqlrcon) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrconnection *connection=NULL;
	ZEND_FETCH_RESOURCE(connection,
				sqlrconnection *,
				sqlrcon,
				-1,
				"sqlrelay connection",
				sqlrelay_connection);
	if (connection) {
		r=connection->commit();
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcon_rollback) {
	ZVAL sqlrcon;
	bool r;
	if (ZEND_NUM_ARGS() != 1 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("z")
				&sqlrcon) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrconnection *connection=NULL;
	ZEND_FETCH_RESOURCE(connection,
				sqlrconnection *,
				sqlrcon,
				-1,
				"sqlrelay connection",
				sqlrelay_connection);
	if (connection) {
		r=connection->rollback();
		RETURN_LONG(r);
	}
	RETURN_LONG(0);
}

DLEXPORT ZEND_FUNCTION(sqlrcon_identify) {
	ZVAL sqlrcon;
	const char *r;
	if (ZEND_NUM_ARGS() != 1 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("z")
				&sqlrcon) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrconnection *connection=NULL;
	ZEND_FETCH_RESOURCE(connection,
				sqlrconnection *,
				sqlrcon,
				-1,
				"sqlrelay connection",
				sqlrelay_connection);
	if (connection) {
		r=connection->identify();
		if (r) {
			RET_STRING(const_cast<char *>(r),1);
		}
	}
	RETURN_FALSE;
}

DLEXPORT ZEND_FUNCTION(sqlrcon_bindformat) {
	ZVAL sqlrcon;
	const char *r;
	if (ZEND_NUM_ARGS() != 1 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("z")
				&sqlrcon) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrconnection *connection=NULL;
	ZEND_FETCH_RESOURCE(connection,
				sqlrconnection *,
				sqlrcon,
				-1,
				"sqlrelay connection",
				sqlrelay_connection);
	if (connection) {
		r=connection->bindFormat();
		if (r) {
			RET_STRING(const_cast<char *>(r),1);
		}
	}
	RETURN_FALSE;
}

DLEXPORT ZEND_FUNCTION(sqlrcon_dbversion) {
	ZVAL sqlrcon;
	const char *r;
	if (ZEND_NUM_ARGS() != 1 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("z")
				&sqlrcon) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrconnection *connection=NULL;
	ZEND_FETCH_RESOURCE(connection,
				sqlrconnection *,
				sqlrcon,
				-1,
				"sqlrelay connection",
				sqlrelay_connection);
	if (connection) {
		r=connection->dbVersion();
		if (r) {
			RET_STRING(const_cast<char *>(r),1);
		}
	}
	RETURN_FALSE;
}


DLEXPORT ZEND_FUNCTION(sqlrcon_dbhostname) {
	ZVAL sqlrcon;
	const char *r;
	if (ZEND_NUM_ARGS() != 1 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("z")
				&sqlrcon) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrconnection *connection=NULL;
	ZEND_FETCH_RESOURCE(connection,
				sqlrconnection *,
				sqlrcon,
				-1,
				"sqlrelay connection",
				sqlrelay_connection);
	if (connection) {
		r=connection->dbHostName();
		if (r) {
			RET_STRING(const_cast<char *>(r),1);
		}
	}
	RETURN_FALSE;
}


DLEXPORT ZEND_FUNCTION(sqlrcon_dbipaddress) {
	ZVAL sqlrcon;
	const char *r;
	if (ZEND_NUM_ARGS() != 1 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("z")
				&sqlrcon) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrconnection *connection=NULL;
	ZEND_FETCH_RESOURCE(connection,
				sqlrconnection *,
				sqlrcon,
				-1,
				"sqlrelay connection",
				sqlrelay_connection);
	if (connection) {
		r=connection->dbIpAddress();
		if (r) {
			RET_STRING(const_cast<char *>(r),1);
		}
	}
	RETURN_FALSE;
}

DLEXPORT ZEND_FUNCTION(sqlrcon_serverversion) {
	ZVAL sqlrcon;
	const char *r;
	if (ZEND_NUM_ARGS() != 1 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("z")
				&sqlrcon) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrconnection *connection=NULL;
	ZEND_FETCH_RESOURCE(connection,
				sqlrconnection *,
				sqlrcon,
				-1,
				"sqlrelay connection",
				sqlrelay_connection);
	if (connection) {
		r=connection->serverVersion();
		if (r) {
			RET_STRING(const_cast<char *>(r),1);
		}
	}
	RETURN_FALSE;
}

DLEXPORT ZEND_FUNCTION(sqlrcon_clientversion) {
	ZVAL sqlrcon;
	const char *r;
	if (ZEND_NUM_ARGS() != 1 || 
		GET_PARAMETERS(
				ZEND_NUM_ARGS() TSRMLS_CC,
				PARAMS("z")
				&sqlrcon) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sqlrconnection *connection=NULL;
	ZEND_FETCH_RESOURCE(connection,
				sqlrconnection *,
				sqlrcon,
				-1,
				"sqlrelay connection",
				sqlrelay_connection);
	if (connection) {
		r=connection->clientVersion();
		if (r) {
			RET_STRING(const_cast<char *>(r),1);
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
	ZEND_FE(sqlrcon_setbindvariabledelimiters,NULL)
	ZEND_FE(sqlrcon_getbindvariabledelimiterquestionmarksupported,NULL)
	ZEND_FE(sqlrcon_getbindvariabledelimitercolonsupported,NULL)
	ZEND_FE(sqlrcon_getbindvariabledelimiteratsignsupported,NULL)
	ZEND_FE(sqlrcon_getbindvariabledelimiterdollarsignsupported,NULL)
	ZEND_FE(sqlrcon_enablekerberos,NULL)
	ZEND_FE(sqlrcon_enabletls,NULL)
	ZEND_FE(sqlrcon_disableencryption,NULL)
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

ZEND_GET_MODULE(sql_relay)

}
