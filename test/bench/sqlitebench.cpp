// Copyright (c) 2014  David Muse
// See the file COPYING for more information
#ifndef _WIN32

#include "../../config.h"

extern "C" {
	#include <sqlite3.h>
}

#include "bench.h"

class sqlitebenchmarks : public benchmarks {
	public:
		sqlitebenchmarks(const char *connectstring,
					const char *db,
					uint64_t queries,
					uint64_t rows,
					uint32_t cols,
					uint32_t colsize,
					uint16_t samples,
					uint64_t rsbs,
					bool debug);
};

class sqlitebenchconnection : public benchconnection {
	friend class sqlitebenchcursor;
	public:
			sqlitebenchconnection(const char *connectstring,
						const char *dbtype);
			~sqlitebenchconnection();

		bool	connect();
		bool	disconnect();

	private:
		const char	*db;

		sqlite3		*sqlitecon;
};

class sqlitebenchcursor : public benchcursor {
	public:
			sqlitebenchcursor(benchconnection *con);
			~sqlitebenchcursor();

		bool	open();
		bool	query(const char *query, bool getcolumns);
		bool	close();

	private:
		sqlitebenchconnection	*sbcon;
		sqlite3_stmt		*sqlitestmt;
};

sqlitebenchmarks::sqlitebenchmarks(const char *connectstring,
					const char *db,
					uint64_t queries,
					uint64_t rows,
					uint32_t cols,
					uint32_t colsize,
					uint16_t samples,
					uint64_t rsbs,
					bool debug) :
					benchmarks(connectstring,db,
						queries,rows,cols,colsize,
						samples,rsbs,debug) {
	con=new sqlitebenchconnection(connectstring,db);
	cur=new sqlitebenchcursor(con);
}


sqlitebenchconnection::sqlitebenchconnection(
				const char *connectstring,
				const char *db) :
				benchconnection(connectstring,db) {
	db=getParam("db");
	sqlite3_open(db,&sqlitecon);
}

sqlitebenchconnection::~sqlitebenchconnection() {
	sqlite3_close(sqlitecon);
}

bool sqlitebenchconnection::connect() {
	return true;
}

bool sqlitebenchconnection::disconnect() {
	return true;
}

sqlitebenchcursor::sqlitebenchcursor(benchconnection *con) :
							benchcursor(con) {
	sbcon=(sqlitebenchconnection *)con;
}

sqlitebenchcursor::~sqlitebenchcursor() {
}

bool sqlitebenchcursor::open() {
	return true;
}

bool sqlitebenchcursor::query(const char *query, bool getcolumns) {

	bool	retval=false;

	if (sqlite3_prepare_v2(sbcon->sqlitecon,
					query,
					charstring::length(query),
					&sqlitestmt,
					NULL)==SQLITE_OK) {

		int	res=sqlite3_step(sqlitestmt);
		if (res==SQLITE_ROW) {

			int	ncolumn=sqlite3_column_count(sqlitestmt);
			if (getcolumns) {
				for (int i=0; i<ncolumn; i++) {
					sqlite3_column_name(sqlitestmt,i);
					sqlite3_column_type(sqlitestmt,i);
				}
			}

			while (sqlite3_step(sqlitestmt)==SQLITE_ROW) {
				for (int i=0; i<ncolumn; i++) {
					sqlite3_column_text(sqlitestmt,i);
				}
			}

			retval=true;

		} else if (res==SQLITE_DONE) {
			retval=true;
		}
	}

	sqlite3_finalize(sqlitestmt);
	return retval;
}

bool sqlitebenchcursor::close() {
	return true;
}

extern "C" {
	benchmarks *new_sqlitebenchmarks(const char *connectstring,
						const char *db,
						uint64_t queries,
						uint64_t rows,
						uint32_t cols,
						uint32_t colsize,
						uint16_t samples,
						uint64_t rsbs,
						bool debug) {
		return new sqlitebenchmarks(connectstring,db,queries,
						rows,cols,colsize,
						samples,rsbs,debug);
	}
}

#endif
