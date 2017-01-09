// Copyright (c) 2016  David Muse
// See the file COPYING for more information

#include <rudiments/dynamiclib.h>


// for now we only support 10.5...
#undef DB2VERSION
#define DB2VERSION	10


// types...
typedef unsigned char	SQLCHAR;
typedef short		SQLSMALLINT;
typedef unsigned short	SQLUSMALLINT;
typedef int		SQLINTEGER;
typedef unsigned int	SQLUINTEGER;
typedef void *		SQLPOINTER;
#ifdef ODBC64
	typedef	int64_t		SQLLEN;
	typedef	uint64_t	SQLULEN;

	typedef void *		SQLHSTMT;
	typedef void *		SQLHANDLE;
	typedef void *		SQLHENV;
	typedef void *		SQLHDBC;
#else
	#define	SQLLEN		SQLINTEGER
	#define	SQLULEN		SQLUINTEGER

	typedef unsigned int	SQLHSTMT;
	typedef unsigned int	SQLHANDLE;
	typedef unsigned int	SQLHENV;
	typedef unsigned int	SQLHDBC;
#endif

typedef short		SQLRETURN;


// structs...
struct SQL_DATE_STRUCT {
	SQLSMALLINT	year;
	SQLUSMALLINT	month;
	SQLUSMALLINT	day;
};

struct SQL_TIMESTAMP_STRUCT {
	SQLSMALLINT	year;
	SQLUSMALLINT	month;
	SQLUSMALLINT	day;
	SQLUSMALLINT	hour;
	SQLUSMALLINT	minute;
	SQLUSMALLINT	second;
	SQLUINTEGER	fraction;
};


// function pointers...
SQLRETURN (*SQLAllocHandle)(SQLSMALLINT fHandleType,
				SQLHANDLE hInput,
				SQLHANDLE *phOutput);

SQLRETURN (*SQLFreeHandle)(SQLSMALLINT fHandleType,
				SQLHANDLE hHandle);

SQLRETURN (*SQLSetConnectAttr)(SQLHDBC hdbc,
				SQLINTEGER fOption,
				SQLPOINTER pvParam,
				SQLINTEGER fStrLen);

SQLRETURN (*SQLConnect)(SQLHDBC hdbc,
				SQLCHAR *szDSN,
				SQLSMALLINT cbDSN,
				SQLCHAR *szUID,
				SQLSMALLINT cbUID,
				SQLCHAR *szAuthStr,
				SQLSMALLINT cbAuthStr);

SQLRETURN (*SQLGetDiagRec)(SQLSMALLINT fHandleType,
				SQLHANDLE hHandle,
				SQLSMALLINT iRecNumber,
				SQLCHAR *pszSqlState,
				SQLINTEGER *pfNativeError,
				SQLCHAR *pszErrorMsg,
				SQLSMALLINT cbErrorMsgMax,
				SQLSMALLINT *pcbErrorMsg);

SQLRETURN (*SQLGetInfo)(SQLHDBC hdbc,
				SQLUSMALLINT fInfoType,
				SQLPOINTER rgbInfoValue,
				SQLSMALLINT cbInfoValueMax,
				SQLSMALLINT *pcbInfoValue);

SQLRETURN (*SQLDisconnect)(SQLHDBC hdbc);

SQLRETURN (*SQLEndTran)(SQLSMALLINT fHandleType,
				SQLHANDLE hHandle,
				SQLSMALLINT fType);

SQLRETURN (*SQLSetStmtAttr)(SQLHSTMT hstmt,
				SQLINTEGER fOption,
				SQLPOINTER pvParam,
				SQLINTEGER fStrLen);

SQLRETURN (*SQLGetStmtAttr)(SQLHSTMT StatementHandle,
				SQLINTEGER Attribute,
				SQLPOINTER Value,
				SQLINTEGER BufferLength,
				SQLINTEGER *StringLength);

SQLRETURN (*SQLPrepare)(SQLHSTMT hstmt,
				SQLCHAR *szSqlStr,
				SQLINTEGER cbSqlStr);

SQLRETURN (*SQLBindParameter)(SQLHSTMT hstmt,
				SQLUSMALLINT ipar,
				SQLSMALLINT fParamType,
				SQLSMALLINT fCType,
				SQLSMALLINT fSqlType,
				SQLULEN cbColDef,
				SQLSMALLINT ibScale,
				SQLPOINTER rgbValue,
				SQLLEN cbValueMax,
				SQLLEN *pcbValue);

