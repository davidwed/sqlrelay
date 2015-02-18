// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

// note that config.h must come first to avoid some macro redefinition warnings
#include <config.h>

#include <sql.h>
#include <sqlext.h>
#include <sqlucode.h>
#include <sqltypes.h>

// note that sqlrserver.h must be included after sqltypes.h to
// get around a problem with CHAR/xmlChar in gnome-xml
#include <sqlrelay/sqlrserver.h>
#include <rudiments/charstring.h>
#include <rudiments/error.h>
#include <rudiments/stdio.h>

#include <datatypes.h>
#include <defines.h>

#ifdef HAVE_IODBC
	#include <iodbcinst.h>
#endif

#define FETCH_AT_ONCE		10
#define MAX_SELECT_LIST_SIZE	256
#define MAX_ITEM_BUFFER_SIZE	32768

struct odbccolumn {
	char		name[4096];
	uint16_t	namelength;
	// SQLColAttribute requires that these are signed, 32 bit integers
	int32_t		type;
	int32_t		length;
	int32_t		precision;
	int32_t		scale;
	int32_t		nullable;
	uint16_t	primarykey;
	uint16_t	unique;
	uint16_t	partofkey;
	uint16_t	unsignednumber;
	uint16_t	zerofill;
	uint16_t	binary;
	uint16_t	autoincrement;
};

struct datebind {
	int16_t		*year;
	int16_t		*month;
	int16_t		*day;
	int16_t		*hour;
	int16_t		*minute;
	int16_t		*second;
	int32_t		*microsecond;
	const char	**tz;
	char		*buffer;
};

class odbcconnection;

class odbccursor : public sqlrservercursor {
	friend class odbcconnection;
	private:
				odbccursor(sqlrserverconnection *conn,
							uint16_t id);
				~odbccursor();
		bool		prepareQuery(const char *query,
						uint32_t length);
		bool		allocateStatementHandle();
		void		initializeRowAndColumnCounts();
		bool		inputBind(const char *variable, 
						uint16_t variablesize,
						const char *value, 
						uint32_t valuesize,
						short *isnull);
		bool		inputBind(const char *variable, 
						uint16_t variablesize,
						int64_t *value);
		bool		inputBind(const char *variable, 
						uint16_t variablesize,
						double *value, 
						uint32_t precision,
						uint32_t scale);
		bool		inputBind(const char *variable,
						uint16_t variablesize,
						int64_t year,
						int16_t month,
						int16_t day,
						int16_t hour,
						int16_t minute,
						int16_t second,
						int32_t microsecond,
						const char *tz,
						char *buffer,
						uint16_t buffersize,
						int16_t *isnull);
		bool		outputBind(const char *variable, 
						uint16_t variablesize,
						const char *value, 
						uint16_t valuesize,
						short *isnull);
		bool		outputBind(const char *variable,
						uint16_t variablesize,
						int64_t *value,
						int16_t *isnull);
		bool		outputBind(const char *variable,
						uint16_t variablesize,
						double *value,
						uint32_t *precision,
						uint32_t *scale,
						int16_t *isnull);
		bool		outputBind(const char *variable,
						uint16_t variablesize,
						int16_t *year,
						int16_t *month,
						int16_t *day,
						int16_t *hour,
						int16_t *minute,
						int16_t *second,
						int32_t *microsecond,
						const char **tz,
						char *buffer,
						uint16_t buffersize,
						int16_t *isnull);
		short		nonNullBindValue();
		short		nullBindValue();
		bool		bindValueIsNull(short isnull);
		bool		executeQuery(const char *query,
						uint32_t length);
		bool		handleColumns();
		void		errorMessage(char *errorbuffer,
						uint32_t errorbufferlength,
						uint32_t *errorlength,
						int64_t	*errorcode,
						bool *liveconnection);
		uint64_t	affectedRows();
		uint32_t	colCount();
		const char	*getColumnName(uint32_t i);
		uint16_t	getColumnNameLength(uint32_t i);
		uint16_t	getColumnType(uint32_t i);
		uint32_t	getColumnLength(uint32_t i);
		uint32_t	getColumnPrecision(uint32_t i);
		uint32_t	getColumnScale(uint32_t i);
		uint16_t	getColumnIsNullable(uint32_t i);
		uint16_t	getColumnIsUnsigned(uint32_t i);
		uint16_t	getColumnIsBinary(uint32_t i);
		uint16_t	getColumnIsAutoIncrement(uint32_t i);
		bool		noRowsToReturn();
		bool		fetchRow();
		void		getField(uint32_t col,
					const char **field,
					uint64_t *fieldlength,
					bool *blob,
					bool *null);
		void		nextRow();
		void		closeResultSet();


		SQLRETURN	erg;
		SQLHSTMT	stmt;
		SQLSMALLINT	ncols;
		SQLINTEGER 	affectedrows;

// this code is here in case unixodbc ever 
// successfully supports array fetches

/*#ifdef HAVE_UNIXODBC
		char		field[MAX_SELECT_LIST_SIZE]
					[FETCH_AT_ONCE]
					[MAX_ITEM_BUFFER_SIZE];
		SQLINTEGER	indicator[MAX_SELECT_LIST_SIZE]
						[FETCH_AT_ONCE];
#else*/
		char		field[MAX_SELECT_LIST_SIZE]
					[MAX_ITEM_BUFFER_SIZE];
		SQLINTEGER	indicator[MAX_SELECT_LIST_SIZE];
//#endif
		odbccolumn 	col[MAX_SELECT_LIST_SIZE];

		datebind	**outdatebind;

		uint32_t	row;
		uint32_t	maxrow;
		uint32_t	totalrows;
		uint32_t	rownumber;

		stringbuffer	errormsg;

		odbcconnection	*odbcconn;
};

