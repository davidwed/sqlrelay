#include <pqdefinitions.h>

extern "C" {

int PQgetline(PGconn *conn, char *string, int length) {
	// return EOF if we're at the end of input
	// 0 if an entire line has been read
	// 1 if more data is left to be read
	string[0]='\0';
	return EOF;
}

int PQputline(PGconn *conn, const char *string) {
	return EOF;
}

int PQgetlineAsync(PGconn *conn, char *buffer, int bufsize) {
	buffer[0]='\0';
	return EOF;
}

int PQputnbytes(PGconn *conn, const char *buffer, int nbytes) {
	return EOF;
}

int PQendcopy(PGconn *conn) {
	// return 0 on success, nonzero otherwise
	return -1;
}


}
