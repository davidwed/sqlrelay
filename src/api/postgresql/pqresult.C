#include <pqdefinitions.h>
#include <stdlib.h>
#include <string.h>
#include <rudiments/string.h>

extern "C" {

// lame... I stole this code from sqlrcursor
char	*skipWhitespaceAndComments(const char *querybuffer) {
	// scan the query, bypassing whitespace and comments.
	char	*ptr=(char *)querybuffer;
	while (*ptr && 
		(*ptr==' ' || *ptr=='\n' || *ptr=='	' || *ptr=='-')) {

		// skip any comments
		if (*ptr=='-') {
			while (*ptr && *ptr!='\n') {
				ptr++;
			}
		}
		ptr++;
	}
	return ptr;
}

// lame... I stole this code from sqlrcursor too
int	queryIsNotSelect(const char *querybuffer) {

	// scan the query, bypassing whitespace and comments.
	char	*ptr=skipWhitespaceAndComments(querybuffer);

	// if the query is a select but not a select into then return false,
	// otherwise return true
	if (!strncasecmp(ptr,"select",6) && 
			strncasecmp(ptr,"select into ",12)) {
		return 0;
	}
	return 1;
}

void PQclear(PGresult *res) {
	//printf("PQclear\n");

	if (res) {
		res->parent->nonblockingmode=res->previousnonblockingmode;
		delete res->sqlrcur;
		delete res;
	}
}

PGresult *PQexec(PGconn *conn, const char *query) {
	//printf("PQexec\n");

	PGresult	*result=new PGresult;
	result->parent=conn;
	result->previousnonblockingmode=conn->nonblockingmode;
	result->queryisnotselect=1;

	delete[] conn->error;
	conn->error=NULL;

	if (query && query[0]) {
		result->sqlrcur=new sqlrcursor(conn->sqlrcon);
		result->sqlrcur->copyReferences();
		if (result->sqlrcur->sendQuery(query)) {
			if (queryIsNotSelect(query)) {
				result->execstatus=PGRES_COMMAND_OK;
			} else {
				result->execstatus=PGRES_TUPLES_OK;
				result->queryisnotselect=0;
			}
		} else {
			conn->error=new char[
				strlen(result->sqlrcur->errorMessage())+2];
			sprintf(conn->error,"%s\n",
				result->sqlrcur->errorMessage());
			PQclear(result);
			return NULL;
		}
	} else {
		result->sqlrcur=NULL;
		result->execstatus=PGRES_EMPTY_QUERY;
	}

	return result;
}

ExecStatusType PQresultStatus(const PGresult *res) {
	//printf("PQresultStatus\n");
	// FIXME: what should I return if res is NULL
	return (res)?res->execstatus:PGRES_FATAL_ERROR;
}

char *PQresStatus(ExecStatusType status) {
	//printf("PQresStatus\n");

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
	//printf("PQresultErrorMessage\n");
	return res->sqlrcur->errorMessage();
}


int PQntuples(const PGresult *res) {
	//printf("PQntuples\n");
	return res->sqlrcur->rowCount();
}

int PQnfields(const PGresult *res) {
	//printf("PQnfields\n");
	return res->sqlrcur->colCount();
}

int PQbinaryTuples(const PGresult *res) {
	//printf("PQbinaryTuples\n");
	// return 1 if result set contains binary data, 0 otherwise
	return 1;
}

char *PQfname(const PGresult *res, int field_num) {
	//printf("PQfname\n");
	return res->sqlrcur->getColumnName(field_num);
}

int PQfnumber(const PGresult *res, const char *field_name) {
	//printf("PQfnumber\n");
	for (int i=0; i<res->sqlrcur->colCount(); i++) {
		if (!strcmp(field_name,res->sqlrcur->getColumnName(i))) {
			return i;
		}
	}
	return -1;
}

Oid PQftype(const PGresult *res, int field_num) {
	//printf("PQftype\n");
	return atoi(res->sqlrcur->getColumnType(field_num));
}

int PQfsize(const PGresult *res, int field_num) {
	//printf("PQfsize\n");
	return res->sqlrcur->getColumnLength(field_num);
}

int PQfmod(const PGresult *res, int field_num) {
	//printf("PQfmod badly implemented\n");
	return -1;
}

char *PQcmdStatus(PGresult *res) {
	//printf("PQcmdStatus badly implemented\n");
	// should return a string represeting the "command type" like:
	//	SELECT, INSERT, UPDATE, DROP, etc.
	if (res->queryisnotselect) {
		return "";
	} else {
		return "SELECT";
	}
}

char *PQoidStatus(const PGresult *res) {
	//printf("PQoidStatus badly implemented\n");
	// return OID of tuple if query was an insert,
	// otherwise return InvalidOid
	return "InvalidOid";
}

Oid PQoidValue(const PGresult *res) {
	//printf("PQoidValue badly implemented\n");
	// return OID of tuple if query was an insert,
	// otherwise return InvalidOid
	return InvalidOid;
}

char *PQcmdTuples(PGresult *res) {
	//printf("PQcmdTuples\n");
	return string::parseNumber((long)res->sqlrcur->affectedRows());
}

char *PQgetvalue(const PGresult *res, int tup_num, int field_num) {
	//printf("PQgetvalue\n");
	return res->sqlrcur->getField(tup_num,field_num);
}

int PQgetlength(const PGresult *res, int tup_num, int field_num) {
	//printf("PQgetlength\n");
	return res->sqlrcur->getFieldLength(tup_num,field_num);
}

int PQgetisnull(const PGresult *res, int tup_num, int field_num) {
	//printf("PQgetisnull\n");
	return (res->sqlrcur->getField(tup_num,field_num)==(char *)NULL);
}

// Make an empty PGresult with given status (some apps find this
// useful). If conn is not NULL and status indicates an error, the
// conn's errorMessage is copied.
PGresult *PQmakeEmptyPGresult(PGconn *conn, ExecStatusType status) {
	//printf("PQmakeEmptyPGresult\n");
	PGresult	*result=new PGresult;
	result->sqlrcur=NULL;
	result->execstatus=status;
	result->parent=conn;
	result->previousnonblockingmode=conn->nonblockingmode;
	return result;
}

}