class odbcconnection : public sqlrserverconnection {
	friend class odbccursor;
	public:
			odbcconnection(sqlrservercontroller *cont);
	private:
		void		handleConnectString();
		bool		logIn(const char **error, const char **warning);
		const char	*logInError(const char *errmsg);
		sqlrservercursor	*newCursor(uint16_t id);
		void		deleteCursor(sqlrservercursor *curs);
		void		logOut();
#if (ODBCVER>=0x0300)
		bool		autoCommitOn();
		bool		autoCommitOff();
		bool		commit();
		bool		rollback();
		void		errorMessage(char *errorbuffer,
						uint32_t errorbufferlength,
						uint32_t *errorlength,
						int64_t	*errorcode,
						bool *liveconnection);
#endif
		bool		ping();
		const char	*identify();
		const char	*dbVersion();
		bool		getListsByApiCalls();
		bool		getDatabaseList(sqlrservercursor *cursor,
						const char *wild);
		bool		getTableList(sqlrservercursor *cursor,
						const char *wild);
		bool		getDatabaseOrTableList(
						sqlrservercursor *cursor,
						const char *wild,
						bool table);
		bool		getColumnList(sqlrservercursor *cursor,
						const char *table,
						const char *wild);
		bool		setIsolationLevel(const char *isolevel);

		SQLRETURN	erg;
		SQLHENV		env;
		SQLHDBC		dbc;

		const char	*dsn;
		uint64_t	timeout;

		stringbuffer	errormessage;

		char		dbversion[512];

#if (ODBCVER>=0x0300)
		stringbuffer	errormsg;
#endif
};

#ifdef HAVE_SQLCONNECTW
#include <iconv.h>
#include <wchar.h>

#define USER_CODING "UTF8"

char *buffers[200];
int nextbuf=0;

void printerror(const char *error) {
	char	*err=error::getErrorString();
	stderror.printf("%s: %s\n",error,err);
	delete[] err;
}

int ucslen(char* str) {
	char *ptr=str;
	int res=0;
	while (!(*ptr==0 && *(ptr+1)==0)) {
		res++;
		ptr+=2;
	}
	return res;
}

char *conv_to_user_coding(char *inbuf) {
	
	size_t	insize=ucslen(inbuf)*2;
	size_t	avail=insize+4;
	//char	*outbuf=(char*)malloc(avail);
	char	*outbuf=new char[avail];
	char	*wrptr=outbuf;

	iconv_t	cd=iconv_open(USER_CODING,"UCS-2");
	if (cd==(iconv_t)-1) {
		/* Something went wrong. */
		printerror("error in iconv_open");

		/* Terminate the output string. */
		*outbuf='\0';
		return outbuf;
	}

	char	*inptr=inbuf;
		
#ifdef ICONV_CONST_CHAR
	size_t	nconv=iconv(cd,(const char **)&inptr,&insize,&wrptr,&avail);
#else
	size_t	nconv=iconv(cd,&inptr,&insize,&wrptr,&avail);
#endif
	if (nconv==(size_t)-1) {
		stdoutput.printf("conv_to_user_coding: error in iconv\n");
	}		
	
	/* Terminate the output string. */
	*(wrptr)='\0';
				
	if (nconv==(size_t)-1) {
		stdoutput.printf("wrptr='%s'\n",wrptr);
	}

	if (iconv_close(cd)!=0) {
		printerror("iconv_close");
	}
	return outbuf;
}

char *conv_to_ucs(char *inbuf) {
	
	size_t	insize=charstring::length(inbuf);
	size_t	avail=insize*2+4;
	//char	*outbuf=(char *)malloc(avail);
	char	*outbuf=new char[avail];
	char	*wrptr=outbuf;

	iconv_t	cd=iconv_open("UCS-2",USER_CODING);
	if (cd==(iconv_t)-1) {
		/* Something went wrong.  */
		printerror("error in iconv_open");

		/* Terminate the output string.  */
		*outbuf = L'\0';
		return outbuf;
	}

	char *inptr = inbuf;
		
#ifdef ICONV_CONST_CHAR
	size_t nconv=iconv(cd,(const char **)&inptr,&insize,&wrptr,&avail);
#else
	size_t nconv=iconv(cd,&inptr,&insize,&wrptr,&avail);
#endif
	if (nconv == (size_t) -1) {
		stdoutput.printf("conv_to_ucs: error in iconv\n");
	}
	
	/* Terminate the output string.  */
	*((wchar_t *)wrptr)=L'\0';
	
	if (nconv==(size_t)-1) {
		stdoutput.printf("inbuf='%s'\n",inbuf);
	}

	if (iconv_close (cd) != 0) {
		printerror("error in iconv_close");
	}
	return outbuf;
}
#endif

odbcconnection::odbcconnection(sqlrservercontroller *cont) :
					sqlrserverconnection(cont) {
}


void odbcconnection::handleConnectString() {
	dsn=cont->getConnectStringValue("dsn");
	cont->setUser(cont->getConnectStringValue("user"));
	cont->setPassword(cont->getConnectStringValue("password"));
	const char	*autocom=cont->getConnectStringValue("autocommit");
	cont->setAutoCommitBehavior((autocom &&
		!charstring::compareIgnoringCase(autocom,"yes")));
	if (!charstring::compare(
			cont->getConnectStringValue("fakebinds"),"yes")) {
		cont->fakeInputBinds();
	}

	const char	*to=cont->getConnectStringValue("timeout");
	if (!charstring::length(to)) {
		// for back-compatibility
		timeout=5;
	} else {
		timeout=charstring::toInteger(to);
	}
}

bool odbcconnection::logIn(const char **error, const char **warning) {

	// allocate environment handle
#if (ODBCVER >= 0x0300)
	erg=SQLAllocHandle(SQL_HANDLE_ENV,SQL_NULL_HANDLE,&env);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		*error="Failed to allocate environment handle";
		SQLFreeHandle(SQL_HANDLE_ENV,env);
		return false;
	}
	erg=SQLSetEnvAttr(env,SQL_ATTR_ODBC_VERSION,
				(void *)SQL_OV_ODBC3,0);
