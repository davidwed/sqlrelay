#include <pqdefinitions.h>

// FIXME: these functions are dummied up to use the synchronous interface

extern "C" {

typedef enum {
	PGRES_POLLING_FAILED = 0,
	PGRES_POLLING_READING,
	PGRES_POLLING_WRITING,
	PGRES_POLLING_OK,
	PGRES_POLLING_ACTIVE
} PostgresPollingStatusType;

PGconn *PQconnectStart(const char *conninfo) {
	// SQL Relay doesn't connect right away, so just pass through the
	// normal connect process here
	return PQconnectdb(conninfo);
}

PostgresPollingStatusType PQconnectPoll(PGconn *conn) {
	return PGRES_POLLING_OK;
}

int PQresetStart(PGconn *conn) {
	char	*savestring=conn->conninfo;
	PQfinish(conn);
	conn=PQconnectdb(savestring);
	// return 1 for success, 0 for failure
	return 1;
}

PostgresPollingStatusType PQresetPoll(PGconn *conn) {
	return PGRES_POLLING_OK;
}

int PQrequestCancel(PGconn *conn) {
	delete conn->sqlrcon;
	conn->sqlrcon=NULL;
	return TRUE;
}

int PQsendQuery(PGconn *conn, const char *query) {

	// FIXME:
	// "query" could contain multiple queries,
	// parse them out and fork a thread to run each query
	conn->currentresult=PQexec(conn,query);
	return TRUE;
}

PGresult *PQgetResult(PGconn *conn) {

	// FIXME:
	// Should poll to see if one of the queries started in
	// PQsendQuery is done, if so, return it's result set.
	// If all queries are done, return NULL
	PGresult	*retval=conn->currentresult;
	conn->currentresult=NULL;
	return retval;
}

int PQisBusy(PGconn *conn) {
	return FALSE;
}

int PQconsumeInput(PGconn *conn) {
	return TRUE;
}

int PQsetnonblocking(PGconn *conn, int arg) {
	conn->nonblockingmode=arg;
	return TRUE;
}

int PQisnonblocking(const PGconn *conn) {
	return conn->nonblockingmode;
}

int PQflush(PGconn *conn) {
	// return 0 on success or EOF on failure
	return 0;
}

int PQsendSome(PGconn *conn) {
	// Have no idea what should be returned here
	return 0;
}

}
