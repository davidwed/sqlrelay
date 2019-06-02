// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information
#ifndef SQLRBENCHMARKS_H
#define SQLRBENCHMARKS_H

#include <rudiments/parameterstring.h>
#include <rudiments/stringbuffer.h>
#include <rudiments/randomnumber.h>
#include <rudiments/dictionary.h>
#include <rudiments/linkedlist.h>

class sqlrbenchconnection;
class sqlrbenchcursor;

class sqlrbench {
	public:
			sqlrbench(const char *connectstring,
						const char *db,
						uint64_t queries,
						uint64_t rows,
						uint32_t cols,
						uint32_t colsize,
						uint16_t samples,
						uint64_t rsbs,
						bool debug);
		virtual	~sqlrbench();
		void	shutDown();
		bool	run(
			dictionary< float, linkedlist< float > *> *selectstats,
			dictionary< float, linkedlist< float > *> *dmlstats);

	protected:
		sqlrbenchconnection	*con;
		sqlrbenchcursor		*cur;
		bool				issqlrelay;

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
		void	benchDML(uint64_t queries,
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

class sqlrbenchconnection {
	public:
			sqlrbenchconnection(const char *connectstring,
								const char *db);
		virtual	~sqlrbenchconnection();

		virtual	bool	connect()=0;
		virtual	bool	disconnect()=0;

	protected:
		const char	*getParam(const char *param);

		parameterstring	pstring;
		const char	*db;
};

class sqlrbenchcursor {
	public:
			sqlrbenchcursor(sqlrbenchconnection *bcon);
		virtual	~sqlrbenchcursor();

		virtual	bool	open();
		virtual	bool	query(const char *query, bool getcolumns)=0;
		virtual	bool	close();

	protected:
		sqlrbenchconnection	*bcon;
};

#endif
