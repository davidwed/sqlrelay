// Copyright (c) 2014  David Muse
// See the file COPYING for more information
#include "../../config.h"

#include "sybasebench.h"

sybasebenchmarks::sybasebenchmarks(const char *connectstring,
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
	con=new sybasebenchconnection(connectstring,db);
	cur=new sybasebenchcursor(con);
}


sybasebenchconnection::sybasebenchconnection(
				const char *connectstring,
				const char *db) :
				benchconnection(connectstring,db) {
	sybase=getParam("sybase");
	lang=getParam("lang");
	server=getParam("server");
	db=getParam("db");
	user=getParam("user");
	password=getParam("password");
}

sybasebenchconnection::~sybasebenchconnection() {
}

bool sybasebenchconnection::connect() {
	return true;
}

bool sybasebenchconnection::disconnect() {
	return true;
}


sybasebenchcursor::sybasebenchcursor(benchconnection *con) : benchcursor(con) {
	sybbcon=(sybasebenchconnection *)con;
}

sybasebenchcursor::~sybasebenchcursor() {
}

bool sybasebenchcursor::open() {
	return true;
}

bool sybasebenchcursor::query(const char *query, bool getcolumns) {
	return true;
}

bool sybasebenchcursor::close() {
	return true;
}
