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
	//printf("PQconnectStart\n");
	// SQL Relay doesn't connect right away, so just pass through the
	// normal connect process here
	return PQconnectdb(conninfo);
}

PostgresPollingStatusType PQconnectPoll(PGconn *conn) {
	//printf("PQconnectPoll\n");
	return PGRES_POLLING_OK;
}

int PQresetStart(PGconn *conn) {
	//printf("PQresetStart\n");
	char	*savestring=conn->conninfo;
	PQfinish(conn);
	conn=PQconnectdb(savestring);
	// return 1 for success, 0 for failure
	return 1;
}

PostgresPollingStatusType PQresetPoll(PGconn *conn) {
	//printf("PQresetPoll\n");
	return PGRES_POLLING_OK;
}

int PQrequestCancel(PGconn *conn) {
	//printf("PQrequestCancel\n");
	delete conn->sqlrcon;
	conn->sqlrcon=NULL;
	return TRUE;
}

int PQsendQuery(PGconn *conn, const char *query) {
	//printf("PQsendQuery: %s\n",query);

	// FIXME:
	// "query" could contain multiple queries,
	// parse them out and fork a thread to run each query
	conn->currentresult=PQexec(conn,query);
	return TRUE;
}

PGresult *PQgetResult(PGconn *conn) {
	//printf("PQgetResult\n");

	// FIXME:
	// Should poll to see if one of the queries started in
	// PQsendQuery is done, if so, return it's result set.
	// If all queries are done, return NULL
	PGresult	*retval=conn->currentresult;
	conn->currentresult=NULL;
	return retval;
}

int PQisBusy(PGconn *conn) {
	//printf("PQisBusy\n");
	return FALSE;
}

int PQconsumeInput(PGconn *conn) {
	//printf("PQconsumeInput\n");
	return TRUE;
}

int PQsetnonblocking(PGconn *conn, int arg) {
	//printf("PQsetnonblocking\n");
	conn->nonblockingmode=arg;
	return TRUE;
}

int PQisnonblocking(const PGconn *conn) {
	//printf("PQisnonblocking\n");
	return conn->nonblockingmode;
}

int PQflush(PGconn *conn) {
	//printf("PQflush\n");
	// return 0 on success or EOF on failure
	return 0;
}

int PQsendSome(PGconn *conn) {
	printf("PQsendSome badly implemented\n");
	// Have no idea what should be returned here
	return 0;
}

}
