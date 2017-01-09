// Copyright (c) 2016  David Muse
// See the file COPYING for more information

#include <rudiments/dynamiclib.h>


// types...
typedef	unsigned char		SQLCHAR;
typedef	short			SQLSMALLINT;
typedef	unsigned short		SQLUSMALLINT;
typedef	int			SQLINTEGER;
typedef	unsigned int		SQLUINTEGER;
typedef	void *			SQLPOINTER;
typedef	long long		SQLLEN;
typedef	unsigned long long	SQLULEN;

typedef	void *		SQLHSTMT;
typedef	void *		SQLHANDLE;
typedef	void *		SQLHENV;
typedef	void *		SQLHDBC;
typedef	void *		SQLHWND;

typedef	short		SQLRETURN;

#define	BOOL	int


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
				SQLHANDLE * phOutput);

SQLRETURN (*SQLFreeHandle)(SQLSMALLINT fHandleType,
				SQLHANDLE hHandle);

SQLRETURN (*SQLSetConnectAttr)(SQLHDBC hdbc,
				SQLINTEGER fOption,
				SQLPOINTER pvParam,
				SQLINTEGER fStrLen);

SQLRETURN (*SQLDriverConnect)(SQLHDBC hdbc,
				SQLHWND hwnd,
				SQLCHAR *szConnStrIn,
				SQLSMALLINT cbConnStrIn,
				SQLCHAR *szConnStrOut,
				SQLSMALLINT cbConnStrOutMax,
				SQLSMALLINT *pcbConnStrOut,
				SQLUSMALLINT fDriverCompletion);

SQLRETURN (*SQLGetInfo)(SQLHDBC hdbc,
				SQLUSMALLINT fInfoType,
				SQLPOINTER rgbInfoValue,
				SQLSMALLINT cbInfoValueMax,
				SQLSMALLINT *pcbInfoValue);

SQLRETURN (*SQLGetDiagRec)(SQLSMALLINT fHandleType,
				SQLHANDLE hHandle,
				SQLSMALLINT iRecNumber,
				SQLCHAR * pszSqlState,
				SQLINTEGER * pfNativeError,
				SQLCHAR * pszErrorMsg,
				SQLSMALLINT cbErrorMsgMax,
				SQLSMALLINT * pcbErrorMsg );

SQLRETURN (*SQLDisconnect)(SQLHDBC hdbc);

SQLRETURN (*SQLEndTran)(SQLSMALLINT fHandleType,
				SQLHANDLE hHandle,
				SQLSMALLINT fType);

SQLRETURN (*SQLSetStmtAttr)(SQLHSTMT hstmt,
				SQLINTEGER fOption,
				SQLPOINTER pvParam,
				SQLINTEGER fStrLen);

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

SQLRETURN (*SQLColAttribute)(SQLHSTMT hstmt,
				SQLUSMALLINT icol,
				SQLUSMALLINT fDescType,
				SQLPOINTER rgbDesc,
				SQLSMALLINT cbDescMax,
				SQLSMALLINT *pcbDesc,
				SQLPOINTER pfDesc);

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

SQLRETURN (*SQLGetStmtAttr)(SQLHSTMT StatementHandle,
				SQLINTEGER Attribute,
				SQLPOINTER Value,
				SQLINTEGER BufferLength,
				SQLINTEGER *StringLength);

SQLRETURN (*SQLGetData)(SQLHSTMT hstmt,
				SQLUSMALLINT icol,
				SQLSMALLINT fCType,
				SQLPOINTER rgbValue,
				SQLLEN cbValueMax,
				SQLLEN *pcbValue);

SQLRETURN (*SQLCloseCursor)(SQLHSTMT hStmt);


// constants...
#define	SQL_HANDLE_ENV	1
#define	SQL_HANDLE_DBC	2
#define	SQL_HANDLE_STMT	3

#define	SQL_NULL_HANDLE	0L

#define	SQL_SUCCESS		0
#define	SQL_SUCCESS_WITH_INFO	1
#define	SQL_NO_DATA		100
#define	SQL_NO_TOTAL		(-4)

#define	SQL_NULL_DATA		-1

#define	SQL_COMMIT	0
#define	SQL_ROLLBACK	1

#define	SQL_ATTR_LOGIN_TIMEOUT	103
#define	SQL_LOGIN_TIMEOUT	103

#define	SQL_DRIVER_COMPLETE	1

#define	SQL_DBMS_VER	18

#define	SQL_ATTR_AUTOCOMMIT	102
#define	SQL_AUTOCOMMIT_ON	1UL
#define	SQL_AUTOCOMMIT_OFF	0UL

#define	SQL_ATTR_ROW_ARRAY_SIZE	27
#define	SQL_ATTR_ROW_NUMBER	14

