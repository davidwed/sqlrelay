extern "C" {

typedef enum {
	PGRES_EMPTY_QUERY = 0,
	PGRES_COMMAND_OK,
	PGRES_TUPLES_OK,
	PGRES_COPY_OUT,
	PGRES_COPY_IN,
	PGRES_BAD_RESPONSE,
	PGRES_NONFATAL_ERROR,
	PGRES_FATAL_ERROR
} ExecStatusType;

PGresult *PQexec(PGconn *conn, const char *query) {
	sqlrcursor	*sqlrcur=new sqlrcursor(sqlrconn);
	sqlrcur->sendQuery(query);
}

ExecStatusType PQresultStatus(const PGresult *res) {
}

char *PQresStatus(ExecStatusType status) {
}

char *PQresultErrorMessage(const PGresult *res) {
}


int PQntuples(const PGresult *res) {
	return sqlrcur->rowCount();
}

int PQnfields(const PGresult *res) {
	return sqlrcur->colCount();
}

int PQbinaryTuples(const PGresult *res) {
}

char *PQfname(const PGresult *res, int field_num) {
	return sqlrcur->getColumnName(field_num);
}

int PQfnumber(const PGresult *res, const char *field_name) {
}

Oid PQftype(const PGresult *res, int field_num) {
	return sqlrcur->getColumnType(field_num);
}

int PQfsize(const PGresult *res, int field_num) {
	return sqlrcur->getColumnLength(field_num);
}

int PQfmod(const PGresult *res, int field_num) {
}

char *PQcmdStatus(PGresult *res) {
}

char *PQoidStatus(const PGresult *res) {
}

Oid PQoidValue(const PGresult *res) {
}

char *PQcmdTuples(PGresult *res) {
	return sqlrcur->affectedRowCount();
}

char *PQgetvalue(const PGresult *res, int tup_num, int field_num) {
	return sqlrcur->getField(tup_num,field_num);
}

int PQgetlength(const PGresult *res, int tup_num, int field_num) {
	return sqlrcur->getFieldLength(tup_num,field_num);
}

int PQgetisnull(const PGresult *res, int tup_num, int field_num) {
	return sqlrcur->getFieldLength(tup_num,field_num)==(char *)NULL;
}

void PQclear(PGresult *res) {
	delete sqlrcur;
}

// Make an empty PGresult with given status (some apps find this
// useful). If conn is not NULL and status indicates an error, the
// conn's errorMessage is copied.
PGresult *PQmakeEmptyPGresult(PGconn *conn, ExecStatusType status) {
}

}
