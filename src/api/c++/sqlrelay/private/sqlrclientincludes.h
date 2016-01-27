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

// older compilers require sqlrclientbindvar to be defined here because of the
// reference to dynamicvar<sqlrclientbindvar> in private/sqlrcursor.h
class sqlrclientbindvar {
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

		sqlrclientbindvartype_t 	type;

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
class sqlrcursor;
class sqlrcursorprivate;
class sqlrclientcolumn;
