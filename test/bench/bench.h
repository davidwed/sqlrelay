// Copyright (c) 2014  David Muse
// See the file COPYING for more information
#ifndef BENCH_H
#define BENCH_H

#include <rudiments/parameterstring.h>
#include <rudiments/stringbuffer.h>
#include <rudiments/randomnumber.h>
#include <rudiments/dictionary.h>
#include <rudiments/linkedlist.h>

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
						uint16_t samples,
						uint64_t rsbs,
						bool debug);
		virtual	~benchmarks();
		void	shutDown();
		bool	run(dictionary< float, linkedlist< float > *> *stats);

	protected:
		benchconnection	*con;
		benchcursor	*cur;
		bool		issqlrelay;

	private:
		char	*createQuery(uint32_t cols, uint32_t colsize);
		char	*insertQuery(uint32_t cols, uint32_t colsize);
		void	appendRandomString(stringbuffer *str, uint32_t colsize);
		void	benchSelect(const char *selectquery,
					uint64_t queries,
					uint64_t rows, uint32_t cols,
					uint32_t colsize, uint16_t samples,
					dictionary< float,
						linkedlist< float > *> *stats);

		const char	*connectstring;
		const char	*db;
		uint64_t	queries;
		uint64_t	rows;
		uint32_t	cols;
		uint32_t	colsize;
		uint16_t	samples;
		uint64_t	rsbs;
		bool		debug;

		randomnumber	rnd;

		bool		shutdown;
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

		virtual	bool	open();
		virtual	bool	query(const char *query, bool getcolumns)=0;
		virtual	bool	close();

	protected:
		benchconnection	*bcon;
};

#endif
