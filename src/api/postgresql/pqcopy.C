#include <pqdefinitions.h>

extern "C" {

int PQgetline(PGconn *conn, char *string, int length) {
	//printf("PQgetline\n");
	// return EOF if we're at the end of input
	// 0 if an entire line has been read
	// 1 if more data is left to be read
	string[0]='\0';
	return EOF;
}

int PQputline(PGconn *conn, const char *string) {
	//printf("PQputline\n");
	return EOF;
}

int PQgetlineAsync(PGconn *conn, char *buffer, int bufsize) {
	//printf("PQgetlineAsync\n");
	buffer[0]='\0';
	return EOF;
}

int PQputnbytes(PGconn *conn, const char *buffer, int nbytes) {
	//printf("PQputnbytes\n");
	return EOF;
}

int PQendcopy(PGconn *conn) {
	//printf("PQendcopy\n");
	// return 0 on success, nonzero otherwise
	return -1;
}


}
