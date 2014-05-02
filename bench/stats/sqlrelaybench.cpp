// Copyright (c) 2014  David Muse
// See the file COPYING for more information
#include <rudiments/charstring.h>

#include "sqlrelaybench.h"

sqlrelaybenchconnection::sqlrelaybenchconnection(
				const char *connectstring, const char *dbtype) :
					benchconnection(connectstring,dbtype) {
	host=getParam("host");
	port=charstring::toInteger(getParam("port"));
	socket=getParam("socket");
	user=getParam("user");
	password=getParam("password");
	sqlrcon=new sqlrconnection(host,port,socket,user,password,0,1);
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
							benchcursor(con){
	sqlrbcon=(sqlrelaybenchconnection *)con;
	sqlrcur=new sqlrcursor(sqlrbcon->sqlrcon);
}

sqlrelaybenchcursor::~sqlrelaybenchcursor() {
	delete sqlrcur;
}

bool sqlrelaybenchcursor::createTable() {
	return sqlrcur->sendQuery(bcon->getCreateQuery());
}

bool sqlrelaybenchcursor::dropTable() {
	return sqlrcur->sendQuery(bcon->getDropQuery());
}

bool sqlrelaybenchcursor::insertQuery() {
	return sqlrcur->sendQuery(bcon->getInsertQuery());
}

bool sqlrelaybenchcursor::updateQuery() {
	return sqlrcur->sendQuery(bcon->getUpdateQuery());
}

bool sqlrelaybenchcursor::deleteQuery() {
	return sqlrcur->sendQuery(bcon->getDeleteQuery());
}

bool sqlrelaybenchcursor::selectQuery() {
	return sqlrcur->sendQuery(bcon->getSelectQuery());
}
