// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information
#include "../../config.h"

#include "sqlrbench.h"

class firebirdbench : public sqlrbench {
	public:
		firebirdbench(const char *connectstring,
					const char *db,
					uint64_t queries,
					uint64_t rows,
					uint32_t cols,
					uint32_t colsize,
					uint16_t samples,
					uint64_t rsbs,
					bool debug);
};

class firebirdbenchconnection : public sqlrbenchconnection {
	friend class firebirdbenchcursor;
	public:
			firebirdbenchconnection(const char *connectstring,
						const char *dbtype);

		bool	connect();
		bool	disconnect();

	private:
		const char	*dbase;
		const char	*dialect;
		const char	*user;
		const char	*password;
};

class firebirdbenchcursor : public sqlrbenchcursor {
	public:
			firebirdbenchcursor(sqlrbenchconnection *con);

		bool	open();
		bool	query(const char *query, bool getcolumns);
		bool	close();

	private:
		firebirdbenchconnection	*fbbcon;
};

firebirdbench::firebirdbench(const char *connectstring,
					const char *db,
					uint64_t queries,
					uint64_t rows,
					uint32_t cols,
					uint32_t colsize,
					uint16_t samples,
					uint64_t rsbs,
					bool debug) :
					sqlrbench(connectstring,db,
						queries,rows,cols,colsize,
						samples,rsbs,debug) {
	con=new firebirdbenchconnection(connectstring,db);
	cur=new firebirdbenchcursor(con);
}


firebirdbenchconnection::firebirdbenchconnection(
				const char *connectstring,
				const char *db) :
				sqlrbenchconnection(connectstring,db) {
	dbase=getParam("db");
	dialect=getParam("dialect");
	user=getParam("user");
	password=getParam("password");
}

bool firebirdbenchconnection::connect() {
	return true;
}

bool firebirdbenchconnection::disconnect() {
	return true;
}

firebirdbenchcursor::firebirdbenchcursor(sqlrbenchconnection *con) :
						sqlrbenchcursor(con) {
	fbbcon=(firebirdbenchconnection *)con;
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

extern "C" {
	sqlrbench *new_firebirdbench(const char *connectstring,
						const char *db,
						uint64_t queries,
						uint64_t rows,
						uint32_t cols,
						uint32_t colsize,
						uint16_t samples,
						uint64_t rsbs,
						bool debug) {
		return new firebirdbench(connectstring,db,queries,
						rows,cols,colsize,
						samples,rsbs,debug);
	}
}
