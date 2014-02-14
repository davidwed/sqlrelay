// Copyright (c) 2014  David Muse
// See the file COPYING for more information
#ifndef BENCH_H
#define BENCH_H

#include <rudiments/parameterstring.h>

class benchconnection {
	public:
			benchconnection();
		virtual	~benchconnection();

		virtual void	setConnectString(
					parameterstring *connectstring)=0;

		virtual void	setColumnCount(uint32_t columncount);
		virtual void	setRowCount(uint64_t rowcount);

		virtual	void	buildQueries()=0;

		virtual	bool	connect()=0;
		virtual	bool	disconnect()=0;

	protected:
		uint32_t	columncount;
		uint64_t	rowcount;
};

class benchcursor {
	public:
			benchcursor(benchconnection *conn);
		virtual	~benchcursor();

		virtual	bool	createTable()=0;
		virtual	bool	dropTable()=0;

		virtual	bool	insertQuery()=0;
		virtual	bool	updateQuery()=0;
		virtual	bool	deleteQuery()=0;
		virtual	bool	selectQuery()=0;
};

#endif
