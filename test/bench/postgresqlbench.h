// Copyright (c) 2014  David Muse
// See the file COPYING for more information
#ifndef POSTGRESQL_BENCH_H
#define POSTGRESQL_BENCH_H

#include "bench.h"

#include <libpq-fe.h>

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
		const char	*host;
		const char	*port;
		const char	*dbname;
		const char	*user;
		const char	*password;
		const char	*sslmode;

#ifdef HAVE_POSTGRESQL_PQCONNECTDB
		stringbuffer	conninfo;
#endif

		PGconn	*pgconn;
};

class postgresqlbenchcursor : public benchcursor {
	public:
			postgresqlbenchcursor(benchconnection *con);
			~postgresqlbenchcursor();

		bool	query(const char *query, bool getcolumns);

	private:
		postgresqlbenchconnection	*pgbcon;

		PGresult	*pgresult;
};

#endif
