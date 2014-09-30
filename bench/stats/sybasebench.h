// Copyright (c) 2014  David Muse
// See the file COPYING for more information
#ifndef SYBASE_BENCH_H
#define SYBASE_BENCH_H

#include "bench.h"


class sybasebenchmarks : public benchmarks {
	public:
		sybasebenchmarks(const char *connectstring,
					const char *db,
					uint64_t queries,
					uint64_t rows,
					uint32_t cols,
					uint32_t colsize,
					uint16_t iterations,
					bool debug);
};

class sybasebenchconnection : public benchconnection {
	friend class sybasebenchcursor;
	public:
			sybasebenchconnection(const char *connectstring,
						const char *dbtype);
			~sybasebenchconnection();

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

class sybasebenchcursor : public benchcursor {
	public:
			sybasebenchcursor(benchconnection *con);
			~sybasebenchcursor();

		bool	open();
		bool	query(const char *query, bool getcolumns);
		bool	close();

	private:
		sybasebenchconnection	*sybbcon;
};

#endif
