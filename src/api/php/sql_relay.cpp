/* Copyright (c) 2000  Adam Kropielnicki
   See the file COPYING for more information */

// Some versions of PHP 7.0 need INT64_MIN/MAX and (U)INT*_C to be defined.
// These gyrations are necessary when using C++.
#include <rudiments/private/config.h>
#ifdef RUDIMENTS_HAVE_STDINT_H
	#define __STDC_LIMIT_MACROS
	#define __STDC_CONSTANT_MACROS
	#include <stdint.h>
#endif

#include <config.h>

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
			#if PHPMAJORVERSION < 8
				#undef __cplusplus
				#define cpluspluswasdefined
			#endif
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
	// On some versions of Mac OS X (10.4), php.h ultimately includes
	// dyld.h, but somehow _Bool avoids getting defined by stdbool.h
	// Define it here.
	#ifdef RUDIMENTS_HAVE_MACH_O_DYLD_H
		typedef int     _Bool;
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
		RETURN_STR(zend_string_init(a,charstring::getLength(a),0))
	#define RET_STRINGL(a,b,c) \
		RETURN_STR(zend_string_init(a,b,0))


	#define ADD_ASSOC_STRINGL(a,b,c,d,e) \
		add_assoc_stringl(a,b,zend_string_init(c,d,0)->val,d)
	#define ADD_NEXT_INDEX_STRING(a,b,c) \
		add_next_index_string( \
			a,zend_string_init(b,charstring::getLength(b),0)->val)
	#define ADD_NEXT_INDEX_STRINGL(a,b,c,d) \
		add_next_index_stringl(a,zend_string_init(b,c,0)->val,c)

	#define HASH_INDEX_FIND(a,b,c) c=zend_hash_index_find(a,b)

	// for 7.2 and greater...
	#if PHP_MAJOR_VERSION > 7 || PHP_MINOR_VERSION > 2
		#define ARRAY_INIT_CANT_FAIL 1
	#endif

	// for earlier than 7.4...
	#if PHP_MAJOR_VERSION == 7 && PHP_MINOR_VERSION < 4
		#define ADD_ASSOC_NULL(a,b) add_assoc_unset(a,b)
		#define	ADD_NEXT_INDEX_NULL(a) add_next_index_unset(a)
	#else
		#define ADD_ASSOC_NULL(a,b) add_assoc_null(a,b)
		#define	ADD_NEXT_INDEX_NULL(a) add_next_index_null(a)
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
	#define ADD_ASSOC_NULL(a,b) add_assoc_unset(a,b)
	#define ADD_NEXT_INDEX_STRING(a,b,c) add_next_index_string(a,b,c)
	#define ADD_NEXT_INDEX_STRINGL(a,b,c,d) add_next_index_stringl(a,b,c,d)
	#define	ADD_NEXT_INDEX_NULL(a) add_next_index_unset(a)

	#define HASH_INDEX_FIND(a,b,c) zend_hash_index_find(a,b,(void **)&c)
#endif

#if PHP_MAJOR_VERSION >= 5
	#define ARGINFO(a) a
#else
	#define ARGINFO(a) NULL
	#define ZEND_BEGIN_ARG_INFO_EX(a,b,c,d)
	#define ZEND_END_ARG_INFO()
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
			ADD_NEXT_INDEX_NULL(return_value);
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
			ADD_ASSOC_NULL(return_value,const_cast<char *>(rC[i]));
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

