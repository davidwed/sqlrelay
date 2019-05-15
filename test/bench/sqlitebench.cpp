// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information
#ifndef _WIN32

#include "../../config.h"

extern "C" {
	#include <sqlite3.h>
}

#include "sqlrbench.h"

class sqlitebench : public sqlrbench {
	public:
		sqlitebench(const char *connectstring,
					const char *db,
					uint64_t queries,
					uint64_t rows,
					uint32_t cols,
					uint32_t colsize,
					uint16_t samples,
					uint64_t rsbs,
					bool debug);
};

class sqlitebenchconnection : public sqlrbenchconnection {
	friend class sqlitebenchcursor;
	public:
			sqlitebenchconnection(const char *connectstring,
						const char *dbtype);

		bool	connect();
		bool	disconnect();

	private:
		const char	*dbase;

		sqlite3		*sqlitecon;
};

class sqlitebenchcursor : public sqlrbenchcursor {
	public:
			sqlitebenchcursor(sqlrbenchconnection *con);

		bool	open();
		bool	query(const char *query, bool getcolumns);
		bool	close();

	private:
		sqlitebenchconnection	*sbcon;
		sqlite3_stmt		*sqlitestmt;
};

sqlitebench::sqlitebench(const char *connectstring,
					const char *db,
					uint64_t queries,
					uint64_t rows,
					uint32_t cols,
					uint32_t colsize,
					uint16_t samples,
					uint64_t rsbs,
					bool debug) :
					sqlrbench(connectstring,db,
						queries,rows,cols,colsize,
						samples,rsbs,debug) {
	con=new sqlitebenchconnection(connectstring,db);
	cur=new sqlitebenchcursor(con);
}


sqlitebenchconnection::sqlitebenchconnection(
				const char *connectstring,
				const char *db) :
				sqlrbenchconnection(connectstring,db) {
	dbase=getParam("db");
}

bool sqlitebenchconnection::connect() {
	return (sqlite3_open(dbase,&sqlitecon)==SQLITE_OK);
}

bool sqlitebenchconnection::disconnect() {
	sqlite3_close(sqlitecon);
	return true;
}

sqlitebenchcursor::sqlitebenchcursor(sqlrbenchconnection *con) :
							sqlrbenchcursor(con) {
	sbcon=(sqlitebenchconnection *)con;
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
	sqlrbench *new_sqlitebench(const char *connectstring,
						const char *db,
						uint64_t queries,
						uint64_t rows,
						uint32_t cols,
						uint32_t colsize,
						uint16_t samples,
						uint64_t rsbs,
						bool debug) {
		return new sqlitebench(connectstring,db,queries,
						rows,cols,colsize,
						samples,rsbs,debug);
	}
}

#endif
