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
	host=getParam("host");
	port=getParam("port");
	dbname=getParam("db");
	user=getParam("user");
	password=getParam("password");
	sslmode=getParam("sslmode");
	if (!charstring::length(sslmode)) {
		sslmode="disable";
	}
}

postgresqlbenchconnection::~postgresqlbenchconnection() {
}

bool postgresqlbenchconnection::connect() {
#ifdef HAVE_POSTGRESQL_PQCONNECTDB
	conninfo.clear();
	conninfo.append("user=")->append(user);
	conninfo.append(" password=")->append(password);
	if (host && host[0]) {
		conninfo.append(" host=")->append(host);
	}
	if (port && port[0]) {
		conninfo.append(" port=")->append(port);
	}
	if (dbname && dbname[0]) {
		conninfo.append(" dbname=")->append(dbname);
	}
	if (sslmode && sslmode[0]) {
		conninfo.append(" sslmode=")->append(sslmode);
	}
	pgconn=PQconnectdb(conninfo.getString());
#else
	pgconn=PQsetdbLogin(host,port,NULL,NULL,dbname,user,password);
#endif
	if (PQstatus(pgconn)==CONNECTION_BAD) {
		stdoutput.printf("PQsetdbLogin failed\n");
		return false;
	}
	return true;
}

bool postgresqlbenchconnection::disconnect() {
	PQfinish(pgconn);
	return true;
}


postgresqlbenchcursor::postgresqlbenchcursor(benchconnection *con) :
							benchcursor(con) {
	pgbcon=(postgresqlbenchconnection *)con;
}

postgresqlbenchcursor::~postgresqlbenchcursor() {
}

bool postgresqlbenchcursor::query(const char *query, bool getcolumns) {

	pgresult=PQexec(pgbcon->pgconn,query);
	if (pgresult==(PGresult *)NULL) {
		stdoutput.printf("PQexec failed\n");
		return false;
	}

	int32_t	cols=PQnfields(pgresult);

	if (getcolumns) {
		for (int i=0; i<cols; i++) {
			PQfname(pgresult,i);
			PQftype(pgresult,i);
			PQfsize(pgresult,i);
		}
	}

	// run through the rows
	int	rows=PQntuples(pgresult);
	for (int i=0; i<rows; i++) {
		for (int j=0; j<cols; j++) {
			//printf("%s,",PQgetvalue(pgresult,i,j));
			PQgetvalue(pgresult,i,j);
		}
		//printf("\n");
	}
	PQclear(pgresult);

	return true;
}