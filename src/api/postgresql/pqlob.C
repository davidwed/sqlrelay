#include <pqdefinitions.h>

extern "C" {

int lo_open(PGconn *conn, Oid lobjId, int mode) {
	return -1;
}

int lo_close(PGconn *conn, int fd) {
	return -1;
}

int lo_read(PGconn *conn, int fd, char *buf, size_t len) {
	return -1;
}

int lo_write(PGconn *conn, int fd, char *buf, size_t len) {
	return -1;
}

int lo_lseek(PGconn *conn, int fd, int offset, int whence) {
	return -1;
}

Oid lo_creat(PGconn *conn, int mode) {
	return InvalidOid;
}

int lo_tell(PGconn *conn, int fd) {
	return -1;
}

int lo_unlink(PGconn *conn, Oid lobjId) {
	return -1;
}

Oid lo_import(PGconn *conn, const char *filename) {
	return InvalidOid;
}

int lo_export(PGconn *conn, Oid lobjId, const char *filename) {
	return -1;
}

}
