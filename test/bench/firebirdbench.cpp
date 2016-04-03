// Copyright (c) 2014  David Muse
// See the file COPYING for more information
#include "../../config.h"

#include "firebirdbench.h"

class firebirdbenchconnection : public benchconnection {
	friend class firebirdbenchcursor;
	public:
			firebirdbenchconnection(const char *connectstring,
						const char *dbtype);
			~firebirdbenchconnection();

		bool	connect();
		bool	disconnect();
		bool	commit();

	private:
		const char	*db;
		const char	*dialect;
		const char	*user;
		const char	*password;
};

class firebirdbenchcursor : public benchcursor {
	public:
			firebirdbenchcursor(benchconnection *con);
			~firebirdbenchcursor();

		bool	open();
		bool	query(const char *query, bool getcolumns);
		bool	close();

	private:
		firebirdbenchconnection	*fbbcon;
};

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

bool firebirdbenchconnection::commit() {
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