#else
	erg=SQLAllocEnv(&env);
#endif
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		*error="Failed to allocate environment handle";
#if (ODBCVER >= 0x0300)
		SQLFreeHandle(SQL_HANDLE_ENV,env);
#else
		SQLFreeEnv(env);
#endif
		return false;
	}

	// allocate connection handle
#if (ODBCVER >= 0x0300)
	erg=SQLAllocHandle(SQL_HANDLE_DBC,env,&dbc);
#else
	erg=SQLAllocConnect(env,&dbc);
#endif
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		*error="Failed to allocate connection handle";
#if (ODBCVER >= 0x0300)
		SQLFreeHandle(SQL_HANDLE_ENV,env);
		SQLFreeHandle(SQL_HANDLE_DBC,dbc);
#else
		SQLFreeConnect(dbc);
		SQLFreeEnv(env);
#endif
		return false;
	}

	// set the connect timeout
#if (ODBCVER >= 0x0300)
	if (timeout) {
		SQLSetConnectAttr(dbc,SQL_LOGIN_TIMEOUT,
					(SQLPOINTER *)timeout,0);
		if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
			*error="Failed to set timeout";
			SQLFreeHandle(SQL_HANDLE_DBC,dbc);
			SQLFreeHandle(SQL_HANDLE_ENV,env);
			return false;
		}
	}
#endif

	// connect to the database
	char *user_asc=(char*)cont->getUser();
	char *password_asc=(char*)cont->getPassword();
	char *dsn_asc=(char*)dsn;
#ifdef HAVE_SQLCONNECTW
	char *user_ucs=(char*)conv_to_ucs(user_asc);
	char *password_ucs=(char*)conv_to_ucs(password_asc);
	char *dsn_ucs=(char*)conv_to_ucs(dsn_asc);
	erg=SQLConnectW(dbc,(SQLWCHAR *)dsn_ucs,SQL_NTS,
				(SQLWCHAR *)user_ucs,SQL_NTS,
				(SQLWCHAR *)password_ucs,SQL_NTS);
				
	if (user_ucs) {
		delete[] user_ucs;
	}
	if (password_ucs) {
		delete[] password_ucs;
	}
	if (dsn_ucs) {
		delete[] dsn_ucs;
	}
#else
	erg=SQLConnect(dbc,(SQLCHAR *)dsn_asc,SQL_NTS,
				(SQLCHAR *)user_asc,SQL_NTS,
				(SQLCHAR *)password_asc,SQL_NTS);
#endif
	
	if (erg==SQL_SUCCESS_WITH_INFO) {
		*warning=logInError(NULL);
	} else if (erg!=SQL_SUCCESS) {
		*error=logInError("SQLConnect failed");
#if (ODBCVER >= 0x0300)
		SQLFreeHandle(SQL_HANDLE_ENV,env);
		SQLFreeHandle(SQL_HANDLE_DBC,dbc);
#else
		SQLFreeConnect(dbc);
		SQLFreeEnv(env);
#endif
		return false;
	}
	return true;
}

const char *odbcconnection::logInError(const char *errmsg) {

	errormessage.clear();
	if (errmsg) {
		errormessage.append(errmsg)->append(": ");
	}

	// get the error message from db2
	SQLCHAR		state[10];
	SQLINTEGER	nativeerrnum;
	SQLCHAR		errorbuffer[1024];
	SQLSMALLINT	errlength;

	SQLGetDiagRec(SQL_HANDLE_DBC,dbc,1,state,&nativeerrnum,
					errorbuffer,1024,&errlength);
	errormessage.append(errorbuffer,errlength);
	return errormessage.getString();
}

sqlrservercursor *odbcconnection::newCursor(uint16_t id) {
	return (sqlrservercursor *)new odbccursor((sqlrserverconnection *)this,id);
}

void odbcconnection::deleteCursor(sqlrservercursor *curs) {
	delete (odbccursor *)curs;
}

void odbcconnection::logOut() {
	SQLDisconnect(dbc);
#if (ODBCVER >= 0x0300)
	SQLFreeHandle(SQL_HANDLE_DBC,dbc);
	SQLFreeHandle(SQL_HANDLE_ENV,env);
#else
	SQLFreeConnect(dbc);
	SQLFreeEnv(env);
#endif
}

bool odbcconnection::ping() {
	return true;
}

const char *odbcconnection::identify() {
	return "odbc";
}

const char *odbcconnection::dbVersion() {
	SQLSMALLINT	dbversionlen;
	SQLGetInfo(dbc,SQL_DBMS_VER,
			(SQLPOINTER)dbversion,
			(SQLSMALLINT)sizeof(dbversion),
			&dbversionlen);
	return dbversion;
}

bool odbcconnection::getListsByApiCalls() {
	return true;
}

bool odbcconnection::getDatabaseList(sqlrservercursor *cursor, const char *wild) {
	return getDatabaseOrTableList(cursor,wild,false);
}

bool odbcconnection::getTableList(sqlrservercursor *cursor, const char *wild) {
	return getDatabaseOrTableList(cursor,wild,true);
}

bool odbcconnection::getDatabaseOrTableList(sqlrservercursor *cursor,
							const char *wild,
							bool table) {

	odbccursor	*odbccur=(odbccursor *)cursor;

	// allocate the statement handle
	if (!odbccur->allocateStatementHandle()) {
		return false;
	}

	// initialize row and column counts
	odbccur->initializeRowAndColumnCounts();

	// SQLTables takes non-const arguments, so we have to make
	// copies of the various arguments that we want to pass in.
	char	*allcatalogs=(table)?NULL:
				charstring::duplicate(SQL_ALL_CATALOGS);
	char	*wildcopy=charstring::duplicate(wild);
	char	*empty=new char[1];
	empty[0]='\0';

	// get the table/database list
	erg=SQLTables(odbccur->stmt,
			(SQLCHAR *)((table)?empty:allcatalogs),SQL_NTS,
			(SQLCHAR *)empty,SQL_NTS,
			(SQLCHAR *)wildcopy,charstring::length(wildcopy),
			(SQLCHAR *)empty,SQL_NTS);
	bool	retval=(erg==SQL_SUCCESS || erg==SQL_SUCCESS_WITH_INFO);
	delete[] empty;
	delete[] wildcopy;
	delete[] allcatalogs;

	// parse the column information
	return (retval)?odbccur->handleColumns():false;
}