// FIXME: flesh these out
ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcon_alloc,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcon_free,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcon_setconnecttimeout,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcon_setresponsetimeout,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcon_setbindvariabledelimiters,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcon_getbindvariabledelimiterquestionmarksupported,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcon_getbindvariabledelimitercolonsupported,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcon_getbindvariabledelimiteratsignsupported,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcon_getbindvariabledelimiterdollarsignsupported,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcon_enablekerberos,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcon_enabletls,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcon_disableencryption,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcon_endsession,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcon_suspendsession,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcon_getconnectionport,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcon_getconnectionsocket,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcon_resumesession,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcon_errormessage,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcon_errornumber,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcon_debugon,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcon_debugoff,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcon_getdebug,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcon_setdebugfile,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcon_setclientinfo,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcon_getclientinfo,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_alloc,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_free,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_setresultsetbuffersize,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_getresultsetbuffersize,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_dontgetcolumninfo,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_getcolumninfo,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_mixedcasecolumnnames,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_uppercasecolumnnames,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_lowercasecolumnnames,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_cachetofile,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_setcachettl,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_getcachefilename,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_cacheoff,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_getdatabaselist,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_gettablelist,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_getcolumnlist,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_sendquery,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_sendquerywithlength,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_sendfilequery,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_preparequery,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_preparequerywithlength,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_preparefilequery,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_substitution,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_clearbinds,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_countbindvariables,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_inputbind,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_inputbindblob,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_inputbindclob,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_defineoutputbindstring,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_defineoutputbindinteger,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_defineoutputbinddouble,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_defineoutputbindblob,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_defineoutputbindclob,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_defineoutputbindcursor,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_substitutions,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_inputbinds,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_validatebinds,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_validbind,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_executequery,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_fetchfrombindcursor,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_getoutputbindstring,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_getoutputbindblob,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_getoutputbindclob,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_getoutputbindinteger,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_getoutputbinddouble,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_getoutputbindlength,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_getoutputbindcursor,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_opencachedresultset,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_colcount,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_rowcount,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_totalrows,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_affectedrows,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_firstrowindex,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_endofresultset,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_errormessage,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_errornumber,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_getnullsasemptystrings,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_getnullsasnulls,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_getfield,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_getfieldasinteger,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_getfieldasdouble,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_getfieldlength,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_getrow,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_getrowassoc,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_getrowlengths,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_getrowlengthsassoc,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_getcolumnnames,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_getcolumnname,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_getcolumntype,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_getcolumnlength,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_getcolumnprecision,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_getcolumnscale,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_getcolumnisnullable,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_getcolumnisprimarykey,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_getcolumnisunique,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_getcolumnispartofkey,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_getcolumnisunsigned,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_getcolumniszerofilled,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_getcolumnisbinary,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_getcolumnisautoincrement,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_getlongest,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_getresultsetid,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_suspendresultset,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_resumeresultset,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcur_resumecachedresultset,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcon_ping,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcon_identify,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcon_selectdatabase,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcon_getcurrentdatabase,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcon_getlastinsertid,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcon_autocommiton,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcon_autocommitoff,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcon_begin,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcon_commit,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcon_rollback,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcon_bindformat,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcon_dbversion,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcon_dbhostname,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcon_dbipaddress,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcon_serverversion,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlrcon_clientversion,0,0,0)
ZEND_END_ARG_INFO()


