// Copyright (c) 2014  David Muse
// See the file COPYING for more information
#include "../../config.h"

#include <libpq-fe.h>

#include "bench.h"

class postgresqlbenchmarks : public benchmarks {
	public:
		postgresqlbenchmarks(const char *connectstring,
					const char *db,
					uint64_t queries,
					uint64_t rows,
					uint32_t cols,
					uint32_t colsize,
					uint16_t samples,
					uint64_t rsbs,
					bool debug);
};

class postgresqlbenchconnection : public benchconnection {
	friend class postgresqlbenchcursor;
	public:
			postgresqlbenchconnection(const char *connectstring,
						const char *dbtype);
			~postgresqlbenchconnection();

		bool	connect();
		bool	disconnect();

	private:
		const char	*host;
		const char	*port;
		const char	*dbname;
		const char	*user;
		const char	*password;
		const char	*sslmode;

#ifdef HAVE_POSTGRESQL_PQCONNECTDB
		stringbuffer	conninfo;
#endif

		PGconn	*pgconn;
};

class postgresqlbenchcursor : public benchcursor {
	public:
			postgresqlbenchcursor(benchconnection *con);
			~postgresqlbenchcursor();

		bool	query(const char *query, bool getcolumns);

	private:
		postgresqlbenchconnection	*pgbcon;

		PGresult	*pgresult;
};

postgresqlbenchmarks::postgresqlbenchmarks(const char *connectstring,
					const char *db,
					uint64_t queries,
					uint64_t rows,
					uint32_t cols,
					uint32_t colsize,
					uint16_t samples,
					uint64_t rsbs,
					bool debug) :
					benchmarks(connectstring,db,
						queries,rows,cols,colsize,
						samples,rsbs,debug) {
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
	pgconn=NULL;
}

postgresqlbenchconnection::~postgresqlbenchconnection() {
}

bool postgresqlbenchconnection::connect() {
#ifdef HAVE_POSTGRESQL_PQCONNECTDB
	conninfo.clear();
	conninfo.append("user=")->append(user);
	conninfo.append(" password=")->append(password);
	if (!charstring::isNullOrEmpty(host)) {
		conninfo.append(" host=")->append(host);
	}
	if (!charstring::isNullOrEmpty(port)) {
		conninfo.append(" port=")->append(port);
	}
	if (!charstring::isNullOrEmpty(dbname)) {
		conninfo.append(" dbname=")->append(dbname);
	}
	if (!charstring::isNullOrEmpty(sslmode)) {
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

#ifdef HAVE_POSTGRESQL_PQPREPARE
	// prepare the query
	pgresult=PQprepare(pgbcon->pgconn,"",query,0,NULL);
	PQclear(pgresult);

	// run the query
	pgresult=PQexecPrepared(pgbcon->pgconn,"",0,NULL,NULL,NULL,0);
	if (pgresult==(PGresult *)NULL) {
		stdoutput.printf("PQexec failed\n");
		return false;
	}
#else
	// run the query
	pgresult=PQexec(pgbcon->pgconn,query);
	if (pgresult==(PGresult *)NULL) {
		stdoutput.printf("PQexec failed\n");
		return false;
	}
#endif

	// get the affected row count
	PQcmdTuples(pgresult);

	// get the column count
	int32_t	cols=PQnfields(pgresult);

	// run through the columns
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
			PQgetisnull(pgresult,i,j);
			PQgetvalue(pgresult,i,j);
			PQgetlength(pgresult,i,j);
		}
	}

	// free the result set
	PQclear(pgresult);

	return true;
}

extern "C" {
	benchmarks *new_postgresqlbenchmarks(const char *connectstring,
						const char *db,
						uint64_t queries,
						uint64_t rows,
						uint32_t cols,
						uint32_t colsize,
						uint16_t samples,
						uint64_t rsbs,
						bool debug) {
		return new postgresqlbenchmarks(connectstring,db,queries,
						rows,cols,colsize,
						samples,rsbs,debug);
	}
}
