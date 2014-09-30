// Copyright (c) 2014  David Muse
// See the file COPYING for more information
#include "../../config.h"

#include "firebirdbench.h"

firebirdbenchmarks::firebirdbenchmarks(const char *connectstring,
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
	con=new firebirdbenchconnection(connectstring,db);
	cur=new firebirdbenchcursor(con);
}


firebirdbenchconnection::firebirdbenchconnection(
				const char *connectstring,
				const char *db) :
				benchconnection(connectstring,db) {
	db=getParam("db");
	dialect=getParam("dialect");
	user=getParam("user");
	password=getParam("password");
}

firebirdbenchconnection::~firebirdbenchconnection() {
}

bool firebirdbenchconnection::connect() {
	return true;
}

bool firebirdbenchconnection::disconnect() {
	return true;
}


firebirdbenchcursor::firebirdbenchcursor(benchconnection *con) :
							benchcursor(con) {
	fbbcon=(firebirdbenchconnection *)con;
}

firebirdbenchcursor::~firebirdbenchcursor() {
}

bool firebirdbenchcursor::open() {
	return true;
}

bool firebirdbenchcursor::query(const char *query, bool getcolumns) {
	return true;
}

bool firebirdbenchcursor::close() {
	return true;
}
