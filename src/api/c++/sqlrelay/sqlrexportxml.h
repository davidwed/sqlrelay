// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#ifndef SQLREXPORTXML_H
#define SQLREXPORTXML_H

#include <sqlrelay/private/sqlrexportxmlincludes.h>

class SQLRCLIENT_DLLSPEC sqlrexportxml : public sqlrexport {
	public:
			sqlrexportxml();
			~sqlrexportxml();

		bool	exportToFile(const char *filename, const char *table);

	#include <sqlrelay/private/sqlrexportxml.h>
};

#endif
