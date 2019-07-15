// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <config.h>

#include <sqlrelay/sqlrclient.h>
#include <rudiments/bytestring.h>
#include <rudiments/linkedlist.h>
#include <rudiments/singlylinkedlist.h>
#include <rudiments/parameterstring.h>
#include <rudiments/charstring.h>
#include <rudiments/character.h>
#include <rudiments/environment.h>
#include <rudiments/stdio.h>
#include <rudiments/error.h>
/*#ifdef _WIN32
	#define DEBUG_MESSAGES 1
	#define DEBUG_TO_FILE 1
	#ifdef _WIN32
		static const char debugfile[]="C:\\Tmp\\sqlrodbcdebug.txt";
	#else
		static const char debugfile[]="/tmp/sqlrodbcdebug.txt";
	#endif
#endif*/
#include <rudiments/debugprint.h>

// windows needs this (don't include for __CYGWIN__ though)
#ifdef _WIN32
	#include <windows.h>
#endif

// older unixodbc needs this
#if !defined(_WIN32) && \
	defined(RUDIMENTS_HAVE_LONG_LONG) && \
	!defined(HAVE_LONG_LONG)
	#define HAVE_LONG_LONG 1
#endif

#include <sql.h>
#include <sqlext.h>
#include <odbcinst.h>

#include <parsedatetime.h>
#include <defines.h>

#ifndef SQL_NULL_DESC
	#define SQL_NULL_DESC 0
#endif

#ifdef _WIN64
	#undef SQLCOLATTRIBUTE_SQLLEN
	#define SQLCOLATTRIBUTE_SQLLEN 1
#endif

#ifdef _WIN32
	#ifdef _WIN64
	typedef SQLLEN * NUMERICATTRIBUTETYPE;
	#else
	typedef SQLPOINTER NUMERICATTRIBUTETYPE;
	#endif
#else
	#ifdef SQLCOLATTRIBUTE_SQLLEN
	typedef SQLLEN * NUMERICATTRIBUTETYPE;
	#else
	typedef SQLPOINTER NUMERICATTRIBUTETYPE;
	#endif
#endif

#ifndef HAVE_SQLROWSETSIZE
typedef SQLULEN SQLROWSETSIZE;
#endif

#define ODBC_INI "odbc.ini"

extern "C" {

static	uint16_t	stmtid=0;

struct CONN;

struct ENV {
	SQLINTEGER			odbcversion;
	singlylinkedlist<CONN *>	connlist;
	char				*error;
	int64_t				errn;
	const char			*sqlstate;
	SQLSMALLINT			sqlerrorindex;
};

struct STMT;

struct CONN {
	sqlrconnection			*con;
	ENV				*env;
	singlylinkedlist<STMT *>	stmtlist;
	char				*error;
	int64_t				errn;
	const char			*sqlstate;

	char				dsn[1024];

	char				server[1024];
	uint16_t			port;
	char				socket[1024];
	char				user[1024];
	char				password[1024];
	int32_t				retrytime;
	int32_t				tries;

	char				krb[4];
	char				krbservice[16];
	char				krbmech[128];
	char				krbflags[1024];

	char				tls[4];
	char				tlsversion[16];
	char				tlscert[1024];
	char				tlspassword[1024];
	char				tlsciphers[1024];
	char				tlsvalidate[1024];
	char				tlsca[1024];
	uint16_t			tlsdepth;

	char				debug[1024];

	char				columnnamecase[6];


	char				db[1024];
	uint64_t			resultsetbuffersize;
	bool				dontgetcolumninfo;
	bool				nullsasnulls;
	bool				lazyconnect;
	bool				clearbindsduringprepare;

	char				bindvariabledelimiters[5];

	bool				attrmetadataid;
	SQLSMALLINT			sqlerrorindex;
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
	void	print() {}
};

struct outputbind {
	SQLUSMALLINT	parameternumber;
	SQLSMALLINT	valuetype;
	SQLULEN		lengthprecision;
	SQLSMALLINT	parameterscale;
	SQLPOINTER	parametervalue;
	SQLLEN		bufferlength;
	SQLLEN		*strlen_or_ind;
	void	print() {}
};

struct STMT {
	sqlrcursor				*cur;
	uint64_t				currentfetchrow;
	uint64_t				currentstartrow;
	uint64_t				currentgetdatarow;
	CONN					*conn;
	char					*name;
	char					*error;
	int64_t					errn;
	const char				*sqlstate;
	dictionary<int32_t, FIELD *>		fieldlist;
	rowdesc					*approwdesc;
	paramdesc				*appparamdesc;
	rowdesc					*improwdesc;
	paramdesc				*impparamdesc;
	dictionary<int32_t, char *>		inputbindstrings;
	dictionary<int32_t,outputbind *>	outputbinds;
	dictionary<int32_t,outputbind *>	inputoutputbinds;
	SQLROWSETSIZE				*rowsfetchedptr;
	SQLUSMALLINT				*rowstatusptr;
	bool					executed;
	bool					executedbynumresultcols;
	SQLRETURN				executedbynumresultcolsresult;
	SQLULEN					rowbindtype;
	SQLSMALLINT				sqlerrorindex;
	bool					nodata;
	bool					dataatexec;
	dictionary<SQLUSMALLINT,SQLPOINTER>	dataatexecdict;
	linkedlist<SQLUSMALLINT>		*dataatexeckeys;
	SQLCHAR					*dataatexecstatement;
	SQLINTEGER				dataatexecstatementlength;
	SQLUSMALLINT				putdatabind;
	bytebuffer				putdatabuffer;
	uint64_t				rowsetsize;
	uint64_t				rowarraysize;
	uint64_t				*coloffsets;
	SQLULEN					*paramsprocessed;
	SQLULEN					*parambindoffsetptr;
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
	env->sqlerrorindex=1;
	debugPrintf("  error: %s\n",env->error);
	debugPrintf("  errn: %lld\n",env->errn);
	debugPrintf("  sqlstate: %s\n",env->sqlstate);
}

static void SQLR_ENVClearError(ENV *env) {
	debugFunction();
	SQLR_ENVSetError(env,NULL,0,"00000");
	env->sqlerrorindex=0;
}

static void SQLR_CONNSetError(CONN *conn, const char *error,
				int64_t errn, const char *sqlstate) {
	debugFunction();

	// set the error, convert NULL's to empty strings,
	// some apps have trouble with NULLS
	delete[] conn->error;
	conn->error=charstring::duplicate((error)?error:"");
	conn->errn=errn;
	conn->sqlstate=(sqlstate)?sqlstate:"";
	conn->sqlerrorindex=1;
	debugPrintf("  error: %s\n",conn->error);
	debugPrintf("  errn: %lld\n",conn->errn);
	debugPrintf("  sqlstate: %s\n",conn->sqlstate);
}

static void SQLR_CONNClearError(CONN *conn) {
	debugFunction();
	SQLR_CONNSetError(conn,NULL,0,"00000");
	conn->sqlerrorindex=0;
	SQLR_ENVClearError(conn->env);
}

static void SQLR_STMTSetError(STMT *stmt, const char *error,
				int64_t errn, const char *sqlstate) {
	debugFunction();

	// set the error, convert NULL's to empty strings,
	// some apps have trouble with NULLS
	delete[] stmt->error;
	stmt->error=charstring::duplicate((error)?error:"");
	stmt->errn=errn;
	stmt->sqlstate=(sqlstate)?sqlstate:"";
	stmt->sqlerrorindex=1;
	debugPrintf("  error: %s\n",stmt->error);
	debugPrintf("  errn: %lld\n",stmt->errn);
	debugPrintf("  sqlstate: %s\n",stmt->sqlstate);
}

static void SQLR_STMTClearError(STMT *stmt) {
	debugFunction();
	SQLR_STMTSetError(stmt,NULL,0,"00000");
	stmt->sqlerrorindex=0;
	SQLR_CONNClearError(stmt->conn);
}

static SQLRETURN SQLR_SQLAllocHandle(SQLSMALLINT handletype,
					SQLHANDLE inputhandle,
					SQLHANDLE *outputhandle) {
	debugFunction();

	switch (handletype) {
		case SQL_HANDLE_ENV:
			{
			debugPrintf("  handletype: SQL_HANDLE_ENV\n");
			if (outputhandle) {
				ENV	*env=new ENV;
				#if (ODBCVER >= 0x0300)
					env->odbcversion=SQL_OV_ODBC3;
				#else
					env->odbcversion=SQL_OV_ODBC2;
				#endif
				*outputhandle=(SQLHANDLE)env;
				env->error=NULL;
				SQLR_ENVClearError(env);
			}
			return SQL_SUCCESS;
			}
		case SQL_HANDLE_DBC:
			{
			debugPrintf("  handletype: SQL_HANDLE_DBC\n");
			ENV	*env=(ENV *)inputhandle;
			if (inputhandle==SQL_NULL_HENV || !env) {
				debugPrintf("  NULL env handle\n");
				if (outputhandle) {
					*outputhandle=SQL_NULL_HENV;
				}
				return SQL_INVALID_HANDLE;
			}
			if (outputhandle) {
				CONN	*conn=new CONN;
				conn->con=NULL;
				*outputhandle=(SQLHANDLE)conn;
				conn->env=env;
				conn->error=NULL;
				SQLR_CONNClearError(conn);
				env->connlist.append(conn);
				conn->attrmetadataid=false;
			}
			return SQL_SUCCESS;
			}
		case SQL_HANDLE_STMT:
			{
			debugPrintf("  handletype: SQL_HANDLE_STMT\n");
			CONN	*conn=(CONN *)inputhandle;
			if (inputhandle==SQL_NULL_HANDLE ||
						!conn || !conn->con) {
				debugPrintf("  NULL conn handle\n");
				*outputhandle=SQL_NULL_HENV;
				return SQL_INVALID_HANDLE;
			}
			if (outputhandle) {
				STMT	*stmt=new STMT;
				stmt->cur=new sqlrcursor(conn->con,true);
				*outputhandle=(SQLHANDLE)stmt;
				stmt->currentfetchrow=0;
				stmt->currentstartrow=0;
				stmt->currentgetdatarow=0;
				stmt->conn=conn;
				conn->stmtlist.append(stmt);
				stmt->name=NULL;
				stmt->error=NULL;
				SQLR_STMTClearError(stmt);
				stmt->improwdesc=new rowdesc;
				stmt->improwdesc->stmt=stmt;
				stmt->impparamdesc=new paramdesc;
				stmt->impparamdesc->stmt=stmt;
				stmt->approwdesc=stmt->improwdesc;
				stmt->appparamdesc=stmt->impparamdesc;
				stmt->rowsfetchedptr=NULL;
				stmt->rowstatusptr=NULL;
				stmt->executed=false;
				stmt->executedbynumresultcols=false;
				stmt->executedbynumresultcolsresult=SQL_SUCCESS;
				stmt->rowbindtype=SQL_BIND_BY_COLUMN;
				stmt->nodata=false;
				stmt->dataatexec=false;
				stmt->dataatexecdict.
					setTrackInsertionOrder(true);
				stmt->dataatexeckeys=NULL;
				stmt->dataatexecstatement=NULL;
				stmt->dataatexecstatementlength=0;
				stmt->putdatabind=0;
				stmt->putdatabuffer.clear();
				stmt->rowsetsize=conn->resultsetbuffersize;
				stmt->rowarraysize=conn->resultsetbuffersize;
				stmt->coloffsets=NULL;
				stmt->paramsprocessed=NULL;
				stmt->parambindoffsetptr=NULL;

				// set flags
				if (!charstring::compare(
					conn->columnnamecase,"upper")) {
					stmt->cur->upperCaseColumnNames();
				} else if (!charstring::compare(
					conn->columnnamecase,"lower")) {
					stmt->cur->lowerCaseColumnNames();
				}
				stmt->cur->lazyFetch();
				if (conn->clearbindsduringprepare) {
					stmt->cur->
						clearBindsDuringPrepare();
				} else {
					stmt->cur->
						dontClearBindsDuringPrepare();
				}
				if (conn->dontgetcolumninfo) {
					stmt->cur->dontGetColumnInfo();
				} else {
					stmt->cur->getColumnInfo();
				}
				if (conn->nullsasnulls) {
					stmt->cur->getNullsAsNulls();
				} else {
					stmt->cur->getNullsAsEmptyStrings();
				}
			}
			return SQL_SUCCESS;
			}
		case SQL_HANDLE_DESC:
			debugPrintf("  handletype: SQL_HANDLE_DESC\n");
			// FIXME: no idea what to do here
			return SQL_ERROR;
		default:
			debugPrintf("  invalid handletype: %d\n",handletype);
			break;
	}
	return SQL_ERROR;
}

SQLRETURN SQL_API SQLAllocHandleStd(SQLSMALLINT handletype,
					SQLHANDLE inputhandle,
					SQLHANDLE *outputhandle) {
	debugFunction();
	SQLRETURN	retval=SQLR_SQLAllocHandle(handletype,
							inputhandle,
							outputhandle);
	if (retval==SQL_SUCCESS && handletype==SQL_HANDLE_ENV) {
		#if (ODBCVER >= 0x0300)
		((ENV *)inputhandle)->odbcversion=SQL_OV_ODBC3;
		#endif
	}
	return retval;
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

static SQLLEN SQLR_GetCColumnTypeSize(SQLSMALLINT targettype) {
	switch (targettype) {
		case SQL_C_BIT:
			return sizeof(unsigned char);
		case SQL_C_SHORT:
		case SQL_C_SSHORT:
			return sizeof(SQLSMALLINT);
		case SQL_C_USHORT:
			return sizeof(SQLUSMALLINT);
		case SQL_C_TINYINT:
		case SQL_C_STINYINT:
			return sizeof(SQLCHAR);
		case SQL_C_UTINYINT:
			return sizeof(unsigned char);
		case SQL_C_LONG:
		case SQL_C_SLONG:
			return sizeof(SQLINTEGER);
		case SQL_C_ULONG:
			return sizeof(SQLUINTEGER);
		case SQL_C_SBIGINT:
			return sizeof(SQLBIGINT);
		case SQL_C_UBIGINT:
			return sizeof(SQLUBIGINT);
		case SQL_C_FLOAT:
			return sizeof(SQLREAL);
		case SQL_C_DOUBLE:
			return sizeof(SQLDOUBLE);
		case SQL_C_NUMERIC:
			return sizeof(SQL_NUMERIC_STRUCT);
		case SQL_C_DATE:
		case SQL_C_TYPE_DATE:
			return sizeof(DATE_STRUCT);
		case SQL_C_TIME:
		case SQL_C_TYPE_TIME:
			return sizeof(TIME_STRUCT);
		case SQL_C_TIMESTAMP:
		case SQL_C_TYPE_TIMESTAMP:
			return sizeof(TIMESTAMP_STRUCT);
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
			return sizeof(SQL_INTERVAL_STRUCT);
		case SQL_C_GUID:
			return 36;
		default:
			return 0;
	}
}

#ifdef DEBUG_MESSAGES
static const char *SQLR_GetCColumnTypeName(SQLSMALLINT targettype) {
	switch (targettype) {
		case SQL_C_CHAR:
			return "SQL_C_CHAR";
		case SQL_C_BIT:
			return "SQL_C_BIT";
		case SQL_C_SHORT:
			return "SQL_C_SHORT";
		case SQL_C_USHORT:
			return "SQL_C_USHORT";
		case SQL_C_SSHORT:
			return "SQL_C_SSHORT";
		case SQL_C_TINYINT:
			return "SQL_C_TINYINT";
		case SQL_C_UTINYINT:
			return "SQL_C_UTINYINT";
		case SQL_C_STINYINT:
			return "SQL_C_STINYINT";
		case SQL_C_LONG:
			return "SQL_C_LONG";
		case SQL_C_ULONG:
			return "SQL_C_ULONG";
		case SQL_C_SLONG:
			return "SQL_C_SLONG";
		case SQL_C_SBIGINT:
			return "SQL_C_SBIGINT";
		case SQL_C_UBIGINT:
			return "SQL_C_UBIGINT";
		case SQL_C_FLOAT:
			return "SQL_C_FLOAT";
		case SQL_C_DOUBLE:
			return "SQL_C_DOUBLE";
		case SQL_C_NUMERIC:
			return "SQL_C_NUMERIC";
		case SQL_C_DATE:
			return "SQL_C_DATE";
		case SQL_C_TYPE_DATE:
			return "SQL_C_TYPE_DATE";
		case SQL_C_TIME:
			return "SQL_C_TIME";
		case SQL_C_TYPE_TIME:
			return "SQL_C_TYPE_TIME";
		case SQL_C_TIMESTAMP:
			return "SQL_C_TIMESTAMP";
		case SQL_C_TYPE_TIMESTAMP:
			return "SQL_C_TYPE_TIMESTAMP";
		case SQL_C_INTERVAL_YEAR:
			return "SQL_C_INTERVAL_YEAR";
		case SQL_C_INTERVAL_MONTH:
			return "SQL_C_INTERVAL_MONTH";
		case SQL_C_INTERVAL_DAY:
			return "SQL_C_INTERVAL_DAY";
		case SQL_C_INTERVAL_HOUR:
			return "SQL_C_INTERVAL_HOUR";
		case SQL_C_INTERVAL_MINUTE:
			return "SQL_C_INTERVAL_MINUTE";
		case SQL_C_INTERVAL_SECOND:
			return "SQL_C_INTERVAL_SECOND";
		case SQL_C_INTERVAL_YEAR_TO_MONTH:
			return "SQL_C_INTERVAL_YEAR_TO_MONTH";
		case SQL_C_INTERVAL_DAY_TO_HOUR:
			return "SQL_C_INTERVAL_DAY_TO_HOUR";
		case SQL_C_INTERVAL_DAY_TO_MINUTE:
			return "SQL_C_INTERVAL_DAY_TO_MINUTE";
		case SQL_C_INTERVAL_DAY_TO_SECOND:
			return "SQL_C_INTERVAL_DAY_TO_SECOND";
		case SQL_C_INTERVAL_HOUR_TO_MINUTE:
			return "SQL_C_INTERVAL_HOUR_TO_MINUTE";
		case SQL_C_INTERVAL_HOUR_TO_SECOND:
			return "SQL_C_INTERVAL_HOUR_TO_SECOND";
		case SQL_C_INTERVAL_MINUTE_TO_SECOND:
			return "SQL_C_INTERVAL_MINUTE_TO_SECOND";
		case SQL_C_GUID:
			return "SQL_C_GUID";
		default:
			return "unknown";
	}
}
#endif

SQLRETURN SQL_API SQLBindCol(SQLHSTMT statementhandle,
					SQLUSMALLINT columnnumber,
					SQLSMALLINT targettype,
					SQLPOINTER targetvalue,
					SQLLEN bufferlength,
					SQLLEN *strlen_or_ind) {
	debugFunction();
	debugPrintf("  columnnumber: %d\n",
				(int)columnnumber);
	debugPrintf("  targettype  : %s\n",
				SQLR_GetCColumnTypeName(targettype));

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("  NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	if (columnnumber<1) {
		debugPrintf("  invalid column: %d\n",columnnumber);
		SQLR_STMTSetError(stmt,NULL,0,"07009");
		return SQL_ERROR;
	}

	debugPrintf("  binding column\n");

	FIELD	*field=new FIELD;
	field->targettype=targettype;
	field->targetvalue=targetvalue;
	if (bufferlength) {
		field->bufferlength=bufferlength;
		debugPrintf("  bufferlength (supplied) : %lld\n",
					(uint64_t)bufferlength);
	} else {
		field->bufferlength=SQLR_GetCColumnTypeSize(targettype);
		debugPrintf("  bufferlength (from type): %lld\n",
					(uint64_t)field->bufferlength);
	}
	field->strlen_or_ind=strlen_or_ind;

	stmt->fieldlist.setValue(columnnumber-1,field);

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

static SQLRETURN SQLR_SQLCancelHandle(SQLSMALLINT handletype,
						SQLHANDLE handle) {
	debugFunction();

	if (handletype==SQL_HANDLE_ENV) {
		ENV	*env=(ENV *)handle;
		if (handle==SQL_NULL_HENV || !env) {
			debugPrintf("  NULL env handle\n");
			return SQL_INVALID_HANDLE;
		}
		SQLR_ENVSetError(env,
			"Invalid attribute/option identifier",0,"HY092");
	} else if (handletype==SQL_HANDLE_DBC) {
		CONN	*conn=(CONN *)handle;
		if (handle==SQL_NULL_HANDLE || !conn || !conn->con) {
			debugPrintf("  NULL conn handle\n");
			return SQL_INVALID_HANDLE;
		}
		SQLR_CONNSetError(conn,
			"Driver does not support this function",0,"IM001");
	} else if (handletype==SQL_HANDLE_STMT) {
		STMT	*stmt=(STMT *)handle;
		if (handle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
			debugPrintf("  NULL stmt handle\n");
			return SQL_INVALID_HANDLE;
		}
		SQLR_STMTSetError(stmt,
			"Driver does not support this function",0,"IM001");
	}
	return SQL_ERROR;
}

SQLRETURN SQL_API SQLCancel(SQLHSTMT statementhandle) {
	debugFunction();
	return SQLR_SQLCancelHandle(SQL_HANDLE_STMT,(SQLHANDLE)statementhandle);
}

SQLRETURN SQL_API SQLCancelHandle(SQLSMALLINT handletype, SQLHANDLE handle) {
	debugFunction();
	return SQLR_SQLCancelHandle(handletype,handle);
}

static void SQLR_ResetParams(STMT *stmt) {
	debugFunction();

	stmt->cur->clearBinds();
	stmt->inputbindstrings.clearAndArrayDeleteValues();
	stmt->outputbinds.clearAndDeleteValues();
	stmt->inputoutputbinds.clearAndDeleteValues();
}

static SQLRETURN SQLR_SQLCloseCursor(SQLHSTMT statementhandle) {
	debugFunction();

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("  NULL stmt handle\n");
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
	}
	if (!charstring::compare(ctype,"CHAR")) {
		return SQL_CHAR;
	}
	if (!charstring::compare(ctype,"INT")) {
		return SQL_INTEGER;
	}
	if (!charstring::compare(ctype,"SMALLINT")) {
		return SQL_SMALLINT;
	}
	if (!charstring::compare(ctype,"TINYINT")) {
		return SQL_TINYINT;
	}
	if (!charstring::compare(ctype,"MONEY")) {
		return SQL_CHAR;
	}
	if (!charstring::compare(ctype,"DATETIME")) {
		// FIXME: need parameter indicating whether
		// to map this to SQL_DATE or SQL_TIMESTAMP.
		// MySQL, for example, may use DATE for dates and
		// TIMESTAMP for datetimes.
		return SQL_TIMESTAMP;
	}
	if (!charstring::compare(ctype,"NUMERIC")) {
		return SQL_NUMERIC;
	}
	if (!charstring::compare(ctype,"DECIMAL")) {
		return SQL_DECIMAL;
	}
	if (!charstring::compare(ctype,"SMALLDATETIME")) {
		return SQL_TIMESTAMP;
	}
	if (!charstring::compare(ctype,"SMALLMONEY")) {
		return SQL_CHAR;
	}
	if (!charstring::compare(ctype,"IMAGE")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"BINARY")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"BIT")) {
		return SQL_BIT;
	}
	if (!charstring::compare(ctype,"REAL")) {
		return SQL_REAL;
	}
	if (!charstring::compare(ctype,"FLOAT")) {
		return SQL_FLOAT;
	}
	if (!charstring::compare(ctype,"TEXT")) {
		return SQL_CHAR;
	}
	if (!charstring::compare(ctype,"VARCHAR")) {
		return SQL_VARCHAR;
	}
	if (!charstring::compare(ctype,"VARBINARY")) {
		return SQL_VARBINARY;
	}
	if (!charstring::compare(ctype,"LONGCHAR")) {
		return SQL_CHAR;
	}
	if (!charstring::compare(ctype,"LONGBINARY")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"LONG")) {
		return SQL_CHAR;
	}
	if (!charstring::compare(ctype,"ILLEGAL")) {
		return SQL_CHAR;
	}
	if (!charstring::compare(ctype,"SENSITIVITY")) {
		return SQL_CHAR;
	}
	if (!charstring::compare(ctype,"BOUNDARY")) {
		return SQL_CHAR;
	}
	if (!charstring::compare(ctype,"VOID")) {
		return SQL_CHAR;
	}
	if (!charstring::compare(ctype,"USHORT")) {
		return SQL_SMALLINT;
	}

	// added by lago
	if (!charstring::compare(ctype,"UNDEFINED")) {
		return SQL_UNKNOWN_TYPE;
	}
	if (!charstring::compare(ctype,"DOUBLE")) {
		return SQL_DOUBLE;
	}
	if (!charstring::compare(ctype,"DATE")) {
		// FIXME: optionally map to SQL_TIMESTAMP?
		return SQL_DATE;
	}
	if (!charstring::compare(ctype,"TIME")) {
		return SQL_TIME;
	}
	if (!charstring::compare(ctype,"TIMESTAMP")) {
		return SQL_TIMESTAMP;
	}

	// added by msql
	if (!charstring::compare(ctype,"UINT")) {
		return SQL_INTEGER;
	}
	if (!charstring::compare(ctype,"LASTREAL")) {
		return SQL_REAL;
	}

	// added by mysql
	if (!charstring::compare(ctype,"STRING")) {
		return SQL_CHAR;
	}
	if (!charstring::compare(ctype,"VARSTRING")) {
		return SQL_VARCHAR;
	}
	if (!charstring::compare(ctype,"LONGLONG")) {
		return SQL_BIGINT;
	}
	if (!charstring::compare(ctype,"MEDIUMINT")) {
		return SQL_INTEGER;
	}
	if (!charstring::compare(ctype,"YEAR")) {
		return SQL_SMALLINT;
	}
	if (!charstring::compare(ctype,"NEWDATE")) {
		// FIXME: optionally map to SQL_TIMESTAMP?
		return SQL_DATE;
	}
	if (!charstring::compare(ctype,"NULL")) {
		return SQL_CHAR;
	}
	if (!charstring::compare(ctype,"ENUM")) {
		return SQL_CHAR;
	}
	if (!charstring::compare(ctype,"SET")) {
		return SQL_CHAR;
	}
	if (!charstring::compare(ctype,"TINYBLOB")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"MEDIUMBLOB")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"LONGBLOB")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"BLOB")) {
		return SQL_BINARY;
	}

	// added by oracle
	if (!charstring::compare(ctype,"VARCHAR2")) {
		return SQL_VARCHAR;
	}
	if (!charstring::compare(ctype,"NUMBER")) {
		return SQL_NUMERIC;
	}
	if (!charstring::compare(ctype,"ROWID")) {
		return SQL_BIGINT;
	}
	if (!charstring::compare(ctype,"RAW")) {
		return SQL_VARBINARY;
	}
	if (!charstring::compare(ctype,"LONG_RAW")) {
		return SQL_LONGVARBINARY;
	}
	if (!charstring::compare(ctype,"MLSLABEL")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"CLOB")) {
		return SQL_LONGVARCHAR;
	}
	if (!charstring::compare(ctype,"BFILE")) {
		return SQL_LONGVARBINARY;
	}

	// added by odbc
	if (!charstring::compare(ctype,"BIGINT")) {
		return SQL_BIGINT;
	}
	if (!charstring::compare(ctype,"INTEGER")) {
		return SQL_INTEGER;
	}
	if (!charstring::compare(ctype,"LONGVARBINARY")) {
		return SQL_LONGVARBINARY;
	}
	if (!charstring::compare(ctype,"LONGVARCHAR")) {
		return SQL_LONGVARCHAR;
	}

	// added by db2
	if (!charstring::compare(ctype,"GRAPHIC")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"VARGRAPHIC")) {
		return SQL_VARBINARY;
	}
	if (!charstring::compare(ctype,"LONGVARGRAPHIC")) {
		return SQL_LONGVARBINARY;
	}
	if (!charstring::compare(ctype,"DBCLOB")) {
		return SQL_LONGVARCHAR;
	}
	if (!charstring::compare(ctype,"DATALINK")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"USER_DEFINED_TYPE")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"SHORT_DATATYPE")) {
		return SQL_SMALLINT;
	}
	if (!charstring::compare(ctype,"TINY_DATATYPE")) {
		return SQL_TINYINT;
	}

	// added by firebird
	if (!charstring::compare(ctype,"D_FLOAT")) {
		return SQL_DOUBLE;
	}
	if (!charstring::compare(ctype,"ARRAY")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"QUAD")) {
		return SQL_BIGINT;
	}
	if (!charstring::compare(ctype,"INT64")) {
		return SQL_BIGINT;
	}
	if (!charstring::compare(ctype,"DOUBLE PRECISION")) {
		return SQL_DOUBLE;
	}

	// added by postgresql
	if (!charstring::compare(ctype,"BOOL")) {
		return SQL_CHAR;
	}
	if (!charstring::compare(ctype,"BYTEA")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"NAME")) {
		return SQL_CHAR;
	}
	if (!charstring::compare(ctype,"INT8")) {
		return SQL_BIGINT;
	}
	if (!charstring::compare(ctype,"INT2")) {
		return SQL_SMALLINT;
	}
	if (!charstring::compare(ctype,"INT2VECTOR")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"INT4")) {
		return SQL_INTEGER;
	}
	if (!charstring::compare(ctype,"REGPROC")) {
		return SQL_BIGINT;
	}
	if (!charstring::compare(ctype,"OID")) {
		return SQL_BIGINT;
	}
	if (!charstring::compare(ctype,"TID")) {
		return SQL_BIGINT;
	}
	if (!charstring::compare(ctype,"XID")) {
		return SQL_BIGINT;
	}
	if (!charstring::compare(ctype,"CID")) {
		return SQL_BIGINT;
	}
	if (!charstring::compare(ctype,"OIDVECTOR")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"SMGR")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"POINT")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"LSEG")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"PATH")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"BOX")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"POLYGON")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"LINE")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"LINE_ARRAY")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"FLOAT4")) {
		return SQL_FLOAT;
	}
	if (!charstring::compare(ctype,"FLOAT8")) {
		return SQL_DOUBLE;
	}
	if (!charstring::compare(ctype,"ABSTIME")) {
		return SQL_INTEGER;
	}
	if (!charstring::compare(ctype,"RELTIME")) {
		return SQL_INTEGER;
	}
	if (!charstring::compare(ctype,"TINTERVAL")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"CIRCLE")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"CIRCLE_ARRAY")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"MONEY_ARRAY")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"MACADDR")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"INET")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"CIDR")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"BOOL_ARRAY")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"BYTEA_ARRAY")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"CHAR_ARRAY")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"NAME_ARRAY")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"INT2_ARRAY")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"INT2VECTOR_ARRAY")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"INT4_ARRAY")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"REGPROC_ARRAY")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"TEXT_ARRAY")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"OID_ARRAY")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"TID_ARRAY")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"XID_ARRAY")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"CID_ARRAY")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"OIDVECTOR_ARRAY")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"BPCHAR_ARRAY")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"VARCHAR_ARRAY")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"INT8_ARRAY")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"POINT_ARRAY")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"LSEG_ARRAY")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"PATH_ARRAY")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"BOX_ARRAY")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"FLOAT4_ARRAY")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"FLOAT8_ARRAY")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"ABSTIME_ARRAY")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"RELTIME_ARRAY")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"TINTERVAL_ARRAY")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"POLYGON_ARRAY")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"ACLITEM")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"ACLITEM_ARRAY")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"MACADDR_ARRAY")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"INET_ARRAY")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"CIDR_ARRAY")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"BPCHAR")) {
		return SQL_CHAR;
	}
	if (!charstring::compare(ctype,"TIMESTAMP_ARRAY")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"DATE_ARRAY")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"TIME_ARRAY")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"TIMESTAMPTZ")) {
		return SQL_TIMESTAMP;
	}
	if (!charstring::compare(ctype,"TIMESTAMPTZ_ARRAY")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"INTERVAL")) {
		return SQL_INTERVAL;
	}
	if (!charstring::compare(ctype,"INTERVAL_ARRAY")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"NUMERIC_ARRAY")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"TIMETZ")) {
		return SQL_TIME;
	}
	if (!charstring::compare(ctype,"TIMETZ_ARRAY")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"BIT_ARRAY")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"VARBIT")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"VARBIT_ARRAY")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"REFCURSOR")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"REFCURSOR_ARRAY")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"REGPROCEDURE")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"REGOPER")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"REGOPERATOR")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"REGCLASS")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"REGTYPE")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"REGPROCEDURE_ARRAY")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"REGOPER_ARRAY")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"REGOPERATOR_ARRAY")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"REGCLASS_ARRAY")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"REGTYPE_ARRAY")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"RECORD")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"CSTRING")) {
		return SQL_CHAR;
	}
	if (!charstring::compare(ctype,"ANY")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"ANYARRAY")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"TRIGGER")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"LANGUAGE_HANDLER")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"INTERNAL")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"OPAQUE")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"ANYELEMENT")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"PG_TYPE")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"PG_ATTRIBUTE")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"PG_PROC")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"PG_CLASS")) {
		return SQL_BINARY;
	}
	// none added by sqlite
	// added by sqlserver
	if (!charstring::compare(ctype,"UBIGINT")) {
		return SQL_BIGINT;
	}
	if (!charstring::compare(ctype,"UNIQUEIDENTIFIER")) {
		return SQL_BINARY;
	}
	// added by informix
	if (!charstring::compare(ctype,"SMALLFLOAT")) {
		return SQL_FLOAT;
	}
	if (!charstring::compare(ctype,"BYTE")) {
		return SQL_BINARY;
	}
	if (!charstring::compare(ctype,"BOOLEAN")) {
		return SQL_CHAR;
	}
	// also added by mysql
	if (!charstring::compare(ctype,"TINYTEXT")) {
		return SQL_LONGVARCHAR;
	}
	if (!charstring::compare(ctype,"MEDIUMTEXT")) {
		return SQL_LONGVARCHAR;
	}
	if (!charstring::compare(ctype,"LONGTEXT")) {
		return SQL_LONGVARCHAR;
	}
	if (!charstring::compare(ctype,"JSON")) {
		return SQL_LONGVARCHAR;
	}
	if (!charstring::compare(ctype,"GEOMETRY")) {
		return SQL_BINARY;
	}
	// also added by oracle
	if (!charstring::compare(ctype,"SDO_GEOMETRY")) {
		return SQL_BINARY;
	}
	// added by mssql
	if (!charstring::compare(ctype,"NCHAR")) {
		#ifdef SQL_WCHAR
			return SQL_WCHAR;
		#else
			return SQL_CHAR;
		#endif
	}
	if (!charstring::compare(ctype,"NVARCHAR")) {
		#ifdef SQL_WVARCHAR
			return SQL_WVARCHAR;
		#else
			return SQL_VARCHAR;
		#endif
	}
	if (!charstring::compare(ctype,"NTEXT")) {
		#ifdef SQL_WLONGVARCHAR
			return SQL_WLONGVARCHAR;
		#else
			return SQL_LONGVARCHAR;
		#endif
	}
	if (!charstring::compare(ctype,"XML")) {
		return SQL_LONGVARCHAR;
	}
	if (!charstring::compare(ctype,"DATETIMEOFFSET")) {
		return SQL_TIMESTAMP;
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
		case SQL_DATE:
		// case SQL_DATETIME:
		// 	(ODBC 3 dup of SQL_DATE)
			// FIXME: need parameter indicating whether
			// to map this to SQL_C_DATE or SQL_C_TIMESTAMP.
			// MySQL, for example, may use DATE for dates and
			// TIMESTAMP for datetimes.
			return SQL_C_TIMESTAMP;
		case SQL_VARCHAR:
			return SQL_C_CHAR;
		case SQL_TYPE_DATE:
			return SQL_C_DATE;
		case SQL_TYPE_TIME:
			return SQL_C_TIME;
		case SQL_TYPE_TIMESTAMP:
			return SQL_C_TIMESTAMP;
		case SQL_TIME:
		// case SQL_INTERVAL:
		// 	(ODBC 3 dup of SQL_TIME)
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

static SQLULEN SQLR_GetColumnSize(sqlrcursor *cur, uint32_t col) {
	switch (SQLR_MapColumnType(cur,col)) {
		case SQL_UNKNOWN_TYPE:
		case SQL_CHAR:
		case SQL_NUMERIC:
		case SQL_DECIMAL:
		case SQL_VARCHAR:
		case SQL_LONGVARCHAR:
		case SQL_BINARY:
		case SQL_VARBINARY:
		case SQL_LONGVARBINARY:
		#ifdef SQL_WCHAR
		case SQL_WCHAR:
		#endif
		#ifdef SQL_WVARCHAR
		case SQL_WVARCHAR:
		#endif
		#ifdef SQL_WLONGVARCHAR
		case SQL_WLONGVARCHAR:
		#endif
			{
			// FIXME: this really ought to be sorted out in the
			// connection code, rather than here.
			uint32_t	precision=cur->getColumnPrecision(col);
			uint32_t	length=cur->getColumnLength(col);
			uint32_t	size=(length>precision)?
							length:precision;
			// FIXME: is there a better fallback value?
			return (size)?size:32768;
			}
		case SQL_INTEGER:
			return 10;
		case SQL_SMALLINT:
			return 5;
		case SQL_FLOAT:
			return 15;
		case SQL_REAL:
			return 7;
		case SQL_DOUBLE:
			return 15;
		case SQL_DATE:
		// case SQL_DATETIME:
		// 	(ODBC 3 dup of SQL_DATE)
			// FIXME: need parameter indicating whether
			// to map this to the length of SQL_C_DATE or
			// SQL_C_TIMESTAMP.  MySQL, for example, may use DATE
			// for dates and TIMESTAMP for datetimes.
			return 25;
		case SQL_TYPE_DATE:
			return 10;
		case SQL_TYPE_TIME:
			return 8;
		case SQL_TYPE_TIMESTAMP:
			return 25;
		case SQL_TIME:
		// case SQL_INTERVAL:
		// 	(ODBC 3 dup of SQL_TIME)
			return 25;
		case SQL_TIMESTAMP:
			return 25;
		case SQL_BIGINT:
			return 20;
		case SQL_TINYINT:
			return 3;
		case SQL_BIT:
			return 1;
		case SQL_GUID:
			return 36;
	}
	return 0;
}

static SQLRETURN SQLR_SQLColAttribute(SQLHSTMT statementhandle,
					SQLUSMALLINT columnnumber,
					SQLUSMALLINT fieldidentifier,
					SQLPOINTER characterattribute,
					SQLSMALLINT bufferlength,
					SQLSMALLINT *stringlength,
					NUMERICATTRIBUTETYPE numericattribute) {
	debugFunction();
	debugPrintf("  columnnumber: %d\n",(int)columnnumber);
	debugPrintf("  bufferlength: %d\n",(int)bufferlength);

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("  NULL stmt handle\n");
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
	if (columnnumber<1 || columnnumber>colcount) {
		debugPrintf("  invalid column: %d\n",columnnumber);
		SQLR_STMTSetError(stmt,NULL,0,"07009");
		return SQL_ERROR;
	}

	// get a zero-based version of the columnnumber
	uint32_t	col=columnnumber-1;

	switch (fieldidentifier) {
		case SQL_DESC_COUNT:
		case SQL_COLUMN_COUNT:
			debugPrintf("  fieldidentifier: "
					"SQL_DESC/COLUMN_COUNT\n");
			*(SQLSMALLINT *)numericattribute=colcount;
			debugPrintf("  count: %lld\n",
				(int64_t)*(SQLSMALLINT *)numericattribute);
			break;
		case SQL_DESC_TYPE:
		//case SQL_DESC_CONCISE_TYPE:
		//	(dup of SQL_COLUMN_TYPE)
		case SQL_COLUMN_TYPE:
			debugPrintf("  fieldidentifier: "
					"SQL_DESC_TYPE/"
					"SQL_DESC_CONCISE_TYPE/"
					"COLUMN_TYPE\n");
			*(SQLSMALLINT *)numericattribute=
					SQLR_MapColumnType(stmt->cur,col);
			debugPrintf("  type: %lld\n",
				(int64_t)*(SQLSMALLINT *)numericattribute);
			break;
		case SQL_DESC_LENGTH:
		case SQL_DESC_OCTET_LENGTH:
		case SQL_COLUMN_LENGTH:
			debugPrintf("  fieldidentifier: "
					"SQL_DESC_LENGTH/COLUMN_LENGTH/"
					"SQL_DESC_OCTET_LENGTH\n");
			*(SQLINTEGER *)numericattribute=
					SQLR_GetColumnSize(stmt->cur,col);
			debugPrintf("  length: %lld\n",
				(int64_t)*(SQLSMALLINT *)numericattribute);
			break;
		case SQL_DESC_PRECISION:
		case SQL_COLUMN_PRECISION:
			debugPrintf("  fieldidentifier: "
					"SQL_DESC/COLUMN_PRECISION\n");
			*(SQLSMALLINT *)numericattribute=
					stmt->cur->getColumnPrecision(col);
			debugPrintf("  precision: %lld\n",
				(int64_t)*(SQLSMALLINT *)numericattribute);
			break;
		case SQL_DESC_SCALE:
		case SQL_COLUMN_SCALE:
			debugPrintf("  fieldidentifier: "
					"SQL_DESC/COLUMN_SCALE\n");
			*(SQLSMALLINT *)numericattribute=
					stmt->cur->getColumnScale(col);
			debugPrintf("  scale: %lld\n",
				(int64_t)*(SQLSMALLINT *)numericattribute);
			break;
		case SQL_DESC_NULLABLE:
		case SQL_COLUMN_NULLABLE:
			debugPrintf("  fieldidentifier: "
					"SQL_DESC/COLUMN_NULLABLE\n");
			*(SQLSMALLINT *)numericattribute=
					(stmt->cur->getColumnIsNullable(col))?
						SQL_NULLABLE:SQL_NO_NULLS;
			debugPrintf("  nullable: %lld\n",
				(int64_t)*(SQLSMALLINT *)numericattribute);
			break;
		case SQL_DESC_NAME:
		case SQL_COLUMN_NAME:
			debugPrintf("  fieldidentifier: "
					"SQL_DESC/COLUMN_NAME\n");
			{
			// SQL Relay doesn't know about column aliases,
			// just return the name.
			const char *name=stmt->cur->getColumnName(col);
			charstring::safeCopy((char *)characterattribute,
							bufferlength,name);
			debugPrintf("  name: \"%s\"\n",
					(const char *)characterattribute);
			if (stringlength) {
				*stringlength=charstring::length(name);
				debugPrintf("  length: %d\n",
						(int)*stringlength);
			} else {
				debugPrintf("  NULL stringlength "
						"(not copying out length)\n");
			}
			}
			break;
		case SQL_DESC_UNNAMED:
			debugPrintf("  fieldidentifier: "
					"SQL_DESC_UNNAMED\n");
			if (!charstring::isNullOrEmpty(
					stmt->cur->getColumnName(col))) {
				*(SQLSMALLINT *)numericattribute=SQL_NAMED;
			} else {
				*(SQLSMALLINT *)numericattribute=SQL_UNNAMED;
			} 
			debugPrintf("  unnamed: %lld\n",
				(int64_t)*(SQLSMALLINT *)numericattribute);
			break;
		//case SQL_DESC_AUTO_UNIQUE_VALUE:
		//	(dup of SQL_COLUMN_AUTO_INCREMENT)
		case SQL_COLUMN_AUTO_INCREMENT:
			debugPrintf("  fieldidentifier: "
					"SQL_DESC_AUTO_UNIQUE_VALUE/"
					"SQL_COLUMN_AUTO_INCREMENT\n");
			*(SQLINTEGER *)numericattribute=stmt->cur->
					getColumnIsAutoIncrement(col);
			debugPrintf("  auto-increment: %lld\n",
				(int64_t)*(SQLSMALLINT *)numericattribute);
			break;
		case SQL_DESC_BASE_COLUMN_NAME:
			debugPrintf("  fieldidentifier: "
					"SQL_DESC_BASE_COLUMN_NAME\n");
			// SQL Relay doesn't know this, in particular, return
			// an empty string.
			charstring::safeCopy((char *)characterattribute,
							bufferlength,"");
			debugPrintf("  base column name: \"%s\"\n",
					(const char *)characterattribute);
			if (stringlength) {
				*stringlength=0;
				debugPrintf("  length: %d\n",
						(int)*stringlength);
			} else {
				debugPrintf("  NULL stringlength "
						"(not copying out length)\n");
			}
			break;
		case SQL_DESC_BASE_TABLE_NAME:
			debugPrintf("  fieldidentifier: "
					"SQL_DESC_BASE_TABLE_NAME\n");
			// SQL Relay doesn't know this, return an empty string.
			charstring::safeCopy((char *)characterattribute,
							bufferlength,"");
			debugPrintf("  base table name: \"%s\"\n",
					(const char *)characterattribute);
			if (stringlength) {
				*stringlength=0;
				debugPrintf("  length: %d\n",
						(int)*stringlength);
			} else {
				debugPrintf("  NULL stringlength "
						"(not copying out length)\n");
			}
			break;
		//case SQL_DESC_CASE_SENSITIVE:
		//	(dup of SQL_COLUMN_CASE_SENSITIVE)
		case SQL_COLUMN_CASE_SENSITIVE:
			debugPrintf("  fieldidentifier: "
					"SQL_DESC/COLUMN_CASE_SENSITIVE\n");
			// not supported, return true
			*(SQLSMALLINT *)numericattribute=SQL_TRUE;
			debugPrintf("  case sensitive: %lld\n",
				(int64_t)*(SQLSMALLINT *)numericattribute);
			break;
		//case SQL_DESC_CATALOG_NAME:
		//	(dup of SQL_COLUMN_QUALIFIER_NAME)
		case SQL_COLUMN_QUALIFIER_NAME:
			debugPrintf("  fieldidentifier: "
					"SQL_DESC_CATALOG_NAME/"
					"SQL_COLUMN_QUALIFIER_NAME\n");
			// not supported, return empty string
			charstring::safeCopy((char *)characterattribute,
							bufferlength,"");
			debugPrintf("  column qualifier name: \"%s\"\n",
					(const char *)characterattribute);
			if (stringlength) {
				*stringlength=0;
				debugPrintf("  length: %d\n",
						(int)*stringlength);
			} else {
				debugPrintf("  NULL stringlength "
						"(not copying out length)\n");
			}
			break;
		//case SQL_DESC_DISPLAY_SIZE:
		//	(dup of SQL_COLUMN_DISPLAY_SIZE)
		case SQL_COLUMN_DISPLAY_SIZE:
			debugPrintf("  fieldidentifier: "
					"SQL_DESC/COLUMN_DISPLAY_SIZE\n");
			*(SQLLEN *)numericattribute=stmt->cur->getLongest(col);
			debugPrintf("  display size: %lld\n",
				(int64_t)*(SQLSMALLINT *)numericattribute);
			break;
		//case SQL_DESC_FIXED_PREC_SCALE
		//	(dup of SQL_COLUMN_MONEY)
		case SQL_COLUMN_MONEY:
			{
			debugPrintf("  fieldidentifier: "
					"SQL_DESC_FIXED_PREC_SCALE/"
					"SQL_COLUMN_MONEY\n");
			const char	*type=stmt->cur->getColumnType(col);
			debugPrintf("  fixed prec scale: ");
			if (!charstring::compareIgnoringCase(
							type,"money") ||
				!charstring::compareIgnoringCase(
							type,"smallmoney")) {
				*(SQLSMALLINT *)numericattribute=SQL_TRUE;
				debugPrintf("  true\n");
			} else {
				*(SQLSMALLINT *)numericattribute=SQL_FALSE;
				debugPrintf("  false\n");
			}
			}
			break;
		//case SQL_DESC_LABEL
		//	(dup of SQL_COLUMN_LABEL)
		case SQL_COLUMN_LABEL:
			{
			debugPrintf("  fieldidentifier: "
					"SQL_DESC/COLUMN_LABEL\n");
			const char *name=stmt->cur->getColumnName(col);
			charstring::safeCopy((char *)characterattribute,
							bufferlength,name);
			debugPrintf("  label: \"%s\"\n",
					(const char *)characterattribute);
			if (stringlength) {
				*stringlength=charstring::length(name);
				debugPrintf("  length: %d\n",
						(int)*stringlength);
			} else {
				debugPrintf("  NULL stringlength "
						"(not copying out length)\n");
			}
			}
			break;
		case SQL_DESC_LITERAL_PREFIX:
			{
			debugPrintf("  fieldidentifier: "
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
				} else {
					debugPrintf("  NULL stringlength "
						"(not copying out length)\n");
				}
			} else if (type==SQL_BINARY ||
					type==SQL_VARBINARY ||
					type==SQL_LONGVARBINARY) {
				charstring::safeCopy((char *)characterattribute,
							bufferlength,"0x");
				if (stringlength) {
					*stringlength=2;
				} else {
					debugPrintf("  NULL stringlength "
						"(not copying out length)\n");
				}
			} else {
				charstring::safeCopy((char *)characterattribute,
							bufferlength,"");
				if (stringlength) {
					*stringlength=0;
				} else {
					debugPrintf("  NULL stringlength "
						"(not copying out length)\n");
				}
			}
			debugPrintf("  literal prefix: %s\n",
					(const char *)characterattribute);
			if (stringlength) {
				debugPrintf("  length: %d\n",
						(int)*stringlength);
			} else {
				debugPrintf("  NULL stringlength "
						"(not copying out length)\n");
			}
			}
			break;
		case SQL_DESC_LITERAL_SUFFIX:
			{
			debugPrintf("  fieldidentifier: "
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
				} else {
					debugPrintf("  NULL stringlength "
						"(not copying out length)\n");
				}
			} else {
				charstring::safeCopy((char *)characterattribute,
							bufferlength,"");
				if (stringlength) {
					*stringlength=0;
				} else {
					debugPrintf("  NULL stringlength "
						"(not copying out length)\n");
				}
			}
			debugPrintf("  literal prefix: %s\n",
					(const char *)characterattribute);
			if (stringlength) {
				debugPrintf("  length: %d\n",
						(int)*stringlength);
			} else {
				debugPrintf("  NULL stringlength "
						"(not copying out length)\n");
			}
			}
			break;
		case SQL_DESC_LOCAL_TYPE_NAME:
			{
			debugPrintf("  fieldidentifier: "
					"SQL_DESC_LOCAL_TYPE_NAME\n");
			const char *name=stmt->cur->getColumnType(col);
			charstring::safeCopy((char *)characterattribute,
							bufferlength,name);
			debugPrintf("  local type name: \"%s\"\n",
					(const char *)characterattribute);
			if (stringlength) {
				*stringlength=charstring::length(name);
				debugPrintf("  length: %d\n",
						(int)*stringlength);
			} else {
				debugPrintf("  NULL stringlength "
						"(not copying out length)\n");
			}
			}
			break;
		case SQL_DESC_NUM_PREC_RADIX:
			debugPrintf("  fieldidentifier: "
					"SQL_DESC_NUM_PREC_RADIX\n");
			// FIXME: 2 for approximate numeric types,
			// 10 for exact numeric types, 0 otherwise
			*(SQLINTEGER *)numericattribute=0;
			debugPrintf("  num prec radix: %lld\n",
				(int64_t)*(SQLSMALLINT *)numericattribute);
			break;
		//case SQL_DESC_SCHEMA_NAME
		//	(dup of SQL_COLUMN_OWNER_NAME)
		case SQL_COLUMN_OWNER_NAME:
			debugPrintf("  fieldidentifier: "
					"SQL_DESC_SCHEMA_NAME/"
					"SQL_COLUMN_OWNER_NAME\n");
			// SQL Relay doesn't know this, return an empty string.
			charstring::safeCopy((char *)characterattribute,
							bufferlength,"");
			debugPrintf("  owner name: \"%s\"\n",
					(const char *)characterattribute);
			if (stringlength) {
				*stringlength=0;
				debugPrintf("  length: %d\n",
						(int)*stringlength);
			} else {
				debugPrintf("  NULL stringlength "
						"(not copying out length)\n");
			}
			break;
		//case SQL_DESC_SEARCHABLE
		//	(dup of SQL_COLUMN_SEARCHABLE)
		case SQL_COLUMN_SEARCHABLE:
			debugPrintf("  fieldidentifier: "
					"SQL_DESC/COLUMN_SEARCHABLE\n");
			// not supported, return searchable
			*(SQLINTEGER *)numericattribute=SQL_SEARCHABLE;
			debugPrintf("  updatable: SQL_SEARCHABLE\n");
			break;
		//case SQL_DESC_TYPE_NAME
		//	(dup of SQL_COLUMN_TYPE_NAME)
		case SQL_COLUMN_TYPE_NAME:
			debugPrintf("  fieldidentifier: "
					"SQL_DESC/COLUMN_TYPE_NAME\n");
			{
			debugPrintf("  fieldidentifier: "
					"SQL_DESC_LOCAL_TYPE_NAME\n");
			const char *name=stmt->cur->getColumnType(col);
			charstring::safeCopy((char *)characterattribute,
							bufferlength,name);
			debugPrintf("  type name: \"%s\"\n",
					(const char *)characterattribute);
			if (stringlength) {
				*stringlength=charstring::length(name);
				debugPrintf("  length: %d\n",
						(int)*stringlength);
			} else {
				debugPrintf("  NULL stringlength "
						"(not copying out length)\n");
			}
			}
			break;
		//case SQL_DESC_TABLE_NAME
		//	(dup of SQL_COLUMN_TABLE_NAME)
		case SQL_COLUMN_TABLE_NAME:
			debugPrintf("  fieldidentifier: "
					"SQL_DESC/COLUMN_TABLE_NAME\n");
			// not supported, return an empty string
			charstring::safeCopy((char *)characterattribute,
							bufferlength,"");
			debugPrintf("  table name: \"%s\"\n",
					(const char *)characterattribute);
			if (stringlength) {
				*stringlength=0;
				debugPrintf("  length: %d\n",
						(int)*stringlength);
			} else {
				debugPrintf("  NULL stringlength "
						"(not copying out length)\n");
			}
			break;
		//case SQL_DESC_UNSIGNED
		//	(dup of SQL_COLUMN_UNSIGNED)
		case SQL_COLUMN_UNSIGNED:
			debugPrintf("  fieldidentifier: "
					"SQL_DESC/COLUMN_UNSIGNED\n");
			*(SQLSMALLINT *)numericattribute=
					stmt->cur->getColumnIsUnsigned(col);
			debugPrintf("  unsigned: %lld\n",
				(int64_t)*(SQLSMALLINT *)numericattribute);
			break;
		//case SQL_DESC_UPDATABLE
		//	(dup of SQL_COLUMN_UPDATABLE)
		case SQL_COLUMN_UPDATABLE:
			debugPrintf("  fieldidentifier: "
					"SQL_DESC/COLUMN_UPDATEABLE\n");
			// not supported, return unknown
			*(SQLINTEGER *)numericattribute=
					SQL_ATTR_READWRITE_UNKNOWN;
			debugPrintf("  updatable: SQL_ATTR_READWRITE_UNKNOWN\n");
			break;
		#if (ODBCVER < 0x0300)
		case SQL_COLUMN_DRIVER_START:
			debugPrintf("  fieldidentifier: "
					"SQL_COLUMN_DRIVER_START\n");
			// not supported, return 0
			*(SQLINTEGER *)numericattribute=0;
			debugPrintf("  driver start: %lld\n",
				(int64_t)*(SQLSMALLINT *)numericattribute);
			break;
		#endif
		default:
			debugPrintf("  invalid valuetype\n");
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

static void SQLR_BuildObjectName(stringbuffer *object,
				SQLCHAR *catalogname,
				SQLSMALLINT namelength1,
				SQLCHAR *schemaname,
				SQLSMALLINT namelength2,
				SQLCHAR *objectname,
				SQLSMALLINT namelength3) {
	debugFunction();

	// FIXME: I suspect I'll be revisiting this in the future...
	//
	// Databases disagree about the definition of catalog and schema.
	// Some db's don't have the concept of a schema.
	// In some databases, the terms are interchangeable.
	//
	// It's possible for an app to accidentally pass in the same name for
	// catalog and schema, intending to only have specified one or the
	// other.
	//
	// The workaround here is: if the catalog and schema are the same,
	// don't include the schema in the name.
	//
	// Unfortunately, there are probably cases where the catalog and
	// schema are legitimately the same and the app means to pass them
	// both.  I'll bet that I'll be revisiting this code someday.

	// FIXME: some apps pass schema.object as the object name.  Arguably,
	// this should be caught.

	// FIXME: oracle needs schema.object@catalog instead of
	// catalog.schema.object

	if (namelength1) {
		if (namelength1==SQL_NTS) {
			debugPrintf("  catalog: %s\n",catalogname);
			object->append(catalogname);
		} else {
			debugPrintf("  catalog: %.*s\n",
					namelength1,catalogname);
			object->append(catalogname,namelength1);
		}
	}

	if (namelength2) {

		if (namelength2!=namelength1 ||
			charstring::compare((char *)schemaname,
						(char *)catalogname,
						namelength1)) {

			if (object->getStringLength()) {
				object->append('.');
			}
			if (namelength2==SQL_NTS) {
				debugPrintf("  schema: %s\n",schemaname);
				object->append(schemaname);
			} else {
				debugPrintf("  schema: %.*s\n",
						namelength2,schemaname);
				object->append(schemaname,namelength2);
			}
		} else {
			if (namelength2==SQL_NTS) {
				debugPrintf("  schema: %s (ignored)\n",
						schemaname);
			} else {
				debugPrintf("  schema: %.*s (ignored)\n",
						namelength2,schemaname);
			}
		}
	}

	if (namelength3) {
		if (object->getStringLength()) {
			object->append('.');
		}
		if (namelength3==SQL_NTS) {
			debugPrintf("  object: %s\n",objectname);
			object->append(objectname);
		} else {
			debugPrintf("  object: %.*s\n",
					namelength3,objectname);
			object->append(objectname,namelength3);
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
		debugPrintf("  NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	// FIXME: this code treats xxxname as a search pattern in all cases
	// xxxname is a case-insensitive search pattern if:
	// * SQL_ODBC_VERSION is SQL_OV_ODBC3
	// * SQL_ATTR_METADATA_ID is SQL_FALSE
	// otherwise it's a case-insensitive literal

	stringbuffer	table;
	SQLR_BuildObjectName(&table,catalogname,namelength1,
					schemaname,namelength2,
					tablename,namelength3);

	if (namelength4==SQL_NTS) {
		namelength4=charstring::length(columnname);
	}
	char	*wild=charstring::duplicate(
				(const char *)columnname,namelength4);
	if (!charstring::compare(wild,"%")) {
		delete[] wild;
		wild=NULL;
	}

	debugPrintf("  table: %s\n",table.getString());
	debugPrintf("  wild: %s\n",(wild)?wild:"");

	// reinit row indices
	stmt->currentfetchrow=0;
	stmt->currentstartrow=0;
	stmt->currentgetdatarow=0;

	// clear the error
	SQLR_STMTClearError(stmt);

	SQLRETURN	retval=
		(stmt->cur->getColumnList(table.getString(),wild,
						SQLRCLIENTLISTFORMAT_ODBC))?
							SQL_SUCCESS:SQL_ERROR;
	delete[] wild;

	// the statement has been executed
	stmt->executed=true;
	stmt->nodata=false;

	debugPrintf("  %s\n",(retval==SQL_SUCCESS)?"success":"error");

	// handle errors
	if (retval!=SQL_SUCCESS) {
		SQLR_STMTSetError(stmt,stmt->cur->errorMessage(),
					stmt->cur->errorNumber(),NULL);
	}
	return retval;
}


static SQLRETURN SQLR_SQLConnect(SQLHDBC connectionhandle,
					parameterstring *connparams,
					SQLCHAR *dsn,
					SQLSMALLINT dsnlength,
					SQLCHAR *user,
					SQLSMALLINT userlength,
					SQLCHAR *password,
					SQLSMALLINT passwordlength) {
	debugFunction();

	CONN	*conn=(CONN *)connectionhandle;
	if (connectionhandle==SQL_NULL_HANDLE || !conn) {
		debugPrintf("  NULL conn handle\n");
		return SQL_INVALID_HANDLE;
	}

	// copy the dsn, sometimes it's not NULL-terminated
	if (dsnlength==SQL_NTS) {
		dsnlength=charstring::length(dsn);
	}
	if ((size_t)dsnlength>=sizeof(conn->dsn)) {
		dsnlength=sizeof(conn->dsn)-1;
	}
	charstring::safeCopy(conn->dsn,sizeof(conn->dsn),
					(const char *)dsn,dsnlength);
	conn->dsn[dsnlength]='\0';

	// get data from dsn...
	// FIXME: If the dsn is an empty string then SQLGetPrivateProfileString
	// will fetch the first matching fields it can find from any DSN
	// FIXME: parameter names should be case insensitive
	// FIXME: parameter names should support brackets
	SQLGetPrivateProfileString((const char *)conn->dsn,"Server","",
					conn->server,sizeof(conn->server),
					ODBC_INI);
	char	portbuf[6];
	SQLGetPrivateProfileString((const char *)conn->dsn,"Port","",
					portbuf,sizeof(portbuf),
					ODBC_INI);
	conn->port=(uint16_t)charstring::toUnsignedInteger(portbuf);
	SQLGetPrivateProfileString((const char *)conn->dsn,"Socket","",
					conn->socket,sizeof(conn->socket),
					ODBC_INI);
	if (!charstring::isNullOrEmpty(user)) {
		if (userlength==SQL_NTS) {
			userlength=charstring::length(user);
		}
		if ((size_t)userlength>=sizeof(conn->user)) {
			userlength=sizeof(conn->user)-1;
		}
		charstring::safeCopy(conn->user,sizeof(conn->user),
					(const char *)user,userlength);
		conn->user[userlength]='\0';
	} else {
		SQLGetPrivateProfileString((const char *)conn->dsn,
						"User","",
						conn->user,
						sizeof(conn->user),
						ODBC_INI);
	}
	parameterstring	pstr;
	if (!charstring::isNullOrEmpty(password)) {
		if (passwordlength==SQL_NTS) {
			passwordlength=charstring::length(password);
		}
	} else {
		// SQLGetPrivateProfileString doesn't appear to be able to
		// extract Passwords on all platforms.
		pstr.parse(conn->dsn);
		password=(SQLCHAR *)pstr.getValue("Password");
		passwordlength=charstring::length(password);
	}
	if ((size_t)passwordlength>=sizeof(conn->password)) {
		passwordlength=sizeof(conn->password)-1;
	}
	charstring::safeCopy(conn->password,sizeof(conn->password),
				(const char *)password,passwordlength);
	conn->password[passwordlength]='\0';
	char	retrytimebuf[11];
	SQLGetPrivateProfileString((const char *)conn->dsn,"RetryTime","0",
					retrytimebuf,sizeof(retrytimebuf),
					ODBC_INI);
	conn->retrytime=(int32_t)charstring::toInteger(retrytimebuf);
	char	triesbuf[6];
	SQLGetPrivateProfileString((const char *)conn->dsn,"Tries","1",
					triesbuf,sizeof(triesbuf),
					ODBC_INI);
	conn->tries=(int32_t)charstring::toInteger(triesbuf);

	// krb options
	SQLGetPrivateProfileString((const char *)conn->dsn,"Krb","0",
					conn->krb,sizeof(conn->krb),
					ODBC_INI);
	SQLGetPrivateProfileString((const char *)conn->dsn,"Krbservice","",
					conn->krbservice,
					sizeof(conn->krbservice),
					ODBC_INI);
	SQLGetPrivateProfileString((const char *)conn->dsn,"Krbmech","",
					conn->krbmech,
					sizeof(conn->krbmech),
					ODBC_INI);
	SQLGetPrivateProfileString((const char *)conn->dsn,"Krbflags","",
					conn->krbflags,
					sizeof(conn->krbflags),
					ODBC_INI);

	// tls options
	SQLGetPrivateProfileString((const char *)conn->dsn,"Tls","0",
					conn->tls,sizeof(conn->tls),
					ODBC_INI);
	SQLGetPrivateProfileString((const char *)conn->dsn,"Tlsversion","",
					conn->tlsversion,
					sizeof(conn->tlsversion),
					ODBC_INI);
	SQLGetPrivateProfileString((const char *)conn->dsn,"Tlscert","",
					conn->tlscert,
					sizeof(conn->tlscert),
					ODBC_INI);
	SQLGetPrivateProfileString((const char *)conn->dsn,"Tlspassword","",
					conn->tlspassword,
					sizeof(conn->tlspassword),
					ODBC_INI);
	SQLGetPrivateProfileString((const char *)conn->dsn,"Tlsciphers","",
					conn->tlsciphers,
					sizeof(conn->tlsciphers),
					ODBC_INI);
	SQLGetPrivateProfileString((const char *)conn->dsn,"Tlsvalidate","",
					conn->tlsvalidate,
					sizeof(conn->tlsvalidate),
					ODBC_INI);
	SQLGetPrivateProfileString((const char *)conn->dsn,"Tlsca","",
					conn->tlsca,
					sizeof(conn->tlsca),
					ODBC_INI);
	char	tlsdepthbuf[6];
	SQLGetPrivateProfileString((const char *)conn->dsn,"Tlsdepth","",
					tlsdepthbuf,
					sizeof(tlsdepthbuf),
					ODBC_INI);
	conn->tlsdepth=(uint16_t)charstring::toUnsignedInteger(tlsdepthbuf);

	// db
	SQLGetPrivateProfileString((const char *)conn->dsn,"Db","",
					conn->db,sizeof(conn->db),
					ODBC_INI);

	// flags
	SQLGetPrivateProfileString((const char *)conn->dsn,"Debug","0",
					conn->debug,
					sizeof(conn->debug),
					ODBC_INI);
	SQLGetPrivateProfileString((const char *)conn->dsn,
					"ColumnNameCase","mixed",
					conn->columnnamecase,
					sizeof(conn->columnnamecase),
					ODBC_INI);
	char	resultsetbuffersizebuf[21];
	SQLGetPrivateProfileString((const char *)conn->dsn,
					"ResultSetBufferSize","0",
					resultsetbuffersizebuf,
					sizeof(resultsetbuffersizebuf),
					ODBC_INI);
	conn->resultsetbuffersize=
		(uint64_t)charstring::toInteger(resultsetbuffersizebuf);
	char	dontgetcolumninfobuf[6];
	SQLGetPrivateProfileString((const char *)conn->dsn,
					"DontGetColumnInfo","no",
					dontgetcolumninfobuf,
					sizeof(dontgetcolumninfobuf),
					ODBC_INI);
	conn->dontgetcolumninfo=charstring::isYes(dontgetcolumninfobuf);
	char	nullsasnullsbuf[6];
	SQLGetPrivateProfileString((const char *)conn->dsn,
					"NullsAsNulls","no",
					nullsasnullsbuf,
					sizeof(nullsasnullsbuf),
					ODBC_INI);
	conn->nullsasnulls=charstring::isYes(nullsasnullsbuf);
	char	lazyconnectbuf[6];
	SQLGetPrivateProfileString((const char *)conn->dsn,
					"LazyConnect","yes",
					lazyconnectbuf,
					sizeof(lazyconnectbuf),
					ODBC_INI);
	conn->lazyconnect=!charstring::isNo(lazyconnectbuf);
	char	clearbindsduringpreparebuf[6];
	SQLGetPrivateProfileString((const char *)conn->dsn,
					"ClearBindsDuringPrepare","yes",
					clearbindsduringpreparebuf,
					sizeof(clearbindsduringpreparebuf),
					ODBC_INI);
	conn->clearbindsduringprepare=
		!charstring::isNo(clearbindsduringpreparebuf);

	// bind variable delimiters
	SQLGetPrivateProfileString((const char *)conn->dsn,
					"BindVariableDelimiters","?:@$",
					conn->bindvariabledelimiters,
					sizeof(conn->bindvariabledelimiters),
					ODBC_INI);

	// override dsn values with values passed in via the connectstring
	if (connparams!=NULL) {
		const char	*connserver=connparams->getValue("Server");
		if (connserver!=NULL) {
			charstring::safeCopy(conn->server,
						sizeof(conn->server),
						connserver);
		}
		const char	*connport=connparams->getValue("Port");
		if (connport!=NULL) {
			conn->port=(uint16_t)
				charstring::toUnsignedInteger(connport);
		}
		const char	*connsocket=connparams->getValue("Socket");
		if (connsocket!=NULL) {
			charstring::safeCopy(conn->socket,
						sizeof(conn->socket),
						connsocket);
		}
		const char	*connretrytime=
				connparams->getValue("RetryTime");
		if (connretrytime!=NULL) {
			conn->retrytime=(int32_t)
				charstring::toInteger(connretrytime);
		}
		const char	*conntries=connparams->getValue("Tries");
		if (conntries!=NULL) {
			conn->tries=(int32_t)charstring::toInteger(conntries);
		}
		// FIXME: krb options
		// FIXME: tls options
		const char	*conn_db=connparams->getValue("Db");
		if (conn_db!=NULL) {
			charstring::safeCopy(conn->db,sizeof(conn->db),conn_db);
		}
		const char	*conndebug=connparams->getValue("Debug");
		if (conndebug!=NULL) {
			charstring::safeCopy(conn->debug,
						sizeof(conn->debug),
						conndebug);
		}
		// FIXME: other flags
		const char	*connlazyconnect=
				connparams->getValue("LazyConnect");
		if (connlazyconnect!=NULL) {
			conn->lazyconnect=!charstring::isNo(connlazyconnect);
		}
		// FIXME: other flags

		const char	*conn_bindvariabledelimiters=
				connparams->getValue("BindVariableDelimiters");
		if (conn_bindvariabledelimiters!=NULL) {
			charstring::safeCopy(conn->bindvariabledelimiters,
					sizeof(conn->bindvariabledelimiters),
					conn_bindvariabledelimiters);
		}
	}


	debugPrintf("  DSN: %s\n",conn->dsn);
	debugPrintf("  DSN Length: %d\n",dsnlength);
	debugPrintf("  Server: %s\n",conn->server);
	debugPrintf("  Port: %d\n",(int)conn->port);
	debugPrintf("  Socket: %s\n",conn->socket);
	debugPrintf("  User: %s\n",conn->user);
	debugPrintf("  Password: %s\n",conn->password);
	debugPrintf("  RetryTime: %d\n",(int)conn->retrytime);
	debugPrintf("  Tries: %d\n",(int)conn->tries);
	debugPrintf("  Krb: %s\n",conn->krb);
	debugPrintf("  Krbservice: %s\n",conn->krbservice);
	debugPrintf("  Krbmech: %s\n",conn->krbmech);
	debugPrintf("  Krbflags: %s\n",conn->krbflags);
	debugPrintf("  Tls: %s\n",conn->tls);
	debugPrintf("  Tlsversion: %s\n",conn->tlsversion);
	debugPrintf("  Tlscert: %s\n",conn->tlscert);
	debugPrintf("  Tlspassword: %s\n",conn->tlspassword);
	debugPrintf("  Tlsciphers: %s\n",conn->tlsciphers);
	debugPrintf("  Tlsvalidate: %s\n",conn->tlsvalidate);
	debugPrintf("  Tlsca: %s\n",conn->tlsca);
	debugPrintf("  Tlsdepth: %d\n",conn->tlsdepth);
	debugPrintf("  Db: %s\n",conn->db);
	debugPrintf("  Debug: %s\n",conn->debug);
	debugPrintf("  ColumnNameCase: %s\n",conn->columnnamecase);
	debugPrintf("  ResultSetBufferSize: %lld\n",conn->resultsetbuffersize);
	debugPrintf("  DontGetColumnInfo: %d\n",conn->dontgetcolumninfo);
	debugPrintf("  NullsAsNulls: %d\n",conn->nullsasnulls);
	debugPrintf("  LazyConnect: %d\n",conn->lazyconnect);
	debugPrintf("  ClearBindsDuringPrepare: %d\n",
					conn->clearbindsduringprepare);
	debugPrintf("  BindVariableDelimiters: %s\n",
					conn->bindvariabledelimiters);

	// create connection
	conn->con=new sqlrconnection(conn->server,
					conn->port,
					conn->socket,
					conn->user,
					conn->password,
					conn->retrytime,
					conn->tries,
					true);

	// enable kerberos or tls
	if (sqlrconnection::isYes(conn->krb)) {
		conn->con->enableKerberos(conn->krbservice,
							conn->krbmech,
							conn->krbflags);
	} else if (sqlrconnection::isYes(conn->tls)) {
		conn->con->enableTls(conn->tlsversion,
						conn->tlscert,
						conn->tlspassword,
						conn->tlsciphers,
						conn->tlsvalidate,
						conn->tlsca,
						conn->tlsdepth);
	}

	// enable debug
	if (charstring::isYes(conn->debug)) {
		conn->con->debugOn();
	} else if (!charstring::isNo(conn->debug) &&
			!charstring::isNullOrEmpty(conn->debug)) {
		conn->con->setDebugFile(conn->debug);
		conn->con->debugOn();
	}

	#ifdef DEBUG_MESSAGES
	conn->con->debugOn();
	#endif

	conn->con->setBindVariableDelimiters(conn->bindvariabledelimiters);

	// if we're not doing lazy connects, then do something lightweight
	// that will verify whether SQL Relay is available or not
	if (!conn->lazyconnect && !conn->con->identify()) {
		delete conn->con;
		conn->con=NULL;
		return SQL_ERROR;
	}

	if (!charstring::isNullOrEmpty(conn->db)) {
		conn->con->selectDatabase(conn->db);
	}

	// don't allow the result set buffer size to be set to "fetch all rows"
	if (!conn->resultsetbuffersize) {
		conn->resultsetbuffersize=1;
	}

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
	return SQLR_SQLConnect(connectionhandle,NULL,dsn,dsnlength,
				user,userlength,password,passwordlength);
}

SQLRETURN SQL_API SQLCopyDesc(SQLHDESC SourceDescHandle,
					SQLHDESC TargetDescHandle) {
	debugFunction();
	// FIXME: do something?
	// I guess the desc handles are ARD, APD, IRD and IPD's.
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
	debugPrintf("  columnnumber : %d\n",(int)columnnumber);

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("  NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	// make sure we're attempting to get a valid column
	uint32_t	colcount=stmt->cur->colCount();
	if (columnnumber<1 || columnnumber>colcount) {
		debugPrintf("  invalid column: %d\n",columnnumber);
		SQLR_STMTSetError(stmt,NULL,0,"07009");
		return SQL_ERROR;
	}

	// get a zero-based version of the columnnumber
	uint32_t	col=columnnumber-1;

	if (columnname) {
		charstring::safeCopy((char *)columnname,bufferlength,
					stmt->cur->getColumnName(col));
		debugPrintf("  columnname   : %s\n",columnname);
	}
	if (namelength) {
		*namelength=charstring::length(columnname);
		debugPrintf("  namelength   : %d\n",*namelength);
	}
	if (datatype) {
		*datatype=SQLR_MapColumnType(stmt->cur,col);
		debugPrintf("  datatype     : %s\n",
					stmt->cur->getColumnType(col));
	}
	if (columnsize) {
		*columnsize=SQLR_GetColumnSize(stmt->cur,col);
		debugPrintf("  columnsize   : %lld\n",(uint64_t)*columnsize);
	}
	if (decimaldigits) {
		*decimaldigits=(SQLSMALLINT)stmt->cur->getColumnScale(col);
		debugPrintf("  decimaldigits: %d\n",*decimaldigits);
	}
	if (nullable) {
		*nullable=(stmt->cur->getColumnIsNullable(col))?
						SQL_NULLABLE:SQL_NO_NULLS;
		debugPrintf("  nullable     : %d\n",
				stmt->cur->getColumnIsNullable(col));
	}

	return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLDisconnect(SQLHDBC connectionhandle) {
	debugFunction();

	CONN	*conn=(CONN *)connectionhandle;
	if (connectionhandle==SQL_NULL_HANDLE || !conn || !conn->con) {
		debugPrintf("  NULL conn handle\n");
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
			debugPrintf("  handletype: SQL_HANDLE_ENV\n");

			ENV	*env=(ENV *)handle;
			if (handle==SQL_NULL_HENV || !env) {
				debugPrintf("  NULL env handle\n");
				return SQL_INVALID_HANDLE;
			}

			for (singlylinkedlistnode<CONN *>	*node=
						env->connlist.getFirst();
						node; node=node->getNext()) {

				if (completiontype==SQL_COMMIT) {
					debugPrintf("  commit\n");
					node->getValue()->con->commit();
				} else if (completiontype==SQL_ROLLBACK) {
					debugPrintf("  rollback\n");
					node->getValue()->con->rollback();
				}
			}

			return SQL_SUCCESS;
		}
		case SQL_HANDLE_DBC:
		{
			debugPrintf("  handletype: SQL_HANDLE_DBC\n");

			CONN	*conn=(CONN *)handle;
			if (handle==SQL_NULL_HANDLE || !conn || !conn->con) {
				debugPrintf("  NULL conn handle\n");
				return SQL_INVALID_HANDLE;
			}

			if (completiontype==SQL_COMMIT) {
				debugPrintf("  commit\n");
				conn->con->commit();
			} else if (completiontype==SQL_ROLLBACK) {
				debugPrintf("  rollback\n");
				conn->con->rollback();
			}

			return SQL_SUCCESS;
		}
		default:
			debugPrintf("  invalid handletype\n");
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

	SQLRETURN	retval=SQL_NO_DATA;
	SQLSMALLINT	*recnumber=NULL;

	if (environmenthandle && environmenthandle!=SQL_NULL_HENV) {

		debugPrintf("  handletype: SQL_HANDLE_ENV\n");

		recnumber=&(((ENV *)environmenthandle)->sqlerrorindex);
		if (*recnumber) {
			retval=SQLR_SQLGetDiagRec(SQL_HANDLE_ENV,
						(SQLHANDLE)environmenthandle,
						*recnumber,sqlstate,
						nativeerror,messagetext,
						bufferlength,textlength);
			(*recnumber)--;
		} else {
			debugPrintf("  no more records\n");
		}
		return retval;

	} else if (connectionhandle && connectionhandle!=SQL_NULL_HANDLE) {

		debugPrintf("  handletype: SQL_HANDLE_DBC\n");

		recnumber=&(((CONN *)connectionhandle)->sqlerrorindex);
		if (*recnumber) {
			retval=SQLR_SQLGetDiagRec(SQL_HANDLE_DBC,
						(SQLHANDLE)connectionhandle,
						*recnumber,sqlstate,
						nativeerror,messagetext,
						bufferlength,textlength);
			(*recnumber)--;
		} else {
			debugPrintf("  no more records\n");
		}
		return retval;

	} else if (statementhandle && statementhandle!=SQL_NULL_HSTMT) {

		debugPrintf("  handletype: SQL_HANDLE_STMT\n");

		recnumber=&(((STMT *)statementhandle)->sqlerrorindex);
		if (*recnumber) {
			retval=SQLR_SQLGetDiagRec(SQL_HANDLE_STMT,
						(SQLHANDLE)statementhandle,
						*recnumber,sqlstate,
						nativeerror,messagetext,
						bufferlength,textlength);
			(*recnumber)--;
		} else {
			debugPrintf("  no more records\n");
		}
		return retval;
	}
	debugPrintf("  no valid handle\n");
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
		bytestring::zero(guid,sizeof(SQLGUID));
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

	linkedlist<dictionarynode<int32_t, outputbind *> *>
				*list=stmt->outputbinds.getList();
	for (linkedlistnode<dictionarynode<int32_t, outputbind *> *>
					*node=list->getFirst();
					node; node=node->getNext()) {

		outputbind	*ob=node->getValue()->getValue();

		// convert parameternumber to a string
		char	*parametername=charstring::parseNumber(
						ob->parameternumber);
		debugPrintf("\n");
		debugPrintf("  parametername: %s\n",parametername);
		debugPrintf("  parameternumber: %d\n",ob->parameternumber);
		debugPrintf("  valuetype: %d\n",ob->valuetype);
		debugPrintf("  lengthprecision: %lld\n",
					(uint64_t)ob->lengthprecision);
		debugPrintf("  parameterscale: %ld\n",ob->parameterscale);
		debugPrintf("  bufferlength: %lld\n",
					(uint64_t)ob->bufferlength);
		debugPrintf("  strlen_or_ind: %lld\n",
					(uint64_t)ob->strlen_or_ind);

		// FIXME: handle NULL values

		if (!ob->parametervalue) {
			debugPrintf("  parametervalue is NULL, "
					"(not copying out any value)\n");
			delete[] parametername;
			continue;
		}

		switch (ob->valuetype) {
			case SQL_C_CHAR:
				{
				debugPrintf("  valuetype: SQL_C_CHAR\n");
				const char	*str=
					stmt->cur->getOutputBindString(
								parametername);
				uint32_t	len=
					stmt->cur->getOutputBindLength(
								parametername);
				if (!str) {
					debugPrintf("  value is NULL\n");
					if (ob->strlen_or_ind) {
						*(ob->strlen_or_ind)=
							SQL_NULL_DATA;
					} else {
						debugPrintf("  strlen_or_ind "
								"is NULL\n");
					}
				} else {

					// make sure to incldue the
					// null-terminator
					charstring::safeCopy(
						(char *)ob->parametervalue,
						ob->bufferlength,
						str,len+1);

					// make sure to null-terminate
					// (even if data has to be truncated)
					((char *)ob->parametervalue)[
						ob->bufferlength-1]='\0';

					if (ob->strlen_or_ind) {
						*(ob->strlen_or_ind)=len;
					} else {
						debugPrintf("  strlen_or_ind "
								"is NULL\n");
					}
					debugPrintf("  value: \"%.*s\" (%d)\n",
								len,str,len);
				}
				}
				break;
			case SQL_C_SLONG:
			case SQL_C_LONG:
				debugPrintf("  valuetype: "
					"SQL_C_SLONG/SQL_C_LONG\n");
				*((int32_t *)ob->parametervalue)=
					(int32_t)
					stmt->cur->getOutputBindInteger(
								parametername);
				debugPrintf("  value: %d\n",
					*((int32_t *)ob->parametervalue));
				break;
			//case SQL_C_BOOKMARK:
			//	(dup of SQL_C_ULONG)
			case SQL_C_ULONG:
				debugPrintf("  valuetype: "
					"SQL_C_ULONG/SQL_C_BOOKMARK\n");
				*((uint32_t *)ob->parametervalue)=
					(uint32_t)
					stmt->cur->getOutputBindInteger(
								parametername);
				debugPrintf("  value: %d\n",
					*((int32_t *)ob->parametervalue));
				break;
			case SQL_C_SSHORT:
			case SQL_C_SHORT:
				debugPrintf("  valuetype: "
					"SQL_C_SSHORT/SQL_C_SHORT\n");
				*((int16_t *)ob->parametervalue)=
					(int16_t)
					stmt->cur->getOutputBindInteger(
								parametername);
				debugPrintf("  value: %d\n",
					*((int16_t *)ob->parametervalue));
				break;
			case SQL_C_USHORT:
				debugPrintf("  valuetype: SQL_C_USHORT\n");
				*((uint16_t *)ob->parametervalue)=
					(uint16_t)
					stmt->cur->getOutputBindInteger(
								parametername);
				debugPrintf("  value: %d\n",
					*((int16_t *)ob->parametervalue));
				break;
			case SQL_C_FLOAT:
				debugPrintf("  valuetype: SQL_C_FLOAT\n");
				*((float *)ob->parametervalue)=
					(float)stmt->cur->getOutputBindDouble(
								parametername);
				break;
			case SQL_C_DOUBLE:
				debugPrintf("  valuetype: SQL_C_DOUBLE\n");
				*((double *)ob->parametervalue)=
					(double)stmt->cur->getOutputBindDouble(
								parametername);
				break;
			case SQL_C_NUMERIC:
				debugPrintf("  valuetype: SQL_C_NUMERIC\n");
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
				debugPrintf("  valuetype: "
					"SQL_C_DATE/SQL_C_TYPE_DATE\n");
				int16_t	year;
				int16_t	month;
				int16_t	day;
				int16_t	hour;
				int16_t	minute;
				int16_t	second;
				int32_t	microsecond;
				const char	*tz;
				bool	isnegative;
				stmt->cur->getOutputBindDate(parametername,
							&year,&month,&day,
							&hour,&minute,&second,
							&microsecond,&tz,
							&isnegative);
				DATE_STRUCT	*ds=
					(DATE_STRUCT *)ob->parametervalue;
				ds->year=year;
				ds->month=month;
				ds->day=day;

				debugPrintf("    year: %d\n",ds->year);
				debugPrintf("    month: %d\n",ds->month);
				debugPrintf("    day: %d\n",ds->day);
				}
				break;
			case SQL_C_TIME:
			case SQL_C_TYPE_TIME:
				{
				debugPrintf("  valuetype: "
					"SQL_C_TIME/SQL_C_TYPE_TIME\n");
				int16_t	year;
				int16_t	month;
				int16_t	day;
				int16_t	hour;
				int16_t	minute;
				int16_t	second;
				int32_t	microsecond;
				const char	*tz;
				bool	isnegative;
				stmt->cur->getOutputBindDate(parametername,
							&year,&month,&day,
							&hour,&minute,&second,
							&microsecond,&tz,
							&isnegative);
				TIME_STRUCT	*ts=
					(TIME_STRUCT *)ob->parametervalue;
				ts->hour=hour;
				ts->minute=minute;
				ts->second=second;

				debugPrintf("    hour: %d\n",ts->hour);
				debugPrintf("    minute: %d\n",ts->minute);
				debugPrintf("    second: %d\n",ts->second);
				}
				break;
			case SQL_C_TIMESTAMP:
			case SQL_C_TYPE_TIMESTAMP:
				{
				debugPrintf("  valuetype: "
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
				bool	isnegative;
				stmt->cur->getOutputBindDate(parametername,
							&year,&month,&day,
							&hour,&minute,&second,
							&microsecond,&tz,
							&isnegative);
				TIMESTAMP_STRUCT	*ts=
					(TIMESTAMP_STRUCT *)ob->parametervalue;
				ts->year=year;
				ts->month=month;
				ts->day=day;
				ts->hour=hour;
				ts->minute=minute;
				ts->second=second;
				ts->fraction=microsecond*1000;

				debugPrintf("    year: %d\n",ts->year);
				debugPrintf("    month: %d\n",ts->month);
				debugPrintf("    day: %d\n",ts->day);
				debugPrintf("    hour: %d\n",ts->hour);
				debugPrintf("    minute: %d\n",ts->minute);
				debugPrintf("    second: %d\n",ts->second);
				debugPrintf("    fraction: %d\n",ts->fraction);
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
				debugPrintf("  valuetype: SQL_C_INTERVAL_XXX\n");
				SQLR_ParseInterval(
					(SQL_INTERVAL_STRUCT *)
							ob->parametervalue,
					stmt->cur->getOutputBindString(
							parametername),
					stmt->cur->getOutputBindLength(
							parametername));
				break;
			//case SQL_C_VARBOOKMARK:
			//	(dup of SQL_C_BINARY)
			case SQL_C_BINARY:
				{
				debugPrintf("  valuetype: "
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
				debugPrintf("  valuetype: SQL_C_BIT\n");
				const char	*val=
					stmt->cur->getOutputBindString(
								parametername);
				((unsigned char *)ob->parametervalue)[0]=
					(charstring::contains("YyTt",val) ||
					charstring::toInteger(val))?'1':'0';
				}
				break;
			case SQL_C_SBIGINT:
				debugPrintf("  valuetype: SQL_C_SBIGINT\n");
				*((int64_t *)ob->parametervalue)=
				(int64_t)stmt->cur->getOutputBindInteger(
								parametername);
				debugPrintf("  value: %lld\n",
					*((int64_t *)ob->parametervalue));
				break;
			case SQL_C_UBIGINT:
				debugPrintf("  valuetype: SQL_C_UBIGINT\n");
				*((uint64_t *)ob->parametervalue)=
				(uint64_t)stmt->cur->getOutputBindInteger(
								parametername);
				debugPrintf("  value: %lld\n",
					*((int64_t *)ob->parametervalue));
				break;
			case SQL_C_TINYINT:
			case SQL_C_STINYINT:
				debugPrintf("  valuetype: "
					"SQL_C_TINYINT/SQL_C_STINYINT\n");
				*((char *)ob->parametervalue)=
				(char)stmt->cur->getOutputBindInteger(
								parametername);
				break;
			case SQL_C_UTINYINT:
				debugPrintf("  valuetype: SQL_C_UTINYINT\n");
				*((unsigned char *)ob->parametervalue)=
				(unsigned char)stmt->cur->getOutputBindInteger(
								parametername);
				break;
			case SQL_C_GUID:
				debugPrintf("  valuetype: SQL_C_GUID\n");
				SQLR_ParseGuid(
					(SQLGUID *)ob->parametervalue,
					stmt->cur->getOutputBindString(
							parametername),
					stmt->cur->getOutputBindLength(
							parametername));
				break;
			default:
				debugPrintf("  invalue valuetype\n");
				break;
		}

		// clean up
		delete[] parametername;
	}
}

static void SQLR_FetchInputOutputBinds(SQLHSTMT statementhandle) {
	debugFunction();

	STMT	*stmt=(STMT *)statementhandle;

	linkedlist<dictionarynode<int32_t, outputbind *> *>
				*list=stmt->inputoutputbinds.getList();
	for (linkedlistnode<dictionarynode<int32_t, outputbind *> *>
					*node=list->getFirst();
					node; node=node->getNext()) {

		outputbind	*ob=node->getValue()->getValue();

		// convert parameternumber to a string
		char	*parametername=charstring::parseNumber(
						ob->parameternumber);
		debugPrintf("\n");
		debugPrintf("  parametername: %s\n",parametername);
		debugPrintf("  parameternumber: %d\n",ob->parameternumber);
		debugPrintf("  valuetype: %d\n",ob->valuetype);
		debugPrintf("  lengthprecision: %lld\n",
					(uint64_t)ob->lengthprecision);
		debugPrintf("  parameterscale: %ld\n",ob->parameterscale);
		debugPrintf("  bufferlength: %lld\n",
					(uint64_t)ob->bufferlength);
		debugPrintf("  strlen_or_ind: %lld\n",
					(uint64_t)ob->strlen_or_ind);

		// FIXME: handle NULL values

		if (!ob->parametervalue) {
			debugPrintf("  parametervalue is NULL, "
					"(not copying out any value)\n");
			delete[] parametername;
			continue;
		}

		switch (ob->valuetype) {
			case SQL_C_CHAR:
				{
				debugPrintf("  valuetype: SQL_C_CHAR\n");
				const char	*str=
					stmt->cur->getInputOutputBindString(
								parametername);
				uint32_t	len=
					stmt->cur->getInputOutputBindLength(
								parametername);
				if (!str) {
					debugPrintf("  value is NULL\n");
					if (ob->strlen_or_ind) {
						*(ob->strlen_or_ind)=
							SQL_NULL_DATA;
					} else {
						debugPrintf("  strlen_or_ind "
								"is NULL\n");
					}
				} else {

					// make sure to incldue the
					// null-terminator
					charstring::safeCopy(
						(char *)ob->parametervalue,
						ob->bufferlength,
						str,len+1);

					// make sure to null-terminate
					// (even if data has to be truncated)
					((char *)ob->parametervalue)[
						ob->bufferlength-1]='\0';

					if (ob->strlen_or_ind) {
						*(ob->strlen_or_ind)=len;
					} else {
						debugPrintf("  strlen_or_ind "
								"is NULL\n");
					}
					debugPrintf("  value: \"%.*s\" (%d)\n",
								len,str,len);
				}
				}
				break;
			case SQL_C_SLONG:
			case SQL_C_LONG:
				debugPrintf("  valuetype: "
					"SQL_C_SLONG/SQL_C_LONG\n");
				*((int32_t *)ob->parametervalue)=
					(int32_t)
					stmt->cur->getInputOutputBindInteger(
								parametername);
				debugPrintf("  value: %d\n",
					*((int32_t *)ob->parametervalue));
				break;
			//case SQL_C_BOOKMARK:
			//	(dup of SQL_C_ULONG)
			case SQL_C_ULONG:
				debugPrintf("  valuetype: "
					"SQL_C_ULONG/SQL_C_BOOKMARK\n");
				*((uint32_t *)ob->parametervalue)=
					(uint32_t)
					stmt->cur->getInputOutputBindInteger(
								parametername);
				debugPrintf("  value: %d\n",
					*((int32_t *)ob->parametervalue));
				break;
			case SQL_C_SSHORT:
			case SQL_C_SHORT:
				debugPrintf("  valuetype: "
					"SQL_C_SSHORT/SQL_C_SHORT\n");
				*((int16_t *)ob->parametervalue)=
					(int16_t)
					stmt->cur->getInputOutputBindInteger(
								parametername);
				debugPrintf("  value: %d\n",
					*((int16_t *)ob->parametervalue));
				break;
			case SQL_C_USHORT:
				debugPrintf("  valuetype: SQL_C_USHORT\n");
				*((uint16_t *)ob->parametervalue)=
					(uint16_t)
					stmt->cur->getInputOutputBindInteger(
								parametername);
				debugPrintf("  value: %d\n",
					*((int16_t *)ob->parametervalue));
				break;
			case SQL_C_FLOAT:
				debugPrintf("  valuetype: SQL_C_FLOAT\n");
				*((float *)ob->parametervalue)=
					(float)stmt->cur->
						getInputOutputBindDouble(
								parametername);
				break;
			case SQL_C_DOUBLE:
				debugPrintf("  valuetype: SQL_C_DOUBLE\n");
				*((double *)ob->parametervalue)=
					(double)stmt->cur->
						getInputOutputBindDouble(
								parametername);
				break;
			case SQL_C_NUMERIC:
				debugPrintf("  valuetype: SQL_C_NUMERIC\n");
				SQLR_ParseNumeric(
					(SQL_NUMERIC_STRUCT *)
							ob->parametervalue,
					stmt->cur->getInputOutputBindString(
								parametername),
					stmt->cur->getInputOutputBindLength(
								parametername));
				break;
			case SQL_C_DATE:
			case SQL_C_TYPE_DATE:
				{
				debugPrintf("  valuetype: "
					"SQL_C_DATE/SQL_C_TYPE_DATE\n");
				int16_t	year;
				int16_t	month;
				int16_t	day;
				int16_t	hour;
				int16_t	minute;
				int16_t	second;
				int32_t	microsecond;
				const char	*tz;
				bool	isnegative;
				stmt->cur->getInputOutputBindDate(
							parametername,
							&year,&month,&day,
							&hour,&minute,&second,
							&microsecond,&tz,
							&isnegative);
				DATE_STRUCT	*ds=
					(DATE_STRUCT *)ob->parametervalue;
				ds->year=year;
				ds->month=month;
				ds->day=day;

				debugPrintf("    year: %d\n",ds->year);
				debugPrintf("    month: %d\n",ds->month);
				debugPrintf("    day: %d\n",ds->day);
				}
				break;
			case SQL_C_TIME:
			case SQL_C_TYPE_TIME:
				{
				debugPrintf("  valuetype: "
					"SQL_C_TIME/SQL_C_TYPE_TIME\n");
				int16_t	year;
				int16_t	month;
				int16_t	day;
				int16_t	hour;
				int16_t	minute;
				int16_t	second;
				int32_t	microsecond;
				const char	*tz;
				bool	isnegative;
				stmt->cur->getInputOutputBindDate(
							parametername,
							&year,&month,&day,
							&hour,&minute,&second,
							&microsecond,&tz,
							&isnegative);
				TIME_STRUCT	*ts=
					(TIME_STRUCT *)ob->parametervalue;
				ts->hour=hour;
				ts->minute=minute;
				ts->second=second;

				debugPrintf("    hour: %d\n",ts->hour);
				debugPrintf("    minute: %d\n",ts->minute);
				debugPrintf("    second: %d\n",ts->second);
				}
				break;
			case SQL_C_TIMESTAMP:
			case SQL_C_TYPE_TIMESTAMP:
				{
				debugPrintf("  valuetype: "
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
				bool	isnegative;
				stmt->cur->getInputOutputBindDate(
							parametername,
							&year,&month,&day,
							&hour,&minute,&second,
							&microsecond,&tz,
							&isnegative);
				TIMESTAMP_STRUCT	*ts=
					(TIMESTAMP_STRUCT *)ob->parametervalue;
				ts->year=year;
				ts->month=month;
				ts->day=day;
				ts->hour=hour;
				ts->minute=minute;
				ts->second=second;
				ts->fraction=microsecond*1000;

				debugPrintf("    year: %d\n",ts->year);
				debugPrintf("    month: %d\n",ts->month);
				debugPrintf("    day: %d\n",ts->day);
				debugPrintf("    hour: %d\n",ts->hour);
				debugPrintf("    minute: %d\n",ts->minute);
				debugPrintf("    second: %d\n",ts->second);
				debugPrintf("    fraction: %d\n",ts->fraction);
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
				debugPrintf("  valuetype: SQL_C_INTERVAL_XXX\n");
				SQLR_ParseInterval(
					(SQL_INTERVAL_STRUCT *)
							ob->parametervalue,
					stmt->cur->getInputOutputBindString(
								parametername),
					stmt->cur->getInputOutputBindLength(
								parametername));
				break;
			//case SQL_C_VARBOOKMARK:
			//	(dup of SQL_C_BINARY)
			/*case SQL_C_BINARY:
				{
				debugPrintf("  valuetype: "
					"SQL_C_BINARY/SQL_C_VARBOOKMARK\n");
				charstring::safeCopy(
					(char *)ob->parametervalue,
					ob->bufferlength,
					stmt->cur->getInputOutputBindBlob(
								parametername),
					stmt->cur->getInputOutputBindLength(
								parametername));
				break;
				}*/
			case SQL_C_BIT:
				{
				debugPrintf("  valuetype: SQL_C_BIT\n");
				const char	*val=
					stmt->cur->getInputOutputBindString(
								parametername);
				((unsigned char *)ob->parametervalue)[0]=
					(charstring::contains("YyTt",val) ||
					charstring::toInteger(val))?'1':'0';
				}
				break;
			case SQL_C_SBIGINT:
				debugPrintf("  valuetype: SQL_C_SBIGINT\n");
				*((int64_t *)ob->parametervalue)=
				(int64_t)stmt->cur->getInputOutputBindInteger(
								parametername);
				debugPrintf("  value: %lld\n",
					*((int64_t *)ob->parametervalue));
				break;
			case SQL_C_UBIGINT:
				debugPrintf("  valuetype: SQL_C_UBIGINT\n");
				*((uint64_t *)ob->parametervalue)=
				(uint64_t)stmt->cur->getInputOutputBindInteger(
								parametername);
				debugPrintf("  value: %lld\n",
					*((int64_t *)ob->parametervalue));
				break;
			case SQL_C_TINYINT:
			case SQL_C_STINYINT:
				debugPrintf("  valuetype: "
					"SQL_C_TINYINT/SQL_C_STINYINT\n");
				*((char *)ob->parametervalue)=
				(char)stmt->cur->getInputOutputBindInteger(
								parametername);
				break;
			case SQL_C_UTINYINT:
				debugPrintf("  valuetype: SQL_C_UTINYINT\n");
				*((unsigned char *)ob->parametervalue)=
				(unsigned char)stmt->cur->
						getInputOutputBindInteger(
								parametername);
				break;
			case SQL_C_GUID:
				debugPrintf("  valuetype: SQL_C_GUID\n");
				SQLR_ParseGuid(
					(SQLGUID *)ob->parametervalue,
					stmt->cur->getInputOutputBindString(
								parametername),
					stmt->cur->getInputOutputBindLength(
								parametername));
				break;
			default:
				debugPrintf("  invalue valuetype\n");
				break;
		}

		// clean up
		delete[] parametername;
	}
}

static uint32_t SQLR_TrimQuery(SQLCHAR *statementtext, SQLINTEGER textlength) {

	// find the length of the string
	uint32_t	length=0;
	if (textlength==SQL_NTS) {
		length=charstring::length((const char *)statementtext);
	} else {
		length=textlength;
	}

	// if the length is 0 then it's definitely already trimmed
	if (!textlength) {
		return 0;
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

static SQLRETURN SQLR_SQLExecDirect(SQLHSTMT statementhandle,
						SQLCHAR *statementtext,
						SQLINTEGER textlength) {
	debugFunction();

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("  NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	// defer execution if there are any data-at-exec binds
	if (stmt->dataatexec) {
		debugPrintf("  data-at-exec detected, deferring execution\n");
		stmt->dataatexecstatement=statementtext;
		stmt->dataatexecstatementlength=textlength;
		return SQL_NEED_DATA;
	}

	// reinit row indices
	stmt->currentfetchrow=0;
	stmt->currentstartrow=0;
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
	debugPrintf("  statement: \"%s\" (%d)\n",
			debugstr.getString(),(int)statementtextlength);
	#endif
	bool	result=stmt->cur->sendQuery((const char *)statementtext,
							statementtextlength);

	// the statement has been executed
	stmt->executed=true;
	stmt->nodata=false;

	// handle success
	if (result) {
		debugPrintf("  success\n");
		SQLR_FetchOutputBinds(stmt);
		SQLR_FetchInputOutputBinds(stmt);
		return SQL_SUCCESS;
	}

	// handle error
	debugPrintf("  error\n");
	SQLR_STMTSetError(stmt,stmt->cur->errorMessage(),
				stmt->cur->errorNumber(),NULL);
	return SQL_ERROR;
}

SQLRETURN SQL_API SQLExecDirect(SQLHSTMT statementhandle,
					SQLCHAR *statementtext,
					SQLINTEGER textlength) {
	debugFunction();
	return SQLR_SQLExecDirect(statementhandle,statementtext,textlength);
}

static SQLRETURN SQLR_SQLExecute(SQLHSTMT statementhandle) {
	debugFunction();

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("  NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	// defer execution if there are any data-at-exec binds
	if (stmt->dataatexec) {
		debugPrintf("  data-at-exec detected, deferring execution\n");
		stmt->dataatexecstatement=NULL;
		stmt->dataatexecstatementlength=0;
		return SQL_NEED_DATA;
	}

	// don't actually do anything if the statement
	// was already executed by SQLNumResultCols
	if (stmt->executedbynumresultcols) {
		debugPrintf("  already executed by SQLNumResultCols...\n");
		stmt->executedbynumresultcols=false;
		return stmt->executedbynumresultcolsresult;
	}

	// reinit row indices
	stmt->currentfetchrow=0;
	stmt->currentstartrow=0;
	stmt->currentgetdatarow=0;

	// clear the error
	SQLR_STMTClearError(stmt);

	// run the query
	bool	result=stmt->cur->executeQuery();

	// the statement has been executed
	stmt->executed=true;
	stmt->nodata=false;

	// handle success
	if (result) {

		// set the number of sets of input binds that were processed
		// (always 1 because we don't support array binds)
		if (stmt->paramsprocessed) {
			*(stmt->paramsprocessed)=1;
		}

		SQLR_FetchOutputBinds(stmt);
		SQLR_FetchInputOutputBinds(stmt);
		return SQL_SUCCESS;
	}

	// handle error
	SQLR_STMTSetError(stmt,stmt->cur->errorMessage(),
				stmt->cur->errorNumber(),NULL);
	return SQL_ERROR;
}

SQLRETURN SQL_API SQLExecute(SQLHSTMT statementhandle) {
	debugFunction();
	return SQLR_SQLExecute(statementhandle);
}

static SQLRETURN SQLR_SQLSetPos(SQLHSTMT statementhandle,
					SQLSETPOSIROW irow,
					SQLUSMALLINT foption,
					SQLUSMALLINT flock);

static SQLRETURN SQLR_SQLGetData(SQLHSTMT statementhandle,
					SQLUSMALLINT columnnumber,
					SQLSMALLINT targettype,
					SQLPOINTER targetvalue,
					SQLLEN bufferlength,
					SQLLEN *strlen_or_ind);

static SQLRETURN SQLR_Fetch(SQLHSTMT statementhandle,
					SQLULEN *pcrow,
					SQLUSMALLINT *rgfrowstatus,
					uint64_t rowstofetch) {
	debugFunction();

	// no need to validate stmt, the various functions that call
	// SQLR_Fetch have already validated it
	STMT	*stmt=(STMT *)statementhandle;

	// set the result set buffer size
	// (it's safe to set this here because we always lazy-fetch)
	stmt->cur->setResultSetBufferSize(rowstofetch);

	// fetch the row(s)
	debugPrintf("  currentfetchrow: %lld\n",stmt->currentfetchrow);
	SQLRETURN	fetchresult=
			(stmt->cur->getRow(stmt->currentfetchrow))?
					SQL_SUCCESS:SQL_NO_DATA_FOUND;

	// Determine the number of rows that were actually fetched.
	uint64_t	rowsfetched=0;
	if (fetchresult==SQL_NO_DATA_FOUND) {
		stmt->nodata=true;
	} else {
		uint64_t	firstrowindex=stmt->cur->firstRowIndex();
		uint64_t	rowcount=stmt->cur->rowCount();
		uint64_t	lastrowindex=(rowcount)?rowcount-1:0;
		uint64_t	bufferedrowcount=lastrowindex-firstrowindex+1;
		rowsfetched=(firstrowindex==stmt->currentfetchrow)?
							bufferedrowcount:0;
		debugPrintf("  firstrowindex   : %lld\n",firstrowindex);
		debugPrintf("  rowcount        : %lld\n",rowcount);
		debugPrintf("  lastrowindex    : %lld\n",lastrowindex);
		debugPrintf("  bufferedrowcount: %lld\n",bufferedrowcount);
	}

	debugPrintf("  rowstofetch: %lld\n",rowstofetch);
	debugPrintf("  rowsfetched: %lld\n",rowsfetched);

	// update fetched row counter
	if (pcrow) {
		*pcrow=rowsfetched;
	}

	// update row statuses
	if (rgfrowstatus) {
		for (SQLULEN i=0; i<rowstofetch; i++) {
			rgfrowstatus[i]=(i<rowsfetched)?
						SQL_ROW_SUCCESS:SQL_ROW_NOROW;
		}
	}

	// reinit column positions
	uint32_t	colcount=stmt->cur->colCount();
	delete[] stmt->coloffsets;
	stmt->coloffsets=new uint64_t[rowsfetched*colcount];
	for (uint64_t i=0; i<rowsfetched*colcount; i++) {
		stmt->coloffsets[i]=0;
	}

	// bail here if no data was found
	if (fetchresult==SQL_NO_DATA_FOUND) {
		debugPrintf("  NO DATA FOUND\n");
		return fetchresult;
	}

	// Update the "start row" (the index of the first row of the block of
	// rows that was just fetched).  If we end up calling SQLR_SQLSetPos()
	// below will, then it will need this to be set correctly here.
	stmt->currentstartrow=stmt->currentfetchrow;

	// update column binds (if we have any)
	if (stmt->fieldlist.getList()->getLength()) {

		for (uint64_t row=0; row<rowsfetched; row++) {

			// set the position within the block
			// of rows that we just fetched
			SQLR_SQLSetPos(statementhandle,row+1,SQL_POSITION,0);

			for (uint32_t index=0; index<colcount; index++) {

				// get the bound field, if this
				// field isn't bound, move on
				FIELD	*field=NULL;
				if (!stmt->fieldlist.getValue(index,&field)) {
					continue;
				}

				// handle the targetvalue
				unsigned char	*targetvalue=NULL;
				if (field->targetvalue) {
					targetvalue=((unsigned char *)
						field->targetvalue)+
						(field->bufferlength*row);
				}

				// get the data into the bound column
				SQLRETURN	getdataresult=
					SQLR_SQLGetData(
						statementhandle,
						index+1,
						field->targettype,
						targetvalue,
						field->bufferlength,
						&(field->strlen_or_ind[row]));
				if (getdataresult!=SQL_SUCCESS) {
					return getdataresult;
				}
			}
		}
	}

	// update the "fetch row"
	// (the index of the row that the next call to this function
	// will call getRow() on to fetch the next block of rows)
	stmt->currentfetchrow=stmt->currentfetchrow+rowsfetched;

	// Update the "get data row" (the index of the row that the next call
	// to SQLGetData() will operate on).  If we ended up calling
	// SQLR_SQLSetPos() above, then it will have modified this value, and
	// we need to reset it here so future calls to SQLSetPos()/SQLGetData()
	// will work as expected.
	stmt->currentgetdatarow=stmt->currentstartrow;

	debugPrintf("  currentstartrow  : %lld\n",stmt->currentstartrow);
	debugPrintf("  currentfetchrow  : %lld\n",stmt->currentfetchrow);
	debugPrintf("  currentgetdatarow: %lld\n",stmt->currentgetdatarow);

	// At this point:
	//
	// currentstartrow is set to the index of the first row of this block
	// of rows.
	//
	// currentfetchrow is set to the index of the first row of the next
	// block of rows.
	//
	// currentgetdatarow is also set to the index of the first row of this
	// block of rows.  This may not seem correct, but it is.  This allows
	// SQLGetData to be used to re-fetch fields, starting with the first
	// one in the row.  SQLSetPos can be used to get fields from the other
	// rows.

	return fetchresult;
}

SQLRETURN SQL_API SQLFetch(SQLHSTMT statementhandle) {
	debugFunction();

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("  NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	SQLULEN	localrowsfetched;
	SQLRETURN	retval=SQLR_Fetch(statementhandle,
						&localrowsfetched,
						stmt->rowstatusptr,
						stmt->rowarraysize);
	if (stmt->rowsfetchedptr) {
		*stmt->rowsfetchedptr=localrowsfetched;
	}
	return retval;
}

SQLRETURN SQL_API SQLFetchScroll(SQLHSTMT statementhandle,
					SQLSMALLINT fetchorientation,
					SQLLEN fetchoffset) {
	debugFunction();

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("  NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	// for now we only support SQL_FETCH_NEXT
	if (fetchorientation!=SQL_FETCH_NEXT) {
		debugPrintf("  invalid fetchorientation\n");
		stmt->sqlstate="HY106";
		return SQL_ERROR;
	}

	SQLULEN	localrowsfetched=0;
	SQLRETURN	retval=SQLR_Fetch(statementhandle,
						&localrowsfetched,
						stmt->rowstatusptr,
						stmt->rowarraysize);
	if (stmt->rowsfetchedptr) {
		*stmt->rowsfetchedptr=localrowsfetched;
	}
	return retval;
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
			debugPrintf("  handletype: SQL_HANDLE_ENV\n");
			ENV	*env=(ENV *)handle;
			if (handle==SQL_NULL_HENV || !env) {
				debugPrintf("  NULL env handle\n");
				return SQL_INVALID_HANDLE;
			}
			env->connlist.clear();
			delete[] env->error;
			delete env;
			return SQL_SUCCESS;
			}
		case SQL_HANDLE_DBC:
			{
			debugPrintf("  handletype: SQL_HANDLE_DBC\n");
			CONN	*conn=(CONN *)handle;
			if (handle==SQL_NULL_HANDLE || !conn || !conn->con) {
				debugPrintf("  NULL conn handle\n");
				return SQL_INVALID_HANDLE;
			}
			conn->env->connlist.removeAll(conn);
			conn->stmtlist.clear();
			delete conn->con;
			delete[] conn->error;
			delete conn;
			return SQL_SUCCESS;
			}
		case SQL_HANDLE_STMT:
			{
			debugPrintf("  handletype: SQL_HANDLE_STMT\n");
			STMT	*stmt=(STMT *)handle;
			if (handle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
				debugPrintf("  NULL stmt handle\n");
				return SQL_INVALID_HANDLE;
			}
			stmt->conn->stmtlist.removeAll(stmt);
			delete stmt->improwdesc;
			delete stmt->impparamdesc;
			delete[] stmt->coloffsets;
			delete stmt->cur;
			delete stmt;
			return SQL_SUCCESS;
			}
		case SQL_HANDLE_DESC:
			debugPrintf("  handletype: SQL_HANDLE_DESC\n");
			// FIXME: no idea what to do here,
			// for now just report success
			return SQL_SUCCESS;
		default:
			debugPrintf("  invalid handletype\n");
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
		debugPrintf("  NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	switch (option) {
		case SQL_CLOSE:
			debugPrintf("  option: SQL_CLOSE\n");
			return SQLR_SQLCloseCursor(statementhandle);
		case SQL_DROP:
			debugPrintf("  option: SQL_DROP\n");
			return SQLR_SQLFreeHandle(SQL_HANDLE_STMT,
						(SQLHANDLE)statementhandle);
		case SQL_UNBIND:
			debugPrintf("  option: SQL_UNBIND\n");
			stmt->fieldlist.clear();
			return SQL_SUCCESS;
		case SQL_RESET_PARAMS:
			debugPrintf("  option: SQL_RESET_PARAMS\n");
			SQLR_ResetParams(stmt);
			return SQL_SUCCESS;
		default:
			debugPrintf("  invalid option\n");
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
		debugPrintf("  NULL conn handle\n");
		return SQL_INVALID_HANDLE;
	}

	union {
		const char	*strval;
		SQLUINTEGER	uintval;
	} val;
	int16_t	type=-1;

	switch (attribute) {
		case SQL_ACCESS_MODE:
			debugPrintf("  unsupported attribute: "
						"SQL_ACCESS_MODE\n");
			// FIXME: implement
			break;
		case SQL_AUTOCOMMIT:
			debugPrintf("  unsupported attribute: "
						"SQL_AUTOCOMMIT\n");
			// FIXME: implement
			break;
		case SQL_LOGIN_TIMEOUT:
			debugPrintf("  unsupported attribute: "
						"SQL_LOGIN_TIMEOUT\n");
			// FIXME: implement
			break;
		case SQL_OPT_TRACE:
			debugPrintf("  unsupported attribute: "
						"SQL_OPT_TRACE\n");
			// FIXME: implement
			break;
		case SQL_OPT_TRACEFILE:
			debugPrintf("  unsupported attribute: "
						"SQL_OPT_TRACEFILE\n");
			// FIXME: implement
			break;
		case SQL_TRANSLATE_DLL:
			debugPrintf("  unsupported attribute: "
						"SQL_TRANSLATE_DLL\n");
			// FIXME: implement
			break;
		case SQL_TRANSLATE_OPTION:
			debugPrintf("  unsupported attribute: "
						"SQL_TRANSLATE_OPTION\n");
			// FIXME: implement
			break;
		case SQL_TXN_ISOLATION:
			debugPrintf("  attribute: SQL_TXN_ISOLATION\n");
			// FIXME: this isn't always true
			val.uintval=SQL_TXN_READ_COMMITTED;
			type=1;
			break;
		//case SQL_ATTR_CURRENT_CATALOG:
		//	(dup of SQL_CURRENT_QUALIFIER)
		case SQL_CURRENT_QUALIFIER:
			debugPrintf("  attribute: SQL_CURRENT_QUALIFIER/"
						"SQL_ATTR_CURRENT_CATALOG\n");
			val.strval=conn->con->getCurrentDatabase();
			type=0;
			break;
		case SQL_ODBC_CURSORS:
			debugPrintf("  unsupported attribute: "
						"SQL_ODBC_CURSORS\n");
			// FIXME: implement
			break;
		case SQL_QUIET_MODE:
			debugPrintf("  attribute: SQL_QUIET_MODE\n");
			// SQL Relay doesn't need to do anything with this
			break;
		case SQL_PACKET_SIZE:
			debugPrintf("  unsupported attribute: "
						"SQL_PACKET_SIZE\n");
			// FIXME: implement
			break;
	#if (ODBCVER >= 0x0300)
		case SQL_ATTR_CONNECTION_TIMEOUT:
			debugPrintf("  unsupported attribute: "
					"SQL_ATTR_CONNECTION_TIMEOUT\n");
			// FIXME: implement
			break;
		case SQL_ATTR_DISCONNECT_BEHAVIOR:
			debugPrintf("  unsupported attribute: "
					"SQL_ATTR_DISCONNECT_BEHAVIOR\n");
			// FIXME: implement
			break;
		case SQL_ATTR_ENLIST_IN_DTC:
			debugPrintf("  unsupported attribute: "
						"SQL_ATTR_ENLIST_IN_DTC\n");
			// FIXME: implement
			break;
		case SQL_ATTR_ENLIST_IN_XA:
			debugPrintf("  unsupported attribute: "
						"SQL_ATTR_ENLIST_IN_XA\n");
			// FIXME: implement
			break;
		case SQL_ATTR_AUTO_IPD:
			debugPrintf("  unsupported attribute: "
						"SQL_ATTR_AUTO_IPD\n");
			// FIXME: implement
			break;
		case SQL_ATTR_METADATA_ID:
			debugPrintf("  attribute: SQL_ATTR_METADATA_ID\n");
			val.uintval=(conn->attrmetadataid)?SQL_TRUE:SQL_FALSE;
			type=1;
			break;
	#endif
		default:
			debugPrintf("  invalid attribute: %d\n",attribute);
			SQLR_CONNSetError(conn,
				"Optional field not implemented",0,"HYC00");
			return SQL_ERROR;
	}


	// copy out the value and length
	SQLSMALLINT	valuelength=0;
	switch (type) {
		case -1:
			debugPrintf("  (not copying out any value)\n");
			break;
		case 0:
			debugPrintf("  strval: %s\n",val.strval);
			valuelength=charstring::length(val.strval);
			debugPrintf("  bufferlength: %d\n",(int)bufferlength);
			if (value && bufferlength) {

				charstring::safeCopy((char *)value,
							bufferlength,
							val.strval);

				// make sure to null-terminate
				// (even if data has to be truncated)
				((char *)value)[bufferlength-1]='\0';

				if (valuelength>bufferlength) {
					debugPrintf("  WARNING! valuelength>"
							"bufferlength\n");
				}
			} else {
				if (!value) {
					debugPrintf("  NULL value "
						"(not copying out strval)\n");
				}
				if (!bufferlength) {
					debugPrintf("  0 bufferlength "
						"(not copying out strval)\n");
				}
			}
			break;
		case 1:
			debugPrintf("  uintval: %d\n",val.uintval);
			valuelength=sizeof(SQLUINTEGER);
			if (value) {
				*((SQLUINTEGER *)value)=val.uintval;
			} else {
				debugPrintf("  NULL value "
						"(not copying out uintval)\n");
			}
			break;
	}
	debugPrintf("  valuelength: %d\n",(int)valuelength);
	if (stringlength) {
		*stringlength=valuelength;
	} else {
		debugPrintf("  NULL stringlength "
					"(not copying out valuelength)\n");
	}

	return SQL_SUCCESS;
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
		debugPrintf("  NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	if (!stmt->name) {
		stmt->name=charstring::parseNumber(stmtid);
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
	int32_t	usec=-1;
	bool	isnegative=false;

	// get day/month format
	bool	ddmm=charstring::isYes(environment::getValue(
					"SQLR_ODBC_DATE_DDMM"));
	bool		yyyyddmm=ddmm;
	const char	*yyyyddmmstr=environment::getValue(
					"SQLR_ODBC_DATE_YYYYDDMM");
	if (yyyyddmmstr) {
		yyyyddmm=charstring::isYes(yyyyddmmstr);
	}

	// parse
	parseDateTime(value,ddmm,yyyyddmm,"/-:",&year,&month,&day,
				&hour,&minute,&second,&usec,&isnegative);

	// copy data out
	ds->year=(year!=-1)?year:0;
	ds->month=(month!=-1)?month:0;
	ds->day=(day!=-1)?day:0;

	debugPrintf("    value: %s\n",value);
	debugPrintf("    year: %d\n",ds->year);
	debugPrintf("    month: %d\n",ds->month);
	debugPrintf("    day: %d\n",ds->day);
}

static void SQLR_ParseTime(TIME_STRUCT *ts, const char *value) {

	// result variables
	int16_t	year=-1;
	int16_t	month=-1;
	int16_t	day=-1;
	int16_t	hour=-1;
	int16_t	minute=-1;
	int16_t	second=-1;
	int32_t	usec=-1;
	bool	isnegative=false;

	// get day/month format
	bool	ddmm=charstring::isYes(environment::getValue(
					"SQLR_ODBC_DATE_DDMM"));
	bool		yyyyddmm=ddmm;
	const char	*yyyyddmmstr=environment::getValue(
					"SQLR_ODBC_DATE_YYYYDDMM");
	if (yyyyddmmstr) {
		yyyyddmm=charstring::isYes(yyyyddmmstr);
	}

	// parse
	parseDateTime(value,ddmm,yyyyddmm,"/-:",&year,&month,&day,
				&hour,&minute,&second,&usec,&isnegative);

	// copy data out
	ts->hour=(hour!=-1)?hour:0;
	ts->minute=(minute!=-1)?minute:0;
	ts->second=(second!=-1)?second:0;

	debugPrintf("    value: %s\n",value);
	debugPrintf("    hour: %d\n",ts->hour);
	debugPrintf("    minute: %d\n",ts->minute);
	debugPrintf("    second: %d\n",ts->second);
}

static void SQLR_ParseTimeStamp(TIMESTAMP_STRUCT *tss, const char *value) {

	// result variables
	int16_t	year=-1;
	int16_t	month=-1;
	int16_t	day=-1;
	int16_t	hour=-1;
	int16_t	minute=-1;
	int16_t	second=-1;
	int32_t	usec=-1;
	bool	isnegative=false;

	// get day/month format
	bool	ddmm=charstring::isYes(environment::getValue(
					"SQLR_ODBC_DATE_DDMM"));
	bool		yyyyddmm=ddmm;
	const char	*yyyyddmmstr=environment::getValue(
					"SQLR_ODBC_DATE_YYYYDDMM");
	if (yyyyddmmstr) {
		yyyyddmm=charstring::isYes(yyyyddmmstr);
	}

	// parse
	parseDateTime(value,ddmm,yyyyddmm,"/-:",&year,&month,&day,
				&hour,&minute,&second,&usec,&isnegative);

	// copy data out
	tss->year=(year!=-1)?year:0;
	tss->month=(month!=-1)?month:0;
	tss->day=(day!=-1)?day:0;
	tss->hour=(hour!=-1)?hour:0;
	tss->minute=(minute!=-1)?minute:0;
	tss->second=(second!=-1)?second:0;
	tss->fraction=(usec!=-1)?usec*1000:0;

	debugPrintf("    value: %s\n",value);
	debugPrintf("    year: %d\n",tss->year);
	debugPrintf("    month: %d\n",tss->month);
	debugPrintf("    day: %d\n",tss->day);
	debugPrintf("    hour: %d\n",tss->hour);
	debugPrintf("    minute: %d\n",tss->minute);
	debugPrintf("    second: %d\n",tss->second);
	debugPrintf("    fraction: %d\n",tss->fraction);
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
		debugPrintf("  NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	// bail if we've already fetched beyond the end
	if (stmt->nodata) {
		debugPrintf("  after the end of the result set\n");
		SQLR_STMTSetError(stmt,NULL,0,"24000");
		return SQL_ERROR;
	}

	// bail if bufferlength < 0
	if (bufferlength<0) {
		debugPrintf("  bufferlength < 0 (%lld)\n",
						(int64_t)bufferlength);
		SQLR_STMTSetError(stmt,
			"Invalid string or buffer length",0,"HY090");
		return SQL_ERROR;
	}

	debugPrintf("  row   : %lld\n",stmt->currentgetdatarow);
	debugPrintf("  column: %d\n",(int)columnnumber);
	debugPrintf("  bufferlength: %lld\n",(int64_t)bufferlength);

	// make sure we're attempting to get a valid column
	uint32_t	colcount=stmt->cur->colCount();
	if (columnnumber<1 || columnnumber>colcount) {
		debugPrintf("  invalid column: %d\n",columnnumber);
		SQLR_STMTSetError(stmt,"Invalid descriptor index",0,"07009");
		return SQL_ERROR;
	}

	// get a zero-based version of the columnnumber
	uint32_t	col=columnnumber-1;

	// get the field
	const char	*field=stmt->cur->getField(
					stmt->currentgetdatarow,col);
	uint32_t	fieldlength=stmt->cur->getFieldLength(
					stmt->currentgetdatarow,col);
	debugPrintf("  field: %.*s%s",
			(fieldlength<=80)?fieldlength:80,
			field,(fieldlength>80)?"...\n":"\n");
	debugPrintf("  fieldlength: %d\n",fieldlength);

	// get the offset
	uint64_t	*offset=&(stmt->coloffsets[
					(stmt->currentgetdatarow-
					stmt->cur->firstRowIndex())*
					stmt->cur->colCount()+col]);
	debugPrintf("  offset: %lld\n",*offset);

	// handle NULL fields
	if (!field) {
		if (strlen_or_ind) {
			*strlen_or_ind=SQL_NULL_DATA;
		}
		debugPrintf("  null field\n");
		return SQL_SUCCESS;
	}

	// reset targettype based on column type
	if (targettype==SQL_C_DEFAULT) {
		targettype=SQLR_MapCColumnType(stmt->cur,col);
		debugPrintf("  targettype SQL_C_DEFAULT, "
						"mapped to: %d (from %s)\n",
						(int)targettype,
						stmt->cur->getColumnType(col));
	}

	// initialize strlen indicator
	if (strlen_or_ind) {
		*strlen_or_ind=SQLR_GetCColumnTypeSize(targettype);
		debugPrintf("  strlen_or_ind (from type): %lld\n",
						(int64_t)*strlen_or_ind);
	} else {
		debugPrintf("  NULL strlen_or_ind (not setting from type)\n");
	}

	// get the field data
	bool	trunc=false;
	bool	nodata=false;
	switch (targettype) {
		case SQL_C_CHAR:
		#ifdef SQL_C_WCHAR
		case SQL_C_WCHAR:
		#endif
			{
			debugPrintf("  targettype: SQL_C_(W)CHAR\n");

			uint32_t	bytestocopy;
			if (*offset<fieldlength) {

				// reset field and fieldlength re. offset
				field+=*offset;
				fieldlength-=*offset;

				// calculate size to copy
				// make sure to include the null-terminator
				if ((uint32_t)bufferlength<fieldlength+1) {
					bytestocopy=bufferlength;
					*offset+=bytestocopy;
					trunc=true;
				} else {
					bytestocopy=fieldlength+1;
					*offset+=fieldlength;
				}
			} else {
				if (*offset>fieldlength) {
					nodata=true;
				} else {
					(*offset)++;
				}
				fieldlength=0;
				bytestocopy=0;
			}
			debugPrintf("  bytestocopy: %ld\n",bytestocopy);

			if (strlen_or_ind) {
				*strlen_or_ind=fieldlength;
			}

			if (targetvalue) {
				charstring::copy((char *)targetvalue,
							(const char *)field,
							bytestocopy);

				// make sure to null-terminate
				// (even if data has to be truncated)
				if (trunc) {
					((char *)targetvalue)
						[bytestocopy-1]='\0';
				}

				debugPrintf("  value: %.*s%s",
					(bytestocopy<=80)?bytestocopy:80,
					(char *)targetvalue,
					(bytestocopy>80)?"...\n":"\n");
			}
			}
			break;
		case SQL_C_SSHORT:
		case SQL_C_SHORT:
			debugPrintf("  targettype: SQL_C_(S)SHORT\n");
			if (targetvalue) {
				*((SQLSMALLINT *)targetvalue)=
					(SQLSMALLINT)
						charstring::toInteger(field);
				debugPrintf("  value: %d\n",
						*((SQLSMALLINT *)targetvalue));
			}
			break;
		case SQL_C_USHORT:
			debugPrintf("  targettype: SQL_C_USHORT\n");
			if (targetvalue) {
				*((SQLUSMALLINT *)targetvalue)=
					(SQLUSMALLINT)
						charstring::toInteger(field);
				debugPrintf("  value: %d\n",
						*((SQLUSMALLINT *)targetvalue));
			}
			break;
		case SQL_C_SLONG:
		case SQL_C_LONG:
			debugPrintf("  targettype: SQL_C_(S)LONG\n");
			if (targetvalue) {
				*((SQLINTEGER *)targetvalue)=
					(SQLINTEGER)
						charstring::toInteger(field);
				debugPrintf("  value: %ld\n",
						*((SQLINTEGER *)targetvalue));
			}
			break;
		//case SQL_C_BOOKMARK:
		//	(dup of SQL_C_ULONG)
		case SQL_C_ULONG:
			debugPrintf("  targettype: SQL_C_ULONG\n");
			if (targetvalue) {
				*((SQLUINTEGER *)targetvalue)=
					(SQLUINTEGER)
						charstring::toInteger(field);
				debugPrintf("  value: %ld\n",
						*((SQLUINTEGER *)targetvalue));
			}
			break;
		case SQL_C_FLOAT:
			debugPrintf("  targettype: SQL_C_FLOAT\n");
			if (targetvalue) {
				*((SQLREAL *)targetvalue)=
					(SQLREAL)charstring::toFloatC(field);
				debugPrintf("  value: %f\n",
						*((SQLREAL *)targetvalue));
			}
			break;
		case SQL_C_DOUBLE:
			debugPrintf("  targettype: SQL_C_DOUBLE\n");
			if (targetvalue) {
				*((SQLDOUBLE *)targetvalue)=
					(SQLDOUBLE)charstring::toFloatC(field);
				debugPrintf("  value: %f\n",
						*((SQLDOUBLE *)targetvalue));
			}
			break;
		case SQL_C_BIT:
			debugPrintf("  targettype: SQL_C_BIT\n");
			if (targetvalue) {
				((unsigned char *)targetvalue)[0]=
					(charstring::contains("YyTt",field) ||
					charstring::toInteger(field))?'1':'0';
				debugPrintf("  value: %c\n",
					*((unsigned char *)targetvalue));
			}
			break;
		case SQL_C_STINYINT:
		case SQL_C_TINYINT:
			debugPrintf("  targettype: SQL_C_(S)TINYINT\n");
			if (targetvalue) {
				*((SQLSCHAR *)targetvalue)=
					charstring::toInteger(field);
				debugPrintf("  value: %d\n",
						*((SQLSCHAR *)targetvalue));
			}
			break;
		case SQL_C_UTINYINT:
			debugPrintf("  targettype: SQL_C_UTINYINT\n");
			if (targetvalue) {
				*((SQLCHAR *)targetvalue)=
					charstring::toInteger(field);
				debugPrintf("  value: %d\n",
					*((SQLCHAR *)targetvalue));
			}
			break;
		case SQL_C_SBIGINT:
			debugPrintf("  targettype: SQL_C_SBIGINT\n");
			if (targetvalue) {
				*((SQLBIGINT *)targetvalue)=
					charstring::toInteger(field);
				debugPrintf("  value: %lld\n",
						*((SQLBIGINT *)targetvalue));
			}
			break;
		case SQL_C_UBIGINT:
			debugPrintf("  targettype: SQL_C_UBIGINT\n");
			if (targetvalue) {
				*((SQLUBIGINT *)targetvalue)=
					charstring::toInteger(field);
				debugPrintf("  value: %lld\n",
						*((SQLUBIGINT *)targetvalue));
			}
			break;
		//case SQL_C_VARBOOKMARK:
		//	(dup of SQL_C_BINARY)
		case SQL_C_BINARY:
			{
			debugPrintf("  targettype: "
				"SQL_C_BINARY/SQL_C_VARBOOKMARK\n");

			uint32_t	bytestocopy;
			if (*offset<fieldlength) {

				// reset field and fieldlength re. offset
				field+=*offset;
				fieldlength-=*offset;

				// calculate size to copy
				if ((uint32_t)bufferlength<fieldlength) {
					bytestocopy=bufferlength;
					*offset+=bytestocopy;
					trunc=true;
				} else {
					bytestocopy=fieldlength;
					*offset+=fieldlength;
				}
			} else {
				if (*offset>fieldlength) {
					nodata=true;
				} else {
					(*offset)++;
				}
				fieldlength=0;
				bytestocopy=0;
			}
			debugPrintf("  bytestocopy: %ld\n",bytestocopy);

			if (strlen_or_ind) {
				*strlen_or_ind=fieldlength;
			}

			if (targetvalue) {
				bytestring::copy((void *)targetvalue,
							(const void *)field,
							bytestocopy);
				debugPrintf("  value: ");
				debugSafePrint((char *)targetvalue,
					(bytestocopy<=80)?bytestocopy:80);
				if (bytestocopy>80) {
					debugPrintf("...");
				}
				debugPrintf("\n");
			}
			}
			break;
		case SQL_C_DATE:
		case SQL_C_TYPE_DATE:
			debugPrintf("  targettype: "
					"SQL_C_DATE/SQL_C_TYPE_DATE\n");
			if (targetvalue) {
				SQLR_ParseDate(
					(DATE_STRUCT *)targetvalue,field);
			}
			break;
		case SQL_C_TIME:
		case SQL_C_TYPE_TIME:
			debugPrintf("  targettype: "
					"SQL_C_TIME/SQL_C_TYPE_TIME\n");
			if (targetvalue) {
				SQLR_ParseTime(
					(TIME_STRUCT *)targetvalue,field);
			}
			break;
		case SQL_C_TIMESTAMP:
		case SQL_C_TYPE_TIMESTAMP:
			debugPrintf("  targettype: "
				"SQL_C_TIMESTAMP/SQL_C_TYPE_TIMESTAMP\n");
			if (targetvalue) {
				SQLR_ParseTimeStamp(
					(TIMESTAMP_STRUCT *)targetvalue,field);
			}
			break;
		case SQL_C_NUMERIC:
			debugPrintf("  targettype: SQL_C_NUMERIC\n");
			if (targetvalue) {
				SQLR_ParseNumeric(
					(SQL_NUMERIC_STRUCT *)targetvalue,
					field,fieldlength);
			}
			break;
		case SQL_C_GUID:
			debugPrintf("  targettype: SQL_C_GUID\n");
			if (targetvalue) {
				SQLR_ParseGuid((SQLGUID *)targetvalue,
							field,fieldlength);
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
			debugPrintf("  targettype: SQL_C_INTERVAL_XXX\n");
			if (targetvalue) {
				SQLR_ParseInterval((SQL_INTERVAL_STRUCT *)
							targetvalue,
							field,fieldlength);
			}
			break;
		default:
			debugPrintf("  invalid targettype\n");
			return SQL_ERROR;
	}

	debugPrintf("  offset: %lld\n",*offset);
	debugPrintf("  trunc: %d\n",trunc);

	if (!targetvalue) {
		debugPrintf("  NULL targetvalue (not copying out value)\n");
	}

	if (strlen_or_ind) {
		debugPrintf("  strlen_or_ind: %lld\n",(int64_t)*strlen_or_ind);
		if (*strlen_or_ind>bufferlength) {
			debugPrintf("  WARNING! strlen_or_ind>bufferlength\n");
		}
	} else {
		debugPrintf("  NULL strlen_or_ind (not copying out length)\n");
	}

	if (trunc) {
		debugPrintf("  returning SQL_SUCCESS_WITH_INFO\n");
		SQLR_STMTSetError(stmt,
			"String data, right truncation",0,"01004");
		return SQL_SUCCESS_WITH_INFO;
	}
	if (nodata) {
		debugPrintf("  returning SQL_NO_DATA\n");
		return SQL_NO_DATA;
	}
	debugPrintf("  returning SQL_SUCCESS\n");
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

static const char *odbc3states[]={
	"01S00","01S01","01S02","01S06","01S07","07S01","08S01",
	"21S01","21S02","25S01","25S02","25S03",
	"42S01","42S02","42S11","42S12","42S21","42S22",
	"HY095","HY097","HY098","HY099","HY100","HY101","HY105",
	"HY107","HY109","HY110","HY111","HYT00","HYT01",
	"IM001","IM002","IM003","IM004","IM005","IM006","IM007",
	"IM008","IM010","IM011","IM012",NULL
};

SQLRETURN SQL_API SQLGetDiagField(SQLSMALLINT handletype,
					SQLHANDLE handle,
					SQLSMALLINT recnumber,
					SQLSMALLINT diagidentifier,
					SQLPOINTER diaginfo,
					SQLSMALLINT bufferlength,
					SQLSMALLINT *stringlength) {
	debugFunction();

	debugPrintf("  recnumber: %d\n",(int)recnumber);

	// SQL Relay doesn't have more than 1 error record
	if (recnumber>1) {
		return SQL_NO_DATA;
	}

	union {
		const char	*strval;
		SQLLEN		lenval;
	} val;
	int16_t	type=-1;

	switch (handletype) {
		case SQL_HANDLE_ENV:
			{
			debugPrintf("  handletype: SQL_HANDLE_ENV\n");
			ENV	*env=(ENV *)handle;
			if (handle==SQL_NULL_HENV || !env) {
				debugPrintf("  NULL env handle\n");
				return SQL_INVALID_HANDLE;
			}
			// nothing currently supported
			debugPrintf("  diagidentifier: %d (unsupported)\n",
								diagidentifier);
			return SQL_NO_DATA;
			}
		case SQL_HANDLE_DBC:
			{
			debugPrintf("  handletype: SQL_HANDLE_DBC\n");

			// invalid handle...
			CONN	*conn=(CONN *)handle;
			if (handle==SQL_NULL_HSTMT || !conn) {
				debugPrintf("  NULL conn handle\n");
				return SQL_INVALID_HANDLE;
			}

			// get the requested data
			switch (diagidentifier) {
				case SQL_DIAG_CLASS_ORIGIN:
					debugPrintf("  diagidentifier: "
						"SQL_DIAG_CLASS_ORIGIN\n");
					debugPrintf("  sqlstate: %s\n",
							conn->sqlstate);
					if (!charstring::compare(
						conn->sqlstate,"IM",2)) {
						val.strval="ODBC 3.0";
					} else {
						val.strval="ISO 9075";
					}
					type=0;
					break;
				case SQL_DIAG_SUBCLASS_ORIGIN:
					debugPrintf("  diagidentifier: "
						"SQL_DIAG_SUBCLASS_ORIGIN\n");
					debugPrintf("  sqlstate: %s\n",
							conn->sqlstate);
					if (charstring::inSet(
							conn->sqlstate,
							odbc3states)) {
						val.strval="ODBC 3.0";
					} else {
						val.strval="ISO 9075";
					}
					type=0;
					break;
				case SQL_DIAG_CONNECTION_NAME:
					debugPrintf("  diagidentifier: "
						"SQL_DIAG_CONNECTION_NAME\n");
					// return the server name for this too
					val.strval=conn->server;
					type=0;
					break;
				case SQL_DIAG_SERVER_NAME:
					debugPrintf("  diagidentifier: "
						"SQL_DIAG_SERVER_NAME\n");
					val.strval=conn->server;
					type=0;
					break;
				default:
					// anything else is not supported
					debugPrintf("  diagidentifier: %d "
							"(unsupported)\n",
							diagidentifier);
					return SQL_NO_DATA;
			}
			}
			break;
		case SQL_HANDLE_STMT:
			{
			debugPrintf("  handletype: SQL_HANDLE_STMT\n");
			STMT	*stmt=(STMT *)handle;
			if (handle==SQL_NULL_HSTMT || !stmt) {
				debugPrintf("  NULL stmt handle\n");
				return SQL_INVALID_HANDLE;
			}
			switch (diagidentifier) {
				case SQL_DIAG_ROW_COUNT:
					debugPrintf("  diagidentifier: "
						"SQL_DIAG_ROW_COUNT: %lld\n",
						stmt->cur->affectedRows());
					val.lenval=stmt->cur->affectedRows();
					type=1;
					break;
				case SQL_DIAG_SQLSTATE:
					debugPrintf("  diagidentifier: "
						"SQL_DIAG_SQLSTATE: %s\n",
						stmt->sqlstate);
					val.strval=stmt->sqlstate;
					type=0;
					break;
				case SQL_DIAG_CURSOR_ROW_COUNT:
					val.lenval=
						stmt->cur->rowCount()-
						stmt->cur->firstRowIndex();
					debugPrintf("  diagidentifier: "
						"SQL_CURSOR_DIAG_ROW_COUNT: "
							"%lld\n",val.lenval);
					type=1;
					break;
				default:
					// anything else is not supported
					debugPrintf("  diagidentifier: %d "
							"(unsupported)\n",
							diagidentifier);
					return SQL_NO_DATA;
			}
			}
			break;
		case SQL_HANDLE_DESC:
			debugPrintf("  handletype: SQL_HANDLE_DESC\n");
			debugPrintf("  diagidentifier: %d (unsupported)\n",
								diagidentifier);
			// not supported
			return SQL_NO_DATA;
		default:
			debugPrintf("  invalid handletype\n");
			return SQL_ERROR;
	}


	debugPrintf("  bufferlength: %d\n",(int)bufferlength);

	// copy out the value and length
	SQLSMALLINT	valuelength=0;
	switch (type) {
		case -1:
			debugPrintf("  (not copying out any value)\n");
			break;
		case 0:
			debugPrintf("  strval: %s\n",val.strval);
			valuelength=charstring::length(val.strval);
			if (diaginfo && bufferlength) {

				charstring::safeCopy((char *)diaginfo,
							bufferlength,
							val.strval);

				// make sure to null-terminate
				// (even if data has to be truncated)
				((char *)diaginfo)[bufferlength-1]='\0';

				if (valuelength>bufferlength) {
					debugPrintf("  WARNING! valuelength>"
							"bufferlength\n");
				}
			} else {
				if (!diaginfo) {
					debugPrintf("  NULL diaginfo or "
						"(not copying out strval)\n");
				}
				if (!bufferlength) {
					debugPrintf("  0 bufferlength "
						"(not copying out strval)\n");
				}
			}
			break;
		case 1:
			debugPrintf("  lenval: %d\n",val.lenval);
			valuelength=sizeof(SQLLEN);
			if (diaginfo) {
				*((SQLLEN *)diaginfo)=val.lenval;
			} else {
				debugPrintf("  NULL diaginfo "
						"(not copying out lenval)\n");
			}
			break;
	}
	debugPrintf("  valuelength: %d\n",(int)valuelength);
	if (stringlength) {
		*stringlength=valuelength;
	} else {
		debugPrintf("  NULL stringlength "
					"(not copying out valuelength)\n");
	}

	return SQL_SUCCESS;
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

	debugPrintf("  recnumber: %d\n",(int)recnumber);

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
			debugPrintf("  handletype: SQL_HANDLE_ENV\n");
			ENV	*env=(ENV *)handle;
			if (handle==SQL_NULL_HSTMT || !env) {
				debugPrintf("  NULL env handle\n");
				return SQL_INVALID_HANDLE;
			}
			error=env->error;
			errn=env->errn;
			sqlst=env->sqlstate;
			}
			break;
		case SQL_HANDLE_DBC:
			{
			debugPrintf("  handletype: SQL_HANDLE_DBC\n");
			CONN	*conn=(CONN *)handle;
			if (handle==SQL_NULL_HSTMT || !conn) {
				debugPrintf("  NULL conn handle\n");
				return SQL_INVALID_HANDLE;
			}
			error=conn->error;
			errn=conn->errn;
			sqlst=conn->sqlstate;
			}
			break;
		case SQL_HANDLE_STMT:
			{
			debugPrintf("  handletype: SQL_HANDLE_STMT\n");
			STMT	*stmt=(STMT *)handle;
			if (handle==SQL_NULL_HSTMT || !stmt) {
				debugPrintf("  NULL stmt handle\n");
				return SQL_INVALID_HANDLE;
			}
			error=stmt->error;
			errn=stmt->errn;
			sqlst=stmt->sqlstate;
			}
			break;
		case SQL_HANDLE_DESC:
			debugPrintf("  handletype: SQL_HANDLE_DESC\n");
			// not supported
			return SQL_ERROR;
		default:
			debugPrintf("  invalid handletype\n");
			return SQL_ERROR;
	}

	// finagle sqlst
	if (charstring::isNullOrEmpty(sqlst)) {
		if (!charstring::isNullOrEmpty(error)) {
			// General error
			sqlst="HY000";
		} else {
			// success
			sqlst="00000";
		}
	}

	// copy out the data
	if (nativeerror) {
		*nativeerror=errn;
		debugPrintf("  nativeerror: %lld\n",(int64_t)*nativeerror);
	} else {
		debugPrintf("  NULL nativerror "
				"(not copying out: %lld)\n",(int64_t)errn);
	}
	if (sqlstate) {
		charstring::copy((char *)sqlstate,sqlst);
		debugPrintf("  sqlstate: %s\n",
				((char *)sqlstate)?(char *)sqlstate:"");
	} else {
		debugPrintf("  NULL sqlstate "
				"(not copying out: %s)\n",(sqlst)?sqlst:"");
	}

	SQLSMALLINT	valuelength=charstring::length(error);
	if (messagetext && bufferlength) {

		charstring::safeCopy((char *)messagetext,
					(size_t)bufferlength,
					error);
		valuelength=charstring::length(messagetext);

		// make sure to null-terminate
		// (even if data has to be truncated)
		((char *)messagetext)[bufferlength-1]='\0';

		if (valuelength>bufferlength) {
			debugPrintf("  WARNING! valuelength>textlength\n");
		}

		debugPrintf("  messagetext: %s\n",messagetext);
	} else {
		debugPrintf("  NULL value or 0 bufferlength "
					"(not copying out: %s)\n",error);
	}
	if (textlength) {
		*textlength=valuelength;
		debugPrintf("  textlength: %d\n",(int)*textlength);
	} else {
		debugPrintf("  NULL textlength (not copying out: %d\n",
								valuelength);
	}

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
		debugPrintf("  NULL env handle\n");
		return SQL_INVALID_HANDLE;
	}

	SQLUINTEGER	val;

	switch (attribute) {
		case SQL_ATTR_OUTPUT_NTS:
			debugPrintf("  attribute: "
					"SQL_ATTR_OUTPUT_NTS\n");
			// this one is hardcoded to true
			// and can't be set to false
			val=SQL_TRUE;
			break;
		case SQL_ATTR_ODBC_VERSION:
			debugPrintf("  attribute: "
					"SQL_ATTR_ODBC_VERSION\n");
			val=env->odbcversion;
			debugPrintf("    odbcversion: %d\n",
						(int)env->odbcversion);
			break;
		case SQL_ATTR_CONNECTION_POOLING:
			debugPrintf("  attribute: "
					"SQL_ATTR_CONNECTION_POOLING\n");
			// this one is hardcoded to "off"
			// and can't be changed
			val=SQL_CP_OFF;
			break;
		case SQL_ATTR_CP_MATCH:
			debugPrintf("  attribute: "
					"SQL_ATTR_CP_MATCH\n");
			// this one is hardcoded to "default"
			// and can't be changed
			val=SQL_CP_MATCH_DEFAULT;
			break;
		default:
			debugPrintf("  invalid attribute: %d\n",attribute);
			SQLR_ENVSetError(env,
				"Optional field not implemented",0,"HYC00");
			return SQL_ERROR;
			break;
	}


	// copy out the value and length
	SQLSMALLINT	valuelength=sizeof(SQLUINTEGER);
	if (value) {
		debugPrintf("  uintval: %d\n",val);
		*((SQLUINTEGER *)value)=val;
	} else {
		debugPrintf("  NULL value (not copying out uintval)\n");
	}
	debugPrintf("  valuelength: %d\n",(int)valuelength);
	if (stringlength) {
		*stringlength=valuelength;
	} else {
		debugPrintf("  NULL stringlength "
					"(not copying out valuelength)\n");
	}

	return SQL_SUCCESS;
}

static SQLRETURN SQLR_SQLGetFunctions(SQLHDBC connectionhandle,
					SQLUSMALLINT functionid,
					SQLUSMALLINT *supported) {
	debugFunction();

	CONN	*conn=(CONN *)connectionhandle;
	if (connectionhandle==SQL_NULL_HANDLE || !conn || !conn->con) {
		debugPrintf("  NULL conn handle\n");
		return SQL_INVALID_HANDLE;
	}

	switch (functionid) {
		case SQL_API_ALL_FUNCTIONS:
			debugPrintf("  functionid: "
				"SQL_API_ALL_FUNCTIONS "
				"- true\n");

			for (uint16_t i=0; i<100; i++) {
				if (i==SQL_API_ALL_FUNCTIONS
					#if (ODBCVER >= 0x0300)
					|| i==SQL_API_ODBC3_ALL_FUNCTIONS
					#endif
					) {
					supported[i]=SQL_TRUE;
				} else {
					SQLR_SQLGetFunctions(
							connectionhandle,
							i,&supported[i]);
				}
			}

			// clear any error that might have been set during
			// the recursive call
			SQLR_CONNClearError(conn);

			break;
		case SQL_API_SQLALLOCCONNECT:
			debugPrintf("  functionid: "
				"SQL_API_SQLALLOCCONNECT "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		case SQL_API_SQLALLOCENV:
			debugPrintf("  functionid: "
				"SQL_API_SQLALLOCENV "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		#if (ODBCVER >= 0x0300)
		case SQL_API_SQLALLOCHANDLE:
			debugPrintf("  functionid: "
				"SQL_API_SQLALLOCHANDLE "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		#endif
		case SQL_API_SQLALLOCSTMT:
			debugPrintf("  functionid: "
				"SQL_API_SQLALLOCSTMT "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		case SQL_API_SQLBINDCOL:
			debugPrintf("  functionid: "
				"SQL_API_SQLBINDCOL "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		#if (ODBCVER >= 0x0300)
		case SQL_API_SQLBINDPARAM:
			debugPrintf("  functionid: "
				"SQL_API_SQLBINDPARAM "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		case SQL_API_SQLCLOSECURSOR:
			debugPrintf("  functionid: "
				"SQL_API_SQLCLOSECURSOR "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		case SQL_API_SQLCOLATTRIBUTE:
		//case SQL_API_SQLCOLATTRIBUTES:
			debugPrintf("  functionid: "
				"SQL_API_SQLCOLATTRIBUTE "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		#endif
		case SQL_API_SQLCONNECT:
			debugPrintf("  functionid: "
				"SQL_API_SQLCONNECT "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		#if (ODBCVER >= 0x0300)
		case SQL_API_SQLCOPYDESC:
			debugPrintf("  functionid: "
				"SQL_API_SQLCOPYDESC "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		#endif
		case SQL_API_SQLDESCRIBECOL:
			debugPrintf("  functionid: "
				"SQL_API_SQLDESCRIBECOL "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		case SQL_API_SQLDISCONNECT:
			debugPrintf("  functionid: "
				"SQL_API_SQLDISCONNECT "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		#if (ODBCVER >= 0x0300)
		case SQL_API_SQLENDTRAN:
			debugPrintf("  functionid: "
				"SQL_API_SQLENDTRAN "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		#endif
		case SQL_API_SQLERROR:
			debugPrintf("  functionid: "
				"SQL_API_SQLERROR "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		case SQL_API_SQLEXECDIRECT:
			debugPrintf("  functionid: "
				"SQL_API_SQLEXECDIRECT "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		case SQL_API_SQLEXECUTE:
			debugPrintf("  functionid: "
				"SQL_API_SQLEXECUTE "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		case SQL_API_SQLFETCH:
			debugPrintf("  functionid: "
				"SQL_API_SQLFETCH "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		#if (ODBCVER >= 0x0300)
		case SQL_API_SQLFETCHSCROLL:
			debugPrintf("  functionid: "
				"SQL_API_SQLFETCHSCROLL "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		#endif
		case SQL_API_SQLFREECONNECT:
			debugPrintf("  functionid: "
				"SQL_API_SQLFREECONNECT "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		case SQL_API_SQLFREEENV:
			debugPrintf("  functionid: "
				"SQL_API_SQLFREEENV "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		#if (ODBCVER >= 0x0300)
		case SQL_API_SQLFREEHANDLE:
			debugPrintf("  functionid: "
				"SQL_API_SQLFREEHANDLE "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		#endif
		case SQL_API_SQLFREESTMT:
			debugPrintf("  functionid: "
				"SQL_API_SQLFREESTMT "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		case SQL_API_SQLGETCONNECTOPTION:
			debugPrintf("  functionid: "
				"SQL_API_SQLGETCONNECTOPTION "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		case SQL_API_SQLGETCURSORNAME:
			debugPrintf("  functionid: "
				"SQL_API_SQLGETCURSORNAME "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		case SQL_API_SQLGETDATA:
			debugPrintf("  functionid: "
				"SQL_API_SQLGETDATA "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		#if (ODBCVER >= 0x0300)
		case SQL_API_SQLGETDIAGFIELD:
			debugPrintf("  functionid: "
				"SQL_API_SQLGETDIAGFIELD "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		case SQL_API_SQLGETDIAGREC:
			debugPrintf("  functionid: "
				"SQL_API_SQLGETDIAGREC "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		case SQL_API_SQLGETENVATTR:
			debugPrintf("  functionid: "
				"SQL_API_SQLGETENVATTR "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		#endif
		case SQL_API_SQLGETFUNCTIONS:
			debugPrintf("  functionid: "
				"SQL_API_SQLGETFUNCTIONS "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		case SQL_API_SQLGETINFO:
			debugPrintf("  functionid: "
				"SQL_API_SQLGETINFO "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		#if (ODBCVER >= 0x0300)
		case SQL_API_SQLGETSTMTATTR:
			debugPrintf("  functionid: "
				"SQL_API_SQLGETSTMTATTR "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		#endif
		case SQL_API_SQLGETSTMTOPTION:
			debugPrintf("  functionid: "
				"SQL_API_SQLGETSTMTOPTION "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		case SQL_API_SQLGETTYPEINFO:
			debugPrintf("  functionid: "
				"SQL_API_SQLGETTYPEINFO "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		case SQL_API_SQLNUMRESULTCOLS:
			debugPrintf("  functionid: "
				"SQL_API_SQLNUMRESULTCOLS "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		case SQL_API_SQLPREPARE:
			debugPrintf("  functionid: "
				"SQL_API_SQLPREPARE "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		case SQL_API_SQLROWCOUNT:
			debugPrintf("  functionid: "
				"SQL_API_SQLROWCOUNT "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		#if (ODBCVER >= 0x0300)
		case SQL_API_SQLSETCONNECTATTR:
			debugPrintf("  functionid: "
				"SQL_API_SQLSETCONNECTATTR "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		#endif
		case SQL_API_SQLSETCONNECTOPTION:
			debugPrintf("  functionid: "
				"SQL_API_SQLSETCONNECTOPTION "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		case SQL_API_SQLSETCURSORNAME:
			debugPrintf("  functionid: "
				"SQL_API_SQLSETCURSORNAME "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		#if (ODBCVER >= 0x0300)
		case SQL_API_SQLSETENVATTR:
			debugPrintf("  functionid: "
				"SQL_API_SQLSETENVATTR "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		#endif
		case SQL_API_SQLSETPARAM:
			debugPrintf("  functionid: "
				"SQL_API_SQLSETPARAM "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		#if (ODBCVER >= 0x0300)
		case SQL_API_SQLSETSTMTATTR:
			debugPrintf("  functionid: "
				"SQL_API_SQLSETSTMTATTR "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		#endif
		case SQL_API_SQLSETSTMTOPTION:
			debugPrintf("  functionid: "
				"SQL_API_SQLSETSTMTOPTION "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		case SQL_API_SQLTRANSACT:
			debugPrintf("  functionid: "
				"SQL_API_SQLTRANSACT "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		#if (ODBCVER >= 0x0300)
		case SQL_API_SQLALLOCHANDLESTD:
			debugPrintf("  functionid: "
				"SQL_API_SQLALLOCHANDLESTD "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		#endif
		case SQL_API_SQLBINDPARAMETER:
			debugPrintf("  functionid: "
				"SQL_API_SQLBINDPARAMETER "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		case SQL_API_SQLBROWSECONNECT:
			debugPrintf("  functionid: "
				"SQL_API_SQLBROWSECONNECT "
				"- false\n");
			*supported=SQL_FALSE;
			break;
		case SQL_API_SQLDRIVERCONNECT:
			debugPrintf("  functionid: "
				"SQL_API_SQLDRIVERCONNECT "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		case SQL_API_SQLEXTENDEDFETCH:
			debugPrintf("  functionid: "
				"SQL_API_SQLEXTENDEDFETCH "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		case SQL_API_SQLMORERESULTS:
			debugPrintf("  functionid: "
				"SQL_API_SQLMORERESULTS "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		case SQL_API_SQLNUMPARAMS:
			debugPrintf("  functionid: "
				"SQL_API_SQLNUMPARAMS "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		case SQL_API_SQLPARAMOPTIONS:
			debugPrintf("  functionid: "
				"SQL_API_SQLPARAMOPTIONS "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		case SQL_API_SQLSETPOS:
			debugPrintf("  functionid: "
				"SQL_API_SQLSETPOS "
				"- false\n");
			// FIXME: this is implemented, sort-of...
			*supported=SQL_FALSE;
			break;
		case SQL_API_SQLSETSCROLLOPTIONS:
			debugPrintf("  functionid: "
				"SQL_API_SQLSETSCROLLOPTIONS "
				"- false\n");
			*supported=SQL_FALSE;
			break;
		case SQL_API_SQLCANCEL:
			debugPrintf("  functionid: "
				"SQL_API_SQLCANCEL "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		case SQL_API_SQLCOLUMNS:
			debugPrintf("  functionid: "
				"SQL_API_SQLCOLUMNS "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		case SQL_API_SQLDATASOURCES:
			debugPrintf("  functionid: "
				"SQL_API_SQLDATASOURCES "
				"- false\n");
			*supported=SQL_FALSE;
			break;
		case SQL_API_SQLPUTDATA:
			debugPrintf("  functionid: "
				"SQL_API_SQLPUTDATA "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		case SQL_API_SQLPARAMDATA:
			debugPrintf("  functionid: "
				"SQL_API_SQLPARAMDATA "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		#if (ODBCVER >= 0x0300)
		case SQL_API_SQLGETCONNECTATTR:
			debugPrintf("  functionid: "
				"SQL_API_SQLGETCONNECTATTR "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		#endif
		case SQL_API_SQLSPECIALCOLUMNS:
			debugPrintf("  functionid: "
				"SQL_API_SQLSPECIALCOLUMNS "
				"- false\n");
			*supported=SQL_FALSE;
			break;
		case SQL_API_SQLSTATISTICS:
			debugPrintf("  functionid: "
				"SQL_API_SQLSTATISTICS "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		case SQL_API_SQLTABLES:
			debugPrintf("  functionid: "
				"SQL_API_SQLTABLES "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		#if (ODBCVER >= 0x0300)
		case SQL_API_SQLBULKOPERATIONS:
			debugPrintf("  functionid: "
				"SQL_API_SQLBULKOPERATIONS "
				"- false\n");
			*supported=SQL_FALSE;
			break;
		#endif
		case SQL_API_SQLCOLUMNPRIVILEGES:
			debugPrintf("  functionid: "
				"SQL_API_SQLCOLUMNPRIVILEGES "
				"- false\n");
			*supported=SQL_FALSE;
			break;
		case SQL_API_SQLDESCRIBEPARAM:
			debugPrintf("  functionid: "
				"SQL_API_SQLDESCRIBEPARAM "
				"- false\n");
			*supported=SQL_FALSE;
			break;
		case SQL_API_SQLFOREIGNKEYS:
			debugPrintf("  functionid: "
				"SQL_API_SQLFOREIGNKEYS "
				"- false\n");
			*supported=SQL_FALSE;
			break;
		case SQL_API_SQLNATIVESQL:
			debugPrintf("  functionid: "
				"SQL_API_SQLNATIVESQL "
				"- false\n");
			*supported=SQL_FALSE;
			break;
		case SQL_API_SQLPRIMARYKEYS:
			debugPrintf("  functionid: "
				"SQL_API_SQLPRIMARYKEYS "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		case SQL_API_SQLPROCEDURECOLUMNS:
			debugPrintf("  functionid: "
				"SQL_API_SQLPROCEDURECOLUMNS "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		case SQL_API_SQLPROCEDURES:
			debugPrintf("  functionid: "
				"SQL_API_SQLPROCEDURES "
				"- true\n");
			*supported=SQL_TRUE;
			break;
		case SQL_API_SQLTABLEPRIVILEGES:
			debugPrintf("  functionid: "
				"SQL_API_SQLTABLEPRIVILEGES "
				"- false\n");
			*supported=SQL_FALSE;
			break;
		case SQL_API_SQLDRIVERS:
			debugPrintf("  functionid: "
				"SQL_API_SQLDRIVERS "
				"- true (provided by driver manager)\n");
			*supported=SQL_TRUE;
			break;
		#if (ODBCVER >= 0x0300)
		case SQL_API_SQLGETDESCFIELD:
			debugPrintf("  functionid: "
				"SQL_API_SQLGETDESCFIELD "
				"- false\n");
			*supported=SQL_FALSE;
			break;
		case SQL_API_SQLGETDESCREC:
			debugPrintf("  functionid: "
				"SQL_API_SQLGETDESCREC "
				"- false\n");
			*supported=SQL_FALSE;
			break;
		case SQL_API_SQLSETDESCFIELD:
			debugPrintf("  functionid: "
				"SQL_API_SQLSETDESCFIELD "
				"- false\n");
			*supported=SQL_FALSE;
			break;
		case SQL_API_SQLSETDESCREC:
			debugPrintf("  functionid: "
				"SQL_API_SQLSETDESCREC "
				"- false\n");
			*supported=SQL_FALSE;
			break;
		case SQL_API_ODBC3_ALL_FUNCTIONS:
			debugPrintf("  functionid: "
				"SQL_API_ODBC3_ALL_FUNCTIONS "
				"- true\n");

			// populate the bitmap...
			for (uint16_t i=0;
				i<SQL_API_ODBC3_ALL_FUNCTIONS_SIZE*16; i++) {

				// determine the bitmap element
				// and position within the element
				uint16_t	element=i/16;
				uint16_t	position=i%16;

				// init the bitmap element
				if (!position) {
					supported[element]=0;
				}

				// is this function supported?
				SQLUSMALLINT	sup=SQL_FALSE;
				if (i==SQL_API_ALL_FUNCTIONS ||
					i==SQL_API_ODBC3_ALL_FUNCTIONS) {
					sup=SQL_TRUE;
				} else {
					SQLR_SQLGetFunctions(
							connectionhandle,
							i,&sup);
				}

				// update the bitmap
				supported[element]|=sup<<position;

				// debug...
				debugPrintf("%d(%d:%d) = %d  ",
						i,element,position,sup);
				debugPrintf("(%d = ",element);
				debugPrintBits((uint16_t)supported[element]);
				debugPrintf(")\n");
			}
			// clear any error that might have been set during
			// the recursive call
			SQLR_CONNClearError(conn);
			break;
		#endif
		#if (ODBCVER >= 0x0380)
		case SQL_API_SQLCANCELHANDLE:
			debugPrintf("  functionid: "
				"SQL_API_SQLCANCELHANDLE "
				"- false\n");
			*supported=SQL_FALSE;
			break;
		#endif
		default:
			debugPrintf("  invalid functionid: %d\n",functionid);
			*supported=SQL_FALSE;
			SQLR_CONNSetError(conn,"Function type out of range",
								0,"HY095");
			return SQL_ERROR;
	}

	return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLGetFunctions(SQLHDBC connectionhandle,
					SQLUSMALLINT functionid,
					SQLUSMALLINT *supported) {
	debugFunction();
	return SQLR_SQLGetFunctions(connectionhandle,functionid,supported);
}

SQLRETURN SQL_API SQLGetInfo(SQLHDBC connectionhandle,
					SQLUSMALLINT infotype,
					SQLPOINTER infovalue,
					SQLSMALLINT bufferlength,
					SQLSMALLINT *stringlength) {
	debugFunction();

	// some bits of info need a valid conn handle, but others don't
	CONN	*conn=(CONN *)connectionhandle;
	if ((connectionhandle==SQL_NULL_HANDLE || !conn || !conn->con) &&
		(infotype==SQL_DATA_SOURCE_NAME ||
			infotype==SQL_SERVER_NAME ||
			infotype==SQL_DRIVER_VER ||
			infotype==SQL_DBMS_NAME ||
			infotype==SQL_DBMS_VER ||
			infotype==SQL_ODBC_VER ||
			infotype==SQL_DATABASE_NAME ||
			infotype==SQL_USER_NAME)) {
		debugPrintf("  NULL conn handle\n");
		return SQL_INVALID_HANDLE;
	}

	union {
		const char	*strval;
		SQLUINTEGER	uintval;
		SQLUSMALLINT	usmallintval;
	} val;
	int16_t	type=-1;

	switch (infotype) {
		case SQL_ACTIVE_CONNECTIONS:
			// aka SQL_MAX_DRIVER_CONNECTIONS
			// aka SQL_MAXIMUM_DRIVER_CONNECTIONS
			debugPrintf("  infotype: "
					"SQL_ACTIVE_CONNECTIONS/"
					"SQL_MAX_DRIVER_CONNECTIONS/"
					"SQL_MAXIMUM_DRIVER_CONNECTIONS\n");
			// 0 means no max or unknown
			val.usmallintval=0;
			type=2;
			break;
		case SQL_ACTIVE_STATEMENTS:
			// aka SQL_MAX_CONCURRENT_ACTIVITIES 
			// aka SQL_MAXIMUM_CONCURRENT_ACTIVITIES
			debugPrintf("  infotype: "
					"SQL_ACTIVE_STATEMENTS/"
					"SQL_MAX_CONCURRENT_ACTIVITIES/"
					"SQL_MAXIMUM_CONCURRENT_ACTIVITIES\n");
			// 0 means no max or unknown
			val.usmallintval=0;
			type=2;
			break;
		case SQL_DATA_SOURCE_NAME:
			debugPrintf("  infotype: "
					"SQL_DATA_SOURCE_NAME\n");
			val.strval=conn->dsn;
			type=0;
			break;
		case SQL_FETCH_DIRECTION:
			debugPrintf("  infotype: "
					"SQL_FETCH_DIRECTION\n");
			// FIXME: for now...
			val.uintval=SQL_FD_FETCH_NEXT;
			type=1;
			break;
		case SQL_SERVER_NAME:
			debugPrintf("  infotype: "
					"SQL_SERVER_NAME\n");
			val.strval=conn->server;
			type=0;
			break;
		case SQL_SEARCH_PATTERN_ESCAPE:
			debugPrintf("  infotype: "
					"SQL_SEARCH_PATTERN_ESCAPE\n");
			val.strval="\\";
			type=0;
			break;
		case SQL_DATABASE_NAME:
			debugPrintf("  infotype: "
					"SQL_DATABASE_NAME\n");
			val.strval=conn->con->getCurrentDatabase();
			type=0;
			break;
		case SQL_DBMS_NAME:
			debugPrintf("  infotype: "
					"SQL_DBMS_NAME\n");
			val.strval=conn->con->identify();
			type=0;
			break;
		case SQL_DBMS_VER:
			debugPrintf("  infotype: "
					"SQL_DBMS_VER\n");
			val.strval=conn->con->dbVersion();
			type=0;
			break;
		case SQL_ACCESSIBLE_TABLES:
			debugPrintf("  infotype: "
					"SQL_ACCESSIBLE_TABLES\n");
			val.strval="N";
			type=0;
			break;
		case SQL_ACCESSIBLE_PROCEDURES:
			debugPrintf("  infotype: "
					"SQL_ACCESSIBLE_PROCEDURES\n");
			val.strval="N";
			type=0;
			break;
		case SQL_CURSOR_COMMIT_BEHAVIOR:
			debugPrintf("  infotype: "
					"SQL_CURSOR_COMMIT_BEHAVIOR\n");
			// FIXME: is this true for all db's?
			val.usmallintval=SQL_CB_CLOSE;
			type=2;
			break;
		case SQL_DATA_SOURCE_READ_ONLY:
			debugPrintf("  infotype: "
					"SQL_DATA_SOURCE_READ_ONLY\n");
			// FIXME: this isn't always true
			val.strval="N";
			type=0;
			break;
		case SQL_DEFAULT_TXN_ISOLATION:
			debugPrintf("  infotype: "
					"SQL_DEFAULT_TXN_ISOLATION\n");
			// FIXME: this isn't always true, especially for mysql
			val.uintval=SQL_TXN_READ_COMMITTED;
			type=1;
			break;
		case SQL_IDENTIFIER_CASE:
			debugPrintf("  infotype: "
					"SQL_IDENTIFIER_CASE\n");
			// FIXME: this isn't true for all db's
			val.usmallintval=SQL_IC_MIXED;
			type=2;
			break;
		case SQL_IDENTIFIER_QUOTE_CHAR:
			debugPrintf("  infotype: "
					"SQL_IDENTIFIER_QUOTE_CHAR\n");
			if (!charstring::compare(conn->con->identify(),
								"mysql")) {
				// mysql uses a back-tick
				val.strval="`";
			} else {
				// SQL-92 defines " as the quote char,
				// which most db's support, so fall back to
				// that
				val.strval="\"";
			}
			type=0;
			break;
		case SQL_MAX_COLUMN_NAME_LEN:
			// aka SQL_MAXIMUM_COLUMN_NAME_LENGTH
			debugPrintf("  infotype: "
					"SQL_MAX_COLUMN_NAME_LEN/"
					"SQL_MAXIMUM_COLUMN_NAME_LEN\n");
			// 0 means no max or unknown
			// FIXME: FIPS intermediate level returns >= 128
			val.usmallintval=0;
			type=2;
			break;
		case SQL_MAX_CURSOR_NAME_LEN:
			// aka SQL_MAXIMUM_CURSOR_NAME_LENGTH
			debugPrintf("  infotype: "
					"SQL_MAX_CURSOR_NAME_LEN/"
					"SQL_MAXIMUM_CURSOR_NAME_LEN\n");
			// 0 means no max or unknown
			// FIXME: FIPS intermediate level returns >= 128
			val.usmallintval=0;
			type=2;
			break;
		case SQL_MAX_OWNER_NAME_LEN:
			// aka SQL_MAX_SCHEMA_NAME_LEN
			// aka SQL_MAXIMUM_SCHEMA_NAME_LENGTH
			debugPrintf("  infotype: "
					"SQL_MAX_OWNER_NAME_LEN/"
					"SQL_MAX_SCHEMA_NAME_LEN/"
					"SQL_MAXIMUM_SCHEMA_NAME_LEN\n");
			// 0 means no max or unknown, which is the case.
			// But, returning 0 causes some apps (Delphi):
			// * not to call SQLGetInfo(SQL_USER_NAME)
			// * thus not to recognize the schema name returned by
			// 	SQLTables as the schema
			// * thus to build and quote table names like "dbo.tbl"
			// 	rather than "dbo"."tbl", which fails.
			// Hilariously, Delphi doesn't actually use this value
			// to size the schema-name buffer.  It just doesn't
			// call SQLGetInfo(SQL_USER_NAME) if it returns 0.
			val.usmallintval=128;
			type=2;
			break;
		case SQL_MAX_CATALOG_NAME_LEN:
			// aka SQL_MAXIMUM_CATALOG_NAME_LENGTH
			debugPrintf("  infotype: "
					"SQL_MAX_CATALOG_NAME_LEN\n");
			// 0 means no max or unknown
			// FIXME: FIPS intermediate level returns >= 128
			val.usmallintval=0;
			type=2;
			break;
		case SQL_MAX_TABLE_NAME_LEN:
			debugPrintf("  infotype: "
					"SQL_MAX_TABLE_NAME_LEN\n");
			// 0 means no max or unknown
			// FIXME: FIPS entry level returns >= 18
			// FIXME: FIPS intermediate level returns >= 128
			val.usmallintval=0;
			type=2;
			break;
		case SQL_SCROLL_CONCURRENCY:
			debugPrintf("  infotype: "
					"SQL_SCROLL_CONCURRENCY\n");
			val.uintval=SQL_SCCO_READ_ONLY;
			type=1;
			break;
		case SQL_TXN_CAPABLE:
			// aka SQL_TRANSACTION_CAPABLE
			debugPrintf("  infotype: "
					"SQL_TXN_CAPABLE\n");
			// FIXME: this isn't true for all db's
			val.usmallintval=SQL_TC_ALL;
			type=2;
			break;
		case SQL_USER_NAME:
			debugPrintf("  infotype: "
					"SQL_USER_NAME\n");
			// Really, when an app calls this, they usually
			// want the schema, not user.  In most databases,
			// there is 1 schema per user, so they are synonymous,
			// but not in all (MS SQL Server).
			val.strval=conn->con->getCurrentSchema();
			// This isn't currently (as of 1.2.0) implemented
			// for all connection modules, so we'll fall back to
			// conn->user if we don't get anything for the schema,
			// which was how this was implemented prior to 1.2.0
			// anyway.
			if (charstring::isNullOrEmpty(val.strval)) {
				val.strval=conn->user;
			}
			type=0;
			break;
		case SQL_TXN_ISOLATION_OPTION:
			// aka SQL_TRANSACTION_ISOLATION_OPTION
			debugPrintf("  infotype: "
					"SQL_TXN_ISOLATION_OPTION/"
					"SQL_TRANSACTION_ISOLATION_OPTION\n");
			// FIXME: this isn't true for all db's
			val.uintval=SQL_TXN_READ_UNCOMMITTED|
					SQL_TXN_READ_COMMITTED|
					SQL_TXN_REPEATABLE_READ|
					SQL_TXN_SERIALIZABLE;
			type=1;
			break;
		case SQL_INTEGRITY:
			// aka SQL_ODBC_SQL_OPT_IEF
			debugPrintf("  infotype: "
					"SQL_INTEGRITY/"
					"SQL_ODBC_SQL_OPT_IEF\n");
			// FIXME: this isn't true for all db's
			val.strval="Y";
			type=0;
			break;
		case SQL_GETDATA_EXTENSIONS:
			debugPrintf("  infotype: "
					"SQL_GETDATA_EXTENSIONS\n");
			val.uintval=SQL_GD_BLOCK;
			type=1;
			break;
		case SQL_NULL_COLLATION:
			debugPrintf("  infotype: "
					"SQL_NULL_COLLATION\n");
			// FIXME: is this true for all db's?
			val.usmallintval=SQL_NC_LOW;
			type=2;
			break;
		case SQL_ALTER_TABLE:
			debugPrintf("  infotype: "
					"SQL_ALTER_TABLE\n");
			// FIXME: this isn't true for all db's
			val.uintval=0
				#if (ODBCVER >= 0x0200)
				|SQL_AT_ADD_COLUMN
				|SQL_AT_DROP_COLUMN
				#endif
				#if (ODBCVER >= 0x0300)
				|SQL_AT_ADD_COLUMN_SINGLE
				|SQL_AT_ADD_COLUMN_DEFAULT
				|SQL_AT_ADD_COLUMN_COLLATION
				|SQL_AT_SET_COLUMN_DEFAULT
				|SQL_AT_DROP_COLUMN_DEFAULT
				|SQL_AT_DROP_COLUMN_CASCADE
				|SQL_AT_DROP_COLUMN_RESTRICT
				|SQL_AT_ADD_TABLE_CONSTRAINT
				|SQL_AT_DROP_TABLE_CONSTRAINT_CASCADE
				|SQL_AT_DROP_TABLE_CONSTRAINT_RESTRICT
				|SQL_AT_CONSTRAINT_NAME_DEFINITION
				|SQL_AT_CONSTRAINT_INITIALLY_DEFERRED
				|SQL_AT_CONSTRAINT_INITIALLY_IMMEDIATE
				|SQL_AT_CONSTRAINT_DEFERRABLE
				|SQL_AT_CONSTRAINT_NON_DEFERRABLE
				#endif
				;
			type=1;
			break;
		case SQL_ORDER_BY_COLUMNS_IN_SELECT:
			debugPrintf("  infotype: "
					"SQL_ORDER_BY_COLUMNS_IN_SELECT\n");
			// FIXME: is this true for all db's?
			val.strval="N";
			type=0;
			break;
		case SQL_SPECIAL_CHARACTERS:
			debugPrintf("  infotype: "
					"SQL_SPECIAL_CHARACTERS\n");
			val.strval="#$_";
			type=0;
			break;
		case SQL_MAX_COLUMNS_IN_GROUP_BY:
			// aka SQL_MAXIMUM_COLUMNS_IN_GROUP_BY
			debugPrintf("  infotype: "
					"SQL_MAX_COLUMNS_IN_GROUP_BY/"
					"SQL_MAXIMUM_COLUMNS_IN_GROUP_BY\n");
			// 0 means no max or unknown
			val.usmallintval=0;
			type=2;
			break;
		case SQL_MAX_COLUMNS_IN_INDEX:
			// aka SQL_MAXIMUM_COLUMNS_IN_INDEX
			debugPrintf("  infotype: "
					"SQL_MAX_COLUMNS_IN_INDEX/"
					"SQL_MAXIMUM_COLUMNS_IN_INDEX\n");
			// 0 means no max or unknown
			val.usmallintval=0;
			type=2;
			break;
		case SQL_MAX_COLUMNS_IN_ORDER_BY:
			// aka SQL_MAXIMUM_COLUMNS_IN_ORDER_BY
			debugPrintf("  infotype: "
					"SQL_MAX_COLUMNS_IN_ORDER_BY/"
					"SQL_MAXIMUM_COLUMNS_IN_ORDER_BY\n");
			// 0 means no max or unknown
			val.usmallintval=0;
			type=2;
			break;
		case SQL_MAX_COLUMNS_IN_SELECT:
			// aka SQL_MAXIMUM_COLUMNS_IN_SELECT
			debugPrintf("  infotype: "
					"SQL_MAX_COLUMNS_IN_SELECT/"
					"SQL_MAXIMUM_COLUMNS_IN_SELECT\n");
			// 0 means no max or unknown
			val.usmallintval=0;
			type=2;
			break;
		case SQL_MAX_COLUMNS_IN_TABLE:
			debugPrintf("  infotype: "
					"SQL_MAX_COLUMNS_IN_TABLE\n");
			// 0 means no max or unknown
			val.usmallintval=0;
			type=2;
			break;
		case SQL_MAX_INDEX_SIZE:
			// aka SQL_MAXIMUM_INDEX_SIZE
			debugPrintf("  infotype: "
					"SQL_MAX_INDEX_SIZE/"
					"SQL_MAXIMUM_INDEX_SIZE\n");
			// 0 means no max or unknown
			val.uintval=0;
			type=1;
			break;
		case SQL_MAX_ROW_SIZE:
			// aka SQL_MAXIMUM_ROW_SIZE
			debugPrintf("  infotype: "
					"SQL_MAX_ROW_SIZE/"
					"SQL_MAXIMUM_ROW_SIZE\n");
			// 0 means no max or unknown
			val.uintval=0;
			type=1;
			break;
		case SQL_MAX_STATEMENT_LEN:
			// aka SQL_MAXIMUM_STATEMENT_LENGTH
			debugPrintf("  infotype: "
					"SQL_MAX_STATEMENT_LEN/"
					"SQL_MAXIMUM_STATEMENT_LENGTH\n");
			// 0 means no max or unknown
			val.uintval=0;
			type=1;
			break;
		case SQL_MAX_TABLES_IN_SELECT:
			// aka SQL_MAXIMUM_TABLES_IN_SELECT
			debugPrintf("  infotype: "
					"SQL_MAX_TABLES_IN_SELECT/"
					"SQL_MAXIMUM_TABLES_IN_SELECT\n");
			// 0 means no max or unknown
			val.usmallintval=0;
			type=2;
			break;
		case SQL_MAX_USER_NAME_LEN:
			// aka SQL_MAXIMUM_USER_NAME_LENGTH
			debugPrintf("  infotype: "
					"SQL_MAX_USER_NAME_LEN/"
					"SQL_MAXIMUM_USER_NAME_LENGTH\n");
			// 0 means no max or unknown
			val.usmallintval=0;
			type=2;
			break;
		#if (ODBCVER >= 0x0300)
		case SQL_OJ_CAPABILITIES:
			// aka SQL_OUTER_JOIN_CAPABILITIES
			debugPrintf("  infotype: "
					"SQL_OJ_CAPABILITIES/"
					"SQL_OUTER_JOIN_CAPABILITIES\n");
			// FIXME: this isn't true for all db's
			val.uintval=SQL_OJ_LEFT|
					SQL_OJ_RIGHT|
					SQL_OJ_FULL|
					SQL_OJ_NESTED|
					SQL_OJ_NOT_ORDERED|
					SQL_OJ_INNER|
					SQL_OJ_ALL_COMPARISON_OPS;
			type=1;
			break;
		case SQL_XOPEN_CLI_YEAR:
			debugPrintf("  infotype: "
					"SQL_XOPEN_CLI_YEAR\n");
			// FIXME: what actual year?
			val.strval="1996";
			type=0;
			break;
		case SQL_CURSOR_SENSITIVITY:
			debugPrintf("  infotype: "
					"SQL_CURSOR_SENSITIVITY\n");
			val.uintval=SQL_UNSPECIFIED;
			type=1;
			break;
		case SQL_DESCRIBE_PARAMETER:
			debugPrintf("  infotype: "
					"SQL_DESCRIBE_PARAMETER\n");
			val.strval="N";
			type=0;
			break;
		case SQL_CATALOG_NAME:
			debugPrintf("  infotype: "
					"SQL_CATALOG_NAME\n");
			// FIXME: is this true for all db's?
			val.strval="Y";
			type=0;
			break;
		case SQL_COLLATION_SEQ:
			debugPrintf("  infotype: "
					"SQL_COLLATION_SEQ\n");
			val.strval="";
			type=0;
			break;
		case SQL_MAX_IDENTIFIER_LEN:
			// aka SQL_MAXIMUM_IDENTIFIER_LENGTH
			debugPrintf("  infotype: "
					"SQL_MAX_IDENTIFIER_LEN/"
					"SQL_MAXIMUM_IDENTIFIER_LENGTH\n");
			// FIXME: is this true for all db's?
			val.usmallintval=128;
			type=2;
			break;
		#endif
		case SQL_DRIVER_HDBC:
			debugPrintf("  unsupported infotype: "
						"SQL_DRIVER_HDBC\n");
			break;
		case SQL_DRIVER_HENV:
			debugPrintf("  unsupported infotype: "
						"SQL_DRIVER_HENV\n");
			break;
		case SQL_DRIVER_HSTMT:
			debugPrintf("  unsupported infotype: "
						"SQL_DRIVER_HSTMT\n");
			break;
		case SQL_DRIVER_NAME:
			debugPrintf("  infotype: "
					"SQL_DRIVER_NAME\n");
			#ifdef _WIN32
			val.strval="libsqlrodbc.dll";
			#else
			val.strval="libsqlrodbc.so";
			#endif
			type=0;
			break;
		case SQL_DRIVER_VER:
			debugPrintf("  infotype: "
					"SQL_DRIVER_VER\n");
			val.strval=conn->con->clientVersion();
			type=0;
			break;
		case SQL_ODBC_API_CONFORMANCE:
			debugPrintf("  infotype: "
					"SQL_ODBC_API_CONFORMANCE\n");
			val.usmallintval=SQL_OAC_LEVEL2;
			type=2;
			break;
		case SQL_ODBC_VER:
			debugPrintf("  infotype: "
					"SQL_ODBC_VER\n");
			// FIXME: this should be of format ##.##.####
			// (major.minor.release)
			// Though, I think the driver manager always
			// intercepts this and returns its version.
			val.strval=conn->con->clientVersion();
			type=0;
			break;
		case SQL_ROW_UPDATES:
			debugPrintf("  infotype: "
					"SQL_ROW_UPDATES\n");
			val.strval="N";
			type=0;
			break;
		case SQL_ODBC_SAG_CLI_CONFORMANCE:
			debugPrintf("  unsupported infotype: "
					"SQL_ODBC_SAG_CLI_CONFORMANCE\n");
			break;
		case SQL_ODBC_SQL_CONFORMANCE:
			debugPrintf("  infotype: "
					"SQL_ODBC_SQL_CONFORMANCE\n");
			val.usmallintval=SQL_OSC_EXTENDED;
			type=2;
			break;
		case SQL_PROCEDURES:
			debugPrintf("  infotype: "
					"SQL_PROCEDURES\n");
			// FIXME: this isn't true for all db's
			val.strval="Y";
			type=0;
			break;
		case SQL_CONCAT_NULL_BEHAVIOR:
			debugPrintf("  infotype: "
					"SQL_CONCAT_NULL_BEHAVIOR\n");
			// FIXME: is this true for all db's?
			val.usmallintval=SQL_CB_NON_NULL;
			type=2;
			break;
		case SQL_CURSOR_ROLLBACK_BEHAVIOR:
			debugPrintf("  infotype: "
					"SQL_CURSOR_ROLLBACK_BEHAVIOR\n");
			// FIXME: is this true for all db's?
			val.usmallintval=SQL_CB_CLOSE;
			type=2;
			break;
		case SQL_EXPRESSIONS_IN_ORDERBY:
			debugPrintf("  infotype: "
					"SQL_EXPRESSIONS_IN_ORDERBY\n");
			// FIXME: is this true for all db's?
			val.strval="Y";
			type=0;
			break;
		case SQL_MAX_PROCEDURE_NAME_LEN:
			debugPrintf("  infotype: "
					"SQL_MAX_PROCEDURE_NAME_LEN\n");
			// 0 means no max or unknown
			val.usmallintval=0;
			type=2;
			break;
		case SQL_MULT_RESULT_SETS:
			debugPrintf("  infotype: "
					"SQL_MULT_RESULT_SETS\n");
			val.strval="N";
			type=0;
			break;
		case SQL_MULTIPLE_ACTIVE_TXN:
			debugPrintf("  infotype: "
					"SQL_MULTIPLE_ACTIVE_TXN\n");
			val.strval="N";
			type=0;
			break;
		case SQL_OUTER_JOINS:
			debugPrintf("  infotype: "
					"SQL_OUTER_JOINS\n");
			// FIXME: is this true for all db's?
			val.strval="Y";
			type=0;
			break;
		case SQL_OWNER_TERM:
			// aka SQL_SCHEMA_TERM
			debugPrintf("  infotype: "
					"SQL_OWNER_TERM/"
					"SQL_SCHEMA_TERM\n");
			val.strval="schema";
			type=0;
			break;
		case SQL_PROCEDURE_TERM:
			debugPrintf("  infotype: "
					"SQL_PROCEDURE_TERM\n");
			val.strval="stored procedure";
			type=0;
			break;
		case SQL_QUALIFIER_NAME_SEPARATOR:
			// aka SQL_CATALOG_NAME_SEPARATOR
			debugPrintf("  infotype: "
					"SQL_QUALIFIER_NAME_SEPARATOR/"
					"SQL_CATALOG_NAME_SEPARATOR\n");
			// FIXME: are there db's other than oracle where the
			// catalog separator is an @?
			if (!charstring::compare(conn->con->identify(),
								"oracle")) {
				val.strval="@";
			} else {
				val.strval=".";
			}
			type=0;
			break;
		case SQL_QUALIFIER_TERM:
			// aka SQL_CATALOG_TERM
			debugPrintf("  infotype: "
					"SQL_QUALIFIER_TERM/"
					"SQL_CATALOG_TERM\n");
			val.strval="catalog";
			type=0;
			break;
		case SQL_SCROLL_OPTIONS:
			debugPrintf("  infotype: "
					"SQL_SCROLL_OPTIONS\n");
			val.uintval=SQL_SO_FORWARD_ONLY;
			type=1;
			break;
		case SQL_TABLE_TERM:
			debugPrintf("  infotype: "
					"SQL_TABLE_TERM\n");
			val.strval="table";
			type=0;
			break;
		case SQL_CONVERT_FUNCTIONS:
			debugPrintf("  infotype: "
					"SQL_CONVERT_FUNCTIONS\n");
			// FIXME: this isn't true for all db's
			val.uintval=SQL_FN_CVT_CAST|SQL_FN_CVT_CONVERT;
			type=1;
			break;
		case SQL_NUMERIC_FUNCTIONS:
			debugPrintf("  infotype: "
					"SQL_NUMERIC_FUNCTIONS\n");
			// FIXME: this isn't true for all db's
			val.uintval=SQL_FN_NUM_ABS|
					SQL_FN_NUM_ACOS|
					SQL_FN_NUM_ASIN|
					SQL_FN_NUM_ATAN|
					SQL_FN_NUM_ATAN2|
					SQL_FN_NUM_CEILING|
					SQL_FN_NUM_COS|
					SQL_FN_NUM_COT|
					SQL_FN_NUM_DEGREES|
					SQL_FN_NUM_EXP|
					SQL_FN_NUM_FLOOR|
					SQL_FN_NUM_LOG|
					SQL_FN_NUM_LOG10|
					SQL_FN_NUM_MOD|
					SQL_FN_NUM_PI|
					SQL_FN_NUM_POWER|
					SQL_FN_NUM_RADIANS|
					SQL_FN_NUM_RAND|
					SQL_FN_NUM_ROUND|
					SQL_FN_NUM_SIGN|
					SQL_FN_NUM_SIN|
					SQL_FN_NUM_SQRT|
					SQL_FN_NUM_TAN|
					SQL_FN_NUM_TRUNCATE;
			type=1;
			break;
		case SQL_STRING_FUNCTIONS:
			debugPrintf("  infotype: "
					"SQL_STRING_FUNCTIONS\n");
			// FIXME: this isn't true for all db's
			val.uintval=SQL_FN_STR_CONCAT|
					SQL_FN_STR_INSERT|
					SQL_FN_STR_LEFT|
					SQL_FN_STR_LTRIM|
					SQL_FN_STR_LENGTH|
					SQL_FN_STR_LOCATE|
					SQL_FN_STR_LCASE|
					SQL_FN_STR_REPEAT|
					SQL_FN_STR_REPLACE|
					SQL_FN_STR_RIGHT|
					SQL_FN_STR_RTRIM|
					SQL_FN_STR_SUBSTRING|
					SQL_FN_STR_UCASE|
					SQL_FN_STR_ASCII|
					SQL_FN_STR_CHAR|
					SQL_FN_STR_DIFFERENCE|
					SQL_FN_STR_LOCATE_2|
					SQL_FN_STR_SOUNDEX|
					SQL_FN_STR_SPACE|
					SQL_FN_STR_BIT_LENGTH|
					SQL_FN_STR_CHAR_LENGTH|
					SQL_FN_STR_CHARACTER_LENGTH|
					SQL_FN_STR_OCTET_LENGTH|
					SQL_FN_STR_POSITION;
			type=1;
			break;
		case SQL_SYSTEM_FUNCTIONS:
			debugPrintf("  infotype: "
					"SQL_SYSTEM_FUNCTIONS\n");
			// FIXME: this isn't true for all db's
			val.uintval=SQL_FN_SYS_DBNAME|
					SQL_FN_SYS_IFNULL|
					SQL_FN_SYS_USERNAME;
			type=1;
			break;
		case SQL_TIMEDATE_FUNCTIONS:
			debugPrintf("  infotype: "
					"SQL_TIMEDATE_FUNCTIONS\n");
			// FIXME: this isn't true for all db's
			val.uintval=SQL_FN_TD_CURRENT_DATE|
					SQL_FN_TD_CURRENT_TIME|
					SQL_FN_TD_CURRENT_TIMESTAMP|
					SQL_FN_TD_CURDATE|
					SQL_FN_TD_CURTIME|
					SQL_FN_TD_DAYNAME|
					SQL_FN_TD_DAYOFMONTH|
					SQL_FN_TD_DAYOFWEEK|
					SQL_FN_TD_DAYOFYEAR|
					SQL_FN_TD_EXTRACT|
					SQL_FN_TD_HOUR|
					SQL_FN_TD_MINUTE|
					SQL_FN_TD_MONTH|
					SQL_FN_TD_MONTHNAME|
					SQL_FN_TD_NOW|
					SQL_FN_TD_QUARTER|
					SQL_FN_TD_SECOND|
					SQL_FN_TD_TIMESTAMPADD|
					SQL_FN_TD_TIMESTAMPDIFF|
					SQL_FN_TD_WEEK|
					SQL_FN_TD_YEAR;
			type=1;
			break;
		case SQL_CONVERT_BIGINT:
			debugPrintf("  infotype: "
					"SQL_CONVERT_BIGINT\n");
			// FIXME: 0 means no no conversions are supported
			val.uintval=0;
			type=1;
			break;
		case SQL_CONVERT_BINARY:
			debugPrintf("  infotype: "
					"SQL_CONVERT_BINARY\n");
			// FIXME: 0 means no no conversions are supported
			val.uintval=0;
			type=1;
			break;
		case SQL_CONVERT_BIT:
			debugPrintf("  infotype: "
					"SQL_CONVERT_BIT\n");
			// FIXME: 0 means no no conversions are supported
			val.uintval=0;
			type=1;
			break;
		case SQL_CONVERT_CHAR:
			debugPrintf("  infotype: "
					"SQL_CONVERT_CHAR\n");
			// FIXME: 0 means no no conversions are supported
			val.uintval=0;
			type=1;
			break;
		case SQL_CONVERT_DATE:
			debugPrintf("  infotype: "
					"SQL_CONVERT_DATE\n");
			// FIXME: 0 means no no conversions are supported
			val.uintval=0;
			type=1;
			break;
		case SQL_CONVERT_DECIMAL:
			debugPrintf("  infotype: "
					"SQL_CONVERT_DECIMAL\n");
			// FIXME: 0 means no no conversions are supported
			val.uintval=0;
			type=1;
			break;
		case SQL_CONVERT_DOUBLE:
			debugPrintf("  infotype: "
					"SQL_CONVERT_DOUBLE\n");
			// FIXME: 0 means no no conversions are supported
			val.uintval=0;
			type=1;
			break;
		case SQL_CONVERT_FLOAT:
			debugPrintf("  infotype: "
					"SQL_CONVERT_FLOAT\n");
			// FIXME: 0 means no no conversions are supported
			val.uintval=0;
			type=1;
			break;
		case SQL_CONVERT_INTEGER:
			debugPrintf("  infotype: "
					"SQL_CONVERT_INTEGER\n");
			// FIXME: 0 means no no conversions are supported
			val.uintval=0;
			type=1;
			break;
		case SQL_CONVERT_LONGVARCHAR:
			debugPrintf("  infotype: "
					"SQL_CONVERT_LONGVARCHAR\n");
			// FIXME: 0 means no no conversions are supported
			val.uintval=0;
			type=1;
			break;
		case SQL_CONVERT_NUMERIC:
			debugPrintf("  infotype: "
					"SQL_CONVERT_NUMERIC\n");
			// FIXME: 0 means no no conversions are supported
			val.uintval=0;
			type=1;
			break;
		case SQL_CONVERT_REAL:
			debugPrintf("  infotype: "
					"SQL_CONVERT_REAL\n");
			// FIXME: 0 means no no conversions are supported
			val.uintval=0;
			type=1;
			break;
		case SQL_CONVERT_SMALLINT:
			debugPrintf("  infotype: "
					"SQL_CONVERT_SMALLINT\n");
			// FIXME: 0 means no no conversions are supported
			val.uintval=0;
			type=1;
			break;
		case SQL_CONVERT_TIME:
			debugPrintf("  infotype: "
					"SQL_CONVERT_TIME\n");
			// FIXME: 0 means no no conversions are supported
			val.uintval=0;
			type=1;
			break;
		case SQL_CONVERT_TIMESTAMP:
			debugPrintf("  infotype: "
					"SQL_CONVERT_TIMESTAMP\n");
			// FIXME: 0 means no no conversions are supported
			val.uintval=0;
			type=1;
			break;
		case SQL_CONVERT_TINYINT:
			debugPrintf("  infotype: "
					"SQL_CONVERT_TINYINT\n");
			// FIXME: 0 means no no conversions are supported
			val.uintval=0;
			type=1;
			break;
		case SQL_CONVERT_VARBINARY:
			debugPrintf("  infotype: "
					"SQL_CONVERT_VARBINARY\n");
			// FIXME: 0 means no no conversions are supported
			val.uintval=0;
			type=1;
			break;
		case SQL_CONVERT_VARCHAR:
			debugPrintf("  infotype: "
					"SQL_CONVERT_VARCHAR\n");
			// FIXME: 0 means no no conversions are supported
			val.uintval=0;
			type=1;
			break;
		case SQL_CONVERT_LONGVARBINARY:
			debugPrintf("  infotype: "
					"SQL_CONVERT_LONGVARBINARY\n");
			// FIXME: 0 means no no conversions are supported
			val.uintval=0;
			type=1;
			break;
		case SQL_CORRELATION_NAME:
			debugPrintf("  infotype: "
					"SQL_CORRELATION_NAME\n");
			val.usmallintval=SQL_CN_ANY;
			type=2;
			break;
		case SQL_NON_NULLABLE_COLUMNS:
			debugPrintf("  infotype: "
					"SQL_NON_NULLABLE_COLUMNS\n");
			val.usmallintval=SQL_NNC_NON_NULL;
			type=2;
			break;
		case SQL_DRIVER_HLIB:
			debugPrintf("  unsupported infotype: "
					"SQL_DRIVER_HLIB\n");
			break;
		case SQL_DRIVER_ODBC_VER:
			debugPrintf("  infotype: "
					"SQL_DRIVER_ODBC_VER\n");
			#if (ODBCVER == 0x0380)
				val.strval="03.80";
			#elif (ODBCVER >= 0x0300)
				val.strval="03.00";
			#else
				val.strval="02.00";
			#endif
			type=0;
			break;
		case SQL_LOCK_TYPES:
			debugPrintf("  infotype: "
					"SQL_LOCK_TYPES\n");
			// FIXME: is this true for all db's?
			val.uintval=SQL_LCK_NO_CHANGE|
					SQL_LCK_EXCLUSIVE|
					SQL_LCK_UNLOCK;
			type=1;
			break;
		case SQL_POS_OPERATIONS:
			debugPrintf("  infotype: "
					"SQL_POS_OPERATIONS\n");
			// FIXME: for now...
			val.usmallintval=SQL_POS_POSITION;
			type=2;
			break;
		case SQL_POSITIONED_STATEMENTS:
			debugPrintf("  infotype: "
					"SQL_POSITIONED_STATEMENTS\n");
			// none, for now...
			val.uintval=0;
			type=1;
			break;
		case SQL_BOOKMARK_PERSISTENCE:
			debugPrintf("  infotype: "
					"SQL_BOOKMARK_PERSISTENCE\n");
			// FIXME: none, for now...
			val.uintval=0;
			type=1;
			break;
		case SQL_STATIC_SENSITIVITY:
			debugPrintf("  infotype: "
					"SQL_STATIC_SENSITIVITY\n");
			val.uintval=0;
			type=1;
			break;
		case SQL_FILE_USAGE:
			debugPrintf("  infotype: "
					"SQL_FILE_USAGE\n");
			val.uintval=SQL_FILE_NOT_SUPPORTED;
			type=1;
			break;
		case SQL_COLUMN_ALIAS:
			debugPrintf("  infotype: "
					"SQL_COLUMN_ALIAS\n");
			// FIXME: this isn't true for all db's
			val.strval="Y";
			type=0;
			break;
		case SQL_GROUP_BY:
			debugPrintf("  infotype: "
					"SQL_GROUP_BY\n");
			// FIXME: is this true for all db's?
			val.usmallintval=
					#if (ODBCVER >= 0x0300)
					SQL_GB_COLLATE
					#else
					SQL_GB_GROUP_BY_EQUALS_SELECT
					#endif
					;
			type=2;
			break;
		case SQL_KEYWORDS:
			debugPrintf("  infotype: "
					"SQL_KEYWORDS\n");
			// FIXME: this isn't true for all db's
			val.strval=SQL_ODBC_KEYWORDS;
			type=0;
			break;
		case SQL_OWNER_USAGE:
			// aka SQL_SCHEMA_USAGE
			debugPrintf("  infotype: "
					"SQL_OWNER_USAGE/"
					"SQL_SCHEMA_USAGE\n");
			// FIXME: this isn't true for all db's
			val.uintval=SQL_SU_DML_STATEMENTS|
					SQL_SU_PROCEDURE_INVOCATION|
					SQL_SU_TABLE_DEFINITION|
					SQL_SU_INDEX_DEFINITION|
					SQL_SU_PRIVILEGE_DEFINITION;
			type=1;
			break;
		case SQL_QUALIFIER_USAGE:
			// aka SQL_CATALOG_USAGE
			debugPrintf("  infotype: "
					"SQL_QUALIFIER_USAGE/"
					"SQL_CATALOG_USAGE\n");
			// FIXME: this isn't true for all db's
			val.uintval=SQL_SU_DML_STATEMENTS|
					SQL_SU_PROCEDURE_INVOCATION|
					SQL_SU_TABLE_DEFINITION|
					SQL_SU_INDEX_DEFINITION|
					SQL_SU_PRIVILEGE_DEFINITION;
			type=1;
			break;
		case SQL_QUOTED_IDENTIFIER_CASE:
			debugPrintf("  infotype: "
					"SQL_QUOTED_IDENTIFIER_CASE\n");
			val.usmallintval=SQL_IC_SENSITIVE;
			type=2;
			break;
		case SQL_SUBQUERIES:
			debugPrintf("  infotype: "
					"SQL_SUBQUERIES\n");
			// FIXME: is this true for all db's?
			val.uintval=SQL_SQ_CORRELATED_SUBQUERIES|
					SQL_SQ_COMPARISON|
					SQL_SQ_EXISTS|
					SQL_SQ_IN|
					SQL_SQ_QUANTIFIED;
			type=1;
			break;
		case SQL_UNION:
			// aka SQL_UNION_STATEMENT
			debugPrintf("  infotype: "
					"SQL_UNION/"
					"SQL_UNION_STATEMENT\n");
			// FIXME: this isn't true for all db's
			val.uintval=SQL_U_UNION|SQL_U_UNION_ALL;
			type=1;
			break;
		case SQL_MAX_ROW_SIZE_INCLUDES_LONG:
			debugPrintf("  infotype: "
					"SQL_MAX_ROW_SIZE_INCLUDES_LONG\n");
			val.strval="N";
			type=0;
			break;
		case SQL_MAX_CHAR_LITERAL_LEN:
			debugPrintf("  infotype: "
					"SQL_MAX_CHAR_LITERAL_LEN\n");
			// 0 means no max or unknown
			val.uintval=0;
			type=1;
			break;
		case SQL_TIMEDATE_ADD_INTERVALS:
			debugPrintf("  infotype: "
					"SQL_TIMEDATE_ADD_INTERVALS\n");
			// FIXME: this isn't true for all db's
			// I think Oracle 12c supports intervals
			val.uintval=0;
				/*SQL_FN_TSI_FRAC_SECOND|
				SQL_FN_TSI_SECOND|
				SQL_FN_TSI_MINUTE|
				SQL_FN_TSI_HOUR|
				SQL_FN_TSI_DAY|
				SQL_FN_TSI_WEEK|
				SQL_FN_TSI_MONTH|
				SQL_FN_TSI_QUARTER|
				SQL_FN_TSI_YEAR;*/
			type=1;
			break;
		case SQL_TIMEDATE_DIFF_INTERVALS:
			debugPrintf("  infotype: "
					"SQL_TIMEDATE_DIFF_INTERVALS\n");
			// FIXME: this isn't true for all db's
			// I think Oracle 12c supports intervals
			val.uintval=0;
				/*SQL_FN_TSI_FRAC_SECOND|
				SQL_FN_TSI_SECOND|
				SQL_FN_TSI_MINUTE|
				SQL_FN_TSI_HOUR|
				SQL_FN_TSI_DAY|
				SQL_FN_TSI_WEEK|
				SQL_FN_TSI_MONTH|
				SQL_FN_TSI_QUARTER|
				SQL_FN_TSI_YEAR;*/
			type=1;
			break;
		case SQL_NEED_LONG_DATA_LEN:
			debugPrintf("  infotype: "
					"SQL_NEED_LONG_DATA_LEN\n");
			val.strval="Y";
			type=0;
			break;
		case SQL_MAX_BINARY_LITERAL_LEN:
			debugPrintf("  infotype: "
					"SQL_MAX_BINARY_LITERAL_LEN\n");
			// 0 means no max or unknown
			val.uintval=0;
			type=1;
			break;
		case SQL_LIKE_ESCAPE_CLAUSE:
			debugPrintf("  infotype: "
					"SQL_LIKE_ESCAPE_CLAUSE\n");
			val.strval="Y";
			type=0;
			break;
		case SQL_QUALIFIER_LOCATION:
			// aka SQL_CATALOG_LOCATION
			debugPrintf("  infotype: "
					"SQL_QUALIFIER_LOCATION/"
					"SQL_CATALOG_LOCATION\n");
			// FIXME: are there db's other than oracle where the
			// catalog is at the end?
			if (!charstring::compare(conn->con->identify(),
								"oracle")) {
				val.usmallintval=SQL_CL_END;
			} else {
				val.usmallintval=SQL_CL_START;
			}
			type=2;
			break;
		#if (ODBCVER >= 0x0300)
		case SQL_ACTIVE_ENVIRONMENTS:
			debugPrintf("  infotype: "
					"SQL_ACTIVE_ENVIRONMENTS\n");
			// 0 means no max or unknown
			val.usmallintval=0;
			type=2;
			break;
		case SQL_ALTER_DOMAIN:
			debugPrintf("  infotype: "
					"SQL_ALTER_DOMAIN\n");
			// FIXME: no idea
			val.uintval=SQL_AD_ADD_DOMAIN_CONSTRAINT|
					SQL_AD_ADD_DOMAIN_DEFAULT|
					SQL_AD_CONSTRAINT_NAME_DEFINITION|
					SQL_AD_DROP_DOMAIN_CONSTRAINT|
					SQL_AD_DROP_DOMAIN_DEFAULT;
			type=1;
			break;
		case SQL_SQL_CONFORMANCE:
			debugPrintf("  infotype: "
					"SQL_SQL_CONFORMANCE\n");
			// FIXME: no idea, conservative guess...
			val.uintval=SQL_SC_SQL92_ENTRY;
					/*SQL_SC_FIPS127_2_TRANSITIONAL
					SQL_SC_SQL92_FULL
					SQL_SC_SQL92_INTERMEDIATE*/
			type=1;
			break;
		case SQL_DATETIME_LITERALS:
			debugPrintf("  infotype: "
					"SQL_DATETIME_LITERALS\n");
			// FIXME: this isn't true for all db's
			// I think Oracle 12c supports intervals
			val.uintval=0;
				/*SQL_DL_SQL92_DATE|
				SQL_DL_SQL92_TIME|
				SQL_DL_SQL92_TIMESTAMP|
				SQL_DL_SQL92_INTERVAL_YEAR|
				SQL_DL_SQL92_INTERVAL_MONTH|
				SQL_DL_SQL92_INTERVAL_DAY|
				SQL_DL_SQL92_INTERVAL_HOUR|
				SQL_DL_SQL92_INTERVAL_MINUTE|
				SQL_DL_SQL92_INTERVAL_SECOND|
				SQL_DL_SQL92_INTERVAL_YEAR_TO_MONTH|
				SQL_DL_SQL92_INTERVAL_DAY_TO_HOUR
				SQL_DL_SQL92_INTERVAL_DAY_TO_MINUTE|
				SQL_DL_SQL92_INTERVAL_DAY_TO_SECOND|
				SQL_DL_SQL92_INTERVAL_HOUR_TO_MINUTE|
				SQL_DL_SQL92_INTERVAL_HOUR_TO_SECOND|
				SQL_DL_SQL92_INTERVAL_MINUTE_TO_SECOND;*/
			type=1;
			break;
		case SQL_ASYNC_MODE:
			debugPrintf("  infotype: "
					"SQL_ASYNC_MODE\n");
			val.uintval=SQL_AM_NONE;
			type=1;
			break;
		case SQL_BATCH_ROW_COUNT:
			debugPrintf("  infotype: "
					"SQL_BATCH_ROW_COUNT\n");
			// FIXME: this might not be correct
			val.uintval=0;
			type=1;
			break;
		case SQL_BATCH_SUPPORT:
			debugPrintf("  infotype: "
					"SQL_BATCH_SUPPORT\n");
			// FIXME: this might not be correct
			val.uintval=0;
			type=1;
			break;
		case SQL_CONVERT_WCHAR:
			debugPrintf("  infotype: "
					"SQL_CONVERT_WCHAR\n");
			// FIXME: 0 means no no conversions are supported
			val.uintval=0;
			type=1;
			break;
		case SQL_CONVERT_INTERVAL_DAY_TIME:
			debugPrintf("  infotype: "
					"SQL_CONVERT_INTERVAL_DAY_TIME\n");
			// FIXME: 0 means no no conversions are supported
			val.uintval=0;
			type=1;
			break;
		case SQL_CONVERT_INTERVAL_YEAR_MONTH:
			debugPrintf("  infotype: "
					"SQL_CONVERT_INTERVAL_YEAR_MONTH\n");
			// FIXME: 0 means no no conversions are supported
			val.uintval=0;
			type=1;
			break;
		case SQL_CONVERT_WLONGVARCHAR:
			debugPrintf("  infotype: "
					"SQL_CONVERT_WLONGVARCHAR\n");
			// FIXME: 0 means no no conversions are supported
			val.uintval=0;
			type=1;
			break;
		case SQL_CONVERT_WVARCHAR:
			debugPrintf("  infotype: "
					"SQL_CONVERT_WVARCHAR\n");
			// FIXME: 0 means no no conversions are supported
			val.uintval=0;
			type=1;
			break;
		case SQL_CREATE_ASSERTION:
			debugPrintf("  infotype: "
					"SQL_CREATE_ASSERTION\n");
			// FIXME: not sure about this...
			val.uintval=0;
				/*SQL_CA_CREATE_ASSERTION|
				SQL_CA_CONSTRAINT_INITIALLY_DEFERRED|
				SQL_CA_CONSTRAINT_INITIALLY_IMMEDIATE|
				SQL_CA_CONSTRAINT_DEFERRABLE|
				SQL_CA_CONSTRAINT_NON_DEFERRABLE;*/
			type=1;
			break;
		case SQL_CREATE_CHARACTER_SET:
			debugPrintf("  infotype: "
					"SQL_CREATE_CHARACTER_SET\n");
			// FIXME: not sure about this...
			val.uintval=0;
				/*SQL_CCS_CREATE_CHARACTER_SET|
				SQL_CCS_COLLATE_CLAUSE|
				SQL_CCS_LIMITED_COLLATION;*/
			type=1;
			break;
		case SQL_CREATE_COLLATION:
			debugPrintf("  infotype: "
					"SQL_CREATE_COLLATION\n");
			// FIXME: not sure about this...
			val.uintval=0;
				//SQL_CCOL_CREATE_COLLATION;
			type=1;
			break;
		case SQL_CREATE_DOMAIN:
			debugPrintf("  infotype: "
					"SQL_CREATE_DOMAIN\n");
			// FIXME: not sure about this...
			val.uintval=0;
				/*SQL_CDO_CREATE_DOMAIN|
				SQL_CDO_CONSTRAINT_NAME_DEFINITION|
				SQL_CDO_DEFAULT|
				SQL_CDO_CONSTRAINT|
				SQL_CDO_COLLATION|
				SQL_CDO_CONSTRAINT_INITIALLY_DEFERRED|
				SQL_CDO_CONSTRAINT_INITIALLY_IMMEDIATE|
				SQL_CDO_CONSTRAINT_DEFERRABLE|
				SQL_CDO_CONSTRAINT_NON_DEFERRABLE;*/
			type=1;
			break;
		case SQL_CREATE_SCHEMA:
			debugPrintf("  infotype: "
					"SQL_CREATE_SCHEMA\n");
			// FIXME: is this true for all db's?
			val.uintval=SQL_CS_CREATE_SCHEMA|
						SQL_CS_AUTHORIZATION|
						SQL_CS_DEFAULT_CHARACTER_SET;
			type=1;
			break;
		case SQL_CREATE_TABLE:
			debugPrintf("  infotype: "
					"SQL_CREATE_TABLE\n");
			// FIXME: this isn't true for all db's
			val.uintval=SQL_CT_CREATE_TABLE|
					SQL_CT_TABLE_CONSTRAINT|
					SQL_CT_CONSTRAINT_NAME_DEFINITION|
					SQL_CT_COMMIT_DELETE|
					SQL_CT_GLOBAL_TEMPORARY|
					SQL_CT_COLUMN_CONSTRAINT|
					SQL_CT_COLUMN_DEFAULT|
					SQL_CT_COLUMN_COLLATION|
					SQL_CT_CONSTRAINT_INITIALLY_IMMEDIATE|
					SQL_CT_CONSTRAINT_NON_DEFERRABLE;
			type=1;
			break;
		case SQL_CREATE_TRANSLATION:
			debugPrintf("  infotype: "
					"SQL_CREATE_TRANSLATION\n");
			// FIXME: not sure about this...
			val.uintval=0;
				//SQL_CTR_CREATE_TRANSLATION;
			type=1;
			break;
		case SQL_CREATE_VIEW:
			debugPrintf("  infotype: "
					"SQL_CREATE_VIEW\n");
			// FIXME: is this true for all db's?
			val.uintval=SQL_CV_CREATE_VIEW|
					SQL_CV_CHECK_OPTION|
					SQL_CV_CASCADED|
					SQL_CV_LOCAL ;
			type=1;
			break;
		case SQL_DRIVER_HDESC:
			debugPrintf("  unsupported infotype: "
					"SQL_DRIVER_HDESC\n");
			break;
		case SQL_DROP_ASSERTION:
			debugPrintf("  infotype: "
					"SQL_DROP_ASSERTION\n");
			// FIXME: not sure about this...
			val.uintval=0;
				//SQL_DA_DROP_ASSERTION;
			type=1;
			break;
		case SQL_DROP_CHARACTER_SET:
			debugPrintf("  infotype: "
					"SQL_DROP_CHARACTER_SET\n");
			val.uintval=0;
				//SQL_DCS_DROP_CHARACTER_SET;
			type=1;
			break;
		case SQL_DROP_COLLATION:
			debugPrintf("  infotype: "
					"SQL_DROP_COLLATION\n");
			val.uintval=0;
				//SQL_DC_DROP_COLLATION;
			type=1;
			break;
		case SQL_DROP_DOMAIN:
			debugPrintf("  infotype: "
					"SQL_DROP_DOMAIN\n");
			val.uintval=0;
				//SQL_DD_DROP_DOMAIN|
				//SQL_DD_CASCADE|
				//SQL_DD_RESTRICT;
			type=1;
			break;
		case SQL_DROP_SCHEMA:
			debugPrintf("  infotype: "
					"SQL_DROP_SCHEMA\n");
			// FIXME: is this true for all db's?
			val.uintval=SQL_DS_DROP_SCHEMA|
					SQL_DS_CASCADE|
					SQL_DS_RESTRICT;
			type=1;
			break;
		case SQL_DROP_TABLE:
			debugPrintf("  infotype: "
					"SQL_DROP_TABLE\n");
			// FIXME: is this true for all db's?
			val.uintval=SQL_DT_DROP_TABLE|
					SQL_DT_CASCADE|
					SQL_DT_RESTRICT;
			type=1;
			break;
		case SQL_DROP_TRANSLATION:
			debugPrintf("  infotype: "
					"SQL_DROP_TRANSLATION\n");
			// FIXME: not sure about this...
			val.uintval=0;
				//SQL_DTR_DROP_TRANSLATION;
			type=1;
			break;
		case SQL_DROP_VIEW:
			debugPrintf("  infotype: "
					"SQL_DROP_VIEW\n");
			// FIXME: is this true for all db's?
			val.uintval=SQL_DV_DROP_VIEW|
					SQL_DV_CASCADE|
					SQL_DV_RESTRICT;
			type=1;
			break;
		case SQL_DYNAMIC_CURSOR_ATTRIBUTES1:
			debugPrintf("  infotype: "
					"SQL_DYNAMIC_CURSOR_ATTRIBUTES1\n");
			// for now...
			val.uintval=SQL_CA1_NEXT|SQL_CA1_POS_POSITION;
			type=1;
			break;
		case SQL_DYNAMIC_CURSOR_ATTRIBUTES2:
			debugPrintf("  infotype: "
					"SQL_DYNAMIC_CURSOR_ATTRIBUTES2\n");
			val.uintval=SQL_CA2_READ_ONLY_CONCURRENCY;
			type=1;
			break;
		case SQL_FORWARD_ONLY_CURSOR_ATTRIBUTES1:
			debugPrintf("  infotype: "
				"SQL_FORWARD_ONLY_CURSOR_ATTRIBUTES1\n");
			// for now...
			val.uintval=SQL_CA1_NEXT|SQL_CA1_POS_POSITION;
			type=1;
			break;
		case SQL_FORWARD_ONLY_CURSOR_ATTRIBUTES2:
			debugPrintf("  infotype: "
				"SQL_FORWARD_ONLY_CURSOR_ATTRIBUTES2\n");
			val.uintval=SQL_CA2_READ_ONLY_CONCURRENCY;
			type=1;
			break;
		case SQL_INDEX_KEYWORDS:
			debugPrintf("  infotype: "
					"SQL_INDEX_KEYWORDS\n");
			// FIXME: is this true for all db's?
			val.uintval=SQL_IK_ALL;
			type=1;
			break;
		case SQL_INFO_SCHEMA_VIEWS:
			debugPrintf("  infotype: "
					"SQL_INFO_SCHEMA_VIEWS\n");
			// FIXME: this isn't true for all db's
			val.uintval=0;
				/*SQL_ISV_ASSERTIONS
				SQL_ISV_CHARACTER_SETS
				SQL_ISV_CHECK_CONSTRAINTS
				SQL_ISV_COLLATIONS
				SQL_ISV_COLUMN_DOMAIN_USAGE
				SQL_ISV_COLUMN_PRIVILEGES
				SQL_ISV_COLUMNS
				SQL_ISV_CONSTRAINT_COLUMN_USAGE
				SQL_ISV_CONSTRAINT_TABLE_USAGE
				SQL_ISV_DOMAIN_CONSTRAINTS
				SQL_ISV_DOMAINS
				SQL_ISV_KEY_COLUMN_USAGE
				SQL_ISV_REFERENTIAL_CONSTRAINTS
				SQL_ISV_SCHEMATA
				SQL_ISV_SQL_LANGUAGES
				SQL_ISV_TABLE_CONSTRAINTS
				SQL_ISV_TABLE_PRIVILEGES
				SQL_ISV_TABLES
				SQL_ISV_TRANSLATIONS
				SQL_ISV_USAGE_PRIVILEGES
				SQL_ISV_VIEW_COLUMN_USAGE
				SQL_ISV_VIEW_TABLE_USAGE
				SQL_ISV_VIEWS;*/
			type=1;
			break;
		case SQL_KEYSET_CURSOR_ATTRIBUTES1:
			debugPrintf("  infotype: "
					"SQL_KEYSET_CURSOR_ATTRIBUTES1\n");
			// for now...
			val.uintval=SQL_CA1_NEXT|SQL_CA1_POS_POSITION;
			type=1;
			break;
		case SQL_KEYSET_CURSOR_ATTRIBUTES2:
			debugPrintf("  infotype: "
					"SQL_KEYSET_CURSOR_ATTRIBUTES2\n");
			val.uintval=SQL_CA2_READ_ONLY_CONCURRENCY;
			type=1;
			break;
		case SQL_MAX_ASYNC_CONCURRENT_STATEMENTS:
			debugPrintf("  infotype: "
				"SQL_MAX_ASYNC_CONCURRENT_STATEMENTS\n");
			// 0 means no max or unknown
			val.uintval=0;
			type=1;
			break;
		case SQL_ODBC_INTERFACE_CONFORMANCE:
			debugPrintf("  infotype: "
					"SQL_ODBC_INTERFACE_CONFORMANCE\n");
			val.uintval=SQL_OIC_CORE;
			type=1;
			break;
		case SQL_PARAM_ARRAY_ROW_COUNTS:
			debugPrintf("  infotype: "
					"SQL_PARAM_ARRAY_ROW_COUNTS\n");
			// batch sql is not supported
			val.uintval=0;
			type=1;
			break;
		case SQL_PARAM_ARRAY_SELECTS:
			debugPrintf("  infotype: "
					"SQL_PARAM_ARRAY_SELECTS\n");
			// for now...
			val.uintval=SQL_PAS_NO_SELECT;
			type=1;
			break;
		case SQL_SQL92_DATETIME_FUNCTIONS:
			debugPrintf("  infotype: "
					"SQL_SQL92_DATETIME_FUNCTIONS\n");
			// FIXME: this isn't true for all db's
			val.uintval=SQL_SDF_CURRENT_DATE|
					SQL_SDF_CURRENT_TIME|
					SQL_SDF_CURRENT_TIMESTAMP;
			type=1;
			break;
		case SQL_SQL92_FOREIGN_KEY_DELETE_RULE:
			debugPrintf("  infotype: "
					"SQL_SQL92_FOREIGN_KEY_DELETE_RULE\n");
			// FIXME: this isn't true for all db's
			val.uintval=SQL_SFKD_CASCADE;
			type=1;
			break;
		case SQL_SQL92_FOREIGN_KEY_UPDATE_RULE:
			debugPrintf("  infotype: "
					"SQL_SQL92_FOREIGN_KEY_UPDATE_RULE\n");
			// FIXME: this isn't true for all db's
			val.uintval=SQL_SFKU_CASCADE;
			type=1;
			break;
		case SQL_SQL92_GRANT:
			debugPrintf("  infotype: "
					"SQL_SQL92_GRANT\n");
			// FIXME: this isn't true for all db's
			val.uintval=SQL_SG_DELETE_TABLE|
					SQL_SG_INSERT_COLUMN|
					SQL_SG_INSERT_TABLE|
					SQL_SG_REFERENCES_TABLE|
					SQL_SG_REFERENCES_COLUMN|
					SQL_SG_SELECT_TABLE|
					SQL_SG_UPDATE_COLUMN|
					SQL_SG_UPDATE_TABLE|
					SQL_SG_USAGE_ON_DOMAIN|
					SQL_SG_USAGE_ON_CHARACTER_SET|
					SQL_SG_USAGE_ON_COLLATION|
					SQL_SG_USAGE_ON_TRANSLATION|
					SQL_SG_WITH_GRANT_OPTION;
			type=1;
			break;
		case SQL_SQL92_NUMERIC_VALUE_FUNCTIONS:
			debugPrintf("  infotype: "
					"SQL_SQL92_NUMERIC_VALUE_FUNCTIONS\n");
			// FIXME: this isn't true for all db's
			val.uintval=SQL_SNVF_BIT_LENGTH|
					SQL_SNVF_CHAR_LENGTH|
					SQL_SNVF_CHARACTER_LENGTH|
					SQL_SNVF_EXTRACT|
					SQL_SNVF_OCTET_LENGTH|
					SQL_SNVF_POSITION;
			type=1;
			break;
		case SQL_SQL92_PREDICATES:
			debugPrintf("  infotype: "
					"SQL_SQL92_PREDICATES\n");
			// FIXME: this isn't true for all db's
			val.uintval=SQL_SP_BETWEEN|
					SQL_SP_COMPARISON|
					SQL_SP_EXISTS|
					SQL_SP_IN|
					SQL_SP_ISNOTNULL|
					SQL_SP_ISNULL|
					SQL_SP_LIKE|
					SQL_SP_MATCH_FULL|
					SQL_SP_MATCH_PARTIAL|
					SQL_SP_MATCH_UNIQUE_FULL|
					SQL_SP_MATCH_UNIQUE_PARTIAL|
					SQL_SP_OVERLAPS|
					SQL_SP_QUANTIFIED_COMPARISON|
					SQL_SP_UNIQUE;
			type=1;
			break;
		case SQL_SQL92_RELATIONAL_JOIN_OPERATORS:
			debugPrintf("  infotype: "
				"SQL_SQL92_RELATIONAL_JOIN_OPERATORS\n");
			// FIXME: this isn't true for all db's
			val.uintval=SQL_SRJO_CORRESPONDING_CLAUSE|
					SQL_SRJO_CROSS_JOIN|
					SQL_SRJO_EXCEPT_JOIN|
					SQL_SRJO_FULL_OUTER_JOIN|
					SQL_SRJO_INNER_JOIN|
					SQL_SRJO_INTERSECT_JOIN|
					SQL_SRJO_LEFT_OUTER_JOIN|
					SQL_SRJO_NATURAL_JOIN|
					SQL_SRJO_RIGHT_OUTER_JOIN|
					SQL_SRJO_UNION_JOIN;
			type=1;
			break;
		case SQL_SQL92_REVOKE:
			debugPrintf("  infotype: "
					"SQL_SQL92_REVOKE\n");
			// FIXME: this isn't true for all db's
			val.uintval=SQL_SR_CASCADE|
					SQL_SR_DELETE_TABLE|
					SQL_SR_GRANT_OPTION_FOR|
					SQL_SR_INSERT_COLUMN|
					SQL_SR_INSERT_TABLE|
					SQL_SR_REFERENCES_COLUMN|
					SQL_SR_REFERENCES_TABLE|
					SQL_SR_RESTRICT|
					SQL_SR_SELECT_TABLE|
					SQL_SR_UPDATE_COLUMN|
					SQL_SR_UPDATE_TABLE|
					SQL_SR_USAGE_ON_DOMAIN|
					SQL_SR_USAGE_ON_CHARACTER_SET|
					SQL_SR_USAGE_ON_COLLATION|
					SQL_SR_USAGE_ON_TRANSLATION;
			type=1;
			break;
		case SQL_SQL92_ROW_VALUE_CONSTRUCTOR:
			debugPrintf("  infotype: "
					"SQL_SQL92_ROW_VALUE_CONSTRUCTOR\n");
			// FIXME: this isn't true for all db's
			val.uintval=SQL_SRVC_VALUE_EXPRESSION|
					SQL_SRVC_NULL|
					SQL_SRVC_DEFAULT|
					SQL_SRVC_ROW_SUBQUERY;
			type=1;
			break;
		case SQL_SQL92_STRING_FUNCTIONS:
			debugPrintf("  infotype: "
					"SQL_SQL92_STRING_FUNCTIONS\n");
			// FIXME: this isn't true for all db's
			val.uintval=SQL_SSF_CONVERT|
					SQL_SSF_LOWER|
					SQL_SSF_UPPER|
					SQL_SSF_SUBSTRING|
					SQL_SSF_TRANSLATE|
					SQL_SSF_TRIM_BOTH|
					SQL_SSF_TRIM_LEADING|
					SQL_SSF_TRIM_TRAILING;
			type=1;
			break;
		case SQL_SQL92_VALUE_EXPRESSIONS:
			debugPrintf("  infotype: "
					"SQL_SQL92_VALUE_EXPRESSIONS\n");
			// FIXME: this isn't true for all db's
			val.uintval=SQL_SVE_CASE|
					SQL_SVE_CAST|
					SQL_SVE_COALESCE|
					SQL_SVE_NULLIF;
			type=1;
			break;
		case SQL_STANDARD_CLI_CONFORMANCE:
			debugPrintf("  infotype: "
					"SQL_STANDARD_CLI_CONFORMANCE\n");
			// FIXME: no idea, conservative guess...
			val.uintval=SQL_SCC_XOPEN_CLI_VERSION1;
					//SQL_SCC_ISO92_CLI;
			type=1;
			break;
		case SQL_STATIC_CURSOR_ATTRIBUTES1:
			debugPrintf("  infotype: "
					"SQL_STATIC_CURSOR_ATTRIBUTES1\n");
			// for now...
			val.uintval=SQL_CA1_NEXT|SQL_CA1_POS_POSITION;
			type=1;
			break;
		case SQL_STATIC_CURSOR_ATTRIBUTES2:
			debugPrintf("  infotype: "
					"SQL_STATIC_CURSOR_ATTRIBUTES2\n");
			val.uintval=SQL_CA2_READ_ONLY_CONCURRENCY;
			type=1;
			break;
		case SQL_AGGREGATE_FUNCTIONS:
			debugPrintf("  infotype: "
					"SQL_AGGREGATE_FUNCTIONS\n");
			// FIXME: is this true for all db's?
			val.uintval=SQL_AF_ALL|
					SQL_AF_AVG|
					SQL_AF_COUNT|
					SQL_AF_DISTINCT|
					SQL_AF_MAX|
					SQL_AF_MIN|
					SQL_AF_SUM;
			type=1;
			break;
		case SQL_DDL_INDEX:
			debugPrintf("  infotype: "
					"SQL_DDL_INDEX\n");
			val.uintval=SQL_DI_CREATE_INDEX|SQL_DI_DROP_INDEX;
			type=1;
			break;
		case SQL_DM_VER:
			debugPrintf("  infotype: "
					"SQL_DM_VER\n");
			// Really, only the driver manager should
			// implement this.  But, for consistency...
			val.strval="03.80";
			type=0;
			break;
		case SQL_INSERT_STATEMENT:
			debugPrintf("  infotype: "
					"SQL_INSERT_STATEMENT\n");
			// FIXME: is this true for all db's?
			val.uintval=SQL_IS_INSERT_LITERALS|
					SQL_IS_INSERT_SEARCHED|
					SQL_IS_SELECT_INTO;
			type=1;
			break;
		#if (ODBCVER >= 0x0380)
		case SQL_ASYNC_DBC_FUNCTIONS:
			debugPrintf("  infotype: "
					"SQL_ASYNC_DBC_FUNCTIONS\n");
			// for now...
			val.uintval=SQL_ASYNC_DBC_NOT_CAPABLE;
			type=1;
			break;
		#endif
		#endif
		#ifdef SQL_DTC_TRANSITION_COST
		case SQL_DTC_TRANSITION_COST:
			debugPrintf("  unsupported infotype: "
					"SQL_DTC_TRANSITION_COST\n");
			break;
		#endif
		default:
			debugPrintf("  invalid infotype: %d\n",infotype);
			SQLR_CONNSetError(conn,
				"Optional field not implemented",0,"HYC00");
			return SQL_ERROR;
	}


	// copy out the value and length
	SQLSMALLINT	valuelength=0;
	switch (type) {
		case -1:
			debugPrintf("  (not copying out any value)\n");
			break;
		case 0:
			debugPrintf("  strval: %s\n",val.strval);
			valuelength=charstring::length(val.strval);
			debugPrintf("  bufferlength: %d\n",(int)bufferlength);
			if (infovalue && bufferlength) {

				charstring::safeCopy((char *)infovalue,
							bufferlength,
							val.strval);

				// make sure to null-terminate
				// (even if data has to be truncated)
				((char *)infovalue)[bufferlength-1]='\0';

				if (valuelength>bufferlength) {
					debugPrintf("  WARNING! valuelength>"
							"bufferlength\n");
				}
			} else {
				if (!infovalue) {
					debugPrintf("  NULL infovalue "
						"(not copying out strval)\n");
				}
				if (!bufferlength) {
					debugPrintf("  0 bufferlength "
						"(not copying out strval)\n");
				}
			}
			break;
		case 1:
			debugPrintf("  uintval: %d\n",val.uintval);
			valuelength=sizeof(SQLUINTEGER);
			if (infovalue) {
				*((SQLUINTEGER *)infovalue)=val.uintval;
			} else {
				debugPrintf("  NULL infovalue "
						"(not copying out uintval)\n");
			}
			break;
		case 2:
			debugPrintf("  usmallintval: %d\n",val.usmallintval);
			valuelength=sizeof(SQLUSMALLINT);
			if (infovalue) {
				*((SQLUSMALLINT *)infovalue)=val.usmallintval;
			} else {
				debugPrintf("  NULL infovalue "
						"(not copying out "
						"usmallintval)\n");
			}
			break;
	}
	debugPrintf("  valuelength: %d\n",(int)valuelength);
	if (stringlength) {
		*stringlength=valuelength;
	} else {
		debugPrintf("  NULL stringlength "
					"(not copying out valuelength)\n");
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
		debugPrintf("  NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	union {
		rowdesc		*rowdescptrval;
		paramdesc	*paramdescptrval;
		SQLULEN		ulenval;
		SQLUSMALLINT	*usmallintptrval;
		SQLROWSETSIZE	*rowsetsizeptrval;
	} val;
	int16_t	type=-1;

	switch (attribute) {
		#if (ODBCVER >= 0x0300)
		case SQL_ATTR_APP_ROW_DESC:
			debugPrintf("  attribute: "
					"SQL_ATTR_APP_ROW_DESC\n");
			val.rowdescptrval=stmt->approwdesc;
			type=0;
			break;
		case SQL_ATTR_APP_PARAM_DESC:
			debugPrintf("  attribute: "
					"SQL_ATTR_APP_PARAM_DESC\n");
			val.paramdescptrval=stmt->appparamdesc;
			type=1;
			break;
		case SQL_ATTR_IMP_ROW_DESC:
			debugPrintf("  attribute: "
					"SQL_ATTR_IMP_ROW_DESC\n");
			val.rowdescptrval=stmt->improwdesc;
			type=0;
			break;
		case SQL_ATTR_IMP_PARAM_DESC:
			debugPrintf("  attribute: "
					"SQL_ATTR_IMP_PARAM_DESC\n");
			val.paramdescptrval=stmt->impparamdesc;
			type=1;
			break;
		case SQL_ATTR_CURSOR_SCROLLABLE:
			debugPrintf("  attribute: "
					"SQL_ATTR_CURSOR_SCROLLABLE\n");
			val.ulenval=SQL_NONSCROLLABLE;
			type=2;
			break;
		case SQL_ATTR_CURSOR_SENSITIVITY:
			debugPrintf("  attribute: "
					"SQL_ATTR_CURSOR_SENSITIVITY\n");
			val.ulenval=SQL_UNSPECIFIED;
			type=2;
			break;
		#endif
		//case SQL_ATTR_QUERY_TIMEOUT:
		case SQL_QUERY_TIMEOUT:
			debugPrintf("  attribute: "
					"SQL_ATTR_QUERY_TIMEOUT/"
					"SQL_QUERY_TIMEOUT\n");
			val.ulenval=0;
			type=2;
			break;
		//case SQL_ATTR_MAX_ROWS:
		case SQL_MAX_ROWS:
			debugPrintf("  attribute: "
					"SQL_ATTR_MAX_ROWS/"
					"SQL_MAX_ROWS:\n");
			val.ulenval=0;
			type=2;
			break;
		//case SQL_ATTR_NOSCAN:
		case SQL_NOSCAN:
			debugPrintf("  attribute: "
					"SQL_ATTR_NOSCAN/"
					"SQL_NOSCAN\n");
			// FIXME: is this true for all db's?
			val.ulenval=SQL_NOSCAN_OFF;
			type=2;
			break;
		//case SQL_ATTR_MAX_LENGTH:
		case SQL_MAX_LENGTH:
			debugPrintf("  attribute: "
					"SQL_ATTR_MAX_LENGTH/"
					"SQL_MAX_LENGTH\n");
			val.ulenval=0;
			type=2;
			break;
		//case SQL_ATTR_ASYNC_ENABLE:
		case SQL_ASYNC_ENABLE:
			debugPrintf("  attribute: "
					"SQL_ATTR_ASYNC_ENABLE/"
					"SQL_ASYNC_ENABLE\n");
			val.ulenval=SQL_ASYNC_ENABLE_OFF;
			type=2;
			break;
		//case SQL_ATTR_ROW_BIND_TYPE:
		case SQL_BIND_TYPE:
			debugPrintf("  attribute: "
					"SQL_ATTR_ROW_BIND_TYPE/"
					"SQL_BIND_TYPE\n");
			val.ulenval=stmt->rowbindtype;
			type=2;
			break;
		//case SQL_ATTR_CONCURRENCY:
		//case SQL_ATTR_CURSOR_TYPE:
		case SQL_CURSOR_TYPE:
			debugPrintf("  attribute: "
					"SQL_ATTR_CONCURRENCY/"
					"SQL_ATTR_CURSOR_TYPE/"
					"SQL_CURSOR_TYPE\n");
			val.ulenval=SQL_CURSOR_FORWARD_ONLY;
			type=2;
			break;
		case SQL_CONCURRENCY:
			debugPrintf("  attribute: "
					"SQL_CONCURRENCY\n");
			val.ulenval=SQL_CONCUR_READ_ONLY;
			type=2;
			break;
		//case SQL_ATTR_KEYSET_SIZE:
		case SQL_KEYSET_SIZE:
			debugPrintf("  attribute: "
					"SQL_ATTR_KEYSET_SIZE/"
					"SQL_KEYSET_SIZE\n");
			val.ulenval=0;
			type=2;
			break;
		case SQL_ROWSET_SIZE:
			debugPrintf("  attribute: "
					"SQL_ROWSET_SIZE\n");
			val.ulenval=stmt->rowsetsize;
			type=2;
			break;
		//case SQL_ATTR_SIMULATE_CURSOR:
		case SQL_SIMULATE_CURSOR:
			debugPrintf("  attribute: "
					"SQL_ATTR_SIMULATE_CURSOR/"
					"SQL_SIMULATE_CURSOR\n");
			// FIXME: I'm not sure this is true...
			val.ulenval=SQL_SC_UNIQUE;
			type=2;
			break;
		//case SQL_ATTR_RETRIEVE_DATA:
		case SQL_RETRIEVE_DATA:
			debugPrintf("  attribute: "
					"SQL_ATTR_RETRIEVE_DATA/"
					"SQL_RETRIEVE_DATA\n");
			val.ulenval=SQL_RD_ON;
			type=2;
			break;
		//case SQL_ATTR_USE_BOOKMARKS:
		case SQL_USE_BOOKMARKS:
			debugPrintf("  attribute: "
					"SQL_ATTR_USE_BOOKMARKS/"
					"SQL_USE_BOOKMARKS\n");
			val.ulenval=SQL_UB_OFF;
			type=2;
			break;
		case SQL_GET_BOOKMARK:
			debugPrintf("  unsupported attribute: "
					"SQL_GET_BOOKMARK\n");
			// FIXME: implement
			break;
		// case SQL_ATTR_ROW_NUMBER
		case SQL_ROW_NUMBER:
			debugPrintf("  attribute: "
					"SQL_ATTR_ROW_NUMBER/"
					"SQL_ROW_NUMBER\n");
			// FIXME: implement
			break;
		#if (ODBCVER >= 0x0300)
		case SQL_ATTR_ENABLE_AUTO_IPD:
			debugPrintf("  attribute: "
					"SQL_ATTR_ENABLE_AUTO_IPD\n");
			val.ulenval=SQL_TRUE;
			type=2;
			break;
		case SQL_ATTR_FETCH_BOOKMARK_PTR:
			debugPrintf("  unsupported attribute: "
					"SQL_ATTR_FETCH_BOOKMARK_PTR\n");
			// FIXME: implement
			break;
		case SQL_ATTR_PARAM_BIND_OFFSET_PTR:
			debugPrintf("  unsupported attribute: "
					"SQL_ATTR_PARAM_BIND_OFFSET_PTR\n");
			// FIXME: implement
			break;
		case SQL_ATTR_PARAM_BIND_TYPE:
			debugPrintf("  unsupported attribute: "
					"SQL_ATTR_PARAM_BIND_TYPE\n");
			// FIXME: implement
			break;
		case SQL_ATTR_PARAM_OPERATION_PTR:
			debugPrintf("  unsupported attribute: "
					"SQL_ATTR_PARAM_OPERATION_PTR\n");
			// FIXME: implement
			break;
		case SQL_ATTR_PARAM_STATUS_PTR:
			debugPrintf("  unsupported attribute: "
					"SQL_ATTR_PARAM_STATUS_PTR\n");
			// FIXME: implement
			break;
		case SQL_ATTR_PARAMS_PROCESSED_PTR:
			debugPrintf("  unsupported attribute: "
					"SQL_ATTR_PARAMS_PROCESSED_PTR\n");
			// FIXME: implement
			break;
		case SQL_ATTR_PARAMSET_SIZE:
			debugPrintf("  unsupported attribute: "
					"SQL_ATTR_PARAMSET_SIZE\n");
			val.ulenval=1;
			type=2;
			break;
		case SQL_ATTR_ROW_BIND_OFFSET_PTR:
			debugPrintf("  unsupported attribute: "
					"SQL_ATTR_ROW_BIND_OFFSET_PTR\n");
			// FIXME: implement
			break;
		case SQL_ATTR_ROW_OPERATION_PTR:
			debugPrintf("  unsupported attribute: "
					"SQL_ATTR_ROW_OPERATION_PTR\n");
			// FIXME: implement
			break;
		case SQL_ATTR_ROW_STATUS_PTR:
			debugPrintf("  attribute: "
					"SQL_ATTR_ROW_STATUS_PTR\n");
			val.usmallintptrval=stmt->rowstatusptr;
			type=3;
			break;
		case SQL_ATTR_ROWS_FETCHED_PTR:
			debugPrintf("  attribute: "
					"SQL_ATTR_ROWS_FETCHED_PTR\n");
			val.rowsetsizeptrval=stmt->rowsfetchedptr;
			type=4;
			break;
		case SQL_ATTR_ROW_ARRAY_SIZE:
			debugPrintf("  attribute: "
					"SQL_ATTR_ROW_ARRAY_SIZE\n");
			val.ulenval=stmt->rowarraysize;
			type=2;
			break;
		#endif
		#if (ODBCVER < 0x0300)
		case SQL_STMT_OPT_MAX:
			debugPrintf("  unsupported attribute: "
					"SQL_STMT_OPT_MAX\n");
			// FIXME: implement
			break;
		case SQL_STMT_OPT_MIN:
			debugPrintf("  unsupported attribute: "
					"SQL_STMT_OPT_MIN\n");
			// FIXME: implement
			break;
		#endif
		default:
			debugPrintf("  invalid attribute: %d\n",attribute);
			SQLR_STMTSetError(stmt,
				"Optional field not implemented",0,"HYC00");
			return SQL_ERROR;
	}


	// copy out the value and length
	SQLSMALLINT	valuelength=0;
	switch (type) {
		case -1:
			debugPrintf("  (not copying out any value)\n");
			break;
		case 0:
			if (value) {
				*((rowdesc **)value)=val.rowdescptrval;
				valuelength=sizeof(rowdesc *);
			} else {
				debugPrintf("  NULL value "
						"(not copying out data)\n");
			}
			break;
		case 1:
			if (value) {
				*((paramdesc **)value)=val.paramdescptrval;
				valuelength=sizeof(paramdesc *);
			} else {
				debugPrintf("  NULL value "
						"(not copying out data)\n");
			}
			break;
		case 2:
			if (value) {
				*((SQLULEN *)value)=val.ulenval;
				valuelength=sizeof(SQLULEN);
			} else {
				debugPrintf("  NULL value "
						"(not copying out data)\n");
			}
			break;
		case 3:
			if (value) {
				*((SQLUSMALLINT **)value)=val.usmallintptrval;
				valuelength=sizeof(SQLUSMALLINT *);
			} else {
				debugPrintf("  NULL value "
						"(not copying out data)\n");
			}
			break;
		case 4:
			if (value) {
				*((SQLROWSETSIZE **)value)=val.rowsetsizeptrval;
				valuelength=sizeof(SQLROWSETSIZE *);
			} else {
				debugPrintf("  NULL value "
						"(not copying out data)\n");
			}
			break;
	}
	debugPrintf("  valuelength: %d\n",(int)valuelength);
	if (stringlength) {
		*stringlength=valuelength;
	} else {
		debugPrintf("  NULL stringlength "
					"(not copying out valuelength)\n");
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
					SQLSMALLINT type) {
	debugFunction();

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("  NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	debugPrintf("  type: %d\n",type);

	// remap date/time types to the appropriate odbc2/3 type
	if (stmt->conn->env->odbcversion==SQL_OV_ODBC2) {
		switch (type) {
			case SQL_TYPE_DATE:
				type=SQL_DATE;
				break;
			case SQL_TYPE_TIME:
				type=SQL_TIME;
				break;
			case SQL_TYPE_TIMESTAMP:
				type=SQL_TIMESTAMP;
				break;
		}
	} else {
		switch (type) {
			case SQL_DATE:
				type=SQL_TYPE_DATE;
				break;
			case SQL_TIME:
				type=SQL_TYPE_TIME;
				break;
			case SQL_TIMESTAMP:
				type=SQL_TYPE_TIMESTAMP;
				break;
		}
	}
	debugPrintf("  remapped type: %d\n",type);

	// reinit row indices
	stmt->currentfetchrow=0;
	stmt->currentstartrow=0;
	stmt->currentgetdatarow=0;

	// clear the error
	SQLR_STMTClearError(stmt);

	// map the numeric type to a string
	const char	*typestring="";
	switch (type) {
		case SQL_CHAR:
			typestring="CHAR";
			break;
		case SQL_VARCHAR:
			typestring="VARCHAR";
			break;
		case SQL_LONGVARCHAR:
			typestring="LONGVARCHAR";
			break;
		#ifdef SQL_WCHAR
		case SQL_WCHAR:
			typestring="WCHAR";
			break;
		#endif
		#ifdef SQL_WVARCHAR
		case SQL_WVARCHAR:
			typestring="WVARCHAR";
			break;
		#endif
		#ifdef SQL_WLONGVARCHAR
		case SQL_WLONGVARCHAR:
			typestring="WLONGVARCHAR";
			break;
		#endif
		case SQL_DECIMAL:
			typestring="DECIMAL";
			break;
		case SQL_NUMERIC:
			typestring="NUMERIC";
			break;
		case SQL_SMALLINT:
			typestring="SMALLINT";
			break;
		case SQL_INTEGER:
			typestring="INTEGER";
			break;
		case SQL_REAL:
			typestring="REAL";
			break;
		case SQL_FLOAT:
			typestring="FLOAT";
			break;
		case SQL_DOUBLE:
			typestring="DOUBLE";
			break;
		case SQL_DATE:
		// case SQL_DATETIME:
		// 	(ODBC 3 dup of SQL_DATE)
			typestring="DATE";
			break;
		case SQL_TIME:
		// case SQL_INTERVAL:
		// 	(ODBC 3 dup of SQL_TIME)
			typestring="TIME";
			break;
		case SQL_TIMESTAMP:
			typestring="TIMESTAMP";
			break;
		case SQL_BIT:
			typestring="BIT";
			break;
		case SQL_TINYINT:
			typestring="TINYINT";
			break;
		case SQL_BIGINT:
			typestring="BIGINT";
			break;
		case SQL_BINARY:
			typestring="BINARY";
			break;
		case SQL_VARBINARY:
			typestring="VARBINARY";
			break;
		case SQL_LONGVARBINARY:
			typestring="LONGVARBINARY";
			break;
		case SQL_TYPE_DATE:
			typestring="TYPE_DATE";
			break;
		case SQL_TYPE_TIME:
			typestring="TYPE_TIME";
			break;
		case SQL_TYPE_TIMESTAMP:
			typestring="TYPE_TIMESTAMP";
			break;
		#ifdef SQL_TYPE_UTCDATETIME
		case SQL_TYPE_UTCDATETIME:
			typestring="TYPE_UTCDATETIME";
			break;
		#endif
		#ifdef SQL_TYPE_UTCTIME
		case SQL_TYPE_UCTTIME:
			typestring="TYPE_UTCTIME";
			break;
		#endif
		case SQL_INTERVAL_MONTH:
			typestring="INTERVAL_MONTH";
			break;
		case SQL_INTERVAL_YEAR:
			typestring="INTERVAL_YEAR";
			break;
		case SQL_INTERVAL_YEAR_TO_MONTH:
			typestring="INTERVAL_YEAR_TO_MONTH";
			break;
		case SQL_INTERVAL_DAY:
			typestring="INTERVAL_DAY";
			break;
		case SQL_INTERVAL_HOUR:
			typestring="INTERVAL_HOUR";
			break;
		case SQL_INTERVAL_MINUTE:
			typestring="INTERVAL_MINUTE";
			break;
		case SQL_INTERVAL_SECOND:
			typestring="INTERVAL_SECOND";
			break;
		case SQL_INTERVAL_DAY_TO_HOUR:
			typestring="INTERVAL_DAY_TO_HOUR";
			break;
		case SQL_INTERVAL_DAY_TO_MINUTE:
			typestring="INTERVAL_DAY_TO_MINUTE";
			break;
		case SQL_INTERVAL_DAY_TO_SECOND:
			typestring="INTERVAL_DAY_TO_SECOND";
			break;
		case SQL_INTERVAL_HOUR_TO_MINUTE:
			typestring="INTERVAL_HOUR_TO_MINUTE";
			break;
		case SQL_INTERVAL_HOUR_TO_SECOND:
			typestring="INTERVAL_HOUR_TO_SECOND";
			break;
		case SQL_INTERVAL_MINUTE_TO_SECOND:
			typestring="INTERVAL_MINUTE_TO_SECOND";
			break;
		case SQL_GUID:
			typestring="GUID";
			break;
		case SQL_ALL_TYPES:
			typestring="*";
			break;
		default:
			typestring="";
			break;
	}

	// get the type info
	SQLRETURN	retval=(stmt->cur->getTypeInfoList(typestring,NULL,
						SQLRCLIENTLISTFORMAT_ODBC))?
							SQL_SUCCESS:SQL_ERROR;

	// the statement has been executed
	stmt->executed=true;
	stmt->nodata=false;

	debugPrintf("  %s\n",(retval==SQL_SUCCESS)?"success":"error");

	// handle errors
	if (retval!=SQL_SUCCESS) {
		SQLR_STMTSetError(stmt,stmt->cur->errorMessage(),
					stmt->cur->errorNumber(),NULL);
	}
	return retval;
}

SQLRETURN SQL_API SQLNumResultCols(SQLHSTMT statementhandle,
					SQLSMALLINT *columncount) {
	debugFunction();

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("  NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	SQLRETURN	result=SQL_SUCCESS;

	// Some db's apparently support this after the prepare phase, prior
	// to execution.  SQL Relay doesn't but we can fake that by executing
	// here and bypassing the next attempt to execute.
	if (!stmt->executed) {
		debugPrintf("  not executed yet...\n");
		stmt->executedbynumresultcolsresult=SQLR_SQLExecute(stmt);
		stmt->executedbynumresultcols=true;
		result=stmt->executedbynumresultcolsresult;
	}

	*columncount=(SQLSMALLINT)stmt->cur->colCount();
	debugPrintf("  columncount: %d\n",(int)*columncount);

	return result;
}

SQLRETURN SQL_API SQLParamData(SQLHSTMT statementhandle,
					SQLPOINTER *value) {
	debugFunction();

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("  NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	// We're faking SQLBindParameter data-at-exec by:
	// * tracking the data-at-exec binds in SQLBindParameter
	// * deferring SQLExecDirect/Execute
	// * buffering the data locally in SQLParamData/SQLPutData
	// * binding the buffered data data
	// * then finally actually running SQLExecDirect/Execute

	// bail if there is no data-at-exec
	if (!stmt->dataatexec) {
		debugPrintf("  no data-at-exec detected\n");
		return SQL_SUCCESS;
	}

	// if there's an existing putdata then bind it
	if (stmt->putdatabind) {
		// FIXME: for now we only support SQL_C_DATA, but eventually
		// we'll need to do some type-checking here and handle different
		// data types in different ways
		char	*parametername=charstring::parseNumber(
						stmt->putdatabind);
		debugPrintf("  parametername: %s\n",
					parametername);
		debugPrintf("  value: \"%.*s\"\n",
					stmt->putdatabuffer.getSize(),
					stmt->putdatabuffer.getBuffer());
		stmt->cur->inputBind(parametername,
				(const char *)stmt->putdatabuffer.getBuffer(),
				stmt->putdatabuffer.getSize());
		delete[] parametername;
	}

	// get the data-at-exec keys (parameter numbers)
	if (!stmt->dataatexeckeys) {
		debugPrintf("  getting keys\n");
		stmt->dataatexeckeys=stmt->dataatexecdict.getKeys();
	}

	// if there's a data-at-exec buffer then return it,
	// and remove it from the dictionary
	if (stmt->dataatexeckeys->getLength()) {

		// get the first bind number
		linkedlistnode<SQLUSMALLINT>	*keynode=
					stmt->dataatexeckeys->getFirst();

		// get the corresponding bind buffer node
		dictionarynode<SQLUSMALLINT,SQLPOINTER>	*valuenode=
					stmt->dataatexecdict.getNode(
							keynode->getValue());

		// keep track of the bind number
		stmt->putdatabind=keynode->getValue();
		debugPrintf("  put-data bind: %d\n",stmt->putdatabind);

		// get the bind buffer
		SQLPOINTER	val=valuenode->getValue();

		// pass the buffer out
		if (value) {
			*value=val;
		}

		// remove the buffer and key
		stmt->dataatexecdict.remove(valuenode);
		stmt->dataatexeckeys->remove(keynode);

		// reset the put data buffer
		stmt->putdatabuffer.clear();

		// return "need data"
		debugPrintf("  SQL NEED DATA\n");
		return SQL_NEED_DATA;
	}

	// reset the various data-at-exec related things
	// (do this prior to execute to prevent looping forever)
	// FIXME: also reset in SQLFreeStmt?
	stmt->dataatexec=false;
	delete stmt->dataatexeckeys;
	stmt->dataatexeckeys=NULL;
	stmt->putdatabind=0;
	stmt->putdatabuffer.clear();

	// exec/exec-direct will have been deferred until now...
	SQLRETURN	retval=SQL_ERROR;
	if (stmt->dataatexecstatement) {
		// if we have a query then exec-direct it
		debugPrintf("  exec-direc'ing...\n");
		retval=SQLR_SQLExecDirect(
					statementhandle,
					stmt->dataatexecstatement,
					stmt->dataatexecstatementlength);

		// reset statement/length
		stmt->dataatexecstatement=NULL;
		stmt->dataatexecstatementlength=0;
	} else {
		// otherwise just execute
		debugPrintf("  exececuting...\n");
		retval=SQLR_SQLExecute(statementhandle);
	}

	return retval;
}

SQLRETURN SQL_API SQLPrepare(SQLHSTMT statementhandle,
					SQLCHAR *statementtext,
					SQLINTEGER textlength) {
	debugFunction();

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("  NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	// trim query
	uint32_t	statementtextlength=SQLR_TrimQuery(
						statementtext,textlength);

	// prepare the query
	#ifdef DEBUG_MESSAGES
	stringbuffer	debugstr;
	debugstr.append(statementtext,statementtextlength);
	debugPrintf("  statement: \"%s\",%d)\n",
			debugstr.getString(),(int)statementtextlength);
	#endif
	stmt->cur->prepareQuery((const char *)statementtext,
						statementtextlength);

	// the statement has not been executed yet
	stmt->executed=false;
	stmt->executedbynumresultcols=false;

	return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLPutData(SQLHSTMT statementhandle,
					SQLPOINTER data,
					SQLLEN strlen_or_ind) {
	debugFunction();

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("  NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	// We're faking SQLBindParameter data-at-exec below.
	// See SQLParamData for more details.

	// deal with invalid buffers/lengths
	if (strlen_or_ind<0 && strlen_or_ind!=SQL_NTS) {
		SQLR_STMTSetError(stmt,
			"Invalid string or buffer length",0,"HY090");
	}
	if (strlen_or_ind && !data) {
		SQLR_STMTSetError(stmt,
			"Invalid use of null pointer",0,"HY009");
	}

	// handle null-terminated strings
	if (strlen_or_ind==SQL_NTS) {
		strlen_or_ind=charstring::length((const char *)data);
	}

	// handle null/empty data
	if (!strlen_or_ind || strlen_or_ind==SQL_NULL_DATA) {
		debugPrintf("  strlen_or_ind 0 or NULL\n");
		debugPrintf("  not copying out any data\n");
		return SQL_SUCCESS;
	}

	// FIXME: for now we only support SQL_C_DATA, but eventually we'll
	// need to do some type-checking here and print out debug for
	// different data types in different ways
	debugPrintf("  strlen_or_ind   : %lld\n",strlen_or_ind);
	debugPrintf("  copying out data: \"%.*s\"\n",strlen_or_ind,data);

	// copy data to putdata
	stmt->putdatabuffer.append((unsigned char *)data,strlen_or_ind);

	return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLRowCount(SQLHSTMT statementhandle,
					SQLLEN *rowcount) {
	debugFunction();

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("  NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	if (rowcount) {
		*rowcount=stmt->cur->affectedRows();
		debugPrintf("  rowcount: %lld\n",(int64_t)*rowcount);
	} else {
		debugPrintf("  rowcount is null (not copying out %lld)\n",
						stmt->cur->affectedRows());
	}

	return SQL_SUCCESS;
}

static SQLRETURN SQLR_SQLSetConnectAttr(SQLHDBC connectionhandle,
					SQLINTEGER attribute,
					SQLPOINTER value,
					SQLINTEGER stringlength) {
	debugFunction();

	CONN	*conn=(CONN *)connectionhandle;
	if ((connectionhandle==SQL_NULL_HANDLE || !conn || !conn->con) &&
			(attribute==SQL_AUTOCOMMIT ||
			attribute==SQL_ATTR_METADATA_ID)) {
		debugPrintf("  attribute: %d\n",attribute);
		debugPrintf("  NULL conn handle\n");
		return SQL_INVALID_HANDLE;
	}

	// use reinterpret_cast and assignment to smaller
	// sized value to avoid compiler warnings
	SQLUINTEGER	val=reinterpret_cast<uint64_t>(value);

	switch (attribute) {
		#ifdef SQL_AUTOCOMMIT
		case SQL_AUTOCOMMIT:
		{
			debugPrintf("  attribute: SQL_AUTOCOMMIT\n");
			debugPrintf("  val: %lld\n",(uint64_t)val);
			if (val==SQL_AUTOCOMMIT_ON) {
				debugPrintf("  ON\n");
				if (conn->con->autoCommitOn()) {
					debugPrintf("  success\n");
					return SQL_SUCCESS;
				}
				SQLR_CONNSetError(conn,
					conn->con->errorMessage(),
					conn->con->errorNumber(),NULL);
				debugPrintf("  failed\n");
				return SQL_ERROR;
			} else if (val==SQL_AUTOCOMMIT_OFF) {
				debugPrintf("  OFF\n");
				if (conn->con->autoCommitOff()) {
					debugPrintf("  success\n");
					return SQL_SUCCESS;
				}
				SQLR_CONNSetError(conn,
					conn->con->errorMessage(),
					conn->con->errorNumber(),NULL);
				debugPrintf("  failed\n");
				return SQL_ERROR;
			}
			debugPrintf("  unsupported val: %d "
					"(but returning success)\n",val);
			return SQL_SUCCESS;
		}
		#endif

 		case SQL_ACCESS_MODE:
 			debugPrintf("  attribute: SQL_ACCESS_MODE "
				"(unsupported but returning success)\n");
			// FIXME: implement...
			return SQL_SUCCESS;
		case SQL_LOGIN_TIMEOUT:
			debugPrintf("  attribute: SQL_LOGIN_TIMEOUT "
				"(unsupported but returning success)\n");
			// FIXME: implement...
			return SQL_SUCCESS;
		case SQL_OPT_TRACE:
			debugPrintf("  attribute: SQL_OPT_TRACE "
				"(unsupported but returning success)\n");
			// FIXME: implement...
			return SQL_SUCCESS;
		case SQL_OPT_TRACEFILE:
			debugPrintf("  attribute: SQL_OPT_TRACEFILE "
				"(unsupported but returning success)\n");
			// FIXME: implement...
			return SQL_SUCCESS;
		case SQL_TRANSLATE_DLL:
			debugPrintf("  attribute: SQL_TRANSLATE_DLL "
				"(unsupported but returning success)\n");
			// FIXME: implement...
			return SQL_SUCCESS;
		case SQL_TRANSLATE_OPTION:
			debugPrintf("  attribute: SQL_TRANSLATE_OPTION "
				"(unsupported but returning success)\n");
			// FIXME: implement...
			return SQL_SUCCESS;
		case SQL_TXN_ISOLATION:
			debugPrintf("  attribute: SQL_TXN_ISOLATION "
				"(unsupported but returning success)\n");
			// FIXME: implement...
			return SQL_SUCCESS;
		case SQL_ODBC_CURSORS:
			debugPrintf("  attribute: SQL_ODBC_CURSORS "
				"(unsupported but returning success)\n");
			// FIXME: implement...
			return SQL_SUCCESS;
		case SQL_QUIET_MODE:
			debugPrintf("  attribute: SQL_QUIET_MODE "
				"(unsupported but returning success)\n");
			// FIXME: implement...
			return SQL_SUCCESS;
		case SQL_PACKET_SIZE:
			debugPrintf("  attribute: SQL_PACKET_SIZE "
				"(unsupported but returning success)\n");
			// FIXME: implement...
			return SQL_SUCCESS;
	#if (ODBCVER >= 0x0300)
		case SQL_ATTR_CONNECTION_TIMEOUT:
			debugPrintf("  attribute: SQL_ATTR_CONNECTION_TIMEOUT "
				"(unsupported but returning success)\n");
			// FIXME: implement...
			return SQL_SUCCESS;
		case SQL_ATTR_DISCONNECT_BEHAVIOR:
			debugPrintf("  attribute: "
				"SQL_ATTR_DISCONNECT_BEHAVIOR "
				"(unsupported but returning success)\n");
			// FIXME: implement...
			return SQL_SUCCESS;
		case SQL_ATTR_ENLIST_IN_DTC:
			debugPrintf("  attribute: SQL_ATTR_ENLIST_IN_DTC "
				"(unsupported but returning success)\n");
			// FIXME: implement...
			return SQL_SUCCESS;
		case SQL_ATTR_ENLIST_IN_XA:
			debugPrintf("  attribute: SQL_ATTR_ENLIST_IN_XA "
				"(unsupported but returning success)\n");
			// FIXME: implement...
			return SQL_SUCCESS;
		case SQL_ATTR_AUTO_IPD:
			debugPrintf("  attribute: SQL_ATTR_AUTO_IPD "
				"(unsupported but returning success)\n");
			// FIXME: implement...
			return SQL_SUCCESS;
		case SQL_ATTR_METADATA_ID:
		{
			debugPrintf("  attribute: SQL_ATTR_METADATA_ID\n");
			debugPrintf("  val: %lld\n",(uint64_t)val);
			conn->attrmetadataid=(val==SQL_TRUE);
			return SQL_SUCCESS;
		}
	#endif
		// MS SQL Server-specific calls...
		case 1041:
			debugPrintf("  attribute: 1041 (license file)\n");
			// SQL Relay doesn't need to do anything with this
			return SQL_SUCCESS;
		case 1042:
			debugPrintf("  attribute: 1042 (password)\n");
			// SQL Relay doesn't need to do anything with this
			return SQL_SUCCESS;
		default:
			debugPrintf("  invalid attribute: %d\n",attribute);
			SQLR_CONNSetError(conn,
				"Optional field not implemented",0,"HYC00");
			return SQL_ERROR;
	}

	return SQL_ERROR;
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

	// FIXME:
	// In ODBC2, this method can set some statement-level attributes,
	// which become the defaults for future statements:
	// http://os2ports.os2site.com/pub/DB2/db2l0/db2l0225.htm#HDRWQ3521
	// https://docs.microsoft.com/en-us/sql/odbc/reference/appendixes/sqlsetconnectoption-mapping

	return SQLR_SQLSetConnectAttr(connectionhandle,option,
						(SQLPOINTER)value,0);
}

SQLRETURN SQL_API SQLSetCursorName(SQLHSTMT statementhandle,
					SQLCHAR *cursorname,
					SQLSMALLINT namelength) {
	debugFunction();

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("  NULL stmt handle\n");
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
		debugPrintf("  NULL env handle\n");
		return SQL_INVALID_HANDLE;
	}

	// use reinterpret_cast and assignment to smaller
	// sized value to avoid compiler warnings
	SQLUINTEGER	val=reinterpret_cast<uint64_t>(value);

	switch (attribute) {
		case SQL_ATTR_OUTPUT_NTS:
			debugPrintf("  attribute: "
				"SQL_ATTR_OUTPUT_NTS: %lld\n",
				(uint64_t)val);
			// this can't be set to false
			return (val==SQL_TRUE)?SQL_SUCCESS:SQL_ERROR;
		case SQL_ATTR_ODBC_VERSION:
			debugPrintf("  attribute: "
				"SQL_ATTR_ODBC_VERSION: %lld\n",
				(uint64_t)val);
			switch (val) {
				case SQL_OV_ODBC2:
					env->odbcversion=SQL_OV_ODBC2;
					break;
				#if (ODBCVER >= 0x0300)
				case SQL_OV_ODBC3:
					env->odbcversion=SQL_OV_ODBC3;
					break;
				#endif
			}
			debugPrintf("  odbcversion: %lld\n",
					(int64_t)env->odbcversion);
			return SQL_SUCCESS;
		case SQL_ATTR_CONNECTION_POOLING:
			debugPrintf("  attribute: "
				"SQL_ATTR_CONNECTION_POOLING: %lld\n",
				(uint64_t)val);
			// this can't be set on
			return (val==SQL_CP_OFF)?SQL_SUCCESS:SQL_ERROR;
		case SQL_ATTR_CP_MATCH:
			debugPrintf("  attribute: "
				"SQL_ATTR_CP_MATCH: %lld\n",
				(uint64_t)val);
			// this can't be set to anything but default
			return (val==SQL_CP_MATCH_DEFAULT)?
						SQL_SUCCESS:SQL_ERROR;
		default:
			debugPrintf("  unsupported attribute: %d\n",attribute);
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
		debugPrintf("  attribute: %d\n",attribute);
		debugPrintf("  NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	switch (attribute) {
		#if (ODBCVER >= 0x0300)
		case SQL_ATTR_APP_ROW_DESC:
			debugPrintf("  attribute: SQL_ATTR_APP_ROW_DESC\n");
			stmt->approwdesc=(rowdesc *)value;
			if (stmt->approwdesc==SQL_NULL_DESC) {
				stmt->approwdesc=stmt->improwdesc;
			}
			return SQL_SUCCESS;
		case SQL_ATTR_APP_PARAM_DESC:
			debugPrintf("  attribute: SQL_ATTR_APP_PARAM_DESC\n");
			stmt->appparamdesc=(paramdesc *)value;
			if (stmt->appparamdesc==SQL_NULL_DESC) {
				stmt->appparamdesc=stmt->impparamdesc;
			}
			return SQL_SUCCESS;
		case SQL_ATTR_IMP_ROW_DESC:
			debugPrintf("  attribute: SQL_ATTR_IMP_ROW_DESC\n");
			// read-only
			return SQL_ERROR;
		case SQL_ATTR_IMP_PARAM_DESC:
			debugPrintf("  attribute: SQL_ATTR_IMP_PARAM_DESC\n");
			// read-only
			return SQL_ERROR;
		case SQL_ATTR_CURSOR_SCROLLABLE:
			debugPrintf("  attribute: SQL_ATTR_CURSOR_SCROLLABLE "
				"(unsupported but returning success)\n");
			// FIXME: implement
			return SQL_SUCCESS;
		case SQL_ATTR_CURSOR_SENSITIVITY:
			debugPrintf("  attribute: SQL_ATTR_CURSOR_SENSITIVITY "
				"(unsupported but returning success)\n");
			// FIXME: implement
			return SQL_SUCCESS;
		#endif
		//case SQL_ATTR_QUERY_TIMEOUT:
		case SQL_QUERY_TIMEOUT:
			debugPrintf("  attribute: "
					"SQL_ATTR_QUERY_TIMEOUT/"
					"SQL_QUERY_TIMEOUT "
				"(unsupported but returning success)\n");
			// FIXME: implement
			return SQL_SUCCESS;
		//case SQL_ATTR_MAX_ROWS:
		case SQL_MAX_ROWS:
			debugPrintf("  attribute: "
					"SQL_ATTR_MAX_ROWS/"
					"SQL_MAX_ROWS "
				"(unsupported but returning success)\n");
			// FIXME: implement
			return SQL_SUCCESS;
		//case SQL_ATTR_NOSCAN:
		case SQL_NOSCAN:
			debugPrintf("  attribute: "
					"SQL_ATTR_NOSCAN/"
					"SQL_NOSCAN "
				"(unsupported but returning success)\n");
			// FIXME: implement
			return SQL_SUCCESS;
		//case SQL_ATTR_MAX_LENGTH:
		case SQL_MAX_LENGTH:
			debugPrintf("  attribute: "
					"SQL_ATTR_MAX_LENGTH/"
					"SQL_MAX_LENGTH "
				"(unsupported but returning success)\n");
			// FIXME: implement
			return SQL_SUCCESS;
		case SQL_ASYNC_ENABLE:
			debugPrintf("  attribute: SQL_ASYNC_ENABLE "
				"(unsupported but returning success)\n");
			// FIXME: implement
			return SQL_SUCCESS;
		//case SQL_ATTR_ROW_BIND_TYPE:
		case SQL_BIND_TYPE:
			debugPrintf("  attribute: "
					"SQL_ATTR_ROW_BIND_TYPE/"
					"SQL_BIND_TYPE: "
					"%lld\n",(uint64_t)value);
			stmt->rowbindtype=(SQLULEN)value;
			return SQL_SUCCESS;
		//case SQL_ATTR_CONCURRENCY:
		//case SQL_ATTR_CURSOR_TYPE:
		case SQL_CURSOR_TYPE:
			debugPrintf("  attribute: "
					"SQL_ATTR_CONCURRENCY/"
					"SQL_ATTR_CURSOR_TYPE/"
					"SQL_CURSOR_TYPE "
				"(unsupported but returning success)\n");
			// FIXME: implement
			return SQL_SUCCESS;
		case SQL_CONCURRENCY:
			debugPrintf("  attribute: SQL_CONCURRENCY "
				"(unsupported but returning success)\n");
			// FIXME: implement
			return SQL_SUCCESS;
		//case SQL_ATTR_KEYSET_SIZE:
		case SQL_KEYSET_SIZE:
			debugPrintf("  attribute: "
					"SQL_ATTR_KEYSET_SIZE/"
					"SQL_KEYSET_SIZE "
				"(unsupported but returning success)\n");
			// FIXME: implement
			return SQL_SUCCESS;
		case SQL_ROWSET_SIZE:
			{
			SQLULEN	val=(SQLULEN)value;
			debugPrintf("  attribute: SQL_ROWSET_SIZE: "
						"%lld\n",(uint64_t)val);
			// don't allow this to be set to "fetch all rows"
			if (!val) {
				val=1;
			}
			stmt->rowsetsize=val;
			return SQL_SUCCESS;
			}
		//case SQL_ATTR_SIMULATE_CURSOR:
		case SQL_SIMULATE_CURSOR:
			debugPrintf("  attribute: "
					"SQL_ATTR_SIMULATE_CURSOR/"
					"SQL_SIMULATE_CURSOR "
				"(unsupported but returning success)\n");
			// FIXME: implement
			return SQL_SUCCESS;
		//case SQL_ATTR_RETRIEVE_DATA:
		case SQL_RETRIEVE_DATA:
			{
			SQLULEN	val=(SQLULEN)value;
			debugPrintf("  attribute: "
					"SQL_ATTR_RETRIEVE_DATA/"
					"SQL_RETRIEVE_DATA: %lld\n",
					(uint64_t)val);
			if (val==SQL_RD_ON) {
				return SQL_SUCCESS;
			} else {
				SQLR_STMTSetError(stmt,
					"Invalid attribute value",0,"HY024");
				return SQL_ERROR;
			}
			}
		//case SQL_ATTR_USE_BOOKMARKS:
		case SQL_USE_BOOKMARKS:
			debugPrintf("  attribute: "
					"SQL_ATTR_USE_BOOKMARKS/"
					"SQL_USE_BOOKMARKS "
				"(unsupported but returning success)\n");
			// FIXME: implement
			return SQL_SUCCESS;
		case SQL_GET_BOOKMARK:
			debugPrintf("  attribute: SQL_GET_BOOKMARK "
				"(unsupported but returning success)\n");
			// FIXME: implement
			return SQL_SUCCESS;
		//case SQL_ATTR_ROW_NUMBER:
		case SQL_ROW_NUMBER:
			debugPrintf("  attribute: "
					"SQL_ATTR_ROW_NUMBER/"
					"SQL_ROW_NUMBER\n");
			// read-only
			return SQL_ERROR;
		#if (ODBCVER >= 0x0300)
		case SQL_ATTR_ENABLE_AUTO_IPD:
			debugPrintf("  attribute: SQL_ATTR_ENABLE_AUTO_IPD "
				"(unsupported but returning success)\n");
			// FIXME: implement
			return SQL_SUCCESS;
		case SQL_ATTR_FETCH_BOOKMARK_PTR:
			debugPrintf("  attribute: SQL_ATTR_FETCH_BOOKMARK_PTR "
				"(unsupported but returning success)\n");
			// FIXME: implement
			return SQL_SUCCESS;
		case SQL_ATTR_PARAM_BIND_OFFSET_PTR:
			{
			stmt->parambindoffsetptr=(SQLULEN *)value;
			debugPrintf("  attribute: "
					"SQL_ATTR_PARAM_BIND_OFFSET_PTR: "
					"0x%08x\n",
					stmt->parambindoffsetptr);
			return SQL_SUCCESS;
			}
		case SQL_ATTR_PARAM_BIND_TYPE:
			{
			SQLULEN	val=(SQLULEN)value;
			debugPrintf("  attribute: SQL_ATTR_PARAM_BIND_TYPE: "
							"%lld\n",(uint64_t)val);
			if (val==SQL_PARAM_BIND_BY_COLUMN) {
				return SQL_SUCCESS;
			} else {
				SQLR_STMTSetError(stmt,
					"Invalid attribute value",0,"HY024");
				return SQL_ERROR;
			}
			}
		case SQL_ATTR_PARAM_OPERATION_PTR:
			debugPrintf("  attribute: SQL_ATTR_PARAM_OPERATION_PTR "
				"(unsupported but returning success)\n");
			// FIXME: implement
			return SQL_SUCCESS;
		case SQL_ATTR_PARAM_STATUS_PTR:
			debugPrintf("  attribute: SQL_ATTR_PARAM_STATUS_PTR "
				"(unsupported but returning success)\n");
			// FIXME: implement
			return SQL_SUCCESS;
		case SQL_ATTR_PARAMS_PROCESSED_PTR:
			{
			stmt->paramsprocessed=(SQLULEN *)value;
			debugPrintf("  attribute: "
					"SQL_ATTR_PARAMS_PROCESSED_PTR: "
					"0x%08x\n",stmt->paramsprocessed);
			return SQL_SUCCESS;
			}
		case SQL_ATTR_PARAMSET_SIZE:
			{
			SQLULEN	val=(SQLULEN)value;
			debugPrintf("  attribute: SQL_ATTR_PARAMSET_SIZE: "
					"%lld\n",(uint64_t)val);
			if (val!=1) {
				SQLR_STMTSetError(stmt,
					"Invalid attribute value",0,"HY024");
				return SQL_ERROR;
			}
			return SQL_SUCCESS;
			}
		case SQL_ATTR_ROW_BIND_OFFSET_PTR:
			debugPrintf("  attribute: "	
					"SQL_ATTR_ROW_BIND_OFFSET_PTR "
				"(unsupported but returning success)\n");
			// FIXME: implement
			return SQL_SUCCESS;
		case SQL_ATTR_ROW_OPERATION_PTR:
			debugPrintf("  attribute: SQL_ATTR_ROW_OPERATION_PTR "
				"(unsupported but returning success)\n");
			// FIXME: implement
			return SQL_SUCCESS;
		case SQL_ATTR_ROW_STATUS_PTR:
			debugPrintf("  attribute: SQL_ATTR_ROW_STATUS_PTR\n");
			stmt->rowstatusptr=(SQLUSMALLINT *)value;
			return SQL_SUCCESS;
		case SQL_ATTR_ROWS_FETCHED_PTR:
			debugPrintf("  attribute: SQL_ATTR_ROWS_FETCHED_PTR\n");
			stmt->rowsfetchedptr=(SQLROWSETSIZE *)value;
			return SQL_SUCCESS;
		case SQL_ATTR_ROW_ARRAY_SIZE:
			{
			SQLULEN val=(SQLULEN)value;
			debugPrintf("  attribute: SQL_ATTR_ROW_ARRAY_SIZE: "
						"%lld\n",(uint64_t)val);
			// don't allow this to be set to "fetch all rows"
			if (!val) {
				val=1;
			}
			stmt->rowarraysize=val;
			return SQL_SUCCESS;
			}
		#endif
		#if (ODBCVER < 0x0300)
		case SQL_STMT_OPT_MAX:
			debugPrintf("  attribute: SQL_STMT_OPT_MAX "
				"(unsupported but returning success)\n");
			// FIXME: implement
			return SQL_SUCCESS;
		case SQL_STMT_OPT_MIN:
			debugPrintf("  attribute: SQL_STMT_OPT_MIN "
				"(unsupported but returning success)\n");
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
	return SQLR_SQLSetStmtAttr(statementhandle,option,
						(SQLPOINTER)value,0);
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
		debugPrintf("  NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	// not supported
	SQLR_STMTSetError(stmt,
			"Driver does not support this function",0,"IM001");

	return SQL_ERROR;
}

SQLRETURN SQL_API SQLStatistics(SQLHSTMT statementhandle,
					SQLCHAR *catalogname,
					SQLSMALLINT namelength1,
					SQLCHAR *schemaname,
					SQLSMALLINT namelength2,
					SQLCHAR *tablename,
					SQLSMALLINT namelength3,
					SQLUSMALLINT unique,
					SQLUSMALLINT reserved) {
	debugFunction();

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("  NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	// FIXME: this code treats xxxname as a search pattern in all cases
	// xxxname is a case-insensitive search pattern if:
	// * SQL_ODBC_VERSION is SQL_OV_ODBC3
	// * SQL_ATTR_METADATA_ID is SQL_FALSE
	// otherwise it's a case-insensitive literal

	stringbuffer	table;
	SQLR_BuildObjectName(&table,catalogname,namelength1,
					schemaname,namelength2,
					tablename,namelength3);

	const char	*uniqueness=NULL;
	switch (unique) {
		case SQL_INDEX_UNIQUE:
			uniqueness="unique";
			break;
		case SQL_INDEX_ALL:
			uniqueness="all";
			break;
		default:
			SQLR_STMTSetError(stmt,
			"Uniqueness option type out of range",0,"HY100");
	}

	const char	*accuracy=NULL;
	switch (reserved) {
		case SQL_ENSURE:
			accuracy="ensure";
			break;
		case SQL_QUICK:
			accuracy="quick";
			break;
		default:
			SQLR_STMTSetError(stmt,
			"Accuracy option type out of range",0,"HY101");
	}

	char	*wild;
	charstring::printf(&wild,"%s:%s",uniqueness,accuracy);

	debugPrintf("  table: %s\n",table.getString());
	debugPrintf("  wild: %s\n",(wild)?wild:"");

	// reinit row indices
	stmt->currentfetchrow=0;
	stmt->currentstartrow=0;
	stmt->currentgetdatarow=0;

	// clear the error
	SQLR_STMTClearError(stmt);

	SQLRETURN	retval=
		(stmt->cur->getKeyAndIndexList(table.getString(),wild,
						SQLRCLIENTLISTFORMAT_ODBC))?
							SQL_SUCCESS:SQL_ERROR;
	delete[] wild;

	// the statement has been executed
	stmt->executed=true;
	stmt->nodata=false;

	debugPrintf("  %s\n",(retval==SQL_SUCCESS)?"success":"error");

	// handle errors
	if (retval!=SQL_SUCCESS) {
		SQLR_STMTSetError(stmt,stmt->cur->errorMessage(),
					stmt->cur->errorNumber(),NULL);
	}
	return retval;
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
		debugPrintf("  NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	// reinit row indices
	stmt->currentfetchrow=0;
	stmt->currentstartrow=0;
	stmt->currentgetdatarow=0;

	// clear the error
	SQLR_STMTClearError(stmt);

	// normalize the names
	if (namelength1==SQL_NTS) {
		namelength1=charstring::length(catalogname);
	}
	if (namelength2==SQL_NTS) {
		namelength2=charstring::length(schemaname);
	}
	if (namelength3==SQL_NTS) {
		namelength3=charstring::length(tablename);
	}
	if (namelength4==SQL_NTS) {
		namelength4=charstring::length(tabletype);
	}
	char	*catname=charstring::duplicate((char *)catalogname,namelength1);
	char	*schname=charstring::duplicate((char *)schemaname,namelength2);
	char	*tblname=charstring::duplicate((char *)tablename,namelength3);
	char	*tbltype=charstring::duplicate((char *)tabletype,namelength4);

	debugPrintf("  for catalog=%s schema=%s table=%s tabletype=%s\n",
					catname,schname,tblname,tbltype);


	// FIXME: this code treats xxxname as a search pattern in all cases
	// xxxname should be a case-insensitive search pattern if:
	// * SQL_ODBC_VERSION is SQL_OV_ODBC3
	// * SQL_ATTR_METADATA_ID is SQL_FALSE
	// otherwise it should be a case-insensitive literal

	SQLRETURN	retval=SQL_ERROR;
	if (!charstring::compare(catname,SQL_ALL_CATALOGS) &&
				charstring::isNullOrEmpty(schname) &&
				charstring::isNullOrEmpty(tblname) &&
				charstring::isNullOrEmpty(tbltype)) {

		debugPrintf("  getting database list...\n");

		retval=
		(stmt->cur->getDatabaseList(NULL,SQLRCLIENTLISTFORMAT_ODBC))?
							SQL_SUCCESS:SQL_ERROR;

	} else if (!charstring::compare(schname,SQL_ALL_SCHEMAS) &&
				charstring::isNullOrEmpty(catname) &&
				charstring::isNullOrEmpty(tblname) &&
				charstring::isNullOrEmpty(tbltype)) {

		debugPrintf("  getting schema list...\n");

		retval=
		(stmt->cur->getSchemaList(NULL,SQLRCLIENTLISTFORMAT_ODBC))?
							SQL_SUCCESS:SQL_ERROR;

	} else if (!charstring::compare(tbltype,SQL_ALL_TABLE_TYPES) &&
				charstring::isNullOrEmpty(catname) &&
				charstring::isNullOrEmpty(schname) &&
				charstring::isNullOrEmpty(tblname)) {

		debugPrintf("  getting table type list...\n");

		retval=
		(stmt->cur->getTableTypeList(NULL,SQLRCLIENTLISTFORMAT_ODBC))?
							SQL_SUCCESS:SQL_ERROR;

	} else {

		const char	*wild=NULL;

		// If tblname was empty or %, then leave "wild" NULL.
		// Otherwise concatenate catalog/schema's until it's in one
		// of the following formats:
		// * table
		// * schema.table
		// * catalog.schema.table
		// If tblname already contains a . then just use it as-is.
		if (!charstring::contains(tblname,'.')) {

			stringbuffer	wildstr;
			if (!charstring::isNullOrEmpty(catname)) {
				wildstr.append(catname)->append('.');
			}
			if (!charstring::isNullOrEmpty(schname)) {
				wildstr.append(schname)->append('.');
			} else if (wildstr.getStringLength()) {
				wildstr.append("%.");
			}
			if (!charstring::isNullOrEmpty(schname)) {
				wildstr.append(tblname);
			} else {
				wildstr.append('%');
			}
			delete[] tblname;
			tblname=wildstr.detachString();
		}
		wild=tblname;

		debugPrintf("  getting table list...\n");
		debugPrintf("  wild: %s\n",(wild)?wild:"");

		uint16_t	objecttypes=0;
		if (charstring::contains(tbltype,"TABLE")) {
			objecttypes|=DB_OBJECT_TABLE;
		}
		if (charstring::contains(tbltype,"VIEW")) {
			objecttypes|=DB_OBJECT_VIEW;
		}
		if (charstring::contains(tbltype,"ALIAS")) {
			objecttypes|=DB_OBJECT_ALIAS;
		}
		if (charstring::contains(tbltype,"SYNONYM")) {
			objecttypes|=DB_OBJECT_SYNONYM;
		}

		// get the table list
		// FIXME: this list should also be restricted to the
		// specified table type
		retval=
		(stmt->cur->getTableList(
				wild,SQLRCLIENTLISTFORMAT_ODBC,objecttypes))?
							SQL_SUCCESS:SQL_ERROR;
	}

	delete[] catname;
	delete[] schname;
	delete[] tblname;
	delete[] tbltype;

	// the statement has been executed
	stmt->executed=true;
	stmt->nodata=false;

	debugPrintf("  %s\n",(retval==SQL_SUCCESS)?"success":"error");

	// handle errors
	if (retval!=SQL_SUCCESS) {
		SQLR_STMTSetError(stmt,stmt->cur->errorMessage(),
					stmt->cur->errorNumber(),NULL);
	}
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
		debugPrintf("  no valid handle\n");
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
		debugPrintf("  NULL conn handle\n");
		return SQL_INVALID_HANDLE;
	}

	// the connect string may not be null terminated, so make a copy that is
	if (cbconnstrin==SQL_NTS) {
		cbconnstrin=charstring::length(szconnstrin);
	}
	char	*nulltermconnstr=charstring::duplicate(
					(const char *)szconnstrin,
					cbconnstrin);
	debugPrintf("  connectstring: %s\n",nulltermconnstr);

	// parse out DSN, UID/User and PWD/Password from the connect string
	parameterstring	pstr;
	pstr.parse(nulltermconnstr);
	const char	*dsn=pstr.getValue("DSN");
	if (charstring::isNullOrEmpty(dsn)) {
		dsn=pstr.getValue("dsn");
	}
	const char	*uid=pstr.getValue("UID");
	if (charstring::isNullOrEmpty(uid)) {
		uid=pstr.getValue("uid");
	}
	if (charstring::isNullOrEmpty(uid)) {
		uid=pstr.getValue("User");
	}
	const char	*pwd=pstr.getValue("PWD");
	if (charstring::isNullOrEmpty(pwd)) {
		pwd=pstr.getValue("pwd");
	}
	if (charstring::isNullOrEmpty(pwd)) {
		pwd=pstr.getValue("Password");
	}

	debugPrintf("  dsn: %s\n",dsn);
	debugPrintf("  uid: %s\n",uid);
	debugPrintf("  pwd: %s\n",pwd);

	// for now, don't do any prompting...
	switch (fdrivercompletion) {
		case SQL_DRIVER_PROMPT:
			debugPrintf("  fbdrivercompletion: "
					"SQL_DRIVER_PROMPT\n");
			break;
		case SQL_DRIVER_COMPLETE:
			debugPrintf("  fbdrivercompletion: "
					"SQL_DRIVER_COMPLETE\n");
			break;
		case SQL_DRIVER_COMPLETE_REQUIRED:
			debugPrintf("  fbdrivercompletion: "
					"SQL_DRIVER_COMPLETE_REQUIRED\n");
			break;
		case SQL_DRIVER_NOPROMPT:
			debugPrintf("  fbdrivercompletion: "
					"SQL_DRIVER_NOPROMPT\n");
			break;
	}

	// output the updated connect string
	if (pcbconnstrout) {
		if (cbconnstrin==SQL_NTS) {
			*pcbconnstrout=charstring::length(szconnstrin);
		} else {
			*pcbconnstrout=cbconnstrin;
		}
		if (*pcbconnstrout>cbconnstroutmax) {
			*pcbconnstrout=cbconnstroutmax;
		}
		charstring::safeCopy((char *)szconnstrout,
					*pcbconnstrout,nulltermconnstr);
	}

	// clean up
	delete[] nulltermconnstr;

	// build the DSN if it wasn't explicitly provided
	stringbuffer	dsnstr;
	if (charstring::isNullOrEmpty(dsn)) {
		
		// exclude User/Password, we ought to have gotten them earlier
		const char	*names[]={
			"Server",
			"Port",
			"Socket",
			"Retry Time",
			"Tries",
			"Enable Kerberos",
			"Kerberos Service",
			"Kerberos Mech",
			"Kerberos Flags",
			"Enable TLS",
			"TLS Version",
			"TLS Certificate",
			"TLS Certificate Password",
			"TLS Ciphers",
			"TLS Validation",
			"TLS Certificate Authority",
			"TLS Depth",
			"Database",
			"Debug",
			"Column Name Case",
			"Result Set Buffer Size",
			"Don't Get Column Info",
			"Nulls As Nulls",
			"Lazy Connect",
			"Clear Binds During Prepare",
			"Bind Variable Delimiters",
			NULL
		};

		for (const char **name=names; *name; name++) {
			const char	*val=pstr.getValue(*name);
			if (!charstring::isNullOrEmpty(val)) {
				dsnstr.append(*name);
				dsnstr.append('=');
				dsnstr.append(val);
				dsnstr.append(';');
			}
		}
		dsn=dsnstr.getString();
	}

	// the connect string must include a valid dsn or server parameter
	if (charstring::isNullOrEmpty(dsn)) {
		if (charstring::isNullOrEmpty(pstr.getValue("Server"))) {
			return SQL_ERROR;
		} else {
			dsn="SQLRELAY_DEFAULT";
		}
	}

	// connect
	return SQLR_SQLConnect(hdbc,
				&pstr,
				(SQLCHAR *)dsn,
				charstring::length(dsn),
				(SQLCHAR *)uid,
				charstring::length(uid),
				(SQLCHAR *)pwd,
				charstring::length(pwd));
}

SQLRETURN SQL_API SQLBulkOperations(SQLHSTMT statementhandle,
					SQLSMALLINT Operation) {
	debugFunction();

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("  NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	// not supported
	SQLR_STMTSetError(stmt,
			"Driver does not support this function",0,"IM001");

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
		debugPrintf("  NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	// not supported
	SQLR_STMTSetError(stmt,
			"Driver does not support this function",0,"IM001");

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
		debugPrintf("  NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	// not supported
	SQLR_STMTSetError(stmt,
			"Driver does not support this function",0,"IM001");

	return SQL_ERROR;
}

#ifdef HAVE_SQLEXTENDEDFETCH_LEN
SQLRETURN SQL_API SQLExtendedFetch(SQLHSTMT statementhandle,
					SQLUSMALLINT fetchorientation,
					SQLLEN fetchoffset,
					SQLULEN *pcrow,
					SQLUSMALLINT *rgfrowstatus) {
	debugFunction();

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("  NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	return SQLR_Fetch(statementhandle,
				pcrow,
				rgfrowstatus,
				stmt->rowsetsize);
}
#else
SQLRETURN SQL_API SQLExtendedFetch(SQLHSTMT statementhandle,
					SQLUSMALLINT fetchorientation,
					SQLROWOFFSET fetchoffset,
					SQLROWSETSIZE *pcrow,
					SQLUSMALLINT *rgfrowstatus) {
	debugFunction();

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("  NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	SQLULEN	localpcrow=0;
	SQLRETURN	retval=SQLR_Fetch(statementhandle,
						&localpcrow,
						rgfrowstatus,
						stmt->rowsetsize);
	if (pcrow) {
		*pcrow=localpcrow;
	}
	return retval;
}
#endif

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
		debugPrintf("  NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	// not supported
	SQLR_STMTSetError(stmt,
			"Driver does not support this function",0,"IM001");

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
		debugPrintf("  NULL stmt handle\n");
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
					SQLCHAR *catalogname,
					SQLSMALLINT namelength1,
					SQLCHAR *schemaname,
					SQLSMALLINT namelength2,
					SQLCHAR *tablename,
					SQLSMALLINT namelength3) {
	debugFunction();

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("  NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	// FIXME: this code treats xxxname as a search pattern in all cases
	// xxxname is a case-insensitive search pattern if:
	// * SQL_ODBC_VERSION is SQL_OV_ODBC3
	// * SQL_ATTR_METADATA_ID is SQL_FALSE
	// otherwise it's a case-insensitive literal

	stringbuffer	table;
	SQLR_BuildObjectName(&table,catalogname,namelength1,
					schemaname,namelength2,
					tablename,namelength3);

	debugPrintf("  table: %s\n",table.getString());

	// reinit row indices
	stmt->currentfetchrow=0;
	stmt->currentstartrow=0;
	stmt->currentgetdatarow=0;

	// clear the error
	SQLR_STMTClearError(stmt);

	SQLRETURN	retval=
		(stmt->cur->getPrimaryKeysList(table.getString(),NULL,
						SQLRCLIENTLISTFORMAT_ODBC))?
							SQL_SUCCESS:SQL_ERROR;

	// the statement has been executed
	stmt->executed=true;
	stmt->nodata=false;

	debugPrintf("  %s\n",(retval==SQL_SUCCESS)?"success":"error");

	// handle errors
	if (retval!=SQL_SUCCESS) {
		SQLR_STMTSetError(stmt,stmt->cur->errorMessage(),
					stmt->cur->errorNumber(),NULL);
	}
	return retval;
}

SQLRETURN SQL_API SQLProcedureColumns(SQLHSTMT statementhandle,
					SQLCHAR *catalogname,
					SQLSMALLINT namelength1,
					SQLCHAR *schemaname,
					SQLSMALLINT namelength2,
					SQLCHAR *procedurename,
					SQLSMALLINT namelength3,
					SQLCHAR *columnname,
					SQLSMALLINT namelength4) {
	debugFunction();

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("  NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	// FIXME: this code treats xxxname as a search pattern in all cases
	// xxxname is a case-insensitive search pattern if:
	// * SQL_ODBC_VERSION is SQL_OV_ODBC3
	// * SQL_ATTR_METADATA_ID is SQL_FALSE
	// otherwise it's a case-insensitive literal

	stringbuffer	procedure;
	SQLR_BuildObjectName(&procedure,catalogname,namelength1,
					schemaname,namelength2,
					procedurename,namelength3);

	if (namelength4==SQL_NTS) {
		namelength4=charstring::length(columnname);
	}
	char	*wild=charstring::duplicate(
				(const char *)columnname,namelength4);
	if (!charstring::compare(wild,"%")) {
		delete[] wild;
		wild=NULL;
	}

	debugPrintf("  procedure: %s\n",procedure.getString());
	debugPrintf("  wild: %s\n",(wild)?wild:"");

	// reinit row indices
	stmt->currentfetchrow=0;
	stmt->currentstartrow=0;
	stmt->currentgetdatarow=0;

	// clear the error
	SQLR_STMTClearError(stmt);

	SQLRETURN	retval=
		(stmt->cur->getProcedureBindAndColumnList(
						procedure.getString(),wild,
						SQLRCLIENTLISTFORMAT_ODBC))?
							SQL_SUCCESS:SQL_ERROR;
	delete[] wild;

	// the statement has been executed
	stmt->executed=true;
	stmt->nodata=false;

	debugPrintf("  %s\n",(retval==SQL_SUCCESS)?"success":"error");

	// handle errors
	if (retval!=SQL_SUCCESS) {
		SQLR_STMTSetError(stmt,stmt->cur->errorMessage(),
					stmt->cur->errorNumber(),NULL);
	}
	return retval;
}

SQLRETURN SQL_API SQLProcedures(SQLHSTMT statementhandle,
					SQLCHAR *catalogname,
					SQLSMALLINT namelength1,
					SQLCHAR *schemaname,
					SQLSMALLINT namelength2,
					SQLCHAR *procname,
					SQLSMALLINT namelength3) {
	debugFunction();

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt) {
		debugPrintf("  NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	// normalize the names
	if (namelength1==SQL_NTS) {
		namelength1=charstring::length(catalogname);
	}
	if (namelength2==SQL_NTS) {
		namelength2=charstring::length(schemaname);
	}
	if (namelength3==SQL_NTS) {
		namelength3=charstring::length(procname);
	}
	char	*catname=charstring::duplicate((char *)catalogname,namelength1);
	char	*schname=charstring::duplicate((char *)schemaname,namelength2);
	char	*prcname=charstring::duplicate((char *)procname,namelength3);

	debugPrintf("  for catalog=%s schema=%s procedure=%s\n",
						catname,schname,prcname);


	// FIXME: this code treats xxxname as a search pattern in all cases
	// xxxname should be a case-insensitive search pattern if:
	// * SQL_ODBC_VERSION is SQL_OV_ODBC3
	// * SQL_ATTR_METADATA_ID is SQL_FALSE
	// otherwise it should be a case-insensitive literal

	SQLRETURN	retval=SQL_ERROR;

	const char	*wild=prcname;
	if (!charstring::compare(wild,"%")) {
		wild=NULL;
	}

	debugPrintf("  getting procedure list...\n");
	debugPrintf("  wild: %s\n",(wild)?wild:"");

	// FIXME: this list should also be restricted to the
	// specified catalog, schema, and procedure type

	// reinit row indices
	stmt->currentfetchrow=0;
	stmt->currentstartrow=0;
	stmt->currentgetdatarow=0;

	// clear the error
	SQLR_STMTClearError(stmt);

	retval=(stmt->cur->getProcedureList(wild,SQLRCLIENTLISTFORMAT_ODBC))?
							SQL_SUCCESS:SQL_ERROR;

	delete[] catname;
	delete[] schname;
	delete[] prcname;

	// the statement has been executed
	stmt->executed=true;
	stmt->nodata=false;

	debugPrintf("  %s\n",(retval==SQL_SUCCESS)?"success":"error");

	// handle errors
	if (retval!=SQL_SUCCESS) {
		SQLR_STMTSetError(stmt,stmt->cur->errorMessage(),
					stmt->cur->errorNumber(),NULL);
	}
	return retval;
}

static SQLRETURN SQLR_SQLSetPos(SQLHSTMT statementhandle,
					SQLSETPOSIROW irow,
					SQLUSMALLINT foption,
					SQLUSMALLINT flock) {
	debugFunction();

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("  NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	if (foption==SQL_POSITION) {
		if (!irow) {
			irow=1;
		}
		stmt->currentgetdatarow=stmt->currentstartrow+irow-1;
		debugPrintf("  currentgetdatarow=%lld\n",
				stmt->currentgetdatarow);
		return SQL_SUCCESS;
	}

	SQLR_STMTSetError(stmt,
			"Driver does not support this function",0,"IM001");
	return SQL_ERROR;
}

SQLRETURN SQL_API SQLSetPos(SQLHSTMT statementhandle,
					SQLSETPOSIROW irow,
					SQLUSMALLINT foption,
					SQLUSMALLINT flock) {
	return SQLR_SQLSetPos(statementhandle,irow,foption,flock);
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
		debugPrintf("  NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	// not supported
	SQLR_STMTSetError(stmt,
			"Driver does not support this function",0,"IM001");

	return SQL_ERROR;
}

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
	if (stmt->inputbindstrings.getValue(parameternumber,&data)) {
		stmt->inputbindstrings.remove(parameternumber);
		delete[] data;
	}
	stmt->inputbindstrings.setValue(parameternumber,string);

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
	if (stmt->inputbindstrings.getValue(parameternumber,&data)) {
		stmt->inputbindstrings.remove(parameternumber);
		delete[] data;
	}
	stmt->inputbindstrings.setValue(parameternumber,string);

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
	for (uint16_t index=20; index<36; index=index+2) {
		unsigned char	data=guid->Data4[byte];
		string[index+1]=SQLR_HexToChar(data%16);
		data=data/16;
		string[index]=SQLR_HexToChar(data%16);
		byte++;
	}

	// hang on to that string
	char	*data=NULL;
	if (stmt->inputbindstrings.getValue(parameternumber,&data)) {
		stmt->inputbindstrings.remove(parameternumber);
		delete[] data;
	}
	stmt->inputbindstrings.setValue(parameternumber,string);

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
		debugPrintf("  NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	SQLRETURN	retval=SQL_SUCCESS;

	// convert parameternumber to a string
	char	*parametername=charstring::parseNumber(parameternumber);
	debugPrintf("  parametername: %s\n",parametername);
	debugPrintf("  lengthprecision: %lld\n",(uint64_t)lengthprecision);
	debugPrintf("  parameterscale: %lld\n",(uint64_t)parameterscale);
	if (stmt->parambindoffsetptr && *(stmt->parambindoffsetptr)) {
		debugPrintf("  WARNING: stmt->parambindoffsetptr=%lld "
				"(but is unused)\n",
				(uint64_t)*(stmt->parambindoffsetptr));
	}

	bool	dataatexec=false;
	if (strlen_or_ind) {

		debugPrintf("  strlen_or_ind: %lld\n",(int64_t)*strlen_or_ind);

		// handle NULLs by binding a NULL string
		if (*strlen_or_ind==SQL_NULL_DATA) {
			valuetype=SQL_C_CHAR;
			parametervalue=NULL;
		} else

		// catch data-at-exec
		// We're actually faking SQLBindParameter data-at-exec below.
		// See SQLParamData for more details.
		if (*strlen_or_ind==SQL_DATA_AT_EXEC ||
				*strlen_or_ind<=SQL_LEN_DATA_AT_EXEC_OFFSET) {
			dataatexec=true;
		}

	} else {
		debugPrintf("  strlen_or_ind is NULL\n");
	}

	switch (valuetype) {
		case SQL_C_CHAR:
			debugPrintf("  valuetype: SQL_C_CHAR\n");
			// FIXME: support data-at-exec with other types
			if (dataatexec) {
				debugPrintf("  data at exec\n");
				stmt->dataatexec=true;
				stmt->dataatexecdict.setValue(parameternumber,
								parametervalue);
			} else {
				debugPrintf("  value: \"%s\"\n",
							parametervalue);
				stmt->cur->inputBind(parametername,
					(const char *)parametervalue);
			}
			break;
		case SQL_C_LONG:
			debugPrintf("  valuetype: SQL_C_LONG\n");
			debugPrintf("  value: \"%lld\"\n",
				(int64_t)(*((int32_t *)parametervalue)));
			stmt->cur->inputBind(parametername,
				(int64_t)(*((int32_t *)parametervalue)));
			break;
		case SQL_C_SHORT:
			debugPrintf("  valuetype: SQL_C_SHORT\n");
			debugPrintf("  value: \"%lld\"\n",
				(int64_t)(*((int16_t *)parametervalue)));
			stmt->cur->inputBind(parametername,
				(int64_t)(*((int16_t *)parametervalue)));
			break;
		case SQL_C_FLOAT:
			debugPrintf("  valuetype: SQL_C_FLOAT\n");
			debugPrintf("  value: \"%f\"\n",
				(float)(*((double *)parametervalue)));
			stmt->cur->inputBind(parametername,
				(float)(*((double *)parametervalue)),
				lengthprecision,
				parameterscale);
			break;
		case SQL_C_DOUBLE:
			debugPrintf("  valuetype: SQL_C_DOUBLE\n");
			debugPrintf("  value: \"%f\"\n",
				*((double *)parametervalue));
			stmt->cur->inputBind(parametername,
				*((double *)parametervalue),
				lengthprecision,
				parameterscale);
			break;
		case SQL_C_NUMERIC:
			debugPrintf("  valuetype: SQL_C_NUMERIC\n");
			debugPrintf("  value: \"%s\"\n",
				SQLR_BuildNumeric(stmt,parameternumber,
					(SQL_NUMERIC_STRUCT *)parametervalue));
			stmt->cur->inputBind(parametername,
				SQLR_BuildNumeric(stmt,parameternumber,
					(SQL_NUMERIC_STRUCT *)parametervalue));
			break;
		case SQL_C_DATE:
		case SQL_C_TYPE_DATE:
			{
			debugPrintf("  valuetype: SQL_C_DATE/SQL_C_TYPE_DATE\n");
			DATE_STRUCT	*ds=(DATE_STRUCT *)parametervalue;
			debugPrintf("  value: \"%d-%d-%d\"\n",
						ds->year,ds->month,ds->day);
			stmt->cur->inputBind(parametername,
						ds->year,ds->month,ds->day,
						0,0,0,0,NULL,false);
			}
			break;
		case SQL_C_TIME:
		case SQL_C_TYPE_TIME:
			{
			debugPrintf("  valuetype: SQL_C_TIME/SQL_C_TYPE_TIME\n");
			TIME_STRUCT	*ts=(TIME_STRUCT *)parametervalue;
			debugPrintf("  value: \"%d:%d:%d\"\n",
						ts->hour,ts->minute,ts->second);
			stmt->cur->inputBind(parametername,
						0,0,0,
						ts->hour,ts->minute,ts->second,
						0,NULL,false);
			break;
			}
		case SQL_C_TIMESTAMP:
		case SQL_C_TYPE_TIMESTAMP:
			{
			debugPrintf("  valuetype: "
				"SQL_C_TIMESTAMP/SQL_C_TYPE_TIMESTAMP\n");
			TIMESTAMP_STRUCT	*tss=
					(TIMESTAMP_STRUCT *)parametervalue;
			debugPrintf("  value: \"%d-%d-%d %d:%d:%d:%d\"\n",
					tss->year,tss->month,tss->day,
					tss->hour,tss->minute,tss->second,
					tss->fraction/1000);
			stmt->cur->inputBind(parametername,
					tss->year,tss->month,tss->day,
					tss->hour,tss->minute,tss->second,
					tss->fraction/1000,NULL,false);
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
			debugPrintf("  valuetype: SQL_C_INTERVAL_XXX\n");
			stmt->cur->inputBind(parametername,
				SQLR_BuildInterval(stmt,parameternumber,
					(SQL_INTERVAL_STRUCT *)parametervalue));
			break;
		//case SQL_C_VARBOOKMARK:
		//	(dup of SQL_C_BINARY)
		case SQL_C_BINARY:
			debugPrintf("  valuetype: "
				"SQL_C_BINARY/SQL_C_VARBOOKMARK\n");
			stmt->cur->inputBindBlob(parametername,
					(const char *)parametervalue,
					(strlen_or_ind)?*strlen_or_ind:0);
			break;
		case SQL_C_BIT:
			debugPrintf("  valuetype: SQL_C_BIT\n");
			debugPrintf("  value: \"%s\"\n",parametervalue);
			stmt->cur->inputBind(parametername,
				(charstring::contains("YyTt",
					(const char *)parametervalue) ||
				charstring::toInteger(
					(const char *)parametervalue))?"1":"0");
			break;
		case SQL_C_SBIGINT:
			debugPrintf("  valuetype: SQL_C_BIGINT\n");
			debugPrintf("  value: \"%lld\"\n",
				(int64_t)(*((int64_t *)parametervalue)));
			stmt->cur->inputBind(parametername,
				(int64_t)(*((int64_t *)parametervalue)));
			break;
		case SQL_C_UBIGINT:
			debugPrintf("  valuetype: SQL_C_UBIGINT\n");
			debugPrintf("  value: \"%lld\"\n",
				(int64_t)(*((int64_t *)parametervalue)));
			stmt->cur->inputBind(parametername,
				(int64_t)(*((uint64_t *)parametervalue)));
			break;
		case SQL_C_SLONG:
			debugPrintf("  valuetype: SQL_C_SLONG\n");
			debugPrintf("  value: \"%lld\"\n",
				(int64_t)(*((int32_t *)parametervalue)));
			stmt->cur->inputBind(parametername,
				(int64_t)(*((int32_t *)parametervalue)));
			break;
		case SQL_C_SSHORT:
			debugPrintf("  valuetype: SQL_C_SSHORT\n");
			debugPrintf("  value: \"%lld\"\n",
				(int64_t)(*((int16_t *)parametervalue)));
			stmt->cur->inputBind(parametername,
				(int64_t)(*((int16_t *)parametervalue)));
			break;
		case SQL_C_TINYINT:
		case SQL_C_STINYINT:
			debugPrintf("  valuetype: "
				"SQL_C_TINYINT/SQL_C_STINYINT\n");
			debugPrintf("  value: \"%lld\"\n",
					(int64_t)(*((char *)parametervalue)));
			stmt->cur->inputBind(parametername,
					(int64_t)(*((char *)parametervalue)));
			break;
		//case SQL_C_BOOKMARK:
		//	(dup of SQL_C_ULONG)
		case SQL_C_ULONG:
			debugPrintf("  valuetype: SQL_C_ULONG/SQL_C_BOOKMARK\n");
			debugPrintf("  value: \"%lld\"\n",
				(int64_t)(*((uint32_t *)parametervalue)));
			stmt->cur->inputBind(parametername,
				(int64_t)(*((uint32_t *)parametervalue)));
			break;
		case SQL_C_USHORT:
			debugPrintf("  valuetype: SQL_C_USHORT\n");
			debugPrintf("  value: \"%lld\"\n",
				(int64_t)(*((uint16_t *)parametervalue)));
			stmt->cur->inputBind(parametername,
				(int64_t)(*((uint16_t *)parametervalue)));
			break;
		case SQL_C_UTINYINT:
			debugPrintf("  valuetype: SQL_C_UTINYINT\n");
			debugPrintf("  value: \"%lld\"\n",
				(int64_t)(*((unsigned char *)parametervalue)));
			stmt->cur->inputBind(parametername,
				(int64_t)(*((unsigned char *)parametervalue)));
			break;
		case SQL_C_GUID:
			{
			debugPrintf("  valuetype: SQL_C_GUID\n");
			stmt->cur->inputBind(parametername,
				SQLR_BuildGuid(stmt,parameternumber,
						(SQLGUID *)parametervalue));
			}
			break;
		default:
			debugPrintf("  invalid valuetype\n");
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
					SQLLEN *strlen_or_ind);

static SQLRETURN SQLR_InputOutputBindParameter(
					SQLHSTMT statementhandle,
					SQLUSMALLINT parameternumber,
					SQLSMALLINT valuetype,
					SQLULEN lengthprecision,
					SQLSMALLINT parameterscale,
					SQLPOINTER parametervalue,
					SQLLEN bufferlength,
					SQLLEN *strlen_or_ind) {
	debugFunction();

	// FIXME:
	// Currently, SQL Relay doesn't support lob input/output binds.
	// Handle them as output binds for now.
	switch (valuetype) {
		//case SQL_C_VARBOOKMARK:
		//	(dup of SQL_C_BINARY)
		case SQL_C_BINARY:
			return SQLR_OutputBindParameter(
					statementhandle,
					parameternumber,
					valuetype,
					lengthprecision,
					parameterscale,
					parametervalue,
					bufferlength,
					strlen_or_ind);
	}

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("  NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	SQLRETURN	retval=SQL_SUCCESS;

	// convert parameternumber to a string
	char	*parametername=charstring::parseNumber(parameternumber);

	debugPrintf("  parametername: %s\n",parametername);
	debugPrintf("  parameternumber: %d\n",parameternumber);
	debugPrintf("  valuetype: %d\n",valuetype);
	debugPrintf("  lengthprecision: %lld\n",(uint64_t)lengthprecision);
	debugPrintf("  parameterscale: %d\n",parameterscale);
	debugPrintf("  bufferlength: %lld\n",(uint64_t)bufferlength);
	debugPrintf("  strlen_or_ind: %lld\n",(uint64_t)strlen_or_ind);

	// store the output bind for later
	outputbind	*ob=new outputbind;
	ob->parameternumber=parameternumber;
	ob->valuetype=valuetype;
	ob->lengthprecision=lengthprecision;
	ob->parameterscale=parameterscale;
	ob->parametervalue=parametervalue;
	ob->bufferlength=bufferlength;
	ob->strlen_or_ind=strlen_or_ind;
	stmt->inputoutputbinds.setValue(parameternumber,ob);

	switch (valuetype) {
		case SQL_C_CHAR:
		case SQL_C_BIT:
			debugPrintf("  valuetype: SQL_C_CHAR/SQL_C_BIT\n");
			debugPrintf("  value: \"%s\"\n",parametervalue);
			stmt->cur->defineInputOutputBindString(parametername,
						(const char *)parametervalue,
								bufferlength);
			break;
		case SQL_C_LONG:
		case SQL_C_SLONG:
		//case SQL_C_BOOKMARK:
		//	(dup of SQL_C_ULONG)
			debugPrintf("  valuetype: SQL_C_(S)LONG\n");
			debugPrintf("  value: \"%lld\"\n",
				(int64_t)(*((int32_t *)parametervalue)));
			stmt->cur->defineInputOutputBindInteger(parametername,
				(int64_t)(*((int32_t *)parametervalue)));
			break;
		case SQL_C_ULONG:
			debugPrintf("  valuetype: SQL_C_ULONG\n");
			debugPrintf("  value: \"%lld\"\n",
				(int64_t)(*((uint32_t *)parametervalue)));
			stmt->cur->defineInputOutputBindInteger(parametername,
				(int64_t)(*((uint32_t *)parametervalue)));
			break;
		case SQL_C_SBIGINT:
			debugPrintf("  valuetype: SQL_C_SBIGINT\n");
			debugPrintf("  value: \"%lld\"\n",
				(int64_t)(*((int64_t *)parametervalue)));
			stmt->cur->defineInputOutputBindInteger(parametername,
				(int64_t)(*((int64_t *)parametervalue)));
			break;
			break;
		case SQL_C_UBIGINT:
			debugPrintf("  valuetype: SQL_C_USBIGINT\n");
			debugPrintf("  value: \"%lld\"\n",
				(int64_t)(*((uint64_t *)parametervalue)));
			stmt->cur->defineInputOutputBindInteger(parametername,
				(int64_t)(*((uint64_t *)parametervalue)));
			break;
		case SQL_C_SHORT:
		case SQL_C_SSHORT:
			debugPrintf("  valuetype: SQL_C_(S)SHORT\n");
			debugPrintf("  value: \"%lld\"\n",
				(int64_t)(*((int16_t *)parametervalue)));
			stmt->cur->defineInputOutputBindInteger(parametername,
				(int64_t)(*((int16_t *)parametervalue)));
			break;
		case SQL_C_USHORT:
			debugPrintf("  valuetype: SQL_C_USHORT\n");
			debugPrintf("  value: \"%lld\"\n",
				(int64_t)(*((uint16_t *)parametervalue)));
			stmt->cur->defineInputOutputBindInteger(parametername,
				(int64_t)(*((uint16_t *)parametervalue)));
			break;
		case SQL_C_TINYINT:
		case SQL_C_STINYINT:
			debugPrintf("  valuetype: SQL_C_(S)TINYINT\n");
			debugPrintf("  value: \"%lld\"\n",
				(int64_t)(*((char *)parametervalue)));
			stmt->cur->defineInputOutputBindInteger(parametername,
				(int64_t)(*((char *)parametervalue)));
			break;
		case SQL_C_UTINYINT:
			debugPrintf("  valuetype: SQL_C_UTINYINT\n");
			debugPrintf("  value: \"%lld\"\n",
				(int64_t)(*((unsigned char *)parametervalue)));
			stmt->cur->defineInputOutputBindInteger(parametername,
				(int64_t)(*((unsigned char *)parametervalue)));
			break;
		case SQL_C_FLOAT:
			debugPrintf("  valuetype: SQL_C_FLOAT\n");
			debugPrintf("  value: \"%f\"\n",
				(float)(*((double *)parametervalue)));
			stmt->cur->defineInputOutputBindDouble(parametername,
				(float)(*((double *)parametervalue)),
				lengthprecision,
				parameterscale);
			break;
		case SQL_C_DOUBLE:
			debugPrintf("  valuetype: SQL_C_DOUBLE\n");
			debugPrintf("  value: \"%f\"\n",
				*((double *)parametervalue));
			stmt->cur->defineInputOutputBindDouble(parametername,
				*((double *)parametervalue),
				lengthprecision,
				parameterscale);
			break;
		case SQL_C_NUMERIC:
			debugPrintf("  valuetype: SQL_C_NUMERIC\n");
			// bind as a string, the result will be parsed
			stmt->cur->defineInputOutputBindString(parametername,
						(const char *)parametervalue,
								128);
			break;
		case SQL_C_DATE:
		case SQL_C_TYPE_DATE:
			{
			debugPrintf("  valuetype: SQL_C_DATE/SQL_C_TYPE_DATE\n");
			DATE_STRUCT	*ds=(DATE_STRUCT *)parametervalue;
			debugPrintf("  value: \"%d-%d-%d\"\n",
						ds->year,ds->month,ds->day);
			stmt->cur->defineInputOutputBindDate(parametername,
						ds->year,ds->month,ds->day,
						0,0,0,0,NULL,false);
			}
			break;
		case SQL_C_TIME:
		case SQL_C_TYPE_TIME:
			{
			debugPrintf("  valuetype: SQL_C_TIME/SQL_C_TYPE_TIME\n");
			TIME_STRUCT	*ts=(TIME_STRUCT *)parametervalue;
			debugPrintf("  value: \"%d:%d:%d\"\n",
						ts->hour,ts->minute,ts->second);
			stmt->cur->defineInputOutputBindDate(parametername,
						0,0,0,
						ts->hour,ts->minute,ts->second,
						0,NULL,false);
			break;
			}
		case SQL_C_TIMESTAMP:
		case SQL_C_TYPE_TIMESTAMP:
			{
			debugPrintf("  valuetype: "
				"SQL_C_TIMESTAMP/SQL_C_TYPE_TIMESTAMP\n");
			TIMESTAMP_STRUCT	*tss=
					(TIMESTAMP_STRUCT *)parametervalue;
			debugPrintf("  value: \"%d-%d-%d %d:%d:%d:%d\"\n",
					tss->year,tss->month,tss->day,
					tss->hour,tss->minute,tss->second,
					tss->fraction/1000);
			stmt->cur->defineInputOutputBindDate(parametername,
					tss->year,tss->month,tss->day,
					tss->hour,tss->minute,tss->second,
					tss->fraction/1000,NULL,false);
			break;
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
			debugPrintf("  valuetype: SQL_C_INTERVAL_XXX\n");
			// bind as a string, the result will be parsed
			stmt->cur->defineInputOutputBindString(parametername,
						(const char *)parametervalue,
								128);
			break;
		//case SQL_C_VARBOOKMARK:
		//	(dup of SQL_C_BINARY)
		case SQL_C_BINARY:
			debugPrintf("  valuetype: "
				"SQL_C_BINARY/SQL_C_VARBOOKMARK\n");
			//stmt->cur->defineInputOutputBindBlob(parametername);
			break;
		case SQL_C_GUID:
			debugPrintf("  valuetype: SQL_C_GUID\n");
			// bind as a string, the result will be parsed
			stmt->cur->defineInputOutputBindString(parametername,
						(const char *)parametervalue,
								128);
			break;
		default:
			debugPrintf("  invalid valuetype\n");
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
		debugPrintf("  NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	SQLRETURN	retval=SQL_SUCCESS;

	// convert parameternumber to a string
	char	*parametername=charstring::parseNumber(parameternumber);

	debugPrintf("  parametername: %s\n",parametername);
	debugPrintf("  parameternumber: %d\n",parameternumber);
	debugPrintf("  valuetype: %d\n",valuetype);
	debugPrintf("  lengthprecision: %lld\n",(uint64_t)lengthprecision);
	debugPrintf("  parameterscale: %d\n",parameterscale);
	debugPrintf("  bufferlength: %lld\n",(uint64_t)bufferlength);
	debugPrintf("  strlen_or_ind: %lld\n",(uint64_t)strlen_or_ind);

	// store the output bind for later
	outputbind	*ob=new outputbind;
	ob->parameternumber=parameternumber;
	ob->valuetype=valuetype;
	ob->lengthprecision=lengthprecision;
	ob->parameterscale=parameterscale;
	ob->parametervalue=parametervalue;
	ob->bufferlength=bufferlength;
	ob->strlen_or_ind=strlen_or_ind;
	stmt->outputbinds.setValue(parameternumber,ob);

	switch (valuetype) {
		case SQL_C_CHAR:
		case SQL_C_BIT:
			debugPrintf("  valuetype: SQL_C_CHAR/SQL_C_BIT\n");
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
		//case SQL_C_BOOKMARK:
		//	(dup of SQL_C_ULONG)
		case SQL_C_ULONG:
		case SQL_C_USHORT:
		case SQL_C_UTINYINT:
			debugPrintf("  valuetype: SQL_C_(INT of some kind)\n");
			stmt->cur->defineOutputBindInteger(parametername);
			break;
		case SQL_C_FLOAT:
		case SQL_C_DOUBLE:
			debugPrintf("  valuetype: SQL_C_FLOAT/SQL_C_DOUBLE\n");
			stmt->cur->defineOutputBindDouble(parametername);
			break;
		case SQL_C_NUMERIC:
			debugPrintf("  valuetype: SQL_C_NUMERIC\n");
			// bind as a string, the result will be parsed
			stmt->cur->defineOutputBindString(parametername,128);
			break;
		case SQL_C_DATE:
		case SQL_C_TYPE_DATE:
			debugPrintf("  valuetype: SQL_C_DATE/SQL_C_TYPE_DATE\n");
			stmt->cur->defineOutputBindDate(parametername);
			break;
		case SQL_C_TIME:
		case SQL_C_TYPE_TIME:
			debugPrintf("  valuetype: SQL_C_TIME/SQL_C_TYPE_TIME\n");
			stmt->cur->defineOutputBindDate(parametername);
			break;
		case SQL_C_TIMESTAMP:
		case SQL_C_TYPE_TIMESTAMP:
			debugPrintf("  valuetype: "
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
			debugPrintf("  valuetype: SQL_C_INTERVAL_XXX\n");
			// bind as a string, the result will be parsed
			stmt->cur->defineOutputBindString(parametername,128);
			break;
		//case SQL_C_VARBOOKMARK:
		//	(dup of SQL_C_BINARY)
		case SQL_C_BINARY:
			debugPrintf("  valuetype: "
				"SQL_C_BINARY/SQL_C_VARBOOKMARK\n");
			stmt->cur->defineOutputBindBlob(parametername);
			break;
		case SQL_C_GUID:
			debugPrintf("  valuetype: SQL_C_GUID\n");
			// bind as a string, the result will be parsed
			stmt->cur->defineOutputBindString(parametername,128);
			break;
		default:
			debugPrintf("  invalid valuetype\n");
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
		debugPrintf("  NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	debugPrintf("  parametertype: %d\n",parametertype);

	switch (inputoutputtype) {
		case SQL_PARAM_INPUT:
			debugPrintf("  inputoutputtype: "
						"SQL_PARAM_INPUT\n");
			return SQLR_InputBindParameter(statementhandle,
							parameternumber,
							valuetype,
							lengthprecision,
							parameterscale,
							parametervalue,
							strlen_or_ind);
		case SQL_PARAM_INPUT_OUTPUT:
			debugPrintf("  inputoutputtype: "
						"SQL_PARAM_INPUT_OUTPUT\n");
			return SQLR_InputOutputBindParameter(statementhandle,
							parameternumber,
							valuetype,
							lengthprecision,
							parameterscale,
							parametervalue,
							bufferlength,
							strlen_or_ind);
		case SQL_PARAM_OUTPUT:
			debugPrintf("  inputoutputtype: "
						"SQL_PARAM_OUTPUT\n");
			return SQLR_OutputBindParameter(statementhandle,
							parameternumber,
							valuetype,
							lengthprecision,
							parameterscale,
							parametervalue,
							bufferlength,
							strlen_or_ind);
		default:
			debugPrintf("  invalid inputoutputtype\n");
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

#ifdef _WIN32

#define SQLR_BOX	101
#define SQLR_LABEL	102
#define SQLR_EDIT	103
#define SQLR_OK		104
#define SQLR_CANCEL	105

static HINSTANCE	hinst;
static HWND		mainwindow;
static HWND		dsnedit;
static HWND		serveredit;
static HWND		portedit;
static HWND		socketedit;
static HWND		useredit;
static HWND		passwordedit;
static HWND		retrytimeedit;
static HWND		triesedit;
static HWND		krbedit;
static HWND		krbserviceedit;
static HWND		krbmechedit;
static HWND		krbflagsedit;
static HWND		tlsedit;
static HWND		tlsversionedit;
static HWND		tlscertificateedit;
static HWND		tlspasswordedit;
static HWND		tlsciphersedit;
static HWND		tlsvalidateedit;
static HWND		tlscaedit;
static HWND		tlsdepthedit;
static HWND		dbedit;
static HWND		debugedit;
static HWND		columnnamecaseedit;
static HWND		resultsetbuffersizeedit;
static HWND		dontgetcolumninfoedit;
static HWND		nullsasnullsedit;
static HWND		lazyconnectedit;
static HWND		clearbindsduringprepareedit;
static HWND		bindvariabledelimitersedit;

static const char	sqlrwindowclass[]="SQLRWindowClass";
static const int	labelwidth=135;
static const int	labelheight=18;
static const int	labeloffset=2;
static const int	labelcount=12;
static const int	editwidth=120;
static const int	xoffset=8;
static const int	yoffset=8;
static const int	boxwidth=labelwidth+editwidth+xoffset*3;
static const int	mainwindowwidth=boxwidth*3+xoffset*5;
static const int	boxheight=yoffset+
					labelcount*labelheight+
					labelcount*labeloffset+yoffset;
static const int	buttonheight=24;
static const int	buttonwidth=74;
static const int	mainwindowheight=yoffset+
					boxheight+yoffset+
					buttonheight+yoffset;

static WORD				dsnrequest;
static dictionary< char *, char * >	dsndict;

BOOL DllMain(HANDLE hinstdll, DWORD fdwreason, LPVOID lpvreserved) {
	debugFunction();

	if (fdwreason==DLL_PROCESS_ATTACH) {
		hinst=(HINSTANCE)hinstdll;
	}
	return TRUE;
}

static void createLabel(HWND parent, const char *label,
			int x, int y, int width, int height) {
	debugFunction();
	debugPrintf("  label: %s\n",label);

	HWND	labelwin=CreateWindow("STATIC",label,
					WS_CHILD|WS_VISIBLE|SS_RIGHT,
					x,y,width,height,
					parent,(HMENU)SQLR_LABEL,hinst,NULL);
	SendMessage(labelwin,
			WM_SETFONT,
			(WPARAM)GetStockObject(DEFAULT_GUI_FONT),
			MAKELPARAM(FALSE,0));

	debugPrintf("  success\n");
}

static HWND createEdit(HWND parent, const char *defaultvalue,
			int x, int y, int width, int height,
			int charlimit, bool numeric, bool first) {
	debugFunction();
	debugPrintf("  default value: %s\n",defaultvalue);

	DWORD	style=WS_CHILD|WS_VISIBLE|WS_BORDER|WS_TABSTOP|ES_LEFT;
	if (numeric) {
		style|=ES_NUMBER;
	}
	if (first) {
		style|=WS_GROUP;
	}
	HWND	editwin=CreateWindow("EDIT",(defaultvalue)?defaultvalue:"",
					style,x,y,width,height,
					parent,(HMENU)SQLR_EDIT,hinst,NULL);
	SendMessage(editwin,
			WM_SETFONT,
			(WPARAM)GetStockObject(DEFAULT_GUI_FONT),
			MAKELPARAM(FALSE,0));
	SendMessage(editwin,
			EM_SETLIMITTEXT,
			MAKEWPARAM(charlimit,0),
			MAKELPARAM(FALSE,0));

	debugPrintf("  success\n");
	return editwin;
}

static void createButton(HWND parent, const char *label,
					int x, int y, HMENU id,
					bool first) {
	debugFunction();
	debugPrintf("  label: %s\n",label);

	DWORD	style=WS_CHILD|WS_VISIBLE|WS_TABSTOP;
	if (first) {
		style|=WS_GROUP;
	}
	HWND	buttonwin=CreateWindow("BUTTON",label,style,
					x,y,buttonwidth,buttonheight,
					parent,id,hinst,NULL);
	SendMessage(buttonwin,
			WM_SETFONT,
			(WPARAM)GetStockObject(DEFAULT_GUI_FONT),
			MAKELPARAM(FALSE,0));

	debugPrintf("  success\n");
}

static void createControls(HWND hwnd) {
	debugFunction();

	// create boxes to surround the labels and edits
	debugPrintf("  box1\n");
	HWND	box1=CreateWindowEx(WS_EX_CONTROLPARENT,
				"STATIC","",
				WS_CHILD|WS_VISIBLE|SS_GRAYFRAME,
				xoffset,
				yoffset,
				boxwidth,
				boxheight,
				hwnd,(HMENU)SQLR_BOX,hinst,NULL);

	debugPrintf("  box2\n");
	HWND	box2=CreateWindowEx(WS_EX_CONTROLPARENT,
				"STATIC","",
				WS_CHILD|WS_VISIBLE|SS_GRAYFRAME,
				xoffset+boxwidth+xoffset,
				yoffset,
				boxwidth,
				boxheight,
				hwnd,(HMENU)SQLR_BOX,hinst,NULL);

	debugPrintf("  box3\n");
	HWND	box3=CreateWindowEx(WS_EX_CONTROLPARENT,
				"STATIC","",
				WS_CHILD|WS_VISIBLE|SS_GRAYFRAME,
				xoffset+boxwidth+xoffset+boxwidth+xoffset,
				yoffset,
				boxwidth,
				boxheight,
				hwnd,(HMENU)SQLR_BOX,hinst,NULL);

	debugPrintf("  labels...\n");

	// create labels...
	int	x=xoffset;
	int	y=yoffset;
	createLabel(box1,"DSN Name",
			x,y,labelwidth,labelheight);
	createLabel(box1,"Server",
			x,y+=(labelheight+labeloffset),
			labelwidth,labelheight);
	createLabel(box1,"Port",
			x,y+=(labelheight+labeloffset),
			labelwidth,labelheight);
	createLabel(box1,"Socket",
			x,y+=(labelheight+labeloffset),
			labelwidth,labelheight);
	createLabel(box1,"User",
			x,y+=(labelheight+labeloffset),
			labelwidth,labelheight);
	createLabel(box1,"Password",
			x,y+=(labelheight+labeloffset),
			labelwidth,labelheight);
	createLabel(box1,"Retry Time",
			x,y+=(labelheight+labeloffset),
			labelwidth,labelheight);
	createLabel(box1,"Tries",
			x,y+=(labelheight+labeloffset),
			labelwidth,labelheight);
	y=yoffset;
	createLabel(box2,"Enable Kerberos",
			x,y,
			labelwidth,labelheight);
	createLabel(box2,"Kerberos Service",
			x,y+=(labelheight+labeloffset),
			labelwidth,labelheight);
	createLabel(box2,"Kerberos Mech",
			x,y+=(labelheight+labeloffset),
			labelwidth,labelheight);
	createLabel(box2,"Kerberos Flags",
			x,y+=(labelheight+labeloffset),
			labelwidth,labelheight);
	createLabel(box2,"Enable TLS",
			x,y+=(labelheight+labeloffset),
			labelwidth,labelheight);
	createLabel(box2,"TLS Version",
			x,y+=(labelheight+labeloffset),
			labelwidth,labelheight);
	createLabel(box2,"TLS Certificate",
			x,y+=(labelheight+labeloffset),
			labelwidth,labelheight);
	createLabel(box2,"TLS Certificate Password",
			x,y+=(labelheight+labeloffset),
			labelwidth,labelheight);
	createLabel(box2,"TLS Ciphers",
			x,y+=(labelheight+labeloffset),
			labelwidth,labelheight);
	createLabel(box2,"TLS Validation",
			x,y+=(labelheight+labeloffset),
			labelwidth,labelheight);
	createLabel(box2,"TLS Certificate Authority",
			x,y+=(labelheight+labeloffset),
			labelwidth,labelheight);
	createLabel(box2,"TLS Depth",
			x,y+=(labelheight+labeloffset),
			labelwidth,labelheight);
	y=yoffset;
	createLabel(box3,"Database",
			x,y,
			labelwidth,labelheight);
	createLabel(box3,"Debug",
			x,y+=(labelheight+labeloffset),
			labelwidth,labelheight);
	createLabel(box3,"Column Name Case",
			x,y+=(labelheight+labeloffset),
			labelwidth,labelheight);
	createLabel(box3,"Result Set Buffer Size",
			x,y+=(labelheight+labeloffset),
			labelwidth,labelheight);
	createLabel(box3,"Don't Get Column Info",
			x,y+=(labelheight+labeloffset),
			labelwidth,labelheight);
	createLabel(box3,"Nulls As Nulls",
			x,y+=(labelheight+labeloffset),
			labelwidth,labelheight);
	createLabel(box3,"Lazy Connect",
			x,y+=(labelheight+labeloffset),
			labelwidth,labelheight);
	createLabel(box3,"Clear Binds During Prepare",
			x,y+=(labelheight+labeloffset),
			labelwidth,labelheight);
	createLabel(box3,"Bind Variable Delimiters",
			x,y+=(labelheight+labeloffset),
			labelwidth,labelheight);

	debugPrintf("  edits...\n");

	// create edits...
	x=xoffset+labelwidth+xoffset;
	y=yoffset;
	dsnedit=createEdit(box1,
			dsndict.getValue("DSN"),
			x,y,editwidth,labelheight,
			1024,false,true);
	serveredit=createEdit(box1,
			dsndict.getValue("Server"),
			x,y+=(labelheight+labeloffset),editwidth,labelheight,
			1024,false,false);
	portedit=createEdit(box1,
			dsndict.getValue("Port"),
			x,y+=(labelheight+labeloffset),editwidth,labelheight,
			6,true,false);
	socketedit=createEdit(box1,
			dsndict.getValue("Socket"),
			x,y+=(labelheight+labeloffset),editwidth,labelheight,
			1024,false,false);
	useredit=createEdit(box1,
			dsndict.getValue("User"),
			x,y+=(labelheight+labeloffset),editwidth,labelheight,
			1024,false,false);
	passwordedit=createEdit(box1,
			dsndict.getValue("Password"),
			x,y+=(labelheight+labeloffset),editwidth,labelheight,
			1024,false,false);
	retrytimeedit=createEdit(box1,
			dsndict.getValue("RetryTime"),
			x,y+=(labelheight+labeloffset),editwidth,labelheight,
			11,true,false);
	triesedit=createEdit(box1,
			dsndict.getValue("Tries"),
			x,y+=(labelheight+labeloffset),editwidth,labelheight,
			6,true,false);
	y=yoffset;
	krbedit=createEdit(box2,
			dsndict.getValue("Krb"),
			x,y,editwidth,labelheight,
			3,false,false);
	krbserviceedit=createEdit(box2,
			dsndict.getValue("Krbservice"),
			x,y+=(labelheight+labeloffset),editwidth,labelheight,
			1024,false,false);
	krbmechedit=createEdit(box2,
			dsndict.getValue("Krbmech"),
			x,y+=(labelheight+labeloffset),editwidth,labelheight,
			1024,false,false);
	krbflagsedit=createEdit(box2,
			dsndict.getValue("Krbflags"),
			x,y+=(labelheight+labeloffset),editwidth,labelheight,
			1024,false,false);
	tlsedit=createEdit(box2,
			dsndict.getValue("Tls"),
			x,y+=(labelheight+labeloffset),editwidth,labelheight,
			3,false,false);
	tlsversionedit=createEdit(box2,
			dsndict.getValue("Tlsversion"),
			x,y+=(labelheight+labeloffset),editwidth,labelheight,
			1024,false,false);
	tlscertificateedit=createEdit(box2,
			dsndict.getValue("Tlscert"),
			x,y+=(labelheight+labeloffset),editwidth,labelheight,
			1024,false,false);
	tlspasswordedit=createEdit(box2,
			dsndict.getValue("Tlspassword"),
			x,y+=(labelheight+labeloffset),editwidth,labelheight,
			1024,false,false);
	tlsciphersedit=createEdit(box2,
			dsndict.getValue("Tlsciphers"),
			x,y+=(labelheight+labeloffset),editwidth,labelheight,
			1024,false,false);
	tlsvalidateedit=createEdit(box2,
			dsndict.getValue("Tlsvalidate"),
			x,y+=(labelheight+labeloffset),editwidth,labelheight,
			9,false,false);
	tlscaedit=createEdit(box2,
			dsndict.getValue("Tlsca"),
			x,y+=(labelheight+labeloffset),editwidth,labelheight,
			1024,false,false);
	tlsdepthedit=createEdit(box2,
			dsndict.getValue("Tlsdepth"),
			x,y+=(labelheight+labeloffset),editwidth,labelheight,
			20,false,false);
	y=yoffset;
	dbedit=createEdit(box3,
			dsndict.getValue("Db"),
			x,y,editwidth,labelheight,
			1024,false,false);
	debugedit=createEdit(box3,
			dsndict.getValue("Debug"),
			x,y+=(labelheight+labeloffset),editwidth,labelheight,
			1024,false,false);
	columnnamecaseedit=createEdit(box3,
			dsndict.getValue("ColumnNameCase"),
			x,y+=(labelheight+labeloffset),editwidth,labelheight,
			5,false,false);
	resultsetbuffersizeedit=createEdit(box3,
			dsndict.getValue("ResultSetBufferSize"),
			x,y+=(labelheight+labeloffset),editwidth,labelheight,
			20,true,false);
	dontgetcolumninfoedit=createEdit(box3,
			dsndict.getValue("DontGetColumnInfo"),
			x,y+=(labelheight+labeloffset),editwidth,labelheight,
			1,true,false);
	nullsasnullsedit=createEdit(box3,
			dsndict.getValue("NullsAsNulls"),
			x,y+=(labelheight+labeloffset),editwidth,labelheight,
			1,true,false);
	lazyconnectedit=createEdit(box3,
			dsndict.getValue("LazyConnect"),
			x,y+=(labelheight+labeloffset),editwidth,labelheight,
			1,true,false);
	clearbindsduringprepareedit=createEdit(box3,
			dsndict.getValue("ClearBindsDuringPrepare"),
			x,y+=(labelheight+labeloffset),editwidth,labelheight,
			1,true,false);
	bindvariabledelimitersedit=createEdit(box3,
			dsndict.getValue("BindVariableDelimiters"),
			x,y+=(labelheight+labeloffset),editwidth,labelheight,
			4,false,false);

	debugPrintf("  buttons...\n");

	// create buttons...
	x=mainwindowwidth-xoffset-buttonwidth-xoffset-buttonwidth;
	y=yoffset+boxheight+yoffset;
	createButton(hwnd,"OK",x,y,(HMENU)SQLR_OK,true);
	createButton(hwnd,"Cancel",x+=buttonwidth+xoffset,y,
					(HMENU)SQLR_CANCEL,false);

	debugPrintf("  focus...\n");

	// set focus
	SetFocus(dsnedit);

	debugPrintf("  success\n");
}

static void parseDsn(const char *dsn) {
	debugFunction();

	// if the dsn is empty then set some defaults and return...
	if (charstring::isNullOrEmpty(dsn)) {

		debugPrintf("  empty dsn, returning defaults\n");

		// provide some defaults...
		dsndict.setValue("Port",charstring::duplicate("9000"));
		dsndict.setValue("RetryTime",charstring::duplicate("0"));
		dsndict.setValue("Tries",charstring::duplicate("1"));
		dsndict.setValue("Krb",charstring::duplicate("no"));
		dsndict.setValue("Krbservice",charstring::duplicate(""));
		dsndict.setValue("Krbmech",charstring::duplicate(""));
		dsndict.setValue("Krbflags",charstring::duplicate(""));
		dsndict.setValue("Tls",charstring::duplicate("no"));
		dsndict.setValue("Tlsversion",charstring::duplicate(""));
		dsndict.setValue("Tlscert",charstring::duplicate(""));
		dsndict.setValue("Tlspassword",charstring::duplicate(""));
		dsndict.setValue("Tlsciphers",charstring::duplicate(""));
		dsndict.setValue("Tlsvalidate",charstring::duplicate(""));
		dsndict.setValue("Tlsca",charstring::duplicate(""));
		dsndict.setValue("Tlsdepth",charstring::duplicate(""));
		dsndict.setValue("Db",charstring::duplicate(""));
		dsndict.setValue("Debug",charstring::duplicate("0"));
		dsndict.setValue("ColumnNameCase",
					charstring::duplicate("mixed"));
		dsndict.setValue("ResultSetBufferSize",
					charstring::duplicate("0"));
		dsndict.setValue("DontGetColumnInfo",
					charstring::duplicate("0"));
		dsndict.setValue("NullsAsNulls",
					charstring::duplicate("0"));
		dsndict.setValue("LazyConnect",
					charstring::duplicate("1"));
		dsndict.setValue("ClearBindsDuringPrepare",
					charstring::duplicate("1"));
		dsndict.setValue("BindVariableDelimiters",
					charstring::duplicate("?:@$"));

		debugPrintf("  success...\n");
		return;
	}

	debugPrintf("  non-empty dsn, parsing\n");

	// dsn is formatted like:
	// DSN=xxx\0Server=xxx\0Port=xxx\0\0
	for (const char *c=dsn; c && *c; c=c+charstring::length(c)+1) {
		char		**parts;
		uint64_t	partcount;
		charstring::split(c,"=",true,&parts,&partcount);
		dsndict.setValue(parts[0],parts[1]);
	}

	debugPrintf("  parsed successfully\n");

	// But, it usually just contains the DSN name itself and
	// the rest of the bits of data have to be fetched...

	// get the name of the dsn that we were given
	const char	*dsnval=dsndict.getValue("DSN");

	debugPrintf("  DSN=%s\n",dsnval);
	debugPrintf("  getting other values...\n");

	// get the rest of the data...
	if (!dsndict.getValue("Server")) {
		char	*server=new char[1024];
		SQLGetPrivateProfileString(dsnval,"Server","",
						server,1024,ODBC_INI);
		dsndict.setValue("Server",server);
	}
	if (!dsndict.getValue("Port")) {
		char	*port=new char[6];
		SQLGetPrivateProfileString(dsnval,"Port","9000",
						port,6,ODBC_INI);
		dsndict.setValue("Port",port);
	}
	if (!dsndict.getValue("Socket")) {
		char	*socket=new char[1024];
		SQLGetPrivateProfileString(dsnval,"Socket","",
						socket,1024,ODBC_INI);
		dsndict.setValue("Socket",socket);
	}
	if (!dsndict.getValue("User")) {
		char	*user=new char[1024];
		SQLGetPrivateProfileString(dsnval,"User","",
						user,1024,ODBC_INI);
		dsndict.setValue("User",user);
	}
	if (!dsndict.getValue("Password")) {
		// SQLGetPrivateProfileString doesn't appear to be able to
		// extract Passwords on all platforms.
		parameterstring	pstr;
		pstr.parse(dsnval);
		dsndict.setValue("Password",
			charstring::duplicate(pstr.getValue("Password")));
	}
	if (!dsndict.getValue("RetryTime")) {
		char	*retrytime=new char[11];
		SQLGetPrivateProfileString(dsnval,"RetryTime","0",
						retrytime,11,ODBC_INI);
		dsndict.setValue("RetryTime",retrytime);
	}
	if (!dsndict.getValue("Tries")) {
		char	*tries=new char[6];
		SQLGetPrivateProfileString(dsnval,"Tries","1",
						tries,6,ODBC_INI);
		dsndict.setValue("Tries",tries);
	}
	if (!dsndict.getValue("Krb")) {
		char	*krb=new char[4];
		SQLGetPrivateProfileString(dsnval,"Krb","no",
						krb,4,ODBC_INI);
		dsndict.setValue("Krb",krb);
	}
	if (!dsndict.getValue("Krbservice")) {
		char	*krbservice=new char[1024];
		SQLGetPrivateProfileString(dsnval,"Krbservice","",
						krbservice,1024,ODBC_INI);
		dsndict.setValue("Krbservice",krbservice);
	}
	if (!dsndict.getValue("Krbmech")) {
		char	*krbmech=new char[1024];
		SQLGetPrivateProfileString(dsnval,"Krbmech","",
						krbmech,1024,ODBC_INI);
		dsndict.setValue("Krbmech",krbmech);
	}
	if (!dsndict.getValue("Krbflags")) {
		char	*krbflags=new char[1024];
		SQLGetPrivateProfileString(dsnval,"Krbflags","",
						krbflags,1024,ODBC_INI);
		dsndict.setValue("Krbflags",krbflags);
	}
	if (!dsndict.getValue("Tls")) {
		char	*tls=new char[4];
		SQLGetPrivateProfileString(dsnval,"Tls","no",
						tls,4,ODBC_INI);
		dsndict.setValue("Tls",tls);
	}
	if (!dsndict.getValue("Tlsversion")) {
		char	*tlsversion=new char[16];
		SQLGetPrivateProfileString(dsnval,"Tlsversion","",
						tlsversion,16,ODBC_INI);
		dsndict.setValue("Tlsversion",tlsversion);
	}
	if (!dsndict.getValue("Tlscert")) {
		char	*tlscert=new char[1024];
		SQLGetPrivateProfileString(dsnval,"Tlscert","",
						tlscert,1024,ODBC_INI);
		dsndict.setValue("Tlscert",tlscert);
	}
	if (!dsndict.getValue("Tlspassword")) {
		char	*tlspassword=new char[1024];
		SQLGetPrivateProfileString(dsnval,"Tlspassword","",
						tlspassword,1024,ODBC_INI);
		dsndict.setValue("Tlspassword",tlspassword);
	}
	if (!dsndict.getValue("Tlsciphers")) {
		char	*tlsciphers=new char[1024];
		SQLGetPrivateProfileString(dsnval,"Tlsciphers","",
						tlsciphers,1024,ODBC_INI);
		dsndict.setValue("Tlsciphers",tlsciphers);
	}
	if (!dsndict.getValue("Tlsvalidate")) {
		char	*tlsvalidate=new char[1024];
		SQLGetPrivateProfileString(dsnval,"Tlsvalidate","",
						tlsvalidate,1024,ODBC_INI);
		dsndict.setValue("Tlsvalidate",tlsvalidate);
	}
	if (!dsndict.getValue("Tlsca")) {
		char	*tlsca=new char[1024];
		SQLGetPrivateProfileString(dsnval,"Tlsca","",
						tlsca,1024,ODBC_INI);
		dsndict.setValue("Tlsca",tlsca);
	}
	if (!dsndict.getValue("Tlsdepth")) {
		char	*tlsdepth=new char[6];
		SQLGetPrivateProfileString(dsnval,"Tlsdepth","",
						tlsdepth,6,ODBC_INI);
		dsndict.setValue("Tlsdepth",tlsdepth);
	}
	if (!dsndict.getValue("Db")) {
		char	*db=new char[1024];
		SQLGetPrivateProfileString(dsnval,"Db","",
						db,1024,ODBC_INI);
		dsndict.setValue("Db",db);
	}
	if (!dsndict.getValue("Debug")) {
		char	*debug=new char[1024];
		SQLGetPrivateProfileString(dsnval,"Debug","",
						debug,1024,ODBC_INI);
		dsndict.setValue("Debug",debug);
	}
	if (!dsndict.getValue("ColumnNameCase")) {
		char	*columnnamecase=new char[6];
		SQLGetPrivateProfileString(dsnval,"ColumnNameCase","mixed",
						columnnamecase,6,ODBC_INI);
		dsndict.setValue("ColumnNameCase",columnnamecase);
	}
	if (!dsndict.getValue("ResultSetBufferSize")) {
		char	*resultsetbuffersize=new char[20];
		SQLGetPrivateProfileString(dsnval,"ResultSetBufferSize","0",
					resultsetbuffersize,20,ODBC_INI);
		dsndict.setValue("ResultSetBufferSize",resultsetbuffersize);
	}
	if (!dsndict.getValue("DontGetColumnInfo")) {
		char	*dontgetcolumninfo=new char[2];
		SQLGetPrivateProfileString(dsnval,"DontGetColumnInfo","0",
						dontgetcolumninfo,2,ODBC_INI);
		dsndict.setValue("DontGetColumnInfo",dontgetcolumninfo);
	}
	if (!dsndict.getValue("NullsAsNulls")) {
		char	*nullsasnulls=new char[2];
		SQLGetPrivateProfileString(dsnval,"NullsAsNulls","0",
						nullsasnulls,2,ODBC_INI);
		dsndict.setValue("NullsAsNulls",nullsasnulls);
	}
	if (!dsndict.getValue("LazyConnect")) {
		char	*lazyconnect=new char[2];
		SQLGetPrivateProfileString(dsnval,"LazyConnect","1",
						lazyconnect,2,ODBC_INI);
		dsndict.setValue("LazyConnect",lazyconnect);
	}
	if (!dsndict.getValue("ClearBindsDuringPrepare")) {
		char	*clearbindsduringprepare=new char[2];
		SQLGetPrivateProfileString(dsnval,"ClearBindsDuringPrepare","1",
					clearbindsduringprepare,2,ODBC_INI);
		dsndict.setValue("ClearBindsDuringPrepare",
					clearbindsduringprepare);
	}
	if (!dsndict.getValue("BindVariableDelimiters")) {
		char	*bindvariabledelimiters=new char[5];
		SQLGetPrivateProfileString(dsnval,"BindVariableDelimiters",
					"?:@$",
					bindvariabledelimiters,5,ODBC_INI);
		dsndict.setValue("BindVariableDelimiters",
					bindvariabledelimiters);
	}

	debugPrintf("  success...\n");
}

static void dsnError() {
	debugFunction();

	DWORD	pferrorcode;
	char	errormsg[SQL_MAX_MESSAGE_LENGTH+1];
	
	for (WORD ierror=1; ierror<=16; ierror++) {
		if (SQLInstallerError(ierror,&pferrorcode,
					errormsg,sizeof(errormsg),
					NULL)==SQL_NO_DATA) {
			return;
		}

		MessageBox(NULL,errormsg,"Error",MB_OK|MB_ICONERROR);
	}
}

static bool validDsn() {
	debugFunction();

	// FIXME: SQLValidDSN always seems to return false
	return true;

	if (SQLValidDSN(dsndict.getValue("DSN"))==FALSE) {
		dsnError();
		return false;
	}
	return true;
}

static bool removeDsn() {
	debugFunction();
	
	if (SQLRemoveDSNFromIni(dsndict.getValue("DSN"))==FALSE) {
		dsnError();
		return false;
	}
	return true;
}

static void getDsnFromUi() {
	debugFunction();

	// populate dsndict from values in edit windows...

	// DSN...
	int	len=GetWindowTextLength(dsnedit);
	char	*data=new char[len+1];
	GetWindowText(dsnedit,data,len+1);
	delete[] dsndict.getValue("DSN");
	dsndict.setValue("DSN",data);

	// Server...
	len=GetWindowTextLength(serveredit);
	data=new char[len+1];
	GetWindowText(serveredit,data,len+1);
	delete[] dsndict.getValue("Server");
	dsndict.setValue("Server",data);

	// Port...
	len=GetWindowTextLength(portedit);
	data=new char[len+1];
	GetWindowText(portedit,data,len+1);
	delete[] dsndict.getValue("Port");
	dsndict.setValue("Port",data);

	// Socket...
	len=GetWindowTextLength(socketedit);
	data=new char[len+1];
	GetWindowText(socketedit,data,len+1);
	delete[] dsndict.getValue("Socket");
	dsndict.setValue("Socket",data);

	// User...
	len=GetWindowTextLength(useredit);
	data=new char[len+1];
	GetWindowText(useredit,data,len+1);
	delete[] dsndict.getValue("User");
	dsndict.setValue("User",data);

	// Password...
	len=GetWindowTextLength(passwordedit);
	data=new char[len+1];
	GetWindowText(passwordedit,data,len+1);
	delete[] dsndict.getValue("Password");
	dsndict.setValue("Password",data);

	// Retry Time...
	len=GetWindowTextLength(retrytimeedit);
	data=new char[len+1];
	GetWindowText(retrytimeedit,data,len+1);
	delete[] dsndict.getValue("RetryTime");
	dsndict.setValue("RetryTime",data);

	// Tries...
	len=GetWindowTextLength(triesedit);
	data=new char[len+1];
	GetWindowText(triesedit,data,len+1);
	delete[] dsndict.getValue("Tries");
	dsndict.setValue("Tries",data);

	// Krb
	len=GetWindowTextLength(krbedit);
	data=new char[len+1];
	GetWindowText(krbedit,data,len+1);
	delete[] dsndict.getValue("Krb");
	dsndict.setValue("Krb",data);

	// Krbservice
	len=GetWindowTextLength(krbserviceedit);
	data=new char[len+1];
	GetWindowText(krbserviceedit,data,len+1);
	delete[] dsndict.getValue("Krbservice");
	dsndict.setValue("Krbservice",data);

	// Krbmech
	len=GetWindowTextLength(krbmechedit);
	data=new char[len+1];
	GetWindowText(krbmechedit,data,len+1);
	delete[] dsndict.getValue("Krbmech");
	dsndict.setValue("Krbmech",data);

	// Krbflags
	len=GetWindowTextLength(krbflagsedit);
	data=new char[len+1];
	GetWindowText(krbflagsedit,data,len+1);
	delete[] dsndict.getValue("Krbflags");
	dsndict.setValue("Krbflags",data);

	// Tls
	len=GetWindowTextLength(tlsedit);
	data=new char[len+1];
	GetWindowText(tlsedit,data,len+1);
	delete[] dsndict.getValue("Tls");
	dsndict.setValue("Tls",data);

	// Tlsversion
	len=GetWindowTextLength(tlsversionedit);
	data=new char[len+1];
	GetWindowText(tlsversionedit,data,len+1);
	delete[] dsndict.getValue("Tlsversion");
	dsndict.setValue("Tlsversion",data);

	// Tlscert
	len=GetWindowTextLength(tlscertificateedit);
	data=new char[len+1];
	GetWindowText(tlscertificateedit,data,len+1);
	delete[] dsndict.getValue("Tlscert");
	dsndict.setValue("Tlscert",data);

	// Tlspassword
	len=GetWindowTextLength(tlspasswordedit);
	data=new char[len+1];
	GetWindowText(tlspasswordedit,data,len+1);
	delete[] dsndict.getValue("Tlspassword");
	dsndict.setValue("Tlspassword",data);

	// Tlsciphers
	len=GetWindowTextLength(tlsciphersedit);
	data=new char[len+1];
	GetWindowText(tlsciphersedit,data,len+1);
	delete[] dsndict.getValue("Tlsciphers");
	dsndict.setValue("Tlsciphers",data);

	// Tlsvalidate
	len=GetWindowTextLength(tlsvalidateedit);
	data=new char[len+1];
	GetWindowText(tlsvalidateedit,data,len+1);
	delete[] dsndict.getValue("Tlsvalidate");
	dsndict.setValue("Tlsvalidate",data);

	// Tlsca
	len=GetWindowTextLength(tlscaedit);
	data=new char[len+1];
	GetWindowText(tlscaedit,data,len+1);
	delete[] dsndict.getValue("Tlsca");
	dsndict.setValue("Tlsca",data);

	// Tlsdepth
	len=GetWindowTextLength(tlsdepthedit);
	data=new char[len+1];
	GetWindowText(tlsdepthedit,data,len+1);
	delete[] dsndict.getValue("Tlsdepth");
	dsndict.setValue("Tlsdepth",data);

	// Db
	len=GetWindowTextLength(dbedit);
	data=new char[len+1];
	GetWindowText(dbedit,data,len+1);
	delete[] dsndict.getValue("Db");
	dsndict.setValue("Db",data);

	// Debug...
	len=GetWindowTextLength(debugedit);
	data=new char[len+1];
	GetWindowText(debugedit,data,len+1);
	delete[] dsndict.getValue("Debug");
	dsndict.setValue("Debug",data);

	// ColumnNameCase
	len=GetWindowTextLength(columnnamecaseedit);
	data=new char[len+1];
	GetWindowText(columnnamecaseedit,data,len+1);
	delete[] dsndict.getValue("ColumnNameCase");
	dsndict.setValue("ColumnNameCase",data);

	// ResultSetBufferSize
	len=GetWindowTextLength(resultsetbuffersizeedit);
	data=new char[len+1];
	GetWindowText(resultsetbuffersizeedit,data,len+1);
	delete[] dsndict.getValue("ResultSetBufferSize");
	dsndict.setValue("ResultSetBufferSize",data);

	// DontGetColumnInfo
	len=GetWindowTextLength(dontgetcolumninfoedit);
	data=new char[len+1];
	GetWindowText(dontgetcolumninfoedit,data,len+1);
	delete[] dsndict.getValue("DontGetColumnInfo");
	dsndict.setValue("DontGetColumnInfo",data);

	// NullsAsNulls
	len=GetWindowTextLength(nullsasnullsedit);
	data=new char[len+1];
	GetWindowText(nullsasnullsedit,data,len+1);
	delete[] dsndict.getValue("NullsAsNulls");
	dsndict.setValue("NullsAsNulls",data);

	// LazyConnect
	len=GetWindowTextLength(lazyconnectedit);
	data=new char[len+1];
	GetWindowText(lazyconnectedit,data,len+1);
	delete[] dsndict.getValue("LazyConnect");
	dsndict.setValue("LazyConnect",data);

	// ClearBindsDuringPrepare
	len=GetWindowTextLength(clearbindsduringprepareedit);
	data=new char[len+1];
	GetWindowText(clearbindsduringprepareedit,data,len+1);
	delete[] dsndict.getValue("ClearBindsDuringPrepare");
	dsndict.setValue("ClearBindsDuringPrepare",data);

	// BindVariableDelimiters
	len=GetWindowTextLength(bindvariabledelimitersedit);
	data=new char[len+1];
	GetWindowText(bindvariabledelimitersedit,data,len+1);
	delete[] dsndict.getValue("BindVariableDelimiters");
	dsndict.setValue("BindVariableDelimiters",data);
}

static bool writeDsn() {
	debugFunction();

	const char	*dsnname=dsndict.getValue("DSN");
	if (SQLWriteDSNToIni(dsnname,SQL_RELAY)==FALSE) {
		dsnError();
		return false;
	}
	for (linkedlistnode< char * > *key=dsndict.getKeys()->getFirst();
						key; key=key->getNext()) {
		if (!charstring::compare(key->getValue(),"DSN")) {
			continue;
		}
		if (SQLWritePrivateProfileString(
					dsnname,
					key->getValue(),
					dsndict.getValue(key->getValue()),
					ODBC_INI)==FALSE) {
			return false;
		}
	}
	return true;
}


static bool saveDsn() {
	debugFunction();

	// validate dsn
	if (!validDsn()) {
		return false;
	}

	// add/config...
	bool	success=false;
	switch (dsnrequest) {
		case ODBC_ADD_DSN:
			getDsnFromUi();
			success=writeDsn();
			break;
		case ODBC_CONFIG_DSN:
			if (removeDsn()) {
				getDsnFromUi();
				success=writeDsn();
			}
			break;
	}

	return success;
}

static LRESULT CALLBACK windowProc(HWND hwnd, UINT umsg,
				WPARAM wparam, LPARAM lparam) {
	debugFunction();

	switch (umsg) {
		case WM_CREATE:
			debugPrintf("  WM_CREATE\n");
			createControls(hwnd);
			break;
		case WM_CLOSE:
			debugPrintf("  WM_CLOSE\n");
			DestroyWindow(hwnd);
			break;
		case WM_DESTROY:
			debugPrintf("  WM_DESTROY\n");
			PostQuitMessage(0);
			break;
		case WM_COMMAND:
			debugPrintf("  WM_COMMAND\n");
			switch (GetDlgCtrlID((HWND)lparam)) {
				case SQLR_OK:
					debugPrintf("  SQLR_OK\n");
					if (saveDsn()) {
						DestroyWindow(mainwindow);
					}
					break;
				case SQLR_CANCEL:
					debugPrintf("  SQLR_CANCEL\n");
					DestroyWindow(mainwindow);
					break;
			}
		default:
			debugPrintf("  default\n");
			return DefWindowProc(hwnd,umsg,wparam,lparam);
	}
	return 0;
}

BOOL INSTAPI ConfigDSN(HWND hwndparent, WORD frequest,
			LPCSTR lpszdriver, LPCSTR lpszattributes) {
	debugFunction();

	// sanity check
	if (!hwndparent) {
		// FIXME: actually, if this is null, just use the
		// data provided in lpszattributes non-interactively
		return FALSE;
	}

	// parse the dsn
	parseDsn(lpszattributes);

	// handle remove directly...
	if (frequest==ODBC_REMOVE_DSN) {
		bool	success=(validDsn() && removeDsn());
		return (success)?TRUE:FALSE;
	}

	// save request type
	dsnrequest=frequest;

	// display a dialog box displaying values supplied in lpszattributes
	// and prompting the user for data not supplied

	debugPrintf("  ConfigDSN create window class...\n");

	// create a window class...
	WNDCLASS	wcx;
	wcx.style=0;
	wcx.lpfnWndProc=windowProc;
	wcx.cbClsExtra=0;
	wcx.cbWndExtra=0;
	wcx.hInstance=hinst;
	wcx.hIcon=LoadIcon(NULL,IDI_APPLICATION);
	wcx.hCursor=LoadCursor(NULL,IDC_ARROW);
	wcx.hbrBackground=(HBRUSH)COLOR_WINDOW;
	wcx.lpszMenuName=NULL;
	wcx.lpszClassName=sqlrwindowclass;
	if (RegisterClass(&wcx)==FALSE) {
		return FALSE;
	}

	debugPrintf("  ConfigDSN adjust window rect...\n");

	// figure out how big the outside of the window needs to be,
	// based on the desired size of the inside...
	RECT	rect;
	rect.left=0;
	rect.top=0;
	rect.right=mainwindowwidth;
	rect.bottom=mainwindowheight;
	AdjustWindowRect(&rect,
			WS_CAPTION|WS_SYSMENU|WS_THICKFRAME,
			false);

	stringbuffer	reqtitle;
	if (frequest==ODBC_ADD_DSN) {
		reqtitle.append("Create a New Data Source to ");
		reqtitle.append(SQL_RELAY);
	} else {
		reqtitle.append(SQL_RELAY);
		reqtitle.append(" Data Source Configuration");
	}

	debugPrintf("  ConfigDSN create dialog...\n");

	// create the dialog window...
	mainwindow=CreateWindowEx(WS_EX_CONTROLPARENT,
				sqlrwindowclass,
				reqtitle.getString(),
				WS_OVERLAPPED|WS_CAPTION|
				WS_SYSMENU|WS_THICKFRAME,
				CW_USEDEFAULT,
				CW_USEDEFAULT,
				rect.right-rect.left,
				rect.bottom-rect.top,
				NULL,
				NULL,
				hinst,
				NULL);
	if (!mainwindow) {
		return FALSE;
	}

	debugPrintf("  ConfigDSN show window...\n");

	// show the window and take input...
	ShowWindow(mainwindow,SW_SHOWDEFAULT);
	UpdateWindow(mainwindow);
	MSG	msg;
	while (GetMessage(&msg,NULL,0,0)>0) {
		if (IsDialogMessage(mainwindow,&msg)==FALSE) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	debugPrintf("  ConfigDSN clean up...\n");

	// clean up
	UnregisterClass(sqlrwindowclass,hinst);

	// FIXME: clean up dsndict

	debugPrintf("  ConfigDSN success\n");

	return TRUE;
}

#endif

}
