#include <pqdefinitions.h>

extern "C" {

int lo_open(PGconn *conn, Oid lobjId, int mode) {
	printf("lo_open unimplemented\n");
}

int lo_close(PGconn *conn, int fd) {
	printf("lo_close unimplemented\n");
}

int lo_read(PGconn *conn, int fd, char *buf, size_t len) {
	printf("lo_read unimplemented\n");
}

int lo_write(PGconn *conn, int fd, char *buf, size_t len) {
	printf("lo_write unimplemented\n");
}

int lo_lseek(PGconn *conn, int fd, int offset, int whence) {
	printf("lo_lseek unimplemented\n");
}

Oid lo_creat(PGconn *conn, int mode) {
	printf("lo_creat unimplemented\n");
}

int lo_tell(PGconn *conn, int fd) {
	printf("lo_tell unimplemented\n");
}

int lo_unlink(PGconn *conn, Oid lobjId) {
	printf("lo_unlink unimplemented\n");
}

Oid lo_import(PGconn *conn, const char *filename) {
	printf("lo_import unimplemented\n");
}

int lo_export(PGconn *conn, Oid lobjId, const char *filename) {
	printf("lo_export unimplemented\n");
}

}
