// Copyright (c) 2014  David Muse
// See the file COPYING for more information
#include <rudiments/charstring.h>

#include "sqlrelaybench.h"

#include <sqlrelay/sqlrclient.h>

class sqlrelaybenchconnection : public benchconnection {
	friend class sqlrelaybenchcursor;
	public:
			sqlrelaybenchconnection(const char *connectstring,
						const char *dbtype);
			~sqlrelaybenchconnection();

		bool	connect();
		bool	disconnect();

	private:
		const char	*host;
		uint16_t	port;
		const char	*socket;
		const char	*user;
		const char	*password;
		bool		debug;

		sqlrconnection	*sqlrcon;
};

class sqlrelaybenchcursor : public benchcursor {
	public:
			sqlrelaybenchcursor(benchconnection *con);
			~sqlrelaybenchcursor();

		bool	query(const char *query, bool getcolumns);

	private:
		sqlrelaybenchconnection	*sqlrbcon;
		sqlrcursor		*sqlrcur;
};

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
	sqlrcur->setResultSetBufferSize(10);
}

sqlrelaybenchcursor::~sqlrelaybenchcursor() {
	delete sqlrcur;
}

bool sqlrelaybenchcursor::query(const char *query, bool getcolumns) {
	if (getcolumns) {
		sqlrcur->getColumnInfo();
	} else {
		sqlrcur->dontGetColumnInfo();
	}
	// FIXME: run through the rows
	// (now that we're setting a result set buffer size
	return sqlrcur->sendQuery(query);
}
