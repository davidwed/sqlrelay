/*
Portions Copyright (c) 1996-2002, The PostgreSQL Global Development Group
 
Portions Copyright (c) 1994, The Regents of the University of California

Portions Copyright (c) 2003-2004, David Muse

Permission to use, copy, modify, and distribute this software and its documentation for any purpose, without fee, and without a written agreement is hereby granted, provided that the above copyright notice and this paragraph and the following two paragraphs appear in all copies.
 
IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING LOST PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 
THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATIONS TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
*/

#include <pqdefinitions.h>

// FIXME: these functions are dummied up to use the synchronous interface

extern "C" {

typedef enum {
	PGRES_POLLING_FAILED = 0,
	PGRES_POLLING_READING,
	PGRES_POLLING_WRITING,
	PGRES_POLLING_OK,
	PGRES_POLLING_ACTIVE
} PostgresPollingStatusType;

PGconn *PQconnectStart(const char *conninfo) {
	// SQL Relay doesn't connect right away, so just pass through the
	// normal connect process here
	return PQconnectdb(conninfo);
}

PostgresPollingStatusType PQconnectPoll(PGconn *conn) {
	return PGRES_POLLING_OK;
}

int PQresetStart(PGconn *conn) {
	char	*savestring=conn->conninfo;
	PQfinish(conn);
	conn=PQconnectdb(savestring);
	// return 1 for success, 0 for failure
	return 1;
}

PostgresPollingStatusType PQresetPoll(PGconn *conn) {
	return PGRES_POLLING_OK;
}

int PQrequestCancel(PGconn *conn) {
	delete conn->sqlrcon;
	conn->sqlrcon=NULL;
	return TRUE;
}

int PQsendQuery(PGconn *conn, const char *query) {

	// FIXME:
	// "query" could contain multiple queries,
	// parse them out and fork a thread to run each query
	conn->currentresult=PQexec(conn,query);
	return TRUE;
}

PGresult *PQgetResult(PGconn *conn) {

	// FIXME:
	// Should poll to see if one of the queries started in
	// PQsendQuery is done, if so, return it's result set.
	// If all queries are done, return NULL
	PGresult	*retval=conn->currentresult;
	conn->currentresult=NULL;
	return retval;
}

int PQisBusy(PGconn *conn) {
	return FALSE;
}

int PQconsumeInput(PGconn *conn) {
	return TRUE;
}

int PQsetnonblocking(PGconn *conn, int arg) {
	conn->nonblockingmode=arg;
	return TRUE;
}

int PQisnonblocking(const PGconn *conn) {
	return conn->nonblockingmode;
}

int PQflush(PGconn *conn) {
	// return 0 on success or EOF on failure
	return 0;
}

int PQsendSome(PGconn *conn) {
	// Have no idea what should be returned here
	return 0;
}

}