zend_function_entry sql_relay_functions[] = {
	ZEND_FE(sqlrcon_alloc,
		ARGINFO(arginfo_sqlrcon_alloc))
	ZEND_FE(sqlrcon_free,
		ARGINFO(arginfo_sqlrcon_free))
	ZEND_FE(sqlrcon_setconnecttimeout,
		ARGINFO(arginfo_sqlrcon_setconnecttimeout))
	ZEND_FE(sqlrcon_setresponsetimeout,
		ARGINFO(arginfo_sqlrcon_setresponsetimeout))
	ZEND_FE(sqlrcon_setbindvariabledelimiters,
		ARGINFO(arginfo_sqlrcon_setbindvariabledelimiters))
	ZEND_FE(sqlrcon_getbindvariabledelimiterquestionmarksupported,
	ARGINFO(arginfo_sqlrcon_getbindvariabledelimiterquestionmarksupported))
	ZEND_FE(sqlrcon_getbindvariabledelimitercolonsupported,
	ARGINFO(arginfo_sqlrcon_getbindvariabledelimitercolonsupported))
	ZEND_FE(sqlrcon_getbindvariabledelimiteratsignsupported,
	ARGINFO(arginfo_sqlrcon_getbindvariabledelimiteratsignsupported))
	ZEND_FE(sqlrcon_getbindvariabledelimiterdollarsignsupported,
	ARGINFO(arginfo_sqlrcon_getbindvariabledelimiterdollarsignsupported))
	ZEND_FE(sqlrcon_enablekerberos,
		ARGINFO(arginfo_sqlrcon_enablekerberos))
	ZEND_FE(sqlrcon_enabletls,
		ARGINFO(arginfo_sqlrcon_enabletls))
	ZEND_FE(sqlrcon_disableencryption,
		ARGINFO(arginfo_sqlrcon_disableencryption))
	ZEND_FE(sqlrcon_endsession,
		ARGINFO(arginfo_sqlrcon_endsession))
	ZEND_FE(sqlrcon_suspendsession,
		ARGINFO(arginfo_sqlrcon_suspendsession))
	ZEND_FE(sqlrcon_getconnectionport,
		ARGINFO(arginfo_sqlrcon_getconnectionport))
	ZEND_FE(sqlrcon_getconnectionsocket,
		ARGINFO(arginfo_sqlrcon_getconnectionsocket))
	ZEND_FE(sqlrcon_resumesession,
		ARGINFO(arginfo_sqlrcon_resumesession))
	ZEND_FE(sqlrcon_errormessage,
		ARGINFO(arginfo_sqlrcon_errormessage))
	ZEND_FE(sqlrcon_errornumber,
		ARGINFO(arginfo_sqlrcon_errornumber))
	ZEND_FE(sqlrcon_debugon,
		ARGINFO(arginfo_sqlrcon_debugon))
	ZEND_FE(sqlrcon_debugoff,
		ARGINFO(arginfo_sqlrcon_debugoff))
	ZEND_FE(sqlrcon_getdebug,
		ARGINFO(arginfo_sqlrcon_getdebug))
	ZEND_FE(sqlrcon_setdebugfile,
		ARGINFO(arginfo_sqlrcon_setdebugfile))
	ZEND_FE(sqlrcon_setclientinfo,
		ARGINFO(arginfo_sqlrcon_setclientinfo))
	ZEND_FE(sqlrcon_getclientinfo,
		ARGINFO(arginfo_sqlrcon_getclientinfo))
	ZEND_FE(sqlrcur_alloc,
		ARGINFO(arginfo_sqlrcur_alloc))
	ZEND_FE(sqlrcur_free,
		ARGINFO(arginfo_sqlrcur_free))
	ZEND_FE(sqlrcur_setresultsetbuffersize,
		ARGINFO(arginfo_sqlrcur_setresultsetbuffersize))
	ZEND_FE(sqlrcur_getresultsetbuffersize,
		ARGINFO(arginfo_sqlrcur_getresultsetbuffersize))
	ZEND_FE(sqlrcur_dontgetcolumninfo,
		ARGINFO(arginfo_sqlrcur_dontgetcolumninfo))
	ZEND_FE(sqlrcur_getcolumninfo,
		ARGINFO(arginfo_sqlrcur_getcolumninfo))
	ZEND_FE(sqlrcur_mixedcasecolumnnames,
		ARGINFO(arginfo_sqlrcur_mixedcasecolumnnames))
	ZEND_FE(sqlrcur_uppercasecolumnnames,
		ARGINFO(arginfo_sqlrcur_uppercasecolumnnames))
	ZEND_FE(sqlrcur_lowercasecolumnnames,
		ARGINFO(arginfo_sqlrcur_lowercasecolumnnames))
	ZEND_FE(sqlrcur_cachetofile,
		ARGINFO(arginfo_sqlrcur_cachetofile))
	ZEND_FE(sqlrcur_setcachettl,
		ARGINFO(arginfo_sqlrcur_setcachettl))
	ZEND_FE(sqlrcur_getcachefilename,
		ARGINFO(arginfo_sqlrcur_getcachefilename))
	ZEND_FE(sqlrcur_cacheoff,
		ARGINFO(arginfo_sqlrcur_cacheoff))
	ZEND_FE(sqlrcur_getdatabaselist,
		ARGINFO(arginfo_sqlrcur_getdatabaselist))
	ZEND_FE(sqlrcur_gettablelist,
		ARGINFO(arginfo_sqlrcur_gettablelist))
	ZEND_FE(sqlrcur_getcolumnlist,
		ARGINFO(arginfo_sqlrcur_getcolumnlist))
	ZEND_FE(sqlrcur_sendquery,
		ARGINFO(arginfo_sqlrcur_sendquery))
	ZEND_FE(sqlrcur_sendquerywithlength,
		ARGINFO(arginfo_sqlrcur_sendquerywithlength))
	ZEND_FE(sqlrcur_sendfilequery,
		ARGINFO(arginfo_sqlrcur_sendfilequery))
	ZEND_FE(sqlrcur_preparequery,
		ARGINFO(arginfo_sqlrcur_preparequery))
	ZEND_FE(sqlrcur_preparequerywithlength,
		ARGINFO(arginfo_sqlrcur_preparequerywithlength))
	ZEND_FE(sqlrcur_preparefilequery,
		ARGINFO(arginfo_sqlrcur_preparefilequery))
	ZEND_FE(sqlrcur_substitution,
		ARGINFO(arginfo_sqlrcur_substitution))
	ZEND_FE(sqlrcur_clearbinds,
		ARGINFO(arginfo_sqlrcur_clearbinds))
	ZEND_FE(sqlrcur_countbindvariables,
		ARGINFO(arginfo_sqlrcur_countbindvariables))
	ZEND_FE(sqlrcur_inputbind,
		ARGINFO(arginfo_sqlrcur_inputbind))
	ZEND_FE(sqlrcur_inputbindblob,
		ARGINFO(arginfo_sqlrcur_inputbindblob))
	ZEND_FE(sqlrcur_inputbindclob,
		ARGINFO(arginfo_sqlrcur_inputbindclob))
	ZEND_FE(sqlrcur_defineoutputbindstring,
		ARGINFO(arginfo_sqlrcur_defineoutputbindstring))
	ZEND_FE(sqlrcur_defineoutputbindinteger,
		ARGINFO(arginfo_sqlrcur_defineoutputbindinteger))
	ZEND_FE(sqlrcur_defineoutputbinddouble,
		ARGINFO(arginfo_sqlrcur_defineoutputbinddouble))
	ZEND_FE(sqlrcur_defineoutputbindblob,
		ARGINFO(arginfo_sqlrcur_defineoutputbindblob))
	ZEND_FE(sqlrcur_defineoutputbindclob,
		ARGINFO(arginfo_sqlrcur_defineoutputbindclob))
	ZEND_FE(sqlrcur_defineoutputbindcursor,
		ARGINFO(arginfo_sqlrcur_defineoutputbindcursor))
	ZEND_FE(sqlrcur_substitutions,
		ARGINFO(arginfo_sqlrcur_substitutions))
	ZEND_FE(sqlrcur_inputbinds,
		ARGINFO(arginfo_sqlrcur_inputbinds))
	ZEND_FE(sqlrcur_validatebinds,
		ARGINFO(arginfo_sqlrcur_validatebinds))
	ZEND_FE(sqlrcur_validbind,
		ARGINFO(arginfo_sqlrcur_validbind))
	ZEND_FE(sqlrcur_executequery,
		ARGINFO(arginfo_sqlrcur_executequery))
	ZEND_FE(sqlrcur_fetchfrombindcursor,
		ARGINFO(arginfo_sqlrcur_fetchfrombindcursor))
	ZEND_FE(sqlrcur_getoutputbindstring,
		ARGINFO(arginfo_sqlrcur_getoutputbindstring))
	ZEND_FE(sqlrcur_getoutputbindblob,
		ARGINFO(arginfo_sqlrcur_getoutputbindblob))
	ZEND_FE(sqlrcur_getoutputbindclob,
		ARGINFO(arginfo_sqlrcur_getoutputbindclob))
	ZEND_FE(sqlrcur_getoutputbindinteger,
		ARGINFO(arginfo_sqlrcur_getoutputbindinteger))
	ZEND_FE(sqlrcur_getoutputbinddouble,
		ARGINFO(arginfo_sqlrcur_getoutputbinddouble))
	ZEND_FE(sqlrcur_getoutputbindlength,
		ARGINFO(arginfo_sqlrcur_getoutputbindlength))
	ZEND_FE(sqlrcur_getoutputbindcursor,
		ARGINFO(arginfo_sqlrcur_getoutputbindcursor))
	ZEND_FE(sqlrcur_opencachedresultset,
		ARGINFO(arginfo_sqlrcur_opencachedresultset))
	ZEND_FE(sqlrcur_colcount,
		ARGINFO(arginfo_sqlrcur_colcount))
	ZEND_FE(sqlrcur_rowcount,
		ARGINFO(arginfo_sqlrcur_rowcount))
	ZEND_FE(sqlrcur_totalrows,
		ARGINFO(arginfo_sqlrcur_totalrows))
	ZEND_FE(sqlrcur_affectedrows,
		ARGINFO(arginfo_sqlrcur_affectedrows))
	ZEND_FE(sqlrcur_firstrowindex,
		ARGINFO(arginfo_sqlrcur_firstrowindex))
	ZEND_FE(sqlrcur_endofresultset,
		ARGINFO(arginfo_sqlrcur_endofresultset))
	ZEND_FE(sqlrcur_errormessage,
		ARGINFO(arginfo_sqlrcur_errormessage))
	ZEND_FE(sqlrcur_errornumber,
		ARGINFO(arginfo_sqlrcur_errornumber))
	ZEND_FE(sqlrcur_getnullsasemptystrings,
		ARGINFO(arginfo_sqlrcur_getnullsasemptystrings))
	ZEND_FE(sqlrcur_getnullsasnulls,
		ARGINFO(arginfo_sqlrcur_getnullsasnulls))
	ZEND_FE(sqlrcur_getfield,
		ARGINFO(arginfo_sqlrcur_getfield))
	ZEND_FE(sqlrcur_getfieldasinteger,
		ARGINFO(arginfo_sqlrcur_getfieldasinteger))
	ZEND_FE(sqlrcur_getfieldasdouble,
		ARGINFO(arginfo_sqlrcur_getfieldasdouble))
	ZEND_FE(sqlrcur_getfieldlength,
		ARGINFO(arginfo_sqlrcur_getfieldlength))
	ZEND_FE(sqlrcur_getrow,
		ARGINFO(arginfo_sqlrcur_getrow))
	ZEND_FE(sqlrcur_getrowassoc,
		ARGINFO(arginfo_sqlrcur_getrowassoc))
	ZEND_FE(sqlrcur_getrowlengths,
		ARGINFO(arginfo_sqlrcur_getrowlengths))
	ZEND_FE(sqlrcur_getrowlengthsassoc,
		ARGINFO(arginfo_sqlrcur_getrowlengthsassoc))
	ZEND_FE(sqlrcur_getcolumnnames,
		ARGINFO(arginfo_sqlrcur_getcolumnnames))
	ZEND_FE(sqlrcur_getcolumnname,
		ARGINFO(arginfo_sqlrcur_getcolumnname))
	ZEND_FE(sqlrcur_getcolumntype,
		ARGINFO(arginfo_sqlrcur_getcolumntype))
	ZEND_FE(sqlrcur_getcolumnlength,
		ARGINFO(arginfo_sqlrcur_getcolumnlength))
	ZEND_FE(sqlrcur_getcolumnprecision,
		ARGINFO(arginfo_sqlrcur_getcolumnprecision))
	ZEND_FE(sqlrcur_getcolumnscale,
		ARGINFO(arginfo_sqlrcur_getcolumnscale))
	ZEND_FE(sqlrcur_getcolumnisnullable,
		ARGINFO(arginfo_sqlrcur_getcolumnisnullable))
	ZEND_FE(sqlrcur_getcolumnisprimarykey,
		ARGINFO(arginfo_sqlrcur_getcolumnisprimarykey))
	ZEND_FE(sqlrcur_getcolumnisunique,
		ARGINFO(arginfo_sqlrcur_getcolumnisunique))
	ZEND_FE(sqlrcur_getcolumnispartofkey,
		ARGINFO(arginfo_sqlrcur_getcolumnispartofkey))
	ZEND_FE(sqlrcur_getcolumnisunsigned,
		ARGINFO(arginfo_sqlrcur_getcolumnisunsigned))
	ZEND_FE(sqlrcur_getcolumniszerofilled,
		ARGINFO(arginfo_sqlrcur_getcolumniszerofilled))
	ZEND_FE(sqlrcur_getcolumnisbinary,
		ARGINFO(arginfo_sqlrcur_getcolumnisbinary))
	ZEND_FE(sqlrcur_getcolumnisautoincrement,
		ARGINFO(arginfo_sqlrcur_getcolumnisautoincrement))
	ZEND_FE(sqlrcur_getlongest,
		ARGINFO(arginfo_sqlrcur_getlongest))
	ZEND_FE(sqlrcur_getresultsetid,
		ARGINFO(arginfo_sqlrcur_getresultsetid))
	ZEND_FE(sqlrcur_suspendresultset,
		ARGINFO(arginfo_sqlrcur_suspendresultset))
	ZEND_FE(sqlrcur_resumeresultset,
		ARGINFO(arginfo_sqlrcur_resumeresultset))
	ZEND_FE(sqlrcur_resumecachedresultset,
		ARGINFO(arginfo_sqlrcur_resumecachedresultset))
	ZEND_FE(sqlrcon_ping,
		ARGINFO(arginfo_sqlrcon_ping))
	ZEND_FE(sqlrcon_identify,
		ARGINFO(arginfo_sqlrcon_identify))
	ZEND_FE(sqlrcon_selectdatabase,
		ARGINFO(arginfo_sqlrcon_selectdatabase))
	ZEND_FE(sqlrcon_getcurrentdatabase,
		ARGINFO(arginfo_sqlrcon_getcurrentdatabase))
	ZEND_FE(sqlrcon_getlastinsertid,
		ARGINFO(arginfo_sqlrcon_getlastinsertid))
	ZEND_FE(sqlrcon_autocommiton,
		ARGINFO(arginfo_sqlrcon_autocommiton))
	ZEND_FE(sqlrcon_autocommitoff,
		ARGINFO(arginfo_sqlrcon_autocommitoff))
	ZEND_FE(sqlrcon_begin,
		ARGINFO(arginfo_sqlrcon_begin))
	ZEND_FE(sqlrcon_commit,
		ARGINFO(arginfo_sqlrcon_commit))
	ZEND_FE(sqlrcon_rollback,
		ARGINFO(arginfo_sqlrcon_rollback))
	ZEND_FE(sqlrcon_bindformat,
		ARGINFO(arginfo_sqlrcon_bindformat))
	ZEND_FE(sqlrcon_dbversion,
		ARGINFO(arginfo_sqlrcon_dbversion))
	ZEND_FE(sqlrcon_dbhostname,
		ARGINFO(arginfo_sqlrcon_dbhostname))
	ZEND_FE(sqlrcon_dbipaddress,
		ARGINFO(arginfo_sqlrcon_dbipaddress))
	ZEND_FE(sqlrcon_serverversion,
		ARGINFO(arginfo_sqlrcon_serverversion))
	ZEND_FE(sqlrcon_clientversion,
		ARGINFO(arginfo_sqlrcon_clientversion))
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
