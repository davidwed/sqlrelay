// Copyright (c) 2014  David Muse
// See the file COPYING for more information
#ifndef BENCH_H
#define BENCH_H

#include <rudiments/parameterstring.h>
#include <rudiments/stringbuffer.h>
#include <rudiments/randomnumber.h>

class benchconnection;
class benchcursor;

class benchmarks {
	public:
			benchmarks(const char *connectstring,
						const char *db,
						uint64_t queries,
						uint64_t rows,
						uint32_t cols,
						uint32_t colsize,
						uint16_t iterations,
						bool debug);
		virtual	~benchmarks();
		void	run();

	protected:
		benchconnection	*con;
		benchcursor	*cur;

	private:
		char	*createQuery(uint32_t cols, uint32_t colsize);
		char	*insertQuery(uint32_t cols, uint32_t colsize);
		void	appendRandomString(stringbuffer *str, uint32_t colsize);
		void	benchSelect(const char *selectquery,
					uint64_t queries,
					uint64_t rows, uint32_t cols,
					uint32_t colsize, uint16_t iterations);

		const char	*connectstring;
		const char	*db;
		uint64_t	queries;
		uint64_t	rows;
		uint32_t	cols;
		uint32_t	colsize;
		uint16_t	iterations;
		bool		debug;

		randomnumber	rnd;
};

class benchconnection {
	public:
			benchconnection(const char *connectstring,
					const char *db);
		virtual	~benchconnection();

		virtual	bool	connect()=0;
		virtual	bool	disconnect()=0;

	protected:
		const char	*getParam(const char *param);

		parameterstring	pstring;
		const char	*db;
};

class benchcursor {
	public:
			benchcursor(benchconnection *bcon);
		virtual	~benchcursor();

		virtual	bool	query(const char *query)=0;

	protected:
		benchconnection	*bcon;
};

#endif
