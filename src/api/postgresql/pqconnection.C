#include <pqdefinitions.h>
#include <stdlib.h>

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

// connect to the database using paramaters
PGconn *PQsetdbLogin(const char *pghost, const char *pgport,
			 const char *pgoptions, const char *pgtty,
			 const char *dbName,
			 const char *login, const char *pwd) {

	PGconn	*pgconn=new PGconn;
	pgconn->host=(char *)pghost;
	pgconn->port=(char *)pgport;
	pgconn->options=(char *)pgoptions;
	pgconn->tty=(char *)pgtty;
	pgconn->db=(char *)dbName;
	pgconn->user=(char *)login;
	pgconn->password=(char *)pwd;
	pgconn->socket=0;

	pgconn->sqlrcon=new sqlrconnection(pghost,atoi(pgport),"",
						login,pwd,0,1);
}

// older, simpler connection function that didn't have login, password
PGconn *PQsetdb(const char *pghost, const char *pgport,
			 const char *pgoptions, const char *pgtty,
			 const char *dbName) {
	return PQsetdbLogin(pghost,pgport,pgoptions,pgtty,dbName,NULL,NULL);
}

// connect to the database using a conninfo string
PGconn *PQconnectdb(const char *conninfo) {

	char	*host="";
	char	*port="";
	char	*options="";
	char	*tty="";
	char	*db="";
	char	*user="";
	char	*password="";

	// FIXME: extract host, hostaddr, port,
	// user, password, use them below...

	return PQsetdbLogin(host,port,options,tty,db,user,password);
}

// close the current connection
void PQfinish(PGconn *conn) {
	delete conn->sqlrcon;
	delete conn;
}

// close and re-open the connection
void PQreset(PGconn *conn) {
	PQfinish(conn);
	PQsetdbLogin(conn->host,conn->port,conn->options,
			conn->tty,conn->db,conn->user,conn->password);
}

// accessor functions for PGconn objects
char *PQdb(const PGconn *conn) {
	return conn->db;
}

char *PQuser(const PGconn *conn) {
	return conn->user;
}

char *PQpass(const PGconn *conn) {
	return conn->password;
}

char *PQhost(const PGconn *conn) {
	return conn->host;
}

char *PQport(const PGconn *conn) {
	return conn->port;
}

char *PQtty(const PGconn *conn) {
	return conn->tty;
}

char *PQoptions(const PGconn *conn) {
	return conn->options;
}

ConnStatusType PQstatus(const PGconn *conn) {
	return CONNECTION_OK;
}

char *PQerrorMessage(const PGconn *conn) {
	// FIXME: SQL Relay's error messages are cursor-specific
	return NULL;
}

int PQsocket(const PGconn *conn) {
	return conn->socket;
}

int PQbackendPID(const PGconn *conn) {
	return -1;
}

int PQclientEncoding(const PGconn *conn) {
	return -1;
}

int PQsetClientEncoding(PGconn *conn, const char *encoding) {
	return -1;
}


#ifdef USE_SSL
// get the SSL structure associated with a connection
SSL *PQgetssl(PGconn *conn) {
	return conn->ssl;
}

#endif


}
