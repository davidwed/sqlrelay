#include <pqdefinitions.h>
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_STRINGS_H
	#include <strings.h>
#endif
#include <rudiments/charstring.h>

#define NEED_DATATYPESTRING	1
#include <datatypes.h>

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
	if (res) {
		res->parent->nonblockingmode=res->previousnonblockingmode;
		delete res->sqlrcur;
		delete res;
	}
}

PGresult *PQexec(PGconn *conn, const char *query) {

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
	// FIXME: I'm not sure I should return PGRES_FATAL_ERROR if res is NULL
	return (res)?res->execstatus:PGRES_FATAL_ERROR;
}

char *PQresStatus(ExecStatusType status) {
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
	// return 1 if result set contains binary data, 0 otherwise
	for (int i=0; i<res->sqlrcur->colCount(); i++) {
		if (res->sqlrcur->getColumnIsBinary(i)) {
			return 1;
		}
	}
	return 0;
}

char *PQfname(const PGresult *res, int field_num) {
	return res->sqlrcur->getColumnName(field_num);
}

int PQfnumber(const PGresult *res, const char *field_name) {
	for (int i=0; i<res->sqlrcur->colCount(); i++) {
		if (!strcmp(field_name,res->sqlrcur->getColumnName(i))) {
			return i;
		}
	}
	return -1;
}

static Oid postgresqltypemap[]={
	// "UNKNOWN"
	705,
	// addded by freetds
	// "CHAR"
	18,
	// "INT"
	23,
	// "SMALLINT"
	21,
	// "TINYINT"
	21,
	// "MONEY"
	790,
	// "DATETIME"
	1082,
	// "NUMERIC"
	1700,
	// "DECIMAL"
	701,
	// "SMALLDATETIME"
	1082,
	// "SMALLMONEY"
	790,
	// "IMAGE"
	26,
	// "BINARY"
	26,
	// "BIT"
	1560,
	// "REAL"
	700,
	// "FLOAT"
	701,
	// "TEXT"
	25,
	// "VARCHAR"
	1043,
	// "VARBINARY"
	26,
	// "LONGCHAR"
	26,
	// "LONGBINARY"
	26,
	// "LONG"
	26,
	// "ILLEGAL"
	705,
	// "SENSITIVITY"
	705,
	// "BOUNDARY"
	705,
	// "VOID"
	2278,
	// "USHORT"
	21,
	// added by lago
	// "UNDEFINED"
	705,
	// "DOUBLE"
	708,
	// "DATE"
	1082,
	// "TIME"
	1083,
	// "TIMESTAMP"
	1114,
	// added by msql
	// "UINT"
	23,
	// "LASTREAL"
	700,
	// added by mysql
	// "STRING"
	1042,
	// "VARSTRING"
	1043,
	// "LONGLONG"
	20,
	// "MEDIUMINT"
	23,
	// "YEAR"
	23,
	// "NEWDATE"
	1082,
	// "NULL"
	705,
	// "ENUM"
	32,
	// "SET"
	32,
	// "TINYBLOB"
	26,
	// "MEDIUMBLOB"
	26,
	// "LONGBLOB"
	26,
	// "BLOB"
	26,
	// added by oracle
	// "VARCHAR2"
	1043,
	// "NUMBER"
	1022,
	// "ROWID"
	20,
	// "RAW"
	26,
	// "LONG_RAW"
	26,
	// "MLSLABEL"
	26,
	// "CLOB"
	26,
	// "BFILE"
	26,
	// added by odbc
	// "BIGINT"
	20,
	// "INTEGER"
	23,
	// "LONGVARBINARY"
	26,
	// "LONGVARCHAR"
	26,
	// added by db2
	// "GRAPHIC"
	26,
	// "VARGRAPHIC"
	26,
	// "LONGVARGRAPHIC"
	26,
	// "DBCLOB"
	26,
	// "DATALINK"
	26,
	// "USER_DEFINED_TYPE"
	705,
	// "SHORT_DATATYPE"
	21,
	// "TINY_DATATYPE"
	21,
	// added by interbase
	// "D_FLOAT"
	708,
	// "ARRAY"
	2277,
	// "QUAD"
	2277,
	// "INT64"
	20,
	// "DOUBLE PRECISION"
	708,
	// added by postgresql
	// "BOOL"
	16,
	// "BYTEA"
	17,
	// "NAME"
	19,
	// "INT8"
	20,
	// "INT2"
	21,
	// "INT2VECTOR"
	22,
	// "INT4"
	23,
	// "REGPROC"
	24,
	// "OID"
	26,
	// "TID"
	27,
	// "XID"
	28,
	// "CID"
	29,
	// "OIDVECTOR"
	30,
	// "SMGR"
	210,
	// "POINT"
	600,
	// "LSEG"
	601,
	// "PATH"
	602,
	// "BOX"
	603,
	// "POLYGON"
	604,
	// "LINE"
	628,
	// "_LINE"
	629,
	// "FLOAT4"
	700,
	// "FLOAT8"
	701,
	// "ABSTIME"
	702,
	// "RELTIME"
	703,
	// "TINTERVAL"
	704,
	// "CIRCLE"
	718,
	// "_CIRCLE"
	719,
	// "_MONEY"
	791,
	// "MACADDR"
	829,
	// "INET"
	869,
	// "CIDR"
	650,
	// "_BOOL"
	1000,
	// "_BYTEA"
	1001,
	// "_CHAR"
	1002,
	// "_NAME"
	1003,
	// "_INT2"
	1005,
	// "_INT2VECTOR"
	1006,
	// "_INT4"
	1007,
	// "_REGPROC"
	1008,
	// "_TEXT"
	1009,
	// "_oid"
	1028,
	// "_TID"
	1010,
	// "_XID"
	1011,
	// "_CID"
	1012,
	// "_OIDVECTOR"
	1013,
	// "_BPCHAR"
	1014,
	// "_VARCHAR"
	1015,
	// "_INT8"
	1016,
	// "_POINT"
	1017,
	// "_LSEG"
	1018,
	// "_PATH"
	1019,
	// "_BOX"
	1020,
	// "_FLOAT4"
	1021,
	// "_FLOAT8"
	1022,
	// "_ABSTIME"
	1023,
	// "_RELTIME"
	1024,
	// "_TINTERVAL"
	1025,
	// "_POLYGON"
	1027,
	// "ACLITEM"
	1033,
	// "_ACLITEM"
	1034,
	// "_MACADDR"
	1040,
	// "_INET"
	1041,
	// "_CIDR"
	651,
	// "BPCHAR"
	1042,
	// "_TIMESTAMP"
	1114,
	// "_DATE"
	1182,
	// "_TIME"
	1183,
	// "TIMESTAMPTZ"
	1184,
	// "_TIMESTAMPTZ"
	1185,
	// "INTERVAL"
	1186,
	// "_INTERVAL"
	1187,
	// "_NUMERIC"
	1231,
	// "TIMETZ"
	1266,
	// "_TIMETZ"
	1270,
	// "_BIT"
	1561,
	// "VARBIT"
	1562,
	// "_VARBIT"
	1563,
	// "REFCURSOR"
	1790,
	// "_REFCURSOR"
	2201,
	// "REGPROCEDURE"
	2202,
	// "REGOPER"
	2203,
	// "REGOPERATOR"
	2204,
	// "REGCLASS"
	2205,
	// "REGTYPE"
	2206,
	// "_REGPROCEDURE"
	2207,
	// "_REGOPER"
	2208,
	// "_REGOPERATOR"
	2209,
	// "_REGCLASS"
	2210,
	// "_REGTYPE"
	2211,
	// "RECORD"
	2249,
	// "CSTRING"
	2275,
	// "ANY"
	2276,
	// "ANYARRAY"
	2277,
	// "TRIGGER"
	2279,
	// "LANGUAGE_HANDLER"
	2280,
	// "INTERNAL"
	2281,
	// "OPAQUE"
	2282
	// none added by sqlite
};

