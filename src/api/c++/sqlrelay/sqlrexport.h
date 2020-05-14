// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#ifndef SQLREXPORT_H
#define SQLREXPORT_H

#include <sqlrelay/private/sqlrexportincludes.h>

class sqlrexport {
	public:
			sqlrexport();
		virtual	~sqlrexport();

		void	setSqlrCursor(sqlrcursor *sqlrcur);

		void	setIgnoreColumns(bool ignorecolumns);

		void	setLogger(logger *lg);
		void	setCoarseLogLevel(uint8_t coarseloglevel);
		void	setFineLogLevel(uint8_t fineloglevel);
		void	setLogIndent(uint32_t indent);

		void	setShutdownFlag(bool *shuttingdown);

		virtual	bool	exportToFile(const char *filename,
						const char *table)=0;

	#include <sqlrelay/private/sqlrexport.h>
};

#endif
