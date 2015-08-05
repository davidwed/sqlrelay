// Copyright (c) 2014  David Muse
// See the file COPYING for more information
#ifndef SYBASE_BENCH_H
#define SYBASE_BENCH_H

#include "bench.h"

extern "C" {
	#include <ctpublic.h>
}

#define SYBASE_FETCH_AT_ONCE 10
#define SYBASE_MAX_SELECT_LIST_SIZE 256
#define SYBASE_MAX_ITEM_BUFFER_SIZE 2048


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
		const char	*dbname;
		const char	*user;
		const char	*password;

		CS_CONTEXT	*context;
		CS_LOCALE	*locale;
		CS_CONNECTION	*conn;

		static	CS_RETCODE	csMessageCallback(CS_CONTEXT *ctxt,
						CS_CLIENTMSG *msgp);
		static	CS_RETCODE	clientMessageCallback(CS_CONTEXT *ctxt,
						CS_CONNECTION *cnn,
						CS_CLIENTMSG *msgp);
		static	CS_RETCODE	serverMessageCallback(CS_CONTEXT *ctxt,
						CS_CONNECTION *cnn,
						CS_SERVERMSG *msgp);
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

		CS_COMMAND	*cmd;
		CS_INT		resultstype;
		CS_INT		ncols;
		CS_DATAFMT	column[SYBASE_MAX_SELECT_LIST_SIZE];
		char		data[SYBASE_MAX_SELECT_LIST_SIZE]
						[SYBASE_FETCH_AT_ONCE]
						[SYBASE_MAX_ITEM_BUFFER_SIZE];
		CS_INT		datalength[SYBASE_MAX_SELECT_LIST_SIZE]
						[SYBASE_FETCH_AT_ONCE];
		CS_SMALLINT	nullindicator[SYBASE_MAX_SELECT_LIST_SIZE]
						[SYBASE_FETCH_AT_ONCE];
		CS_INT		rowcount;
};

#endif