#define	SQL_PARAM_INPUT		1
#define	SQL_PARAM_OUTPUT	4

#define	SQL_COLUMN_TYPE			2
#define	SQL_COLUMN_PRECISION		4
#define	SQL_COLUMN_SCALE		5
#define	SQL_COLUMN_UNSIGNED		8
#define	SQL_COLUMN_AUTO_INCREMENT	11
#define	SQL_COLUMN_LABEL		18

#define	SQL_CHAR			1
#define	SQL_NUMERIC			2
#define	SQL_DECIMAL			3
#define	SQL_INTEGER			4
#define	SQL_SMALLINT			5
#define	SQL_FLOAT			6
#define	SQL_REAL			7
#define	SQL_DOUBLE			8
#define	SQL_DATE			9
#define	SQL_DATETIME			9
#define	SQL_TIME			10
#define	SQL_TIMESTAMP			11
#define	SQL_VARCHAR			12
#define	SQL_LONGVARCHAR			(-1)
#define	SQL_BINARY			(-2)
#define	SQL_VARBINARY			(-3)
#define	SQL_LONGVARBINARY		(-4)
#define	SQL_BIGINT			(-5)
#define	SQL_TINYINT			(-6)
#define	SQL_BIT				(-7)
#define	SQL_WCHAR			(-8)
#define	SQL_WVARCHAR			(-9)
#define	SQL_WLONGVARCHAR		(-10)
#define	SQL_DECFLOAT			(-360)
#define	SQL_INFX_UDT_FIXED		(-100)
#define	SQL_INFX_UDT_VARYING		(-101)
#define	SQL_INFX_UDT_BLOB		(-102)
#define	SQL_INFX_UDT_CLOB		(-103)
#define	SQL_INFX_UDT_LVARCHAR		(-104)
#define	SQL_INFX_RC_ROW			(-105)
#define	SQL_INFX_RC_COLLECTION		(-106)
#define	SQL_INFX_RC_LIST		(-107)
#define	SQL_INFX_RC_SET			(-108)
#define	SQL_INFX_RC_MULTISET		(-109)
#define	SQL_INFX_UNSUPPORTED		(-110)
#define	SQL_INFX_C_SMARTLOB_LOCATOR	(-111)
#define	SQL_INFX_QUALIFIER		(-112)
#define	SQL_INFX_DECIMAL		(-113)
#define	SQL_INFX_BIGINT			(-114)

#define	SQL_C_CHAR		1
#define	SQL_C_LONG		4
#define	SQL_C_DOUBLE		8
#define	SQL_C_DATE		9
#define	SQL_C_TIMESTAMP		11
#define	SQL_C_BINARY		(-2)

#define	SQL_INFX_ATTR_FLAGS		1900

#define	SQL_INFX_ATTR_LO_AUTOMATIC	2262

#define	SQL_TRUE	1

#define	SQL_FETCH_NEXT	1

#define	ISNULLABLE(flags)	((flags&0x0001)?1:0)


