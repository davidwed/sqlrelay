extern "C" {

int PQgetline(PGconn *conn, char *string, int length) {
}

int PQputline(PGconn *conn, const char *string) {
}

int PQgetlineAsync(PGconn *conn, char *buffer, int bufsize) {
}

int PQputnbytes(PGconn *conn, const char *buffer, int nbytes) {
}

int PQendcopy(PGconn *conn) {
}


}
