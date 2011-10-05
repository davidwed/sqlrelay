/*
Portions Copyright (c) 1996-2002, The PostgreSQL Global Development Group
 
Portions Copyright (c) 1994, The Regents of the University of California

Portions Copyright (c) 2003-2004, David Muse

Permission to use, copy, modify, and distribute this software and its documentation for any purpose, without fee, and without a written agreement is hereby granted, provided that the above copyright notice and this paragraph and the following two paragraphs appear in all copies.
 
IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING LOST PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 
THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATIONS TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
*/

#include <pqdefinitions.h>

extern "C" {

int lo_open(PGconn *conn, Oid lobjId, int mode) {
	debugFunction();
	return -1;
}

int lo_close(PGconn *conn, int fd) {
	debugFunction();
	return -1;
}

int lo_read(PGconn *conn, int fd, char *buf, size_t len) {
	debugFunction();
	return -1;
}

int lo_write(PGconn *conn, int fd, char *buf, size_t len) {
	debugFunction();
	return -1;
}

int lo_lseek(PGconn *conn, int fd, int offset, int whence) {
	debugFunction();
	return -1;
}

Oid lo_creat(PGconn *conn, int mode) {
	debugFunction();
	return InvalidOid;
}

int lo_tell(PGconn *conn, int fd) {
	debugFunction();
	return -1;
}

int lo_unlink(PGconn *conn, Oid lobjId) {
	debugFunction();
	return -1;
}

Oid lo_import(PGconn *conn, const char *filename) {
	debugFunction();
	return InvalidOid;
}

int lo_export(PGconn *conn, Oid lobjId, const char *filename) {
	debugFunction();
	return -1;
}

}