bool odbcconnection::getColumnList(sqlrservercursor *cursor,
					const char *table,
					const char *wild) {

	odbccursor	*odbccur=(odbccursor *)cursor;

	// allocate the statement handle
	if (!odbccur->allocateStatementHandle()) {
		return false;
	}

	// initialize row and column counts
	odbccur->initializeRowAndColumnCounts();

	// SQLColumns takes non-const arguments, so we have to make
	// copies of the various arguments that we want to pass in.
	char	*wildcopy=charstring::duplicate(wild);
	char	*tablecopy=charstring::duplicate(table);
	char	*empty=new char[1];
	empty[0]='\0';

	// get the column list
	erg=SQLColumns(odbccur->stmt,
			(SQLCHAR *)empty,SQL_NTS,
			(SQLCHAR *)empty,SQL_NTS,
			(SQLCHAR *)table,charstring::length(tablecopy),
			(SQLCHAR *)wildcopy,charstring::length(wildcopy));
	bool	retval=(erg==SQL_SUCCESS || erg==SQL_SUCCESS_WITH_INFO);
	delete[] empty;
	delete[] wildcopy;
	delete[] tablecopy;

	// parse the column information
	return (retval)?odbccur->handleColumns():false;
}

#if (ODBCVER >= 0x0300)
bool odbcconnection::autoCommitOn() {
	return (SQLSetConnectAttr(dbc,SQL_ATTR_AUTOCOMMIT,
				(SQLPOINTER)SQL_AUTOCOMMIT_ON,
				sizeof(SQLINTEGER))==SQL_SUCCESS);
}

bool odbcconnection::autoCommitOff() {
	return (SQLSetConnectAttr(dbc,SQL_ATTR_AUTOCOMMIT,
				(SQLPOINTER)SQL_AUTOCOMMIT_OFF,
				sizeof(SQLINTEGER))==SQL_SUCCESS);
}

bool odbcconnection::commit() {
	return (SQLEndTran(SQL_HANDLE_ENV,env,SQL_COMMIT)==SQL_SUCCESS);
}

bool odbcconnection::rollback() {
	return (SQLEndTran(SQL_HANDLE_ENV,env,SQL_ROLLBACK)==SQL_SUCCESS);
}

void odbcconnection::errorMessage(char *errorbuffer,
					uint32_t errorbufferlength,
					uint32_t *errorlength,
					int64_t *errorcode,
					bool *liveconnection) {
	SQLCHAR		state[10];
	SQLINTEGER	nativeerrnum;
	SQLSMALLINT	errlength;

	SQLGetDiagRec(SQL_HANDLE_DBC,dbc,1,state,&nativeerrnum,
				(SQLCHAR *)errorbuffer,errorbufferlength,
				&errlength);

	// set return values
	*errorlength=errlength;
	*errorcode=nativeerrnum;
	*liveconnection=true;
}
#endif

bool odbcconnection::setIsolationLevel(const char *isolevel) {
	// FIXME: do nothing for now.  see task #422
	return true;
}

odbccursor::odbccursor(sqlrserverconnection *conn, uint16_t id) :
						sqlrservercursor(conn,id) {
	odbcconn=(odbcconnection *)conn;
	stmt=NULL;
	outdatebind=new datebind *[conn->cont->cfgfl->getMaxBindCount()];
	for (uint16_t i=0; i<conn->cont->cfgfl->getMaxBindCount(); i++) {
		outdatebind[i]=NULL;
	}
}

odbccursor::~odbccursor() {
	delete[] outdatebind;
}

bool odbccursor::prepareQuery(const char *query, uint32_t length) {

	// allocate the statement handle
	if (!allocateStatementHandle()) {
		return false;
	}

// this code is here in case unixodbc or iodbc ever 
// successfully support array fetches

/*#if (ODBCVER >= 0x0300)
	// set the row array size
	erg=SQLSetStmtAttr(stmt,SQL_ATTR_ROW_ARRAY_SIZE,
				(SQLPOINTER)FETCH_AT_ONCE,0);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return false;
	}
#endif*/

	// prepare the query...

#ifdef HAVE_SQLCONNECTW
	//free allocated buffers
	while (nextbuf>0) {
		nextbuf--;
		if (buffers[nextbuf]) {
			delete[] buffers[nextbuf];
		}
	}
	char *query_ucs=conv_to_ucs((char*)query);
	erg=SQLPrepareW(stmt,(SQLWCHAR *)query_ucs,SQL_NTS);
	if (query_ucs) {
		delete[] query_ucs;
	}
#else
	erg=SQLPrepare(stmt,(SQLCHAR *)query,length);
#endif
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return false;
	}

	return true;
}

bool odbccursor::allocateStatementHandle() {

	if (stmt) {
#if (ODBCVER >= 0x0300)
		SQLFreeHandle(SQL_HANDLE_STMT,stmt);
#else
		SQLFreeStmt(stmt,SQL_DROP);
#endif
	}
#if (ODBCVER >= 0x0300)
	erg=SQLAllocHandle(SQL_HANDLE_STMT,odbcconn->dbc,&stmt);
#else
	erg=SQLAllocStmt(odbcconn->dbc,&stmt);
#endif
	return (erg==SQL_SUCCESS || erg==SQL_SUCCESS_WITH_INFO);
}

