// Copyright (c) 2014  David Muse
// See the file COPYING for more information
#include <rudiments/charstring.h>

#include "sqlrelaybench.h"

sqlrelaybenchmarks::sqlrelaybenchmarks(const char *connectstring,
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
	con=new sqlrelaybenchconnection(connectstring,db);
	cur=new sqlrelaybenchcursor(con);
}


sqlrelaybenchconnection::sqlrelaybenchconnection(
				const char *connectstring,
				const char *db) :
				benchconnection(connectstring,db) {
	host=getParam("host");
	port=charstring::toInteger(getParam("port"));
	socket=getParam("socket");
	user=getParam("user");
	password=getParam("password");
	debug=!charstring::compare(getParam("debug"),"yes");
	sqlrcon=new sqlrconnection(host,port,socket,user,password,0,1);
	if (debug) {
		sqlrcon->debugOn();
	}
}

sqlrelaybenchconnection::~sqlrelaybenchconnection() {
	delete sqlrcon;
}

bool sqlrelaybenchconnection::connect() {
	return true;
}

bool sqlrelaybenchconnection::disconnect() {
	sqlrcon->endSession();
	return true;
}


sqlrelaybenchcursor::sqlrelaybenchcursor(benchconnection *con) :
							benchcursor(con) {
	sqlrbcon=(sqlrelaybenchconnection *)con;
	sqlrcur=new sqlrcursor(sqlrbcon->sqlrcon);
}

sqlrelaybenchcursor::~sqlrelaybenchcursor() {
	delete sqlrcur;
}

bool sqlrelaybenchcursor::query(const char *query) {
	return sqlrcur->sendQuery(query);
}