SQLRETURN (*SQLExecute)(SQLHSTMT hstmt);

SQLRETURN (*SQLNumResultCols)(SQLHSTMT hstmt,
				SQLSMALLINT *pccol);

#ifdef ODBC64
SQLRETURN (*SQLColAttribute)(SQLHSTMT hstmt,
				SQLUSMALLINT icol,
				SQLUSMALLINT fDescType,
				SQLPOINTER rgbDesc,
				SQLSMALLINT cbDescMax,
				SQLSMALLINT *pcbDesc,
				SQLLEN *pfDesc);
#else
SQLRETURN (*SQLColAttribute)(SQLHSTMT hstmt,
				SQLUSMALLINT icol,
				SQLUSMALLINT fDescType,
				SQLPOINTER rgbDesc,
				SQLSMALLINT cbDescMax,
				SQLSMALLINT *pcbDesc,
				SQLPOINTER pfDesc);
#endif

SQLRETURN (*SQLBindCol)(SQLHSTMT hstmt,
				SQLUSMALLINT icol,
				SQLSMALLINT fCType,
				SQLPOINTER rgbValue,
				SQLLEN cbValueMax,
				SQLLEN *pcbValue);

SQLRETURN (*SQLRowCount)(SQLHSTMT hstmt,
				SQLLEN *pcrow);

SQLRETURN (*SQLFetchScroll)(SQLHSTMT StatementHandle,
				SQLSMALLINT FetchOrientation,
				SQLLEN FetchOffset);

SQLRETURN (*SQLGetLength)(SQLHSTMT hstmt,
				SQLSMALLINT LocatorCType,
				SQLINTEGER Locator,
				SQLINTEGER *StringLength,
				SQLINTEGER *IndicatorValue);

SQLRETURN (*SQLGetSubString)(SQLHSTMT hstmt,
				SQLSMALLINT LocatorCType,
				SQLINTEGER SourceLocator,
				SQLUINTEGER FromPosition,
				SQLUINTEGER ForLength,
				SQLSMALLINT TargetCType,
				SQLPOINTER rgbValue,
				SQLINTEGER cbValueMax,
				SQLINTEGER *StringLength,
				SQLINTEGER *IndicatorValue);

SQLRETURN (*SQLCloseCursor)(SQLHSTMT hStmt);


// constants...
#define	SQL_HANDLE_ENV	1
#define	SQL_HANDLE_DBC	2
#define	SQL_HANDLE_STMT	3

#define	SQL_NULL_HANDLE	0L

#define	SQL_SUCCESS		0
#define	SQL_SUCCESS_WITH_INFO	1
#define	SQL_NO_DATA		100

#define	SQL_NULL_DATA	-1
#define	SQL_NTS		-3

#define	SQL_COMMIT	0
#define	SQL_ROLLBACK	1

#define	SQL_ATTR_LOGIN_TIMEOUT	103
#define	SQL_LOGIN_TIMEOUT	103

#define	SQL_DBMS_VER	18

#define	SQL_ATTR_AUTOCOMMIT	102
#define	SQL_AUTOCOMMIT_ON	0UL
#define	SQL_AUTOCOMMIT_OFF	1UL

#define	SQL_ATTR_ROW_NUMBER	14
#define	SQL_ATTR_ROW_STATUS_PTR	25
#define	SQL_ATTR_ROW_ARRAY_SIZE	27

#define	SQL_PARAM_INPUT		1
#define	SQL_PARAM_OUTPUT	4

#define	SQL_COLUMN_TYPE			2
#define	SQL_COLUMN_LENGTH		3
#define	SQL_COLUMN_PRECISION		4
#define	SQL_COLUMN_SCALE		5
#define	SQL_COLUMN_NULLABLE		7
#define	SQL_COLUMN_UNSIGNED		8
#define	SQL_COLUMN_AUTO_INCREMENT	11
#define	SQL_COLUMN_LABEL		18

