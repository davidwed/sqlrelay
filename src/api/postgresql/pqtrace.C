#include <pqdefinitions.h>

extern "C" {

void PQtrace(PGconn *conn, FILE *debug_port) {
	//printf("PQtrace badly implemented\n");
	// FIXME: this should go to a file instead of stdio
	conn->sqlrcon->debugOn();
}

void PQuntrace(PGconn *conn) {
	//printf("PQuntrace\n");
	conn->sqlrcon->debugOff();
}

}