// dlopen infrastructure...
static dynamiclib	lib;
static const char	*module="informix";
static const char	*libname="libifcli.so";
static const char	*pathnames[]={
	"/opt/informix/lib/cli",
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
					SQLHANDLE * phOutput))
				lib.getSymbol("SQLAllocHandle");
	if (!SQLAllocHandle) {
		goto error;
	}

	SQLFreeHandle=(SQLRETURN (*)(SQLSMALLINT fHandleType,
					SQLHANDLE hHandle))
				lib.getSymbol("SQLFreeHandle");
	if (!SQLFreeHandle) {
		goto error;
	}

	SQLSetConnectAttr=(SQLRETURN (*)(SQLHDBC hdbc,
					SQLINTEGER fOption,
					SQLPOINTER pvParam,
					SQLINTEGER fStrLen))
				lib.getSymbol("SQLSetConnectAttr");
	if (!SQLSetConnectAttr) {
		goto error;
	}

	SQLDriverConnect=(SQLRETURN (*)(SQLHDBC hdbc,
					SQLHWND hwnd,
					SQLCHAR *szConnStrIn,
					SQLSMALLINT cbConnStrIn,
					SQLCHAR *szConnStrOut,
					SQLSMALLINT cbConnStrOutMax,
					SQLSMALLINT *pcbConnStrOut,
					SQLUSMALLINT fDriverCompletion))
				lib.getSymbol("SQLDriverConnect");
	if (!SQLDriverConnect) {
		goto error;
	}

	SQLGetInfo=(SQLRETURN (*)(SQLHDBC hdbc,
					SQLUSMALLINT fInfoType,
					SQLPOINTER rgbInfoValue,
					SQLSMALLINT cbInfoValueMax,
					SQLSMALLINT *pcbInfoValue))
				lib.getSymbol("SQLGetInfo");
	if (!SQLGetInfo) {
		goto error;
	}

	SQLGetDiagRec=(SQLRETURN (*)(SQLSMALLINT fHandleType,
					SQLHANDLE hHandle,
					SQLSMALLINT iRecNumber,
					SQLCHAR * pszSqlState,
					SQLINTEGER * pfNativeError,
					SQLCHAR * pszErrorMsg,
					SQLSMALLINT cbErrorMsgMax,
					SQLSMALLINT * pcbErrorMsg ))
				lib.getSymbol("SQLGetDiagRec");
	if (!SQLGetDiagRec) {
		goto error;
	}

	SQLDisconnect=(SQLRETURN (*)(SQLHDBC hdbc))
				lib.getSymbol("SQLDisconnect");
	if (!SQLDisconnect) {
		goto error;
	}
	
	SQLEndTran=(SQLRETURN (*)(SQLSMALLINT fHandleType,
					SQLHANDLE hHandle,
					SQLSMALLINT fType))
				lib.getSymbol("SQLEndTran");
	if (!SQLEndTran) {
		goto error;
	}

	SQLSetStmtAttr=(SQLRETURN (*)(SQLHSTMT hstmt,
					SQLINTEGER fOption,
					SQLPOINTER pvParam,
					SQLINTEGER fStrLen))
				lib.getSymbol("SQLSetStmtAttr");
	if (!SQLSetStmtAttr) {
		goto error;
	}

	SQLPrepare=(SQLRETURN (*)(SQLHSTMT hstmt,
					SQLCHAR *szSqlStr,
					SQLINTEGER cbSqlStr))
				lib.getSymbol("SQLPrepare");
	if (!SQLPrepare) {
		goto error;
	}

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
	if (!SQLBindParameter) {
		goto error;
	}

	SQLExecute=(SQLRETURN (*)(SQLHSTMT hstmt))
				lib.getSymbol("SQLExecute");
	if (!SQLExecute) {
		goto error;
	}

	SQLNumResultCols=(SQLRETURN (*)(SQLHSTMT hstmt,
					SQLSMALLINT *pccol))
				lib.getSymbol("SQLNumResultCols");
	if (!SQLNumResultCols) {
		goto error;
	}

	SQLColAttribute=(SQLRETURN (*)(SQLHSTMT hstmt,
					SQLUSMALLINT icol,
					SQLUSMALLINT fDescType,
					SQLPOINTER rgbDesc,
					SQLSMALLINT cbDescMax,
					SQLSMALLINT *pcbDesc,
					SQLPOINTER pfDesc))
				lib.getSymbol("SQLColAttribute");
	if (!SQLColAttribute) {
		goto error;
	}

	SQLBindCol=(SQLRETURN (*)(SQLHSTMT hstmt,
					SQLUSMALLINT icol,
					SQLSMALLINT fCType,
					SQLPOINTER rgbValue,
					SQLLEN cbValueMax,
					SQLLEN *pcbValue))
				lib.getSymbol("SQLBindCol");
	if (!SQLBindCol) {
		goto error;
	}

	SQLRowCount=(SQLRETURN (*)(SQLHSTMT hstmt,
					SQLLEN *pcrow))
				lib.getSymbol("SQLRowCount");
	if (!SQLRowCount) {
		goto error;
	}

	SQLFetchScroll=(SQLRETURN (*)(SQLHSTMT StatementHandle,
					SQLSMALLINT FetchOrientation,
					SQLLEN FetchOffset))
				lib.getSymbol("SQLFetchScroll");
	if (!SQLFetchScroll) {
		goto error;
	}

	SQLGetStmtAttr=(SQLRETURN (*)(SQLHSTMT StatementHandle,
					SQLINTEGER Attribute,
					SQLPOINTER Value,
					SQLINTEGER BufferLength,
					SQLINTEGER *StringLength))
				lib.getSymbol("SQLGetStmtAttr");
	if (!SQLGetStmtAttr) {
		goto error;
	}

	SQLGetData=(SQLRETURN (*)(SQLHSTMT hstmt,
					SQLUSMALLINT icol,
					SQLSMALLINT fCType,
					SQLPOINTER rgbValue,
					SQLLEN cbValueMax,
					SQLLEN *pcbValue))
				lib.getSymbol("SQLGetData");
	if (!SQLGetData) {
		goto error;
	}

	SQLCloseCursor=(SQLRETURN (*)(SQLHSTMT hStmt))
				lib.getSymbol("SQLCloseCursor");
	if (!SQLCloseCursor) {
		goto error;
	}

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
