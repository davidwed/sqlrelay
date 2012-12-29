// Copyright (c) 2007  David Muse
// See the file COPYING for more information

#include <config.h>
#include <sql.h>
#include <sqlext.h>
#include <odbcinst.h>

#include <sqlrelay/sqlrclient.h>
#include <rudiments/rawbuffer.h>
#include <rudiments/linkedlist.h>
#include <rudiments/parameterstring.h>
#include <rudiments/charstring.h>
#include <rudiments/character.h>
#include <rudiments/environment.h>

#include <parsedatetime.h>

#ifndef SQL_NULL_DESC
	#define SQL_NULL_DESC 0
#endif

#ifdef SQLCOLATTRIBUTE_SQLLEN
typedef SQLLEN * NUMERICATTRIBUTETYPE;
#else
typedef SQLPOINTER NUMERICATTRIBUTETYPE;
#endif

#define ODBC_INI "odbc.ini"

#define DEBUG_MESSAGES 1
#ifdef DEBUG_MESSAGES
	#define debugFunction() printf("%s:%s():%d:\n",__FILE__,__FUNCTION__,__LINE__); fflush(stdout);
	#define debugPrintf(format, ...) printf(format, ## __VA_ARGS__); fflush(stdout);
#else
	#define debugFunction() /* */
	#define debugPrintf(format, ...) /* */
#endif

extern "C" {

static	uint16_t	stmtid=0;

class CONN;

struct ENV {
	SQLINTEGER		odbcversion;
	linkedlist< CONN * >	connlist;
	char			*error;
	int64_t			errn;
	const char		*sqlstate;
};

class STMT;

struct CONN {
	sqlrconnection		*con;
	ENV			*env;
	linkedlist< STMT * >	stmtlist;
	char			*error;
	int64_t			errn;
	const char		*sqlstate;
	char			server[1024];
	uint16_t		port;
	char			socket[1024];
	char			user[1024];
	char			password[1024];
	int32_t			retrytime;
	int32_t			tries;
};

struct rowdesc {
	STMT	*stmt;
};

struct paramdesc {
	STMT	*stmt;
};

struct FIELD {
	SQLSMALLINT	targettype;
	SQLPOINTER	targetvalue;
	SQLLEN		bufferlength;
	SQLLEN		*strlen_or_ind;
};

struct outputbind {
	SQLUSMALLINT	parameternumber;
	SQLSMALLINT	valuetype;
	SQLULEN		lengthprecision;
	SQLSMALLINT	parameterscale;
	SQLPOINTER	parametervalue;
	SQLLEN		bufferlength;
	SQLLEN		*strlen_or_ind;
};

struct STMT {
	sqlrcursor				*cur;
	uint64_t				currentfetchrow;
	uint64_t				currentgetdatarow;
	CONN					*conn;
	char					*name;
	char					*error;
	int64_t					errn;
	const char				*sqlstate;
	numericdictionary< FIELD * >		fieldlist;
	rowdesc					*approwdesc;
	paramdesc				*appparamdesc;
	rowdesc					*improwdesc;
	paramdesc				*impparamdesc;
	numericdictionary< char * >		inputbindstrings;
	numericdictionary< outputbind * >	outputbinds;
	SQLROWSETSIZE				*rowsfetchedptr;
	SQLUSMALLINT				*rowstatusptr;
};

static SQLRETURN SQLR_SQLAllocHandle(SQLSMALLINT handletype,
					SQLHANDLE inputhandle,
					SQLHANDLE *outputhandle);

SQLRETURN SQL_API SQLAllocConnect(SQLHENV environmenthandle,
					SQLHDBC *connectionhandle) {
	debugFunction();
	return SQLR_SQLAllocHandle(SQL_HANDLE_DBC,
				(SQLHANDLE)environmenthandle,
				(SQLHANDLE *)connectionhandle);
}

SQLRETURN SQL_API SQLAllocEnv(SQLHENV *environmenthandle) {
	debugFunction();
	return SQLR_SQLAllocHandle(SQL_HANDLE_ENV,NULL,
				(SQLHANDLE *)environmenthandle);
}

static void SQLR_ENVSetError(ENV *env, const char *error,
				int64_t errn, const char *sqlstate) {
	debugFunction();

	// set the error, convert NULL's to empty strings,
	// some apps have trouble with NULLS
	delete[] env->error;
	env->error=charstring::duplicate((error)?error:"");
	env->errn=errn;
	env->sqlstate=(sqlstate)?sqlstate:"";
	debugPrintf("error: %s\n",env->error);
	debugPrintf("errn: %lld\n",env->errn);
	debugPrintf("sqlstate: %s\n",env->sqlstate);
}

/*static void SQLR_ENVClearError(ENV *env) {
	debugFunction();
	SQLR_ENVSetError(env,NULL,0,NULL);
}*/

static void SQLR_CONNSetError(CONN *conn, const char *error,
				int64_t errn, const char *sqlstate) {
	debugFunction();

	// set the error, convert NULL's to empty strings,
	// some apps have trouble with NULLS
	delete[] conn->error;
	conn->error=charstring::duplicate((error)?error:"");
	conn->errn=errn;
	conn->sqlstate=(sqlstate)?sqlstate:"";
	debugPrintf("error: %s\n",conn->error);
	debugPrintf("errn: %lld\n",conn->errn);
	debugPrintf("sqlstate: %s\n",conn->sqlstate);
}

/*static void SQLR_CONNClearError(CONN *conn) {
	debugFunction();
	SQLR_CONNSetError(conn,NULL,0,NULL);
}*/

static void SQLR_STMTSetError(STMT *stmt, const char *error,
				int64_t errn, const char *sqlstate) {
	debugFunction();

	// set the error, convert NULL's to empty strings,
	// some apps have trouble with NULLS
	delete[] stmt->error;
	stmt->error=charstring::duplicate((error)?error:"");
	stmt->errn=errn;
	stmt->sqlstate=(sqlstate)?sqlstate:"";
	debugPrintf("error: %s\n",stmt->error);
	debugPrintf("errn: %lld\n",stmt->errn);
	debugPrintf("sqlstate: %s\n",stmt->sqlstate);
}

static void SQLR_STMTClearError(STMT *stmt) {
	debugFunction();
	SQLR_STMTSetError(stmt,NULL,0,NULL);
}

static SQLRETURN SQLR_SQLAllocHandle(SQLSMALLINT handletype,
					SQLHANDLE inputhandle,
					SQLHANDLE *outputhandle) {
	debugFunction();

	switch (handletype) {
		case SQL_HANDLE_ENV:
			{
			debugPrintf("handletype: SQL_HANDLE_ENV\n");
			ENV	*env=new ENV;
			env->odbcversion=0;
			*outputhandle=(SQLHANDLE)env;
			env->error=NULL;
			env->errn=0;
			env->sqlstate=NULL;
			return SQL_SUCCESS;
			}
		case SQL_HANDLE_DBC:
			{
			debugPrintf("handletype: SQL_HANDLE_DBC\n");
			ENV	*env=(ENV *)inputhandle;
			if (inputhandle==SQL_NULL_HENV || !env) {
				debugPrintf("NULL env handle\n");
				return SQL_INVALID_HANDLE;
			}
			CONN	*conn=new CONN;
			conn->con=NULL;
			*outputhandle=(SQLHANDLE)conn;
			conn->error=NULL;
			conn->errn=0;
			conn->sqlstate=NULL;
			env->connlist.append(conn);
			conn->env=env;
			return SQL_SUCCESS;
			}
		case SQL_HANDLE_STMT:
			{
			debugPrintf("handletype: SQL_HANDLE_STMT\n");
			CONN	*conn=(CONN *)inputhandle;
			if (inputhandle==SQL_NULL_HANDLE ||
						!conn || !conn->con) {
				debugPrintf("NULL conn handle\n");
				return SQL_INVALID_HANDLE;
			}
			STMT	*stmt=new STMT;
			stmt->cur=new sqlrcursor(conn->con);
			*outputhandle=(SQLHANDLE)stmt;
			stmt->currentfetchrow=0;
			stmt->currentgetdatarow=0;
			stmt->conn=conn;
			conn->stmtlist.append(stmt);
			stmt->name=NULL;
			stmt->error=NULL;
			stmt->errn=0;
			stmt->sqlstate=NULL;
			stmt->improwdesc=new rowdesc;
			stmt->improwdesc->stmt=stmt;
			stmt->impparamdesc=new paramdesc;
			stmt->impparamdesc->stmt=stmt;
			stmt->approwdesc=stmt->improwdesc;
			stmt->appparamdesc=stmt->impparamdesc;
			stmt->rowsfetchedptr=NULL;
			stmt->rowstatusptr=NULL;
			return SQL_SUCCESS;
			}
		case SQL_HANDLE_DESC:
			debugPrintf("handletype: SQL_HANDLE_DESC\n");
			// FIXME: no idea what to do here
			return SQL_ERROR;
		default:
			debugPrintf("invalid handletype\n");
			break;
	}
	return SQL_ERROR;
}

SQLRETURN SQL_API SQLAllocHandle(SQLSMALLINT handletype,
					SQLHANDLE inputhandle,
					SQLHANDLE *outputhandle) {
	debugFunction();
	return SQLR_SQLAllocHandle(handletype,inputhandle,outputhandle);
}

SQLRETURN SQL_API SQLAllocStmt(SQLHDBC connectionhandle,
					SQLHSTMT *statementhandle) {
	debugFunction();
	return SQLR_SQLAllocHandle(SQL_HANDLE_STMT,
				(SQLHANDLE)connectionhandle,
				(SQLHANDLE *)statementhandle);
}

SQLRETURN SQL_API SQLBindCol(SQLHSTMT statementhandle,
					SQLUSMALLINT columnnumber,
					SQLSMALLINT targettype,
					SQLPOINTER targetvalue,
					SQLLEN bufferlength,
					SQLLEN *strlen_or_ind) {
	debugFunction();
	debugPrintf("columnnumber: %d\n",(int)columnnumber);

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	if (columnnumber<1) {
		SQLR_STMTSetError(stmt,NULL,0,"07009");
		return SQL_ERROR;
	}

	FIELD	*field=new FIELD;
	field->targettype=targettype;
	field->targetvalue=targetvalue;
	field->bufferlength=bufferlength;
	field->strlen_or_ind=strlen_or_ind;

	stmt->fieldlist.setData(columnnumber-1,field);

	return SQL_SUCCESS;
}

static SQLRETURN SQLR_SQLBindParameter(SQLHSTMT statementhandle,
					SQLUSMALLINT parameternumber,
					SQLSMALLINT inputoutputtype,
					SQLSMALLINT valuetype,
					SQLSMALLINT parametertype,
					SQLULEN lengthprecision,
					SQLSMALLINT parameterscale,
					SQLPOINTER parametervalue,
					SQLLEN bufferlength,
					SQLLEN *strlen_or_ind);

SQLRETURN SQL_API SQLBindParam(SQLHSTMT statementhandle,
					SQLUSMALLINT parameternumber,
					SQLSMALLINT valuetype,
					SQLSMALLINT parametertype,
					SQLULEN lengthprecision,
					SQLSMALLINT parameterscale,
					SQLPOINTER parametervalue,
					SQLLEN *strlen_or_ind) {
	debugFunction();
	return SQLR_SQLBindParameter(statementhandle,
					parameternumber,
					SQL_PARAM_INPUT,
					valuetype,
					parametertype,
					lengthprecision,
					parameterscale,
					parametervalue,
					0,
					strlen_or_ind);
}

SQLRETURN SQL_API SQLCancel(SQLHSTMT statementhandle) {
	debugFunction();

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	// not supported
	SQLR_STMTSetError(stmt,NULL,0,"IM001");
	return SQL_ERROR;
}

static void SQLR_ResetParams(STMT *stmt) {
	debugFunction();

	// clear bind variables
	stmt->cur->clearBinds();

	// clear input bind list
	numericdictionarylist< char * >	*ibslist=
					stmt->inputbindstrings.getList();
	for (dictionarylistnode< int32_t, char * > *node=
						ibslist->getFirstNode();
		node; node=(dictionarylistnode< int32_t, char * > *)
							node->getNext()) {
		delete[] node->getData();
	}
	ibslist->clear();

	// clear output bind list
	numericdictionarylist< outputbind * >	*oblist=
					stmt->outputbinds.getList();
	for (dictionarylistnode< int32_t, outputbind * > *node=
						oblist->getFirstNode();
		node; node=(dictionarylistnode< int32_t, outputbind * > *)
							node->getNext()) {
		delete node->getData();
	}
	oblist->clear();
}

static SQLRETURN SQLR_SQLCloseCursor(SQLHSTMT statementhandle) {
	debugFunction();

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	SQLR_ResetParams(stmt);
	stmt->cur->closeResultSet();

	return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLCloseCursor(SQLHSTMT statementhandle) {
	debugFunction();
	return SQLR_SQLCloseCursor(statementhandle);
}

static SQLSMALLINT SQLR_MapColumnType(sqlrcursor *cur, uint32_t col) {
	const char	*ctype=cur->getColumnType(col);
	if (!charstring::compare(ctype,"UNKNOWN")) {
		return SQL_UNKNOWN_TYPE;
	} else if (!charstring::compare(ctype,"CHAR")) {
		return SQL_CHAR;
	} else if (!charstring::compare(ctype,"INT")) {
		return SQL_INTEGER;
	} else if (!charstring::compare(ctype,"SMALLINT")) {
		return SQL_SMALLINT;
	} else if (!charstring::compare(ctype,"TINYINT")) {
		return SQL_TINYINT;
	} else if (!charstring::compare(ctype,"MONEY")) {
		return SQL_CHAR;
	} else if (!charstring::compare(ctype,"DATETIME")) {
		return SQL_DATETIME;
	} else if (!charstring::compare(ctype,"NUMERIC")) {
		return SQL_NUMERIC;
	} else if (!charstring::compare(ctype,"DECIMAL")) {
		return SQL_DECIMAL;
	} else if (!charstring::compare(ctype,"SMALLDATETIME")) {
		return SQL_TIMESTAMP;
	} else if (!charstring::compare(ctype,"SMALLMONEY")) {
		return SQL_CHAR;
	} else if (!charstring::compare(ctype,"IMAGE")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"BINARY")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"BIT")) {
		return SQL_BIT;
	} else if (!charstring::compare(ctype,"REAL")) {
		return SQL_REAL;
	} else if (!charstring::compare(ctype,"FLOAT")) {
		return SQL_FLOAT;
	} else if (!charstring::compare(ctype,"TEXT")) {
		return SQL_CHAR;
	} else if (!charstring::compare(ctype,"VARCHAR")) {
		return SQL_VARCHAR;
	} else if (!charstring::compare(ctype,"VARBINARY")) {
		return SQL_VARBINARY;
	} else if (!charstring::compare(ctype,"LONGCHAR")) {
		return SQL_CHAR;
	} else if (!charstring::compare(ctype,"LONGBINARY")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"LONG")) {
		return SQL_CHAR;
	} else if (!charstring::compare(ctype,"ILLEGAL")) {
		return SQL_CHAR;
	} else if (!charstring::compare(ctype,"SENSITIVITY")) {
		return SQL_CHAR;
	} else if (!charstring::compare(ctype,"BOUNDARY")) {
		return SQL_CHAR;
	} else if (!charstring::compare(ctype,"VOID")) {
		return SQL_CHAR;
	} else if (!charstring::compare(ctype,"USHORT")) {
		return SQL_SMALLINT;

	// added by lago
	} else if (!charstring::compare(ctype,"UNDEFINED")) {
		return SQL_UNKNOWN_TYPE;
	} else if (!charstring::compare(ctype,"DOUBLE")) {
		return SQL_DOUBLE;
	} else if (!charstring::compare(ctype,"DATE")) {
		return SQL_DATETIME;
	} else if (!charstring::compare(ctype,"TIME")) {
		return SQL_TIME;
	} else if (!charstring::compare(ctype,"TIMESTAMP")) {
		return SQL_TIMESTAMP;
	// added by msql
	} else if (!charstring::compare(ctype,"UINT")) {
		return SQL_INTEGER;
	} else if (!charstring::compare(ctype,"LASTREAL")) {
		return SQL_REAL;
	// added by mysql
	} else if (!charstring::compare(ctype,"STRING")) {
		return SQL_CHAR;
	} else if (!charstring::compare(ctype,"VARSTRING")) {
		return SQL_VARCHAR;
	} else if (!charstring::compare(ctype,"LONGLONG")) {
		return SQL_BIGINT;
	} else if (!charstring::compare(ctype,"MEDIUMINT")) {
		return SQL_INTEGER;
	} else if (!charstring::compare(ctype,"YEAR")) {
		return SQL_SMALLINT;
	} else if (!charstring::compare(ctype,"NEWDATE")) {
		return SQL_DATETIME;
	} else if (!charstring::compare(ctype,"NULL")) {
		return SQL_CHAR;
	} else if (!charstring::compare(ctype,"ENUM")) {
		return SQL_CHAR;
	} else if (!charstring::compare(ctype,"SET")) {
		return SQL_CHAR;
	} else if (!charstring::compare(ctype,"TINYBLOB")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"MEDIUMBLOB")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"LONGBLOB")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"BLOB")) {
		return SQL_BINARY;

	// added by oracle
	} else if (!charstring::compare(ctype,"VARCHAR2")) {
		return SQL_VARCHAR;
	} else if (!charstring::compare(ctype,"NUMBER")) {
		return SQL_NUMERIC;
	} else if (!charstring::compare(ctype,"ROWID")) {
		return SQL_BIGINT;
	} else if (!charstring::compare(ctype,"RAW")) {
		return SQL_VARBINARY;
	} else if (!charstring::compare(ctype,"LONG_RAW")) {
		return SQL_LONGVARBINARY;
	} else if (!charstring::compare(ctype,"MLSLABEL")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"CLOB")) {
		return SQL_LONGVARCHAR;
	} else if (!charstring::compare(ctype,"BFILE")) {
		return SQL_LONGVARBINARY;

	// added by odbc
	} else if (!charstring::compare(ctype,"BIGINT")) {
		return SQL_BIGINT;
	} else if (!charstring::compare(ctype,"INTEGER")) {
		return SQL_INTEGER;
	} else if (!charstring::compare(ctype,"LONGVARBINARY")) {
		return SQL_LONGVARBINARY;
	} else if (!charstring::compare(ctype,"LONGVARCHAR")) {
		return SQL_LONGVARCHAR;

	// added by db2
	} else if (!charstring::compare(ctype,"GRAPHIC")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"VARGRAPHIC")) {
		return SQL_VARBINARY;
	} else if (!charstring::compare(ctype,"LONGVARGRAPHIC")) {
		return SQL_LONGVARBINARY;
	} else if (!charstring::compare(ctype,"DBCLOB")) {
		return SQL_LONGVARCHAR;
	} else if (!charstring::compare(ctype,"DATALINK")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"USER_DEFINED_TYPE")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"SHORT_DATATYPE")) {
		return SQL_SMALLINT;
	} else if (!charstring::compare(ctype,"TINY_DATATYPE")) {
		return SQL_TINYINT;

	// added by firebird
	} else if (!charstring::compare(ctype,"D_FLOAT")) {
		return SQL_DOUBLE;
	} else if (!charstring::compare(ctype,"ARRAY")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"QUAD")) {
		return SQL_BIGINT;
	} else if (!charstring::compare(ctype,"INT64")) {
		return SQL_BIGINT;
	} else if (!charstring::compare(ctype,"DOUBLE PRECISION")) {
		return SQL_DOUBLE;

	// added by postgresql
	} else if (!charstring::compare(ctype,"BOOL")) {
		return SQL_CHAR;
	} else if (!charstring::compare(ctype,"BYTEA")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"NAME")) {
		return SQL_CHAR;
	} else if (!charstring::compare(ctype,"INT8")) {
		return SQL_BIGINT;
	} else if (!charstring::compare(ctype,"INT2")) {
		return SQL_SMALLINT;
	} else if (!charstring::compare(ctype,"INT2VECTOR")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"INT4")) {
		return SQL_INTEGER;
	} else if (!charstring::compare(ctype,"REGPROC")) {
		return SQL_BIGINT;
	} else if (!charstring::compare(ctype,"OID")) {
		return SQL_BIGINT;
	} else if (!charstring::compare(ctype,"TID")) {
		return SQL_BIGINT;
	} else if (!charstring::compare(ctype,"XID")) {
		return SQL_BIGINT;
	} else if (!charstring::compare(ctype,"CID")) {
		return SQL_BIGINT;
	} else if (!charstring::compare(ctype,"OIDVECTOR")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"SMGR")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"POINT")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"LSEG")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"PATH")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"BOX")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"POLYGON")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"LINE")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"LINE_ARRAY")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"FLOAT4")) {
		return SQL_FLOAT;
	} else if (!charstring::compare(ctype,"FLOAT8")) {
		return SQL_DOUBLE;
	} else if (!charstring::compare(ctype,"ABSTIME")) {
		return SQL_INTEGER;
	} else if (!charstring::compare(ctype,"RELTIME")) {
		return SQL_INTEGER;
	} else if (!charstring::compare(ctype,"TINTERVAL")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"CIRCLE")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"CIRCLE_ARRAY")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"MONEY_ARRAY")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"MACADDR")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"INET")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"CIDR")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"BOOL_ARRAY")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"BYTEA_ARRAY")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"CHAR_ARRAY")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"NAME_ARRAY")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"INT2_ARRAY")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"INT2VECTOR_ARRAY")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"INT4_ARRAY")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"REGPROC_ARRAY")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"TEXT_ARRAY")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"OID_ARRAY")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"TID_ARRAY")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"XID_ARRAY")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"CID_ARRAY")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"OIDVECTOR_ARRAY")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"BPCHAR_ARRAY")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"VARCHAR_ARRAY")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"INT8_ARRAY")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"POINT_ARRAY")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"LSEG_ARRAY")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"PATH_ARRAY")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"BOX_ARRAY")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"FLOAT4_ARRAY")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"FLOAT8_ARRAY")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"ABSTIME_ARRAY")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"RELTIME_ARRAY")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"TINTERVAL_ARRAY")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"POLYGON_ARRAY")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"ACLITEM")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"ACLITEM_ARRAY")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"MACADDR_ARRAY")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"INET_ARRAY")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"CIDR_ARRAY")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"BPCHAR")) {
		return SQL_CHAR;
	} else if (!charstring::compare(ctype,"TIMESTAMP_ARRAY")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"DATE_ARRAY")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"TIME_ARRAY")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"TIMESTAMPTZ")) {
		return SQL_TIMESTAMP;
	} else if (!charstring::compare(ctype,"TIMESTAMPTZ_ARRAY")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"INTERVAL")) {
		return SQL_INTERVAL;
	} else if (!charstring::compare(ctype,"INTERVAL_ARRAY")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"NUMERIC_ARRAY")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"TIMETZ")) {
		return SQL_TIME;
	} else if (!charstring::compare(ctype,"TIMETZ_ARRAY")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"BIT_ARRAY")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"VARBIT")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"VARBIT_ARRAY")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"REFCURSOR")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"REFCURSOR_ARRAY")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"REGPROCEDURE")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"REGOPER")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"REGOPERATOR")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"REGCLASS")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"REGTYPE")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"REGPROCEDURE_ARRAY")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"REGOPER_ARRAY")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"REGOPERATOR_ARRAY")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"REGCLASS_ARRAY")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"REGTYPE_ARRAY")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"RECORD")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"CSTRING")) {
		return SQL_CHAR;
	} else if (!charstring::compare(ctype,"ANY")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"ANYARRAY")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"TRIGGER")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"LANGUAGE_HANDLER")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"INTERNAL")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"OPAQUE")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"ANYELEMENT")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"PG_TYPE")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"PG_ATTRIBUTE")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"PG_PROC")) {
		return SQL_BINARY;
	} else if (!charstring::compare(ctype,"PG_CLASS")) {
		return SQL_BINARY;
	}
	return SQL_CHAR;
}

