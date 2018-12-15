// Copyright (c) 1999-2018 David Muse
// See the COPYING file for more information.

#include <rudiments/dynamicarray.h>
#include <rudiments/stringbuffer.h>
#include <rudiments/socketclient.h>

#ifndef SQLRCLIENT_DLLSPEC
	#ifdef _WIN32
		#ifdef SQLRCLIENT_EXPORTS
			#define SQLRCLIENT_DLLSPEC __declspec(dllexport)
		#else
			#define SQLRCLIENT_DLLSPEC __declspec(dllimport)
		#endif
	#else
		#define SQLRCLIENT_DLLSPEC
	#endif
#endif

enum sqlrclientbindvartype_t {
	SQLRCLIENTBINDVARTYPE_NULL=0,
	SQLRCLIENTBINDVARTYPE_STRING,
	SQLRCLIENTBINDVARTYPE_INTEGER,
	SQLRCLIENTBINDVARTYPE_DOUBLE,
	SQLRCLIENTBINDVARTYPE_BLOB,
	SQLRCLIENTBINDVARTYPE_CLOB,
	SQLRCLIENTBINDVARTYPE_CURSOR,
	SQLRCLIENTBINDVARTYPE_DATE
};

enum sqlrclientlistformat_t {
	SQLRCLIENTLISTFORMAT_NULL=0,
	SQLRCLIENTLISTFORMAT_MYSQL,
	SQLRCLIENTLISTFORMAT_ODBC
};

class sqlrconnectionprivate;
class sqlrcursor;
class sqlrcursorprivate;
class sqlrclientcolumn;
class sqlrclientbindvar;
