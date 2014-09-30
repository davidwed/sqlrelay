// Copyright (c) 2014  David Muse
// See the file COPYING for more information
#include "../../config.h"

#include "db2bench.h"

db2benchmarks::db2benchmarks(const char *connectstring,
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
	con=new db2benchconnection(connectstring,db);
	cur=new db2benchcursor(con);
}


db2benchconnection::db2benchconnection(
				const char *connectstring,
				const char *db) :
				benchconnection(connectstring,db) {
}

db2benchconnection::~db2benchconnection() {
	db=getParam("db");
	lang=getParam("lang");
	user=getParam("user");
	password=getParam("password");
}

bool db2benchconnection::connect() {
	return true;
}

bool db2benchconnection::disconnect() {
	return true;
}


db2benchcursor::db2benchcursor(benchconnection *con) : benchcursor(con) {
	db2bcon=(db2benchconnection *)con;
}

db2benchcursor::~db2benchcursor() {
}

bool db2benchcursor::open() {
	return true;
}

bool db2benchcursor::query(const char *query, bool getcolumns) {
	return true;
}

bool db2benchcursor::close() {
	return true;
}
