#include <pqdefinitions.h>
#include <stdlib.h>
#include <rudiments/string.h>

extern "C" {

PGresult *PQexec(PGconn *conn, const char *query) {

	PGresult	*pgresult=new PGresult;

	if (query && query[0]) {
		pgresult->sqlrcur=new sqlrcursor(conn->sqlrcon);
		if (pgresult->sqlrcur->sendQuery(query)) {
			pgresult->execstatus=PGRES_COMMAND_OK;
		} else {
			pgresult->execstatus=PGRES_NONFATAL_ERROR;
		}
	} else {
		pgresult->sqlrcur=NULL;
		pgresult->execstatus=PGRES_EMPTY_QUERY;
	}

	return pgresult;
}

ExecStatusType PQresultStatus(const PGresult *res) {
	return res->execstatus;
}

char *PQresStatus(ExecStatusType status) {
	// need to test these out for real to be sure what should be returned
	if (status==PGRES_EMPTY_QUERY) {
		return "PGRES_EMPTY_QUERY";
	} else if (status==PGRES_COMMAND_OK) {
		return "PGRES_COMMAND_OK";
	} else if (status==PGRES_TUPLES_OK) {
		return "PGRES_TUPLES_OK";
	} else if (status==PGRES_COPY_OUT) {
		return "PGRES_COPY_OUT";
	} else if (status==PGRES_COPY_IN) {
		return "PGRES_COPY_IN";
	} else if (status==PGRES_BAD_RESPONSE) {
		return "PGRES_BAD_RESPONSE";
	} else if (status==PGRES_NONFATAL_ERROR) {
		return "PGRES_NONFATAL_ERROR";
	} else if (status==PGRES_FATAL_ERROR) {
		return "PGRES_FATAL_ERROR";
	}
	return NULL;
}

char *PQresultErrorMessage(const PGresult *res) {
	return res->sqlrcur->errorMessage();
}


int PQntuples(const PGresult *res) {
	return res->sqlrcur->rowCount();
}

int PQnfields(const PGresult *res) {
	return res->sqlrcur->colCount();
}

int PQbinaryTuples(const PGresult *res) {
	// what does this do?
	return -1;
}

char *PQfname(const PGresult *res, int field_num) {
	return res->sqlrcur->getColumnName(field_num);
}

int PQfnumber(const PGresult *res, const char *field_name) {
	// what does this do?
	return -1;
}

Oid PQftype(const PGresult *res, int field_num) {
	return atoi(res->sqlrcur->getColumnType(field_num));
}

int PQfsize(const PGresult *res, int field_num) {
	return res->sqlrcur->getColumnLength(field_num);
}

int PQfmod(const PGresult *res, int field_num) {
	// what does this do?
	return -1;
}

char *PQcmdStatus(PGresult *res) {
	// what does this do?
	return "";
}

char *PQoidStatus(const PGresult *res) {
	// what does this do?
	return "";
}

Oid PQoidValue(const PGresult *res) {
	// what does this do?
	return 0;
}

char *PQcmdTuples(PGresult *res) {
	return string::parseNumber((long)res->sqlrcur->affectedRows());
}

char *PQgetvalue(const PGresult *res, int tup_num, int field_num) {
	return res->sqlrcur->getField(tup_num,field_num);
}

int PQgetlength(const PGresult *res, int tup_num, int field_num) {
	return res->sqlrcur->getFieldLength(tup_num,field_num);
}

int PQgetisnull(const PGresult *res, int tup_num, int field_num) {
	return (res->sqlrcur->getField(tup_num,field_num)==(char *)NULL);
}

void PQclear(PGresult *res) {
	delete res->sqlrcur;
	delete res;
}

// Make an empty PGresult with given status (some apps find this
// useful). If conn is not NULL and status indicates an error, the
// conn's errorMessage is copied.
PGresult *PQmakeEmptyPGresult(PGconn *conn, ExecStatusType status) {
	// not sure we can actually do anything for this
	return NULL;
}

}
