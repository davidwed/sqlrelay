// Copyright (c) 2007  David Muse
// See the file COPYING for more information

#include <sql.h>
#include <sqlext.h>
#include <odbcinst.h>

#include <sqlrelay/sqlrclient.h>
#include <rudiments/rawbuffer.h>
#include <rudiments/linkedlist.h>

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

struct ENV;
struct CONN;

struct FIELD {
	SQLSMALLINT	targettype;
	SQLPOINTER	targetvalue;
	SQLLEN		bufferlength;
	SQLLEN		*strlen_or_ind;
};

struct STMT {
	sqlrcursor			*cur;
	uint64_t			currentrow;
	CONN				*conn;
	linkedlist< char * >		errorlist;
	char				*name;
	numericdictionary< FIELD * >	fieldlist;
};

struct CONN {
	sqlrconnection		*con;
	ENV			*env;
	linkedlist< STMT * >	stmtlist;
};

struct ENV {
	SQLINTEGER		odbcversion;
	linkedlist< CONN * >	connlist;
};

static SQLRETURN SQLR_SQLAllocConnect(SQLHENV environmenthandle,
					SQLHDBC *connectionhandle) {
	debugFunction();

	ENV	*env=(ENV *)environmenthandle;
	if (environmenthandle==SQL_NULL_HENV || !env) {
		debugPrintf("NULL env handle\n");
		return SQL_INVALID_HANDLE;
	}

	CONN	*conn=new CONN;
	conn->con=NULL;
	*connectionhandle=(SQLHDBC)conn;

	env->connlist.append(conn);
	conn->env=env;
	
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

	CONN	*conn=(CONN *)connectionhandle;
	if (connectionhandle==SQL_NULL_HANDLE || !conn || !conn->con) {
		debugPrintf("NULL conn handle\n");
		return SQL_INVALID_HANDLE;
	}

	STMT	*stmt=new STMT;
	stmt->cur=new sqlrcursor(conn->con);
	*statementhandle=(SQLHSTMT)stmt;
	stmt->name=NULL;

	conn->stmtlist.append(stmt);
	stmt->conn=conn;

	return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLAllocStmt(SQLHDBC connectionhandle,
					SQLHSTMT *statementhandle) {
	debugFunction();
	return SQLR_SQLAllocStmt(connectionhandle,statementhandle);
}

SQLRETURN SQL_API SQLBindCol(SQLHSTMT statementhandle,
					SQLUSMALLINT columnnumber,
					SQLSMALLINT targettype,
					SQLPOINTER targetvalue,
					SQLLEN bufferlength,
					SQLLEN *strlen_or_ind) {
	debugFunction();
	debugPrintf("columnnumber: %d\n",columnnumber);

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	if (columnnumber<1) {
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

SQLRETURN SQLR_SQLInputBindParam(SQLHSTMT statementhandle,
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

	// FIXME: implement this
	switch (valuetype) {
		case SQL_C_CHAR:
			break;
		case SQL_C_LONG:
			break;
		case SQL_C_SHORT:
			break;
		case SQL_C_FLOAT:
			break;
		case SQL_C_DOUBLE:
			break;
		case SQL_C_NUMERIC:
			break;
		case SQL_C_DEFAULT:
			break;
		case SQL_C_DATE:
			break;
		case SQL_C_TIME:
			break;
		case SQL_C_TIMESTAMP:
			break;
		case SQL_C_TYPE_DATE:
			break;
		case SQL_C_TYPE_TIME:
			break;
		case SQL_C_TYPE_TIMESTAMP:
			break;
		case SQL_C_INTERVAL_YEAR:
			break;
		case SQL_C_INTERVAL_MONTH:
			break;
		case SQL_C_INTERVAL_DAY:
			break;
		case SQL_C_INTERVAL_HOUR:
			break;
		case SQL_C_INTERVAL_MINUTE:
			break;
		case SQL_C_INTERVAL_SECOND:
			break;
		case SQL_C_INTERVAL_YEAR_TO_MONTH:
			break;
		case SQL_C_INTERVAL_DAY_TO_HOUR:
			break;
		case SQL_C_INTERVAL_DAY_TO_MINUTE:
			break;
		case SQL_C_INTERVAL_DAY_TO_SECOND:
			break;
		case SQL_C_INTERVAL_HOUR_TO_MINUTE:
			break;
		case SQL_C_INTERVAL_HOUR_TO_SECOND:
			break;
		case SQL_C_INTERVAL_MINUTE_TO_SECOND:
			break;
		case SQL_C_BINARY:
			break;
		case SQL_C_BIT:
			break;
		case SQL_C_SBIGINT:
			break;
		case SQL_C_UBIGINT:
			break;
		case SQL_C_TINYINT:
			break;
		case SQL_C_SLONG:
			break;
		case SQL_C_SSHORT:
			break;
		case SQL_C_STINYINT:
			break;
		case SQL_C_ULONG:
			break;
		case SQL_C_USHORT:
			break;
		case SQL_C_UTINYINT:
			break;
		// apparently the same as SQL_C_ULONG
		//case SQL_C_BOOKMARK:
			//break;
		case SQL_C_GUID:
			break;
		// apparently the same as SQL_C_BINARY:
		//case SQL_C_VARBOOKMARK:
			//break;
	}

	return SQL_ERROR;
}

SQLRETURN SQLR_SQLOutputBindParam(SQLHSTMT statementhandle,
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

	// FIXME: implement this
	switch (valuetype) {
		case SQL_C_CHAR:
			break;
		case SQL_C_LONG:
			break;
		case SQL_C_SHORT:
			break;
		case SQL_C_FLOAT:
			break;
		case SQL_C_DOUBLE:
			break;
		case SQL_C_NUMERIC:
			break;
		case SQL_C_DEFAULT:
			break;
		case SQL_C_DATE:
			break;
		case SQL_C_TIME:
			break;
		case SQL_C_TIMESTAMP:
			break;
		case SQL_C_TYPE_DATE:
			break;
		case SQL_C_TYPE_TIME:
			break;
		case SQL_C_TYPE_TIMESTAMP:
			break;
		case SQL_C_INTERVAL_YEAR:
			break;
		case SQL_C_INTERVAL_MONTH:
			break;
		case SQL_C_INTERVAL_DAY:
			break;
		case SQL_C_INTERVAL_HOUR:
			break;
		case SQL_C_INTERVAL_MINUTE:
			break;
		case SQL_C_INTERVAL_SECOND:
			break;
		case SQL_C_INTERVAL_YEAR_TO_MONTH:
			break;
		case SQL_C_INTERVAL_DAY_TO_HOUR:
			break;
		case SQL_C_INTERVAL_DAY_TO_MINUTE:
			break;
		case SQL_C_INTERVAL_DAY_TO_SECOND:
			break;
		case SQL_C_INTERVAL_HOUR_TO_MINUTE:
			break;
		case SQL_C_INTERVAL_HOUR_TO_SECOND:
			break;
		case SQL_C_INTERVAL_MINUTE_TO_SECOND:
			break;
		case SQL_C_BINARY:
			break;
		case SQL_C_BIT:
			break;
		case SQL_C_SBIGINT:
			break;
		case SQL_C_UBIGINT:
			break;
		case SQL_C_TINYINT:
			break;
		case SQL_C_SLONG:
			break;
		case SQL_C_SSHORT:
			break;
		case SQL_C_STINYINT:
			break;
		case SQL_C_ULONG:
			break;
		case SQL_C_USHORT:
			break;
		case SQL_C_UTINYINT:
			break;
		// apparently the same as SQL_C_ULONG
		//case SQL_C_BOOKMARK:
			//break;
		case SQL_C_GUID:
			break;
		// apparently the same as SQL_C_BINARY:
		//case SQL_C_VARBOOKMARK:
			//break;
	}

	return SQL_ERROR;
}

SQLRETURN SQLR_SQLBindParam(SQLHSTMT statementhandle,
					SQLUSMALLINT parameternumber,
					SQLSMALLINT valuetype,
					SQLSMALLINT parametertype,
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

	switch (parametertype) {
		case SQL_PARAM_INPUT:
			return SQLR_SQLInputBindParam(statementhandle,
							parameternumber,
							valuetype,
							lengthprecision,
							parameterscale,
							parametervalue,
							strlen_or_ind);
		case SQL_PARAM_INPUT_OUTPUT:
			return SQLR_SQLInputBindParam(statementhandle,
							parameternumber,
							valuetype,
							lengthprecision,
							parameterscale,
							parametervalue,
							strlen_or_ind) &&
				SQLR_SQLOutputBindParam(statementhandle,
							parameternumber,
							valuetype,
							lengthprecision,
							parameterscale,
							parametervalue,
							strlen_or_ind);
		case SQL_PARAM_OUTPUT:
			return SQLR_SQLOutputBindParam(statementhandle,
							parameternumber,
							valuetype,
							lengthprecision,
							parameterscale,
							parametervalue,
							strlen_or_ind);
	}

	return SQL_ERROR;
}

#if (ODBCVER >= 0x0300)
SQLRETURN SQL_API SQLBindParam(SQLHSTMT statementhandle,
					SQLUSMALLINT parameternumber,
					SQLSMALLINT valuetype,
					SQLSMALLINT parametertype,
					SQLULEN lengthprecision,
					SQLSMALLINT parameterscale,
					SQLPOINTER parametervalue,
					SQLLEN *strlen_or_ind) {
	debugFunction();
	return SQLR_SQLBindParam(statementhandle,
					parameternumber,
					valuetype,
					parametertype,
					lengthprecision,
					parameterscale,
					parametervalue,
					strlen_or_ind);
}
#endif

SQLRETURN SQL_API SQLCancel(SQLHSTMT statementhandle) {
	debugFunction();

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	// not supported by SQL Relay
	return SQL_ERROR;
}

SQLRETURN SQLR_SQLCloseCursor(SQLHSTMT statementhandle) {
	debugFunction();

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	// FIXME: implement this

	return SQL_SUCCESS;
}

#if (ODBCVER >= 0x0300)
SQLRETURN SQL_API SQLCloseCursor(SQLHSTMT statementhandle) {
	debugFunction();
	return SQLR_SQLCloseCursor(statementhandle);
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

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	// not supported by SQL Relay
	return SQL_ERROR;
}


SQLRETURN SQL_API SQLConnect(SQLHDBC connectionhandle,
					SQLCHAR *servername,
					SQLSMALLINT namelength1,
					SQLCHAR *username,
					SQLSMALLINT namelength2,
					SQLCHAR *authentication,
					SQLSMALLINT namelength3) {
	debugFunction();

	CONN	*conn=(CONN *)connectionhandle;
	if (connectionhandle==SQL_NULL_HANDLE || !conn) {
		debugPrintf("NULL conn handle\n");
		return SQL_INVALID_HANDLE;
	}

	// FIXME: servername is really the dsn, so we need to use it to look up
	// the host, port, socket and default user/password in odbc.ini
	// Use SQLGetPrivateProfileString?
	/*SQLCHAR	buf[100];
	SQLGetPrivateProfileString((LPCSTR)servername,"driver","",(LPSTR)buf,100,"odbc.ini");
	printf("buf=%s\n",buf);*/
	conn->con=new sqlrconnection("localhost",8006,"",
					"mysqltest",
					"mysqltest",
					0,1);
conn->con->debugOn();

	return SQL_SUCCESS;
}

#if (ODBCVER >= 0x0300)
SQLRETURN SQL_API SQLCopyDesc(SQLHDESC SourceDescHandle,
					SQLHDESC TargetDescHandle) {
	debugFunction();

	// FIXME: implement this...
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

	ENV	*env=(ENV *)environmenthandle;
	if (environmenthandle==SQL_NULL_HENV || !env) {
		debugPrintf("NULL env handle\n");
		return SQL_INVALID_HANDLE;
	}

	// not supported by SQL Relay
	return SQL_ERROR;
}

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
	debugPrintf("columnnumber: %d\n",columnnumber);

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	if (columnnumber<1 || columnnumber>stmt->cur->colCount()) {
		return SQL_ERROR;
	}

	snprintf((char *)columnname,bufferlength,
			stmt->cur->getColumnName(columnnumber-1));
	*namelength=charstring::length(columnname);
	// FIXME: map SQLR column types to ODBC column types
	*datatype=SQL_CHAR;
	/*const char	*ctype=stmt->cur->getColumnType(columnnumber-1);
	if (!charstring::compare(ctype,"")) {
		// SQL type: BIGINT
		*datatype=SQL_BIGINT
	} else if (!charstring::compare(ctype,"")) {
		// SQL type: BINARY
		*datatype=SQL_BINARY
	} else if (!charstring::compare(ctype,"")) {
		// SQL type: BLOB
		*datatype=SQL_BLOB
	} else if (!charstring::compare(ctype,"")) {
		// SQL type: BLOB LOCATOR
		*datatype=SQL_BLOB_LOCATOR
	} else if (!charstring::compare(ctype,"")) {
		// SQL type: CHAR
		*datatype=SQL_CHAR
	} else if (!charstring::compare(ctype,"")) {
		// SQL type: CHAR FOR BIT DATA
		*datatype=SQL_BINARY
	} else if (!charstring::compare(ctype,"")) {
		// SQL type: CLOB
		*datatype=SQL_CLOB
	} else if (!charstring::compare(ctype,"")) {
		// SQL type: CLOB LOCATOR
		*datatype=SQL_CLOB_LOCATOR
	} else if (!charstring::compare(ctype,"")) {
		// SQL type: DATE
		*datatype=SQL_TYPE_DATE
	} else if (!charstring::compare(ctype,"")) {
		// SQL type: DBCLOB
		*datatype=SQL_DBCLOB
	} else if (!charstring::compare(ctype,"")) {
		// SQL type: DBCLOB LOCATOR
		*datatype=SQL_DBCLOB_LOCATOR
	} else if (!charstring::compare(ctype,"")) {
		// SQL type: DECFLOAT or DECFLOAT
		*datatype=SQL_DECFLOAT
	} else if (!charstring::compare(ctype,"")) {
		// SQL type: DECIMAL
		*datatype=SQL_DECIMAL
	} else if (!charstring::compare(ctype,"")) {
		// SQL type: DOUBLE
		*datatype=SQL_DOUBLE
	} else if (!charstring::compare(ctype,"")) {
		// SQL type: FLOAT
		*datatype=SQL_FLOAT
	} else if (!charstring::compare(ctype,"")) {
		// SQL type: GRAPHIC
		*datatype=SQL_GRAPHIC
	} else if (!charstring::compare(ctype,"")) {
		// SQL type: INTEGER
		*datatype=SQL_INTEGER
	} else if (!charstring::compare(ctype,"")) {
		// SQL type: LONG VARCHAR
		*datatype=SQL_LONGVARCHAR
	} else if (!charstring::compare(ctype,"")) {
		// SQL type: LONG VARCHAR FOR BIT DATA
		*datatype=SQL_LONGVARBINARY
	} else if (!charstring::compare(ctype,"")) {
		// SQL type: LONG VARGRAPHIC
		*datatype=SQL_LONGVARGRAPHIC
	} else if (!charstring::compare(ctype,"")) {
		// SQL type: NUMERIC
		*datatype=SQL_NUMERIC
	} else if (!charstring::compare(ctype,"")) {
		// SQL type: REAL
		*datatype=SQL_REAL
	} else if (!charstring::compare(ctype,"")) {
		// SQL type: ROWID
		*datatype=SQL_ROWID
	} else if (!charstring::compare(ctype,"")) {
		// SQL type: SMALLINT
		*datatype=SQL_SMALLINT
	} else if (!charstring::compare(ctype,"")) {
		// SQL type: TIME
		*datatype=SQL_TYPE_TIME
	} else if (!charstring::compare(ctype,"")) {
		// SQL type: TIMESTAMP
		*datatype=SQL_TYPE_TIMESTAMP
	} else if (!charstring::compare(ctype,"")) {
		// SQL type: VARBINARY
		*datatype=SQL_VARBINARY
	} else if (!charstring::compare(ctype,"")) {
		// SQL type: VARCHAR
		*datatype=SQL_VARCHAR
	} else if (!charstring::compare(ctype,"")) {
		// SQL type: VARCHAR FOR BIT DATA
		*datatype=SQL_VARBINARY
	} else if (!charstring::compare(ctype,"")) {
		// SQL type: VARGRAPHIC
		*datatype=SQL_VARGRAPHIC
	} else if (!charstring::compare(ctype,"")) {
		// SQL type: XML
		*datatype=SQL_XML
	}*/
	*columnsize=(SQLSMALLINT)
			stmt->cur->getColumnPrecision(columnnumber-1);
	*decimaldigits=(SQLSMALLINT)
			stmt->cur->getColumnScale(columnnumber-1);
	*nullable=(SQLSMALLINT)
			stmt->cur->getColumnIsNullable(columnnumber-1);

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

SQLRETURN SQLR_SQLEndTran(SQLSMALLINT handletype,
					SQLHANDLE handle,
					SQLSMALLINT completiontype) {
	debugFunction();

	switch (handletype) {
		case SQL_HANDLE_ENV:
		{

			ENV	*env=(ENV *)handle;
			if (handle==SQL_NULL_HENV || !env) {
				debugPrintf("NULL env handle\n");
				return SQL_INVALID_HANDLE;
			}

			for (linkedlistnode< CONN * >	*node=
				env->connlist.getNodeByIndex(0); node;
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
	}

	return SQL_ERROR;
}

#if (ODBCVER >= 0x0300)
SQLRETURN SQL_API SQLEndTran(SQLSMALLINT handletype,
					SQLHANDLE handle,
					SQLSMALLINT completiontype) {
	debugFunction();
	return SQLR_SQLEndTran(handletype,handle,completiontype);
}
#endif

SQLRETURN SQL_API SQLError(SQLHENV environmenthandle,
					SQLHDBC connectionhandle,
					SQLHSTMT statementhandle,
					SQLCHAR *sqlstate,
					SQLINTEGER *nativeerror,
					SQLCHAR *messagetext,
					SQLSMALLINT bufferlength,
					SQLSMALLINT *textlength) {
	debugFunction();

	ENV	*env=(ENV *)environmenthandle;
	if (environmenthandle==SQL_NULL_HENV || !env) {
		debugPrintf("NULL env handle\n");
		return SQL_INVALID_HANDLE;
	}

	CONN	*conn=(CONN *)connectionhandle;
	if (connectionhandle==SQL_NULL_HANDLE || !conn || !conn->con) {
		debugPrintf("NULL conn handle\n");
		return SQL_INVALID_HANDLE;
	}

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	// FIXME: implement this

	return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLExecDirect(SQLHSTMT statementhandle,
					SQLCHAR *statementtext,
					SQLINTEGER textlength) {
	debugFunction();
	debugPrintf("statement: \"%s\"\n",statementtext);
	debugPrintf("length: %d\n",textlength);

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

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

	// FIXME: push error onto list

	return SQL_ERROR;
}

SQLRETURN SQL_API SQLExecute(SQLHSTMT statementhandle) {
	debugFunction();

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	stmt->currentrow=0;

	if (stmt->cur->executeQuery()) {
		return SQL_SUCCESS;
	}

	// FIXME: push error onto list

	return SQL_ERROR;
}

static SQLRETURN SQLR_SQLGetData(SQLHSTMT statementhandle,
					SQLUSMALLINT columnnumber,
					SQLSMALLINT targettype,
					SQLPOINTER targetvalue,
					SQLLEN bufferlength,
					SQLLEN *strlen_or_ind) {
	debugFunction();
	debugPrintf("columnnumber: %d\n",columnnumber);
	debugPrintf("targettype: %d\n",targettype);

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	if (columnnumber<1 || columnnumber>stmt->cur->colCount()) {
		return SQL_ERROR;
	}

	const char	*field=stmt->cur->getField(stmt->currentrow-1,
							columnnumber-1);

	*strlen_or_ind=SQL_NULL_DATA;

	if (field) {

		// FIXME: implement for non-char
		switch (targettype) {
			case SQL_C_CHAR:
				debugPrintf("SQL_C_CHAR\n");
				// SQLCHAR *
				// unsigned char *
				snprintf((char *)targetvalue,
						bufferlength,field);
				*strlen_or_ind=(SQLLEN)stmt->cur->
						getFieldLength(
							stmt->currentrow-1,
							columnnumber-1);
				break;
			case SQL_C_SSHORT:
				debugPrintf("SQL_C_SSHORT\n");
				// SQLSMALLINT
				// short int
				break;
			case SQL_C_USHORT:
				debugPrintf("SQL_C_USHORT\n");
				// SQLUSMALLINT
				// unsigned short int
				break;
			case SQL_C_SLONG:
				debugPrintf("SQL_C_SLONG\n");
				// SQLINTEGER
				// long int
				break;
			case SQL_C_ULONG:
				debugPrintf("SQL_C_ULONG\n");
				// SQLUINTEGER
				// unsigned long int
				break;
			case SQL_C_FLOAT:
				debugPrintf("SQL_C_FLOAT\n");
				// SQLREAL
				// float
				break;
			case SQL_C_DOUBLE:
				debugPrintf("SQL_C_DOUBLE\n");
				// SQLDOUBLE, SQLFLOAT
				// double
				break;
			case SQL_C_BIT:
				debugPrintf("SQL_C_BIT\n");
				// SQLCHAR
				// unsigned char
				break;
			case SQL_C_STINYINT:
				debugPrintf("SQL_C_STINYINT\n");
				// SQLSCHAR
				// signed char
				break;
			case SQL_C_UTINYINT:
				debugPrintf("SQL_C_UTINYINT\n");
				// SQLCHAR
				// unsigned char
				break;
			case SQL_C_SBIGINT:
				debugPrintf("SQL_C_SBIGINT\n");
				// SQLBIGINT
				// _int64
				break;
			case SQL_C_UBIGINT:
				debugPrintf("SQL_C_UBIGINT\n");
				// SQLUBIGINT
				// unsigned _int64
				break;
			case SQL_C_BINARY:
				debugPrintf("SQL_C_BINARY\n");
				// SQLCHAR *
				// unsigned char *
				break;
			// apparently a dup of SQL_C_ULONG
			//case SQL_C_BOOKMARK:
				//debugPrintf("SQL_C_BOOKMARK\n");
				// BOOKMARK
				// unsigned long int
				//break;
			// apparentley a dup of SQL_C_BINARY
			//case SQL_C_VARBOOKMARK:
				//debugPrintf("SQL_C_VARBOOKMARK\n");
				// SQLCHAR *
				// unsigned char *
				break;
			case SQL_C_TYPE_DATE:
				debugPrintf("SQL_C_TYPE_DATE\n");
				// SQL_DATE_STRUCT
				/*
				struct tagDATE_STRUCT {
 					SQLSMALLINT year;
 					SQLUSMALLINT month;
 					SQLUSMALLINT day;
				} DATE_STRUCT;
				*/
				break;
			case SQL_C_TYPE_TIME:
				debugPrintf("SQL_C_TYPE_TIME\n");
				// SQL_TIME_STRUCT
				/*
				struct tagTIME_STRUCT {
 					SQLUSMALLINT hour;
 					SQLUSMALLINT minute;
 					SQLUSMALLINT second;
				} TIME_STRUCT;
				*/
				break;
			case SQL_C_TYPE_TIMESTAMP:
				debugPrintf("SQL_C_TYPE_TIMESTAMP\n");
				// SQL_TIMESTAMP_STRUCT
				/*
				struct tagTIMESTAMP_STRUCT {
 					SQLSMALLINT year;
 					SQLUSMALLINT month;
 					SQLUSMALLINT day;
 					SQLUSMALLINT hour;
 					SQLUSMALLINT minute;
 					SQLUSMALLINT second;
 					SQLUINTEGER fraction;
				} TIMESTAMP_STRUCT;
				*/
				break;
			case SQL_C_NUMERIC:
				debugPrintf("SQL_C_NUMERIC\n");
 				// SQL_NUMERIC_STRUCT
				/*
				struct tagSQL_NUMERIC_STRUCT {
 					SQLCHAR precision;
 					SQLSCHAR scale;
 					SQLCHAR sign;
 					SQLCHAR val[SQL_MAX_NUMERIC_LEN];
				} SQL_NUMERIC_STRUCT;
				*/
				break;
			case SQL_C_GUID:
				debugPrintf("SQL_C_GUID\n");
				// SQLGUID
				/* struct tagSQLGUID {
					DWORD Data1;
					WORD Data2;
					WORD Data3;
					BYTE Data4[8];
				} SQLGUID;
				*/
				break;
			//case  "C-interval-types????"
				// debugPrintf("SQL_C_GUID\n");
				// SQL_INTERVAL_STRUCT
				/*
 				typedef struct tagSQL_INTERVAL_STRUCT {
					SQLINTERVAL interval_type; 
					SQLSMALLINT interval_sign;
					union {
						SQL_YEAR_MONTH_STRUCT
								year_month;
						SQL_DAY_SECOND_STRUCT
								day_second;
					} intval;
				} SQL_INTERVAL_STRUCT;

				typedef enum {
					SQL_IS_YEAR = 1,
					SQL_IS_MONTH = 2,
					SQL_IS_DAY = 3,
					SQL_IS_HOUR = 4,
					SQL_IS_MINUTE = 5,
					SQL_IS_SECOND = 6,
					SQL_IS_YEAR_TO_MONTH = 7,
					SQL_IS_DAY_TO_HOUR = 8,
					SQL_IS_DAY_TO_MINUTE = 9,
					SQL_IS_DAY_TO_SECOND = 10,
					SQL_IS_HOUR_TO_MINUTE = 11,
					SQL_IS_HOUR_TO_SECOND = 12,
					SQL_IS_MINUTE_TO_SECOND = 13
				} SQLINTERVAL;
				
				typedef struct tagSQL_YEAR_MONTH {
					SQLUINTEGER year;
					SQLUINTEGER month; 
				} SQL_YEAR_MONTH_STRUCT;
				
				typedef struct tagSQL_DAY_SECOND {
					SQLUINTEGER day;
					SQLUINTEGER hour;
					SQLUINTEGER minute;
					SQLUINTEGER second;
					SQLUINTEGER fraction;
				} SQL_DAY_SECOND_STRUCT;
				*/
		}
	}
	return SQL_SUCCESS;
}

SQLRETURN SQLR_PopulateBindColumns(SQLHSTMT statementhandle) {
	debugFunction();

	STMT	*stmt=(STMT *)statementhandle;

	for (uint32_t index=0; index<stmt->cur->colCount(); index++) {

		FIELD	*field=NULL;
		stmt->fieldlist.getData(index,&field);

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

SQLRETURN SQLR_SQLFetch(SQLHSTMT statementhandle) {
	debugFunction();

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	if (stmt->cur->getRow(stmt->currentrow)) {
		stmt->currentrow++;
		return SQLR_PopulateBindColumns(statementhandle);
	}
	return SQL_NO_DATA_FOUND;
}


SQLRETURN SQL_API SQLFetch(SQLHSTMT statementhandle) {
	debugFunction();
	return SQLR_SQLFetch(statementhandle);
}

SQLRETURN SQLR_SQLFetchScroll(SQLHSTMT statementhandle,
					SQLSMALLINT fetchorientation,
					SQLROWOFFSET fetchoffset) {
	debugFunction();

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	// FIXME: implement the rest of these,
	// update the row status array
	switch (fetchorientation) {
		case SQL_FETCH_NEXT:
			return SQLR_SQLFetch(statementhandle);
		case SQL_FETCH_PRIOR:
			break;
		case SQL_FETCH_FIRST:
			break;
		case SQL_FETCH_LAST:
			break;
		case SQL_FETCH_ABSOLUTE:
			break;
		case SQL_FETCH_RELATIVE:
			break;
		case SQL_FETCH_BOOKMARK:
			break;
	}

	return SQL_ERROR;
}

#if (ODBCVER >= 0x0300)
SQLRETURN SQL_API SQLFetchScroll(SQLHSTMT statementhandle,
					SQLSMALLINT fetchorientation,
					SQLROWOFFSET fetchoffset) {
	debugFunction();
	return SQLR_SQLFetchScroll(statementhandle,
					fetchorientation,
					fetchoffset);
}
#endif

static SQLRETURN SQLR_SQLFreeConnect(SQLHDBC connectionhandle) {
	debugFunction();

	CONN	*conn=(CONN *)connectionhandle;
	if (connectionhandle==SQL_NULL_HANDLE || !conn || !conn->con) {
		debugPrintf("NULL conn handle\n");
		return SQL_INVALID_HANDLE;
	}

	conn->env->connlist.removeAllByData(conn);
	conn->stmtlist.clear();
	delete conn->con;
	delete conn;

	return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLFreeConnect(SQLHDBC connectionhandle) {
	debugFunction();
	return SQLR_SQLFreeConnect(connectionhandle);
}

static SQLRETURN SQLR_SQLFreeEnv(SQLHENV environmenthandle) {
	debugFunction();

	ENV	*env=(ENV *)environmenthandle;
	if (environmenthandle==SQL_NULL_HENV || !env) {
		debugPrintf("NULL env handle\n");
		return SQL_INVALID_HANDLE;
	}

	env->connlist.clear();
	delete env;

	return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLFreeEnv(SQLHENV environmenthandle) {
	debugFunction();
	return SQLR_SQLFreeEnv(environmenthandle);
}

static SQLRETURN SQLR_SQLFreeStmt(SQLHSTMT statementhandle,
					SQLUSMALLINT option) {
	debugFunction();

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	switch (option) {
		case SQL_CLOSE:
			debugPrintf("SQL_CLOSE\n");
			return SQLR_SQLCloseCursor(statementhandle);
		case SQL_DROP:
			debugPrintf("SQL_DROP\n");
			stmt->conn->stmtlist.removeAllByData(stmt);
			delete stmt->cur;
			delete stmt;
			break;
		case SQL_UNBIND:
			debugPrintf("SQL_UNBIND\n");
			stmt->fieldlist.clear();
			break;
		case SQL_RESET_PARAMS:
			debugPrintf("SQL_RESET_PARAMS\n");
			stmt->cur->clearBinds();
			break;
	}

	return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLFreeStmt(SQLHSTMT statementhandle,
					SQLUSMALLINT option) {
	debugFunction();
	return SQLR_SQLFreeStmt(statementhandle,option);
}

#if (ODBCVER >= 0x0300)
SQLRETURN SQL_API SQLFreeHandle(SQLSMALLINT handletype,
					SQLHANDLE handle) {
	debugFunction();
	debugPrintf("handletype: %d\n",handletype);

	switch (handletype) {
		case SQL_HANDLE_ENV:
			return SQLR_SQLFreeEnv((SQLHENV)handle);
		case SQL_HANDLE_DBC:
			return SQLR_SQLFreeConnect((SQLHDBC)handle);
		case SQL_HANDLE_STMT:
			return SQLR_SQLFreeStmt((SQLHSTMT)handle,SQL_DROP);
		// SQL_HANDLE_DESC not supported by sqlrelay
	}
	return SQL_ERROR;
}
#endif

SQLRETURN SQLR_SQLGetConnectAttr(SQLHDBC connectionhandle,
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

	// not supported by SQL Relay
	// autocommit is settable, but not gettable
	/*
	SQL_ACCESS_MODE:
	SQL_AUTOCOMMIT:
	SQL_LOGIN_TIMEOUT:
	SQL_OPT_TRACE:
	SQL_OPT_TRACEFILE:
	SQL_TRANSLATE_DLL:
	SQL_TRANSLATE_OPTION:
	SQL_TXN_ISOLATION:
	SQL_CURRENT_QUALIFIER:
	SQL_ODBC_CURSORS:
	SQL_QUIET_MODE:
	SQL_PACKET_SIZE:
#if (ODBCVER >= 0x0300)
	SQL_ATTR_CONNECTION_TIMEOUT:
	SQL_ATTR_DISCONNECT_BEHAVIOR:
	SQL_ATTR_ENLIST_IN_DTC:
	SQL_ATTR_ENLIST_IN_XA:
	SQL_ATTR_AUTO_IPD:
	SQL_ATTR_METADATA_ID:
#endif
	*/

	return SQL_ERROR;
}

#if (ODBCVER >= 0x0300)
SQLRETURN SQL_API SQLGetConnectAttr(SQLHDBC connectionhandle,
					SQLINTEGER attribute,
					SQLPOINTER value,
					SQLINTEGER bufferlength,
					SQLINTEGER *stringlength) {
	debugFunction();
	return SQLR_SQLGetConnectAttr(connectionhandle,
					attribute,
					value,
					bufferlength,
					stringlength);
}
#endif

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
		sprintf(stmt->name,"%d",stmtid);
		stmtid++;
	}
	if (cursorname) {
		snprintf((char *)cursorname,bufferlength,stmt->name);
	}
	if (namelength) {
		*namelength=charstring::length(stmt->name);
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
	return SQLR_SQLGetData(statementhandle,columnnumber,targettype,
				targetvalue,bufferlength,strlen_or_ind);
}

#if (ODBCVER >= 0x0300)
SQLRETURN SQLGetDescField(SQLHDESC DescriptorHandle,
					SQLSMALLINT RecNumber,
					SQLSMALLINT FieldIdentifier,
					SQLPOINTER Value,
					SQLINTEGER BufferLength,
					SQLINTEGER *StringLength) {
	debugFunction();

	// not supported by SQL Relay

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

	// not supported by SQL Relay

	return SQL_ERROR;
}

SQLRETURN SQLR_SQLGetDiagFieldEnv(SQLHANDLE handle,
					SQLSMALLINT recnumber,
					SQLSMALLINT diagidentifier,
					SQLPOINTER diaginfo,
					SQLSMALLINT bufferlength,
					SQLSMALLINT *stringlength) {
	debugFunction();

	ENV	*env=(ENV *)handle;
	if (handle==SQL_NULL_HENV || !env) {
		debugPrintf("NULL env handle\n");
		return SQL_INVALID_HANDLE;
	}

	// FIXME: implement this

	return SQL_ERROR;
}

SQLRETURN SQLR_SQLGetDiagFieldConnect(SQLHANDLE handle,
					SQLSMALLINT recnumber,
					SQLSMALLINT diagidentifier,
					SQLPOINTER diaginfo,
					SQLSMALLINT bufferlength,
					SQLSMALLINT *stringlength) {
	debugFunction();

	CONN	*conn=(CONN *)handle;
	if (handle==SQL_NULL_HANDLE || !conn || !conn->con) {
		debugPrintf("NULL conn handle\n");
		return SQL_INVALID_HANDLE;
	}

	// FIXME: implement this

	return SQL_ERROR;
}

SQLRETURN SQLR_SQLGetDiagFieldStmt(SQLHANDLE handle,
					SQLSMALLINT recnumber,
					SQLSMALLINT diagidentifier,
					SQLPOINTER diaginfo,
					SQLSMALLINT bufferlength,
					SQLSMALLINT *stringlength) {
	debugFunction();

	STMT	*stmt=(STMT *)handle;
	if (handle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	// FIXME: implement this

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

	switch (handletype) {
		case SQL_HANDLE_ENV:
			return SQLR_SQLGetDiagFieldEnv(handle,
							recnumber,
							diagidentifier,
							diaginfo,
							bufferlength,
							stringlength);
		case SQL_HANDLE_DBC:
			return SQLR_SQLGetDiagFieldConnect(handle,
							recnumber,
							diagidentifier,
							diaginfo,
							bufferlength,
							stringlength);
		case SQL_HANDLE_STMT:
			return SQLR_SQLGetDiagFieldStmt(handle,
							recnumber,
							diagidentifier,
							diaginfo,
							bufferlength,
							stringlength);
		// SQL_HANDLE_DESC not supported by sqlrelay
	}

	return SQL_ERROR;
}

SQLRETURN SQLR_SQLGetDiagRecEnv(SQLHANDLE handle,
					SQLSMALLINT recnumber,
					SQLCHAR *sqlstate,
					SQLINTEGER *nativeerror,
					SQLCHAR *messagetext,
					SQLSMALLINT bufferlength,
					SQLSMALLINT *textlength) {
	debugFunction();

	ENV	*env=(ENV *)handle;
	if (handle==SQL_NULL_HENV || !env) {
		debugPrintf("NULL env handle\n");
		return SQL_INVALID_HANDLE;
	}

	// FIXME: implement this

	return SQL_ERROR;
}

SQLRETURN SQLR_SQLGetDiagRecConnect(SQLHANDLE handle,
					SQLSMALLINT recnumber,
					SQLCHAR *sqlstate,
					SQLINTEGER *nativeerror,
					SQLCHAR *messagetext,
					SQLSMALLINT bufferlength,
					SQLSMALLINT *textlength) {
	debugFunction();

	CONN	*conn=(CONN *)handle;
	if (handle==SQL_NULL_HANDLE || !conn || !conn->con) {
		debugPrintf("NULL conn handle\n");
		return SQL_INVALID_HANDLE;
	}

	// FIXME: implement this

	return SQL_ERROR;
}

SQLRETURN SQLR_SQLGetDiagRecStmt(SQLHANDLE handle,
					SQLSMALLINT recnumber,
					SQLCHAR *sqlstate,
					SQLINTEGER *nativeerror,
					SQLCHAR *messagetext,
					SQLSMALLINT bufferlength,
					SQLSMALLINT *textlength) {
	debugFunction();

	STMT	*stmt=(STMT *)handle;
	if (handle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	// FIXME: implement this

	return SQL_ERROR;
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

	switch (handletype) {
		case SQL_HANDLE_ENV:
			return SQLR_SQLGetDiagRecEnv(handle,
							recnumber,
							sqlstate,
							nativeerror,
							messagetext,
							bufferlength,
							textlength);
		case SQL_HANDLE_DBC:
			return SQLR_SQLGetDiagRecConnect(handle,
							recnumber,
							sqlstate,
							nativeerror,
							messagetext,
							bufferlength,
							textlength);
		case SQL_HANDLE_STMT:
			return SQLR_SQLGetDiagRecStmt(handle,
							recnumber,
							sqlstate,
							nativeerror,
							messagetext,
							bufferlength,
							textlength);
		// SQL_HANDLE_DESC not supported by sqlrelay
	}

	return SQL_ERROR;
}

SQLRETURN SQL_API SQLGetEnvAttr(SQLHENV environmenthandle,
					SQLINTEGER attribute,
					SQLPOINTER value,
					SQLINTEGER bufferlength,
					SQLINTEGER *stringlength) {
	debugFunction();
	debugPrintf("attribute: %d\n",attribute);

	ENV	*env=(ENV *)environmenthandle;
	if (environmenthandle==SQL_NULL_HENV || !env) {
		debugPrintf("NULL env handle\n");
		return SQL_INVALID_HANDLE;
	}

	switch (attribute) {
		case SQL_ATTR_OUTPUT_NTS:
			// this one is hardcoded to true
			// and can't be set to false
			*((SQLINTEGER *)value)=SQL_TRUE;
			break;
		case SQL_ATTR_ODBC_VERSION:
			if (value) {
				*((SQLINTEGER *)value)=env->odbcversion;
			}
			if (stringlength) {
				*stringlength=sizeof(SQLINTEGER);
			}
			return SQL_SUCCESS;
		case SQL_ATTR_CONNECTION_POOLING:
			// this one is hardcoded to "off"
			// and can't be changed
			*((SQLUINTEGER *)value)=SQL_CP_OFF;
			break;
		case SQL_ATTR_CP_MATCH:
			// this one is hardcoded to "default"
			// and can't be changed
			*((SQLUINTEGER *)value)=SQL_CP_MATCH_DEFAULT;
			break;
	}
	return SQL_ERROR;
}
#endif /* ODBCVER >= 0x0300 */

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
		case SQL_API_SQLALLOCENV:
		#if (ODBCVER >= 0x0300)
		case SQL_API_SQLALLOCHANDLE:
		#endif
		case SQL_API_SQLALLOCSTMT:
		case SQL_API_SQLBINDCOL:
		#if (ODBCVER >= 0x0300)
		case SQL_API_SQLBINDPARAM:
		#endif
		#if (ODBCVER >= 0x0300)
		case SQL_API_SQLCLOSECURSOR:
		case SQL_API_SQLCOLATTRIBUTE:
		#endif
		case SQL_API_SQLCONNECT:
		#if (ODBCVER >= 0x0300)
		case SQL_API_SQLCOPYDESC:
		#endif
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
		case SQL_API_SQLGETCONNECTOPTION:
		case SQL_API_SQLGETCURSORNAME:
		case SQL_API_SQLGETDATA:
		#if (ODBCVER >= 0x0300)
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
		case SQL_API_SQLPREPARE:
		case SQL_API_SQLROWCOUNT:
		#if (ODBCVER >= 0x0300)
		case SQL_API_SQLSETCONNECTATTR:
		#endif
		case SQL_API_SQLSETCONNECTOPTION:
		case SQL_API_SQLSETCURSORNAME:
		#if (ODBCVER >= 0x0300)
		case SQL_API_SQLSETENVATTR:
		#endif
		case SQL_API_SQLSETPARAM:
		#if (ODBCVER >= 0x0300)
		case SQL_API_SQLSETSTMTATTR:
		#endif
		case SQL_API_SQLSETSTMTOPTION:
		case SQL_API_SQLTRANSACT:
		#if (ODBCVER >= 0x0300)
		case SQL_API_SQLALLOCHANDLESTD:
		#endif /* ODBCVER >= 0x0300 */
		case SQL_API_SQLBINDPARAMETER:
		case SQL_API_SQLBROWSECONNECT:
		// dupe of SQL_API_SQLCOLATTRIBUTE
		//case SQL_API_SQLCOLATTRIBUTES:
		case SQL_API_SQLDRIVERCONNECT:
		case SQL_API_SQLEXTENDEDFETCH:
		case SQL_API_SQLMORERESULTS:
		case SQL_API_SQLNUMPARAMS:
		case SQL_API_SQLPARAMOPTIONS:
		case SQL_API_SQLSETPOS:
		case SQL_API_SQLSETSCROLLOPTIONS:
			*supported=SQL_TRUE;
			break;
		case SQL_API_SQLCANCEL:
		case SQL_API_SQLCOLUMNS:
		case SQL_API_SQLDATASOURCES:
		case SQL_API_SQLPUTDATA:
		case SQL_API_SQLPARAMDATA:
		#if (ODBCVER >= 0x0300)
		case SQL_API_SQLGETCONNECTATTR:
		#endif
		case SQL_API_SQLSPECIALCOLUMNS:
		case SQL_API_SQLSTATISTICS:
		case SQL_API_SQLTABLES:
		#if (ODBCVER >= 0x0300)
		case SQL_API_SQLBULKOPERATIONS:
		#endif
		case SQL_API_SQLCOLUMNPRIVILEGES:
		case SQL_API_SQLDESCRIBEPARAM:
		case SQL_API_SQLFOREIGNKEYS:
		case SQL_API_SQLNATIVESQL:
		case SQL_API_SQLPRIMARYKEYS:
		case SQL_API_SQLPROCEDURECOLUMNS:
		case SQL_API_SQLPROCEDURES:
		case SQL_API_SQLTABLEPRIVILEGES:
		case SQL_API_SQLDRIVERS:
		#if (ODBCVER >= 0x0300)
		case SQL_API_SQLGETDESCFIELD:
		case SQL_API_SQLGETDESCREC:
		case SQL_API_SQLSETDESCFIELD:
		case SQL_API_SQLSETDESCREC:
		#endif
			*supported=SQL_FALSE;
			break;
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

	CONN	*conn=(CONN *)connectionhandle;
	if (connectionhandle==SQL_NULL_HANDLE || !conn || !conn->con) {
		debugPrintf("NULL conn handle\n");
		return SQL_INVALID_HANDLE;
	}

	uint16_t	outsize=0;

	char	*strval="";
	// FIXME: there are tons more of these...
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
	debugPrintf("bufferlength: %d\n",bufferlength);

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	// FIXME: implement this for real

	uint16_t	outsize=0;

	char		*strval="";
	uint16_t	val16=0xDEAD;
	uint32_t	val32=0xDEADBEEF;
	uint64_t	val64=0xDEADBEEFDEADBEEF;
	switch (attribute) {
		#if (ODBCVER >= 0x0300)
		case SQL_ATTR_APP_ROW_DESC:
			break;
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
		#endif
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
		// dupe of SQL_ASYNC_ENABLE
		//case SQL_ATTR_ASYNC_ENABLE:
		// dupe of case SQL_CURSOR_TYPE
		//case SQL_ATTR_CONCURRENCY:
		// dupe of SQL_CURSOR_TYPE
		//case SQL_ATTR_CURSOR_TYPE:
		case SQL_ATTR_ENABLE_AUTO_IPD:
			break;
		case SQL_ATTR_FETCH_BOOKMARK_PTR:
			break;
		// dupe of SQL_KEYSET_SIZE
		//case SQL_ATTR_KEYSET_SIZE:
		// dupe of SQL_MAX_LENGTH
		//case SQL_ATTR_MAX_LENGTH:
		// dupe of SQL_MAX_ROWS
		//case SQL_ATTR_MAX_ROWS:
		// dupe of SQL_NOSCAN
		//case SQL_ATTR_NOSCAN:
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
		// dupe of SQL_QUERY_TIMEOUT
		//case SQL_ATTR_QUERY_TIMEOUT:
		// dupe of SQL_RETRIEVE_DATA
		//case SQL_ATTR_RETRIEVE_DATA:
		case SQL_ATTR_ROW_BIND_OFFSET_PTR:
			break;
		// dupe of SQL_BIND_TYPE
		// case SQL_ATTR_ROW_BIND_TYPE:
		// dupe of SQL_ROW_NUMBER
		//case SQL_ATTR_ROW_NUMBER:
		case SQL_ATTR_ROW_OPERATION_PTR:
			break;
		case SQL_ATTR_ROW_STATUS_PTR:
			break;
		case SQL_ATTR_ROWS_FETCHED_PTR:
			break;
		case SQL_ATTR_ROW_ARRAY_SIZE:
			break;
		// dupe of SQL_SIMULATE_CURSOR
		//case SQL_ATTR_SIMULATE_CURSOR:
		// dupe of SQL_USE_BOOKMARKS
		//case SQL_ATTR_USE_BOOKMARKS:
		#endif
		#if (ODBCVER < 0x0300)
		case SQL_STMT_OPT_MAX:
			break;
		case SQL_STMT_OPT_MIN:
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

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	// FIXME: implement this

	return SQL_ERROR;
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

	// not supported by SQL Relay

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

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	// not supported by SQL Relay

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

	*rowcount=(SQLSMALLINT)stmt->cur->affectedRows();

	return SQL_SUCCESS;
}

SQLRETURN SQLR_SQLSetConnectAttr(SQLHDBC connectionhandle,
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
		if ((uint64_t)value==(uint64_t)SQL_AUTOCOMMIT_ON) {
			if (conn->con->autoCommitOn()) {
				return SQL_SUCCESS;
			}
		} else if ((uint64_t)value==(uint64_t)SQL_AUTOCOMMIT_OFF) {
			if (conn->con->autoCommitOff()) {
				return SQL_SUCCESS;
			}
		}
	}

	// Other attributes, not supported by SQL Relay
	/*
 	SQL_ACCESS_MODE
	SQL_LOGIN_TIMEOUT
	SQL_OPT_TRACE
	SQL_OPT_TRACEFILE
	SQL_TRANSLATE_DLL
	SQL_TRANSLATE_OPTION
	SQL_TXN_ISOLATION
	SQL_CURRENT_QUALIFIER
	SQL_ODBC_CURSORS
	SQL_QUIET_MODE
	SQL_PACKET_SIZE
#if (ODBCVER >= 0x0300)
	SQL_ATTR_CONNECTION_TIMEOUT
	SQL_ATTR_DISCONNECT_BEHAVIOR
	SQL_ATTR_ENLIST_IN_DTC
	SQL_ATTR_ENLIST_IN_XA
	SQL_ATTR_AUTO_IPD
	SQL_ATTR_METADATA_ID
#endif
	*/

	return SQL_ERROR;
}

#if (ODBCVER >= 0x0300)
SQLRETURN SQL_API SQLSetConnectAttr(SQLHDBC connectionhandle,
					SQLINTEGER attribute,
					SQLPOINTER value,
					SQLINTEGER stringlength) {
	debugFunction();
	return SQLR_SQLSetConnectAttr(connectionhandle,attribute,
						value,stringlength);
}
#endif /* ODBCVER >= 0x0300 */

SQLRETURN SQL_API SQLSetConnectOption(SQLHDBC connectionhandle,
					SQLUSMALLINT option,
					SQLULEN value) {
	debugFunction();
	// FIXME: map values and call SQLR_SetConnectAttr
	return SQL_ERROR;
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
	stmt->name=charstring::duplicate((const char *)cursorname,namelength);

	return SQL_SUCCESS;
}

#if (ODBCVER >= 0x0300)
SQLRETURN SQL_API SQLSetDescField(SQLHDESC DescriptorHandle,
					SQLSMALLINT RecNumber,
					SQLSMALLINT FieldIdentifier,
					SQLPOINTER Value,
					SQLINTEGER BufferLength) {
	debugFunction();

	// not supported by sqlrelay

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

	// not supported by sqlrelay

	return SQL_ERROR;
}

SQLRETURN SQL_API SQLSetEnvAttr(SQLHENV environmenthandle,
					SQLINTEGER attribute,
					SQLPOINTER value,
					SQLINTEGER stringlength) {
	debugFunction();
	debugPrintf("attribute: %d\n",attribute);

	ENV	*env=(ENV *)environmenthandle;
	if (environmenthandle==SQL_NULL_HENV || !env) {
		debugPrintf("NULL env handle\n");
		return SQL_INVALID_HANDLE;
	}

	switch (attribute) {
		case SQL_ATTR_OUTPUT_NTS:
			// this can't be set to false
			if ((uint64_t)value==SQL_TRUE) {
				return SQL_SUCCESS;
			} else {
				return SQL_ERROR;
			}
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
			// this can't be set on
			if ((uint64_t)value==SQL_CP_OFF) {
				return SQL_SUCCESS;
			} else {
				return SQL_ERROR;
			}
		case SQL_ATTR_CP_MATCH:
			// this can't be set to anything but default
			if ((uint64_t)value==SQL_CP_MATCH_DEFAULT) {
				return SQL_SUCCESS;
			} else {
				return SQL_ERROR;
			}
	}
	return SQL_ERROR;
}
#endif

SQLRETURN SQLR_SQLSetStmtAttr(SQLHSTMT statementhandle,
					SQLINTEGER attribute,
					SQLPOINTER value,
					SQLINTEGER stringlength) {
	debugFunction();

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	// FIXME: implement this
	switch (attribute) {
		#if (ODBCVER >= 0x0300)
		case SQL_ATTR_APP_ROW_DESC:
			break;
		case SQL_ATTR_APP_PARAM_DESC:
			break;
		case SQL_ATTR_IMP_ROW_DESC:
			break;
		case SQL_ATTR_IMP_PARAM_DESC:
			break;
		case SQL_ATTR_CURSOR_SCROLLABLE:
			break;
		case SQL_ATTR_CURSOR_SENSITIVITY:
			break;
		#endif
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
		// dupe of SQL_ASYNC_ENABLE
		//case SQL_ATTR_ASYNC_ENABLE:
		// dupe of case SQL_CURSOR_TYPE
		//case SQL_ATTR_CONCURRENCY:
		// dupe of SQL_CURSOR_TYPE
		//case SQL_ATTR_CURSOR_TYPE:
		case SQL_ATTR_ENABLE_AUTO_IPD:
			break;
		case SQL_ATTR_FETCH_BOOKMARK_PTR:
			break;
		// dupe of SQL_KEYSET_SIZE
		//case SQL_ATTR_KEYSET_SIZE:
		// dupe of SQL_MAX_LENGTH
		//case SQL_ATTR_MAX_LENGTH:
		// dupe of SQL_MAX_ROWS
		//case SQL_ATTR_MAX_ROWS:
		// dupe of SQL_NOSCAN
		//case SQL_ATTR_NOSCAN:
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
		// dupe of SQL_QUERY_TIMEOUT
		//case SQL_ATTR_QUERY_TIMEOUT:
		// dupe of SQL_RETRIEVE_DATA
		//case SQL_ATTR_RETRIEVE_DATA:
		case SQL_ATTR_ROW_BIND_OFFSET_PTR:
			break;
		// dupe of SQL_BIND_TYPE
		// case SQL_ATTR_ROW_BIND_TYPE:
		// dupe of SQL_ROW_NUMBER
		//case SQL_ATTR_ROW_NUMBER:
		case SQL_ATTR_ROW_OPERATION_PTR:
			break;
		case SQL_ATTR_ROW_STATUS_PTR:
			break;
		case SQL_ATTR_ROWS_FETCHED_PTR:
			break;
		case SQL_ATTR_ROW_ARRAY_SIZE:
			break;
		// dupe of SQL_SIMULATE_CURSOR
		//case SQL_ATTR_SIMULATE_CURSOR:
		// dupe of SQL_USE_BOOKMARKS
		//case SQL_ATTR_USE_BOOKMARKS:
		#endif
		#if (ODBCVER < 0x0300)
		case SQL_STMT_OPT_MAX:
			break;
		case SQL_STMT_OPT_MIN:
			break;
		#endif
	}

	return SQL_SUCCESS;
}

#if (ODBCVER >= 0x0300)
SQLRETURN SQL_API SQLSetStmtAttr(SQLHSTMT statementhandle,
					SQLINTEGER attribute,
					SQLPOINTER value,
					SQLINTEGER stringlength) {
	debugFunction();
	return SQLR_SQLSetStmtAttr(statementhandle,attribute,
						value,stringlength);
}
#endif

SQLRETURN SQL_API SQLSetStmtOption(SQLHSTMT statementhandle,
					SQLUSMALLINT option,
					SQLROWCOUNT value) {
	debugFunction();
	return SQLR_SQLSetStmtAttr(statementhandle,option,(SQLPOINTER)value,0);
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

	// not supported by SQL Relay

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

	// not supported by SQL Relay

	return SQL_ERROR;
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

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	// not supported by SQL Relay

	return SQL_ERROR;
}

SQLRETURN SQL_API SQLTransact(SQLHENV environmenthandle,
					SQLHDBC connectionhandle,
					SQLUSMALLINT completiontype) {
	if (connectionhandle) {
		return SQLR_SQLEndTran(SQL_HANDLE_DBC,
					connectionhandle,
					completiontype);
	} else if (environmenthandle) {
		return SQLR_SQLEndTran(SQL_HANDLE_ENV,
					environmenthandle,
					completiontype);
	}
	return SQL_INVALID_HANDLE;
}

SQLRETURN SQL_API SQLDriverConnect(SQLHDBC hdbc,
					SQLHWND hwnd,
					SQLCHAR *szConnStrIn,
					SQLSMALLINT cbConnStrIn,
					SQLCHAR *szConnStrOut,
					SQLSMALLINT cbConnStrOutMax,
					SQLSMALLINT *pcbConnStrOut,
					SQLUSMALLINT fDriverCompletion) {
	debugFunction();

	// FIXME: implement this

	return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLBrowseConnect(SQLHDBC hdbc,
					SQLCHAR *szConnStrIn,
					SQLSMALLINT cbConnStrIn,
					SQLCHAR *szConnStrOut,
					SQLSMALLINT cbConnStrOutMax,
					SQLSMALLINT *pcbConnStrOut) {
	debugFunction();

	// FIXME: implement this

	return SQL_SUCCESS;
}

#if (ODBCVER >= 0x0300)
SQLRETURN SQL_API SQLBulkOperations(SQLHSTMT statementhandle,
					SQLSMALLINT Operation) {
	debugFunction();

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	// not supported by sqlrelay

	return SQL_ERROR;
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

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

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

	// not supported by sqlrelay

	return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLDescribeParam(SQLHSTMT statementhandle,
					SQLUSMALLINT ipar,
					SQLSMALLINT *pfSqlType,
					SQLULEN *pcbParamDef,
					SQLSMALLINT *pibScale,
					SQLSMALLINT *pfNullable) {
	debugFunction();

	// not supported by sqlrelay

	return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLExtendedFetch(SQLHSTMT statementhandle,
					SQLUSMALLINT ffetchtype,
					SQLROWOFFSET irow,
					SQLROWSETSIZE *pcrow,
					SQLUSMALLINT *rgfrowstatus) {
	debugFunction();
	SQLRETURN	retval=SQLR_SQLFetchScroll(statementhandle,
							ffetchtype,irow);
	// FIXME: set to value of SQL_ATTR_ROWS_FETCHED_PTR statement attr
	*pcrow=0;
	// FIXME: set to array of statuses from SQL_ATTR_ROW_STATUS_PTR
	// statement attr
	*rgfrowstatus=0;
	return retval;
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

	// not supported by sqlrelay

	return SQL_ERROR;
}

SQLRETURN SQL_API SQLMoreResults(SQLHSTMT statementhandle) {
	debugFunction();
	return SQL_NO_DATA_FOUND;
}

SQLRETURN SQL_API SQLNativeSql(SQLHDBC hdbc,
					SQLCHAR *szSqlStrIn,
					SQLINTEGER cbSqlStrIn,
					SQLCHAR *szSqlStr,
					SQLINTEGER cbSqlStrMax,
					SQLINTEGER *pcbSqlStr) {
	debugFunction();

	// not supported by sqlrelay

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

	*pcpar=(SQLSMALLINT)stmt->cur->countBindVariables();

	return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLParamOptions(SQLHSTMT statementhandle,
					SQLULEN crow,
					SQLULEN *pirow) {
	debugFunction();

	// FIXME: map values and call SQLR_SQLSetStmtAttr

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

	// not supported by sqlrelay

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

	// not supported by sqlrelay

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

	// not supported by sqlrelay

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

	// FIXME: implement this

	switch (foption) {
		case SQL_POSITION:
		case SQL_REFRESH:
		case SQL_UPDATE:
		case SQL_DELETE:
		case SQL_ADD:
			break;
	}

	switch (flock) {
		case SQL_LOCK_NO_CHANGE:
		case SQL_LOCK_EXCLUSIVE:
		case SQL_LOCK_UNLOCK:
			break;
	}

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

	// not supported by sqlrelay

	return SQL_ERROR;
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

	// not supported by sqlrelay

	return SQL_ERROR;
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
	return SQLR_SQLBindParam(statementhandle,
					parameternumber,
					valuetype,
					parametertype,
					lengthprecision,
					parameterscale,
					parametervalue,
					strlen_or_ind);
}

SQLRETURN SQL_API SQLBindParameter(SQLHSTMT statementhandle,
					SQLUSMALLINT ipar,
					SQLSMALLINT fparamtype,
					SQLSMALLINT fctype,
					SQLSMALLINT fsqltype,
					SQLULEN cbcoldef,
					SQLSMALLINT ibscale,
					SQLPOINTER rgbvalue,
					SQLLEN cbvaluemax,
					SQLLEN *pcbvalue) {
	debugFunction();
	return SQLR_SQLBindParam(statementhandle,
					ipar,
					fctype,
					fparamtype,
					cbcoldef,
					ibscale,
					rgbvalue,
					pcbvalue);
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
