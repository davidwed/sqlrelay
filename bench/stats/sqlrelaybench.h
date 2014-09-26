// Copyright (c) 2014  David Muse
// See the file COPYING for more information
#ifndef SQLRELAY_BENCH_H
#define SQLRELAY_BENCH_H

#include <sqlrelay/sqlrclient.h>

#include "bench.h"

class sqlrelaybenchmarks : public benchmarks {
	public:
		sqlrelaybenchmarks(const char *connectstring,
					const char *db,
					uint64_t queries,
					uint64_t rows,
					uint32_t cols,
					uint32_t colsize,
					uint16_t iterations,
					bool debug);
};

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
		bool		debug;

		sqlrconnection	*sqlrcon;
};

class sqlrelaybenchcursor : public benchcursor {
	public:
			sqlrelaybenchcursor(benchconnection *con);
			~sqlrelaybenchcursor();

		bool	query(const char *query);

	private:
		sqlrelaybenchconnection	*sqlrbcon;
		sqlrcursor		*sqlrcur;
};

#endif
