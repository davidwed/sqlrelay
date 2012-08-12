// Copyright (c) 1999-2001  David Muse
// See the COPYING file for more information.

#ifndef BINDVAR_H
#define BINDVAR_H

#include <sqlrelay/private/dll.h>

enum bindtype {
	NULL_BIND,
	STRING_BIND,
	INTEGER_BIND,
	DOUBLE_BIND,
	BLOB_BIND,
	CLOB_BIND,
	CURSOR_BIND,
	DATE_BIND
};

class SQLRCLIENT_DLLSPEC bindvar {
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
		bindtype	type;
		bool		send;

		bool		substituted;
		bool		donesubstituting;
};

#endif
