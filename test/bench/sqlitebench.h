// Copyright (c) 2014  David Muse
// See the file COPYING for more information
#ifndef SQLITE_BENCH_H
#define SQLITE_BENCH_H

#include "bench.h"


class sqlitebenchmarks : public benchmarks {
	public:
		sqlitebenchmarks(const char *connectstring,
					const char *db,
					uint64_t queries,
					uint64_t rows,
					uint32_t cols,
					uint32_t colsize,
					uint16_t iterations,
					bool debug);
};

class sqlitebenchconnection : public benchconnection {
	friend class sqlitebenchcursor;
	public:
			sqlitebenchconnection(const char *connectstring,
						const char *dbtype);
			~sqlitebenchconnection();

		bool	connect();
		bool	disconnect();

	private:
		const char	*db;
};

class sqlitebenchcursor : public benchcursor {
	public:
			sqlitebenchcursor(benchconnection *con);
			~sqlitebenchcursor();

		bool	open();
		bool	query(const char *query, bool getcolumns);
		bool	close();

	private:
		sqlitebenchconnection	*sbcon;
};

#endif
