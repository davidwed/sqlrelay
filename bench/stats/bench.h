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
						uint64_t cons,
						uint64_t queries,
						uint64_t rows,
						uint32_t cols,
						uint32_t colsize,
						bool debug);
		virtual	~benchmarks();
		void	run();

	protected:
		benchconnection	*con;
		benchcursor	*cur;

	private:
		char	*buildQuery(const char *query, uint64_t key);
		void	appendRandomString(stringbuffer *str);

		const char	*connectstring;
		const char	*db;
		uint64_t	cons;
		uint64_t	queries;
		uint64_t	rows;
		uint32_t	cols;
		uint32_t	colsize;
		bool		debug;

		randomnumber	rnd;

		char	*createquery;
		char	*dropquery;
		char	*insertquery;
		char	*updatequery;
		char	*deletequery;
		char	*selectquery;
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
