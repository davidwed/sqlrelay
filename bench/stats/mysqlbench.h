// Copyright (c) 2014  David Muse
// See the file COPYING for more information
#ifndef MYSQL_BENCH_H
#define MYSQL_BENCH_H

extern "C" {
	#include <mysql.h>
}

#include "bench.h"

class mysqlbenchmarks : public benchmarks {
	public:
		mysqlbenchmarks(const char *connectstring,
					const char *db,
					uint64_t queries,
					uint64_t rows,
					uint32_t cols,
					uint32_t colsize,
					uint16_t iterations,
					bool debug);
};

class mysqlbenchconnection : public benchconnection {
	friend class mysqlbenchcursor;
	public:
			mysqlbenchconnection(const char *connectstring,
						const char *dbtype);
			~mysqlbenchconnection();

		bool	connect();
		bool	disconnect();

	private:
		const char	*host;
		uint16_t	port;
		const char	*socket;
		const char	*dbname;
		const char	*user;
		const char	*password;

		MYSQL	mysql;
};

class mysqlbenchcursor : public benchcursor {
	public:
			mysqlbenchcursor(benchconnection *con);
			~mysqlbenchcursor();

		bool	query(const char *query, bool getcolumns);

	private:
		mysqlbenchconnection	*mbcon;
};

#endif