#define	SQL_CHAR		1
#define	SQL_NUMERIC		2
#define	SQL_DECIMAL		3
#define	SQL_INTEGER		4
#define	SQL_SMALLINT		5
#define	SQL_FLOAT		6
#define	SQL_REAL		7
#define	SQL_DOUBLE		8
#define	SQL_DATE		9
#define	SQL_TIMESTAMP		11
#define	SQL_VARCHAR		12
#define	SQL_TYPE_DATE		91
#define	SQL_TYPE_TIME		92
#define	SQL_TYPE_TIMESTAMP	93
#define	SQL_LONGVARCHAR		(-1)
#define	SQL_BINARY		(-2)
#define	SQL_VARBINARY		(-3)
#define	SQL_LONGVARBINARY	(-4)
#define	SQL_BIGINT		(-5)
#define	SQL_TINYINT		(-6)
#define	SQL_BIT			(-7)
#define	SQL_GRAPHIC		(-95)
#define	SQL_VARGRAPHIC		(-96)
#define	SQL_LONGVARGRAPHIC	(-97)
#define	SQL_BLOB		(-98)
#define	SQL_CLOB		(-99)
#define	SQL_DBCLOB		(-350)
#define	SQL_DATALINK		(-400)
#define	SQL_USER_DEFINED_TYPE	(-450)

#define	SQL_C_CHAR		1
#define	SQL_C_LONG		4
#define	SQL_C_DOUBLE		8
#define	SQL_C_DATE		9
#define	SQL_C_TIMESTAMP		11
#define	SQL_C_BINARY		(-2)
#define	SQL_C_BLOB_LOCATOR	31
#define	SQL_C_CLOB_LOCATOR	41

#define	SQL_FETCH_NEXT	1

#define	SQL_ROW_SUCCESS			0
#define	SQL_ROW_SUCCESS_WITH_INFO	6


// dlopen infrastructure...
static dynamiclib	lib;
static const char	*module="db2";
static const char	*libname="libdb2.so";
static const char	*pathnames[]={
	"/opt/ibm/db2/V10.5/lib64",
	"/opt/ibm/db2/V10.5/lib32",
	NULL
};

