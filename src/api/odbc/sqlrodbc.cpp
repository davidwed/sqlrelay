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

struct CONN;

struct STMT {
	sqlrcursor				*cur;
	uint64_t				currentfetchrow;
	uint64_t				currentgetdatarow;
	CONN					*conn;
	char					*error;
	int64_t					errno;
	char					*name;
	numericdictionary< FIELD * >		fieldlist;
	SQLHANDLE				rowdesc;
	SQLHANDLE				paramdesc;
	SQLHANDLE				improwdesc;
	SQLHANDLE				impparamdesc;
	numericdictionary< outputbind * >	outputbinds;
};

struct ENV;

struct CONN {
	sqlrconnection		*con;
	ENV			*env;
	linkedlist< STMT * >	stmtlist;
	char			*error;
	int64_t			errno;
	char			server[1024];
	uint16_t		port;
	char			socket[1024];
	char			user[1024];
	char			password[1024];
	int32_t			retrytime;
	int32_t			tries;
};

struct ENV {
	SQLINTEGER		odbcversion;
	linkedlist< CONN * >	connlist;
	char			*error;
	int64_t			errno;
};

#if (ODBCVER < 0x0300)
SQLRETURN SQL_API SQLAllocConnect(SQLHENV environmenthandle,
					SQLHDBC *connectionhandle) {
	debugFunction();
	return SQLAllocHandle(SQL_HANDLE_DBC,
				(SQLHANDLE)environmenthandle,
				(SQLHANDLE *)connectionhandle);
}

SQLRETURN SQL_API SQLAllocEnv(SQLHENV *environmenthandle) {
	debugFunction();
	return SQLAllocHandle(SQL_HANDLE_ENV,NULL,
				(SQLHANDLE *)environmenthandle);
}
#endif

SQLRETURN SQL_API SQLAllocHandle(SQLSMALLINT handletype,
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
			env->errno=0;
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
			conn->errno=0;
			env->connlist.append(conn);
			conn->env=env;
			return SQL_SUCCESS;
			}
		#ifdef SQL_HANDLE_DBC_INFO_TOKEN
		case SQL_HANDLE_DBC_INFO_TOKEN:
			debugPrintf("handletype: SQL_HANDLE_DBC_INFO_TOKEN\n");
			// FIXME: no idea what to do here
			return SQL_ERROR;
		#endif
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
			stmt->name=NULL;
			stmt->error=NULL;
			stmt->errno=0;
			conn->stmtlist.append(stmt);
			stmt->conn=conn;
			stmt->rowdesc=(SQLHANDLE)stmt;
			stmt->paramdesc=(SQLHANDLE)stmt;
			stmt->improwdesc=(SQLHANDLE)stmt;
			stmt->impparamdesc=(SQLHANDLE)stmt;
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

#if (ODBCVER < 0x0300)
SQLRETURN SQL_API SQLAllocStmt(SQLHDBC connectionhandle,
					SQLHSTMT *statementhandle) {
	debugFunction();
	return SQLAllocHandle(SQL_HANDLE_STMT,
				(SQLHANDLE)connectionhandle,
				(SQLHANDLE *)statementhandle);
}
#endif

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