static SQLSMALLINT SQLR_MapCColumnType(sqlrcursor *cur, uint32_t col) {
	switch (SQLR_MapColumnType(cur,col)) {
		case SQL_UNKNOWN_TYPE:
			return SQL_C_CHAR;
		case SQL_CHAR:
			return SQL_C_CHAR;
		case SQL_NUMERIC:
			return SQL_C_CHAR;
		case SQL_DECIMAL:
			return SQL_C_CHAR;
		case SQL_INTEGER:
			return SQL_C_LONG;
		case SQL_SMALLINT:
			return SQL_C_SHORT;
		case SQL_FLOAT:
			return SQL_C_FLOAT;
		case SQL_REAL:
			return SQL_C_FLOAT;
		case SQL_DOUBLE:
			return SQL_C_DOUBLE;
		case SQL_DATETIME:
			return SQL_C_TIMESTAMP;
		case SQL_VARCHAR:
			return SQL_C_CHAR;
		case SQL_TYPE_DATE:
			return SQL_C_DATE;
		case SQL_TYPE_TIME:
			return SQL_C_TIME;
		case SQL_TYPE_TIMESTAMP:
			return SQL_C_TIMESTAMP;
		// dup of SQL_TIME
		// case SQL_INTERVAL:
		case SQL_TIME:
			return SQL_C_TIME;
		case SQL_TIMESTAMP:
			return SQL_C_TIMESTAMP;
		case SQL_LONGVARCHAR:
			return SQL_C_CHAR;
		case SQL_BINARY:
			return SQL_C_BINARY;
		case SQL_VARBINARY:
			return SQL_C_BINARY;
		case SQL_LONGVARBINARY:
			return SQL_C_BINARY;
		case SQL_BIGINT:
			return SQL_C_SBIGINT;
		case SQL_TINYINT:
			return SQL_C_TINYINT;
		case SQL_BIT:
			return SQL_C_BIT;
		case SQL_GUID:
			return SQL_C_GUID;
	}
	return SQL_C_CHAR;
}