bool odbccursor::inputBind(const char *variable,
				uint16_t variablesize,
				const char *value,
				uint32_t valuesize,
				short *isnull) {

	#ifdef HAVE_SQLCONNECTW
	char *value_ucs=conv_to_ucs((char*)value);
	valuesize=ucslen(value_ucs)*2;
	buffers[nextbuf]=value_ucs;
	nextbuf++;
	#endif
						
	if (*isnull==SQL_NULL_DATA) {
		// the 4th parameter (ValueType) must by
		// SQL_C_BINARY for this to work with blobs
		erg=SQLBindParameter(stmt,
				charstring::toInteger(variable+1),
				SQL_PARAM_INPUT,
				/*#ifdef HAVE_SQLCONNECTW
				SQL_C_WCHAR,
				#else
				SQL_C_CHAR,
				#endif*/
				SQL_C_BINARY,
				SQL_CHAR,
				1,
				0,
				#ifdef HAVE_SQLCONNECTW
				(SQLPOINTER)value_ucs,
				#else
				(SQLPOINTER)value,
				#endif
				valuesize,
				#ifdef SQLBINDPARAMETER_SQLLEN
				(SQLLEN *)isnull
				#else
				(SQLINTEGER *)isnull
				#endif
				);
	} else {
		erg=SQLBindParameter(stmt,
				charstring::toInteger(variable+1),
				SQL_PARAM_INPUT,
				#ifdef HAVE_SQLCONNECTW
				SQL_C_WCHAR,
				#else
				SQL_C_CHAR,
				#endif
				SQL_CHAR,
				valuesize,
				0,
				#ifdef HAVE_SQLCONNECTW
				(SQLPOINTER)value_ucs,
				#else
				(SQLPOINTER)value,
				#endif
				valuesize,
				#ifdef SQLBINDPARAMETER_SQLLEN
				(SQLLEN *)NULL
				#else
				(SQLINTEGER *)NULL
				#endif
				);
	}
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return false;
	}
	return true;
}

bool odbccursor::inputBind(const char *variable,
				uint16_t variablesize,
				int64_t *value) {

	erg=SQLBindParameter(stmt,
				charstring::toInteger(variable+1),
				SQL_PARAM_INPUT,
				SQL_C_LONG,
				SQL_INTEGER,
				0,
				0,
				value,
				sizeof(int64_t),
				#ifdef SQLBINDPARAMETER_SQLLEN
				(SQLLEN *)NULL
				#elif defined(SQLBINDPARAMETER_SQLLEN)
				(unsigned long *)NULL
				#else
				(SQLINTEGER *)NULL
				#endif
				);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return false;
	}
	return true;
}

bool odbccursor::inputBind(const char *variable,
				uint16_t variablesize,
				double *value,
				uint32_t precision,
				uint32_t scale) {

	erg=SQLBindParameter(stmt,
				charstring::toInteger(variable+1),
				SQL_PARAM_INPUT,
				SQL_C_DOUBLE,
				SQL_DECIMAL,
				precision,
				scale,
				value,
				sizeof(double),
				#ifdef SQLBINDPARAMETER_SQLLEN
				(SQLLEN *)NULL
				#elif defined(SQLBINDPARAMETER_SQLLEN)
				(unsigned long *)NULL
				#else
				(SQLINTEGER *)NULL
				#endif
				);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return false;
	}
	return true;
}

bool odbccursor::inputBind(const char *variable,
				uint16_t variablesize,
				int64_t year,
				int16_t month,
				int16_t day,
				int16_t hour,
				int16_t minute,
				int16_t second,
				int32_t microsecond,
				const char *tz,
				char *buffer,
				uint16_t buffersize,
				int16_t *isnull) {

	SQL_TIMESTAMP_STRUCT	*ts=(SQL_TIMESTAMP_STRUCT *)buffer;
	ts->year=year;
	ts->month=month;
	ts->day=day;
	ts->hour=hour;
	ts->minute=minute;
	ts->second=second;
	ts->fraction=microsecond*1000;

	erg=SQLBindParameter(stmt,
				charstring::toInteger(variable+1),
				SQL_PARAM_INPUT,
				SQL_C_TIMESTAMP,
				SQL_TIMESTAMP,
				0,
				0,
				buffer,
				0,
				#ifdef SQLBINDPARAMETER_SQLLEN
				(SQLLEN *)NULL
				#elif defined(SQLBINDPARAMETER_SQLLEN)
				(unsigned long *)NULL
				#else
				(SQLINTEGER *)NULL
				#endif
				);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return false;
	}
	return true;
}

bool odbccursor::outputBind(const char *variable, 
				uint16_t variablesize,
				const char *value, 
				uint16_t valuesize, 
				short *isnull) {

	outdatebind[outbindcount]=NULL;

	erg=SQLBindParameter(stmt,
				charstring::toInteger(variable+1),
				SQL_PARAM_OUTPUT,
				SQL_C_CHAR,
				SQL_CHAR,
				0,
				0,
				(SQLPOINTER)value,
				valuesize,
				#ifdef SQLBINDPARAMETER_SQLLEN
				(SQLLEN *)isnull
				#elif defined(SQLBINDPARAMETER_SQLLEN)
				(unsigned long *)isnull
				#else
				(SQLINTEGER *)isnull
				#endif
				);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return false;
	}
	return true;
}

bool odbccursor::outputBind(const char *variable,
				uint16_t variablesize,
				int64_t *value,
				int16_t *isnull) {

	outdatebind[outbindcount]=NULL;

	*value=0;

	erg=SQLBindParameter(stmt,
				charstring::toInteger(variable+1),
				SQL_PARAM_OUTPUT,
				SQL_C_LONG,
				SQL_INTEGER,
				0,
				0,
				value,
				sizeof(int64_t),
				#ifdef SQLBINDPARAMETER_SQLLEN
				(SQLLEN *)isnull
				#elif defined(SQLBINDPARAMETER_SQLLEN)
				(unsigned long *)isnull
				#else
				(SQLINTEGER *)isnull
				#endif
				);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return false;
	}
	return true;
}

