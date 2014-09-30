// Copyright (c) 2014  David Muse
// See the file COPYING for more information
#ifndef DB2_BENCH_H
#define DB2_BENCH_H

#include "bench.h"


class db2benchmarks : public benchmarks {
	public:
		db2benchmarks(const char *connectstring,
					const char *db,
					uint64_t queries,
					uint64_t rows,
					uint32_t cols,
					uint32_t colsize,
					uint16_t iterations,
					bool debug);
};

class db2benchconnection : public benchconnection {
	friend class db2benchcursor;
	public:
			db2benchconnection(const char *connectstring,
						const char *dbtype);
			~db2benchconnection();

		bool	connect();
		bool	disconnect();

	private:
		const char	*db;
		const char	*lang;
		const char	*user;
		const char	*password;
};

class db2benchcursor : public benchcursor {
	public:
			db2benchcursor(benchconnection *con);
			~db2benchcursor();

		bool	open();
		bool	query(const char *query, bool getcolumns);
		bool	close();

	private:
		db2benchconnection	*db2bcon;
};

#endif
