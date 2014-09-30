// Copyright (c) 2014  David Muse
// See the file COPYING for more information
#ifndef FIREBIRD_BENCH_H
#define FIREBIRD_BENCH_H

#include "bench.h"


class firebirdbenchmarks : public benchmarks {
	public:
		firebirdbenchmarks(const char *connectstring,
					const char *db,
					uint64_t queries,
					uint64_t rows,
					uint32_t cols,
					uint32_t colsize,
					uint16_t iterations,
					bool debug);
};

class firebirdbenchconnection : public benchconnection {
	friend class firebirdbenchcursor;
	public:
			firebirdbenchconnection(const char *connectstring,
						const char *dbtype);
			~firebirdbenchconnection();

		bool	connect();
		bool	disconnect();

	private:
		const char	*db;
		const char	*dialect;
		const char	*user;
		const char	*password;
};

class firebirdbenchcursor : public benchcursor {
	public:
			firebirdbenchcursor(benchconnection *con);
			~firebirdbenchcursor();

		bool	open();
		bool	query(const char *query, bool getcolumns);
		bool	close();

	private:
		firebirdbenchconnection	*fbbcon;
};

#endif
