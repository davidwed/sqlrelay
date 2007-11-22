// Copyright (c) 2007  David Muse
// See the file COPYING for more information

#include <sql.h>
#include <sqlext.h>

#include <sqlrelay/sqlrclient.h>
#include <rudiments/rawbuffer.h>

#define DEBUG_MESSAGES 1
#ifdef DEBUG_MESSAGES
	#define debugFunction() printf("%s:%s():%d:\n",__FILE__,__FUNCTION__,__LINE__); fflush(stdout);
	#define debugPrintf(format, ...) printf(format, ## __VA_ARGS__); fflush(stdout);
#else
	#define debugFunction() /* */
	#define debugPrintf(format, ...) /* */
#endif

extern "C" {

struct ENV {
	SQLINTEGER	odbcversion;
};

struct CONN {
	sqlrconnection	*conn;
};

struct STMT {
	sqlrcursor	*cur;
	uint64_t	currentrow;
};

// from sql.h
static SQLRETURN SQLR_SQLAllocConnect(SQLHENV environmenthandle,
					SQLHDBC *connectionhandle) {
	debugFunction();

	if (environmenthandle==SQL_NULL_HENV) {
		debugPrintf("NULL env handle\n");
		return SQL_INVALID_HANDLE;
	}

	ENV	*env=(ENV *)environmenthandle;

	CONN	*conn=new CONN;
	conn->conn=NULL;
	*connectionhandle=(SQLHDBC)conn;
	
	return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLAllocConnect(SQLHENV environmenthandle,
					SQLHDBC *connectionhandle) {
	debugFunction();
	return SQLR_SQLAllocConnect(environmenthandle,connectionhandle);
}

static SQLRETURN SQLR_SQLAllocEnv(SQLHENV *environmenthandle) {
	debugFunction();

	ENV	*env=new ENV;
	env->odbcversion=0;
	*environmenthandle=(SQLHENV)env;
	
	return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLAllocEnv(SQLHENV *environmenthandle) {
	debugFunction();
	return SQLR_SQLAllocEnv(environmenthandle);
}

static SQLRETURN SQLR_SQLAllocStmt(SQLHDBC connectionhandle,
					SQLHSTMT *statementhandle) {
	debugFunction();

	if (connectionhandle==SQL_NULL_HANDLE) {
		debugPrintf("NULL conn handle\n");
		return SQL_INVALID_HANDLE;
	}

	CONN	*conn=(CONN *)connectionhandle;

	STMT	*stmt=new STMT;
	stmt->cur=new sqlrcursor(conn->conn);
	*statementhandle=(SQLHSTMT)stmt;

	return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLAllocStmt(SQLHDBC connectionhandle,
					SQLHSTMT *statementhandle) {
	debugFunction();
	return SQLR_SQLAllocStmt(connectionhandle,statementhandle);
}

SQLRETURN SQL_API SQLBindCol(SQLHSTMT statementhandle,
					SQLUSMALLINT ColumnNumber,
					SQLSMALLINT TargetType,
					SQLPOINTER TargetValue,
					SQLLEN BufferLength,
					SQLLEN *StrLen_or_Ind) {
	debugFunction();

	if (statementhandle==SQL_NULL_HSTMT) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	STMT	*stmt=(STMT *)statementhandle;

	return SQL_SUCCESS;
}

#if (ODBCVER >= 0x0300)
SQLRETURN SQL_API SQLBindParam(SQLHSTMT statementhandle,
					SQLUSMALLINT ParameterNumber,
					SQLSMALLINT ValueType,
					SQLSMALLINT ParameterType,
					SQLULEN LengthPrecision,
					SQLSMALLINT ParameterScale,
					SQLPOINTER ParameterValue,
					SQLLEN *StrLen_or_Ind) {
	debugFunction();

	if (statementhandle==SQL_NULL_HSTMT) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	STMT	*stmt=(STMT *)statementhandle;

	return SQL_SUCCESS;
}
#endif

SQLRETURN SQL_API SQLCancel(SQLHSTMT statementhandle) {
	debugFunction();

	if (statementhandle==SQL_NULL_HSTMT) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	STMT	*stmt=(STMT *)statementhandle;

	return SQL_SUCCESS;
}

#if (ODBCVER >= 0x0300)
SQLRETURN SQL_API SQLCloseCursor(SQLHSTMT statementhandle) {
	debugFunction();

	if (statementhandle==SQL_NULL_HSTMT) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	STMT	*stmt=(STMT *)statementhandle;

	return SQL_SUCCESS;
}
#endif


SQLRETURN SQL_API SQLColumns(SQLHSTMT statementhandle,
					SQLCHAR *CatalogName,
					SQLSMALLINT NameLength1,
					SQLCHAR *SchemaName,
					SQLSMALLINT NameLength2,
					SQLCHAR *TableName,
					SQLSMALLINT NameLength3,
					SQLCHAR *ColumnName,
					SQLSMALLINT NameLength4) {
	debugFunction();

	if (statementhandle==SQL_NULL_HSTMT) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	STMT	*stmt=(STMT *)statementhandle;

	return SQL_SUCCESS;
}


SQLRETURN SQL_API SQLConnect(SQLHDBC connectionhandle,
					SQLCHAR *servername,
					SQLSMALLINT namelength1,
					SQLCHAR *username,
					SQLSMALLINT namelength2,
					SQLCHAR *authentication,
					SQLSMALLINT namelength3) {
	debugFunction();

	if (connectionhandle==SQL_NULL_HANDLE) {
		debugPrintf("NULL conn handle\n");
		return SQL_INVALID_HANDLE;
	}

	// FIXME: servername is really the dsn, so we need to use it to look up
	// the host, port, socket and default user/password in odbc.ini
	CONN	*conn=(CONN *)connectionhandle;
	conn->conn=new sqlrconnection("localhost",8006,"",
					"mysqltest",
					"mysqltest",
					0,1);
	debugPrintf("conn->conn=%08x\n",conn->conn);
conn->conn->debugOn();

	return SQL_SUCCESS;
}

#if (ODBCVER >= 0x0300)
SQLRETURN SQL_API SQLCopyDesc(SQLHDESC SourceDescHandle,
					SQLHDESC TargetDescHandle) {
	debugFunction();
	return SQL_SUCCESS;
}
#endif

SQLRETURN SQL_API SQLDataSources(SQLHENV environmenthandle,
					SQLUSMALLINT Direction,
					SQLCHAR *ServerName,
					SQLSMALLINT BufferLength1,
					SQLSMALLINT *NameLength1,
					SQLCHAR *Description,
					SQLSMALLINT BufferLength2,
					SQLSMALLINT *NameLength2) {
	debugFunction();

	if (environmenthandle==SQL_NULL_HENV) {
		debugPrintf("NULL env handle\n");
		return SQL_INVALID_HANDLE;
	}

	ENV	*env=(ENV *)environmenthandle;
	return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLDescribeCol(SQLHSTMT statementhandle,
					SQLUSMALLINT ColumnNumber,
					SQLCHAR *ColumnName,
					SQLSMALLINT BufferLength,
					SQLSMALLINT *NameLength,
					SQLSMALLINT *DataType,
					SQLULEN *ColumnSize,
					SQLSMALLINT *DecimalDigits,
					SQLSMALLINT *Nullable) {
	debugFunction();

	if (statementhandle==SQL_NULL_HSTMT) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	STMT	*stmt=(STMT *)statementhandle;

	return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLDisconnect(SQLHDBC connectionhandle) {
	debugFunction();

	if (connectionhandle==SQL_NULL_HANDLE) {
		debugPrintf("NULL conn handle\n");
		return SQL_INVALID_HANDLE;
	}

	CONN	*conn=(CONN *)connectionhandle;

	return SQL_SUCCESS;
}

#if (ODBCVER >= 0x0300)
SQLRETURN SQL_API SQLEndTran(SQLSMALLINT HandleType,
					SQLHANDLE Handle,
					SQLSMALLINT CompletionType) {
	debugFunction();
	return SQL_SUCCESS;
}
#endif

SQLRETURN SQL_API SQLError(SQLHENV environmenthandle,
					SQLHDBC connectionhandle,
					SQLHSTMT statementhandle,
					SQLCHAR *Sqlstate,
					SQLINTEGER *NativeError,
					SQLCHAR *MessageText,
					SQLSMALLINT BufferLength,
					SQLSMALLINT *TextLength) {
	debugFunction();

	if (environmenthandle==SQL_NULL_HENV) {
		debugPrintf("NULL env handle\n");
		return SQL_INVALID_HANDLE;
	}

	ENV	*env=(ENV *)environmenthandle;

	if (connectionhandle==SQL_NULL_HANDLE) {
		debugPrintf("NULL conn handle\n");
		return SQL_INVALID_HANDLE;
	}

	CONN	*conn=(CONN *)connectionhandle;

	if (statementhandle==SQL_NULL_HSTMT) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	STMT	*stmt=(STMT *)statementhandle;

	return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLExecDirect(SQLHSTMT statementhandle,
					SQLCHAR *statementtext,
					SQLINTEGER textlength) {
	debugFunction();
	debugPrintf("statement: \"%s\"\n",statementtext);
	debugPrintf("length: %d\n",textlength);

	if (statementhandle==SQL_NULL_HSTMT) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	STMT	*stmt=(STMT *)statementhandle;

	if (textlength==SQL_NTS || textlength==SQL_NTSL) {
		if (stmt->cur->sendQuery((const char *)statementtext)) {
			return SQL_SUCCESS;
		}
	} else {
		if (stmt->cur->sendQuery((const char *)statementtext,
						(uint32_t)textlength)) {
			return SQL_SUCCESS;
		}
	}
	return SQL_ERROR;
}

SQLRETURN SQL_API SQLExecute(SQLHSTMT statementhandle) {
	debugFunction();

	if (statementhandle==SQL_NULL_HSTMT) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	STMT	*stmt=(STMT *)statementhandle;
	stmt->currentrow=0;

	if (stmt->cur->executeQuery()) {
		return SQL_SUCCESS;
	}
	return SQL_ERROR;
}

SQLRETURN SQL_API SQLFetch(SQLHSTMT statementhandle) {
	debugFunction();

	if (statementhandle==SQL_NULL_HSTMT) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	STMT	*stmt=(STMT *)statementhandle;
	if (stmt->cur->getRow(stmt->currentrow)) {
		stmt->currentrow++;
		return SQL_SUCCESS;
	}
	return SQL_NO_DATA_FOUND;
}

#if (ODBCVER >= 0x0300)
SQLRETURN SQL_API SQLFetchScroll(SQLHSTMT statementhandle,
					SQLSMALLINT FetchOrientation,
					SQLROWOFFSET FetchOffset) {
	debugFunction();

	if (statementhandle==SQL_NULL_HSTMT) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	STMT	*stmt=(STMT *)statementhandle;

	return SQL_SUCCESS;
}
#endif

SQLRETURN SQL_API SQLFreeConnect(SQLHDBC connectionhandle) {
	debugFunction();

	if (connectionhandle==SQL_NULL_HANDLE) {
		debugPrintf("NULL conn handle\n");
		return SQL_INVALID_HANDLE;
	}

	CONN	*conn=(CONN *)connectionhandle;
	delete conn->conn;
	delete conn;

	return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLFreeEnv(SQLHENV environmenthandle) {
	debugFunction();

	if (environmenthandle==SQL_NULL_HENV) {
		debugPrintf("NULL env handle\n");
		return SQL_INVALID_HANDLE;
	}

	ENV	*env=(ENV *)environmenthandle;
	delete env;

	return SQL_SUCCESS;
}

#if (ODBCVER >= 0x0300)
SQLRETURN SQL_API SQLFreeHandle(SQLSMALLINT HandleType,
					SQLHANDLE Handle) {
	debugFunction();
	return SQL_SUCCESS;
}
#endif

SQLRETURN SQL_API SQLFreeStmt(SQLHSTMT statementhandle,
					SQLUSMALLINT Option) {
	debugFunction();

	if (statementhandle==SQL_NULL_HSTMT) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	STMT	*stmt=(STMT *)statementhandle;
	delete stmt->cur;
	delete stmt;

	return SQL_SUCCESS;
}

#if (ODBCVER >= 0x0300)
SQLRETURN SQL_API SQLGetConnectAttr(SQLHDBC connectionhandle,
					SQLINTEGER Attribute,
					SQLPOINTER Value,
					SQLINTEGER BufferLength,
					SQLINTEGER *StringLength) {
	debugFunction();

	if (connectionhandle==SQL_NULL_HANDLE) {
		debugPrintf("NULL conn handle\n");
		return SQL_INVALID_HANDLE;
	}

	CONN	*conn=(CONN *)connectionhandle;

	return SQL_SUCCESS;
}
#endif

SQLRETURN SQL_API SQLGetConnectOption(SQLHDBC connectionhandle,
					SQLUSMALLINT Option,
					SQLPOINTER Value) {
	debugFunction();

	if (connectionhandle==SQL_NULL_HANDLE) {
		debugPrintf("NULL conn handle\n");
		return SQL_INVALID_HANDLE;
	}

	CONN	*conn=(CONN *)connectionhandle;

	return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLGetCursorName(SQLHSTMT statementhandle,
					SQLCHAR *CursorName,
					SQLSMALLINT BufferLength,
					SQLSMALLINT *NameLength) {
	debugFunction();

	if (statementhandle==SQL_NULL_HSTMT) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	STMT	*stmt=(STMT *)statementhandle;

	return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLGetData(SQLHSTMT statementhandle,
					SQLUSMALLINT columnnumber,
					SQLSMALLINT targettype,
					SQLPOINTER targetvalue,
					SQLLEN bufferlength,
					SQLLEN *strlen_or_ind) {
	debugFunction();
	debugPrintf("columnnumber: %d\n",columnnumber);
	debugPrintf("targettype: %d\n",targettype);

	if (statementhandle==SQL_NULL_HSTMT) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	STMT	*stmt=(STMT *)statementhandle;

	if (columnnumber<1 || columnnumber>stmt->cur->colCount()) {
		return SQL_ERROR;
	}

	const char	*field=stmt->cur->getField(stmt->currentrow-1,
							columnnumber-1);

	if (!field) {
		*strlen_or_ind=SQL_NULL_DATA;
	} else {

		// FIXME: pay attention to target type and handle binary data
		//switch (targettype) {
		//}

		snprintf((char *)targetvalue,bufferlength,field);
		*strlen_or_ind=(SQLLEN)stmt->cur->getFieldLength(
							stmt->currentrow-1,
							columnnumber-1);
	}
	return SQL_SUCCESS;
}

#if (ODBCVER >= 0x0300)
SQLRETURN SQLGetDescField(SQLHDESC DescriptorHandle,
					SQLSMALLINT RecNumber,
					SQLSMALLINT FieldIdentifier,
					SQLPOINTER Value,
					SQLINTEGER BufferLength,
					SQLINTEGER *StringLength) {
	debugFunction();
	return SQL_SUCCESS;
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
	return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLGetDiagField(SQLSMALLINT HandleType,
					SQLHANDLE Handle,
					SQLSMALLINT RecNumber,
					SQLSMALLINT DiagIdentifier,
					SQLPOINTER DiagInfo,
					SQLSMALLINT BufferLength,
					SQLSMALLINT *StringLength) {
	debugFunction();
	return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLGetDiagRec(SQLSMALLINT HandleType,
					SQLHANDLE Handle,
					SQLSMALLINT RecNumber,
					SQLCHAR *Sqlstate,
					SQLINTEGER *NativeError,
					SQLCHAR *MessageText,
					SQLSMALLINT BufferLength,
					SQLSMALLINT *TextLength) {
	debugFunction();
	//return SQL_SUCCESS;
	return SQL_ERROR;
}

SQLRETURN SQL_API SQLGetEnvAttr(SQLHENV environmenthandle,
					SQLINTEGER attribute,
					SQLPOINTER value,
					SQLINTEGER bufferlength,
					SQLINTEGER *stringlength) {
	debugFunction();
	debugPrintf("attribute: %d\n",attribute);

	if (environmenthandle==SQL_NULL_HENV) {
		debugPrintf("NULL env handle\n");
		return SQL_INVALID_HANDLE;
	}

	ENV	*env=(ENV *)environmenthandle;

	switch (attribute) {
		case SQL_ATTR_OUTPUT_NTS:
			return SQL_SUCCESS;
		case SQL_ATTR_ODBC_VERSION:
			if (value) {
				*((SQLINTEGER *)value)=env->odbcversion;
			}
			if (stringlength) {
				*stringlength=sizeof(SQLINTEGER);
			}
			return SQL_SUCCESS;
		case SQL_ATTR_CONNECTION_POOLING:
			return SQL_SUCCESS;
		case SQL_ATTR_CP_MATCH:
			return SQL_SUCCESS;
	}
	return SQL_ERROR;
}
#endif /* ODBCVER >= 0x0300 */

SQLRETURN SQL_API SQLGetFunctions(SQLHDBC connectionhandle,
					SQLUSMALLINT functionid,
					SQLUSMALLINT *supported) {
	debugFunction();

	if (connectionhandle==SQL_NULL_HANDLE) {
		debugPrintf("NULL conn handle\n");
		return SQL_INVALID_HANDLE;
	}

	CONN	*conn=(CONN *)connectionhandle;

	switch (functionid) {
		case SQL_API_SQLALLOCCONNECT:
		case SQL_API_SQLALLOCENV:
		#if (ODBCVER >= 0x0300)
		case SQL_API_SQLALLOCHANDLE:
		#endif
		case SQL_API_SQLALLOCSTMT:
		case SQL_API_SQLBINDCOL:
		#if (ODBCVER >= 0x0300)
		case SQL_API_SQLBINDPARAM:
		#endif
		case SQL_API_SQLCANCEL:
		#if (ODBCVER >= 0x0300)
		case SQL_API_SQLCLOSECURSOR:
		case SQL_API_SQLCOLATTRIBUTE:
		#endif
		case SQL_API_SQLCOLUMNS:
		case SQL_API_SQLCONNECT:
		#if (ODBCVER >= 0x0300)
		case SQL_API_SQLCOPYDESC:
		#endif
		case SQL_API_SQLDATASOURCES:
		case SQL_API_SQLDESCRIBECOL:
		case SQL_API_SQLDISCONNECT:
		#if (ODBCVER >= 0x0300)
		case SQL_API_SQLENDTRAN:
		#endif
		case SQL_API_SQLERROR:
		case SQL_API_SQLEXECDIRECT:
		case SQL_API_SQLEXECUTE:
		case SQL_API_SQLFETCH:
		#if (ODBCVER >= 0x0300)
		case SQL_API_SQLFETCHSCROLL:
		#endif
		case SQL_API_SQLFREECONNECT:
		case SQL_API_SQLFREEENV:
		#if (ODBCVER >= 0x0300)
		case SQL_API_SQLFREEHANDLE:
		#endif
		case SQL_API_SQLFREESTMT:
		#if (ODBCVER >= 0x0300)
		case SQL_API_SQLGETCONNECTATTR:
		#endif
		case SQL_API_SQLGETCONNECTOPTION:
		case SQL_API_SQLGETCURSORNAME:
		case SQL_API_SQLGETDATA:
		#if (ODBCVER >= 0x0300)
		case SQL_API_SQLGETDESCFIELD:
		case SQL_API_SQLGETDESCREC:
		case SQL_API_SQLGETDIAGFIELD:
		case SQL_API_SQLGETDIAGREC:
		case SQL_API_SQLGETENVATTR:
		#endif
		case SQL_API_SQLGETFUNCTIONS:
		case SQL_API_SQLGETINFO:
		#if (ODBCVER >= 0x0300)
		case SQL_API_SQLGETSTMTATTR:
		#endif
		case SQL_API_SQLGETSTMTOPTION:
		case SQL_API_SQLGETTYPEINFO:
		case SQL_API_SQLNUMRESULTCOLS:
		case SQL_API_SQLPARAMDATA:
		case SQL_API_SQLPREPARE:
		case SQL_API_SQLPUTDATA:
		case SQL_API_SQLROWCOUNT:
		#if (ODBCVER >= 0x0300)
		case SQL_API_SQLSETCONNECTATTR:
		#endif
		case SQL_API_SQLSETCONNECTOPTION:
		case SQL_API_SQLSETCURSORNAME:
		#if (ODBCVER >= 0x0300)
		case SQL_API_SQLSETDESCFIELD:
		case SQL_API_SQLSETDESCREC:
		case SQL_API_SQLSETENVATTR:
		#endif
		case SQL_API_SQLSETPARAM:
		#if (ODBCVER >= 0x0300)
		case SQL_API_SQLSETSTMTATTR:
		#endif
		case SQL_API_SQLSETSTMTOPTION:
		case SQL_API_SQLSPECIALCOLUMNS:
		case SQL_API_SQLSTATISTICS:
		case SQL_API_SQLTABLES:
		case SQL_API_SQLTRANSACT:
		#if (ODBCVER >= 0x0300)
		case SQL_API_SQLALLOCHANDLESTD:
		case SQL_API_SQLBULKOPERATIONS:
		#endif /* ODBCVER >= 0x0300 */
		case SQL_API_SQLBINDPARAMETER:
		case SQL_API_SQLBROWSECONNECT:
		// dupe of SQL_API_SQLCOLATTRIBUTE
		//case SQL_API_SQLCOLATTRIBUTES:
		case SQL_API_SQLCOLUMNPRIVILEGES:
		case SQL_API_SQLDESCRIBEPARAM:
		case SQL_API_SQLDRIVERCONNECT:
		case SQL_API_SQLDRIVERS:
		case SQL_API_SQLEXTENDEDFETCH:
		case SQL_API_SQLFOREIGNKEYS:
		case SQL_API_SQLMORERESULTS:
		case SQL_API_SQLNATIVESQL:
		case SQL_API_SQLNUMPARAMS:
		case SQL_API_SQLPARAMOPTIONS:
		case SQL_API_SQLPRIMARYKEYS:
		case SQL_API_SQLPROCEDURECOLUMNS:
		case SQL_API_SQLPROCEDURES:
		case SQL_API_SQLSETPOS:
		case SQL_API_SQLSETSCROLLOPTIONS:
		case SQL_API_SQLTABLEPRIVILEGES:
			*supported=SQL_TRUE;
	}

	return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLGetInfo(SQLHDBC connectionhandle,
					SQLUSMALLINT infotype,
					SQLPOINTER infovalue,
					SQLSMALLINT bufferlength,
					SQLSMALLINT *stringlength) {
	debugFunction();
	debugPrintf("infotype: %d\n",infotype);
	debugPrintf("bufferlength: %d\n",bufferlength);

	if (connectionhandle==SQL_NULL_HANDLE) {
		debugPrintf("NULL conn handle\n");
		return SQL_INVALID_HANDLE;
	}

	CONN	*conn=(CONN *)connectionhandle;

	uint16_t	outsize=0;

	char	*strval="";
	switch (infotype) {
		case SQL_DRIVER_ODBC_VER:
			strval="03.00";
			break;
		case SQL_XOPEN_CLI_YEAR:
			strval="2007";
			break;
	}

	switch (outsize) {
		case 0:
			// string
			snprintf((char *)infovalue,bufferlength,strval);
			if (stringlength) {
				*stringlength=(SQLSMALLINT)
						charstring::length(strval);
			}
			break;
		case 16:
			// 16-bit integer
			*stringlength=(SQLSMALLINT)sizeof(uint16_t);
			break;
		case 32:
			// 32-bit integer
			*stringlength=(SQLSMALLINT)sizeof(uint32_t);
			break;
	}

	return SQL_SUCCESS;
}

static SQLRETURN SQLR_SQLGetStmtAttr(SQLHSTMT statementhandle,
					SQLINTEGER attribute,
					SQLPOINTER value,
					SQLINTEGER bufferlength,
					SQLINTEGER *stringlength) {
	debugFunction();
	debugPrintf("attribute: %d\n",attribute);
	debugPrintf("value: 0x%08x\n",value);
	debugPrintf("bufferlength: %d\n",bufferlength);
	debugPrintf("stringlength: 0x%08x\n",stringlength);

	if (statementhandle==SQL_NULL_HSTMT) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	STMT	*stmt=(STMT *)statementhandle;

	uint16_t	outsize=0;

	char		*strval="";
	uint16_t	val16=0xDEAD;
	uint32_t	val32=0xDEADBEEF;
	uint64_t	val64=0xDEADBEEFDEADBEEF;
	switch (attribute) {
		case SQL_QUERY_TIMEOUT:
			break;
		case SQL_MAX_ROWS:
			break;
		case SQL_NOSCAN:
			break;
		case SQL_MAX_LENGTH:
			break;
		case SQL_ASYNC_ENABLE:
			break;
		case SQL_BIND_TYPE:
			break;
		case SQL_CURSOR_TYPE:
			break;
		case SQL_CONCURRENCY:
			break;
		case SQL_KEYSET_SIZE:
			break;
		case SQL_ROWSET_SIZE:
			break;
		case SQL_SIMULATE_CURSOR:
			break;
		case SQL_RETRIEVE_DATA:
			break;
		case SQL_USE_BOOKMARKS:
			break;
		case SQL_GET_BOOKMARK:
			break;
		case SQL_ROW_NUMBER:
			break;
		#if (ODBCVER >= 0x0300)
		case SQL_ATTR_APP_ROW_DESC:
		case SQL_ATTR_APP_PARAM_DESC:
		case SQL_ATTR_IMP_ROW_DESC:
		case SQL_ATTR_IMP_PARAM_DESC:
			if (bufferlength==8) {
				outsize=64;
			} else {
				outsize=32;
			}
			break;
		case SQL_ATTR_CURSOR_SCROLLABLE:
			break;
		case SQL_ATTR_CURSOR_SENSITIVITY:
			break;
		// dup of SQL_ASYNC_ENABLE:
		//case SQL_ATTR_ASYNC_ENABLE:
		case SQL_ATTR_ENABLE_AUTO_IPD:
			break;
		case SQL_ATTR_FETCH_BOOKMARK_PTR:
			break;
		case SQL_ATTR_PARAM_BIND_OFFSET_PTR:
			break;
		case SQL_ATTR_PARAM_BIND_TYPE:
			break;
		case SQL_ATTR_PARAM_OPERATION_PTR:
			break;
		case SQL_ATTR_PARAM_STATUS_PTR:
			break;
		case SQL_ATTR_PARAMS_PROCESSED_PTR:
			break;
		case SQL_ATTR_PARAMSET_SIZE:
			break;
		case SQL_ATTR_ROW_BIND_OFFSET_PTR:
			break;
		case SQL_ATTR_ROW_OPERATION_PTR:
			break;
		case SQL_ATTR_ROW_STATUS_PTR:
			break;
		case SQL_ATTR_ROWS_FETCHED_PTR:
			break;
		case SQL_ATTR_ROW_ARRAY_SIZE:
			break;
		#endif
	}

	switch (outsize) {
		case 0:
			// string
			if (bufferlength>-1) {
				snprintf((char *)value,bufferlength,strval);
			} else {
				sprintf((char *)value,strval);
			}
			if (stringlength) {
				*stringlength=(SQLSMALLINT)
						charstring::length(strval);
			}
			break;
		case 16:
			// 16-bit integer
			rawbuffer::copy((void *)value,
					(const void *)&val16,
					sizeof(uint16_t));
			if (stringlength) {
				*stringlength=(SQLSMALLINT)sizeof(uint16_t);
			}
			break;
		case 32:
			// 32-bit integer
			rawbuffer::copy((void *)value,
					(const void *)&val32,
					sizeof(uint32_t));
			if (stringlength) {
				*stringlength=(SQLSMALLINT)sizeof(uint32_t);
			}
			break;
		case 64:
			// 64-bit integer
			rawbuffer::copy((void *)value,
					(const void *)&val64,
					sizeof(uint64_t));
			if (stringlength) {
				*stringlength=(SQLSMALLINT)sizeof(uint64_t);
			}
			break;
	}

	return SQL_SUCCESS;
}
#if (ODBCVER >= 0x0300)
SQLRETURN SQL_API SQLGetStmtAttr(SQLHSTMT statementhandle,
					SQLINTEGER attribute,
					SQLPOINTER value,
					SQLINTEGER bufferlength,
					SQLINTEGER *stringlength) {
	debugFunction();
	return SQLR_SQLGetStmtAttr(statementhandle,attribute,
					value,bufferlength,stringlength);
}
#endif /* ODBCVER >= 0x0300 */

SQLRETURN SQL_API SQLGetStmtOption(SQLHSTMT statementhandle,
					SQLUSMALLINT option,
					SQLPOINTER value) {
	debugFunction();
	return SQLR_SQLGetStmtAttr(statementhandle,option,value,-1,NULL);
}

SQLRETURN SQL_API SQLGetTypeInfo(SQLHSTMT statementhandle,
					SQLSMALLINT DataType) {
	debugFunction();

	if (statementhandle==SQL_NULL_HSTMT) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	STMT	*stmt=(STMT *)statementhandle;

	return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLNumResultCols(SQLHSTMT statementhandle,
					SQLSMALLINT *columncount) {
	debugFunction();

	if (statementhandle==SQL_NULL_HSTMT) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	STMT	*stmt=(STMT *)statementhandle;
	*columncount=(SQLSMALLINT)stmt->cur->colCount();

	return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLParamData(SQLHSTMT statementhandle,
					SQLPOINTER *Value) {
	debugFunction();

	if (statementhandle==SQL_NULL_HSTMT) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	STMT	*stmt=(STMT *)statementhandle;

	return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLPrepare(SQLHSTMT statementhandle,
					SQLCHAR *statementtext,
					SQLINTEGER textlength) {
	debugFunction();

	if (statementhandle==SQL_NULL_HSTMT) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	STMT	*stmt=(STMT *)statementhandle;

	if (textlength==SQL_NTS || textlength==SQL_NTSL) {
		debugPrintf("prepareQuery(\"%s\")\n",statementtext);
		stmt->cur->prepareQuery((const char *)statementtext);
	} else {
		debugPrintf("prepareQuery(\"%s\",%d)\n",
					statementtext,textlength);
		stmt->cur->prepareQuery((const char *)statementtext,
						(uint32_t)textlength);
	}

	return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLPutData(SQLHSTMT statementhandle,
					SQLPOINTER Data,
					SQLLEN StrLen_or_Ind) {
	debugFunction();

	if (statementhandle==SQL_NULL_HSTMT) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	STMT	*stmt=(STMT *)statementhandle;

	return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLRowCount(SQLHSTMT statementhandle,
					SQLLEN *rowcount) {
	debugFunction();

	if (statementhandle==SQL_NULL_HSTMT) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	STMT	*stmt=(STMT *)statementhandle;
	*rowcount=(SQLSMALLINT)stmt->cur->affectedRows();

	return SQL_SUCCESS;
}

#if (ODBCVER >= 0x0300)
SQLRETURN SQL_API SQLSetConnectAttr(SQLHDBC connectionhandle,
					SQLINTEGER Attribute,
					SQLPOINTER Value,
					SQLINTEGER StringLength) {
	debugFunction();

	if (connectionhandle==SQL_NULL_HANDLE) {
		debugPrintf("NULL conn handle\n");
		return SQL_INVALID_HANDLE;
	}

	CONN	*conn=(CONN *)connectionhandle;

	return SQL_SUCCESS;
}
#endif /* ODBCVER >= 0x0300 */

SQLRETURN SQL_API SQLSetConnectOption(SQLHDBC connectionhandle,
					SQLUSMALLINT Option,
					SQLULEN Value) {
	debugFunction();

	if (connectionhandle==SQL_NULL_HANDLE) {
		debugPrintf("NULL conn handle\n");
		return SQL_INVALID_HANDLE;
	}

	CONN	*conn=(CONN *)connectionhandle;

	return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLSetCursorName(SQLHSTMT statementhandle,
					SQLCHAR *CursorName,
					SQLSMALLINT NameLength) {
	debugFunction();

	if (statementhandle==SQL_NULL_HSTMT) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	STMT	*stmt=(STMT *)statementhandle;

	return SQL_SUCCESS;
}

#if (ODBCVER >= 0x0300)
SQLRETURN SQL_API SQLSetDescField(SQLHDESC DescriptorHandle,
					SQLSMALLINT RecNumber,
					SQLSMALLINT FieldIdentifier,
					SQLPOINTER Value,
					SQLINTEGER BufferLength) {
	debugFunction();
	return SQL_SUCCESS;
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
	return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLSetEnvAttr(SQLHENV environmenthandle,
					SQLINTEGER attribute,
					SQLPOINTER value,
					SQLINTEGER stringlength) {
	debugFunction();
	debugPrintf("attribute: %d\n",attribute);

	if (environmenthandle==SQL_NULL_HENV) {
		debugPrintf("NULL env handle\n");
		return SQL_INVALID_HANDLE;
	}

	ENV	*env=(ENV *)environmenthandle;

	switch (attribute) {
		case SQL_ATTR_OUTPUT_NTS:
			return SQL_SUCCESS;
		case SQL_ATTR_ODBC_VERSION:
			switch ((uint64_t)value) {
				case SQL_OV_ODBC2:
					env->odbcversion=SQL_OV_ODBC2;
					break;
				case SQL_OV_ODBC3:
					env->odbcversion=SQL_OV_ODBC3;
					break;
			}
			return SQL_SUCCESS;
		case SQL_ATTR_CONNECTION_POOLING:
			return SQL_SUCCESS;
		case SQL_ATTR_CP_MATCH:
			return SQL_SUCCESS;
	}
	return SQL_ERROR;
}
#endif /* ODBCVER >= 0x0300 */

SQLRETURN SQL_API SQLSetParam(SQLHSTMT statementhandle,
					SQLUSMALLINT ParameterNumber,
					SQLSMALLINT ValueType,
					SQLSMALLINT ParameterType,
					SQLULEN LengthPrecision,
					SQLSMALLINT ParameterScale,
					SQLPOINTER ParameterValue,
					SQLLEN *StrLen_or_Ind) {
	debugFunction();

	if (statementhandle==SQL_NULL_HSTMT) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	STMT	*stmt=(STMT *)statementhandle;

	return SQL_SUCCESS;
}

#if (ODBCVER >= 0x0300)
SQLRETURN SQL_API SQLSetStmtAttr(SQLHSTMT statementhandle,
					SQLINTEGER Attribute,
					SQLPOINTER Value,
					SQLINTEGER StringLength) {
	debugFunction();

	if (statementhandle==SQL_NULL_HSTMT) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	STMT	*stmt=(STMT *)statementhandle;

	return SQL_SUCCESS;
}
#endif

SQLRETURN SQL_API SQLSetStmtOption(SQLHSTMT statementhandle,
					SQLUSMALLINT Option,
					SQLROWCOUNT Value) {
	debugFunction();

	if (statementhandle==SQL_NULL_HSTMT) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	STMT	*stmt=(STMT *)statementhandle;

	return SQL_SUCCESS;
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

	if (statementhandle==SQL_NULL_HSTMT) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	STMT	*stmt=(STMT *)statementhandle;

	return SQL_SUCCESS;
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

	if (statementhandle==SQL_NULL_HSTMT) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	STMT	*stmt=(STMT *)statementhandle;

	return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLTables(SQLHSTMT statementhandle,
					SQLCHAR *CatalogName,
					SQLSMALLINT NameLength1,
					SQLCHAR *SchemaName,
					SQLSMALLINT NameLength2,
					SQLCHAR *TableName,
					SQLSMALLINT NameLength3,
					SQLCHAR *TableType,
					SQLSMALLINT NameLength4) {
	debugFunction();

	if (statementhandle==SQL_NULL_HSTMT) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	STMT	*stmt=(STMT *)statementhandle;

	return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLTransact(SQLHENV environmenthandle,
					SQLHDBC connectionhandle,
					SQLUSMALLINT CompletionType) {
	debugFunction();

	if (environmenthandle==SQL_NULL_HENV) {
		debugPrintf("NULL env handle\n");
		return SQL_INVALID_HANDLE;
	}

	ENV	*env=(ENV *)environmenthandle;

	if (connectionhandle==SQL_NULL_HANDLE) {
		debugPrintf("NULL conn handle\n");
		return SQL_INVALID_HANDLE;
	}

	CONN	*conn=(CONN *)connectionhandle;

	return SQL_SUCCESS;
}


// from sqlext.h
SQLRETURN SQL_API SQLDriverConnect(SQLHDBC hdbc,
					SQLHWND hwnd,
					SQLCHAR *szConnStrIn,
					SQLSMALLINT cbConnStrIn,
					SQLCHAR *szConnStrOut,
					SQLSMALLINT cbConnStrOutMax,
					SQLSMALLINT *pcbConnStrOut,
					SQLUSMALLINT fDriverCompletion) {
	debugFunction();
	return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLBrowseConnect(SQLHDBC hdbc,
					SQLCHAR *szConnStrIn,
					SQLSMALLINT cbConnStrIn,
					SQLCHAR *szConnStrOut,
					SQLSMALLINT cbConnStrOutMax,
					SQLSMALLINT *pcbConnStrOut) {
	debugFunction();
	return SQL_SUCCESS;
}

#if (ODBCVER >= 0x0300)
SQLRETURN SQL_API SQLBulkOperations(SQLHSTMT statementhandle,
					SQLSMALLINT Operation) {
	debugFunction();

	if (statementhandle==SQL_NULL_HSTMT) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	STMT	*stmt=(STMT *)statementhandle;

	return SQL_SUCCESS;
}
#endif /* ODBCVER >= 0x0300 */

static SQLRETURN SQLR_SQLColAttribute(SQLHSTMT statementhandle,
					SQLUSMALLINT columnnumber,
					SQLUSMALLINT fieldidentifier,
					SQLPOINTER characterattribute,
					SQLSMALLINT bufferlength,
					SQLSMALLINT *stringlength,
					SQLPOINTER numericattribute) {
	debugFunction();
	debugPrintf("columnnumber: %d\n",columnnumber);
	debugPrintf("fieldidentifier: %d\n",fieldidentifier);

	if (statementhandle==SQL_NULL_HSTMT) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	STMT	*stmt=(STMT *)statementhandle;

	if (columnnumber<1 || columnnumber>stmt->cur->colCount()) {
		return SQL_ERROR;
	}

	switch (fieldidentifier) {
		case SQL_COLUMN_COUNT:
			break;
		case SQL_COLUMN_NAME:
			break;
		case SQL_COLUMN_TYPE:
			break;
		case SQL_COLUMN_LENGTH:
			break;
		case SQL_COLUMN_PRECISION:
			break;
		case SQL_COLUMN_SCALE:
			break;
		case SQL_COLUMN_DISPLAY_SIZE:
			numericattribute=
				(SQLPOINTER)stmt->cur->
					getLongest(columnnumber-1);
			break;
		case SQL_COLUMN_NULLABLE:
			break;
		case SQL_COLUMN_UNSIGNED:
			break;
		case SQL_COLUMN_MONEY:
			break;
		case SQL_COLUMN_UPDATABLE:
			break;
		case SQL_COLUMN_AUTO_INCREMENT:
			break;
		case SQL_COLUMN_CASE_SENSITIVE:
			break;
		case SQL_COLUMN_SEARCHABLE:
			break;
		case SQL_COLUMN_TYPE_NAME:
			break;
		case SQL_COLUMN_TABLE_NAME:
			break;
		case SQL_COLUMN_OWNER_NAME:
			break;
		case SQL_COLUMN_QUALIFIER_NAME:
			break;
		case SQL_COLUMN_LABEL:
			snprintf((char *)characterattribute,bufferlength,
				stmt->cur->getColumnName(columnnumber-1));
			if (stringlength) {
				*stringlength=(SQLSMALLINT)
					charstring::length(
						stmt->cur->getColumnName(
							columnnumber-1));
			}
			break;
		#if (ODBCVER < 0x0300)
		case SQL_COLUMN_DRIVER_START:
			break;
		#endif
	}

	return SQL_SUCCESS;
}

#if (ODBCVER >= 0x0300)
SQLRETURN SQL_API SQLColAttribute(SQLHSTMT statementhandle,
					SQLUSMALLINT columnnumber,
					SQLUSMALLINT fieldidentifier,
					SQLPOINTER characterattribute,
					SQLSMALLINT bufferlength,
					SQLSMALLINT *stringlength,
					SQLPOINTER numericattribute) {
	debugFunction();
	return SQLR_SQLColAttribute(statementhandle,
					columnnumber,
					fieldidentifier,
					characterattribute,
					bufferlength,
					stringlength,
					numericattribute);
}
#endif /* ODBCVER >= 0x0300 */

SQLRETURN SQL_API SQLColAttributes(SQLHSTMT statementhandle,
					SQLUSMALLINT icol,
					SQLUSMALLINT fdesctype,
					SQLPOINTER rgbdesc,
					SQLSMALLINT cbdescmax,
					SQLSMALLINT *pcbdesc,
					SQLLEN *pfdesc) {
	return SQLR_SQLColAttribute(statementhandle,
					icol,
					fdesctype,
					rgbdesc,
					cbdescmax,
					pcbdesc,
					(SQLPOINTER)pfdesc);
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
	return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLDescribeParam(SQLHSTMT statementhandle,
					SQLUSMALLINT ipar,
					SQLSMALLINT *pfSqlType,
					SQLULEN *pcbParamDef,
					SQLSMALLINT *pibScale,
					SQLSMALLINT *pfNullable) {
	debugFunction();
	return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLExtendedFetch(SQLHSTMT statementhandle,
					SQLUSMALLINT fFetchType,
					SQLROWOFFSET irow,
					SQLROWSETSIZE *pcrow,
					SQLUSMALLINT *rgfRowStatus) {
	debugFunction();
	return SQL_SUCCESS;
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
	return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLMoreResults(SQLHSTMT statementhandle) {
	debugFunction();
	//return SQL_SUCCESS;
	return SQL_ERROR;
}

SQLRETURN SQL_API SQLNativeSql(SQLHDBC hdbc,
					SQLCHAR *szSqlStrIn,
					SQLINTEGER cbSqlStrIn,
					SQLCHAR *szSqlStr,
					SQLINTEGER cbSqlStrMax,
					SQLINTEGER *pcbSqlStr) {
	debugFunction();
	return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLNumParams(SQLHSTMT statementhandle,
					SQLSMALLINT *pcpar) {
	debugFunction();
	return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLParamOptions(SQLHSTMT statementhandle,
					SQLULEN crow,
					SQLULEN *pirow) {
	debugFunction();
	return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLPrimaryKeys(SQLHSTMT statementhandle,
					SQLCHAR *szCatalogName,
					SQLSMALLINT cbCatalogName,
					SQLCHAR *szSchemaName,
					SQLSMALLINT cbSchemaName,
					SQLCHAR *szTableName,
					SQLSMALLINT cbTableName) {
	debugFunction();
	return SQL_SUCCESS;
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
	return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLProcedures(SQLHSTMT statementhandle,
					SQLCHAR *szCatalogName,
					SQLSMALLINT cbCatalogName,
					SQLCHAR *szSchemaName,
					SQLSMALLINT cbSchemaName,
					SQLCHAR *szProcName,
					SQLSMALLINT cbProcName) {
	debugFunction();
	return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLSetPos(SQLHSTMT statementhandle,
					SQLSETPOSIROW irow,
					SQLUSMALLINT fOption,
					SQLUSMALLINT fLock) {
	debugFunction();
	return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLTablePrivileges(SQLHSTMT statementhandle,
					SQLCHAR *szCatalogName,
					SQLSMALLINT cbCatalogName,
					SQLCHAR *szSchemaName,
					SQLSMALLINT cbSchemaName,
					SQLCHAR *szTableName,
					SQLSMALLINT cbTableName) {
	debugFunction();
	return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLDrivers(SQLHENV environmenthandle,
					SQLUSMALLINT fDirection,
					SQLCHAR *szDriverDesc,
					SQLSMALLINT cbDriverDescMax,
					SQLSMALLINT *pcbDriverDesc,
					SQLCHAR *szDriverAttributes,
					SQLSMALLINT cbDrvrAttrMax,
					SQLSMALLINT *pcbDrvrAttr) {
	debugFunction();

	if (environmenthandle==SQL_NULL_HENV) {
		debugPrintf("NULL env handle\n");
		return SQL_INVALID_HANDLE;
	}

	ENV	*env=(ENV *)environmenthandle;
	return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLBindParameter(SQLHSTMT statementhandle,
					SQLUSMALLINT ipar,
					SQLSMALLINT fParamType,
					SQLSMALLINT fCType,
					SQLSMALLINT fSqlType,
					SQLULEN cbColDef,
					SQLSMALLINT ibScale,
					SQLPOINTER rgbValue,
					SQLLEN cbValueMax,
					SQLLEN *pcbValue) {
	debugFunction();
	return SQL_SUCCESS;
}

#if (ODBCVER >= 0x0300)
SQLRETURN SQLR_SQLAllocHandle(SQLSMALLINT handletype,
					SQLHANDLE inputhandle,
					SQLHANDLE *outputhandle) {
	debugFunction();
	debugPrintf("handletype: %d\n",handletype);

	switch (handletype) {
		case SQL_HANDLE_ENV:
			return SQLR_SQLAllocEnv((SQLHENV *)outputhandle);
		case SQL_HANDLE_DBC:
			return SQLR_SQLAllocConnect((SQLHENV)inputhandle,
						(SQLHDBC *)outputhandle);
		case SQL_HANDLE_STMT:
			return SQLR_SQLAllocStmt((SQLHDBC)inputhandle,
						(SQLHSTMT *)outputhandle);
		case SQL_HANDLE_DESC:
			return SQL_SUCCESS;
	}
	return SQL_ERROR;
}

SQLRETURN SQL_API SQLAllocHandleStd(SQLSMALLINT handletype,
					SQLHANDLE inputhandle,
					SQLHANDLE *outputhandle) {
	debugFunction();
	return SQLR_SQLAllocHandle(handletype,inputhandle,outputhandle);
}

SQLRETURN SQL_API SQLAllocHandle(SQLSMALLINT handletype,
					SQLHANDLE inputhandle,
					SQLHANDLE *outputhandle) {
	debugFunction();
	return SQLR_SQLAllocHandle(handletype,inputhandle,outputhandle);
}
#endif

}
