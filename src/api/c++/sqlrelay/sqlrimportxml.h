// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#ifndef SQLRIMPORTXML_H
#define SQLRIMPORTXML_H

#include <sqlrelay/private/sqlrimportxmlincludes.h>

class sqlrimportxml : public sqlrimport, public xmlsax {
	public:
			sqlrimportxml();
			~sqlrimportxml();

		bool	importFromFile(const char *filename);

	#include <sqlrelay/private/sqlrimportxml.h>
};

#endif
