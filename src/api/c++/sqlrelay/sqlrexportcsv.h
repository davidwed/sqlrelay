// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#ifndef SQLREXPORTCSV_H
#define SQLREXPORTCSV_H

#include <sqlrelay/private/sqlrexportcsvincludes.h>

class SQLRCLIENT_DLLSPEC sqlrexportcsv : public sqlrexport {
	public:
			sqlrexportcsv();
			~sqlrexportcsv();

		bool	exportToFile(const char *filename, const char *table);

	#include <sqlrelay/private/sqlrexportcsv.h>
};

#endif
