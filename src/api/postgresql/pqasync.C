extern "C" {

typedef enum {
	PGRES_POLLING_FAILED = 0,
	PGRES_POLLING_READING,
	PGRES_POLLING_WRITING,
	PGRES_POLLING_OK,
	PGRES_POLLING_ACTIVE
} PostgresPollingStatusType;

PGconn *PQconnectStart(const char *conninfo) {
}

PostgresPollingStatusType PQconnectPoll(PGconn *conn) {
}

int PQresetStart(PGconn *conn) {
}

PostgresPollingStatusType PQresetPoll(PGconn *conn) {
}

int PQrequestCancel(PGconn *conn) {
}

int PQsendQuery(PGconn *conn, const char *query) {
}

PGresult *PQgetResult(PGconn *conn) {
}

int PQisBusy(PGconn *conn) {
}

int PQconsumeInput(PGconn *conn) {
}

int PQsetnonblocking(PGconn *conn, int arg) {
}

int PQisnonblocking(const PGconn *conn) {
}

int PQflush(PGconn *conn) {
}

int PQsendSome(PGconn *conn) {
}

}
