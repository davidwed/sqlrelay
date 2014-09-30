// Copyright (c) 2014  David Muse
// See the file COPYING for more information
#include "../../config.h"

#include "postgresqlbench.h"

postgresqlbenchmarks::postgresqlbenchmarks(const char *connectstring,
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
	con=new postgresqlbenchconnection(connectstring,db);
	cur=new postgresqlbenchcursor(con);
}


postgresqlbenchconnection::postgresqlbenchconnection(
				const char *connectstring,
				const char *db) :
				benchconnection(connectstring,db) {
}

postgresqlbenchconnection::~postgresqlbenchconnection() {
	db=getParam("db");
	lang=getParam("lang");
	user=getParam("user");
	password=getParam("password");
}

bool postgresqlbenchconnection::connect() {
	return true;
}

bool postgresqlbenchconnection::disconnect() {
	return true;
}


postgresqlbenchcursor::postgresqlbenchcursor(benchconnection *con) :
							benchcursor(con) {
	pgbcon=(postgresqlbenchconnection *)con;
}

postgresqlbenchcursor::~postgresqlbenchcursor() {
}

bool postgresqlbenchcursor::open() {
	return true;
}

bool postgresqlbenchcursor::query(const char *query, bool getcolumns) {
	return true;
}

bool postgresqlbenchcursor::close() {
	return true;
}
