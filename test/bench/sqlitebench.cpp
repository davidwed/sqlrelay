// Copyright (c) 2014  David Muse
// See the file COPYING for more information
#ifndef _WIN32

#include "../../config.h"

#include "sqlitebench.h"

sqlitebenchmarks::sqlitebenchmarks(const char *connectstring,
					const char *db,
					uint64_t queries,
					uint64_t rows,
					uint32_t cols,
					uint32_t colsize,
					uint16_t iterations,
					bool debug) :
					benchmarks(connectstring,db,
						queries,rows,cols,colsize,
						iterations,debug) {
	con=new sqlitebenchconnection(connectstring,db);
	cur=new sqlitebenchcursor(con);
}


sqlitebenchconnection::sqlitebenchconnection(
				const char *connectstring,
				const char *db) :
				benchconnection(connectstring,db) {
	db=getParam("db");
}

sqlitebenchconnection::~sqlitebenchconnection() {
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
	return true;
}

bool sqlitebenchcursor::close() {
	return true;
}

#endif
