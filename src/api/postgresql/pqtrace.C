extern "C" {

void PQtrace(PGconn *conn, FILE *debug_port) {
	sqlrconn->debugOn();
}

void PQuntrace(PGconn *conn) {
	sqlrconn->debugOff();
}

}