Oid PQftype(const PGresult *res, int field_num) {

	// if the type is numeric then we're using a postgresql database and
	// typemangling is turned off, so we'll just return the type
	char	*columntype=res->sqlrcur->getColumnType(field_num);
	Oid	oid=atoi(columntype);
	if (oid) {
		return oid;
	}
	
	// if the type is not numeric, then we need to translate to a type
	// number
	for (int index=0; datatypestring[index]; index++) {
		if (!strcasecmp(datatypestring[index],columntype)) {
			return postgresqltypemap[index];
		}
	}

	// if it wasn't found, return 705 for unknown type
	return 705;
}

int PQfsize(const PGresult *res, int field_num) {
	// for char/varchar fields, return -1,
	// otherwise, return the column length
	Oid	oid=PQftype(res,field_num);
	return (oid==1042 || oid==1043)?
		-1:res->sqlrcur->getColumnLength(field_num);
}

int PQfmod(const PGresult *res, int field_num) {
	// for char/varchar fields, return the column length,
	// otherwise, return -1
	Oid	oid=PQftype(res,field_num);
	return (oid==1042 || oid==1043)?
		res->sqlrcur->getColumnLength(field_num):-1;
}

char *PQcmdStatus(PGresult *res) {
	// should return a string represeting the "command type" like:
	//	SELECT, INSERT, UPDATE, DROP, etc.
	if (res->queryisnotselect) {
		return "";
	} else {
		return "SELECT";
	}
}

char *PQoidStatus(const PGresult *res) {
	// return OID of tuple if query was an insert,
	// otherwise return InvalidOid
	return "InvalidOid";
}

Oid PQoidValue(const PGresult *res) {
	// return OID of tuple if query was an insert,
	// otherwise return InvalidOid
	return InvalidOid;
}

char *PQcmdTuples(PGresult *res) {
	return charstring::parseNumber((long)res->sqlrcur->affectedRows());
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

PGresult *PQmakeEmptyPGresult(PGconn *conn, ExecStatusType status) {
	// Make an empty PGresult with given status (some apps find this
	// useful). If conn is not NULL and status indicates an error, the
	// conn's errorMessage is copied.
	PGresult	*result=new PGresult;
	result->sqlrcur=NULL;
	result->execstatus=status;
	result->parent=conn;
	result->previousnonblockingmode=conn->nonblockingmode;
	return result;
}

}
