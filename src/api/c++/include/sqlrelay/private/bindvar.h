// Copyright (c) 1999-2001  David Muse
// See the COPYING file for more information.

#ifndef BINDVAR_H
#define BINDVAR_H

enum bindtype {
	NULL_BIND,
	STRING_BIND,
	LONG_BIND,
	DOUBLE_BIND,
	BLOB_BIND,
	CLOB_BIND,
	CURSOR_BIND
};

class bindvar {
	friend class sqlrcursor;
	private:
		char		*variable;
		union {
			char	*stringval;
			long	longval;
			struct {
				double	value;
				unsigned short	precision;
				unsigned short	scale;
			} doubleval;
			char	*lobval;
			short	cursorid;
		} value;
		unsigned short	valuesize;
		bindtype	type;
		unsigned short	send;
		
};

#endif