bool odbccursor::outputBind(const char *variable,
				uint16_t variablesize,
				double *value,
				uint32_t *precision,
				uint32_t *scale,
				int16_t *isnull) {

	outdatebind[outbindcount]=NULL;

	*value=0.0;

	erg=SQLBindParameter(stmt,
				charstring::toInteger(variable+1),
				SQL_PARAM_OUTPUT,
				SQL_C_DOUBLE,
				SQL_DOUBLE,
				0,
				0,
				value,
				sizeof(double),
				#ifdef SQLBINDPARAMETER_SQLLEN
				(SQLLEN *)isnull
				#elif defined(SQLBINDPARAMETER_SQLLEN)
				(unsigned long *)isnull
				#else
				(SQLINTEGER *)isnull
				#endif
				);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return false;
	}
	return true;
}

bool odbccursor::outputBind(const char *variable,
				uint16_t variablesize,
				int16_t *year,
				int16_t *month,
				int16_t *day,
				int16_t *hour,
				int16_t *minute,
				int16_t *second,
				int32_t *microsecond,
				const char **tz,
				char *buffer,
				uint16_t buffersize,
				int16_t *isnull) {

	datebind	*db=new datebind;
	db->year=year;
	db->month=month;
	db->day=day;
	db->hour=hour;
	db->minute=minute;
	db->second=second;
	db->microsecond=microsecond;
	db->tz=tz;
	db->buffer=buffer;
	outdatebind[outbindcount]=db;

	erg=SQLBindParameter(stmt,
				charstring::toInteger(variable+1),
				SQL_PARAM_OUTPUT,
				SQL_C_TIMESTAMP,
				SQL_TIMESTAMP,
				0,
				0,
				buffer,
				0,
				#ifdef SQLBINDPARAMETER_SQLLEN
				(SQLLEN *)isnull
				#elif defined(SQLBINDPARAMETER_SQLLEN)
				(unsigned long *)isnull
				#else
				(SQLINTEGER *)isnull
				#endif
				);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return false;
	}
	return true;
}

short odbccursor::nonNullBindValue() {
	return 0;
}

short odbccursor::nullBindValue() {
	return SQL_NULL_DATA;
}

bool odbccursor::bindValueIsNull(short isnull) {
	return (isnull==SQL_NULL_DATA);
}

bool odbccursor::executeQuery(const char *query, uint32_t length) {

	// initialize counts
	initializeRowAndColumnCounts();

	// execute the query
	erg=SQLExecute(stmt);
	if (erg!=SQL_SUCCESS &&
			erg!=SQL_SUCCESS_WITH_INFO
#if defined(SQL_NO_DATA)
			&& erg!=SQL_NO_DATA
#elif defined(SQL_NO_DATA_FOUND)
			&& erg!=SQL_NO_DATA_FOUND
#endif
		) {
		return false;
	}

	checkForTempTable(query,length);

	if (!handleColumns()) {
		return false;
	}

	// get the row count
#ifdef SQLROWCOUNT_SQLLEN
	erg=SQLRowCount(stmt,(SQLLEN *)&affectedrows);
#else
	erg=SQLRowCount(stmt,&affectedrows);
#endif
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return false;
	}

	// convert date output binds
	for (uint16_t i=0; i<conn->cont->cfgfl->getMaxBindCount(); i++) {
		if (outdatebind[i]) {
			datebind	*db=outdatebind[i];
			SQL_TIMESTAMP_STRUCT	*ts=
				(SQL_TIMESTAMP_STRUCT *)db->buffer;
			*(db->year)=ts->year;
			*(db->month)=ts->month;
			*(db->day)=ts->day;
			*(db->hour)=ts->hour;
			*(db->minute)=ts->minute;
			*(db->second)=ts->second;
			*(db->microsecond)=ts->fraction/1000;
			*(db->tz)=NULL;
		}
	}

	return true;
}

void odbccursor::initializeRowAndColumnCounts() {
	ncols=0;
	row=0;
	maxrow=0;
	totalrows=0;
}

