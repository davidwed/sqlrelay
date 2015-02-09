// Copyright (c) 2014  David Muse
// See the file COPYING for more information
#ifndef FREETDS_BENCH_H
#define FREETDS_BENCH_H

#include "bench.h"


class freetdsbenchmarks : public benchmarks {
	public:
		freetdsbenchmarks(const char *connectstring,
					const char *db,
					uint64_t queries,
					uint64_t rows,
					uint32_t cols,
					uint32_t colsize,
					uint16_t iterations,
					bool debug);
};

class freetdsbenchconnection : public benchconnection {
	friend class freetdsbenchcursor;
	public:
			freetdsbenchconnection(const char *connectstring,
						const char *dbtype);
			~freetdsbenchconnection();

		bool	connect();
		bool	disconnect();

	private:
		const char	*sybase;
		const char	*lang;
		const char	*server;
		const char	*db;
		const char	*user;
		const char	*password;
};

class freetdsbenchcursor : public benchcursor {
	public:
			freetdsbenchcursor(benchconnection *con);
			~freetdsbenchcursor();

		bool	open();
		bool	query(const char *query, bool getcolumns);
		bool	close();

	private:
		freetdsbenchconnection	*ftdsbcon;
};

#endif
