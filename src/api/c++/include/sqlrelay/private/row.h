// Copyright (c) 1999-2001  David Muse
// See the COPYING file for more information.

#ifndef ROW_H
#define ROW_H

class row {
	friend class sqlrcursor;
	private:
			row(int colcount);
			~row();
		void	resize(int colcount);
		void	addField(int column, 
				const char *buffer, unsigned long length);

		char		*getField(int column) const;
		unsigned long	getFieldLength(int column) const;

		row	*next;

		char		*fields[OPTIMISTIC_COLUMN_COUNT];
		unsigned long	fieldlengths[OPTIMISTIC_COLUMN_COUNT];
		char		**extrafields;
		unsigned long	*extrafieldlengths;

		int	colcount;
};

#endif
