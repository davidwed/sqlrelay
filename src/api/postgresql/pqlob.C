#include <pqdefinitions.h>

extern "C" {

int lo_open(PGconn *conn, Oid lobjId, int mode) {
}

int lo_close(PGconn *conn, int fd) {
}

int lo_read(PGconn *conn, int fd, char *buf, size_t len) {
}

int lo_write(PGconn *conn, int fd, char *buf, size_t len) {
}

int lo_lseek(PGconn *conn, int fd, int offset, int whence) {
}

Oid lo_creat(PGconn *conn, int mode) {
}

int lo_tell(PGconn *conn, int fd) {
}

int lo_unlink(PGconn *conn, Oid lobjId) {
}

Oid lo_import(PGconn *conn, const char *filename) {
}

int lo_export(PGconn *conn, Oid lobjId, const char *filename) {
}

}