bool odbccursor::handleColumns() {

	// get the column count
	erg=SQLNumResultCols(stmt,&ncols);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return false;
	}
	if (ncols>MAX_SELECT_LIST_SIZE) {
		ncols=MAX_SELECT_LIST_SIZE;
	}

	// run through the columns
	for (SQLSMALLINT i=0; i<ncols; i++) {

		if (conn->cont->getSendColumnInfo()==SEND_COLUMN_INFO) {
#if (ODBCVER >= 0x0300)
			// column name

		
			erg=SQLColAttribute(stmt,i+1,SQL_DESC_LABEL,
					col[i].name,4096,
					(SQLSMALLINT *)&(col[i].namelength),
					#ifdef SQLCOLATTRIBUTE_SQLLEN
					(SQLLEN *)NULL
					#else
					NULL
					#endif
					);
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return false;
			}
//orbb
			col[i].namelength=charstring::length(col[i].name);
//orbb

			// column length
			erg=SQLColAttribute(stmt,i+1,SQL_DESC_LENGTH,
					NULL,0,NULL,
					#ifdef SQLCOLATTRIBUTE_SQLLEN
					(SQLLEN *)&(col[i].length)
					#else
					(SQLINTEGER *)&(col[i].length)
					#endif
					);
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return false;
			}
	
			// column type
			erg=SQLColAttribute(stmt,i+1,SQL_DESC_TYPE,
					NULL,0,NULL,
					#ifdef SQLCOLATTRIBUTE_SQLLEN
					(SQLLEN *)&(col[i].type)
					#else
					(SQLINTEGER *)&(col[i].type)
					#endif
					);
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return false;
			}

			// column precision
			erg=SQLColAttribute(stmt,i+1,SQL_DESC_PRECISION,
					NULL,0,NULL,
					#ifdef SQLCOLATTRIBUTE_SQLLEN
					(SQLLEN *)&(col[i].precision)
					#else
					(SQLINTEGER *)&(col[i].precision)
					#endif
					);
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return false;
			}

			// column scale
			erg=SQLColAttribute(stmt,i+1,SQL_DESC_SCALE,
					NULL,0,NULL,
					#ifdef SQLCOLATTRIBUTE_SQLLEN
					(SQLLEN *)&(col[i].scale)
					#else
					(SQLINTEGER *)&(col[i].scale)
					#endif
					);
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return false;
			}

			// column nullable
			erg=SQLColAttribute(stmt,i+1,SQL_DESC_NULLABLE,
					NULL,0,NULL,
					#ifdef SQLCOLATTRIBUTE_SQLLEN
					(SQLLEN *)&(col[i].nullable)
					#else
					(SQLINTEGER *)&(col[i].nullable)
					#endif
					);
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return false;
			}

			// primary key

			// unique

			// part of key

			// unsigned number
			erg=SQLColAttribute(stmt,i+1,SQL_DESC_UNSIGNED,
					NULL,0,NULL,
					#ifdef SQLCOLATTRIBUTE_SQLLEN
					(SQLLEN *)&(col[i].unsignednumber)
					#else
					(SQLINTEGER *)&(col[i].unsignednumber)
					#endif
					);
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return false;
			}

			// zero fill

			// binary

			// autoincrement
			erg=SQLColAttribute(stmt,i+1,SQL_DESC_AUTO_UNIQUE_VALUE,
					NULL,0,NULL,
					#ifdef SQLCOLATTRIBUTE_SQLLEN
					(SQLLEN *)&(col[i].autoincrement)
					#else
					(SQLINTEGER *)&(col[i].autoincrement)
					#endif
					);
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return false;
			}
#else
			// column name
			erg=SQLColAttributes(stmt,i+1,SQL_COLUMN_LABEL,
					col[i].name,4096,
					(SQLSMALLINT *)&(col[i].namelength),
					NULL);
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return false;
			}

			// column length
			erg=SQLColAttributes(stmt,i+1,SQL_COLUMN_LENGTH,
					NULL,0,NULL,
					(SQLINTEGER *)&(col[i].length));
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return false;
			}

			// column type
			erg=SQLColAttributes(stmt,i+1,SQL_COLUMN_TYPE,
					NULL,0,NULL,
					(SQLINTEGER *)&(col[i].type));
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return false;
			}

			// column precision
			erg=SQLColAttributes(stmt,i+1,SQL_COLUMN_PRECISION,
					NULL,0,NULL,
					(SQLINTEGER *)&(col[i].precision));
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return false;
			}

			// column scale
			erg=SQLColAttributes(stmt,i+1,SQL_COLUMN_SCALE,
					NULL,0,NULL,
					(SQLINTEGER *)&(col[i].scale));
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return false;
			}

			// column nullable
			erg=SQLColAttributes(stmt,i+1,SQL_COLUMN_NULLABLE,
					NULL,0,NULL,
					(SQLINTEGER *)&(col[i].nullable));
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return false;
			}

			// primary key

			// unique

			// part of key

			// unsigned number
			erg=SQLColAttributes(stmt,i+1,SQL_COLUMN_UNSIGNED,
					NULL,0,NULL,
					(SQLINTEGER *)&(col[i].unsignednumber));
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return false;
			}

			// zero fill

			// binary

			// autoincrement
#ifdef SQL_DESC_AUTO_UNIQUE_VALUE
			erg=SQLColAttributes(stmt,i+1,
					SQL_COLUMN_AUTO_UNIQUE_VALUE,
					NULL,0,NULL,
					(SQLINTEGER *)&(col[i].autoincrement));
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return false;
			}
#else
			col[i].autoincrement=0;
#endif
#endif
		}


		// bind the column to a buffer
#ifdef HAVE_SQLCONNECTW
		if (col[i].type==-9 || col[i].type==-8) {
			// bind varchar and char fields as wchar
			// bind the column to a buffer
			erg=SQLBindCol(stmt,i+1,SQL_C_WCHAR,
					field[i],MAX_ITEM_BUFFER_SIZE,
					#ifdef SQLBINDCOL_SQLLEN
					(SQLLEN *)&indicator[i]
					#else
					(SQLINTEGER *)&indicator[i]
					#endif
					);

		} else {
			// bind the column to a buffer
			if (col[i].type==93 || col[i].type==91) {
				erg=SQLBindCol(stmt,i+1,SQL_C_BINARY,
						field[i],MAX_ITEM_BUFFER_SIZE,
						#ifdef SQLBINDCOL_SQLLEN
						(SQLLEN *)&indicator[i]
						#else
						(SQLINTEGER *)&indicator[i]
						#endif
						);
			} else {
				erg=SQLBindCol(stmt,i+1,SQL_C_CHAR,
						field[i],MAX_ITEM_BUFFER_SIZE,
						#ifdef SQLBINDCOL_SQLLEN
						(SQLLEN *)&indicator[i]
						#else
						(SQLINTEGER *)&indicator[i]
						#endif
						);

			}

		}
#else
		erg=SQLBindCol(stmt,i+1,SQL_C_CHAR,
				field[i],MAX_ITEM_BUFFER_SIZE,
				#ifdef SQLBINDCOL_SQLLEN
				(SQLLEN *)&indicator[i]
				#else
				(SQLINTEGER *)&indicator[i]
				#endif
				);
#endif
		
		if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
			return false;
		}
	}

	return true;
}

