// Copyright (c) 1999-2001  David Muse
// See the COPYING file for more information.

#include <rudiments/dynamicarray.h>
#include <rudiments/stringbuffer.h>
#include <rudiments/socketclient.h>

#ifdef _WIN32
	#ifdef LIBSQLRCLIENT_EXPORTS
		#define SQLRCLIENT_DLLSPEC __declspec(dllexport)
	#else
		#define SQLRCLIENT_DLLSPEC __declspec(dllimport)
	#endif
#else
	#define SQLRCLIENT_DLLSPEC
#endif

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

class bindvar {
	friend class sqlrcursor;
	private:
		char	*variable;
		union {
			char	*stringval;
			int64_t	integerval;
			struct {
				double		value;
				uint32_t	precision;
				uint32_t	scale;
			} doubleval;
			struct {
				int16_t	year;
				int16_t	month;
				int16_t	day;
				int16_t	hour;
				int16_t	minute;
				int16_t	second;
				int32_t	microsecond;
				char	*tz;
			} dateval;
			char		*lobval;
			uint16_t	cursorid;
		} value;
		uint32_t	valuesize;
		uint32_t	resultvaluesize;
		bindvartype_t 	type;
		bool		send;

		bool		substituted;
		bool		donesubstituting;
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
