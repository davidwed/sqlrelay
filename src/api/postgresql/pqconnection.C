extern "C" {

typedef enum {
	CONNECTION_OK,
	CONNECTION_BAD,
	CONNECTION_STARTED,
	CONNECTION_MADE,
	CONNECTION_AWAITING_RESPONSE,
	CONNECTION_AUTH_OK,
	CONNECTION_SETENV
} ConnStatusType;

// connect to the database using a conninfo string
PGconn *PQconnectdb(const char *conninfo) {

	char	*host="";
	char	*port="";
	char	*user="";
	char	*password="";

	// FIXME: extract host, hostaddr, port,
	// user, password, use them below...

	sqlrconnection	*sqlrconn=new sqlrconnection(host,atoi(port),"",
							user,password,0,1);
}

// connect to the database using paramaters
PGconn *PQsetdbLogin(const char *pghost, const char *pgport,
			 const char *pgoptions, const char *pgtty,
			 const char *dbName,
			 const char *login, const char *pwd) {

	sqlrconnection	*sqlrconn=new sqlrconnection(pghost,pgport,"",
							login,pwd,0,1);
}

// older, simpler connection function that didn't have login, password
PGconn *PQsetdb(const char *pghost, const char *pgport,
			 const char *pgoptions, const char *pgtty,
			 const char *dbName) {
	return PQsetdbLogin(pghost,pgport,pgoptions,pgtty,dbName,NULL,NULL);
}

// close the current connection
void PQfinish(PGconn *conn) {
	delete sqlrconn;
}

// close and re-open the connection
void PQreset(PGconn *conn) {
	PQfinish();
	PQconnectDB(conninfo);
}

// accessor functions for PGconn objects
char *PQdb(const PGconn *conn) {
	return conn->dbName;
}

char *PQuser(const PGconn *conn) {
	return conn->pguser;
}

char *PQpass(const PGconn *conn) {
	return conn->pgpass;
}

char *PQhost(const PGconn *conn) {
	return conn->pghost;
}

char *PQport(const PGconn *conn) {
	return conn->pgport;
}

char *PQtty(const PGconn *conn) {
	return conn->pgtty;
}

char *PQoptions(const PGconn *conn) {
	return conn->pgoptions;
}

ConnStatusType PQstatus(const PGconn *conn) {
	return conn->status;
}

char *PQerrorMessage(const PGconn *conn) {
	// FIXME: SQL Relay's error messages are cursor-specific
}

int PQsocket(const PGconn *conn) {
	return conn->pgunixsocket;
}

int PQbackendPID(const PGconn *conn) {
	// FIXME
}

int PQclientEncoding(const PGconn *conn) {
	return conn->client_encoding;
}

int PQsetClientEncoding(PGconn *conn, const char *encoding) {
	// FIXME
}


#ifdef USE_SSL
// get the SSL structure associated with a connection
SSL *PQgetssl(PGconn *conn) {
	return conn->ssl;
}

#endif


}
