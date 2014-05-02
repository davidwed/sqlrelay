// Copyright (c) 2014  David Muse
// See the file COPYING for more information
#ifndef SQLRELAY_BENCH_H
#define SQLRELAY_BENCH_H

#include <sqlrelay/sqlrclient.h>

#include "bench.h"

class sqlrelaybenchconnection : public benchconnection {
	friend class sqlrelaybenchcursor;
	public:
			sqlrelaybenchconnection(const char *connectstring,
						const char *dbtype);
			~sqlrelaybenchconnection();

		bool	connect();
		bool	disconnect();

	private:
		const char	*host;
		uint16_t	port;
		const char	*socket;
		const char	*user;
		const char	*password;

		sqlrconnection	*sqlrcon;
};

class sqlrelaybenchcursor : public benchcursor {
	public:
			sqlrelaybenchcursor(benchconnection *con);
			~sqlrelaybenchcursor();

		bool	createTable();
		bool	dropTable();

		bool	insertQuery();
		bool	updateQuery();
		bool	deleteQuery();
		bool	selectQuery();

	private:
		sqlrelaybenchconnection	*sqlrbcon;
		sqlrcursor		*sqlrcur;
};

#endif
