// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#ifndef SQLRIMPORT_H
#define SQLRIMPORT_H

#include <sqlrelay/private/sqlrimportincludes.h>

class SQLRCLIENT_DLLSPEC sqlrimport {
	public:
			sqlrimport();
		virtual	~sqlrimport();

		void	setSqlrConnection(sqlrconnection *sqlrcon);
		void	setSqlrCursor(sqlrcursor *sqlrcur);

		void	setDbType(const char *dbtype);
		void	setTable(const char *table);
		void	setIgnoreColumns(bool ignorecolumns);
		void	setCommitCount(uint64_t commitcount);

		void	setLogger(logger *lg);
		void	setCoarseLogLevel(uint8_t coarseloglevel);
		void	setFineLogLevel(uint8_t fineloglevel);
		void	setLogIndent(uint32_t logindent);

		void	setShutdownFlag(bool *shuttingdown);

		void	setLogErrors(bool logerrors);

		virtual	bool	importFromFile(const char *filename)=0;

	#include <sqlrelay/private/sqlrimport.h>
};

#endif