static SQLRETURN SQLR_SQLColAttribute(SQLHSTMT statementhandle,
					SQLUSMALLINT columnnumber,
					SQLUSMALLINT fieldidentifier,
					SQLPOINTER characterattribute,
					SQLSMALLINT bufferlength,
					SQLSMALLINT *stringlength,
					NUMERICATTRIBUTETYPE numericattribute) {
	debugFunction();
	debugPrintf("columnnumber: %d\n",(int)columnnumber);
	debugPrintf("bufferlength: %d\n",(int)bufferlength);

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	// initialize the stringlength and buffer to safe return values
	// in case the app uses the result without checking the error code
	charstring::safeCopy((char *)characterattribute,bufferlength,"");
	if (stringlength) {
		*stringlength=0;
	}
	if (numericattribute) {
		// this will cause a problem if something smaller than 
		// SQLSMALLINT is passed in but nobody should be doing that
		*(SQLSMALLINT *)numericattribute=0;
	}

	// make sure we're attempting to get a valid column
	uint32_t	colcount=stmt->cur->colCount();
	debugPrintf("colcount: %d\n",(int)colcount);
	if (columnnumber<1 || columnnumber>colcount) {
		SQLR_STMTSetError(stmt,NULL,0,"07009");
		return SQL_ERROR;
	}

	// get a zero-based version of the columnnumber
	uint32_t	col=columnnumber-1;

	switch (fieldidentifier) {
		case SQL_DESC_COUNT:
		case SQL_COLUMN_COUNT:
			debugPrintf("fieldidentifier: "
					"SQL_DESC/COLUMN_COUNT\n");
			*(SQLSMALLINT *)numericattribute=colcount;
			debugPrintf("count: %lld\n",
					(int64_t)*numericattribute);
			break;
		case SQL_DESC_TYPE:
		//case SQL_DESC_CONCISE_TYPE:
		case SQL_COLUMN_TYPE:
			debugPrintf("fieldidentifier: "
					"SQL_DESC_TYPE/"
					"SQL_DESC_CONCISE_TYPE/"
					"COLUMN_TYPE\n");
			*(SQLSMALLINT *)numericattribute=
					SQLR_MapColumnType(stmt->cur,col);
			debugPrintf("type: %lld\n",
					(int64_t)*numericattribute);
			break;
		case SQL_DESC_LENGTH:
		case SQL_DESC_OCTET_LENGTH:
		case SQL_COLUMN_LENGTH:
			debugPrintf("fieldidentifier: "
					"SQL_DESC_LENGTH/COLUMN_LENGTH/"
					"SQL_DESC_OCTET_LENGTH\n");
			*(SQLINTEGER *)numericattribute=
					stmt->cur->getColumnLength(col);
			debugPrintf("length: %lld\n",
					(int64_t)*numericattribute);
			break;
		case SQL_DESC_PRECISION:
		case SQL_COLUMN_PRECISION:
			debugPrintf("fieldidentifier: "
					"SQL_DESC/COLUMN_PRECISION\n");
			*(SQLSMALLINT *)numericattribute=
					stmt->cur->getColumnPrecision(col);
			debugPrintf("precision: %lld\n",
					(int64_t)*numericattribute);
			break;
		case SQL_DESC_SCALE:
		case SQL_COLUMN_SCALE:
			debugPrintf("fieldidentifier: "
					"SQL_DESC/COLUMN_SCALE\n");
			*(SQLSMALLINT *)numericattribute=
					stmt->cur->getColumnScale(col);
			debugPrintf("scale: %lld\n",
					(int64_t)*numericattribute);
			break;
		case SQL_DESC_NULLABLE:
		case SQL_COLUMN_NULLABLE:
			debugPrintf("fieldidentifier: "
					"SQL_DESC/COLUMN_NULLABLE\n");
			*(SQLSMALLINT *)numericattribute=
					stmt->cur->getColumnIsNullable(col);
			debugPrintf("nullable: %lld\n",
					(int64_t)*numericattribute);
			break;
		case SQL_DESC_NAME:
		case SQL_COLUMN_NAME:
			debugPrintf("fieldidentifier: "
					"SQL_DESC/COLUMN_NAME\n");
			{
			// SQL Relay doesn't know about column aliases,
			// just return the name.
			const char *name=stmt->cur->getColumnName(col);
			charstring::safeCopy((char *)characterattribute,
							bufferlength,name);
			debugPrintf("name: \"%s\"\n",
					(const char *)characterattribute);
			if (stringlength) {
				*stringlength=charstring::length(name);
				debugPrintf("length: %d\n",(int)*stringlength);
			}
			}
			break;
		case SQL_DESC_UNNAMED:
			debugPrintf("fieldidentifier: "
					"SQL_DESC_UNNAMED\n");
			if (charstring::length(stmt->cur->getColumnName(col))) {
				*(SQLSMALLINT *)numericattribute=SQL_NAMED;
			} else {
				*(SQLSMALLINT *)numericattribute=SQL_UNNAMED;
			} 
			debugPrintf("unnamed: %lld\n",
					(int64_t)*numericattribute);
			break;
		//case SQL_DESC_AUTO_UNIQUE_VALUE:
		case SQL_COLUMN_AUTO_INCREMENT:
			debugPrintf("fieldidentifier: "
					"SQL_DESC_AUTO_UNIQUE_VALUE/"
					"SQL_COLUMN_AUTO_INCREMENT\n");
			*(SQLINTEGER *)numericattribute=stmt->cur->
					getColumnIsAutoIncrement(col);
			debugPrintf("auto-increment: %lld\n",
					(int64_t)*numericattribute);
			break;
		case SQL_DESC_BASE_COLUMN_NAME:
			debugPrintf("fieldidentifier: "
					"SQL_DESC_BASE_COLUMN_NAME\n");
			// SQL Relay doesn't know this, in particular, return
			// an empty string.
			charstring::safeCopy((char *)characterattribute,
							bufferlength,"");
			debugPrintf("base column name: \"%s\"\n",
					(const char *)characterattribute);
			if (stringlength) {
				*stringlength=0;
				debugPrintf("length: %d\n",(int)*stringlength);
			}
			break;
		case SQL_DESC_BASE_TABLE_NAME:
			debugPrintf("fieldidentifier: "
					"SQL_DESC_BASE_TABLE_NAME\n");
			// SQL Relay doesn't know this, return an empty string.
			charstring::safeCopy((char *)characterattribute,
							bufferlength,"");
			debugPrintf("base table name: \"%s\"\n",
					(const char *)characterattribute);
			if (stringlength) {
				*stringlength=0;
				debugPrintf("length: %d\n",(int)*stringlength);
			}
			break;
		//case SQL_DESC_CASE_SENSITIVE:
		case SQL_COLUMN_CASE_SENSITIVE:
			debugPrintf("fieldidentifier: "
					"SQL_DESC/COLUMN_CASE_SENSITIVE\n");
			// not supported, return true
			*(SQLSMALLINT *)numericattribute=SQL_TRUE;
			debugPrintf("case sensitive: %lld\n",
					(int64_t)*numericattribute);
			break;
		//case SQL_DESC_CATALOG_NAME:
		case SQL_COLUMN_QUALIFIER_NAME:
			debugPrintf("fieldidentifier: "
					"SQL_DESC_CATALOG_NAME/"
					"SQL_COLUMN_QUALIFIER_NAME\n");
			// not supported, return empty string
			charstring::safeCopy((char *)characterattribute,
							bufferlength,"");
			debugPrintf("column qualifier name: \"%s\"\n",
					(const char *)characterattribute);
			if (stringlength) {
				*stringlength=0;
				debugPrintf("length: %d\n",(int)*stringlength);
			}
			break;
		//case SQL_DESC_DISPLAY_SIZE:
		case SQL_COLUMN_DISPLAY_SIZE:
			debugPrintf("fieldidentifier: "
					"SQL_DESC/COLUMN_DISPLAY_SIZE\n");
			*(SQLLEN *)numericattribute=stmt->cur->getLongest(col);
			debugPrintf("display size: %lld\n",
					(int64_t)*numericattribute);
			break;
		//case SQL_DESC_FIXED_PREC_SCALE
		case SQL_COLUMN_MONEY:
			{
			debugPrintf("fieldidentifier: "
					"SQL_DESC_FIXED_PREC_SCALE/"
					"SQL_COLUMN_MONEY\n");
			const char	*type=stmt->cur->getColumnType(col);
			debugPrintf("fixed prec scale: ");
			if (!charstring::compareIgnoringCase(
							type,"money") ||
				!charstring::compareIgnoringCase(
							type,"smallmoney")) {
				*(SQLSMALLINT *)numericattribute=SQL_TRUE;
				debugPrintf("true\n");
			} else {
				*(SQLSMALLINT *)numericattribute=SQL_FALSE;
				debugPrintf("false\n");
			}
			}
			break;
		//case SQL_DESC_LABEL
		case SQL_COLUMN_LABEL:
			{
			debugPrintf("fieldidentifier: "
					"SQL_DESC/COLUMN_LABEL\n");
			const char *name=stmt->cur->getColumnName(col);
			charstring::safeCopy((char *)characterattribute,
							bufferlength,name);
			debugPrintf("label: \"%s\"\n",
					(const char *)characterattribute);
			if (stringlength) {
				*stringlength=charstring::length(name);
				debugPrintf("length: %d\n",(int)*stringlength);
			}
			}
			break;
		case SQL_DESC_LITERAL_PREFIX:
			{
			debugPrintf("fieldidentifier: "
					"SQL_DESC_LITERAL_PREFIX\n");
			// single-quote for char, 0x for binary
			SQLSMALLINT	type=SQLR_MapColumnType(stmt->cur,col);
			if (type==SQL_CHAR ||
				type==SQL_VARCHAR ||
				type==SQL_LONGVARCHAR) {
				charstring::safeCopy((char *)characterattribute,
							bufferlength,"'");
				if (stringlength) {
					*stringlength=1;
				}
			} else if (type==SQL_BINARY ||
					type==SQL_VARBINARY ||
					type==SQL_LONGVARBINARY) {
				charstring::safeCopy((char *)characterattribute,
							bufferlength,"0x");
				if (stringlength) {
					*stringlength=2;
				}
			} else {
				charstring::safeCopy((char *)characterattribute,
							bufferlength,"");
				if (stringlength) {
					*stringlength=0;
				}
			}
			debugPrintf("literal prefix: %s\n",
					(const char *)characterattribute);
			if (stringlength) {
				debugPrintf("length: %d\n",(int)*stringlength);
			}
			}
			break;
		case SQL_DESC_LITERAL_SUFFIX:
			{
			debugPrintf("fieldidentifier: "
					"SQL_DESC_LITERAL_SUFFIX\n");
			// single-quote for char
			SQLSMALLINT	type=SQLR_MapColumnType(stmt->cur,col);
			if (type==SQL_CHAR ||
				type==SQL_VARCHAR ||
				type==SQL_LONGVARCHAR) {
				charstring::safeCopy((char *)characterattribute,
							bufferlength,"'");
				if (stringlength) {
					*stringlength=1;
				}
			} else {
				charstring::safeCopy((char *)characterattribute,
							bufferlength,"");
				if (stringlength) {
					*stringlength=0;
				}
			}
			debugPrintf("literal prefix: %s\n",
					(const char *)characterattribute);
			if (stringlength) {
				debugPrintf("length: %d\n",(int)*stringlength);
			}
			}
			break;
		case SQL_DESC_LOCAL_TYPE_NAME:
			{
			debugPrintf("fieldidentifier: "
					"SQL_DESC_LOCAL_TYPE_NAME\n");
			const char *name=stmt->cur->getColumnType(col);
			charstring::safeCopy((char *)characterattribute,
							bufferlength,name);
			debugPrintf("local type name: \"%s\"\n",
					(const char *)characterattribute);
			if (stringlength) {
				*stringlength=charstring::length(name);
				debugPrintf("length: %d\n",(int)*stringlength);
			}
			}
			break;
		case SQL_DESC_NUM_PREC_RADIX:
			debugPrintf("fieldidentifier: "
					"SQL_DESC_NUM_PREC_RADIX\n");
			// FIXME: 2 for approximate numeric types,
			// 10 for exact numeric types, 0 otherwise
			*(SQLINTEGER *)numericattribute=0;
			debugPrintf("num prec radix: %lld\n",
					(int64_t)*numericattribute);
			break;
		//case SQL_DESC_SCHEMA_NAME
		case SQL_COLUMN_OWNER_NAME:
			debugPrintf("fieldidentifier: "
					"SQL_DESC_SCHEMA_NAME/"
					"SQL_COLUMN_OWNER_NAME\n");
			// SQL Relay doesn't know this, return an empty string.
			charstring::safeCopy((char *)characterattribute,
							bufferlength,"");
			debugPrintf("owner name: \"%s\"\n",
					(const char *)characterattribute);
			if (stringlength) {
				*stringlength=0;
				debugPrintf("length: %d\n",(int)*stringlength);
			}
			break;
		//case SQL_DESC_SEARCHABLE
		case SQL_COLUMN_SEARCHABLE:
			debugPrintf("fieldidentifier: "
					"SQL_DESC/COLUMN_SEARCHABLE\n");
			// not supported, return searchable
			*(SQLINTEGER *)numericattribute=SQL_SEARCHABLE;
			debugPrintf("updatable: SQL_SEARCHABLE\n");
			break;
		//case SQL_DESC_TYPE_NAME
		case SQL_COLUMN_TYPE_NAME:
			debugPrintf("fieldidentifier: "
					"SQL_DESC/COLUMN_TYPE_NAME\n");
			{
			debugPrintf("fieldidentifier: "
					"SQL_DESC_LOCAL_TYPE_NAME\n");
			const char *name=stmt->cur->getColumnType(col);
			charstring::safeCopy((char *)characterattribute,
							bufferlength,name);
			debugPrintf("type name: \"%s\"\n",
					(const char *)characterattribute);
			if (stringlength) {
				*stringlength=charstring::length(name);
				debugPrintf("length: %d\n",(int)*stringlength);
			}
			}
			break;
		//case SQL_DESC_TABLE_NAME
		case SQL_COLUMN_TABLE_NAME:
			debugPrintf("fieldidentifier: "
					"SQL_DESC/COLUMN_TABLE_NAME\n");
			// not supported, return an empty string
			charstring::safeCopy((char *)characterattribute,
							bufferlength,"");
			debugPrintf("table name: \"%s\"\n",
					(const char *)characterattribute);
			if (stringlength) {
				*stringlength=0;
				debugPrintf("length: %d\n",(int)*stringlength);
			}
			break;
		//case SQL_DESC_UNSIGNED
		case SQL_COLUMN_UNSIGNED:
			debugPrintf("fieldidentifier: "
					"SQL_DESC/COLUMN_UNSIGNED\n");
			*(SQLSMALLINT *)numericattribute=
					stmt->cur->getColumnIsUnsigned(col);
			debugPrintf("unsigned: %lld\n",
					(int64_t)*numericattribute);
			break;
		//case SQL_DESC_UPDATABLE
		case SQL_COLUMN_UPDATABLE:
			debugPrintf("fieldidentifier: "
					"SQL_DESC/COLUMN_UPDATEABLE\n");
			// not supported, return unknown
			*(SQLINTEGER *)numericattribute=
					SQL_ATTR_READWRITE_UNKNOWN;
			debugPrintf("updatable: SQL_ATTR_READWRITE_UNKNOWN\n");
			break;
		#if (ODBCVER < 0x0300)
		case SQL_COLUMN_DRIVER_START:
			debugPrintf("fieldidentifier: "
					"SQL_COLUMN_DRIVER_START\n");
			// not supported, return 0
			*(SQLINTEGER *)numericattribute=0;
			debugPrintf("driver start: %lld\n",
					(int64_t)*numericattribute);
			break;
		#endif
		default:
			debugPrintf("invalid valuetype\n");
			return SQL_ERROR;
	}

	return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLColAttribute(SQLHSTMT statementhandle,
					SQLUSMALLINT columnnumber,
					SQLUSMALLINT fieldidentifier,
					SQLPOINTER characterattribute,
					SQLSMALLINT bufferlength,
					SQLSMALLINT *stringlength,
					NUMERICATTRIBUTETYPE numericattribute) {
	debugFunction();
	return SQLR_SQLColAttribute(statementhandle,
					columnnumber,
					fieldidentifier,
					characterattribute,
					bufferlength,
					stringlength,
					numericattribute);
}

static void SQLR_BuildTableName(stringbuffer *table,
				SQLCHAR *catalogname,
				SQLSMALLINT namelength1,
				SQLCHAR *schemaname,
				SQLSMALLINT namelength2,
				SQLCHAR *tablename,
				SQLSMALLINT namelength3) {
	debugFunction();
	if (namelength1) {
		if (namelength1==SQL_NTS) {
			table->append(catalogname);
		} else {
			table->append(catalogname,namelength1);
		}
	}
	if (namelength2) {
		if (table->getStringLength()) {
			table->append('.');
		}
		if (namelength2==SQL_NTS) {
			table->append(schemaname);
		} else {
			table->append(schemaname,namelength2);
		}
	}
	if (namelength3) {
		if (table->getStringLength()) {
			table->append('.');
		}
		if (namelength3==SQL_NTS) {
			table->append(tablename);
		} else {
			table->append(tablename,namelength3);
		}
	}
}

SQLRETURN SQL_API SQLColumns(SQLHSTMT statementhandle,
					SQLCHAR *catalogname,
					SQLSMALLINT namelength1,
					SQLCHAR *schemaname,
					SQLSMALLINT namelength2,
					SQLCHAR *tablename,
					SQLSMALLINT namelength3,
					SQLCHAR *columnname,
					SQLSMALLINT namelength4) {
	debugFunction();

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	stringbuffer	table;
	SQLR_BuildTableName(&table,catalogname,namelength1,
				schemaname,namelength2,tablename,namelength3);

	char	*wild=NULL;
	if (namelength4==SQL_NTS) {
		wild=charstring::duplicate((const char *)columnname);
	} else {
		wild=charstring::duplicate((const char *)columnname,
							namelength4);
	}

	debugPrintf("table: %s\n",table.getString());
	debugPrintf("wild: %s\n",(wild)?wild:"");

	SQLRETURN	retval=
			(stmt->cur->getColumnList(table.getString(),wild))?
							SQL_SUCCESS:SQL_ERROR;
	delete[] wild;
	return retval;
}


static SQLRETURN SQLR_SQLConnect(SQLHDBC connectionhandle,
					SQLCHAR *dsn,
					SQLSMALLINT dsnlength,
					SQLCHAR *user,
					SQLSMALLINT userlength,
					SQLCHAR *password,
					SQLSMALLINT passwordlength) {
	debugFunction();

	CONN	*conn=(CONN *)connectionhandle;
	if (connectionhandle==SQL_NULL_HANDLE || !conn) {
		debugPrintf("NULL conn handle\n");
		return SQL_INVALID_HANDLE;
	}

	// get data from dsn
	SQLGetPrivateProfileString((const char *)dsn,"Server","",
					conn->server,sizeof(conn->server),
					ODBC_INI);
	char	portbuf[6];
	SQLGetPrivateProfileString((const char *)dsn,"Port","",
					portbuf,sizeof(portbuf),ODBC_INI);
	conn->port=(uint16_t)charstring::toUnsignedInteger(portbuf);
	SQLGetPrivateProfileString((const char *)dsn,"Socket","",
					conn->socket,sizeof(conn->socket),
					ODBC_INI);
	if (charstring::length(user)) {
		if (userlength==SQL_NTS) {
			charstring::safeCopy(conn->user,
						sizeof(conn->user),
						(const char *)user);
		} else {
			charstring::safeCopy(conn->user,
						sizeof(conn->user),
						(const char *)user,
						userlength);
		}
	} else {
		SQLGetPrivateProfileString((const char *)dsn,"User","",
					conn->user,sizeof(conn->user),
					ODBC_INI);
	}
	if (charstring::length(password)) {
		if (passwordlength==SQL_NTS) {
			charstring::safeCopy(conn->password,
						sizeof(conn->password),
						(const char *)password,
						passwordlength);
		} else {
			charstring::safeCopy(conn->password,
						sizeof(conn->password),
						(const char *)password);
		}
	} else {
		SQLGetPrivateProfileString((const char *)dsn,"Password","",
					conn->password,sizeof(conn->password),
					ODBC_INI);
	}
	char	retrytimebuf[6];
	SQLGetPrivateProfileString((const char *)dsn,"RetryTime","0",
					retrytimebuf,sizeof(retrytimebuf),
					ODBC_INI);
	conn->retrytime=(int32_t)charstring::toInteger(retrytimebuf);
	char	triesbuf[6];
	SQLGetPrivateProfileString((const char *)dsn,"Tries","1",
					triesbuf,sizeof(triesbuf),
					ODBC_INI);
	conn->tries=(int32_t)charstring::toInteger(triesbuf);

	debugPrintf("Server: %s\n",conn->server);
	debugPrintf("Port: %d\n",(int)conn->port);
	debugPrintf("Socket: %s\n",conn->socket);
	debugPrintf("User: %s\n",conn->user);
	debugPrintf("Password: %s\n",conn->password);
	debugPrintf("RetryTime: %d\n",(int)conn->retrytime);
	debugPrintf("Tries: %d\n",(int)conn->tries);

	// create connection
	conn->con=new sqlrconnection(conn->server,
					conn->port,
					conn->socket,
					conn->user,
					conn->password,
					conn->retrytime,
					conn->tries);

	#ifdef DEBUG_MESSAGES
	conn->con->debugOn();
	#endif

	return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLConnect(SQLHDBC connectionhandle,
					SQLCHAR *dsn,
					SQLSMALLINT dsnlength,
					SQLCHAR *user,
					SQLSMALLINT userlength,
					SQLCHAR *password,
					SQLSMALLINT passwordlength) {
	debugFunction();
	return SQLR_SQLConnect(connectionhandle,dsn,dsnlength,
				user,userlength,password,passwordlength);
}

SQLRETURN SQL_API SQLCopyDesc(SQLHDESC SourceDescHandle,
					SQLHDESC TargetDescHandle) {
	debugFunction();
	// currently this doesn't do anything
	// because SQLAllocHandle doesn't do anything
	return SQL_SUCCESS;
}

#if (ODBCVER < 0x0300)
SQLRETURN SQL_API SQLDataSources(SQLHENV environmenthandle,
					SQLUSMALLINT Direction,
					SQLCHAR *ServerName,
					SQLSMALLINT BufferLength1,
					SQLSMALLINT *NameLength1,
					SQLCHAR *Description,
					SQLSMALLINT BufferLength2,
					SQLSMALLINT *NameLength2) {
	debugFunction();
	// FIXME: this is allegedly mapped in ODBC3 but I can't tell what to
	return SQL_ERROR;
}
#endif

SQLRETURN SQL_API SQLDescribeCol(SQLHSTMT statementhandle,
					SQLUSMALLINT columnnumber,
					SQLCHAR *columnname,
					SQLSMALLINT bufferlength,
					SQLSMALLINT *namelength,
					SQLSMALLINT *datatype,
					SQLULEN *columnsize,
					SQLSMALLINT *decimaldigits,
					SQLSMALLINT *nullable) {
	debugFunction();
	debugPrintf("columnnumber: %d\n",(int)columnnumber);

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	// make sure we're attempting to get a valid column
	uint32_t	colcount=stmt->cur->colCount();
	if (columnnumber<1 || columnnumber>colcount) {
		SQLR_STMTSetError(stmt,NULL,0,"07009");
		return SQL_ERROR;
	}

	// get a zero-based version of the columnnumber
	uint32_t	col=columnnumber-1;

	charstring::safeCopy((char *)columnname,bufferlength,
				stmt->cur->getColumnName(col));
	if (namelength) {
		*namelength=charstring::length(columnname);
	}
	if (datatype) {
		*datatype=SQLR_MapColumnType(stmt->cur,col);
	}
	if (columnsize) {
		*columnsize=(SQLSMALLINT)stmt->cur->getColumnPrecision(col);
	}
	if (decimaldigits) {
		*decimaldigits=(SQLSMALLINT)stmt->cur->getColumnScale(col);
	}
	if (nullable) {
		*nullable=(SQLSMALLINT)stmt->cur->getColumnIsNullable(col);
	}

	return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLDisconnect(SQLHDBC connectionhandle) {
	debugFunction();

	CONN	*conn=(CONN *)connectionhandle;
	if (connectionhandle==SQL_NULL_HANDLE || !conn || !conn->con) {
		debugPrintf("NULL conn handle\n");
		return SQL_INVALID_HANDLE;
	}

	conn->con->endSession();

	return SQL_SUCCESS;
}

static SQLRETURN SQLR_SQLEndTran(SQLSMALLINT handletype,
					SQLHANDLE handle,
					SQLSMALLINT completiontype) {
	debugFunction();

	switch (handletype) {
		case SQL_HANDLE_ENV:
		{
			debugPrintf("handletype: SQL_HANDLE_ENV\n");

			ENV	*env=(ENV *)handle;
			if (handle==SQL_NULL_HENV || !env) {
				debugPrintf("NULL env handle\n");
				return SQL_INVALID_HANDLE;
			}

			for (linkedlistnode< CONN * >	*node=
				env->connlist.getFirstNode(); node;
				node=(linkedlistnode< CONN * > *)
							node->getNext()) {

				if (completiontype==SQL_COMMIT) {
					node->getData()->con->commit();
				} else if (completiontype==SQL_ROLLBACK) {
					node->getData()->con->rollback();
				}
			}

			return SQL_SUCCESS;
		}
		case SQL_HANDLE_DBC:
		{
			debugPrintf("handletype: SQL_HANDLE_DBC\n");

			CONN	*conn=(CONN *)handle;
			if (handle==SQL_NULL_HANDLE || !conn || !conn->con) {
				debugPrintf("NULL conn handle\n");
				return SQL_INVALID_HANDLE;
			}

			if (completiontype==SQL_COMMIT) {
				conn->con->commit();
			} else if (completiontype==SQL_ROLLBACK) {
				conn->con->rollback();
			}

			return SQL_SUCCESS;
		}
		default:
			debugPrintf("invalid handletype\n");
			return SQL_ERROR;
	}
}

SQLRETURN SQL_API SQLEndTran(SQLSMALLINT handletype,
					SQLHANDLE handle,
					SQLSMALLINT completiontype) {
	debugFunction();
	return SQLR_SQLEndTran(handletype,handle,completiontype);
}

static SQLRETURN SQLR_SQLGetDiagRec(SQLSMALLINT handletype,
					SQLHANDLE handle,
					SQLSMALLINT recnumber,
					SQLCHAR *sqlstate,
					SQLINTEGER *nativeerror,
					SQLCHAR *messagetext,
					SQLSMALLINT bufferlength,
					SQLSMALLINT *textlength);

SQLRETURN SQL_API SQLError(SQLHENV environmenthandle,
					SQLHDBC connectionhandle,
					SQLHSTMT statementhandle,
					SQLCHAR *sqlstate,
					SQLINTEGER *nativeerror,
					SQLCHAR *messagetext,
					SQLSMALLINT bufferlength,
					SQLSMALLINT *textlength) {
	debugFunction();

	if (environmenthandle && environmenthandle!=SQL_NULL_HENV) {
		return SQLR_SQLGetDiagRec(SQL_HANDLE_ENV,
					(SQLHANDLE)environmenthandle,
					1,sqlstate,
					nativeerror,messagetext,
					bufferlength,textlength);
	} else if (connectionhandle && connectionhandle!=SQL_NULL_HANDLE) {
		return SQLR_SQLGetDiagRec(SQL_HANDLE_DBC,
					(SQLHANDLE)connectionhandle,
					1,sqlstate,
					nativeerror,messagetext,
					bufferlength,textlength);
	} else if (statementhandle && statementhandle!=SQL_NULL_HSTMT) {
		return SQLR_SQLGetDiagRec(SQL_HANDLE_STMT,
					(SQLHANDLE)statementhandle,
					1,sqlstate,
					nativeerror,messagetext,
					bufferlength,textlength);
	}
	debugPrintf("no valid handle\n");
	return SQL_INVALID_HANDLE;
}

static void SQLR_ParseNumeric(SQL_NUMERIC_STRUCT *ns,
				const char *value, uint32_t valuesize) {
	debugFunction();

	// find the negative sign and decimal, if there are any
	const char	*negative=charstring::findFirst(value,'-');
	const char	*decimal=charstring::findFirst(value,'.');

	ns->precision=valuesize-((negative!=NULL)?1:0)-((decimal!=NULL)?1:0);
	ns->scale=(value+valuesize)-decimal;

	// 1=positive, 0=negative
	ns->sign=(negative==NULL);

	//  A number is stored in the val field of the SQL_NUMERIC_STRUCT
	//  structure as a scaled integer, in little endian mode (the leftmost
	//  byte being the least-significant byte). For example, the number
	//  10.001 base 10, with a scale of 4, is scaled to an integer of
	//  100010. Because this is 186AA in hexadecimal format, the value in
	//  SQL_NUMERIC_STRUCT would be "AA 86 01 00 00 ... 00", with the number
	//  of bytes defined by the SQL_MAX_NUMERIC_LEN #define...

	// Get the number as a positive integer by skipping negative signs and
	// decimals.  It should be OK to convert it to a 64-bit integer as
	// SQL_MAX_NUMERIC_LEN should be 16 or less.
	char		*newnumber=new char[valuesize+1];
	const char	*ptr=value;
	uint32_t	index=0;
	for (; *ptr && index<valuesize; index++) {
		if (*ptr=='-' || *ptr=='.') {
			ptr++;
		}
		newnumber[index]=*ptr;
	}
	newnumber[index]='\0';
	int64_t	newinteger=charstring::toInteger(newnumber);
	delete[] newnumber;
	
	// convert to hex, LSB first
	for (uint8_t i=0; i<SQL_MAX_NUMERIC_LEN; i++) {
		ns->val[i]=newinteger%16;
		newinteger=newinteger/16;
	}
}

static void SQLR_ParseInterval(SQL_INTERVAL_STRUCT *is,
				const char *value, uint32_t valuesize) {
	debugFunction();

	// FIXME: implement
	is->interval_type=(SQLINTERVAL)0;
	is->interval_sign=0;
	is->intval.day_second.day=0;
	is->intval.day_second.hour=0;
	is->intval.day_second.minute=0;
	is->intval.day_second.second=0;
	is->intval.day_second.fraction=0;

	//typedef struct tagSQL_INTERVAL_STRUCT
	//   {
	//   SQLINTERVAL interval_type;
	//   SQLSMALLINT   interval_sign;
	//   union
	//      {
	//      SQL_YEAR_MONTH_STRUCT year_month;
	//      SQL_DAY_SECOND_STRUCT day_second;
	//      } intval;
	//   }SQLINTERVAL_STRUCT;
	//
	//typedef enum
	//   {
	//   SQL_IS_YEAR=1,
	//   SQL_IS_MONTH=2,
	//   SQL_IS_DAY=3,
	//   SQL_IS_HOUR=4,
	//   SQL_IS_MINUTE=5,
	//   SQL_IS_SECOND=6,
	//   SQL_IS_YEAR_TO_MONTH=7,
	//   SQL_IS_DAY_TO_HOUR=8,
	//   SQL_IS_DAY_TO_MINUTE=9,
	//   SQL_IS_DAY_TO_SECOND=10,
	//   SQL_IS_HOUR_TO_MINUTE=11,
	//   SQL_IS_HOUR_TO_SECOND=12,
	//   SQL_IS_MINUTE_TO_SECOND=13,
	//   }SQLINTERVAL;
	//
	//typedef struct tagSQL_YEAR_MONTH
	//   {
	//   SQLUINTEGER year;
	//   SQLUINTEGER month;
	//   }SQL_YEAR_MOHTH_STRUCT;
	//
	//typedef struct tagSQL_DAY_SECOND
	//   {
	//   SQLUINTEGER day;
	//   SQLUNINTEGER hour;
	//   SQLUINTEGER minute;
	//   SQLUINTEGER second;
	//   SQLUINTEGER fraction;
	//   }SQL_DAY_SECOND_STRUCT;
}

static char SQLR_CharToHex(const char input) {
	debugFunction();
	char	ch=input;
	character::toUpperCase(ch);
	if (ch>='0' && ch<='9') {
		ch=ch-'0';
	} else if (ch>='A' && ch<='F') {
		ch=ch-'A'+10;
	} else {
		ch=0;
	}
	return ch;
}

static void SQLR_ParseGuid(SQLGUID *guid,
				const char *value, uint32_t valuesize) {
	debugFunction();

	// GUID:
	// 8 digits - 4 digits - 4 digits - 4 digits - 12 digits
	// (all digits hex)
	// XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX

	// sanity check
	if (valuesize!=36 ||
		value[8]!='-' || value[13]!='-' ||
		value[18]!='-' || value[23]!='-') {
		rawbuffer::zero(guid,sizeof(SQLGUID));
		return;
	}

	// 8 hex digits (uint32_t)
	for (uint16_t i=0; i<8; i++) {
		guid->Data1=guid->Data1*16+SQLR_CharToHex(value[i]);
	}

	// dash

	// 4 hex digits (uint16_t)
	for (uint16_t i=9; i<13; i++) {
		guid->Data2=guid->Data2*16+SQLR_CharToHex(value[i]);
	}

	// dash

	// 4 hex digits (uint16_t)
	for (uint16_t i=14; i<18; i++) {
		guid->Data3=guid->Data3*16+SQLR_CharToHex(value[i]);
	}

	// dash

	// 4 hex digits (unsigned char)
	guid->Data4[0]=SQLR_CharToHex(value[19])*16+SQLR_CharToHex(value[20]);
	guid->Data4[1]=SQLR_CharToHex(value[21])*16+SQLR_CharToHex(value[22]);

	// dash

	// 12 hex digits (unsigned char)
	guid->Data4[2]=SQLR_CharToHex(value[24])*16+SQLR_CharToHex(value[25]);
	guid->Data4[3]=SQLR_CharToHex(value[26])*16+SQLR_CharToHex(value[27]);
	guid->Data4[4]=SQLR_CharToHex(value[28])*16+SQLR_CharToHex(value[29]);
	guid->Data4[5]=SQLR_CharToHex(value[30])*16+SQLR_CharToHex(value[31]);
	guid->Data4[6]=SQLR_CharToHex(value[32])*16+SQLR_CharToHex(value[33]);
	guid->Data4[7]=SQLR_CharToHex(value[34])*16+SQLR_CharToHex(value[35]);
}

static void SQLR_FetchOutputBinds(SQLHSTMT statementhandle) {
	debugFunction();

	STMT	*stmt=(STMT *)statementhandle;

	numericdictionarylist< outputbind * >	*list=
					stmt->outputbinds.getList();

	for (dictionarylistnode< int32_t, outputbind * > *node=
						list->getFirstNode();
		node; node=(dictionarylistnode< int32_t, outputbind * > *)
							node->getNext()) {

		outputbind	*ob=node->getData()->getData();

		// convert parameternumber to a string
		char	*parametername=charstring::parseNumber(
						ob->parameternumber);

		switch (ob->valuetype) {
			case SQL_C_CHAR:
				debugPrintf("valuetype: SQL_C_CHAR\n");
				// make sure to null-terminate
				charstring::safeCopy(
					(char *)ob->parametervalue,
					ob->bufferlength,
					stmt->cur->getOutputBindString(
							parametername),
					stmt->cur->getOutputBindLength(
							parametername)+1);
				break;
			case SQL_C_SLONG:
			case SQL_C_LONG:
				debugPrintf("valuetype: "
					"SQL_C_SLONG/SQL_C_LONG\n");
				*((long *)ob->parametervalue)=
					(long)stmt->cur->getOutputBindInteger(
								parametername);
				break;
			//case SQL_C_BOOKMARK: (dup of SQL_C_ULONG)
			case SQL_C_ULONG:
				debugPrintf("valuetype: "
					"SQL_C_ULONG/SQL_C_BOOKMARK\n");
				*((unsigned long *)ob->parametervalue)=
					(unsigned long)
					stmt->cur->getOutputBindInteger(
								parametername);
				break;
			case SQL_C_SSHORT:
			case SQL_C_SHORT:
				debugPrintf("valuetype: "
					"SQL_C_SSHORT/SQL_C_SHORT\n");
				*((short *)ob->parametervalue)=
					(short)stmt->cur->getOutputBindInteger(
								parametername);
				break;
			case SQL_C_USHORT:
				debugPrintf("valuetype: SQL_C_USHORT\n");
				*((unsigned short *)ob->parametervalue)=
					(unsigned short)
					stmt->cur->getOutputBindInteger(
								parametername);
				break;
			case SQL_C_FLOAT:
				debugPrintf("valuetype: SQL_C_FLOAT\n");
				*((float *)ob->parametervalue)=
					(float)stmt->cur->getOutputBindDouble(
								parametername);
				break;
			case SQL_C_DOUBLE:
				debugPrintf("valuetype: SQL_C_DOUBLE\n");
				*((double *)ob->parametervalue)=
					(double)stmt->cur->getOutputBindDouble(
								parametername);
				break;
			case SQL_C_NUMERIC:
				debugPrintf("valuetype: SQL_C_NUMERIC\n");
				SQLR_ParseNumeric(
					(SQL_NUMERIC_STRUCT *)
							ob->parametervalue,
					stmt->cur->getOutputBindString(
							parametername),
					stmt->cur->getOutputBindLength(
							parametername));
				break;
			case SQL_C_DATE:
			case SQL_C_TYPE_DATE:
				{
				debugPrintf("valuetype: "
					"SQL_C_DATE/SQL_C_TYPE_DATE\n");
				int16_t	year;
				int16_t	month;
				int16_t	day;
				int16_t	hour;
				int16_t	minute;
				int16_t	second;
				int32_t	microsecond;
				const char	*tz;
				stmt->cur->getOutputBindDate(parametername,
							&year,&month,&day,
							&hour,&minute,&second,
							&microsecond,&tz);
				DATE_STRUCT	*ds=
					(DATE_STRUCT *)ob->parametervalue;
				ds->year=year;
				ds->month=month;
				ds->day=day;
				}
				break;
			case SQL_C_TIME:
			case SQL_C_TYPE_TIME:
				{
				debugPrintf("valuetype: "
					"SQL_C_TIME/SQL_C_TYPE_TIME\n");
				int16_t	year;
				int16_t	month;
				int16_t	day;
				int16_t	hour;
				int16_t	minute;
				int16_t	second;
				int32_t	microsecond;
				const char	*tz;
				stmt->cur->getOutputBindDate(parametername,
							&year,&month,&day,
							&hour,&minute,&second,
							&microsecond,&tz);
				TIME_STRUCT	*ts=
					(TIME_STRUCT *)ob->parametervalue;
				ts->hour=hour;
				ts->minute=minute;
				ts->second=second;
				}
				break;
			case SQL_C_TIMESTAMP:
			case SQL_C_TYPE_TIMESTAMP:
				{
				debugPrintf("valuetype: "
					"SQL_C_TIMESTAMP/"
					"SQL_C_TYPE_TIMESTAMP\n");
				int16_t	year;
				int16_t	month;
				int16_t	day;
				int16_t	hour;
				int16_t	minute;
				int16_t	second;
				int32_t	microsecond;
				const char	*tz;
				stmt->cur->getOutputBindDate(parametername,
							&year,&month,&day,
							&hour,&minute,&second,
							&microsecond,&tz);
				TIMESTAMP_STRUCT	*ts=
					(TIMESTAMP_STRUCT *)ob->parametervalue;
				ts->year=year;
				ts->month=month;
				ts->day=day;
				ts->hour=hour;
				ts->minute=minute;
				ts->second=second;
				ts->fraction=microsecond*10;
				}
				break;
			case SQL_C_INTERVAL_YEAR:
			case SQL_C_INTERVAL_MONTH:
			case SQL_C_INTERVAL_DAY:
			case SQL_C_INTERVAL_HOUR:
			case SQL_C_INTERVAL_MINUTE:
			case SQL_C_INTERVAL_SECOND:
			case SQL_C_INTERVAL_YEAR_TO_MONTH:
			case SQL_C_INTERVAL_DAY_TO_HOUR:
			case SQL_C_INTERVAL_DAY_TO_MINUTE:
			case SQL_C_INTERVAL_DAY_TO_SECOND:
			case SQL_C_INTERVAL_HOUR_TO_MINUTE:
			case SQL_C_INTERVAL_HOUR_TO_SECOND:
			case SQL_C_INTERVAL_MINUTE_TO_SECOND:
				debugPrintf("valuetype: SQL_C_INTERVAL_XXX\n");
				SQLR_ParseInterval(
					(SQL_INTERVAL_STRUCT *)
							ob->parametervalue,
					stmt->cur->getOutputBindString(
							parametername),
					stmt->cur->getOutputBindLength(
							parametername));
				break;
			//case SQL_C_VARBOOKMARK: (dup of SQL_C_BINARY)
			case SQL_C_BINARY:
				{
				debugPrintf("valuetype: "
					"SQL_C_BINARY/SQL_C_VARBOOKMARK\n");
				charstring::safeCopy(
					(char *)ob->parametervalue,
					ob->bufferlength,
					stmt->cur->getOutputBindBlob(
							parametername),
					stmt->cur->getOutputBindLength(
							parametername));
				break;
				}
			case SQL_C_BIT:
				{
				debugPrintf("valuetype: SQL_C_BIT\n");
				const char	*val=
					stmt->cur->getOutputBindString(
								parametername);
				((unsigned char *)ob->parametervalue)[0]=
					(charstring::contains("YyTt",val) ||
					charstring::toInteger(val))?'1':'0';
				}
				break;
			case SQL_C_SBIGINT:
				debugPrintf("valuetype: SQL_C_SBIGINT\n");
				*((int64_t *)ob->parametervalue)=
				(int64_t)stmt->cur->getOutputBindInteger(
								parametername);
				break;
			case SQL_C_UBIGINT:
				debugPrintf("valuetype: SQL_C_UBIGINT\n");
				*((uint64_t *)ob->parametervalue)=
				(uint64_t)stmt->cur->getOutputBindInteger(
								parametername);
				break;
			case SQL_C_TINYINT:
			case SQL_C_STINYINT:
				debugPrintf("valuetype: "
					"SQL_C_TINYINT/SQL_C_STINYINT\n");
				*((char *)ob->parametervalue)=
				(char)stmt->cur->getOutputBindInteger(
								parametername);
				break;
			case SQL_C_UTINYINT:
				debugPrintf("valuetype: SQL_C_UTINYINT\n");
				*((unsigned char *)ob->parametervalue)=
				(unsigned char)stmt->cur->getOutputBindInteger(
								parametername);
				break;
			case SQL_C_GUID:
				debugPrintf("valuetype: SQL_C_GUID\n");
				SQLR_ParseGuid(
					(SQLGUID *)ob->parametervalue,
					stmt->cur->getOutputBindString(
							parametername),
					stmt->cur->getOutputBindLength(
							parametername));
				break;
			default:
				debugPrintf("invalue valuetype\n");
				break;
		}
	}
}

uint32_t SQLR_TrimQuery(SQLCHAR *statementtext, SQLINTEGER textlength) {

	// find the length of the string
	uint32_t	length=0;
	if (textlength==SQL_NTS) {
		length=charstring::length((const char *)statementtext);
	} else {
		length=textlength;
	}

	// trim trailing whitespace and semicolons
	for (;;) {
		char	ch=statementtext[length-1];
		if (ch==' ' || ch=='	' || ch=='\n' || ch=='\r' || ch==';') {
			length--;
			if (length==0) {
				return length;
			}
		} else {
			return length;
		}
	}
}

SQLRETURN SQL_API SQLExecDirect(SQLHSTMT statementhandle,
					SQLCHAR *statementtext,
					SQLINTEGER textlength) {
	debugFunction();

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	// reinit row indices
	stmt->currentfetchrow=0;
	stmt->currentgetdatarow=0;

	// clear the error
	SQLR_STMTClearError(stmt);

	// trim query
	uint32_t	statementtextlength=SQLR_TrimQuery(
						statementtext,textlength);

	// run the query
	#ifdef DEBUG_MESSAGES
	stringbuffer	debugstr;
	debugstr.append(statementtext,statementtextlength);
	debugPrintf("statement: \"%s\",%d)\n",
			debugstr.getString(),(int)statementtextlength);
	#endif
	bool	result=stmt->cur->sendQuery((const char *)statementtext,
							statementtextlength);

	// handle success
	if (result) {
		SQLR_FetchOutputBinds(stmt);
		return SQL_SUCCESS;
	}

	// handle error
	SQLR_STMTSetError(stmt,stmt->cur->errorMessage(),
				stmt->cur->errorNumber(),NULL);
	return SQL_ERROR;
}

SQLRETURN SQL_API SQLExecute(SQLHSTMT statementhandle) {
	debugFunction();

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	// reinit row indices
	stmt->currentfetchrow=0;
	stmt->currentgetdatarow=0;

	// clear the error
	SQLR_STMTClearError(stmt);

	// run the query
	bool	result=stmt->cur->executeQuery();

	// handle success
	if (result) {
		SQLR_FetchOutputBinds(stmt);
		return SQL_SUCCESS;
	}

	// handle error
	SQLR_STMTSetError(stmt,stmt->cur->errorMessage(),
				stmt->cur->errorNumber(),NULL);
	return SQL_ERROR;
}

static SQLRETURN SQLR_SQLGetData(SQLHSTMT statementhandle,
					SQLUSMALLINT columnnumber,
					SQLSMALLINT targettype,
					SQLPOINTER targetvalue,
					SQLLEN bufferlength,
					SQLLEN *strlen_or_ind);

static SQLRETURN SQLR_Fetch(SQLHSTMT statementhandle, SQLULEN *pcrow,
						SQLUSMALLINT *rgfrowstatus) {
	debugFunction();

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	// fetch the row
	if (!stmt->cur->getRow(stmt->currentfetchrow)) {
		return SQL_NO_DATA_FOUND;
	}
	stmt->currentgetdatarow=stmt->currentfetchrow;

	// Update the number of rows that were fetched in this operation.
	uint64_t	firstrowindex=stmt->cur->firstRowIndex();
	uint64_t	rowcount=stmt->cur->rowCount();
	uint64_t	lastrowindex=(rowcount)?rowcount-1:0;
	uint64_t	bufferedrowcount=lastrowindex-firstrowindex;
	uint64_t	rowsfetched=(firstrowindex==stmt->currentgetdatarow)?
							bufferedrowcount:0;
	if (stmt->rowsfetchedptr) {
		*stmt->rowsfetchedptr=rowsfetched;
	}
	if (pcrow) {
		*pcrow=rowsfetched;
	}

	// update row statuses
	for (SQLULEN i=0; i<stmt->cur->getResultSetBufferSize(); i++) {
		SQLUSMALLINT	status=(i<rowsfetched)?
					SQL_ROW_SUCCESS:SQL_ROW_NOROW;
		if (rgfrowstatus) {
			rgfrowstatus[i]=status;
		}
		if (stmt->rowstatusptr[i]) {
			stmt->rowstatusptr[i]=status;
		}
	}

	// move on to the next row
	stmt->currentfetchrow++;

	// update column binds
	uint32_t	colcount=stmt->cur->colCount();
	for (uint32_t index=0; index<colcount; index++) {

		// get the bound field, if this field isn't bound, move on
		FIELD	*field=NULL;
		if (!stmt->fieldlist.getData(index,&field)) {
			continue;
		}

		// get the data into the bound column
		SQLRETURN	result=SQLR_SQLGetData(statementhandle,
							index+1,
							field->targettype,
							field->targetvalue,
							field->bufferlength,
							field->strlen_or_ind);
		if (result!=SQL_SUCCESS) {
			return result;
		}
	}
	return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLFetch(SQLHSTMT statementhandle) {
	debugFunction();
	return SQLR_Fetch(statementhandle,NULL,NULL);
}

SQLRETURN SQL_API SQLFetchScroll(SQLHSTMT statementhandle,
					SQLSMALLINT fetchorientation,
					SQLLEN fetchoffset) {
	debugFunction();

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	// for now we only support SQL_FETCH_NEXT
	if (fetchorientation!=SQL_FETCH_NEXT) {
		debugPrintf("invalid fetchorientation\n");
		stmt->sqlstate="HY106";
		return SQL_ERROR;
	}

	return SQLR_Fetch(statementhandle,NULL,NULL);
}

static SQLRETURN SQLR_SQLFreeHandle(SQLSMALLINT handletype, SQLHANDLE handle);

SQLRETURN SQL_API SQLFreeConnect(SQLHDBC connectionhandle) {
	debugFunction();
	return SQLR_SQLFreeHandle(SQL_HANDLE_DBC,connectionhandle);
}

SQLRETURN SQL_API SQLFreeEnv(SQLHENV environmenthandle) {
	debugFunction();
	return SQLR_SQLFreeHandle(SQL_HANDLE_ENV,environmenthandle);
}

static SQLRETURN SQLR_SQLFreeHandle(SQLSMALLINT handletype, SQLHANDLE handle) {
	debugFunction();

	switch (handletype) {
		case SQL_HANDLE_ENV:
			{
			debugPrintf("handletype: SQL_HANDLE_ENV\n");
			ENV	*env=(ENV *)handle;
			if (handle==SQL_NULL_HENV || !env) {
				debugPrintf("NULL env handle\n");
				return SQL_INVALID_HANDLE;
			}
			env->connlist.clear();
			delete[] env->error;
			delete env;
			return SQL_SUCCESS;
			}
		case SQL_HANDLE_DBC:
			{
			debugPrintf("handletype: SQL_HANDLE_DBC\n");
			CONN	*conn=(CONN *)handle;
			if (handle==SQL_NULL_HANDLE || !conn || !conn->con) {
				debugPrintf("NULL conn handle\n");
				return SQL_INVALID_HANDLE;
			}
			conn->env->connlist.removeAllByData(conn);
			conn->stmtlist.clear();
			delete conn->con;
			delete[] conn->error;
			delete conn;
			return SQL_SUCCESS;
			}
		case SQL_HANDLE_STMT:
			{
			debugPrintf("handletype: SQL_HANDLE_STMT\n");
			STMT	*stmt=(STMT *)handle;
			if (handle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
				debugPrintf("NULL stmt handle\n");
				return SQL_INVALID_HANDLE;
			}
			stmt->conn->stmtlist.removeAllByData(stmt);
			delete stmt->improwdesc;
			delete stmt->impparamdesc;
			delete stmt->cur;
			delete stmt;
			return SQL_SUCCESS;
			}
		case SQL_HANDLE_DESC:
			debugPrintf("handletype: SQL_HANDLE_DESC\n");
			// FIXME: no idea what to do here,
			// for now just report success
			return SQL_SUCCESS;
		default:
			debugPrintf("invalid handletype\n");
			return SQL_ERROR;
	}
}

SQLRETURN SQL_API SQLFreeHandle(SQLSMALLINT handletype, SQLHANDLE handle) {
	debugFunction();
	return SQLR_SQLFreeHandle(handletype,handle);
}

SQLRETURN SQL_API SQLFreeStmt(SQLHSTMT statementhandle, SQLUSMALLINT option) {
	debugFunction();

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	switch (option) {
		case SQL_CLOSE:
			debugPrintf("option: SQL_CLOSE\n");
			return SQLR_SQLCloseCursor(statementhandle);
		case SQL_DROP:
			debugPrintf("option: SQL_DROP\n");
			return SQLR_SQLFreeHandle(SQL_HANDLE_STMT,
						(SQLHANDLE)statementhandle);
		case SQL_UNBIND:
			debugPrintf("option: SQL_UNBIND\n");
			stmt->fieldlist.clear();
			return SQL_SUCCESS;
		case SQL_RESET_PARAMS:
			debugPrintf("option: SQL_RESET_PARAMS\n");
			SQLR_ResetParams(stmt);
			return SQL_SUCCESS;
		default:
			debugPrintf("invalid option\n");
			return SQL_ERROR;
	}
}

static SQLRETURN SQLR_SQLGetConnectAttr(SQLHDBC connectionhandle,
					SQLINTEGER attribute,
					SQLPOINTER value,
					SQLINTEGER bufferlength,
					SQLINTEGER *stringlength) {
	debugFunction();

	CONN	*conn=(CONN *)connectionhandle;
	if (connectionhandle==SQL_NULL_HANDLE || !conn || !conn->con) {
		debugPrintf("NULL conn handle\n");
		return SQL_INVALID_HANDLE;
	}

	// FIXME: implement
	//SQL_ACCESS_MODE:
	//SQL_AUTOCOMMIT: settable but not gettable
	//SQL_LOGIN_TIMEOUT:
	//SQL_OPT_TRACE:
	//SQL_OPT_TRACEFILE:
	//SQL_TRANSLATE_DLL:
	////SQL_TRANSLATE_OPTION:
	//SQL_TXN_ISOLATION:
	//SQL_CURRENT_QUALIFIER:
	//SQL_ODBC_CURSORS:
	//SQL_QUIET_MODE:
	//SQL_PACKET_SIZE:
	//#if (ODBCVER >= 0x0300)
	//SQL_ATTR_CONNECTION_TIMEOUT:
	//SQL_ATTR_DISCONNECT_BEHAVIOR:
	//SQL_ATTR_ENLIST_IN_DTC:
	//SQL_ATTR_ENLIST_IN_XA:
	//SQL_ATTR_AUTO_IPD:
	//SQL_ATTR_METADATA_ID:
	//#endif

	return SQL_ERROR;
}

SQLRETURN SQL_API SQLGetConnectAttr(SQLHDBC connectionhandle,
					SQLINTEGER attribute,
					SQLPOINTER value,
					SQLINTEGER bufferlength,
					SQLINTEGER *stringlength) {
	debugFunction();
	return SQLR_SQLGetConnectAttr(connectionhandle,attribute,
					value,bufferlength,stringlength);
}

SQLRETURN SQL_API SQLGetConnectOption(SQLHDBC connectionhandle,
					SQLUSMALLINT option,
					SQLPOINTER value) {
	debugFunction();
	return SQLR_SQLGetConnectAttr(connectionhandle,option,value,256,NULL);
}

SQLRETURN SQL_API SQLGetCursorName(SQLHSTMT statementhandle,
					SQLCHAR *cursorname,
					SQLSMALLINT bufferlength,
					SQLSMALLINT *namelength) {
	debugFunction();

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	if (!stmt->name) {
		stmt->name=new char[charstring::integerLength(stmtid)+1];
		sprintf(stmt->name,"%d",(int)stmtid);
		stmtid++;
	}
	if (cursorname) {
		charstring::safeCopy((char *)cursorname,
					bufferlength,stmt->name);
	}
	if (namelength) {
		*namelength=charstring::length(stmt->name);
	}

	return SQL_SUCCESS;
}

static void SQLR_ParseDate(DATE_STRUCT *ds, const char *value) {

	// result variables
	int16_t	year=-1;
	int16_t	month=-1;
	int16_t	day=-1;
	int16_t	hour=-1;
	int16_t	minute=-1;
	int16_t	second=-1;

	// get day/month format
	bool	ddmm=!charstring::compareIgnoringCase(
					environment::getValue(
					"SQLR_ODBC_DATE_DDMM"),
					"yes");

	// parse
	parseDateTime(value,ddmm,false,&year,&month,&day,
					&hour,&minute,&second);

	// copy data out
	ds->year=(year!=-1)?year:0;
	ds->month=(month!=-1)?month:0;
	ds->day=(day!=-1)?day:0;
}

static void SQLR_ParseTime(TIME_STRUCT *ts, const char *value) {

	// result variables
	int16_t	year=-1;
	int16_t	month=-1;
	int16_t	day=-1;
	int16_t	hour=-1;
	int16_t	minute=-1;
	int16_t	second=-1;

	// get day/month format
	bool	ddmm=!charstring::compareIgnoringCase(
					environment::getValue(
					"SQLR_ODBC_DATE_DDMM"),
					"yes");

	// parse
	parseDateTime(value,ddmm,false,&year,&month,&day,
					&hour,&minute,&second);

	// copy data out
	ts->hour=(hour!=-1)?hour:0;
	ts->minute=(minute!=-1)?minute:0;
	ts->second=(second!=-1)?second:0;
}

static void SQLR_ParseTimeStamp(TIMESTAMP_STRUCT *tss, const char *value) {

	// result variables
	int16_t	year=-1;
	int16_t	month=-1;
	int16_t	day=-1;
	int16_t	hour=-1;
	int16_t	minute=-1;
	int16_t	second=-1;

	// get day/month format
	bool	ddmm=!charstring::compareIgnoringCase(
					environment::getValue(
					"SQLR_ODBC_DATE_DDMM"),
					"yes");

	// parse
	parseDateTime(value,ddmm,false,&year,&month,&day,
					&hour,&minute,&second);

	// copy data out
	tss->year=(year!=-1)?year:0;
	tss->month=(month!=-1)?month:0;
	tss->day=(day!=-1)?day:0;
	tss->hour=(hour!=-1)?hour:0;
	tss->minute=(minute!=-1)?minute:0;
	tss->second=(second!=-1)?second:0;
	tss->fraction=0;
}

static SQLRETURN SQLR_SQLGetData(SQLHSTMT statementhandle,
					SQLUSMALLINT columnnumber,
					SQLSMALLINT targettype,
					SQLPOINTER targetvalue,
					SQLLEN bufferlength,
					SQLLEN *strlen_or_ind) {
	debugFunction();

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	debugPrintf("columnnumber: %d\n",(int)columnnumber);

	// make sure we're attempting to get a valid column
	uint32_t	colcount=stmt->cur->colCount();
	if (columnnumber<1 || columnnumber>colcount) {
		SQLR_STMTSetError(stmt,NULL,0,"07009");
		return SQL_ERROR;
	}

	// get a zero-based version of the columnnumber
	uint32_t	col=columnnumber-1;

	// get the field
	const char	*field=stmt->cur->getField(
					stmt->currentgetdatarow,col);
	uint32_t	fieldlength=stmt->cur->getFieldLength(
					stmt->currentgetdatarow,col);

	// initialize NULL indicator
	*strlen_or_ind=SQL_NULL_DATA;

	// handle NULL fields
	if (!field) {
		return SQL_SUCCESS;
	}

	// reset targettype based on column type
	if (targettype==SQL_C_DEFAULT) {
		targettype=SQLR_MapCColumnType(stmt->cur,col);
	}

	// get the field data
	switch (targettype) {
		case SQL_C_CHAR:
			{
			debugPrintf("targettype: SQL_C_CHAR\n");
			if (strlen_or_ind) {
				*strlen_or_ind=fieldlength;
			}
			// make sure to null-terminate
			charstring::safeCopy((char *)targetvalue,
						bufferlength,
						field,fieldlength+1);
			}
			break;
		case SQL_C_SSHORT:
		case SQL_C_SHORT:
		case SQL_C_USHORT:
			debugPrintf("targettype: SQL_C_(X)SHORT\n");
			*((short *)targetvalue)=
				(short)charstring::toInteger(field);
			if (strlen_or_ind) {
				*strlen_or_ind=sizeof(short);
			}
			break;
		case SQL_C_SLONG:
		case SQL_C_LONG:
		//case SQL_C_BOOKMARK: (dup of SQL_C_ULONG)
		case SQL_C_ULONG:
			debugPrintf("targettype: SQL_C_(X)LONG\n");
			*((long *)targetvalue)=
				(long)charstring::toInteger(field);
			if (strlen_or_ind) {
				*strlen_or_ind=sizeof(long);
			}
			break;
		case SQL_C_FLOAT:
			debugPrintf("targettype: SQL_C_FLOAT\n");
			*((float *)targetvalue)=
				(float)charstring::toFloat(field);
			if (strlen_or_ind) {
				*strlen_or_ind=sizeof(float);
			}
			break;
		case SQL_C_DOUBLE:
			debugPrintf("targettype: SQL_C_DOUBLE\n");
			*((double *)targetvalue)=
				(double)charstring::toFloat(field);
			if (strlen_or_ind) {
				*strlen_or_ind=sizeof(double);
			}
			break;
		case SQL_C_BIT:
			debugPrintf("targettype: SQL_C_BIT\n");
			((unsigned char *)targetvalue)[0]=
				(charstring::contains("YyTt",field) ||
				charstring::toInteger(field))?'1':'0';
			*strlen_or_ind=sizeof(unsigned char);
			break;
		case SQL_C_STINYINT:
		case SQL_C_TINYINT:
		case SQL_C_UTINYINT:
			debugPrintf("targettype: SQL_C_(X)TINYINT\n");
			*((char *)targetvalue)=
				charstring::toInteger(field);
			if (strlen_or_ind) {
				*strlen_or_ind=sizeof(char);
			}
			break;
		case SQL_C_SBIGINT:
		case SQL_C_UBIGINT:
			debugPrintf("targettype: SQL_C_(X)BIGINT\n");
			*((int64_t *)targetvalue)=
				charstring::toInteger(field);
			if (strlen_or_ind) {
				*strlen_or_ind=sizeof(int64_t);
			}
			break;
		//case SQL_C_VARBOOKMARK: (dup of SQL_C_BINARY)
		case SQL_C_BINARY:
			{
			debugPrintf("targettype: "
				"SQL_C_BINARY/SQL_C_VARBOOKMARK\n");
			uint32_t	sizetocopy=fieldlength;
			if (bufferlength<(SQLLEN)sizetocopy) {
				sizetocopy=bufferlength;
			}
			if (strlen_or_ind) {
				*strlen_or_ind=sizetocopy;
			}
			rawbuffer::copy((void *)targetvalue,
					(const void *)field,sizetocopy);
			}
			break;
		case SQL_C_DATE:
		case SQL_C_TYPE_DATE:
			debugPrintf("targettype: SQL_C_DATE/SQL_C_TYPE_DATE\n");
			SQLR_ParseDate((DATE_STRUCT *)targetvalue,field);
			break;
		case SQL_C_TIME:
		case SQL_C_TYPE_TIME:
			debugPrintf("targettype: SQL_C_TIME/SQL_C_TYPE_TIME\n");
			SQLR_ParseTime((TIME_STRUCT *)targetvalue,field);
			break;
		case SQL_C_TIMESTAMP:
		case SQL_C_TYPE_TIMESTAMP:
			debugPrintf("targettype: "
				"SQL_C_TIMESTAMP/SQL_C_TYPE_TIMESTAMP\n");
			SQLR_ParseTimeStamp(
				(TIMESTAMP_STRUCT *)targetvalue,field);
			break;
		case SQL_C_NUMERIC:
			debugPrintf("targettype: SQL_C_NUMERIC\n");
			SQLR_ParseNumeric((SQL_NUMERIC_STRUCT *)targetvalue,
							field,fieldlength);
			break;
		case SQL_C_GUID:
			debugPrintf("targettype: SQL_C_GUID\n");
			SQLR_ParseGuid((SQLGUID *)targetvalue,
						field,fieldlength);
			break;
		case SQL_C_INTERVAL_YEAR:
		case SQL_C_INTERVAL_MONTH:
		case SQL_C_INTERVAL_DAY:
		case SQL_C_INTERVAL_HOUR:
		case SQL_C_INTERVAL_MINUTE:
		case SQL_C_INTERVAL_SECOND:
		case SQL_C_INTERVAL_YEAR_TO_MONTH:
		case SQL_C_INTERVAL_DAY_TO_HOUR:
		case SQL_C_INTERVAL_DAY_TO_MINUTE:
		case SQL_C_INTERVAL_DAY_TO_SECOND:
		case SQL_C_INTERVAL_HOUR_TO_MINUTE:
		case SQL_C_INTERVAL_HOUR_TO_SECOND:
		case SQL_C_INTERVAL_MINUTE_TO_SECOND:
			debugPrintf("targettype: SQL_C_INTERVAL_XXX\n");
			SQLR_ParseInterval((SQL_INTERVAL_STRUCT *)
						targetvalue,
						field,fieldlength);
			break;
		default:
			debugPrintf("invalid targettype\n");
			return SQL_ERROR;
	}
	return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLGetData(SQLHSTMT statementhandle,
					SQLUSMALLINT columnnumber,
					SQLSMALLINT targettype,
					SQLPOINTER targetvalue,
					SQLLEN bufferlength,
					SQLLEN *strlen_or_ind) {
	debugFunction();
	return SQLR_SQLGetData(statementhandle,columnnumber,
				targettype,targetvalue,bufferlength,
				strlen_or_ind);
}

SQLRETURN SQL_API SQLGetDescField(SQLHDESC DescriptorHandle,
					SQLSMALLINT RecNumber,
					SQLSMALLINT FieldIdentifier,
					SQLPOINTER Value,
					SQLINTEGER BufferLength,
					SQLINTEGER *StringLength) {
	debugFunction();
	// not supported
	return SQL_ERROR;
}

SQLRETURN SQL_API SQLGetDescRec(SQLHDESC DescriptorHandle,
					SQLSMALLINT RecNumber,
					SQLCHAR *Name,
					SQLSMALLINT BufferLength,
					SQLSMALLINT *StringLength,
					SQLSMALLINT *Type,
					SQLSMALLINT *SubType,
					SQLLEN *Length,
					SQLSMALLINT *Precision,
					SQLSMALLINT *Scale,
					SQLSMALLINT *Nullable) {
	debugFunction();
	// not supported
	return SQL_ERROR;
}

SQLRETURN SQL_API SQLGetDiagField(SQLSMALLINT handletype,
					SQLHANDLE handle,
					SQLSMALLINT recnumber,
					SQLSMALLINT diagidentifier,
					SQLPOINTER diaginfo,
					SQLSMALLINT bufferlength,
					SQLSMALLINT *stringlength) {
	debugFunction();

	// SQL Relay doesn't have more than 1 error record
	if (recnumber>1) {
		return SQL_NO_DATA;
	}

	switch (handletype) {
		case SQL_HANDLE_ENV:
			{
			debugPrintf("handletype: SQL_HANDLE_ENV\n");
			ENV	*env=(ENV *)handle;
			if (handle==SQL_NULL_HENV || !env) {
				debugPrintf("NULL env handle\n");
				return SQL_INVALID_HANDLE;
			}
			// not supported
			SQLR_ENVSetError(env,NULL,0,"IM001");
			return SQL_ERROR;
			}
		case SQL_HANDLE_DBC:
			{
			CONN	*conn=(CONN *)handle;
			if (handle==SQL_NULL_HSTMT || !conn) {
				debugPrintf("NULL conn handle\n");
				return SQL_INVALID_HANDLE;
			}
			debugPrintf("handletype: SQL_HANDLE_DBC\n");
			// not supported
			SQLR_CONNSetError(conn,NULL,0,"IM001");
			return SQL_ERROR;
			}
		case SQL_HANDLE_STMT:
			{
			STMT	*stmt=(STMT *)handle;
			if (handle==SQL_NULL_HSTMT || !stmt) {
				debugPrintf("NULL stmt handle\n");
				return SQL_INVALID_HANDLE;
			}
			debugPrintf("handletype: SQL_HANDLE_STMT\n");
			// FIXME: there are tons more of these...
			if (diagidentifier==SQL_DIAG_ROW_COUNT) {
				*(SQLLEN *)diaginfo=stmt->cur->affectedRows();
				return SQL_SUCCESS;
			}
			// anything else is not supported
			SQLR_STMTSetError(stmt,NULL,0,"IM001");
			return SQL_ERROR;
			}
		case SQL_HANDLE_DESC:
			debugPrintf("handletype: SQL_HANDLE_DESC\n");
			// not supported
			return SQL_ERROR;
	}
	debugPrintf("invalid handletype\n");
	return SQL_ERROR;
}

static SQLRETURN SQLR_SQLGetDiagRec(SQLSMALLINT handletype,
					SQLHANDLE handle,
					SQLSMALLINT recnumber,
					SQLCHAR *sqlstate,
					SQLINTEGER *nativeerror,
					SQLCHAR *messagetext,
					SQLSMALLINT bufferlength,
					SQLSMALLINT *textlength) {
	debugFunction();

	debugPrintf("recnumber: %d\n",(int)recnumber);

	// SQL Relay doesn't have more than 1 error record
	if (recnumber>1) {
		return SQL_NO_DATA;
	}

	// initialize error and sqlstate
	const char	*error=NULL;
	const char	*sqlst=NULL;
	SQLINTEGER	errn=0;

	switch (handletype) {
		case SQL_HANDLE_ENV:
			{
			debugPrintf("handletype: SQL_HANDLE_ENV\n");
			ENV	*env=(ENV *)handle;
			if (handle==SQL_NULL_HSTMT || !env) {
				debugPrintf("NULL env handle\n");
				return SQL_INVALID_HANDLE;
			}
			error=env->error;
			errn=env->errn;
			sqlst=env->sqlstate;
			}
			break;
		case SQL_HANDLE_DBC:
			{
			debugPrintf("handletype: SQL_HANDLE_DBC\n");
			CONN	*conn=(CONN *)handle;
			if (handle==SQL_NULL_HSTMT || !conn) {
				debugPrintf("NULL conn handle\n");
				return SQL_INVALID_HANDLE;
			}
			error=conn->error;
			errn=conn->errn;
			sqlst=conn->sqlstate;
			}
			break;
		case SQL_HANDLE_STMT:
			{
			debugPrintf("handletype: SQL_HANDLE_STMT\n");
			STMT	*stmt=(STMT *)handle;
			if (handle==SQL_NULL_HSTMT || !stmt) {
				debugPrintf("NULL stmt handle\n");
				return SQL_INVALID_HANDLE;
			}
			error=stmt->error;
			errn=stmt->errn;
			sqlst=stmt->sqlstate;
			}
			break;
		case SQL_HANDLE_DESC:
			debugPrintf("handletype: SQL_HANDLE_DESC\n");
			// not supported
			return SQL_ERROR;
		default:
			debugPrintf("invalid handletype\n");
			return SQL_ERROR;
	}

	debugPrintf("messagetext: %s\n",(error)?error:"");
	debugPrintf("nativeerror: %lld\n",(int64_t)errn);
	debugPrintf("sqlstate: %s\n",(sqlst)?sqlst:"");

	// copy out the error and sqlstate
	charstring::safeCopy((char *)messagetext,(size_t)bufferlength,error);
	if (nativeerror) {
		*nativeerror=errn;
	}
	charstring::copy((char *)sqlstate,(sqlst)?sqlst:"HYOOO");

	return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLGetDiagRec(SQLSMALLINT handletype,
					SQLHANDLE handle,
					SQLSMALLINT recnumber,
					SQLCHAR *sqlstate,
					SQLINTEGER *nativeerror,
					SQLCHAR *messagetext,
					SQLSMALLINT bufferlength,
					SQLSMALLINT *textlength) {
	debugFunction();
	return SQLR_SQLGetDiagRec(handletype,handle,recnumber,sqlstate,
					nativeerror,messagetext,bufferlength,
					textlength);
}

SQLRETURN SQL_API SQLGetEnvAttr(SQLHENV environmenthandle,
					SQLINTEGER attribute,
					SQLPOINTER value,
					SQLINTEGER bufferlength,
					SQLINTEGER *stringlength) {
	debugFunction();

	ENV	*env=(ENV *)environmenthandle;
	if (environmenthandle==SQL_NULL_HENV || !env) {
		debugPrintf("NULL env handle\n");
		return SQL_INVALID_HANDLE;
	}

	switch (attribute) {
		case SQL_ATTR_OUTPUT_NTS:
			debugPrintf("attribute: SQL_ATTR_OUTPUT_NTS\n");
			// this one is hardcoded to true
			// and can't be set to false
			*((SQLINTEGER *)value)=SQL_TRUE;
			break;
		case SQL_ATTR_ODBC_VERSION:
			debugPrintf("attribute: SQL_ATTR_ODBC_VERSION\n");
			*((SQLINTEGER *)value)=env->odbcversion;
			debugPrintf("odbcversion: %d\n",(int)env->odbcversion);
			break;
		case SQL_ATTR_CONNECTION_POOLING:
			debugPrintf("attribute: SQL_ATTR_CONNECTION_POOLING\n");
			// this one is hardcoded to "off"
			// and can't be changed
			*((SQLUINTEGER *)value)=SQL_CP_OFF;
			break;
		case SQL_ATTR_CP_MATCH:
			debugPrintf("attribute: SQL_ATTR_CP_MATCH\n");
			// this one is hardcoded to "default"
			// and can't be changed
			*((SQLUINTEGER *)value)=SQL_CP_MATCH_DEFAULT;
			break;
		default:
			debugPrintf("unsupported attribute\n");
			break;
	}
	return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLGetFunctions(SQLHDBC connectionhandle,
					SQLUSMALLINT functionid,
					SQLUSMALLINT *supported) {
	debugFunction();

	CONN	*conn=(CONN *)connectionhandle;
	if (connectionhandle==SQL_NULL_HANDLE || !conn || !conn->con) {
		debugPrintf("NULL conn handle\n");
		return SQL_INVALID_HANDLE;
	}

	switch (functionid) {
		case SQL_API_SQLALLOCCONNECT:
			debugPrintf("functionid: "
				"SQL_API_SQLALLOCCONNECT "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		case SQL_API_SQLALLOCENV:
			debugPrintf("functionid: "
				"SQL_API_SQLALLOCENV "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		#if (ODBCVER >= 0x0300)
		case SQL_API_SQLALLOCHANDLE:
			debugPrintf("functionid: "
				"SQL_API_SQLALLOCHANDLE "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		#endif
		case SQL_API_SQLALLOCSTMT:
			debugPrintf("functionid: "
				"SQL_API_SQLALLOCSTMT "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		case SQL_API_SQLBINDCOL:
			debugPrintf("functionid: "
				"SQL_API_SQLBINDCOL "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		#if (ODBCVER >= 0x0300)
		case SQL_API_SQLBINDPARAM:
			debugPrintf("functionid: "
				"SQL_API_SQLBINDPARAM "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		case SQL_API_SQLCLOSECURSOR:
			debugPrintf("functionid: "
				"SQL_API_SQLCLOSECURSOR "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		case SQL_API_SQLCOLATTRIBUTE:
		//case SQL_API_SQLCOLATTRIBUTES:
			debugPrintf("functionid: "
				"SQL_API_SQLCOLATTRIBUTE "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		#endif
		case SQL_API_SQLCONNECT:
			debugPrintf("functionid: "
				"SQL_API_SQLCONNECT "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		#if (ODBCVER >= 0x0300)
		case SQL_API_SQLCOPYDESC:
			debugPrintf("functionid: "
				"SQL_API_SQLCOPYDESC "
				"- false\n");
			*supported=SQL_FALSE;
			break;
		#endif
		case SQL_API_SQLDESCRIBECOL:
			debugPrintf("functionid: "
				"SQL_API_SQLDESCRIBECOL "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		case SQL_API_SQLDISCONNECT:
			debugPrintf("functionid: "
				"SQL_API_SQLDISCONNECT "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		#if (ODBCVER >= 0x0300)
		case SQL_API_SQLENDTRAN:
			debugPrintf("functionid: "
				"SQL_API_SQLENDTRAN "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		#endif
		case SQL_API_SQLERROR:
			debugPrintf("functionid: "
				"SQL_API_SQLERROR "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		case SQL_API_SQLEXECDIRECT:
			debugPrintf("functionid: "
				"SQL_API_SQLEXECDIRECT "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		case SQL_API_SQLEXECUTE:
			debugPrintf("functionid: "
				"SQL_API_SQLEXECUTE "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		case SQL_API_SQLFETCH:
			debugPrintf("functionid: "
				"SQL_API_SQLFETCH "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		#if (ODBCVER >= 0x0300)
		case SQL_API_SQLFETCHSCROLL:
			debugPrintf("functionid: "
				"SQL_API_SQLFETCHSCROLL "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		#endif
		case SQL_API_SQLFREECONNECT:
			debugPrintf("functionid: "
				"SQL_API_SQLFREECONNECT "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		case SQL_API_SQLFREEENV:
			debugPrintf("functionid: "
				"SQL_API_SQLFREEENV "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		#if (ODBCVER >= 0x0300)
		case SQL_API_SQLFREEHANDLE:
			debugPrintf("functionid: "
				"SQL_API_SQLFREEHANDLE "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		#endif
		case SQL_API_SQLFREESTMT:
			debugPrintf("functionid: "
				"SQL_API_SQLFREESTMT "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		case SQL_API_SQLGETCONNECTOPTION:
			debugPrintf("functionid: "
				"SQL_API_SQLGETCONNECTOPTION "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		case SQL_API_SQLGETCURSORNAME:
			debugPrintf("functionid: "
				"SQL_API_SQLGETCURSORNAME "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		case SQL_API_SQLGETDATA:
			debugPrintf("functionid: "
				"SQL_API_SQLGETDATA "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		#if (ODBCVER >= 0x0300)
		case SQL_API_SQLGETDIAGFIELD:
			debugPrintf("functionid: "
				"SQL_API_SQLGETDIAGFIELD "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		case SQL_API_SQLGETDIAGREC:
			debugPrintf("functionid: "
				"SQL_API_SQLGETDIAGREC "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		case SQL_API_SQLGETENVATTR:
			debugPrintf("functionid: "
				"SQL_API_SQLGETENVATTR "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		#endif
		case SQL_API_SQLGETFUNCTIONS:
			debugPrintf("functionid: "
				"SQL_API_SQLGETFUNCTIONS "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		case SQL_API_SQLGETINFO:
			debugPrintf("functionid: "
				"SQL_API_SQLGETINFO "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		#if (ODBCVER >= 0x0300)
		case SQL_API_SQLGETSTMTATTR:
			debugPrintf("functionid: "
				"SQL_API_SQLGETSTMTATTR "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		#endif
		case SQL_API_SQLGETSTMTOPTION:
			debugPrintf("functionid: "
				"SQL_API_SQLGETSTMTOPTION "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		case SQL_API_SQLGETTYPEINFO:
			debugPrintf("functionid: "
				"SQL_API_SQLGETTYPEINFO "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		case SQL_API_SQLNUMRESULTCOLS:
			debugPrintf("functionid: "
				"SQL_API_SQLNUMRESULTCOLS "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		case SQL_API_SQLPREPARE:
			debugPrintf("functionid: "
				"SQL_API_SQLPREPARE "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		case SQL_API_SQLROWCOUNT:
			debugPrintf("functionid: "
				"SQL_API_SQLROWCOUNT "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		#if (ODBCVER >= 0x0300)
		case SQL_API_SQLSETCONNECTATTR:
			debugPrintf("functionid: "
				"SQL_API_SQLSETCONNECTATTR "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		#endif
		case SQL_API_SQLSETCONNECTOPTION:
			debugPrintf("functionid: "
				"SQL_API_SQLSETCONNECTOPTION "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		case SQL_API_SQLSETCURSORNAME:
			debugPrintf("functionid: "
				"SQL_API_SQLSETCURSORNAME "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		#if (ODBCVER >= 0x0300)
		case SQL_API_SQLSETENVATTR:
			debugPrintf("functionid: "
				"SQL_API_SQLSETENVATTR "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		#endif
		case SQL_API_SQLSETPARAM:
			debugPrintf("functionid: "
				"SQL_API_SQLSETPARAM "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		#if (ODBCVER >= 0x0300)
		case SQL_API_SQLSETSTMTATTR:
			debugPrintf("functionid: "
				"SQL_API_SQLSETSTMTATTR "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		#endif
		case SQL_API_SQLSETSTMTOPTION:
			debugPrintf("functionid: "
				"SQL_API_SQLSETSTMTOPTION "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		case SQL_API_SQLTRANSACT:
			debugPrintf("functionid: "
				"SQL_API_SQLTRANSACT "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		#if (ODBCVER >= 0x0300)
		case SQL_API_SQLALLOCHANDLESTD:
			debugPrintf("functionid: "
				"SQL_API_SQLALLOCHANDLESTD "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		#endif
		case SQL_API_SQLBINDPARAMETER:
			debugPrintf("functionid: "
				"SQL_API_SQLBINDPARAMETER "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		case SQL_API_SQLBROWSECONNECT:
			debugPrintf("functionid: "
				"SQL_API_SQLBROWSECONNECT "
				"- false\n");
			*supported=SQL_FALSE;
			break;
		case SQL_API_SQLDRIVERCONNECT:
			debugPrintf("functionid: "
				"SQL_API_SQLDRIVERCONNECT "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		case SQL_API_SQLEXTENDEDFETCH:
			debugPrintf("functionid: "
				"SQL_API_SQLEXTENDEDFETCH "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		case SQL_API_SQLMORERESULTS:
			debugPrintf("functionid: "
				"SQL_API_SQLMORERESULTS "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		case SQL_API_SQLNUMPARAMS:
			debugPrintf("functionid: "
				"SQL_API_SQLNUMPARAMS "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		case SQL_API_SQLPARAMOPTIONS:
			debugPrintf("functionid: "
				"SQL_API_SQLPARAMOPTIONS "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		case SQL_API_SQLSETPOS:
			debugPrintf("functionid: "
				"SQL_API_SQLSETPOS "
				"- false\n");
			*supported=SQL_FALSE;
			break;
		case SQL_API_SQLSETSCROLLOPTIONS:
			debugPrintf("functionid: "
				"SQL_API_SQLSETSCROLLOPTIONS "
				"- false\n");
			*supported=SQL_FALSE;
			break;
		case SQL_API_SQLCANCEL:
			debugPrintf("functionid: "
				"SQL_API_SQLCANCEL "
				"- false\n");
			*supported=SQL_FALSE;
			break;
		case SQL_API_SQLCOLUMNS:
			debugPrintf("functionid: "
				"SQL_API_SQLCOLUMNS "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		case SQL_API_SQLDATASOURCES:
			debugPrintf("functionid: "
				"SQL_API_SQLDATASOURCES "
				"- false\n");
			*supported=SQL_FALSE;
			break;
		case SQL_API_SQLPUTDATA:
			debugPrintf("functionid: "
				"SQL_API_SQLPUTDATA "
				"- false\n");
			*supported=SQL_FALSE;
			break;
		case SQL_API_SQLPARAMDATA:
			debugPrintf("functionid: "
				"SQL_API_SQLPARAMDATA "
				"- false\n");
			*supported=SQL_FALSE;
			break;
		#if (ODBCVER >= 0x0300)
		case SQL_API_SQLGETCONNECTATTR:
			debugPrintf("functionid: "
				"SQL_API_SQLGETCONNECTATTR "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		#endif
		case SQL_API_SQLSPECIALCOLUMNS:
			debugPrintf("functionid: "
				"SQL_API_SQLSPECIALCOLUMNS "
				"- false\n");
			*supported=SQL_FALSE;
			break;
		case SQL_API_SQLSTATISTICS:
			debugPrintf("functionid: "
				"SQL_API_SQLSTATISTICS "
				"- false\n");
			*supported=SQL_FALSE;
			break;
		case SQL_API_SQLTABLES:
			debugPrintf("functionid: "
				"SQL_API_SQLTABLES "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		#if (ODBCVER >= 0x0300)
		case SQL_API_SQLBULKOPERATIONS:
			debugPrintf("functionid: "
				"SQL_API_SQLBULKOPERATIONS "
				"- false\n");
			*supported=SQL_FALSE;
			break;
		#endif
		case SQL_API_SQLCOLUMNPRIVILEGES:
			debugPrintf("functionid: "
				"SQL_API_SQLCOLUMNPRIVILEGES "
				"- false\n");
			*supported=SQL_FALSE;
			break;
		case SQL_API_SQLDESCRIBEPARAM:
			debugPrintf("functionid: "
				"SQL_API_SQLDESCRIBEPARAM "
				"- false\n");
			*supported=SQL_FALSE;
			break;
		case SQL_API_SQLFOREIGNKEYS:
			debugPrintf("functionid: "
				"SQL_API_SQLFOREIGNKEYS "
				"- false\n");
			*supported=SQL_FALSE;
			break;
		case SQL_API_SQLNATIVESQL:
			debugPrintf("functionid: "
				"SQL_API_SQLNATIVESQL "
				"- false\n");
			*supported=SQL_FALSE;
			break;
		case SQL_API_SQLPRIMARYKEYS:
			debugPrintf("functionid: "
				"SQL_API_SQLPRIMARYKEYS "
				"- false\n");
			*supported=SQL_FALSE;
			break;
		case SQL_API_SQLPROCEDURECOLUMNS:
			debugPrintf("functionid: "
				"SQL_API_SQLPROCEDURECOLUMNS "
				"- false\n");
			*supported=SQL_FALSE;
			break;
		case SQL_API_SQLPROCEDURES:
			debugPrintf("functionid: "
				"SQL_API_SQLPROCEDURES "
				"- false\n");
			*supported=SQL_FALSE;
			break;
		case SQL_API_SQLTABLEPRIVILEGES:
			debugPrintf("functionid: "
				"SQL_API_SQLTABLEPRIVILEGES "
				"- false\n");
			*supported=SQL_FALSE;
			break;
		case SQL_API_SQLDRIVERS:
			debugPrintf("functionid: "
				"SQL_API_SQLDRIVERS "
				"- false\n");
			*supported=SQL_FALSE;
			break;
		#if (ODBCVER >= 0x0300)
		case SQL_API_SQLGETDESCFIELD:
			debugPrintf("functionid: "
				"SQL_API_SQLGETDESCFIELD "
				"- false\n");
			*supported=SQL_FALSE;
			break;
		case SQL_API_SQLGETDESCREC:
			debugPrintf("functionid: "
				"SQL_API_SQLGETDESCREC "
				"- false\n");
			*supported=SQL_FALSE;
			break;
		case SQL_API_SQLSETDESCFIELD:
			debugPrintf("functionid: "
				"SQL_API_SQLSETDESCFIELD "
				"- false\n");
			*supported=SQL_FALSE;
			break;
		case SQL_API_SQLSETDESCREC:
			debugPrintf("functionid: "
				"SQL_API_SQLSETDESCREC "
				"- false\n");
			*supported=SQL_FALSE;
			break;
		#endif
		default:
			debugPrintf("invalid functionid");
			return SQL_ERROR;
	}

	return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLGetInfo(SQLHDBC connectionhandle,
					SQLUSMALLINT infotype,
					SQLPOINTER infovalue,
					SQLSMALLINT bufferlength,
					SQLSMALLINT *stringlength) {
	debugFunction();

	CONN	*conn=(CONN *)connectionhandle;
	if (connectionhandle==SQL_NULL_HANDLE || !conn || !conn->con) {
		debugPrintf("NULL conn handle\n");
		return SQL_INVALID_HANDLE;
	}

	const char	*strval=NULL;

	// FIXME: there are tons more of these...
	// http://vieka.com/esqldoc/esqlref/htm/odbcsqlgetinfo.htm
	switch (infotype) {
		case SQL_DRIVER_ODBC_VER:
			debugPrintf("infotype: SQL_DRIVER_ODBC_VER\n");
			strval="03.00";
			break;
		case SQL_DRIVER_NAME:
			debugPrintf("infotype: SQL_DRIVER_NAME\n");
			strval="SQL Relay";
			break;
		case SQL_DRIVER_VER:
			debugPrintf("infotype: SQL_DRIVER_VER\n");
			strval=conn->con->clientVersion();
			break;
		case SQL_DBMS_NAME:
			debugPrintf("infotype: SQL_DBMS_NAME\n");
			strval=conn->con->identify();
			break;
		case SQL_DBMS_VER:
			debugPrintf("infotype: SQL_DBMS_VER\n");
			strval=conn->con->dbVersion();
			break;
		case SQL_DATABASE_NAME:
			debugPrintf("infotype: SQL_DATABASE_NAME\n");
			strval=conn->con->getCurrentDatabase();
			break;
		case SQL_TXN_CAPABLE:
			debugPrintf("infotype: SQL_TXN_CAPABLE\n");
			// FIXME: this isn't true for all db's
			*(SQLUSMALLINT *)infovalue=SQL_TC_ALL;
			break;
		case SQL_SCROLL_OPTIONS:
			debugPrintf("infotype: SQL_SCROLL_OPTIONS\n");
			*(SQLUINTEGER *)infovalue=SQL_SO_FORWARD_ONLY;
			break;
		default:
			debugPrintf("unsupported infotype\n");
			break;
	}

	// copy out the string value
	if (strval) {
		charstring::safeCopy((char *)infovalue,bufferlength,strval);
		debugPrintf("infovalue: %s\n",(const char *)infovalue);
		if (stringlength) {
			*stringlength=charstring::length(strval);
			debugPrintf("stringlength: %d\n",(int)*stringlength);
		}
	}

	return SQL_SUCCESS;
}

static SQLRETURN SQLR_SQLGetStmtAttr(SQLHSTMT statementhandle,
					SQLINTEGER attribute,
					SQLPOINTER value,
					SQLINTEGER bufferlength,
					SQLINTEGER *stringlength) {
	debugFunction();

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	switch (attribute) {
		#if (ODBCVER >= 0x0300)
		case SQL_ATTR_APP_ROW_DESC:
			debugPrintf("attribute: SQL_ATTR_APP_ROW_DESC\n");
			*(rowdesc **)value=stmt->approwdesc;
			break;
		case SQL_ATTR_APP_PARAM_DESC:
			debugPrintf("attribute: SQL_ATTR_APP_PARAM_DESC\n");
			*(paramdesc **)value=stmt->appparamdesc;
			break;
		case SQL_ATTR_IMP_ROW_DESC:
			debugPrintf("attribute: SQL_ATTR_IMP_ROW_DESC\n");
			*(rowdesc **)value=stmt->improwdesc;
			break;
		case SQL_ATTR_IMP_PARAM_DESC:
			debugPrintf("attribute: SQL_ATTR_IMP_PARAM_DESC\n");
			*(paramdesc **)value=stmt->impparamdesc;
			break;
		case SQL_ATTR_CURSOR_SCROLLABLE:
			debugPrintf("attribute: SQL_ATTR_CURSOR_SCROLLABLE\n");
			// FIXME: implement
			break;
		case SQL_ATTR_CURSOR_SENSITIVITY:
			debugPrintf("attribute: SQL_ATTR_CURSOR_SENSITIVITY\n");
			// FIXME: implement
			break;
		#endif
		//case SQL_ATTR_QUERY_TIMEOUT:
		case SQL_QUERY_TIMEOUT:
			debugPrintf("attribute: "
					"SQL_ATTR_QUERY_TIMEOUT/"
					"SQL_QUERY_TIMEOUT\n");
			// FIXME: implement
			break;
		//case SQL_ATTR_MAX_ROWS:
		case SQL_MAX_ROWS:
			debugPrintf("attribute: "
					"SQL_ATTR_MAX_ROWS/"
					"SQL_MAX_ROWS:\n");
			// FIXME: implement
			break;
		//case SQL_ATTR_NOSCAN:
		case SQL_NOSCAN:
			debugPrintf("attribute: "
					"SQL_ATTR_NOSCAN/"
					"SQL_NOSCAN\n");
			// FIXME: implement
			break;
		//case SQL_ATTR_MAX_LENGTH:
		case SQL_MAX_LENGTH:
			debugPrintf("attribute: "
					"SQL_ATTR_MAX_LENGTH/"
					"SQL_MAX_LENGTH\n");
			// FIXME: implement
			break;
		//case SQL_ATTR_ASYNC_ENABLE:
		case SQL_ASYNC_ENABLE:
			debugPrintf("attribute: "
					"SQL_ATTR_ASYNC_ENABLE/"
					"SQL_ASYNC_ENABLE\n");
			// FIXME: implement
			break;
		//case SQL_ATTR_ROW_BIND_TYPE:
		case SQL_BIND_TYPE:
			debugPrintf("attribute: "
					"SQL_ATTR_BIND_TYPE/"
					"SQL_BIND_TYPE\n");
			// FIXME: implement
			break;
		//case SQL_ATTR_CONCURRENCY:
		//case SQL_ATTR_CURSOR_TYPE:
		case SQL_CURSOR_TYPE:
			debugPrintf("attribute: "
					"SQL_ATTR_CONCURRENCY/"
					"SQL_ATTR_CURSOR_TYPE/"
					"SQL_CURSOR_TYPE\n");
			// FIXME: implement
			break;
		case SQL_CONCURRENCY:
			debugPrintf("attribute: SQL_CONCURRENCY\n");
			// FIXME: implement
			break;
		//case SQL_ATTR_KEYSET_SIZE:
		case SQL_KEYSET_SIZE:
			debugPrintf("attribute: "
					"SQL_ATTR_KEYSET_SIZE/"
					"SQL_KEYSET_SIZE\n");
			// FIXME: implement
			break;
		case SQL_ROWSET_SIZE:
			debugPrintf("attribute: SQL_ROWSET_SIZE\n");
			// FIXME: implement
			break;
		//case SQL_ATTR_SIMULATE_CURSOR:
		case SQL_SIMULATE_CURSOR:
			debugPrintf("attribute: "
					"SQL_ATTR_SIMULATE_CURSOR/"
					"SQL_SIMULATE_CURSOR\n");
			// FIXME: implement
			break;
		//case SQL_ATTR_RETRIEVE_DATA:
		case SQL_RETRIEVE_DATA:
			debugPrintf("attribute: "
					"SQL_ATTR_RETRIEVE_DATA/"
					"SQL_RETRIEVE_DATA\n");
			// FIXME: implement
			break;
		//case SQL_ATTR_USE_BOOKMARKS:
		case SQL_USE_BOOKMARKS:
			debugPrintf("attribute: "
					"SQL_ATTR_USE_BOOKMARKS/"
					"SQL_USE_BOOKMARKS\n");
			// FIXME: implement
			break;
		case SQL_GET_BOOKMARK:
			debugPrintf("attribute: SQL_GET_BOOKMARK\n");
			// FIXME: implement
			break;
		// case SQL_ATTR_ROW_NUMBER
		case SQL_ROW_NUMBER:
			debugPrintf("attribute: "
					"SQL_ATTR_ROW_NUMBER/"
					"SQL_ROW_NUMBER\n");
			// FIXME: implement
			break;
		#if (ODBCVER >= 0x0300)
		case SQL_ATTR_ENABLE_AUTO_IPD:
			debugPrintf("attribute: SQL_ATTR_ENABLE_AUTO_IPD\n");
			// FIXME: implement
			break;
		case SQL_ATTR_FETCH_BOOKMARK_PTR:
			debugPrintf("attribute: SQL_ATTR_FETCH_BOOKMARK_PTR\n");
			// FIXME: implement
			break;
		case SQL_ATTR_PARAM_BIND_OFFSET_PTR:
			debugPrintf("attribute: "
					"SQL_ATTR_PARAM_BIND_OFFSET_PTR\n");
			// FIXME: implement
			break;
		case SQL_ATTR_PARAM_BIND_TYPE:
			debugPrintf("attribute: SQL_ATTR_PARAM_BIND_TYPE\n");
			// FIXME: implement
			break;
		case SQL_ATTR_PARAM_OPERATION_PTR:
			debugPrintf("attribute: "
					"SQL_ATTR_PARAM_OPERATION_PTR\n");
			// FIXME: implement
			break;
		case SQL_ATTR_PARAM_STATUS_PTR:
			debugPrintf("attribute: SQL_ATTR_PARAM_STATUS_PTR\n");
			// FIXME: implement
			break;
		case SQL_ATTR_PARAMS_PROCESSED_PTR:
			debugPrintf("attribute: "
					"SQL_ATTR_PARAMS_PROCESSED_PTR\n");
			// FIXME: implement
			break;
		case SQL_ATTR_PARAMSET_SIZE:
			debugPrintf("attribute: SQL_ATTR_PARAMSET_SIZE\n");
			// FIXME: implement
			break;
		case SQL_ATTR_ROW_BIND_OFFSET_PTR:
			debugPrintf("attribute: "
					"SQL_ATTR_ROW_BIND_OFFSET_PTR\n");
			// FIXME: implement
			break;
		case SQL_ATTR_ROW_OPERATION_PTR:
			debugPrintf("attribute: SQL_ATTR_ROW_OPERATION_PTR\n");
			// FIXME: implement
			break;
		case SQL_ATTR_ROW_STATUS_PTR:
			debugPrintf("attribute: SQL_ATTR_ROW_STATUS_PTR\n");
			// I think this is supposed to be write-only
			break;
		case SQL_ATTR_ROWS_FETCHED_PTR:
			debugPrintf("attribute: SQL_ATTR_ROWS_FETCHED_PTR\n");
			// I think this is supposed to be write-only
			break;
		case SQL_ATTR_ROW_ARRAY_SIZE:
			debugPrintf("attribute: SQL_ATTR_ROW_ARRAY_SIZE\n");
			*(SQLULEN *)value=stmt->cur->getResultSetBufferSize();
			break;
		#endif
		#if (ODBCVER < 0x0300)
		case SQL_STMT_OPT_MAX:
			debugPrintf("attribute: SQL_STMT_OPT_MAX\n");
			// FIXME: implement
			break;
		case SQL_STMT_OPT_MIN:
			debugPrintf("attribute: SQL_STMT_OPT_MIN\n");
			// FIXME: implement
			break;
		#endif
		default:
			debugPrintf("invalid attribute\n");
			return SQL_ERROR;
	}
	return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLGetStmtAttr(SQLHSTMT statementhandle,
					SQLINTEGER attribute,
					SQLPOINTER value,
					SQLINTEGER bufferlength,
					SQLINTEGER *stringlength) {
	debugFunction();
	return SQLR_SQLGetStmtAttr(statementhandle,attribute,
					value,bufferlength,stringlength);
}

SQLRETURN SQL_API SQLGetStmtOption(SQLHSTMT statementhandle,
					SQLUSMALLINT option,
					SQLPOINTER value) {
	debugFunction();
	return SQLR_SQLGetStmtAttr(statementhandle,option,value,-1,NULL);
}

SQLRETURN SQL_API SQLGetTypeInfo(SQLHSTMT statementhandle,
					SQLSMALLINT DataType) {
	debugFunction();
	// not supported, return success though, JDBC-ODBC bridge really
	// wants this function to work and it will fail gracefully when
	// attempts to fetch the result set result in no data
	return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLNumResultCols(SQLHSTMT statementhandle,
					SQLSMALLINT *columncount) {
	debugFunction();

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	*columncount=(SQLSMALLINT)stmt->cur->colCount();
	debugPrintf("columncount: %d\n",(int)*columncount);

	return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLParamData(SQLHSTMT statementhandle,
					SQLPOINTER *Value) {
	debugFunction();

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	// not supported
	SQLR_STMTSetError(stmt,NULL,0,"IM001");

	return SQL_ERROR;
}

SQLRETURN SQL_API SQLPrepare(SQLHSTMT statementhandle,
					SQLCHAR *statementtext,
					SQLINTEGER textlength) {
	debugFunction();

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	// trim query
	uint32_t	statementtextlength=SQLR_TrimQuery(
						statementtext,textlength);

	// prepare the query
	#ifdef DEBUG_MESSAGES
	stringbuffer	debugstr;
	debugstr.append(statementtext,statementtextlength);
	debugPrintf("statement: \"%s\",%d)\n",
			debugstr.getString(),(int)statementtextlength);
	#endif
	stmt->cur->prepareQuery((const char *)statementtext,
						statementtextlength);

	return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLPutData(SQLHSTMT statementhandle,
					SQLPOINTER Data,
					SQLLEN StrLen_or_Ind) {
	debugFunction();

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	// not supported
	SQLR_STMTSetError(stmt,NULL,0,"IM001");

	return SQL_ERROR;
}

SQLRETURN SQL_API SQLRowCount(SQLHSTMT statementhandle,
					SQLLEN *rowcount) {
	debugFunction();

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	*rowcount=stmt->cur->affectedRows();

	return SQL_SUCCESS;
}

static SQLRETURN SQLR_SQLSetConnectAttr(SQLHDBC connectionhandle,
					SQLINTEGER attribute,
					SQLPOINTER value,
					SQLINTEGER stringlength) {
	debugFunction();

	CONN	*conn=(CONN *)connectionhandle;
	if (connectionhandle==SQL_NULL_HANDLE || !conn || !conn->con) {
		debugPrintf("NULL conn handle\n");
		return SQL_INVALID_HANDLE;
	}

	if (attribute==SQL_AUTOCOMMIT) {
		debugPrintf("attribute: SQL_AUTOCOMMIT\n");
		// use reinterpret_cast to avoid compiler warnings
		uint64_t	val=reinterpret_cast<uint64_t>(value);
		if (val==SQL_AUTOCOMMIT_ON) {
			if (conn->con->autoCommitOn()) {
				return SQL_SUCCESS;
			}
		} else if (val==SQL_AUTOCOMMIT_OFF) {
			if (conn->con->autoCommitOff()) {
				return SQL_SUCCESS;
			}
		}
	}

	// FIXME: implement
 	// SQL_ACCESS_MODE
	// SQL_LOGIN_TIMEOUT
	// SQL_OPT_TRACE
	// SQL_OPT_TRACEFILE
	// SQL_TRANSLATE_DLL
	// SQL_TRANSLATE_OPTION
	// SQL_TXN_ISOLATION
	// SQL_CURRENT_QUALIFIER
	// SQL_ODBC_CURSORS
	// SQL_QUIET_MODE
	// SQL_PACKET_SIZE
	// #if (ODBCVER >= 0x0300)
	// SQL_ATTR_CONNECTION_TIMEOUT
	// SQL_ATTR_DISCONNECT_BEHAVIOR
	// SQL_ATTR_ENLIST_IN_DTC
	// SQL_ATTR_ENLIST_IN_XA
	// SQL_ATTR_AUTO_IPD
	// SQL_ATTR_METADATA_ID
	// #endif

	debugPrintf("unsupported attribute\n");

	return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLSetConnectAttr(SQLHDBC connectionhandle,
					SQLINTEGER attribute,
					SQLPOINTER value,
					SQLINTEGER stringlength) {
	debugFunction();
	return SQLR_SQLSetConnectAttr(connectionhandle,attribute,
						value,stringlength);
}

SQLRETURN SQL_API SQLSetConnectOption(SQLHDBC connectionhandle,
					SQLUSMALLINT option,
					SQLULEN value) {
	debugFunction();
	return SQLR_SQLSetConnectAttr(connectionhandle,option,
						(SQLPOINTER)value,0);
}

SQLRETURN SQL_API SQLSetCursorName(SQLHSTMT statementhandle,
					SQLCHAR *cursorname,
					SQLSMALLINT namelength) {
	debugFunction();

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	delete[] stmt->name;
	if (namelength==SQL_NTS) {
		stmt->name=charstring::duplicate((const char *)cursorname);
	} else {
		stmt->name=charstring::duplicate((const char *)cursorname,
								namelength);
	}

	return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLSetDescField(SQLHDESC DescriptorHandle,
					SQLSMALLINT RecNumber,
					SQLSMALLINT FieldIdentifier,
					SQLPOINTER Value,
					SQLINTEGER BufferLength) {
	debugFunction();
	// not supported
	return SQL_ERROR;
}

SQLRETURN SQL_API SQLSetDescRec(SQLHDESC DescriptorHandle,
					SQLSMALLINT RecNumber,
					SQLSMALLINT Type,
					SQLSMALLINT SubType,
					SQLLEN Length,
					SQLSMALLINT Precision,
					SQLSMALLINT Scale,
					SQLPOINTER Data,
					SQLLEN *StringLength,
					SQLLEN *Indicator) {
	debugFunction();
	// not supported
	return SQL_ERROR;
}

SQLRETURN SQL_API SQLSetEnvAttr(SQLHENV environmenthandle,
					SQLINTEGER attribute,
					SQLPOINTER value,
					SQLINTEGER stringlength) {
	debugFunction();

	ENV	*env=(ENV *)environmenthandle;
	if (environmenthandle==SQL_NULL_HENV || !env) {
		debugPrintf("NULL env handle\n");
		return SQL_INVALID_HANDLE;
	}

	switch (attribute) {
		case SQL_ATTR_OUTPUT_NTS:
			debugPrintf("attribute: SQL_ATTR_OUTPUT_NTS\n");
			// this can't be set to false
			return ((uint32_t)value==SQL_TRUE)?
						SQL_SUCCESS:SQL_ERROR;
		case SQL_ATTR_ODBC_VERSION:
			debugPrintf("attribute: SQL_ATTR_ODBC_VERSION\n");
			switch ((uint32_t)value) {
				case SQL_OV_ODBC2:
					env->odbcversion=SQL_OV_ODBC2;
					break;
				case SQL_OV_ODBC3:
					env->odbcversion=SQL_OV_ODBC3;
					break;
			}
			debugPrintf("odbcversion: %d\n",(int)env->odbcversion);
			return SQL_SUCCESS;
		case SQL_ATTR_CONNECTION_POOLING:
			debugPrintf("attribute: SQL_ATTR_CONNECTION_POOLING\n");
			// this can't be set on
			return ((uint32_t)value==SQL_CP_OFF)?
						SQL_SUCCESS:SQL_ERROR;
		case SQL_ATTR_CP_MATCH:
			debugPrintf("attribute: SQL_ATTR_CP_MATCH\n");
			// this can't be set to anything but default
			return ((uint32_t)value==SQL_CP_MATCH_DEFAULT)?
						SQL_SUCCESS:SQL_ERROR;
		default:
			debugPrintf("unsupported attribute\n");
			return SQL_SUCCESS;
	}
}

SQLRETURN SQL_API SQLSetParam(SQLHSTMT statementhandle,
					SQLUSMALLINT parameternumber,
					SQLSMALLINT valuetype,
					SQLSMALLINT parametertype,
					SQLULEN lengthprecision,
					SQLSMALLINT parameterscale,
					SQLPOINTER parametervalue,
					SQLLEN *strlen_or_ind) {
	debugFunction();
	return SQLR_SQLBindParameter(statementhandle,
					parameternumber,
					SQL_PARAM_INPUT,
					valuetype,
					parametertype,
					lengthprecision,
					parameterscale,
					parametervalue,
					0,
					strlen_or_ind);
}

static SQLRETURN SQLR_SQLSetStmtAttr(SQLHSTMT statementhandle,
					SQLINTEGER attribute,
					SQLPOINTER value,
					SQLINTEGER stringlength) {
	debugFunction();

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	switch (attribute) {
		#if (ODBCVER >= 0x0300)
		case SQL_ATTR_APP_ROW_DESC:
			debugPrintf("attribute: SQL_ATTR_APP_ROW_DESC\n");
			stmt->approwdesc=(rowdesc *)value;
			if (stmt->approwdesc==SQL_NULL_DESC) {
				stmt->approwdesc=stmt->improwdesc;
			}
			return SQL_SUCCESS;
		case SQL_ATTR_APP_PARAM_DESC:
			debugPrintf("attribute: SQL_ATTR_APP_PARAM_DESC\n");
			stmt->appparamdesc=(paramdesc *)value;
			if (stmt->appparamdesc==SQL_NULL_DESC) {
				stmt->appparamdesc=stmt->impparamdesc;
			}
			return SQL_SUCCESS;
		case SQL_ATTR_IMP_ROW_DESC:
			debugPrintf("attribute: SQL_ATTR_IMP_ROW_DESC\n");
			// read-only
			return SQL_ERROR;
		case SQL_ATTR_IMP_PARAM_DESC:
			debugPrintf("attribute: SQL_ATTR_IMP_PARAM_DESC\n");
			// read-only
			return SQL_ERROR;
		case SQL_ATTR_CURSOR_SCROLLABLE:
			debugPrintf("attribute: SQL_ATTR_CURSOR_SCROLLABLE\n");
			// FIXME: implement
			return SQL_SUCCESS;
		case SQL_ATTR_CURSOR_SENSITIVITY:
			debugPrintf("attribute: SQL_ATTR_CURSOR_SENSITIVITY\n");
			// FIXME: implement
			return SQL_SUCCESS;
		#endif
		//case SQL_ATTR_QUERY_TIMEOUT:
		case SQL_QUERY_TIMEOUT:
			debugPrintf("attribute: "
					"SQL_ATTR_QUERY_TIMEOUT/"
					"SQL_QUERY_TIMEOUT\n");
			// FIXME: implement
			return SQL_SUCCESS;
		//case SQL_ATTR_MAX_ROWS:
		case SQL_MAX_ROWS:
			debugPrintf("attribute: "
					"SQL_ATTR_MAX_ROWS/"
					"SQL_MAX_ROWS\n");
			// FIXME: implement
			return SQL_SUCCESS;
		//case SQL_ATTR_NOSCAN:
		case SQL_NOSCAN:
			debugPrintf("attribute: "
					"SQL_ATTR_NOSCAN/"
					"SQL_NOSCAN\n");
			// FIXME: implement
			return SQL_SUCCESS;
		//case SQL_ATTR_MAX_LENGTH:
		case SQL_MAX_LENGTH:
			debugPrintf("attribute: "
					"SQL_ATTR_MAX_LENGTH/"
					"SQL_MAX_LENGTH\n");
			// FIXME: implement
			return SQL_SUCCESS;
		case SQL_ASYNC_ENABLE:
			debugPrintf("attribute: SQL_ASYNC_ENABLE\n");
			// FIXME: implement
			return SQL_SUCCESS;
		//case SQL_ATTR_ROW_BIND_TYPE:
		case SQL_BIND_TYPE:
			debugPrintf("attribute: "
					"SQL_ATTR_ROW_BIND_TYPE/"
					"SQL_BIND_TYPE\n");
			// FIXME: implement
			return SQL_SUCCESS;
		//case SQL_ATTR_CONCURRENCY:
		//case SQL_ATTR_CURSOR_TYPE:
		case SQL_CURSOR_TYPE:
			debugPrintf("attribute: "
					"SQL_ATTR_CONCURRENCY/"
					"SQL_ATTR_CURSOR_TYPE/"
					"SQL_CURSOR_TYPE\n");
			// FIXME: implement
			return SQL_SUCCESS;
		case SQL_CONCURRENCY:
			debugPrintf("attribute: SQL_CONCURRENCY\n");
			// FIXME: implement
			return SQL_SUCCESS;
		//case SQL_ATTR_KEYSET_SIZE:
		case SQL_KEYSET_SIZE:
			debugPrintf("attribute: "
					"SQL_ATTR_KEYSET_SIZE/"
					"SQL_KEYSET_SIZE\n");
			// FIXME: implement
			return SQL_SUCCESS;
		case SQL_ROWSET_SIZE:
			debugPrintf("attribute: SQL_ROWSET_SIZE\n");
			// FIXME: implement
			return SQL_SUCCESS;
		//case SQL_ATTR_SIMULATE_CURSOR:
		case SQL_SIMULATE_CURSOR:
			debugPrintf("attribute: "
					"SQL_ATTR_SIMULATE_CURSOR/"
					"SQL_SIMULATE_CURSOR\n");
			// FIXME: implement
			return SQL_SUCCESS;
		//case SQL_ATTR_RETRIEVE_DATA:
		case SQL_RETRIEVE_DATA:
			debugPrintf("attribute: "
					"SQL_ATTR_RETRIEVE_DATA/"
					"SQL_RETRIEVE_DATA\n");
			// FIXME: implement
			return SQL_SUCCESS;
		//case SQL_ATTR_USE_BOOKMARKS:
		case SQL_USE_BOOKMARKS:
			debugPrintf("attribute: "
					"SQL_ATTR_USE_BOOKMARKS/"
					"SQL_USE_BOOKMARKS\n");
			// FIXME: implement
			return SQL_SUCCESS;
		case SQL_GET_BOOKMARK:
			debugPrintf("attribute: SQL_GET_BOOKMARK\n");
			// FIXME: implement
			return SQL_SUCCESS;
		//case SQL_ATTR_ROW_NUMBER:
		case SQL_ROW_NUMBER:
			debugPrintf("attribute: "
					"SQL_ATTR_ROW_NUMBER/"
					"SQL_ROW_NUMBER\n");
			// FIXME: implement
			return SQL_SUCCESS;
		#if (ODBCVER >= 0x0300)
		case SQL_ATTR_ENABLE_AUTO_IPD:
			debugPrintf("attribute: SQL_ATTR_ENABLE_AUTO_IPD\n");
			// FIXME: implement
			return SQL_SUCCESS;
		case SQL_ATTR_FETCH_BOOKMARK_PTR:
			debugPrintf("attribute: SQL_ATTR_FETCH_BOOKMARK_PTR\n");
			// FIXME: implement
			return SQL_SUCCESS;
		case SQL_ATTR_PARAM_BIND_OFFSET_PTR:
			debugPrintf("attribute: "
					"SQL_ATTR_PARAM_BIND_OFFSET_PTR\n");
			// FIXME: implement
			return SQL_SUCCESS;
		case SQL_ATTR_PARAM_BIND_TYPE:
			debugPrintf("attribute: SQL_ATTR_PARAM_BIND_TYPE\n");
			// FIXME: implement
			return SQL_SUCCESS;
		case SQL_ATTR_PARAM_OPERATION_PTR:
			debugPrintf("attribute: "
					"SQL_ATTR_PARAM_OPERATION_PTR\n");
			// FIXME: implement
			return SQL_SUCCESS;
		case SQL_ATTR_PARAM_STATUS_PTR:
			debugPrintf("attribute: SQL_ATTR_PARAM_STATUS_PTR\n");
			// FIXME: implement
			return SQL_SUCCESS;
		case SQL_ATTR_PARAMS_PROCESSED_PTR:
			debugPrintf("attribute: "
					"SQL_ATTR_PARAMS_PROCESSED_PTR\n");
			// FIXME: implement
			return SQL_SUCCESS;
		case SQL_ATTR_PARAMSET_SIZE:
			debugPrintf("attribute: SQL_ATTR_PARAMSET_SIZE\n");
			// FIXME: implement
			return SQL_SUCCESS;
		case SQL_ATTR_ROW_BIND_OFFSET_PTR:
			debugPrintf("attribute: "	
					"SQL_ATTR_ROW_BIND_OFFSET_PTR\n");
			// FIXME: implement
			return SQL_SUCCESS;
		case SQL_ATTR_ROW_OPERATION_PTR:
			debugPrintf("attribute: SQL_ATTR_ROW_OPERATION_PTR\n");
			// FIXME: implement
			return SQL_SUCCESS;
		case SQL_ATTR_ROW_STATUS_PTR:
			debugPrintf("attribute: SQL_ATTR_ROW_STATUS_PTR\n");
			stmt->rowstatusptr=(SQLUSMALLINT *)value;
			return SQL_SUCCESS;
		case SQL_ATTR_ROWS_FETCHED_PTR:
			debugPrintf("attribute: SQL_ATTR_ROWS_FETCHED_PTR\n");
			stmt->rowsfetchedptr=(SQLROWSETSIZE *)value;
			return SQL_SUCCESS;
		case SQL_ATTR_ROW_ARRAY_SIZE:
			debugPrintf("attribute: SQL_ATTR_ROW_ARRAY_SIZE\n");
			// use reinterpret_cast to avoid compiler warnings
			stmt->cur->setResultSetBufferSize(
					reinterpret_cast<uint64_t>(value));
			return SQL_SUCCESS;
		#endif
		#if (ODBCVER < 0x0300)
		case SQL_STMT_OPT_MAX:
			debugPrintf("attribute: SQL_STMT_OPT_MAX\n");
			// FIXME: implement
			return SQL_SUCCESS;
		case SQL_STMT_OPT_MIN:
			debugPrintf("attribute: SQL_STMT_OPT_MIN\n");
			// FIXME: implement
			return SQL_SUCCESS;
		#endif
		default:
			return SQL_ERROR;
	}
}

SQLRETURN SQL_API SQLSetStmtAttr(SQLHSTMT statementhandle,
					SQLINTEGER attribute,
					SQLPOINTER value,
					SQLINTEGER stringlength) {
	debugFunction();
	return SQLR_SQLSetStmtAttr(statementhandle,attribute,
						value,stringlength);
}

SQLRETURN SQL_API SQLSetStmtOption(SQLHSTMT statementhandle,
					SQLUSMALLINT option,
					SQLULEN value) {
	debugFunction();
	return SQLSetStmtAttr(statementhandle,option,(SQLPOINTER)value,0);
}

SQLRETURN SQL_API SQLSpecialColumns(SQLHSTMT statementhandle,
					SQLUSMALLINT IdentifierType,
					SQLCHAR *CatalogName,
					SQLSMALLINT NameLength1,
					SQLCHAR *SchemaName,
					SQLSMALLINT NameLength2,
					SQLCHAR *TableName,
					SQLSMALLINT NameLength3,
					SQLUSMALLINT Scope,
					SQLUSMALLINT Nullable) {
	debugFunction();

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	// not supported
	SQLR_STMTSetError(stmt,NULL,0,"IM001");

	return SQL_ERROR;
}

SQLRETURN SQL_API SQLStatistics(SQLHSTMT statementhandle,
					SQLCHAR *CatalogName,
					SQLSMALLINT NameLength1,
					SQLCHAR *SchemaName,
					SQLSMALLINT NameLength2,
					SQLCHAR *TableName,
					SQLSMALLINT NameLength3,
					SQLUSMALLINT Unique,
					SQLUSMALLINT Reserved) {
	debugFunction();

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	// not supported
	SQLR_STMTSetError(stmt,NULL,0,"IM001");

	return SQL_ERROR;
}

SQLRETURN SQL_API SQLTables(SQLHSTMT statementhandle,
					SQLCHAR *catalogname,
					SQLSMALLINT namelength1,
					SQLCHAR *schemaname,
					SQLSMALLINT namelength2,
					SQLCHAR *tablename,
					SQLSMALLINT namelength3,
					SQLCHAR *tabletype,
					SQLSMALLINT namelength4) {
	debugFunction();

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	char	*wild=NULL;
	if (namelength3==SQL_NTS) {
		wild=charstring::duplicate((const char *)tablename);
	} else {
		wild=charstring::duplicate((const char *)tablename,
							namelength3);
	}
	debugPrintf("wild: %s\n",(wild)?wild:"");

	SQLRETURN	retval=(stmt->cur->getTableList(wild))?
						SQL_SUCCESS:SQL_ERROR;
	delete[] wild;
	return retval;
}

static SQLRETURN SQLR_SQLEndTran(SQLSMALLINT handletype,
					SQLHANDLE handle,
					SQLSMALLINT completiontype);

SQLRETURN SQL_API SQLTransact(SQLHENV environmenthandle,
					SQLHDBC connectionhandle,
					SQLUSMALLINT completiontype) {
	debugFunction();
	if (connectionhandle) {
		return SQLR_SQLEndTran(SQL_HANDLE_DBC,
					connectionhandle,
					completiontype);
	} else if (environmenthandle) {
		return SQLR_SQLEndTran(SQL_HANDLE_ENV,
					environmenthandle,
					completiontype);
	} else {
		debugPrintf("no valid handle\n");
		return SQL_INVALID_HANDLE;
	}
}

SQLRETURN SQL_API SQLDriverConnect(SQLHDBC hdbc,
					SQLHWND hwnd,
					SQLCHAR *szconnstrin,
					SQLSMALLINT cbconnstrin,
					SQLCHAR *szconnstrout,
					SQLSMALLINT cbconnstroutmax,
					SQLSMALLINT *pcbconnstrout,
					SQLUSMALLINT fdrivercompletion) {
	debugFunction();

	CONN	*conn=(CONN *)hdbc;
	if (hdbc==SQL_NULL_HANDLE || !conn) {
		debugPrintf("NULL conn handle\n");
		return SQL_INVALID_HANDLE;
	}

	// the connect string may not be null terminated, so make a copy that is
	char	*nulltermconnstr;
	if (cbconnstrin==SQL_NTS) {
		nulltermconnstr=charstring::duplicate(
					(const char *)szconnstrin);
	} else {
		nulltermconnstr=charstring::duplicate(
					(const char *)szconnstrin,
					cbconnstrin);
	}
	debugPrintf("connectstring: %s\n",nulltermconnstr);

	// parse out DSN, UID and PWD from the connect string
	parameterstring	pstr;
	pstr.parse(nulltermconnstr);
	const char	*servername=pstr.getValue("DSN");
	if (!charstring::length(servername)) {
		servername=pstr.getValue("dsn");
	}
	const char	*username=pstr.getValue("UID");
	if (!charstring::length(username)) {
		username=pstr.getValue("uid");
	}
	const char	*authentication=pstr.getValue("PWD");
	if (!charstring::length(authentication)) {
		authentication=pstr.getValue("pwd");
	}

	debugPrintf("servername: %s\n",servername);
	debugPrintf("username: %s\n",username);
	debugPrintf("authentication: %s\n",authentication);

	// just support SQL_DRIVER_NOPROMPT for now
	switch (fdrivercompletion) {
		case SQL_DRIVER_PROMPT:
			debugPrintf("fbdrivercompletion: "
					"SQL_DRIVER_PROMPT\n");
			return SQL_ERROR;
		case SQL_DRIVER_COMPLETE:
			debugPrintf("fbdrivercompletion: "
					"SQL_DRIVER_COMPLETE\n");
			return SQL_ERROR;
		case SQL_DRIVER_COMPLETE_REQUIRED:
			debugPrintf("fbdrivercompletion: "
					"SQL_DRIVER_COMPLETE_REQUIRED\n");
			return SQL_ERROR;
		case SQL_DRIVER_NOPROMPT:
			debugPrintf("fbdrivercompletion: "
					"SQL_DRIVER_NOPROMPT\n");
			if (!charstring::length(servername)) {
				return SQL_ERROR;
			}
			break;
	}

	// since we don't support prompting and updating the connect string...
	if (cbconnstrin==SQL_NTS) {
		*pcbconnstrout=charstring::length(szconnstrin);
	} else {
		*pcbconnstrout=cbconnstrin;
	}
	*pcbconnstrout=cbconnstrin;
	charstring::safeCopy((char *)szconnstrout,
				*pcbconnstrout,nulltermconnstr);

	// connect
	SQLRETURN	retval=SQLR_SQLConnect(hdbc,
					(SQLCHAR *)servername,
					charstring::length(servername),
					(SQLCHAR *)username,
					charstring::length(username),
					(SQLCHAR *)authentication,
					charstring::length(authentication));

	// clean up
	delete[] nulltermconnstr;

	return retval;
}

SQLRETURN SQL_API SQLBulkOperations(SQLHSTMT statementhandle,
					SQLSMALLINT Operation) {
	debugFunction();

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	// not supported
	SQLR_STMTSetError(stmt,NULL,0,"IM001");

	return SQL_ERROR;
}

SQLRETURN SQL_API SQLColAttributes(SQLHSTMT statementhandle,
					SQLUSMALLINT icol,
					SQLUSMALLINT fdesctype,
					SQLPOINTER rgbdesc,
					SQLSMALLINT cbdescmax,
					SQLSMALLINT *pcbdesc,
					SQLLEN *pfdesc) {
	debugFunction();
	return SQLR_SQLColAttribute(statementhandle,
					icol,
					fdesctype,
					rgbdesc,
					cbdescmax,
					pcbdesc,
					(NUMERICATTRIBUTETYPE)pfdesc);
}

SQLRETURN SQL_API SQLColumnPrivileges(SQLHSTMT statementhandle,
					SQLCHAR *szCatalogName,
					SQLSMALLINT cbCatalogName,
					SQLCHAR *szSchemaName,
					SQLSMALLINT cbSchemaName,
					SQLCHAR *szTableName,
					SQLSMALLINT cbTableName,
					SQLCHAR *szColumnName,
					SQLSMALLINT cbColumnName) {
	debugFunction();

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	// not supported
	SQLR_STMTSetError(stmt,NULL,0,"IM001");

	return SQL_ERROR;
}

SQLRETURN SQL_API SQLDescribeParam(SQLHSTMT statementhandle,
					SQLUSMALLINT ipar,
					SQLSMALLINT *pfSqlType,
					SQLULEN *pcbParamDef,
					SQLSMALLINT *pibScale,
					SQLSMALLINT *pfNullable) {
	debugFunction();

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	// not supported
	SQLR_STMTSetError(stmt,NULL,0,"IM001");

	return SQL_ERROR;
}

SQLRETURN SQL_API SQLExtendedFetch(SQLHSTMT statementhandle,
					SQLUSMALLINT fetchorientation,
					SQLLEN fetchoffset,
					SQLULEN *pcrow,
					SQLUSMALLINT *rgfrowstatus) {
	debugFunction();
	return SQLR_Fetch(statementhandle,NULL,NULL);
}

SQLRETURN SQL_API SQLForeignKeys(SQLHSTMT statementhandle,
					SQLCHAR *szPkCatalogName,
					SQLSMALLINT cbPkCatalogName,
					SQLCHAR *szPkSchemaName,
					SQLSMALLINT cbPkSchemaName,
					SQLCHAR *szPkTableName,
					SQLSMALLINT cbPkTableName,
					SQLCHAR *szFkCatalogName,
					SQLSMALLINT cbFkCatalogName,
					SQLCHAR *szFkSchemaName,
					SQLSMALLINT cbFkSchemaName,
					SQLCHAR *szFkTableName,
					SQLSMALLINT cbFkTableName) {
	debugFunction();

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	// not supported
	SQLR_STMTSetError(stmt,NULL,0,"IM001");

	return SQL_ERROR;
}

SQLRETURN SQL_API SQLMoreResults(SQLHSTMT statementhandle) {
	debugFunction();
	// only supports fetching the first result set of a query
	return SQL_NO_DATA_FOUND;
}

SQLRETURN SQL_API SQLNativeSql(SQLHDBC hdbc,
					SQLCHAR *szSqlStrIn,
					SQLINTEGER cbSqlStrIn,
					SQLCHAR *szSqlStr,
					SQLINTEGER cbSqlStrMax,
					SQLINTEGER *pcbSqlStr) {
	debugFunction();
	// not supported
	return SQL_ERROR;
}

SQLRETURN SQL_API SQLNumParams(SQLHSTMT statementhandle,
					SQLSMALLINT *pcpar) {
	debugFunction();

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	*pcpar=stmt->cur->countBindVariables();

	return SQL_SUCCESS;
}

static SQLRETURN SQLR_SQLSetStmtAttr(SQLHSTMT statementhandle,
					SQLINTEGER attribute,
					SQLPOINTER value,
					SQLINTEGER stringlength);

SQLRETURN SQL_API SQLParamOptions(SQLHSTMT statementhandle,
					SQLULEN crow,
					SQLULEN *pirow) {
	debugFunction();
	return (SQLR_SQLSetStmtAttr(statementhandle,
				SQL_ATTR_PARAMSET_SIZE,
				(SQLPOINTER)crow,0)==SQL_SUCCESS &&
		SQLR_SQLSetStmtAttr(statementhandle,
				SQL_ATTR_PARAMS_PROCESSED_PTR,
				(SQLPOINTER)pirow,0)==SQL_SUCCESS)?
				SQL_SUCCESS:SQL_ERROR;
}

SQLRETURN SQL_API SQLPrimaryKeys(SQLHSTMT statementhandle,
					SQLCHAR *szCatalogName,
					SQLSMALLINT cbCatalogName,
					SQLCHAR *szSchemaName,
					SQLSMALLINT cbSchemaName,
					SQLCHAR *szTableName,
					SQLSMALLINT cbTableName) {
	debugFunction();

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	// not supported
	SQLR_STMTSetError(stmt,NULL,0,"IM001");

	return SQL_ERROR;
}

SQLRETURN SQL_API SQLProcedureColumns(SQLHSTMT statementhandle,
					SQLCHAR *szCatalogName,
					SQLSMALLINT cbCatalogName,
					SQLCHAR *szSchemaName,
					SQLSMALLINT cbSchemaName,
					SQLCHAR *szProcName,
					SQLSMALLINT cbProcName,
					SQLCHAR *szColumnName,
					SQLSMALLINT cbColumnName) {
	debugFunction();

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	// not supported
	SQLR_STMTSetError(stmt,NULL,0,"IM001");

	return SQL_ERROR;
}

SQLRETURN SQL_API SQLProcedures(SQLHSTMT statementhandle,
					SQLCHAR *szCatalogName,
					SQLSMALLINT cbCatalogName,
					SQLCHAR *szSchemaName,
					SQLSMALLINT cbSchemaName,
					SQLCHAR *szProcName,
					SQLSMALLINT cbProcName) {
	debugFunction();

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	// not supported
	SQLR_STMTSetError(stmt,NULL,0,"IM001");

	return SQL_ERROR;
}

SQLRETURN SQL_API SQLSetPos(SQLHSTMT statementhandle,
					SQLSETPOSIROW irow,
					SQLUSMALLINT foption,
					SQLUSMALLINT flock) {
	debugFunction();

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	// not supported
	SQLR_STMTSetError(stmt,NULL,0,"IM001");

	return SQL_ERROR;
}

SQLRETURN SQL_API SQLTablePrivileges(SQLHSTMT statementhandle,
					SQLCHAR *szCatalogName,
					SQLSMALLINT cbCatalogName,
					SQLCHAR *szSchemaName,
					SQLSMALLINT cbSchemaName,
					SQLCHAR *szTableName,
					SQLSMALLINT cbTableName) {
	debugFunction();

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	// not supported
	SQLR_STMTSetError(stmt,NULL,0,"IM001");

	return SQL_ERROR;
}

#if (ODBCVER < 0x0300)
SQLRETURN SQL_API SQLDrivers(SQLHENV environmenthandle,
					SQLUSMALLINT fDirection,
					SQLCHAR *szDriverDesc,
					SQLSMALLINT cbDriverDescMax,
					SQLSMALLINT *pcbDriverDesc,
					SQLCHAR *szDriverAttributes,
					SQLSMALLINT cbDrvrAttrMax,
					SQLSMALLINT *pcbDrvrAttr) {
	debugFunction();

	// FIXME: this is allegedly mapped in ODBC3 but I can't tell what to

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	// not supported
	SQLR_STMTSetError(stmt,NULL,0,"IM001");

	return SQL_ERROR;
}
#endif

static const char *SQLR_BuildNumeric(STMT *stmt,
					int32_t parameternumber,
					SQL_NUMERIC_STRUCT *ns) {
	debugFunction();

	// Get the numeric array as a base-10 number. It should be OK to
	// convert it to a 64-bit integer as SQL_MAX_NUMERIC_LEN should be 16
	// or less.
	//
	//  A number is stored in the val field of the SQL_NUMERIC_STRUCT
	//  structure as a scaled integer, in little endian mode (the leftmost
	//  byte being the least-significant byte). For example, the number
	//  10.001 base 10, with a scale of 4, is scaled to an integer of
	//  100010. Because this is 186AA in hexadecimal format, the value in
	//  SQL_NUMERIC_STRUCT would be "AA 86 01 00 00 ... 00", with the number
	//  of bytes defined by the SQL_MAX_NUMERIC_LEN #define...
	uint64_t	num=0;
	for (int8_t i=SQL_MAX_NUMERIC_LEN; i>=0; i--) {
		num=num*16+ns->val[i];
	}

	// build up a string from that number
	uint8_t	size=ns->precision+ns->sign+((ns->scale>0)?1:0);
	char	*string=new char[size+1];
	string[size]='\0';
	uint8_t	index=size-1;
	for (uint8_t i=0; i<ns->scale; i++) {
		string[index]=num%10+'0';
		num=num/10;
		index--;
	}
	if (ns->scale) {
		string[index]='.';
	}
	uint8_t	tens=ns->precision-ns->scale;
	for (uint8_t i=0; i<tens; i++) {
		string[index]=num%10+'0';
		num=num/10;
		index--;
	}
	if (ns->sign) {
		string[index]='-';
	}

	// hang on to that string
	char	*data=NULL;
	if (stmt->inputbindstrings.getData(parameternumber,&data)) {
		stmt->inputbindstrings.removeData(parameternumber);
		delete[] data;
	}
	stmt->inputbindstrings.setData(parameternumber,string);

	// return the string
	return string;
}

static const char *SQLR_BuildInterval(STMT *stmt,
				int32_t parameternumber,
				SQL_INTERVAL_STRUCT *is) {
	debugFunction();

	// create a string to store the built-up interval
	char	*string=new char[1];

	// FIXME: implement
	string[0]='\0';

	//typedef struct tagSQL_INTERVAL_STRUCT
	//   {
	//   SQLINTERVAL interval_type;
	//   SQLSMALLINT   interval_sign;
	//   union
	//      {
	//      SQL_YEAR_MONTH_STRUCT year_month;
	//      SQL_DAY_SECOND_STRUCT day_second;
	//      } intval;
	//   }SQLINTERVAL_STRUCT;
	//
	//typedef enum
	//   {
	//   SQL_IS_YEAR=1,
	//   SQL_IS_MONTH=2,
	//   SQL_IS_DAY=3,
	//   SQL_IS_HOUR=4,
	//   SQL_IS_MINUTE=5,
	//   SQL_IS_SECOND=6,
	//   SQL_IS_YEAR_TO_MONTH=7,
	//   SQL_IS_DAY_TO_HOUR=8,
	//   SQL_IS_DAY_TO_MINUTE=9,
	//   SQL_IS_DAY_TO_SECOND=10,
	//   SQL_IS_HOUR_TO_MINUTE=11,
	//   SQL_IS_HOUR_TO_SECOND=12,
	//   SQL_IS_MINUTE_TO_SECOND=13,
	//   }SQLINTERVAL;
	//
	//typedef struct tagSQL_YEAR_MONTH
	//   {
	//   SQLUINTEGER year;
	//   SQLUINTEGER month;
	//   }SQL_YEAR_MOHTH_STRUCT;
	//
	//typedef struct tagSQL_DAY_SECOND
	//   {
	//   SQLUINTEGER day;
	//   SQLUNINTEGER hour;
	//   SQLUINTEGER minute;
	//   SQLUINTEGER second;
	//   SQLUINTEGER fraction;
	//   }SQL_DAY_SECOND_STRUCT;

	// hang on to that string
	char	*data=NULL;
	if (stmt->inputbindstrings.getData(parameternumber,&data)) {
		stmt->inputbindstrings.removeData(parameternumber);
		delete[] data;
	}
	stmt->inputbindstrings.setData(parameternumber,string);

	// return the string
	return string;
}

static char SQLR_HexToChar(const char input) {
	debugFunction();
	char	ch=input;
	if (ch>=0 && ch<=9) {
		ch=ch+'0';
	} else if (ch>=10 && ch<=16) {
		ch=ch-10+'A';
	} else {
		ch='0';
	}
	return ch;
}

static const char *SQLR_BuildGuid(STMT *stmt,
				int32_t parameternumber, SQLGUID *guid) {
	debugFunction();

	// create a string to store the built-up guid
	char	*string=new char[37];

	// decode the guid struct
	uint32_t	data1=guid->Data1;
	for (int16_t index=7; index>=0; index--) {
		string[index]=SQLR_HexToChar(data1%16);
		data1=data1/16;
	}
	string[8]='-';

	uint16_t	data2=guid->Data2;
	for (int16_t index=12; index>=9; index--) {
		string[index]=SQLR_HexToChar(data2%16);
		data2=data2/16;
	}
	string[13]='-';

	uint16_t	data3=guid->Data3;
	for (int16_t index=17; index>=14; index--) {
		string[index]=SQLR_HexToChar(data3%16);
		data3=data3/16;
	}
	string[18]='-';

	uint16_t	byte=0;
	for (uint16_t index=20; index<37; index=index+2) {
		unsigned char	data=guid->Data4[byte];
		string[index+1]=SQLR_HexToChar(data%16);
		data=data/16;
		string[index]=SQLR_HexToChar(data%16);
		byte++;
	}

	// hang on to that string
	char	*data=NULL;
	if (stmt->inputbindstrings.getData(parameternumber,&data)) {
		stmt->inputbindstrings.removeData(parameternumber);
		delete[] data;
	}
	stmt->inputbindstrings.setData(parameternumber,string);

	// return the string
	return string;
}

static SQLRETURN SQLR_InputBindParameter(SQLHSTMT statementhandle,
					SQLUSMALLINT parameternumber,
					SQLSMALLINT valuetype,
					SQLULEN lengthprecision,
					SQLSMALLINT parameterscale,
					SQLPOINTER parametervalue,
					SQLLEN *strlen_or_ind) {
	debugFunction();

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	SQLRETURN	retval=SQL_SUCCESS;

	// convert parameternumber to a string
	char	*parametername=charstring::parseNumber(parameternumber);

	switch (valuetype) {
		case SQL_C_CHAR:
			debugPrintf("valuetype: SQL_C_CHAR\n");
			stmt->cur->inputBind(parametername,
					(const char *)parametervalue);
			break;
		case SQL_C_LONG:
			debugPrintf("valuetype: SQL_C_LONG\n");
			stmt->cur->inputBind(parametername,
					(int64_t)(*((long *)parametervalue)));
			break;
		case SQL_C_SHORT:
			debugPrintf("valuetype: SQL_C_SHORT\n");
			stmt->cur->inputBind(parametername,
					(int64_t)(*((short *)parametervalue)));
			break;
		case SQL_C_FLOAT:
			debugPrintf("valuetype: SQL_C_FLOAT\n");
			stmt->cur->inputBind(parametername,
					(float)(*((double *)parametervalue)),
					lengthprecision,
					parameterscale);
			break;
		case SQL_C_DOUBLE:
			debugPrintf("valuetype: SQL_C_DOUBLE\n");
			stmt->cur->inputBind(parametername,
					*((double *)parametervalue),
					lengthprecision,
					parameterscale);
			break;
		case SQL_C_NUMERIC:
			debugPrintf("valuetype: SQL_C_NUMERIC\n");
			stmt->cur->inputBind(parametername,
				SQLR_BuildNumeric(stmt,parameternumber,
					(SQL_NUMERIC_STRUCT *)parametervalue));
			break;
		case SQL_C_DATE:
		case SQL_C_TYPE_DATE:
			{
			debugPrintf("valuetype: SQL_C_DATE/SQL_C_TYPE_DATE\n");
			DATE_STRUCT	*ds=(DATE_STRUCT *)parametervalue;
			stmt->cur->inputBind(parametername,
						ds->year,ds->month,ds->day,
						0,0,0,0,NULL);
			}
			break;
		case SQL_C_TIME:
		case SQL_C_TYPE_TIME:
			{
			debugPrintf("valuetype: SQL_C_TIME/SQL_C_TYPE_TIME\n");
			TIME_STRUCT	*ts=(TIME_STRUCT *)parametervalue;
			stmt->cur->inputBind(parametername,
						0,0,0,
						ts->hour,ts->minute,ts->second,
						0,NULL);
			break;
			}
		case SQL_C_TIMESTAMP:
		case SQL_C_TYPE_TIMESTAMP:
			{
			debugPrintf("valuetype: "
				"SQL_C_TIMESTAMP/SQL_C_TYPE_TIMESTAMP\n");
			TIMESTAMP_STRUCT	*tss=
					(TIMESTAMP_STRUCT *)parametervalue;
			stmt->cur->inputBind(parametername,
					tss->year,tss->month,tss->day,
					tss->hour,tss->minute,tss->second,
					tss->fraction/10,NULL);
			break;
			}
		case SQL_C_INTERVAL_YEAR:
		case SQL_C_INTERVAL_MONTH:
		case SQL_C_INTERVAL_DAY:
		case SQL_C_INTERVAL_HOUR:
		case SQL_C_INTERVAL_MINUTE:
		case SQL_C_INTERVAL_SECOND:
		case SQL_C_INTERVAL_YEAR_TO_MONTH:
		case SQL_C_INTERVAL_DAY_TO_HOUR:
		case SQL_C_INTERVAL_DAY_TO_MINUTE:
		case SQL_C_INTERVAL_DAY_TO_SECOND:
		case SQL_C_INTERVAL_HOUR_TO_MINUTE:
		case SQL_C_INTERVAL_HOUR_TO_SECOND:
		case SQL_C_INTERVAL_MINUTE_TO_SECOND:
			debugPrintf("valuetype: SQL_C_INTERVAL_XXX\n");
			stmt->cur->inputBind(parametername,
				SQLR_BuildInterval(stmt,parameternumber,
					(SQL_INTERVAL_STRUCT *)parametervalue));
			break;
		//case SQL_C_VARBOOKMARK: (dup of SQL_C_BINARY)
		case SQL_C_BINARY:
			debugPrintf("valuetype: "
				"SQL_C_BINARY/SQL_C_VARBOOKMARK\n");
			stmt->cur->inputBindBlob(parametername,
					(const char *)parametervalue,
					lengthprecision);
			break;
		case SQL_C_BIT:
			debugPrintf("valuetype: SQL_C_BIT\n");
			stmt->cur->inputBind(parametername,
				(charstring::contains("YyTt",
					(const char *)parametervalue) ||
				charstring::toInteger(
					(const char *)parametervalue))?"1":"0");
			break;
		case SQL_C_SBIGINT:
			debugPrintf("valuetype: SQL_C_BIGINT\n");
			stmt->cur->inputBind(parametername,
				(int64_t)(*((int64_t *)parametervalue)));
			break;
		case SQL_C_UBIGINT:
			debugPrintf("valuetype: SQL_C_UBIGINT\n");
			stmt->cur->inputBind(parametername,
				(int64_t)(*((uint64_t *)parametervalue)));
			break;
		case SQL_C_SLONG:
			debugPrintf("valuetype: SQL_C_SLONG\n");
			stmt->cur->inputBind(parametername,
					(int64_t)(*((long *)parametervalue)));
			break;
		case SQL_C_SSHORT:
			debugPrintf("valuetype: SQL_C_SSHORT\n");
			stmt->cur->inputBind(parametername,
					(int64_t)(*((short *)parametervalue)));
			break;
		case SQL_C_TINYINT:
		case SQL_C_STINYINT:
			debugPrintf("valuetype: "
				"SQL_C_TINYINT/SQL_C_STINYINT\n");
			stmt->cur->inputBind(parametername,
					(int64_t)(*((char *)parametervalue)));
			break;
		//case SQL_C_BOOKMARK: (dup of SQL_C_ULONG)
		case SQL_C_ULONG:
			debugPrintf("valuetype: SQL_C_ULONG/SQL_C_BOOKMARK\n");
			stmt->cur->inputBind(parametername,
				(int64_t)(*((unsigned long *)parametervalue)));
			break;
		case SQL_C_USHORT:
			debugPrintf("valuetype: SQL_C_USHORT\n");
			stmt->cur->inputBind(parametername,
				(int64_t)(*((unsigned short *)parametervalue)));
			break;
		case SQL_C_UTINYINT:
			debugPrintf("valuetype: SQL_C_UTINYINT\n");
			stmt->cur->inputBind(parametername,
				(int64_t)(*((unsigned char *)parametervalue)));
			break;
		case SQL_C_GUID:
			{
			debugPrintf("valuetype: SQL_C_GUID\n");
			stmt->cur->inputBind(parametername,
				SQLR_BuildGuid(stmt,parameternumber,
						(SQLGUID *)parametervalue));
			}
			break;
		default:
			debugPrintf("invalid valuetype\n");
			retval=SQL_ERROR;
			break;
	}

	delete[] parametername;

	return retval;
}

static SQLRETURN SQLR_OutputBindParameter(SQLHSTMT statementhandle,
					SQLUSMALLINT parameternumber,
					SQLSMALLINT valuetype,
					SQLULEN lengthprecision,
					SQLSMALLINT parameterscale,
					SQLPOINTER parametervalue,
					SQLLEN bufferlength,
					SQLLEN *strlen_or_ind) {
	debugFunction();

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	SQLRETURN	retval=SQL_SUCCESS;

	// convert parameternumber to a string
	char	*parametername=charstring::parseNumber(parameternumber);

	// store the output bind for later
	outputbind	*ob=new outputbind;
	ob->parameternumber=parameternumber;
	ob->valuetype=valuetype;
	ob->lengthprecision=lengthprecision;
	ob->parameterscale=parameterscale;
	ob->parametervalue=parametervalue;
	ob->bufferlength=bufferlength;
	ob->strlen_or_ind=strlen_or_ind;
	stmt->outputbinds.setData(parameternumber,ob);

	switch (valuetype) {
		case SQL_C_CHAR:
		case SQL_C_BIT:
			debugPrintf("valuetype: SQL_C_CHAR/SQL_C_BIT\n");
			stmt->cur->defineOutputBindString(parametername,
								bufferlength);
			break;
		case SQL_C_LONG:
		case SQL_C_SBIGINT:
		case SQL_C_UBIGINT:
		case SQL_C_SHORT:
		case SQL_C_TINYINT:
		case SQL_C_SLONG:
		case SQL_C_SSHORT:
		case SQL_C_STINYINT:
		//case SQL_C_BOOKMARK: dup of SQL_C_ULONG
		case SQL_C_ULONG:
		case SQL_C_USHORT:
		case SQL_C_UTINYINT:
			debugPrintf("valuetype: SQL_C_(INT of some kind)\n");
			stmt->cur->defineOutputBindInteger(parametername);
			break;
		case SQL_C_FLOAT:
		case SQL_C_DOUBLE:
			debugPrintf("valuetype: SQL_C_FLOAT/SQL_C_DOUBLE\n");
			stmt->cur->defineOutputBindDouble(parametername);
			break;
		case SQL_C_NUMERIC:
			debugPrintf("valuetype: SQL_C_NUMERIC\n");
			// bind as a string, the result will be parsed
			stmt->cur->defineOutputBindString(parametername,128);
			break;
		case SQL_C_DATE:
		case SQL_C_TYPE_DATE:
			debugPrintf("valuetype: SQL_C_DATE/SQL_C_TYPE_DATE\n");
			stmt->cur->defineOutputBindDate(parametername);
			break;
		case SQL_C_TIME:
		case SQL_C_TYPE_TIME:
			debugPrintf("valuetype: SQL_C_TIME/SQL_C_TYPE_TIME\n");
			stmt->cur->defineOutputBindDate(parametername);
			break;
		case SQL_C_TIMESTAMP:
		case SQL_C_TYPE_TIMESTAMP:
			debugPrintf("valuetype: "
				"SQL_C_TIMESTAMP/SQL_C_TYPE_TIMESTAMP\n");
			stmt->cur->defineOutputBindDate(parametername);
			break;
		case SQL_C_INTERVAL_YEAR:
		case SQL_C_INTERVAL_MONTH:
		case SQL_C_INTERVAL_DAY:
		case SQL_C_INTERVAL_HOUR:
		case SQL_C_INTERVAL_MINUTE:
		case SQL_C_INTERVAL_SECOND:
		case SQL_C_INTERVAL_YEAR_TO_MONTH:
		case SQL_C_INTERVAL_DAY_TO_HOUR:
		case SQL_C_INTERVAL_DAY_TO_MINUTE:
		case SQL_C_INTERVAL_DAY_TO_SECOND:
		case SQL_C_INTERVAL_HOUR_TO_MINUTE:
		case SQL_C_INTERVAL_HOUR_TO_SECOND:
		case SQL_C_INTERVAL_MINUTE_TO_SECOND:
			debugPrintf("valuetype: SQL_C_INTERVAL_XXX\n");
			// bind as a string, the result will be parsed
			stmt->cur->defineOutputBindString(parametername,128);
			break;
		//case SQL_C_VARBOOKMARK: dup of SQL_C_BINARY:
		case SQL_C_BINARY:
			debugPrintf("valuetype: "
				"SQL_C_BINARY/SQL_C_VARBOOKMARK\n");
			stmt->cur->defineOutputBindBlob(parametername);
			break;
		case SQL_C_GUID:
			debugPrintf("valuetype: SQL_C_GUID\n");
			// bind as a string, the result will be parsed
			stmt->cur->defineOutputBindString(parametername,128);
			break;
		default:
			debugPrintf("invalid valuetype\n");
			retval=SQL_ERROR;
			break;
	}

	delete[] parametername;

	return retval;
}

static SQLRETURN SQLR_SQLBindParameter(SQLHSTMT statementhandle,
					SQLUSMALLINT parameternumber,
					SQLSMALLINT inputoutputtype,
					SQLSMALLINT valuetype,
					SQLSMALLINT parametertype,
					SQLULEN lengthprecision,
					SQLSMALLINT parameterscale,
					SQLPOINTER parametervalue,
					SQLLEN bufferlength,
					SQLLEN *strlen_or_ind) {
	debugFunction();

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	switch (inputoutputtype) {
		case SQL_PARAM_INPUT:
			debugPrintf("parametertype: SQL_PARAM_INPUT\n");
			return SQLR_InputBindParameter(statementhandle,
							parameternumber,
							valuetype,
							lengthprecision,
							parameterscale,
							parametervalue,
							strlen_or_ind);
		case SQL_PARAM_INPUT_OUTPUT:
			debugPrintf("parametertype: SQL_PARAM_INPUT_OUTPUT\n");
			// SQL Relay doesn't currently support in/out params
			return SQL_ERROR;
		case SQL_PARAM_OUTPUT:
			debugPrintf("parametertype: SQL_PARAM_OUTPUT\n");
			return SQLR_OutputBindParameter(statementhandle,
							parameternumber,
							valuetype,
							lengthprecision,
							parameterscale,
							parametervalue,
							bufferlength,
							strlen_or_ind);
		default:
			debugPrintf("invalid parametertype\n");
			return SQL_ERROR;
	}
}

SQLRETURN SQL_API SQLBindParameter(SQLHSTMT statementhandle,
					SQLUSMALLINT parameternumber,
					SQLSMALLINT inputoutputtype,
					SQLSMALLINT valuetype,
					SQLSMALLINT parametertype,
					SQLULEN lengthprecision,
					SQLSMALLINT parameterscale,
					SQLPOINTER parametervalue,
					SQLLEN bufferlength,
					SQLLEN *strlen_or_ind) {
	debugFunction();
	return SQLR_SQLBindParameter(statementhandle,
					parameternumber,
					inputoutputtype,
					valuetype,
					parametertype,
					lengthprecision,
					parameterscale,
					parametervalue,
					bufferlength,
					strlen_or_ind);
}

}
