// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information
#include "../../config.h"

#include <libpq-fe.h>

#include "sqlrbench.h"

class postgresqlbench : public sqlrbench {
	public:
		postgresqlbench(const char *connectstring,
					const char *db,
					uint64_t queries,
					uint64_t rows,
					uint32_t cols,
					uint32_t colsize,
					uint16_t samples,
					uint64_t rsbs,
					bool debug);
};

class postgresqlbenchconnection : public sqlrbenchconnection {
	friend class postgresqlbenchcursor;
	public:
			postgresqlbenchconnection(const char *connectstring,
						const char *dbtype);

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

class postgresqlbenchcursor : public sqlrbenchcursor {
	public:
			postgresqlbenchcursor(sqlrbenchconnection *con);

		bool	query(const char *query, bool getcolumns);

	private:
		postgresqlbenchconnection	*pgbcon;

		PGresult	*pgresult;
};

postgresqlbench::postgresqlbench(const char *connectstring,
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
	con=new postgresqlbenchconnection(connectstring,db);
	cur=new postgresqlbenchcursor(con);
}


postgresqlbenchconnection::postgresqlbenchconnection(
				const char *connectstring,
				const char *db) :
				sqlrbenchconnection(connectstring,db) {
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

postgresqlbenchcursor::postgresqlbenchcursor(sqlrbenchconnection *con) :
							sqlrbenchcursor(con) {
	pgbcon=(postgresqlbenchconnection *)con;
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

	// get the result status
	PQresultStatus(pgresult);

	// get the column, row, and affected row count
	int32_t	cols=PQnfields(pgresult);
	int	rows=PQntuples(pgresult);
	PQcmdTuples(pgresult);

#ifdef HAVE_POSTGRESQL_PQOIDVALUE
	// get the oid of the inserted row, if this was an insert
	PQoidValue(pgresult);
#endif

	// run through the columns
	if (getcolumns) {
		for (int i=0; i<cols; i++) {
			PQfname(pgresult,i);
			PQftype(pgresult,i);
			PQfsize(pgresult,i);
#ifdef HAVE_POSTGRESQL_PQFMOD
			PQfmod(pgresult,i);
#endif
#ifdef HAVE_POSTGRESQL_PQBINARYTUPLES
			PQbinaryTuples(pgresult);
#endif
		}
	}

	// run through the rows
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
	sqlrbench *new_postgresqlbench(const char *connectstring,
						const char *db,
						uint64_t queries,
						uint64_t rows,
						uint32_t cols,
						uint32_t colsize,
						uint16_t samples,
						uint64_t rsbs,
						bool debug) {
		return new postgresqlbench(connectstring,db,queries,
						rows,cols,colsize,
						samples,rsbs,debug);
	}
}