void odbccursor::errorMessage(char *errorbuffer,
					uint32_t errorbufferlength,
					uint32_t *errorlength,
					int64_t *errorcode,
					bool *liveconnection) {
	SQLCHAR		state[10];
	SQLINTEGER	nativeerrnum;
	SQLSMALLINT	errlength;

	SQLGetDiagRec(SQL_HANDLE_STMT,stmt,1,state,&nativeerrnum,
				(SQLCHAR *)errorbuffer,errorbufferlength,
				&errlength);

	// set return values
	*errorlength=errlength;
	*errorcode=nativeerrnum;
	*liveconnection=true;
}

uint64_t odbccursor::affectedRows() {
	return affectedrows;
}

uint32_t odbccursor::colCount() {
	return ncols;
}

const char *odbccursor::getColumnName(uint32_t i) {
	return col[i].name;
}

uint16_t odbccursor::getColumnNameLength(uint32_t i) {
	return col[i].namelength;
}

uint16_t odbccursor::getColumnType(uint32_t i) {
	switch (col[i].type) {
		case SQL_BIGINT:
			return BIGINT_DATATYPE;
		case SQL_BINARY:
			return BINARY_DATATYPE;
		case SQL_BIT:
			return BIT_DATATYPE;
		case SQL_CHAR:
			return CHAR_DATATYPE;
		case SQL_TYPE_DATE:
			return DATE_DATATYPE;
		case SQL_DECIMAL:
			return DECIMAL_DATATYPE;
		case SQL_DOUBLE:
			return DOUBLE_DATATYPE;
		case SQL_FLOAT:
			return FLOAT_DATATYPE;
		case SQL_INTEGER:
			return INTEGER_DATATYPE;
		case SQL_LONGVARBINARY:
			return LONGVARBINARY_DATATYPE;
		case SQL_LONGVARCHAR:
			return LONGVARCHAR_DATATYPE;
		case SQL_NUMERIC:
			return NUMERIC_DATATYPE;
		case SQL_REAL:
			return REAL_DATATYPE;
		case SQL_SMALLINT:
			return SMALLINT_DATATYPE;
		case SQL_TYPE_TIME:
			return TIME_DATATYPE;
		case SQL_TYPE_TIMESTAMP:
			return TIMESTAMP_DATATYPE;
		case SQL_TINYINT:
			return TINYINT_DATATYPE;
		case SQL_VARBINARY:
			return VARBINARY_DATATYPE;
		case SQL_VARCHAR:
			return VARCHAR_DATATYPE;
		default:
			return UNKNOWN_DATATYPE;
	}
}

uint32_t odbccursor::getColumnLength(uint32_t i) {
	return col[i].length;
}

uint32_t odbccursor::getColumnPrecision(uint32_t i) {
	return col[i].precision;
}

uint32_t odbccursor::getColumnScale(uint32_t i) {
	return col[i].scale;
}

uint16_t odbccursor::getColumnIsNullable(uint32_t i) {
	return col[i].nullable;
}

uint16_t odbccursor::getColumnIsUnsigned(uint32_t i) {
	return col[i].unsignednumber;
}

uint16_t odbccursor::getColumnIsBinary(uint32_t i) {
	uint16_t	type=getColumnType(i);
	return (type==BINARY_DATATYPE ||
		type==LONGVARBINARY_DATATYPE ||
		type==VARBINARY_DATATYPE);
}

uint16_t odbccursor::getColumnIsAutoIncrement(uint32_t i) {
	return col[i].autoincrement;
}

bool odbccursor::noRowsToReturn() {
	// if there are no columns, then there can't be any rows either
	return (!ncols);
}

bool odbccursor::fetchRow() {

// this code is here in case unixodbc ever 
// successfully supports array fetches

/*#if (ODBCVER >= 0x0300)
	if (row==FETCH_AT_ONCE) {
		row=0;
	}
	if (row>0 && row==maxrow) {
		return false;
	}
	if (!row) {
		erg=SQLFetchScroll(stmt,SQL_FETCH_NEXT,0);
		if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
			return false;
		}
		SQLGetStmtAttr(stmt,SQL_ATTR_ROW_NUMBER,
				(SQLPOINTER)&rownumber,0,NULL);
		if (rownumber==totalrows) {
			return false;
		}
		maxrow=rownumber-totalrows;
		totalrows=rownumber;
	}
	return true;
#else*/
	erg=SQLFetch(stmt);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return false;
	}
	
#ifdef HAVE_SQLCONNECTW
	//convert char and varchar data to user coding from ucs-2
	for (int i=0; i<ncols; i++) {
		if (col[i].type==-9 || col[i].type==-8) {
			if (indicator[i]!=-1 && field[i]) {
				char *u=conv_to_user_coding(field[i]);
				int len=charstring::length(u);
				charstring::copy(field[i],u);
				indicator[i]=len;
				if (u) {
					delete[] u;
				}
			}
		}
	}
#endif

	return true;
//#endif
}

void odbccursor::getField(uint32_t col,
				const char **fld, uint64_t *fldlength,
				bool *blob, bool *null) {

// this code is here in case unixodbc ever 
// successfully supports array fetches

/*#if (ODBCVER >= 0x0300)

	// handle a null field
	if (indicator[col][row]==SQL_NULL_DATA) {
		*null=true;
		return;
	}

	// handle a non-null field
	*fld=field[col][row];
	*fldlength=indicator[col][row];
#else*/

	// handle a null field
	if (indicator[col]==SQL_NULL_DATA) {
		*null=true;
		return;
	}

	// handle a non-null field
	*fld=field[col];
	*fldlength=indicator[col];
//#endif
}

void odbccursor::nextRow() {

	// this code is here in case unixodbc ever 
	// successfully supports array fetches

	//row++;
}

void odbccursor::closeResultSet() {

	for (uint16_t i=0; i<conn->cont->cfgfl->getMaxBindCount(); i++) {
		delete outdatebind[i];
		outdatebind[i]=NULL;
	}
}

extern "C" {
	sqlrserverconnection *new_odbcconnection(sqlrservercontroller *cont) {
		return new odbcconnection(cont);
	}
}
