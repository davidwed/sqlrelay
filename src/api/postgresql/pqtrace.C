#include <pqdefinitions.h>

extern "C" {

void PQtrace(PGconn *conn, FILE *debug_port) {
	// FIXME: this should go to a file instead of stdio
	conn->sqlrcon->debugOn();
}

void PQuntrace(PGconn *conn) {
	conn->sqlrcon->debugOff();
}

}
