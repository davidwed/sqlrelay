// Copyright (c) 1999-2001  David Muse
// See the COPYING file for more information.

#ifndef COLUMN_H
#define COLUMN_H

class column {
	friend class sqlrcursor;
	private:
		char		*name;
		unsigned short	type;
		unsigned long	length;
		int		longest;
		unsigned char	longdatatype;
};

#endif
