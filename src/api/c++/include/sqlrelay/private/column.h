// Copyright (c) 1999-2001  David Muse
// See the COPYING file for more information.

#ifndef COLUMN_H
#define COLUMN_H

class column {
	public:
		char		*name;
		uint16_t	type;
		char		*typestring;
		uint16_t	typestringlength;
		uint32_t	length;
		uint32_t	longest;
		unsigned char	longdatatype;
		uint32_t	precision;
		uint32_t	scale;
		uint16_t	nullable;
		uint16_t	primarykey;
		uint16_t	unique;
		uint16_t	partofkey;
		uint16_t	unsignednumber;
		uint16_t	zerofill;
		uint16_t	binary;
		uint16_t	autoincrement;
};

#endif
