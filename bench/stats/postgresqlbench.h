// Copyright (c) 2014  David Muse
// See the file COPYING for more information
#ifndef POSTGRESQL_BENCH_H
#define POSTGRESQL_BENCH_H

#include "bench.h"


class postgresqlbenchmarks : public benchmarks {
	public:
		postgresqlbenchmarks(const char *connectstring,
					const char *db,
					uint64_t queries,
					uint64_t rows,
					uint32_t cols,
					uint32_t colsize,
					uint16_t iterations,
					bool debug);
};

class postgresqlbenchconnection : public benchconnection {
	friend class postgresqlbenchcursor;
	public:
			postgresqlbenchconnection(const char *connectstring,
						const char *dbtype);
			~postgresqlbenchconnection();

		bool	connect();
		bool	disconnect();

	private:
		const char	*db;
		const char	*lang;
		const char	*user;
		const char	*password;
};

class postgresqlbenchcursor : public benchcursor {
	public:
			postgresqlbenchcursor(benchconnection *con);
			~postgresqlbenchcursor();

		bool	open();
		bool	query(const char *query, bool getcolumns);
		bool	close();

	private:
		postgresqlbenchconnection	*pgbcon;
};

#endif
