// Copyright (c) 2014  David Muse
// See the file COPYING for more information
#ifndef _WIN32

#include "../../config.h"

#include "freetdsbench.h"

class freetdsbenchconnection : public benchconnection {
	friend class freetdsbenchcursor;
	public:
			freetdsbenchconnection(const char *connectstring,
						const char *dbtype);
			~freetdsbenchconnection();

		bool	connect();
		bool	disconnect();

	private:
		const char	*sybase;
		const char	*lang;
		const char	*server;
		const char	*db;
		const char	*user;
		const char	*password;
};

class freetdsbenchcursor : public benchcursor {
	public:
			freetdsbenchcursor(benchconnection *con);
			~freetdsbenchcursor();

		bool	open();
		bool	query(const char *query, bool getcolumns);
		bool	close();

	private:
		freetdsbenchconnection	*ftdsbcon;
};

freetdsbenchmarks::freetdsbenchmarks(const char *connectstring,
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
	con=new freetdsbenchconnection(connectstring,db);
	cur=new freetdsbenchcursor(con);
}


freetdsbenchconnection::freetdsbenchconnection(
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

freetdsbenchconnection::~freetdsbenchconnection() {
}

bool freetdsbenchconnection::connect() {
	return true;
}

bool freetdsbenchconnection::disconnect() {
	return true;
}


freetdsbenchcursor::freetdsbenchcursor(benchconnection *con) : benchcursor(con) {
	ftdsbcon=(freetdsbenchconnection *)con;
}

freetdsbenchcursor::~freetdsbenchcursor() {
}

bool freetdsbenchcursor::open() {
	return true;
}

bool freetdsbenchcursor::query(const char *query, bool getcolumns) {
	return true;
}

bool freetdsbenchcursor::close() {
	return true;
}

#endif
