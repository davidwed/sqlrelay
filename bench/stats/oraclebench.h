// Copyright (c) 2014  David Muse
// See the file COPYING for more information
#ifndef ORACLE_BENCH_H
#define ORACLE_BENCH_H

extern "C" {
	#include <oci.h>
}

#include "bench.h"

#define FETCH_AT_ONCE		10
#define MAX_ITEM_BUFFER_SIZE	2048
#define MAX_SELECT_LIST_SIZE	256

struct describe {
	OCIParam	*paramd;
	sb4	dbsize;
	sb2	dbtype;
	text	*buf;
	sb4	buflen;
};


class oraclebenchmarks : public benchmarks {
	public:
		oraclebenchmarks(const char *connectstring,
					const char *db,
					uint64_t queries,
					uint64_t rows,
					uint32_t cols,
					uint32_t colsize,
					uint16_t iterations,
					bool debug);
};

class oraclebenchconnection : public benchconnection {
	friend class oraclebenchcursor;
	public:
			oraclebenchconnection(const char *connectstring,
						const char *dbtype);
			~oraclebenchconnection();

		bool	connect();
		bool	disconnect();

	private:
		const char	*sid;
		const char	*user;
		const char	*password;

		OCIEnv		*env;
		OCIServer	*srv;
		OCIError	*err;
		OCISvcCtx	*svc;
		OCISession	*session;
		OCITrans	*trans;
};

class oraclebenchcursor : public benchcursor {
	public:
			oraclebenchcursor(benchconnection *con);
			~oraclebenchcursor();

		bool	open();
		bool	query(const char *query);
		bool	close();

	private:
		oraclebenchconnection	*orabcon;

		OCIStmt		*stmt;
		int32_t		fetchatonce;

		describe	desc[MAX_SELECT_LIST_SIZE];

		OCIDefine	*def[MAX_SELECT_LIST_SIZE];
		ub1		def_buf[MAX_SELECT_LIST_SIZE]
						[FETCH_AT_ONCE]
						[MAX_ITEM_BUFFER_SIZE];
		sb2		def_indp[MAX_SELECT_LIST_SIZE]
							[FETCH_AT_ONCE];
		ub2		def_col_retlen[MAX_SELECT_LIST_SIZE]
							[FETCH_AT_ONCE];
		ub2		def_col_retcode[MAX_SELECT_LIST_SIZE]
							[FETCH_AT_ONCE];
};

#endif
