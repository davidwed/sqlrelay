// Copyright (c) 2014  David Muse
// See the file COPYING for more information
#ifndef DB2_BENCH_H
#define DB2_BENCH_H

#include "../config.h"

#include "bench.h"

#include <sqlcli1.h>

#define FETCH_AT_ONCE		10
#define MAX_SELECT_LIST_SIZE	256
#define MAX_ITEM_BUFFER_SIZE	32768

struct db2column {
	char		*name;
	uint16_t	namelength;
	// SQLColAttribute requires that these are signed, 32 bit integers
	int32_t		type;
	int32_t		length;
	int32_t		precision;
	int32_t		scale;
	int32_t		nullable;
	uint16_t	primarykey;
	uint16_t	unique;
	uint16_t	partofkey;
	uint16_t	unsignednumber;
	uint16_t	zerofill;
	uint16_t	binary;
	uint16_t	autoincrement;
};

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
		const char	*dbname;
		const char	*lang;
		const char	*user;
		const char	*password;

		SQLHENV		env;
		SQLRETURN	erg;
		SQLHDBC		dbc;
};

class db2benchcursor : public benchcursor {
	public:
			db2benchcursor(benchconnection *con);
			~db2benchcursor();

		bool	query(const char *query, bool getcolumns);

	private:
		db2benchconnection	*db2bcon;

		SQLHSTMT	stmt;
		SQLRETURN	erg;
		SQLSMALLINT	ncols;

		db2column	column[MAX_SELECT_LIST_SIZE];
		char		field[MAX_SELECT_LIST_SIZE]
					[FETCH_AT_ONCE][MAX_ITEM_BUFFER_SIZE];
		SQLINTEGER	indicator[MAX_SELECT_LIST_SIZE][FETCH_AT_ONCE];
		#if (DB2VERSION>7)
		SQLUSMALLINT	rowstat[FETCH_AT_ONCE];
		#endif
};

#endif