#if (ODBCVER < 0x0300)
SQLRETURN SQL_API SQLBindParam(SQLHSTMT statementhandle,
					SQLUSMALLINT parameternumber,
					SQLSMALLINT valuetype,
					SQLSMALLINT parametertype,
					SQLULEN lengthprecision,
					SQLSMALLINT parameterscale,
					SQLPOINTER parametervalue,
					SQLLEN *strlen_or_ind) {
	debugFunction();
	return SQLBindParameter(statementhandle,
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

static void SQLR_ResetParams(STMT *stmt) {
	debugFunction();

	// clear bind variables
	stmt->cur->clearBinds();

	// clear output bind list
	numericdictionarylist< outputbind * >	*list=
					stmt->outputbinds.getList();

	for (dictionarylistnode< int32_t, outputbind * > *node=
						list->getFirstNode();
		node; node=(dictionarylistnode< int32_t, outputbind * > *)
							node->getNext()) {
		delete node;
	}
	list->clear();
}

SQLRETURN SQL_API SQLCloseCursor(SQLHSTMT statementhandle) {
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

static SQLSMALLINT SQLR_MapColumnType(sqlrcursor *cur, uint32_t col) {
	// FIXME: map column types to ODBC concise data types
	/*const char	*ctype=cur->getColumnType(col);
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
	return SQL_CHAR;
}

SQLRETURN SQL_API SQLColAttribute(SQLHSTMT statementhandle,
					SQLUSMALLINT columnnumber,
					SQLUSMALLINT fieldidentifier,
					SQLPOINTER characterattribute,
					SQLSMALLINT bufferlength,
					SQLSMALLINT *stringlength,
					NUMERICATTRIBUTETYPE numericattribute) {
	debugFunction();
	debugPrintf("columnnumber: %d\n",columnnumber);
	debugPrintf("bufferlength: %d\n",(int)bufferlength);

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	// make sure we're attempting to get a valid column
	uint32_t	colcount=stmt->cur->colCount();
	if (columnnumber<1 || columnnumber>colcount) {
		return SQL_ERROR;
	}

	// get a zero-based version of the columnnumber
	uint32_t	col=columnnumber-1;

	switch (fieldidentifier) {
		case SQL_DESC_COUNT:
		case SQL_COLUMN_COUNT:
			debugPrintf("fieldidentifier: "
					"SQL_DESC/COLUMN_COUNT\n");
			*numericattribute=colcount;
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
			*numericattribute=SQLR_MapColumnType(stmt->cur,col);
			debugPrintf("type: %lld\n",
					(int64_t)*numericattribute);
			break;
		case SQL_DESC_LENGTH:
		case SQL_DESC_OCTET_LENGTH:
		case SQL_COLUMN_LENGTH:
			debugPrintf("fieldidentifier: "
					"SQL_DESC_LENGTH/COLUMN_LENGTH/"
					"SQL_DESC_OCTET_LENGTH\n");
			*numericattribute=stmt->cur->getColumnLength(col);
			debugPrintf("length: %lld\n",
					(int64_t)*numericattribute);
			break;
		case SQL_DESC_OCTET_LENGTH_PTR:
			debugPrintf("fieldidentifier: "
					"SQL_DESC_OCTET_LENGTH_PTR\n")
			// FIXME: what is this?
			*numericattribute=0;
			debugPrintf("octet length ptr: %lld\n",
					(int64_t)*numericattribute);
			break;
		case SQL_DESC_PRECISION:
		case SQL_COLUMN_PRECISION:
			debugPrintf("fieldidentifier: "
					"SQL_DESC/COLUMN_PRECISION\n");
			*numericattribute=stmt->cur->getColumnPrecision(col);
			debugPrintf("precision: %lld\n",
					(int64_t)*numericattribute);
			break;
		case SQL_DESC_SCALE:
		case SQL_COLUMN_SCALE:
			debugPrintf("fieldidentifier: "
					"SQL_DESC/COLUMN_SCALE\n");
			*numericattribute=stmt->cur->getColumnScale(col);
			debugPrintf("scale: %lld\n",
					(int64_t)*numericattribute);
			break;
		case SQL_DESC_DATETIME_INTERVAL_CODE:
			debugPrintf("fieldidentifier: "
					"SQL_DESC_DATETIME_INTERVAL_CODE\n");
			// FIXME: what is this?
			*numericattribute=0;
			debugPrintf("datetime interval code: %lld\n",
					(int64_t)*numericattribute);
			break;
		case SQL_DESC_NULLABLE:
		case SQL_COLUMN_NULLABLE:
			debugPrintf("fieldidentifier: "
					"SQL_DESC/COLUMN_NULLABLE\n");
			*numericattribute=stmt->cur->getColumnIsNullable(col);
			debugPrintf("nullable: %lld\n",
					(int64_t)*numericattribute);
			break;
		case SQL_DESC_INDICATOR_PTR:
			debugPrintf("fieldidentifier: "
					"SQL_DESC_INDICATOR_PTR\n");
			// FIXME: what is this?
			*numericattribute=0;
			debugPrintf("indicator ptr: %lld\n",
					(int64_t)*numericattribute);
			break;
		case SQL_DESC_DATA_PTR:
			debugPrintf("fieldidentifier: "
					"SQL_DESC_DATA_PTR\n");
			// FIXME: what is this?
			*numericattribute=0;
			debugPrintf("data ptr: %lld\n",
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
				*stringlength=(SQLSMALLINT)
					charstring::length(name);
				debugPrintf("length: %d\n",(int)*stringlength);
			}
			}
			break;
		case SQL_DESC_UNNAMED:
			debugPrintf("fieldidentifier: "
					"SQL_DESC_UNNAMED\n");
			if (charstring::length(stmt->cur->getColumnName(col))) {
				*numericattribute=SQL_NAMED;
			} else {
				*numericattribute=SQL_UNNAMED;
			} 
			debugPrintf("unnamed: %lld\n",
					(int64_t)*numericattribute);
			break;
		case SQL_DESC_ALLOC_TYPE:
			debugPrintf("fieldidentifier: "
					"SQL_DESC_ALLOC_TYPE\n");
			// FIXME: what is this?
			*numericattribute=0;
			debugPrintf("alloc type: %lld\n",
					(int64_t)*numericattribute);
			break;
		case SQL_DESC_ARRAY_SIZE:
			debugPrintf("fieldidentifier: "
					"SQL_DESC_ARRAY_SIZE\n");
			// FIXME: what is this?
			*numericattribute=0;
			debugPrintf("array size: %lld\n",
					(int64_t)*numericattribute);
			break;
		case SQL_DESC_ARRAY_STATUS_PTR:
			debugPrintf("fieldidentifier: "
					"SQL_DESC_ARRAY_STATUS_PTR\n");
			// FIXME: what is this?
			*numericattribute=0;
			debugPrintf("array status ptr: %lld\n",
					(int64_t)*numericattribute);
			break;
		//case SQL_DESC_AUTO_UNIQUE_VALUE:
		case SQL_COLUMN_AUTO_INCREMENT:
			debugPrintf("fieldidentifier: "
					"SQL_DESC_AUTO_UNIQUE_VALUE/"
					"SQL_COLUMN_AUTO_INCREMENT\n");
			*numericattribute=stmt->cur->
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
		case SQL_DESC_BIND_OFFSET_PTR:
			debugPrintf("fieldidentifier: "
					"SQL_DESC_BIND_OFFSET_PTR\n");
			// FIXME: what is this?
			*numericattribute=0;
			debugPrintf("bind offset ptr: %lld\n",
					(int64_t)*numericattribute);
			break;
		//case SQL_DESC_CASE_SENSITIVE:
		case SQL_COLUMN_CASE_SENSITIVE:
			debugPrintf("fieldidentifier: "
					"SQL_DESC/COLUMN_CASE_SENSITIVE\n");
			// FIXME: SQL Relay doesn't know this at all.
			*numericattribute=0;
			debugPrintf("case sensitive: %lld\n",
					(int64_t)*numericattribute);
			break;
		//case SQL_DESC_CATALOG_NAME:
		case SQL_COLUMN_QUALIFIER_NAME:
			debugPrintf("fieldidentifier: "
					"SQL_DESC_CATALOG_NAME/"
					"SQL_COLUMN_QUALIFIER_NAME\n");
			// SQL Relay doesn't know this, return an empty string.
			charstring::safeCopy((char *)characterattribute,
							bufferlength,"");
			debugPrintf("column qualifier name: \"%s\"\n",
					(const char *)characterattribute);
			if (stringlength) {
				*stringlength=0;
				debugPrintf("length: %d\n",(int)*stringlength);
			}
			break;
		case SQL_DESC_DATETIME_INTERVAL_PRECISION:
			debugPrintf("fieldidentifier: "
				"SQL_DESC_DATETIME_INTERVAL_PRECISION\n");
			// FIXME: SQL Relay doesn't know this at all.
			*numericattribute=0;
			debugPrintf("interval precision: %lld\n",
					(int64_t)*numericattribute);
			break;
		//case SQL_DESC_DISPLAY_SIZE:
		case SQL_COLUMN_DISPLAY_SIZE:
			debugPrintf("fieldidentifier: "
					"SQL_DESC/COLUMN_DISPLAY_SIZE\n");
			*numericattribute=stmt->cur->getLongest(col);
			debugPrintf("display size: %lld\n",
					(int64_t)*numericattribute);
			break;
		//case SQL_DESC_FIXED_PREC_SCALE
		case SQL_COLUMN_MONEY:
			debugPrintf("fieldidentifier: "
					"SQL_DESC_FIXED_PREC_SCALE/"
					"SQL_COLUMN_MONEY\n");
			// FIXME: SQL Relay doesn't know this at all.
			*numericattribute=0;
			debugPrintf("fixed prec scale: %lld\n",
					(int64_t)*numericattribute);
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
				*stringlength=(SQLSMALLINT)
					charstring::length(name);
				debugPrintf("length: %d\n",(int)*stringlength);
			}
			}
			break;
		case SQL_DESC_LITERAL_PREFIX:
			debugPrintf("fieldidentifier: "
					"SQL_DESC_LITERAL_PREFIX\n");
			// FIXME: what is this?
			*numericattribute=0;
			debugPrintf("literal prefix: %lld\n",
					(int64_t)*numericattribute);
			break;
		case SQL_DESC_LITERAL_SUFFIX:
			debugPrintf("fieldidentifier: "
					"SQL_DESC_LITERAL_SUFFIX\n");
			// FIXME: what is this?
			*numericattribute=0;
			debugPrintf("literal suffix: %lld\n",
					(int64_t)*numericattribute);
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
				*stringlength=(SQLSMALLINT)
					charstring::length(name);
				debugPrintf("length: %d\n",(int)*stringlength);
			}
			}
			break;
		case SQL_DESC_MAXIMUM_SCALE:
			debugPrintf("fieldidentifier: "
					"SQL_DESC_MAXIMUM_SCALE\n");
			*numericattribute=stmt->cur->getColumnScale(col);
			debugPrintf("max scale: %lld\n",
					(int64_t)*numericattribute);
			break;
		case SQL_DESC_MINIMUM_SCALE:
			debugPrintf("fieldidentifier: "
					"SQL_DESC_MINIMUM_SCALE\n");
			*numericattribute=stmt->cur->getColumnScale(col);
			debugPrintf("min scale: %lld\n",
					(int64_t)*numericattribute);
			break;
		case SQL_DESC_NUM_PREC_RADIX:
			debugPrintf("fieldidentifier: "
					"SQL_DESC_NUM_PREC_RADIX\n");
			// FIXME: what is this?
			*numericattribute=0;
			debugPrintf("num prec radix: %lld\n",
					(int64_t)*numericattribute);
			break;
		case SQL_DESC_PARAMETER_TYPE:
			debugPrintf("fieldidentifier: "
					"SQL_DESC_PARAMETER_TYPE\n");
			// FIXME: what is this?
			break;
		case SQL_DESC_ROWS_PROCESSED_PTR:
			debugPrintf("fieldidentifier: "
					"SQL_DESC_ROWS_PROCESSED_PTR\n");
			// FIXME: what is this?
			*numericattribute=0;
			debugPrintf("rows processed ptr: %lld\n",
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
			// FIXME: SQL Relay doesn't know this at all.
			*numericattribute=0;
			debugPrintf("searchable: %lld\n",
					(int64_t)*numericattribute);
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
				*stringlength=(SQLSMALLINT)
					charstring::length(name);
				debugPrintf("length: %d\n",(int)*stringlength);
			}
			}
			break;
		//case SQL_DESC_TABLE_NAME
		case SQL_COLUMN_TABLE_NAME:
			debugPrintf("fieldidentifier: "
					"SQL_DESC/COLUMN_TABLE_NAME\n");
			// SQL Relay doesn't know this, return an empty string.
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
			*numericattribute=stmt->cur->getColumnIsUnsigned(col);
			debugPrintf("unsigned: %lld\n",
					(int64_t)*numericattribute);
			break;
		//case SQL_DESC_UPDATABLE
		case SQL_COLUMN_UPDATABLE:
			debugPrintf("fieldidentifier: "
					"SQL_DESC/COLUMN_UPDATEABLE\n");
			// FIXME: SQL Relay doesn't know this at all.
			break;
		#if (ODBCVER < 0x0300)
		case SQL_COLUMN_DRIVER_START:
			debugPrintf("fieldidentifier: "
					"SQL_COLUMN_DRIVER_START\n");
			// FIXME: what is this?
			*numericattribute=0;
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

static void SQLR_BuildTableName(stringbuffer *table,
				SQLCHAR *catalogname,
				SQLSMALLINT namelength1,
				SQLCHAR *schemaname,
				SQLSMALLINT namelength2,
				SQLCHAR *tablename,
				SQLSMALLINT namelength3) {
	debugFunction();
	if (namelength1>0) {
		table->append(catalogname,namelength1);
	}
	if (namelength2>0) {
		if (table->getStringLength()) {
			table->append('.');
		}
		table->append(schemaname,namelength2);
	}
	if (namelength3>0) {
		if (table->getStringLength()) {
			table->append('.');
		}
		table->append(tablename,namelength3);
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
	char	*wild=charstring::duplicate((const char *)columnname,
							namelength4);

	debugPrintf("table: %s\n",table.getString());
	debugPrintf("wild: %s\n",(wild)?wild:"");

	SQLRETURN	retval=
			(stmt->cur->getColumnList(table.getString(),wild))?
							SQL_SUCCESS:SQL_ERROR;
	delete[] wild;
	return retval;
}


SQLRETURN SQL_API SQLConnect(SQLHDBC connectionhandle,
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
	SQLGetPrivateProfileString((const char *)dsn,"server","",
					conn->server,sizeof(conn->server),
					ODBC_INI);
	char	portbuf[6];
	SQLGetPrivateProfileString((const char *)dsn,"port","",
					portbuf,sizeof(portbuf),ODBC_INI);
	conn->port=(uint16_t)charstring::toUnsignedInteger(portbuf);
	SQLGetPrivateProfileString((const char *)dsn,"socket","",
					conn->socket,sizeof(conn->socket),
					ODBC_INI);
	if (charstring::length(user)) {
		snprintf(conn->user,sizeof(conn->user),
					(const char *)user);
	} else {
		SQLGetPrivateProfileString((const char *)dsn,"user","",
					conn->user,sizeof(conn->user),
					ODBC_INI);
	}
	if (charstring::length(password)) {
		snprintf(conn->password,sizeof(conn->password),
					(const char *)password);
	} else {
		SQLGetPrivateProfileString((const char *)dsn,"password","",
					conn->password,sizeof(conn->password),
					ODBC_INI);
	}
	char	retrytimebuf[6];
	SQLGetPrivateProfileString((const char *)dsn,"retrytime","0",
					retrytimebuf,sizeof(retrytimebuf),
					ODBC_INI);
	conn->retrytime=(int32_t)charstring::toInteger(retrytimebuf);
	char	triesbuf[6];
	SQLGetPrivateProfileString((const char *)dsn,"tries","1",
					triesbuf,sizeof(triesbuf),
					ODBC_INI);
	conn->tries=(int32_t)charstring::toInteger(triesbuf);

	debugPrintf("server: %s\n",conn->server);
	debugPrintf("port: %d\n",conn->port);
	debugPrintf("socket: %s\n",conn->socket);
	debugPrintf("user: %s\n",conn->user);
	debugPrintf("password: %s\n",conn->password);
	debugPrintf("retrytime: %d\n",conn->retrytime);
	debugPrintf("tries: %d\n",conn->tries);

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

	ENV	*env=(ENV *)environmenthandle;
	if (environmenthandle==SQL_NULL_HENV || !env) {
		debugPrintf("NULL env handle\n");
		return SQL_INVALID_HANDLE;
	}

	// not supported by SQL Relay

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
	debugPrintf("columnnumber: %d\n",columnnumber);

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	// make sure we're attempting to get a valid column
	uint32_t	colcount=stmt->cur->colCount();
	if (columnnumber<1 || columnnumber>colcount) {
		return SQL_ERROR;
	}

	// get a zero-based version of the columnnumber
	uint32_t	col=columnnumber-1;

	charstring::safeCopy((char *)columnname,bufferlength,
				stmt->cur->getColumnName(col));
	*namelength=charstring::length(columnname);
	*datatype=SQLR_MapColumnType(stmt->cur,col);
	*columnsize=(SQLSMALLINT)stmt->cur->getColumnPrecision(col);
	*decimaldigits=(SQLSMALLINT)stmt->cur->getColumnScale(col);
	*nullable=(SQLSMALLINT)stmt->cur->getColumnIsNullable(col);

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

SQLRETURN SQL_API SQLEndTran(SQLSMALLINT handletype,
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

#if (ODBCVER < 0x0300)
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
		return SQLGetDiagRec(SQL_HANDLE_ENV,
					(SQLHANDLE)environmenthandle,
					1,sqlstate,
					nativeerror,messagetext,
					bufferlength,textlength);
	} else if (connectionhandle && connectionhandle!=SQL_NULL_HANDLE) {
		return SQLGetDiagRec(SQL_HANDLE_DBC,
					(SQLHANDLE)connectionhandle,
					1,sqlstate,
					nativeerror,messagetext,
					bufferlength,textlength);
	} else if (statementhandle && statementhandle!=SQL_NULL_HSTMT) {
		return SQLGetDiagRec(SQL_HANDLE_STMT,
					(SQLHANDLE)statementhandle,
					1,sqlstate,
					nativeerror,messagetext,
					bufferlength,textlength);
	}
	debugPrintf("no valid handle\n");
	return SQL_INVALID_HANDLE;
}
#endif

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
				// struct tagSQL_NUMERIC_STRUCT {
				//    SQLCHAR precision;
				//    SQLSCHAR scale;
				//    SQLCHAR sign[g];
				//    SQLCHAR val[SQL_MAX_NUMERIC_LEN];[e], [f] 
				// } SQL_NUMERIC_STRUCT;
				// [e]   A number is stored in the val field of the SQL_NUMERIC_STRUCT structure as a scaled integer, in little endian mode (the leftmost byte being the least-significant byte). For example, the number 10.001 base 10, with a scale of 4, is scaled to an integer of 100010. Because this is 186AA in hexadecimal format, the value in SQL_NUMERIC_STRUCT would be "AA 86 01 00 00 ... 00", with the number of bytes defined by the SQL_MAX_NUMERIC_LEN #define.
				// [f]   The precision and scale fields of the SQL_C_NUMERIC data type are never used for input from an application, only for output from the driver to the application. When the driver writes a numeric value into the SQL_NUMERIC_STRUCT, it will use its own driver-specific default as the value for the precision field, and it will use the value in the SQL_DESC_SCALE field of the application descriptor (which defaults to 0) for the scale field. An application can provide its own values for precision and scale by setting the SQL_DESC_PRECISION and SQL_DESC_SCALE fields of the application descriptor.
				// [g]   The sign field is 1 if positive, 0 if negative.
				break;
			case SQL_C_DATE:
			case SQL_C_TYPE_DATE:
				debugPrintf("valuetype: "
					"SQL_C_DATE/SQL_C_TYPE_DATE\n");
				// FIXME: implement
				// struct tagDATE_STRUCT {
				//    SQLSMALLINT year;
				//    SQLUSMALLINT month;
				//    SQLUSMALLINT day;  
				// } DATE_STRUCT;
				break;
			case SQL_C_TIME:
			case SQL_C_TYPE_TIME:
				debugPrintf("valuetype: "
					"SQL_C_TIME/SQL_C_TYPE_TIME\n");
				// FIXME: implement
				// struct tagTIME_STRUCT {
				//    SQLUSMALLINT hour;
				//    SQLUSMALLINT minute;
				//    SQLUSMALLINT second;
				// } TIME_STRUCT;
				break;
			case SQL_C_TIMESTAMP:
			case SQL_C_TYPE_TIMESTAMP:
				debugPrintf("valuetype: "
					"SQL_C_TIMESTAMP/"
					"SQL_C_TYPE_TIMESTAMP\n");
				// FIXME: implement
				// struct tagTIMESTAMP_STRUCT {
				//    SQLSMALLINT year;
				//    SQLUSMALLINT month;
				//    SQLUSMALLINT day;
				//    SQLUSMALLINT hour;
				//    SQLUSMALLINT minute;
				//    SQLUSMALLINT second;
				//    SQLUINTEGER fraction;[b] 
				// } TIMESTAMP_STRUCT;
				// [b]   The value of the fraction field is the number of billionths of a second and ranges from 0 through 999,999,999 (1 less than 1 billion). For example, the value of the fraction field for a half-second is 500,000,000, for a thousandth of a second (one millisecond) is 1,000,000, for a millionth of a second (one microsecond) is 1,000, and for a billionth of a second (one nanosecond) is 1.
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
				// FIXME: implement
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
				// FIXME: implement
				// struct tagSQLGUID {
				//    DWORD Data1;
				//    WORD Data2;
				//    WORD Data3;
				//    BYTE Data4[8];
				// } SQLGUID;[k]
				// [k]   SQL_C_GUID can be converted only to SQL_CHAR or SQL_WCHAR.
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
	if (textlength==SQL_NTS || textlength==SQL_NTSL) {
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
	delete[] stmt->error;
	stmt->error=NULL;
	stmt->errno=0;

	// trim query
	uint32_t	statementtextlength=SQLR_TrimQuery(
						statementtext,textlength);

	// run the query
	#ifdef DEBUG_MESSAGES
	stringbuffer	debugstr;
	debugstr.append(statementtext,textlength);
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
	stmt->error=charstring::duplicate(stmt->cur->errorMessage());
	stmt->errno=stmt->cur->errorNumber();
	debugPrintf("error is %lld: %s\n",(long long)stmt->errno,stmt->error);
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
	delete[] stmt->error;
	stmt->error=NULL;
	stmt->errno=0;

	// run the query
	bool	result=stmt->cur->executeQuery();

	// handle success
	if (result) {
		SQLR_FetchOutputBinds(stmt);
		return SQL_SUCCESS;
	}

	// handle error
	stmt->error=charstring::duplicate(stmt->cur->errorMessage());
	stmt->errno=stmt->cur->errorNumber();
	debugPrintf("error is %lld: %s\n",(long long)stmt->errno,stmt->error);
	return SQL_ERROR;
}

SQLRETURN SQL_API SQLFetch(SQLHSTMT statementhandle) {
	debugFunction();

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	if (!stmt->cur->getRow(stmt->currentfetchrow)) {
		return SQL_NO_DATA_FOUND;
	}
	stmt->currentgetdatarow=stmt->currentfetchrow;
	stmt->currentfetchrow++;

	uint32_t	colcount=stmt->cur->colCount();
	for (uint32_t index=0; index<colcount; index++) {

		// get the bound field, if this field isn't bound, move on
		FIELD	*field=NULL;
		if (!stmt->fieldlist.getData(index,&field)) {
			continue;
		}

		// get the data into the bound column
		SQLRETURN	result=SQLGetData(statementhandle,
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

SQLRETURN SQL_API SQLFetchScroll(SQLHSTMT statementhandle,
					SQLSMALLINT fetchorientation,
					SQLLEN fetchoffset) {
	debugFunction();

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	switch (fetchorientation) {
		case SQL_FETCH_PRIOR:
			if (stmt->currentfetchrow>1) {
				stmt->currentfetchrow=stmt->currentfetchrow-2;
			} else {
				stmt->currentfetchrow=0;
			}
			break;
		case SQL_FETCH_FIRST:
			stmt->currentfetchrow=0;
			break;
		case SQL_FETCH_LAST:
			// FIXME: implement this
			break;
		case SQL_FETCH_ABSOLUTE:
			stmt->currentfetchrow=fetchoffset;
			break;
		case SQL_FETCH_RELATIVE:
			if (fetchoffset<0 &&
				(SQLLEN)stmt->currentfetchrow<fetchoffset*-1) {
				stmt->currentfetchrow=0;
			} else {
				stmt->currentfetchrow=
					stmt->currentfetchrow+fetchoffset;
			}
			break;
		case SQL_FETCH_BOOKMARK:
			// FIXME: implement this
			// http://msdn2.microsoft.com/en-us/library/ms710174(VS.85).aspx
			break;
		default:
			debugPrintf("invalid fetchorientation\n");
			return SQL_ERROR;
	}

	// FIXME: update the row status array

	return SQLFetch(statementhandle);
}

#if (ODBCVER < 0x0300)
SQLRETURN SQL_API SQLFreeConnect(SQLHDBC connectionhandle) {
	debugFunction();
	return SQLFreeHandle(SQL_HANDLE_DBC,connectionhandle);
}

SQLRETURN SQL_API SQLFreeEnv(SQLHENV environmenthandle) {
	debugFunction();
	return SQLFreeHandle(SQL_HANDLE_ENV,environmenthandle);
}
#endif

SQLRETURN SQL_API SQLFreeHandle(SQLSMALLINT handletype, SQLHANDLE handle) {
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
			delete stmt->cur;
			delete stmt;
			return SQL_SUCCESS;
			}
		case SQL_HANDLE_DESC:
			debugPrintf("handletype: SQL_HANDLE_DESC\n");
			// FIXME: no idea what to do here
			return SQL_ERROR;
		default:
			debugPrintf("invalid handletype\n");
			return SQL_ERROR;
	}
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
			return SQLCloseCursor(statementhandle);
		case SQL_DROP:
			debugPrintf("option: SQL_DROP\n");
			return SQLFreeHandle(SQL_HANDLE_STMT,
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

SQLRETURN SQL_API SQLGetConnectAttr(SQLHDBC connectionhandle,
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

#if (ODBCVER < 0x0300)
SQLRETURN SQL_API SQLGetConnectOption(SQLHDBC connectionhandle,
					SQLUSMALLINT option,
					SQLPOINTER value) {
	debugFunction();
	return SQLGetConnectAttr(connectionhandle,option,value,256,NULL);
}
#endif

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

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	debugPrintf("columnnumber: %d\n",columnnumber);

	// make sure we're attempting to get a valid column
	uint32_t	colcount=stmt->cur->colCount();
	if (columnnumber<1 || columnnumber>colcount) {
		return SQL_ERROR;
	}

	// get a zero-based version of the columnnumber
	uint32_t	col=columnnumber-1;

	// get the field
	const char	*field=stmt->cur->getField(stmt->currentgetdatarow,col);

	// initialize NULL indicator
	*strlen_or_ind=SQL_NULL_DATA;

	// handle NULL fields
	if (!field) {
		return SQL_SUCCESS;
	}

	// reset targettype based on column type
	if (targettype==SQL_C_DEFAULT) {
		targettype=SQLR_MapColumnType(stmt->cur,col);
	}

	// get the field data
	switch (targettype) {
		case SQL_C_CHAR:
			{
			debugPrintf("targettype: SQL_C_CHAR\n");
			size_t	sizetocopy=stmt->cur->getFieldLength(
						stmt->currentgetdatarow,col);
			if (strlen_or_ind) {
				*strlen_or_ind=sizetocopy;
			}
			// make sure to null-terminate
			charstring::safeCopy((char *)targetvalue,
						bufferlength,
						field,sizetocopy+1);
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
			size_t	sizetocopy=stmt->cur->
						getFieldLength(
						stmt->currentgetdatarow,col);
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
			// FIXME: implement
			// struct tagDATE_STRUCT {
			//    SQLSMALLINT year;
			//    SQLUSMALLINT month;
			//    SQLUSMALLINT day;  
			// } DATE_STRUCT;
			break;
		case SQL_C_TIME:
		case SQL_C_TYPE_TIME:
			debugPrintf("targettype: SQL_C_TIME/SQL_C_TYPE_TIME\n");
			// FIXME: implement
			// struct tagTIME_STRUCT {
			//    SQLUSMALLINT hour;
			//    SQLUSMALLINT minute;
			//    SQLUSMALLINT second;
			// } TIME_STRUCT;
			break;
		case SQL_C_TIMESTAMP:
		case SQL_C_TYPE_TIMESTAMP:
			debugPrintf("targettype: "
				"SQL_C_TIMESTAMP/SQL_C_TYPE_TIMESTAMP\n");
			// FIXME: implement
			// struct tagTIMESTAMP_STRUCT {
			//    SQLSMALLINT year;
			//    SQLUSMALLINT month;
			//    SQLUSMALLINT day;
			//    SQLUSMALLINT hour;
			//    SQLUSMALLINT minute;
			//    SQLUSMALLINT second;
			//    SQLUINTEGER fraction;[b] 
			// } TIMESTAMP_STRUCT;
			// [b]   The value of the fraction field is the number of billionths of a second and ranges from 0 through 999,999,999 (1 less than 1 billion). For example, the value of the fraction field for a half-second is 500,000,000, for a thousandth of a second (one millisecond) is 1,000,000, for a millionth of a second (one microsecond) is 1,000, and for a billionth of a second (one nanosecond) is 1.
			break;
		case SQL_C_NUMERIC:
			debugPrintf("targettype: SQL_C_NUMERIC\n");
			// FIXME: implement
			// struct tagSQL_NUMERIC_STRUCT {
			//    SQLCHAR precision;
			//    SQLSCHAR scale;
			//    SQLCHAR sign[g];
			//    SQLCHAR val[SQL_MAX_NUMERIC_LEN];[e], [f] 
			// } SQL_NUMERIC_STRUCT;
			// [e]   A number is stored in the val field of the SQL_NUMERIC_STRUCT structure as a scaled integer, in little endian mode (the leftmost byte being the least-significant byte). For example, the number 10.001 base 10, with a scale of 4, is scaled to an integer of 100010. Because this is 186AA in hexadecimal format, the value in SQL_NUMERIC_STRUCT would be "AA 86 01 00 00 ... 00", with the number of bytes defined by the SQL_MAX_NUMERIC_LEN #define.
			// [f]   The precision and scale fields of the SQL_C_NUMERIC data type are never used for input from an application, only for output from the driver to the application. When the driver writes a numeric value into the SQL_NUMERIC_STRUCT, it will use its own driver-specific default as the value for the precision field, and it will use the value in the SQL_DESC_SCALE field of the application descriptor (which defaults to 0) for the scale field. An application can provide its own values for precision and scale by setting the SQL_DESC_PRECISION and SQL_DESC_SCALE fields of the application descriptor.
			// [g]   The sign field is 1 if positive, 0 if negative.
			break;
		case SQL_C_GUID:
			debugPrintf("targettype: SQL_C_GUID\n");
			// FIXME: implement
			// struct tagSQLGUID {
			//    DWORD Data1;
			//    WORD Data2;
			//    WORD Data3;
			//    BYTE Data4[8];
			// } SQLGUID;[k]
			// [k]   SQL_C_GUID can be converted only to SQL_CHAR or SQL_WCHAR.
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
			// FIXME: implement
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
			break;
		default:
			debugPrintf("invalid targettype\n");
			return SQL_ERROR;
	}
	return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLGetDescField(SQLHDESC DescriptorHandle,
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

SQLRETURN SQL_API SQLGetDiagField(SQLSMALLINT handletype,
					SQLHANDLE handle,
					SQLSMALLINT recnumber,
					SQLSMALLINT diagidentifier,
					SQLPOINTER diaginfo,
					SQLSMALLINT bufferlength,
					SQLSMALLINT *stringlength) {
	debugFunction();

	// FIXME: this can be used to return the number of affected rows
	// when diagidentifier is SQL_DIAG_ROW_COUNT, among other things

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
			// not supported by sqlrelay
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
			// not supported by sqlrelay
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
			// not supported by sqlrelay
			return SQL_ERROR;
			}
		case SQL_HANDLE_DESC:
			debugPrintf("handletype: SQL_HANDLE_DESC\n");
			// SQL_HANDLE_DESC not supported by sqlrelay
			return SQL_ERROR;
	}
	debugPrintf("invalid handletype\n");
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

	debugPrintf("recnumber: %d\n",recnumber);

	// SQL Relay doesn't have more than 1 error record
	if (recnumber>1) {
		return SQL_NO_DATA;
	}

	switch (handletype) {
		case SQL_HANDLE_ENV:
			{
			debugPrintf("handletype: SQL_HANDLE_ENV\n");
			ENV	*env=(ENV *)handle;
			if (handle==SQL_NULL_HSTMT || !env) {
				debugPrintf("NULL env handle\n");
				return SQL_INVALID_HANDLE;
			}
			*nativeerror=env->errno;
			snprintf((char *)messagetext,
				(size_t)bufferlength,env->error);
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
			*nativeerror=conn->errno;
			snprintf((char *)messagetext,
				(size_t)bufferlength,conn->error);
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
			*nativeerror=stmt->errno;
			snprintf((char *)messagetext,
				(size_t)bufferlength,stmt->error);
			}
			break;
		case SQL_HANDLE_DESC:
			debugPrintf("handletype: SQL_HANDLE_DESC\n");
			// SQL_HANDLE_DESC not supported by sqlrelay
			return SQL_ERROR;
		default:
			debugPrintf("invalid handletype\n");
			return SQL_ERROR;
	}

	// set the sqlstate, we don't really have those, but HY means
	// odbc-driver-specific error and 000 is a generic subclass
	charstring::copy((char *)sqlstate,"HY000");

	debugPrintf("sqlstate: %s\n",sqlstate);
	debugPrintf("nativeerror: %lld\n",(long long)*nativeerror);
	debugPrintf("messagetext: %s\n",messagetext);

	return SQL_SUCCESS;
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
			return SQL_SUCCESS;
		case SQL_ATTR_ODBC_VERSION:
			debugPrintf("attribute: SQL_ATTR_ODBC_VERSION\n");
			if (value) {
				*((SQLINTEGER *)value)=env->odbcversion;
			}
			if (stringlength) {
				*stringlength=sizeof(SQLINTEGER);
			}
			return SQL_SUCCESS;
		case SQL_ATTR_CONNECTION_POOLING:
			debugPrintf("attribute: SQL_ATTR_CONNECTION_POOLING\n");
			// this one is hardcoded to "off"
			// and can't be changed
			*((SQLUINTEGER *)value)=SQL_CP_OFF;
			return SQL_SUCCESS;
		case SQL_ATTR_CP_MATCH:
			debugPrintf("attribute: SQL_ATTR_CP_MATCH\n");
			// this one is hardcoded to "default"
			// and can't be changed
			*((SQLUINTEGER *)value)=SQL_CP_MATCH_DEFAULT;
		default:
			debugPrintf("invalid attribute\n");
			return SQL_ERROR;
	}
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
				"- true\n");
			*supported=SQL_TRUE;
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
				"- true\n");
			*supported=SQL_TRUE;
			break;
		// dupe of SQL_API_SQLCOLATTRIBUTE
		//case SQL_API_SQLCOLATTRIBUTES:
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
				"- true\n");
			*supported=SQL_TRUE;
			break;
		case SQL_API_SQLSETSCROLLOPTIONS:
			debugPrintf("functionid: "
				"SQL_API_SQLSETSCROLLOPTIONS "
				"- true\n");
			*supported=SQL_TRUE;
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
				"- false\n");
			*supported=SQL_FALSE;
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
				"- false\n");
			*supported=SQL_FALSE;
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
				"- false\n");
			*supported=SQL_FALSE;
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

	uint16_t	outsize=0;

	SQLSMALLINT	smallintvar=0;
	SQLINTEGER	intvar=0;
	const char	*strval="";

	// FIXME: there are tons more of these...
	switch (infotype) {
		case SQL_DRIVER_ODBC_VER:
			debugPrintf("infotype: SQL_DRIVER_ODBC_VER\n");
			strval="03.00";
			break;
		case SQL_XOPEN_CLI_YEAR:
			debugPrintf("infotype: SQL_XOPEN_CLI_YEAR\n");
			strval="2007";
			break;
		case SQL_DRIVER_NAME:
			debugPrintf("infotype: SQL_DRIVER_NAME\n");
			strval="SQL Relay";
			break;
		case SQL_DRIVER_VER:
			debugPrintf("infotype: SQL_DRIVER_VER\n");
			strval=SQLR_VERSION;
			break;
		case SQL_DBMS_NAME:
			debugPrintf("infotype: SQL_DBMS_NAME\n");
			strval="SQL Relay";
			break;
		case SQL_DBMS_VER:
			debugPrintf("infotype: SQL_DBMS_VER\n");
			strval=SQLR_VERSION;
			break;
		case SQL_TXN_CAPABLE:
			debugPrintf("infotype: SQL_TXN_CAPABLE\n");
			outsize=16;
			// FIXME: this isn't true for all db's sqlrelay supports
			smallintvar=SQL_TC_ALL;
			break;
		case SQL_TXN_ISOLATION_OPTION:
			debugPrintf("infotype: SQL_TXN_ISOLATION_OPTION\n");
			outsize=32;
			// FIXME: this isn't true for all db's sqlrelay supports
			intvar=SQL_TXN_READ_COMMITTED;
			break;
		case SQL_SCROLL_OPTIONS:
			debugPrintf("infotype: SQL_SCROLL_OPTIONS\n");
			outsize=32;
			// FIXME: this isn't exactly true
			intvar=SQL_SO_FORWARD_ONLY;
			break;
		case SQL_USER_NAME:
			debugPrintf("infotype: SQL_USER_NAME\n");
			strval=conn->user;
			break;
		case SQL_BATCH_ROW_COUNT:
			debugPrintf("infotype: SQL_BATCH_ROW_COUNT\n");
			outsize=32;
			intvar=0;
			break;
		case SQL_BATCH_SUPPORT:
			debugPrintf("infotype: SQL_BATCH_SUPPORT\n");
			outsize=32;
			intvar=0;
			break;
		case SQL_PARAM_ARRAY_ROW_COUNTS:
			debugPrintf("infotype: SQL_PARAM_ARRAY_ROW_COUNTS\n");
			outsize=32;
			intvar=0;
			break;
		case SQL_ALTER_TABLE:
			debugPrintf("infotype: SQL_ALTER_TABLE\n");
			// FIXME: this must be implemented
			//
			// SQL_AT_DROP_COLUMN
			// SQL_AT_ADD_COLUMN
			//
			// #if ODBCVER>=0x0300
			// SQL_AT_ADD_CONSTRAINT
			// SQL_AT_COLUMN_SINGLE
			// SQL_AT_ADD_COLUMN_DEFAULT
			// SQL_AT_ADD_COLUMN_COLLATION
			// SQL_AT_SET_COLUMN_DEFAULT
			// SQL_AT_DROP_COLUMN_DEFAULT
			// SQL_AT_DROP_COLUMN_CASCADE
			// SQL_AT_DROP_COLUMN_RESTRICT
			// SQL_AT_ADD_TABLE_CONSTRAINT
			// SQL_AT_DROP_TABLE_CONSTRAINT_CASCADE
			// SQL_AT_DROP_TABLE_CONSTRAINT_RESTRICT
			// SQL_AT_CONSTRAINT_NAME_DEFINITION
			// SQL_AT_CONSTRAINT_INITIALLY_DEFERRED
			// SQL_AT_CONSTRAINT_INITIALLY_IMMEDIATE
			// SQL_AT_CONSTRAINT_DEFERRABLE
			// SQL_AT_CONSTRAINT_NON_DEFERRABLE
			// #endif
			break;
		case SQL_FETCH_DIRECTION:
			debugPrintf("infotype: SQL_FETCH_DIRECTION\n");
			// FIXME: this must be implemented
			// SQL_FD_FETCH_NEXT
			// SQL_FD_FETCH_FIRST
			// SQL_FD_FETCH_LAST
			// SQL_FD_FETCH_PRIOR
			// SQL_FD_FETCH_ABSOLUTE
			// SQL_FD_FETCH_RELATIVE
			// #if defined
			// SQL_FD_FETCH_BOOKMARK
			// #endif
			break;
		case SQL_LOCK_TYPES:
			debugPrintf("infotype: SQL_LOCK_TYPES\n");
			outsize=32;
			intvar=SQL_LCK_NO_CHANGE;
			break;
		case SQL_ODBC_API_CONFORMANCE:
			debugPrintf("infotype: SQL_ODBC_API_CONFORMANCE\n");
			// FIXME: this must be implemented
			// SQL_OAC_NONE
			// SQL_OAC_LEVEL1
			// SQL_OAC_LEVEL2
			break;
		case SQL_ODBC_SQL_CONFORMANCE:
			debugPrintf("infotype: SQL_ODBC_SQL_CONFORMANCE\n");
			// FIXME: this must be implemented
			// SQL_OSC_MINIMUM
			// SQL_OSC_CORE
			// SQL_OSC_EXTENDED
			break;
		case SQL_POS_OPERATIONS:
			debugPrintf("infotype: SQL_POS_OPERATIONS\n");
			outsize=32;
			intvar=SQL_POS_POSITION;
			break;
		case SQL_POSITIONED_STATEMENTS:
			debugPrintf("infotype: SQL_POSITIONED_STATEMENTS\n");
			// FIXME: this must be implemented
			// SQL_PS_POSITIONED_DELETE
			// SQL_PS_POSITIONED_UPDATE
			// SQL_PS_SELECT_FOR_UPDATE
			break;
		case SQL_SCROLL_CONCURRENCY:
			debugPrintf("infotype: SQL_SCROLL_CONCURRENCY\n");
			// FIXME: this must be implemented
			// SQL_SCCO_READ_ONLY
			// SQL_SCCO_LOCK
			// SQL_SCCO_OPT_ROWVER
			// SQL_SCCO_OPT_VALUES
			//
			// #if ODBCVER>=0x0300
			// SQL_SCCO_OPT_TIMESTAMP
			// #endif
			break;
		case SQL_STATIC_SENSITIVITY:
			debugPrintf("infotype: SQL_STATIC_SENSITIVITY\n");
			// FIXME: this must be implemented
			// SQL_SS_ADDITIONS
			// SQL_SS_DELETIONS
			// SQL_SS_UPDATES
			break;
	}

	switch (outsize) {
		case 0:
			// string
			snprintf((char *)infovalue,bufferlength,strval);
			debugPrintf("infovalue: %s\n",(char *)infovalue);
			if (stringlength) {
				*stringlength=(SQLSMALLINT)
						charstring::length(strval);
				debugPrintf("stringlength: %d\n",*stringlength);
			}
			break;
		case 16:
			// 16-bit integer
			*((SQLSMALLINT *)infovalue)=smallintvar;
			debugPrintf("infovalue: %d\n",(int)smallintvar);
			if (stringlength) {
				*stringlength=(SQLSMALLINT)sizeof(uint16_t);
				debugPrintf("stringlength: %d\n",*stringlength);
			}
			break;
		case 32:
			// 32-bit integer
			*((SQLINTEGER *)infovalue)=intvar;
			debugPrintf("infovalue: %d\n",(int)intvar);
			if (stringlength) {
				*stringlength=(SQLSMALLINT)sizeof(uint32_t);
				debugPrintf("stringlength: %d\n",*stringlength);
			}
			break;
	}

	return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLGetStmtAttr(SQLHSTMT statementhandle,
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

	// FIXME: implement the rest of these

	switch (attribute) {
		#if (ODBCVER >= 0x0300)
		case SQL_ATTR_APP_ROW_DESC:
			debugPrintf("attribute: SQL_ATTR_APP_ROW_DESC\n");
			rawbuffer::copy((void *)value,
					(const void *)stmt->rowdesc,
					sizeof(stmt->rowdesc));
			if (stringlength) {
				*stringlength=
					(SQLSMALLINT)sizeof(stmt->rowdesc);
			}
			break;
		case SQL_ATTR_APP_PARAM_DESC:
			debugPrintf("attribute: SQL_ATTR_APP_PARAM_DESC\n");
			rawbuffer::copy((void *)value,
					(const void *)stmt->paramdesc,
					sizeof(stmt->paramdesc));
			if (stringlength) {
				*stringlength=
					(SQLSMALLINT)sizeof(stmt->paramdesc);
			}
			break;
		case SQL_ATTR_IMP_ROW_DESC:
			debugPrintf("attribute: SQL_ATTR_IMP_ROW_DESC\n");
			rawbuffer::copy((void *)value,
					(const void *)stmt->improwdesc,
					sizeof(stmt->improwdesc));
			if (stringlength) {
				*stringlength=
					(SQLSMALLINT)sizeof(stmt->improwdesc);
			}
			break;
		case SQL_ATTR_IMP_PARAM_DESC:
			debugPrintf("attribute: SQL_ATTR_IMP_PARAM_DESC\n");
			rawbuffer::copy((void *)value,
					(const void *)stmt->impparamdesc,
					sizeof(stmt->impparamdesc));
			if (stringlength) {
				*stringlength=
					(SQLSMALLINT)sizeof(stmt->impparamdesc);
			}
			break;
		case SQL_ATTR_CURSOR_SCROLLABLE:
			debugPrintf("attribute: SQL_ATTR_CURSOR_SCROLLABLE\n");
			break;
		case SQL_ATTR_CURSOR_SENSITIVITY:
			debugPrintf("attribute: SQL_ATTR_CURSOR_SENSITIVITY\n");
			break;
		#endif
		case SQL_QUERY_TIMEOUT:
			debugPrintf("attribute: SQL_QUERY_TIMEOUT\n");
			break;
		case SQL_MAX_ROWS:
			debugPrintf("attribute: SQL_MAX_ROWS:\n");
			break;
		case SQL_NOSCAN:
			debugPrintf("attribute: SQL_NOSCAN\n");
			break;
		case SQL_MAX_LENGTH:
			debugPrintf("attribute: SQL_MAX_LENGTH\n");
			break;
		case SQL_ASYNC_ENABLE:
			debugPrintf("attribute: SQL_ASYNC_ENABLE\n");
			break;
		case SQL_BIND_TYPE:
			debugPrintf("attribute: SQL_BIND_TYPE\n");
			break;
		case SQL_CURSOR_TYPE:
			debugPrintf("attribute: SQL_CURSOR_TYPE\n");
			break;
		case SQL_CONCURRENCY:
			debugPrintf("attribute: SQL_CONCURRENCY\n");
			break;
		case SQL_KEYSET_SIZE:
			debugPrintf("attribute: SQL_KEYSET_SIZE\n");
			break;
		case SQL_ROWSET_SIZE:
			debugPrintf("attribute: SQL_ROWSET_SIZE\n");
			break;
		case SQL_SIMULATE_CURSOR:
			debugPrintf("attribute: SQL_SIMULATE_CURSOR\n");
			break;
		case SQL_RETRIEVE_DATA:
			debugPrintf("attribute: SQL_RETRIEVE_DATA\n");
			break;
		case SQL_USE_BOOKMARKS:
			debugPrintf("attribute: SQL_USE_BOOKMARKS\n");
			break;
		case SQL_GET_BOOKMARK:
			debugPrintf("attribute: SQL_GET_BOOKMARK\n");
			break;
		case SQL_ROW_NUMBER:
			debugPrintf("attribute: SQL_ROW_NUMBER\n");
			break;
		#if (ODBCVER >= 0x0300)
		// dupe of SQL_ASYNC_ENABLE
		//case SQL_ATTR_ASYNC_ENABLE:
		// dupe of case SQL_CURSOR_TYPE
		//case SQL_ATTR_CONCURRENCY:
		// dupe of SQL_CURSOR_TYPE
		//case SQL_ATTR_CURSOR_TYPE:
		case SQL_ATTR_ENABLE_AUTO_IPD:
			debugPrintf("attribute: SQL_ATTR_ENABLE_AUTO_IPD\n");
			break;
		case SQL_ATTR_FETCH_BOOKMARK_PTR:
			debugPrintf("attribute: SQL_ATTR_FETCH_BOOKMARK_PTR\n");
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
			debugPrintf("attribute: "
					"SQL_ATTR_PARAM_BIND_OFFSET_PTR\n");
			break;
		case SQL_ATTR_PARAM_BIND_TYPE:
			debugPrintf("attribute: SQL_ATTR_PARAM_BIND_TYPE\n");
			break;
		case SQL_ATTR_PARAM_OPERATION_PTR:
			debugPrintf("attribute: "
					"SQL_ATTR_PARAM_OPERATION_PTR\n");
			break;
		case SQL_ATTR_PARAM_STATUS_PTR:
			debugPrintf("attribute: SQL_ATTR_PARAM_STATUS_PTR\n");
			break;
		case SQL_ATTR_PARAMS_PROCESSED_PTR:
			debugPrintf("attribute: "
					"SQL_ATTR_PARAMS_PROCESSED_PTR\n");
			break;
		case SQL_ATTR_PARAMSET_SIZE:
			debugPrintf("attribute: SQL_ATTR_PARAMSET_SIZE\n");
			break;
		// dupe of SQL_QUERY_TIMEOUT
		//case SQL_ATTR_QUERY_TIMEOUT:
		// dupe of SQL_RETRIEVE_DATA
		//case SQL_ATTR_RETRIEVE_DATA:
		case SQL_ATTR_ROW_BIND_OFFSET_PTR:
			debugPrintf("attribute: "
					"SQL_ATTR_ROW_BIND_OFFSET_PTR\n");
			break;
		// dupe of SQL_BIND_TYPE
		//case SQL_ATTR_ROW_BIND_TYPE:
		// dupe of SQL_ROW_NUMBER
		//case SQL_ATTR_ROW_NUMBER:
		case SQL_ATTR_ROW_OPERATION_PTR:
			debugPrintf("attribute: SQL_ATTR_ROW_OPERATION_PTR\n");
			break;
		case SQL_ATTR_ROW_STATUS_PTR:
			debugPrintf("attribute: SQL_ATTR_ROW_STATUS_PTR\n");
			break;
		case SQL_ATTR_ROWS_FETCHED_PTR:
			debugPrintf("attribute: SQL_ATTR_ROWS_FETCHED_PTR\n");
			break;
		case SQL_ATTR_ROW_ARRAY_SIZE:
			debugPrintf("attribute: SQL_ATTR_ROW_ARRAY_SIZE\n");
			break;
		// dupe of SQL_SIMULATE_CURSOR
		//case SQL_ATTR_SIMULATE_CURSOR:
		// dupe of SQL_USE_BOOKMARKS
		//case SQL_ATTR_USE_BOOKMARKS:
		#endif
		#if (ODBCVER < 0x0300)
		case SQL_STMT_OPT_MAX:
			debugPrintf("attribute: SQL_STMT_OPT_MAX\n");
			break;
		case SQL_STMT_OPT_MIN:
			debugPrintf("attribute: SQL_STMT_OPT_MIN\n");
			break;
		#endif
		default:
			debugPrintf("invalid attribute\n");
			break;
	}
	return SQL_SUCCESS;
}

#if (ODBCVER < 0x0300)
SQLRETURN SQL_API SQLGetStmtOption(SQLHSTMT statementhandle,
					SQLUSMALLINT option,
					SQLPOINTER value) {
	debugFunction();
	return SQLGetStmtAttr(statementhandle,option,value,-1,NULL);
}
#endif

SQLRETURN SQL_API SQLGetTypeInfo(SQLHSTMT statementhandle,
					SQLSMALLINT DataType) {
	debugFunction();

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	// not supported by SQL Relay

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
	debugPrintf("columncount: %d\n",*columncount);

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

	// trim query
	uint32_t	statementtextlength=SQLR_TrimQuery(
						statementtext,textlength);

	// prepare the query
	#ifdef DEBUG_MESSAGES
	stringbuffer	debugstr;
	debugstr.append(statementtext,textlength);
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

SQLRETURN SQL_API SQLSetConnectAttr(SQLHDBC connectionhandle,
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

	// Other attributes, not supported by SQL Relay.
	// If they are ever supported, SQLSetConnectOption will need to be
	// updated as well.
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

	debugPrintf("invalid attribute\n");

	return SQL_ERROR;
}

#if (ODBCVER < 0x0300)
SQLRETURN SQL_API SQLSetConnectOption(SQLHDBC connectionhandle,
					SQLUSMALLINT option,
					SQLULEN value) {
	debugFunction();
	return SQLSetConnectAttr(connectionhandle,option,(SQLPOINTER)value,0);
}
#endif

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

	ENV	*env=(ENV *)environmenthandle;
	if (environmenthandle==SQL_NULL_HENV || !env) {
		debugPrintf("NULL env handle\n");
		return SQL_INVALID_HANDLE;
	}

	switch (attribute) {
		case SQL_ATTR_OUTPUT_NTS:
			debugPrintf("attribute: SQL_ATTR_OUTPUT_NTS\n");
			// this can't be set to false
			return ((uint64_t)value==SQL_TRUE)?
						SQL_SUCCESS:SQL_ERROR;
		case SQL_ATTR_ODBC_VERSION:
			debugPrintf("attribute: SQL_ATTR_ODBC_VERSION\n");
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
			debugPrintf("attribute: SQL_ATTR_CONNECTION_POOLING\n");
			// this can't be set on
			return ((uint64_t)value==SQL_CP_OFF)?
						SQL_SUCCESS:SQL_ERROR;
		case SQL_ATTR_CP_MATCH:
			debugPrintf("attribute: SQL_ATTR_CP_MATCH\n");
			// this can't be set to anything but default
			return ((uint64_t)value==SQL_CP_MATCH_DEFAULT)?
						SQL_SUCCESS:SQL_ERROR;
		default:
			debugPrintf("invalid attribute\n");
			return SQL_ERROR;
	}
}

#if (ODBCVER < 0x0300)
SQLRETURN SQL_API SQLSetParam(SQLHSTMT statementhandle,
					SQLUSMALLINT parameternumber,
					SQLSMALLINT valuetype,
					SQLSMALLINT parametertype,
					SQLULEN lengthprecision,
					SQLSMALLINT parameterscale,
					SQLPOINTER parametervalue,
					SQLLEN *strlen_or_ind) {
	debugFunction();
	return SQLBindParam(statementhandle,
					parameternumber,
					valuetype,
					parametertype,
					lengthprecision,
					parameterscale,
					parametervalue,
					strlen_or_ind);
}
#endif

SQLRETURN SQL_API SQLSetStmtAttr(SQLHSTMT statementhandle,
					SQLINTEGER attribute,
					SQLPOINTER value,
					SQLINTEGER stringlength) {
	debugFunction();

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	// FIXME: implement the rest of these

	switch (attribute) {
		#if (ODBCVER >= 0x0300)
		// these are read-only
		case SQL_ATTR_APP_ROW_DESC:
			debugPrintf("attribute: SQL_ATTR_APP_ROW_DESC\n");
			return SQL_ERROR;
		case SQL_ATTR_APP_PARAM_DESC:
			debugPrintf("attribute: SQL_ATTR_APP_PARAM_DESC\n");
			return SQL_ERROR;
		case SQL_ATTR_IMP_ROW_DESC:
			debugPrintf("attribute: SQL_ATTR_IMP_ROW_DESC\n");
			return SQL_ERROR;
		case SQL_ATTR_IMP_PARAM_DESC:
			debugPrintf("attribute: SQL_ATTR_IMP_PARAM_DESC\n");
			return SQL_ERROR;

		// these are read-write
		case SQL_ATTR_CURSOR_SCROLLABLE:
			debugPrintf("attribute: SQL_ATTR_CURSOR_SCROLLABLE\n");
			return SQL_SUCCESS;
		case SQL_ATTR_CURSOR_SENSITIVITY:
			debugPrintf("attribute: SQL_ATTR_CURSOR_SENSITIVITY\n");
			return SQL_SUCCESS;
		#endif
		case SQL_QUERY_TIMEOUT:
			debugPrintf("attribute: SQL_QUERY_TIMEOUT\n");
			return SQL_SUCCESS;
		case SQL_MAX_ROWS:
			debugPrintf("attribute: SQL_MAX_ROWS\n");
			return SQL_SUCCESS;
		case SQL_NOSCAN:
			debugPrintf("attribute: SQL_NOSCAN\n");
			return SQL_SUCCESS;
		case SQL_MAX_LENGTH:
			debugPrintf("attribute: SQL_MAX_LENGTH\n");
			return SQL_SUCCESS;
		case SQL_ASYNC_ENABLE:
			debugPrintf("attribute: SQL_ASYNC_ENABLE\n");
			return SQL_SUCCESS;
		case SQL_BIND_TYPE:
			debugPrintf("attribute: SQL_BIND_TYPE\n");
			return SQL_SUCCESS;
		case SQL_CURSOR_TYPE:
			debugPrintf("attribute: SQL_CURSOR_TYPE\n");
			return SQL_SUCCESS;
		case SQL_CONCURRENCY:
			debugPrintf("attribute: SQL_CONCURRENCY\n");
			return SQL_SUCCESS;
		case SQL_KEYSET_SIZE:
			debugPrintf("attribute: SQL_KEYSET_SIZE\n");
			return SQL_SUCCESS;
		case SQL_ROWSET_SIZE:
			debugPrintf("attribute: SQL_ROWSET_SIZE\n");
			return SQL_SUCCESS;
		case SQL_SIMULATE_CURSOR:
			debugPrintf("attribute: SQL_SIMULATE_CURSOR\n");
			return SQL_SUCCESS;
		case SQL_RETRIEVE_DATA:
			debugPrintf("attribute: SQL_RETRIEVE_DATA\n");
			return SQL_SUCCESS;
		case SQL_USE_BOOKMARKS:
			debugPrintf("attribute: SQL_USE_BOOKMARKS\n");
			return SQL_SUCCESS;
		case SQL_GET_BOOKMARK:
			debugPrintf("attribute: SQL_GET_BOOKMARK\n");
			return SQL_SUCCESS;
		case SQL_ROW_NUMBER:
			debugPrintf("attribute: SQL_ROW_NUMBER\n");
			return SQL_SUCCESS;
		#if (ODBCVER >= 0x0300)
		// dupe of SQL_ASYNC_ENABLE
		//case SQL_ATTR_ASYNC_ENABLE:
		// dupe of case SQL_CURSOR_TYPE
		//case SQL_ATTR_CONCURRENCY:
		// dupe of SQL_CURSOR_TYPE
		//case SQL_ATTR_CURSOR_TYPE:
		case SQL_ATTR_ENABLE_AUTO_IPD:
			debugPrintf("attribute: SQL_ATTR_ENABLE_AUTO_IPD\n");
			return SQL_SUCCESS;
		case SQL_ATTR_FETCH_BOOKMARK_PTR:
			debugPrintf("attribute: SQL_ATTR_FETCH_BOOKMARK_PTR\n");
			return SQL_SUCCESS;
		// dupe of SQL_KEYSET_SIZE
		//case SQL_ATTR_KEYSET_SIZE:
		// dupe of SQL_MAX_LENGTH
		//case SQL_ATTR_MAX_LENGTH:
		// dupe of SQL_MAX_ROWS
		//case SQL_ATTR_MAX_ROWS:
		// dupe of SQL_NOSCAN
		//case SQL_ATTR_NOSCAN:
		case SQL_ATTR_PARAM_BIND_OFFSET_PTR:
			debugPrintf("attribute: "
					"SQL_ATTR_PARAM_BIND_OFFSET_PTR\n");
			return SQL_SUCCESS;
		case SQL_ATTR_PARAM_BIND_TYPE:
			debugPrintf("attribute: SQL_ATTR_PARAM_BIND_TYPE\n");
			return SQL_SUCCESS;
		case SQL_ATTR_PARAM_OPERATION_PTR:
			debugPrintf("attribute: "
					"SQL_ATTR_PARAM_OPERATION_PTR\n");
			return SQL_SUCCESS;
		case SQL_ATTR_PARAM_STATUS_PTR:
			debugPrintf("attribute: SQL_ATTR_PARAM_STATUS_PTR\n");
			return SQL_SUCCESS;
		case SQL_ATTR_PARAMS_PROCESSED_PTR:
			debugPrintf("attribute: "
					"SQL_ATTR_PARAMS_PROCESSED_PTR\n");
			return SQL_SUCCESS;
		case SQL_ATTR_PARAMSET_SIZE:
			debugPrintf("attribute: SQL_ATTR_PARAMSET_SIZE\n");
			return SQL_SUCCESS;
		// dupe of SQL_QUERY_TIMEOUT
		//case SQL_ATTR_QUERY_TIMEOUT:
		// dupe of SQL_RETRIEVE_DATA
		//case SQL_ATTR_RETRIEVE_DATA:
		case SQL_ATTR_ROW_BIND_OFFSET_PTR:
			debugPrintf("attribute: "	
					"SQL_ATTR_ROW_BIND_OFFSET_PTR\n");
			return SQL_SUCCESS;
		// dupe of SQL_BIND_TYPE
		//case SQL_ATTR_ROW_BIND_TYPE:
		// dupe of SQL_ROW_NUMBER
		//case SQL_ATTR_ROW_NUMBER:
		case SQL_ATTR_ROW_OPERATION_PTR:
			debugPrintf("attribute: SQL_ATTR_ROW_OPERATION_PTR\n");
			return SQL_SUCCESS;
		case SQL_ATTR_ROW_STATUS_PTR:
			debugPrintf("attribute: SQL_ATTR_ROW_STATUS_PTR\n");
			return SQL_SUCCESS;
		case SQL_ATTR_ROWS_FETCHED_PTR:
			debugPrintf("attribute: SQL_ATTR_ROWS_FETCHED_PTR\n");
			return SQL_SUCCESS;
		case SQL_ATTR_ROW_ARRAY_SIZE:
			debugPrintf("attribute: SQL_ATTR_ROW_ARRAY_SIZE\n");
			return SQL_SUCCESS;
		// dupe of SQL_SIMULATE_CURSOR
		//case SQL_ATTR_SIMULATE_CURSOR:
		// dupe of SQL_USE_BOOKMARKS
		//case SQL_ATTR_USE_BOOKMARKS:
		#endif
		#if (ODBCVER < 0x0300)
		case SQL_STMT_OPT_MAX:
			debugPrintf("attribute: SQL_STMT_OPT_MAX\n");
			return SQL_SUCCESS;
		case SQL_STMT_OPT_MIN:
			debugPrintf("attribute: SQL_STMT_OPT_MIN\n");
			return SQL_SUCCESS;
		#endif
		default:
			return SQL_ERROR;
	}
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

	char	*wild=charstring::duplicate((const char *)tablename,
							namelength3);
	debugPrintf("wild: %s\n",(wild)?wild:"");

	SQLRETURN	retval=(stmt->cur->getTableList(wild))?
						SQL_SUCCESS:SQL_ERROR;
	delete[] wild;
	return retval;
}

#if (ODBCVER < 0x0300)
SQLRETURN SQL_API SQLTransact(SQLHENV environmenthandle,
					SQLHDBC connectionhandle,
					SQLUSMALLINT completiontype) {
	debugFunction();
	if (connectionhandle) {
		return SQLEndTran(SQL_HANDLE_DBC,
					connectionhandle,
					completiontype);
	} else if (environmenthandle) {
		return SQLEndTran(SQL_HANDLE_ENV,
					environmenthandle,
					completiontype);
	} else {
		debugPrintf("no valid handle\n");
		return SQL_INVALID_HANDLE;
	}
}
#endif

SQLRETURN SQL_API SQLDriverConnect(SQLHDBC hdbc,
					SQLHWND hwnd,
					SQLCHAR *szConnStrIn,
					SQLSMALLINT cbConnStrIn,
					SQLCHAR *szConnStrOut,
					SQLSMALLINT cbConnStrOutMax,
					SQLSMALLINT *pcbConnStrOut,
					SQLUSMALLINT fDriverCompletion) {
	debugFunction();

	CONN	*conn=(CONN *)hdbc;
	if (hdbc==SQL_NULL_HANDLE || !conn) {
		debugPrintf("NULL conn handle\n");
		return SQL_INVALID_HANDLE;
	}

	// the connect string may not be null terminated, so make a copy that is
	debugPrintf("%s\n",szConnStrIn);
	debugPrintf("%d\n",cbConnStrIn);
	char	*nulltermconnstr;
	if (cbConnStrIn==SQL_NTS) {
		nulltermconnstr=charstring::duplicate(
					(const char *)szConnStrIn);
	} else {
		nulltermconnstr=charstring::duplicate(
					(const char *)szConnStrIn,
					cbConnStrIn);
	}

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


	// FIXME: currently all we support is SQL_DRIVER_NOPROMPT
	switch (fDriverCompletion) {
		case SQL_DRIVER_PROMPT:
			debugPrintf("SQL_DRIVER_PROMPT\n");
			return SQL_ERROR;
		case SQL_DRIVER_COMPLETE:
			debugPrintf("SQL_DRIVER_COMPLETE\n");
			return SQL_ERROR;
		case SQL_DRIVER_COMPLETE_REQUIRED:
			debugPrintf("SQL_DRIVER_COMPLETE_REQUIRED\n");
			return SQL_ERROR;
		case SQL_DRIVER_NOPROMPT:
			debugPrintf("SQL_DRIVER_NOPROMPT\n");
			if (!charstring::length(servername)) {
				return SQL_ERROR;
			}
			break;
	}

	// since we don't support prompting and updating the connect string...
	if (cbConnStrIn==SQL_NTS) {
		*pcbConnStrOut=(SQLSMALLINT)charstring::length(szConnStrIn);
	} else {
		*pcbConnStrOut=(SQLSMALLINT)cbConnStrIn;
	}
	*pcbConnStrOut=(SQLSMALLINT)cbConnStrIn;
	snprintf((char *)szConnStrOut,*pcbConnStrOut,nulltermconnstr);

	// connect
	SQLRETURN	retval=SQLConnect(hdbc,
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

#if (ODBCVER < 0x0300)
SQLRETURN SQL_API SQLColAttributes(SQLHSTMT statementhandle,
					SQLUSMALLINT icol,
					SQLUSMALLINT fdesctype,
					SQLPOINTER rgbdesc,
					SQLSMALLINT cbdescmax,
					SQLSMALLINT *pcbdesc,
					SQLLEN *pfdesc) {
	debugFunction();
	return SQLColAttribute(statementhandle,
					icol,
					fdesctype,
					rgbdesc,
					cbdescmax,
					pcbdesc,
					(NUMERICATTRIBUTETYPE)pfdesc);
}
#endif

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
					SQLLEN irow,
					SQLULEN *pcrow,
					SQLUSMALLINT *rgfrowstatus) {
	debugFunction();
	SQLRETURN	retval=SQLFetchScroll(statementhandle,ffetchtype,irow);
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

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	// not supported by sqlrelay

	return SQL_ERROR;
}

SQLRETURN SQL_API SQLMoreResults(SQLHSTMT statementhandle) {
	debugFunction();
	// SQL Relay only supports fetching the first result set of a query
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

#if (ODBCVER < 0x0300)
SQLRETURN SQL_API SQLParamOptions(SQLHSTMT statementhandle,
					SQLULEN crow,
					SQLULEN *pirow) {
	debugFunction();
	return (SQLSetStmtAttr(statementhandle,
			SQL_ATTR_PARAMSET_SIZE,
			(SQLPOINTER)crow,0)==SQL_SUCCESS &&
		SQLSetStmtAttr(statementhandle,
			SQL_ATTR_PARAMS_PROCESSED_PTR,
			(SQLPOINTER)pirow,0)==SQL_SUCCESS)?
			SQL_SUCCESS:SQL_ERROR;
}
#endif

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

	// set the current row indices
	stmt->currentfetchrow=irow;
	stmt->currentgetdatarow=irow;

	// foption SQL_POSITION doesn't do anything,
	// SQL Relay doesn't support SQL_REFRESH,
	// SQL_UPDATE, SQL_DELETE and SQL_ADD

	// flock SQL_LOCK_NO_CHANGE doesn't do anything,
	// SQL Relay definitely doesn't support
	// SQL_LOCK_EXCLUSIVE and SQL_LOCK_UNLOCK

	// FIXME: update SQL_ATTR_ROW_OPERATION_PTR

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

	STMT	*stmt=(STMT *)statementhandle;
	if (statementhandle==SQL_NULL_HSTMT || !stmt || !stmt->cur) {
		debugPrintf("NULL stmt handle\n");
		return SQL_INVALID_HANDLE;
	}

	// not supported by sqlrelay

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

	ENV	*env=(ENV *)environmenthandle;
	if (environmenthandle==SQL_NULL_HENV || !env) {
		debugPrintf("NULL env handle\n");
		return SQL_INVALID_HANDLE;
	}

	// not supported by sqlrelay

	return SQL_ERROR;
}
#endif

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
					(uint32_t)lengthprecision,
					(uint32_t)parameterscale);
			break;
		case SQL_C_DOUBLE:
			debugPrintf("valuetype: SQL_C_DOUBLE\n");
			stmt->cur->inputBind(parametername,
					*((double *)parametervalue),
					(uint32_t)lengthprecision,
					(uint32_t)parameterscale);
			break;
		case SQL_C_NUMERIC:
			debugPrintf("valuetype: SQL_C_NUMERIC\n");
			retval=SQL_ERROR;
			// FIXME: implement
			// struct tagSQL_NUMERIC_STRUCT {
			//    SQLCHAR precision;
			//    SQLSCHAR scale;
			//    SQLCHAR sign[g];
			//    SQLCHAR val[SQL_MAX_NUMERIC_LEN];[e], [f] 
			// } SQL_NUMERIC_STRUCT;
			// [e]   A number is stored in the val field of the SQL_NUMERIC_STRUCT structure as a scaled integer, in little endian mode (the leftmost byte being the least-significant byte). For example, the number 10.001 base 10, with a scale of 4, is scaled to an integer of 100010. Because this is 186AA in hexadecimal format, the value in SQL_NUMERIC_STRUCT would be "AA 86 01 00 00 ... 00", with the number of bytes defined by the SQL_MAX_NUMERIC_LEN #define.
			// [f]   The precision and scale fields of the SQL_C_NUMERIC data type are never used for input from an application, only for output from the driver to the application. When the driver writes a numeric value into the SQL_NUMERIC_STRUCT, it will use its own driver-specific default as the value for the precision field, and it will use the value in the SQL_DESC_SCALE field of the application descriptor (which defaults to 0) for the scale field. An application can provide its own values for precision and scale by setting the SQL_DESC_PRECISION and SQL_DESC_SCALE fields of the application descriptor.
			// [g]   The sign field is 1 if positive, 0 if negative.
			break;
		case SQL_C_DATE:
		case SQL_C_TYPE_DATE:
			debugPrintf("valuetype: SQL_C_DATE/SQL_C_TYPE_DATE\n");
			retval=SQL_ERROR;
			// FIXME: implement
			// struct tagDATE_STRUCT {
			//    SQLSMALLINT year;
			//    SQLUSMALLINT month;
			//    SQLUSMALLINT day;  
			// } DATE_STRUCT;
			break;
		case SQL_C_TIME:
		case SQL_C_TYPE_TIME:
			debugPrintf("valuetype: SQL_C_TIME/SQL_C_TYPE_TIME\n");
			retval=SQL_ERROR;
			// FIXME: implement
			// struct tagTIME_STRUCT {
			//    SQLUSMALLINT hour;
			//    SQLUSMALLINT minute;
			//    SQLUSMALLINT second;
			// } TIME_STRUCT;
			break;
		case SQL_C_TIMESTAMP:
		case SQL_C_TYPE_TIMESTAMP:
			debugPrintf("valuetype: "
				"SQL_C_TIMESTAMP/SQL_C_TYPE_TIMESTAMP\n");
			retval=SQL_ERROR;
			// FIXME: implement
			// struct tagTIMESTAMP_STRUCT {
			//    SQLSMALLINT year;
			//    SQLUSMALLINT month;
			//    SQLUSMALLINT day;
			//    SQLUSMALLINT hour;
			//    SQLUSMALLINT minute;
			//    SQLUSMALLINT second;
			//    SQLUINTEGER fraction;[b] 
			// } TIMESTAMP_STRUCT;
			// [b]   The value of the fraction field is the number of billionths of a second and ranges from 0 through 999,999,999 (1 less than 1 billion). For example, the value of the fraction field for a half-second is 500,000,000, for a thousandth of a second (one millisecond) is 1,000,000, for a millionth of a second (one microsecond) is 1,000, and for a billionth of a second (one nanosecond) is 1.
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
			retval=SQL_ERROR;
			// FIXME: implement
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
			debugPrintf("valuetype: SQL_C_GUID\n");
			retval=SQL_ERROR;
			// FIXME: implement
			// struct tagSQLGUID {
			//    DWORD Data1;
			//    WORD Data2;
			//    WORD Data3;
			//    BYTE Data4[8];
			// } SQLGUID;[k]
			// [k]   SQL_C_GUID can be converted only to SQL_CHAR or SQL_WCHAR.
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
			retval=SQL_ERROR;
			// FIXME: implement
			// struct tagSQL_NUMERIC_STRUCT {
			//    SQLCHAR precision;
			//    SQLSCHAR scale;
			//    SQLCHAR sign[g];
			//    SQLCHAR val[SQL_MAX_NUMERIC_LEN];[e], [f] 
			// } SQL_NUMERIC_STRUCT;
			// [e]   A number is stored in the val field of the SQL_NUMERIC_STRUCT structure as a scaled integer, in little endian mode (the leftmost byte being the least-significant byte). For example, the number 10.001 base 10, with a scale of 4, is scaled to an integer of 100010. Because this is 186AA in hexadecimal format, the value in SQL_NUMERIC_STRUCT would be "AA 86 01 00 00 ... 00", with the number of bytes defined by the SQL_MAX_NUMERIC_LEN #define.
			// [f]   The precision and scale fields of the SQL_C_NUMERIC data type are never used for input from an application, only for output from the driver to the application. When the driver writes a numeric value into the SQL_NUMERIC_STRUCT, it will use its own driver-specific default as the value for the precision field, and it will use the value in the SQL_DESC_SCALE field of the application descriptor (which defaults to 0) for the scale field. An application can provide its own values for precision and scale by setting the SQL_DESC_PRECISION and SQL_DESC_SCALE fields of the application descriptor.
			// [g]   The sign field is 1 if positive, 0 if negative.
			break;
		case SQL_C_DATE:
		case SQL_C_TYPE_DATE:
			debugPrintf("valuetype: SQL_C_DATE/SQL_C_TYPE_DATE\n");
			retval=SQL_ERROR;
			// FIXME: implement
			// struct tagDATE_STRUCT {
			//    SQLSMALLINT year;
			//    SQLUSMALLINT month;
			//    SQLUSMALLINT day;  
			// } DATE_STRUCT;
			break;
		case SQL_C_TIME:
		case SQL_C_TYPE_TIME:
			debugPrintf("valuetype: SQL_C_TIME/SQL_C_TYPE_TIME\n");
			retval=SQL_ERROR;
			// FIXME: implement
			// struct tagTIME_STRUCT {
			//    SQLUSMALLINT hour;
			//    SQLUSMALLINT minute;
			//    SQLUSMALLINT second;
			// } TIME_STRUCT;
			break;
		case SQL_C_TIMESTAMP:
		case SQL_C_TYPE_TIMESTAMP:
			debugPrintf("valuetype: "
				"SQL_C_TIMESTAMP/SQL_C_TYPE_TIMESTAMP\n");
			retval=SQL_ERROR;
			// FIXME: implement
			// struct tagTIMESTAMP_STRUCT {
			//    SQLSMALLINT year;
			//    SQLUSMALLINT month;
			//    SQLUSMALLINT day;
			//    SQLUSMALLINT hour;
			//    SQLUSMALLINT minute;
			//    SQLUSMALLINT second;
			//    SQLUINTEGER fraction;[b] 
			// } TIMESTAMP_STRUCT;
			// [b]   The value of the fraction field is the number of billionths of a second and ranges from 0 through 999,999,999 (1 less than 1 billion). For example, the value of the fraction field for a half-second is 500,000,000, for a thousandth of a second (one millisecond) is 1,000,000, for a millionth of a second (one microsecond) is 1,000, and for a billionth of a second (one nanosecond) is 1.
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
			retval=SQL_ERROR;
			// FIXME: implement
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
			break;
		//case SQL_C_VARBOOKMARK: dup of SQL_C_BINARY:
		case SQL_C_BINARY:
			debugPrintf("valuetype: "
				"SQL_C_BINARY/SQL_C_VARBOOKMARK\n");
			stmt->cur->defineOutputBindBlob(parametername);
			break;
		case SQL_C_GUID:
			debugPrintf("valuetype: SQL_C_GUID\n");
			retval=SQL_ERROR;
			// FIXME: implement
			// struct tagSQLGUID {
			//    DWORD Data1;
			//    WORD Data2;
			//    WORD Data3;
			//    BYTE Data4[8];
			// } SQLGUID;[k]
			// [k]   SQL_C_GUID can be converted only to SQL_CHAR or SQL_WCHAR.
			break;
		default:
			debugPrintf("invalid valuetype\n");
			retval=SQL_ERROR;
			break;
	}

	delete[] parametername;

	return retval;
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

}