static bool openOnDemand() {

	// buffer to store any errors we might get
	stringbuffer	err;

	// look for the library
	stringbuffer	libfilename;
	const char	**path=pathnames;
	while (*path) {
		libfilename.clear();
		libfilename.append(*path)->append('/')->append(libname);
		if (file::readable(libfilename.getString())) {
			break;
		}
		path++;
	}
	if (!*path) {
		err.append("\nFailed to load ")->append(module);
		err.append(" libraries.\n");
		err.append(libname)->append(" was not found in any "
						"of these paths:\n");
		path=pathnames;
		while (*path) {
			err.append('	')->append(*path)->append('\n');
			path++;
		}
		stdoutput.write(err.getString());
		return false;
	}

	// open the library
	if (!lib.open(libfilename.getString(),true,true)) {
		goto error;
	}

	// get the functions we need
	SQLAllocHandle=(SQLRETURN (*)(SQLSMALLINT fHandleType,
					SQLHANDLE hInput,
					SQLHANDLE *phOutput))
				lib.getSymbol("SQLAllocHandle");

	SQLFreeHandle=(SQLRETURN (*)(SQLSMALLINT fHandleType,
					SQLHANDLE hHandle))
				lib.getSymbol("SQLFreeHandle");

	SQLSetConnectAttr=(SQLRETURN (*)(SQLHDBC hdbc,
					SQLINTEGER fOption,
					SQLPOINTER pvParam,
					SQLINTEGER fStrLen))
				lib.getSymbol("SQLSetConnectAttr");

	SQLConnect=(SQLRETURN (*)(SQLHDBC hdbc,
					SQLCHAR *szDSN,
					SQLSMALLINT cbDSN,
					SQLCHAR *szUID,
					SQLSMALLINT cbUID,
					SQLCHAR *szAuthStr,
					SQLSMALLINT cbAuthStr))
				lib.getSymbol("SQLConnect");

	SQLGetDiagRec=(SQLRETURN (*)(SQLSMALLINT fHandleType,
					SQLHANDLE hHandle,
					SQLSMALLINT iRecNumber,
					SQLCHAR *pszSqlState,
					SQLINTEGER *pfNativeError,
					SQLCHAR *pszErrorMsg,
					SQLSMALLINT cbErrorMsgMax,
					SQLSMALLINT *pcbErrorMsg))
				lib.getSymbol("SQLGetDiagRec");

	SQLGetInfo=(SQLRETURN (*)(SQLHDBC hdbc,
					SQLUSMALLINT fInfoType,
					SQLPOINTER rgbInfoValue,
					SQLSMALLINT cbInfoValueMax,
					SQLSMALLINT *pcbInfoValue))
				lib.getSymbol("SQLGetInfo");

	SQLDisconnect=(SQLRETURN (*)(SQLHDBC hdbc))
				lib.getSymbol("SQLDisconnect");

	SQLEndTran=(SQLRETURN (*)(SQLSMALLINT fHandleType,
					SQLHANDLE hHandle,
					SQLSMALLINT fType))
				lib.getSymbol("SQLEndTran");

	SQLSetStmtAttr=(SQLRETURN (*)(SQLHSTMT hstmt,
					SQLINTEGER fOption,
					SQLPOINTER pvParam,
					SQLINTEGER fStrLen))
				lib.getSymbol("SQLSetStmtAttr");

	SQLGetStmtAttr=(SQLRETURN (*)(SQLHSTMT StatementHandle,
					SQLINTEGER Attribute,
					SQLPOINTER Value,
					SQLINTEGER BufferLength,
					SQLINTEGER *StringLength))
				lib.getSymbol("SQLGetStmtAttr");

	SQLPrepare=(SQLRETURN (*)(SQLHSTMT hstmt,
					SQLCHAR *szSqlStr,
					SQLINTEGER cbSqlStr))
				lib.getSymbol("SQLPrepare");

	SQLBindParameter=(SQLRETURN (*)(SQLHSTMT hstmt,
					SQLUSMALLINT ipar,
					SQLSMALLINT fParamType,
					SQLSMALLINT fCType,
					SQLSMALLINT fSqlType,
					SQLULEN cbColDef,
					SQLSMALLINT ibScale,
					SQLPOINTER rgbValue,
					SQLLEN cbValueMax,
					SQLLEN *pcbValue))
				lib.getSymbol("SQLBindParameter");

	SQLExecute=(SQLRETURN (*)(SQLHSTMT hstmt))
				lib.getSymbol("SQLExecute");

	SQLNumResultCols=(SQLRETURN (*)(SQLHSTMT hstmt,
					SQLSMALLINT *pccol))
				lib.getSymbol("SQLNumResultCols");

#ifdef ODBC64
	SQLColAttribute=(SQLRETURN (*)(SQLHSTMT hstmt,
					SQLUSMALLINT icol,
					SQLUSMALLINT fDescType,
					SQLPOINTER rgbDesc,
					SQLSMALLINT cbDescMax,
					SQLSMALLINT *pcbDesc,
					SQLLEN *pfDesc))
				lib.getSymbol("SQLColAttribute");
#else
	SQLColAttribute=(SQLRETURN (*)(SQLHSTMT hstmt,
					SQLUSMALLINT icol,
					SQLUSMALLINT fDescType,
					SQLPOINTER rgbDesc,
					SQLSMALLINT cbDescMax,
					SQLSMALLINT *pcbDesc,
					SQLPOINTER pfDesc))
				lib.getSymbol("SQLColAttribute");
#endif

	SQLBindCol=(SQLRETURN (*)(SQLHSTMT hstmt,
					SQLUSMALLINT icol,
					SQLSMALLINT fCType,
					SQLPOINTER rgbValue,
					SQLLEN cbValueMax,
					SQLLEN *pcbValue))
				lib.getSymbol("SQLBindCol");

	SQLRowCount=(SQLRETURN (*)(SQLHSTMT hstmt,
					SQLLEN *pcrow))
				lib.getSymbol("SQLRowCount");

	SQLFetchScroll=(SQLRETURN (*)(SQLHSTMT StatementHandle,
					SQLSMALLINT FetchOrientation,
					SQLLEN FetchOffset))
				lib.getSymbol("SQLFetchScroll");

	SQLGetLength=(SQLRETURN (*)(SQLHSTMT hstmt,
					SQLSMALLINT LocatorCType,
					SQLINTEGER Locator,
					SQLINTEGER *StringLength,
					SQLINTEGER *IndicatorValue))
				lib.getSymbol("SQLGetLength");

	SQLGetSubString=(SQLRETURN (*)(SQLHSTMT hstmt,
					SQLSMALLINT LocatorCType,
					SQLINTEGER SourceLocator,
					SQLUINTEGER FromPosition,
					SQLUINTEGER ForLength,
					SQLSMALLINT TargetCType,
					SQLPOINTER rgbValue,
					SQLINTEGER cbValueMax,
					SQLINTEGER *StringLength,
					SQLINTEGER *IndicatorValue))
				lib.getSymbol("SQLGetSubString");

	SQLCloseCursor=(SQLRETURN (*)(SQLHSTMT hStmt))
				lib.getSymbol("SQLCloseCursor");

	// success
	return true;

	// error
error:
	char	*error=lib.getError();
	err.append("\nFailed to load ")->append(module);
	err.append(" libraries on-demand.\n");
	err.append(error)->append('\n');
	delete[] error;
	return false;
}
