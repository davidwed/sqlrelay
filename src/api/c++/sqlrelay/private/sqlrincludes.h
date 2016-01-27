// Copyright (c) 1999-2001  David Muse
// See the COPYING file for more information.


#include <rudiments/inetsocketclient.h>
#include <rudiments/unixsocketclient.h>
#include <rudiments/dynamicarray.h>
#include <rudiments/memorypool.h>
#include <rudiments/stringbuffer.h>
#include <rudiments/file.h>

#include <sqlrelay/private/dll.h>

enum bindvartype_t {
	BINDVARTYPE_NULL=0,
	BINDVARTYPE_STRING,
	BINDVARTYPE_INTEGER,
	BINDVARTYPE_DOUBLE,
	BINDVARTYPE_BLOB,
	BINDVARTYPE_CLOB,
	BINDVARTYPE_CURSOR,
	BINDVARTYPE_DATE
};

enum sqlrclientlistformat_t {
	SQLRCLIENTLISTFORMAT_NULL=0,
	SQLRCLIENTLISTFORMAT_MYSQL,
	SQLRCLIENTLISTFORMAT_ODBC
};

class sqlrconnectionprivate;
class sqlrcursorprivate;
class column;
class bindvar;
class sqlrcursor;
