// Copyright (c) 1999-2001  David Muse
// See the COPYING file for more information.

#ifndef ROW_H
#define ROW_H

#include <stdint.h>

class row {
	friend class sqlrcursor;
	private:
			row(uint32_t colcount);
			~row();
		void	resize(uint32_t colcount);
		void	addField(uint32_t column, 
				const char *buffer, uint32_t length);

		char		*getField(uint32_t column) const;
		uint32_t	getFieldLength(uint32_t column) const;

		row	*next;

		char		*fields[OPTIMISTIC_COLUMN_COUNT];
		uint32_t	fieldlengths[OPTIMISTIC_COLUMN_COUNT];
		char		**extrafields;
		uint32_t	*extrafieldlengths;

		uint32_t	colcount;
};

#endif
