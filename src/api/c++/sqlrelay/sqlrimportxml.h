// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#ifndef SQLRIMPORTXML_H
#define SQLRIMPORTXML_H

#include <sqlrelay/private/sqlrimportxmlincludes.h>

class sqlrimportxml : public xmlsax {
	public:
			sqlrimportxml(sqlrconnection *sqlrcon,
					sqlrcursor *sqlrcur,
					uint64_t commitcount,
					bool verbose,
					const char *dbtype);
			~sqlrimportxml();

	#include <sqlrelay/private/sqlrimportxml.h>
};

#endif
