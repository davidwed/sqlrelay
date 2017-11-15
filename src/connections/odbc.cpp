// Copyright (c) 1999-2016  David Muse
// See the file COPYING for more information

// note that config.h must come first to avoid some macro redefinition warnings
#include <config.h>

// windows needs this and it doesn't appear to hurt on other platforms
#include <rudiments/private/winsock.h>

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
#include <rudiments/process.h>

#include <datatypes.h>
#include <defines.h>

#ifdef HAVE_IODBC
	#include <iodbcinst.h>
#endif

#define MAX_LOB_CHUNK_SIZE	2147483647

struct odbccolumn {
	char		name[4096];
	uint16_t	namelength;
#if (ODBCVER >= 0x0300) && defined(SQLCOLATTRIBUTE_SQLLEN)
	SQLLEN		type;
	SQLLEN		length;
	SQLLEN		precision;
	SQLLEN		scale;
	SQLLEN		nullable;
	SQLLEN		unsignednumber;
	SQLLEN		autoincrement;
#else
	SQLINTEGER	type;
	SQLINTEGER	length;
	SQLINTEGER	precision;
	SQLINTEGER	scale;
	SQLINTEGER	nullable;
	SQLINTEGER	unsignednumber;
	SQLINTEGER	autoincrement;
#endif
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

class SQLRSERVER_DLLSPEC odbccursor : public sqlrservercursor {
	friend class odbcconnection;
	private:
				odbccursor(sqlrserverconnection *conn,
							uint16_t id);
				~odbccursor();
		void		allocateResultSetBuffers(int32_t columncount);
		void		deallocateResultSetBuffers();
		bool		prepareQuery(const char *query,
						uint32_t length);
		bool		allocateStatementHandle();
		void		initializeColCounts();
		void		initializeRowCounts();
		bool		inputBind(const char *variable, 
						uint16_t variablesize,
						const char *value, 
						uint32_t valuesize,
						int16_t *isnull);
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
						bool isnegative,
						char *buffer,
						uint16_t buffersize,
						int16_t *isnull);
		bool		outputBind(const char *variable, 
						uint16_t variablesize,
						char *value, 
						uint32_t valuesize,
						int16_t *isnull);
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
						bool *isnegative,
						char *buffer,
						uint16_t buffersize,
						int16_t *isnull);
		int16_t		nonNullBindValue();
		int16_t		nullBindValue();
		bool		bindValueIsNull(uint16_t isnull);
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
		bool		getLobFieldLength(uint32_t col,
							uint64_t *length);
		bool		getLobFieldSegment(uint32_t col,
					char *buffer, uint64_t buffersize,
					uint64_t offset, uint64_t charstoread,
					uint64_t *charsread);
		void		closeResultSet();


		SQLRETURN	erg;
		SQLHSTMT	stmt;
		SQLSMALLINT	ncols;
		#ifdef SQLROWCOUNT_SQLLEN
		SQLLEN 		affectedrows;
		#else
		SQLINTEGER 	affectedrows;
		#endif

		int32_t		columncount;
		char		**field;
		#ifdef SQLBINDCOL_SQLLEN
		SQLLEN		*loblength;
		SQLLEN		*indicator;
		#else
		SQLINTEGER	*loblength;
		SQLINTEGER	*indicator;
		#endif
		odbccolumn 	*column;

		uint16_t	maxbindcount;
		datebind	**outdatebind;
		int16_t		**outisnullptr;
		#ifdef SQLBINDPARAMETER_SQLLEN
		SQLLEN		*outisnull;
		SQLLEN		sqlnulldata;
		#else
		SQLINTEGER	*outisnull;
		SQLINTEGER	sqlnulldata;
		#endif

		uint32_t	row;
		uint32_t	maxrow;
		uint32_t	totalrows;
		uint32_t	rownumber;

		stringbuffer	errormsg;

		odbcconnection	*odbcconn;
};

class SQLRSERVER_DLLSPEC odbcconnection : public sqlrserverconnection {
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
		bool		supportsAutoCommit();
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
		bool		getSchemaList(sqlrservercursor *cursor,
						const char *wild);
		bool		getTableList(sqlrservercursor *cursor,
						const char *wild);
		bool		getTableTypeList(sqlrservercursor *cursor,
						const char *wild);
		bool		isCurrentCatalog(const char *name);
		bool		getColumnList(sqlrservercursor *cursor,
						const char *table,
						const char *wild);
		bool		getPrimaryKeyList(sqlrservercursor *cursor,
						const char *table,
						const char *wild);
		bool		getKeyAndIndexList(sqlrservercursor *cursor,
						const char *table,
						const char *wild);
		bool		getProcedureBindAndColumnList(
						sqlrservercursor *cursor,
						const char *procedure,
						const char *wild);
		bool		getTypeInfoList(sqlrservercursor *cursor,
						const char *type,
						const char *wild);
		bool		getProcedureList(sqlrservercursor *cursor,
						const char *wild);
		const char	*selectDatabaseQuery();
		char		*getCurrentDatabase();
		char		*getCurrentSchema();
		bool		setIsolationLevel(const char *isolevel);

		SQLRETURN	erg;
		SQLHENV		env;
		SQLHDBC		dbc;

		const char	*dsn;
		uint64_t	timeout;

		const char	*identity;

		const char	*odbcversion;
		bool		mars;

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
	identity=NULL;
	odbcversion=NULL;
	mars=false;
}


void odbcconnection::handleConnectString() {

	sqlrserverconnection::handleConnectString();

	dsn=cont->getConnectStringValue("dsn");

	const char	*to=cont->getConnectStringValue("timeout");
	if (!charstring::length(to)) {
		// for back-compatibility
		timeout=5;
	} else {
		timeout=charstring::toInteger(to);
	}

	identity=cont->getConnectStringValue("identity");

	odbcversion=cont->getConnectStringValue("odbcversion");

	mars=!charstring::compare(cont->getConnectStringValue("mars"),"yes");

	// unixodbc doesn't support array fetches
	cont->setFetchAtOnce(1);
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

	if (!charstring::compare(odbcversion,"2")) {
		erg=SQLSetEnvAttr(env,SQL_ATTR_ODBC_VERSION,
					(void *)SQL_OV_ODBC2,0);
	#ifdef SQL_OV_ODBC3_80
	} else if (!charstring::compare(odbcversion,"3.8")) {
		erg=SQLSetEnvAttr(env,SQL_ATTR_ODBC_VERSION,
					(void *)SQL_OV_ODBC3_80,0);
	#endif
	} else {
		erg=SQLSetEnvAttr(env,SQL_ATTR_ODBC_VERSION,
					(void *)SQL_OV_ODBC3,0);
	}
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

	// enable SQL Server MARS, if configured to do so
	if (mars) {
		SQLSetConnectAttr(dbc,1224,(SQLPOINTER *)1,SQL_IS_UINTEGER);
	}

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

	// get the error message
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
	return (identity)?identity:"odbc";
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

bool odbcconnection::getDatabaseList(sqlrservercursor *cursor,
						const char *wild) {

	odbccursor	*odbccur=(odbccursor *)cursor;

	// allocate the statement handle
	if (!odbccur->allocateStatementHandle()) {
		return false;
	}

	// initialize column and row counts
	odbccur->initializeColCounts();
	odbccur->initializeRowCounts();

	// get the catalogs
	erg=SQLTables(odbccur->stmt,
			(SQLCHAR *)SQL_ALL_CATALOGS,SQL_NTS,
			(SQLCHAR *)"",SQL_NTS,
			(SQLCHAR *)"",SQL_NTS,
			(SQLCHAR *)"",SQL_NTS);
	bool	retval=(erg==SQL_SUCCESS || erg==SQL_SUCCESS_WITH_INFO);

	// parse the column information
	return (retval)?odbccur->handleColumns():false;
}

bool odbcconnection::getSchemaList(sqlrservercursor *cursor,
						const char *wild) {

	odbccursor	*odbccur=(odbccursor *)cursor;

	// allocate the statement handle
	if (!odbccur->allocateStatementHandle()) {
		return false;
	}

	// initialize column and row counts
	odbccur->initializeColCounts();
	odbccur->initializeRowCounts();

	// get the schemas
	erg=SQLTables(odbccur->stmt,
			(SQLCHAR *)"",SQL_NTS,
			(SQLCHAR *)SQL_ALL_SCHEMAS,SQL_NTS,
			(SQLCHAR *)"",SQL_NTS,
			(SQLCHAR *)"",SQL_NTS);
	bool	retval=(erg==SQL_SUCCESS || erg==SQL_SUCCESS_WITH_INFO);

	// parse the column information
	return (retval)?odbccur->handleColumns():false;
}

bool odbcconnection::getTableList(sqlrservercursor *cursor,
					const char *wild) {

	odbccursor	*odbccur=(odbccursor *)cursor;

	// allocate the statement handle
	if (!odbccur->allocateStatementHandle()) {
		return false;
	}

	// initialize column and row counts
	odbccur->initializeColCounts();
	odbccur->initializeRowCounts();

	// various buffers/pointers
	char		catalogbuffer[1024];
	const char	*catalog=NULL;
	char		schemabuffer[1024];
	const char	*schema="";
	const char	*table="";
	const char	*tabletype="TABLE,VIEW,SYNONYM,ALIAS";
	char		**tableparts=NULL;
	uint64_t	tablepartcount=0;

	// get the current catalog (instance)
	SQLINTEGER	cataloglen=0;
	if (SQLGetConnectAttr(dbc,
				SQL_CURRENT_QUALIFIER,
				catalogbuffer,
				sizeof(catalogbuffer),
				&cataloglen)==SQL_SUCCESS) {
		catalogbuffer[cataloglen]='\0';
		catalog=catalogbuffer;
	}

	// get the current user (schema)
	SQLSMALLINT	schemalen=0;
	if (SQLGetInfo(dbc,
			SQL_USER_NAME,
			schemabuffer,
			sizeof(schemabuffer),
			&schemalen)==SQL_SUCCESS) {
		schemabuffer[schemalen]='\0';
		schema=schemabuffer;
	}

	// get the table name (or % for all tables)
	if (charstring::isNullOrEmpty(wild)) {

		// FIXME: should this be SQL_ALL_TABLES?
		table="%";

	} else {

		// the table name might be in one
		// of the following formats:
		// * table
		// * schema.table
		// * catalog.schema.table
		charstring::split(wild,".",true,
				&tableparts,&tablepartcount);

		// reset schema and catalog if necessary
		switch (tablepartcount) {
			case 3:
				catalog=tableparts[0];
				schema=tableparts[1];
				table=tableparts[2];
				break;
			case 2:
				// If there are 2 parts the it could
				// mean:
				// * catalog(.defaultschama).table
				//   or
				// * (currentcatalog.)schema.table...
				// If the first part is not the same as
				// the current catalog, then we'll
				// guess (currentcatalog.)schema.table,
				// but we don't really know for sure.
				// The app may really mean to target
				// another catalog.
				if (charstring::compare(
						tableparts[0],
						catalogbuffer)) {
					schema=tableparts[0];
				}
				table=tableparts[1];
				break;
			case 1:
				table=tableparts[0];
				break;
		}
	}

	// get the table list
	erg=SQLTables(odbccur->stmt,
			(SQLCHAR *)catalog,SQL_NTS,
			(SQLCHAR *)schema,SQL_NTS,
			(SQLCHAR *)table,SQL_NTS,
			(SQLCHAR *)tabletype,SQL_NTS);
	bool	retval=(erg==SQL_SUCCESS || erg==SQL_SUCCESS_WITH_INFO);

	// clean up
	for (uint64_t i=0; i<tablepartcount; i++) {
		delete[] tableparts[i];
	}
	delete[] tableparts;

	// parse the column information
	return (retval)?odbccur->handleColumns():false;
}

bool odbcconnection::getTableTypeList(sqlrservercursor *cursor,
					const char *wild) {

	odbccursor	*odbccur=(odbccursor *)cursor;

	// allocate the statement handle
	if (!odbccur->allocateStatementHandle()) {
		return false;
	}

	// initialize column and row counts
	odbccur->initializeColCounts();
	odbccur->initializeRowCounts();

	// get the table types
	erg=SQLTables(odbccur->stmt,
			(SQLCHAR *)"",SQL_NTS,
			(SQLCHAR *)"",SQL_NTS,
			(SQLCHAR *)"",SQL_NTS,
			(SQLCHAR *)SQL_ALL_TABLE_TYPES,SQL_NTS);
	bool	retval=(erg==SQL_SUCCESS || erg==SQL_SUCCESS_WITH_INFO);

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

	// initialize column and row counts
	odbccur->initializeColCounts();
	odbccur->initializeRowCounts();

	// various buffers/pointers
	char		catalogbuffer[1024];
	const char	*catalog=NULL;
	char		schemabuffer[1024];
	const char	*schema="";
	const char	*tablename="";
	char		**tableparts=NULL;
	uint64_t	tablepartcount=0;

	// get the current catalog (instance)
	SQLINTEGER	cataloglen=0;
	if (SQLGetConnectAttr(dbc,
				SQL_CURRENT_QUALIFIER,
				catalogbuffer,
				sizeof(catalogbuffer),
				&cataloglen)==SQL_SUCCESS) {
		catalogbuffer[cataloglen]='\0';
		catalog=catalogbuffer;
	}

	// get the current user (schema)
	SQLSMALLINT	schemalen=0;
	if (SQLGetInfo(dbc,
			SQL_USER_NAME,
			schemabuffer,
			sizeof(schemabuffer),
			&schemalen)==SQL_SUCCESS) {
		schemabuffer[schemalen]='\0';
		schema=schemabuffer;
	}

	// the table name might be in one
	// of the following formats:
	// * table
	// * schema.table
	// * catalog.schema.table
	charstring::split(table,".",true,&tableparts,&tablepartcount);

	// reset schema and catalog if necessary
	switch (tablepartcount) {
		case 3:
			catalog=tableparts[0];
			schema=tableparts[1];
			tablename=tableparts[2];
			break;
		case 2:
			// If there are 2 parts the it could mean:
			// * catalog(.defaultschama).table
			//   or
			// * (currentcatalog.)schema.table...
			// If the first part is not the same as the current
			// catalog, then we'll guess
			// (currentcatalog.)schema.table, but we don't really
			// know for sure. The app may really mean to target
			// another catalog.
			if (charstring::compare(tableparts[0],catalogbuffer)) {
				schema=tableparts[0];
			}
			tablename=tableparts[1];
			break;
		case 1:
			tablename=tableparts[0];
			break;
	}

	// use % if wild was empty
	wild=(!charstring::isNullOrEmpty(wild))?wild:"%";
		
	// get the column list
	erg=SQLColumns(odbccur->stmt,
			(SQLCHAR *)catalog,SQL_NTS,
			(SQLCHAR *)schema,SQL_NTS,
			(SQLCHAR *)tablename,SQL_NTS,
			(SQLCHAR *)wild,SQL_NTS);
	bool	retval=(erg==SQL_SUCCESS || erg==SQL_SUCCESS_WITH_INFO);

	// clean up
	for (uint64_t i=0; i<tablepartcount; i++) {
		delete[] tableparts[i];
	}
	delete[] tableparts;

	// parse the column information
	return (retval)?odbccur->handleColumns():false;
}

bool odbcconnection::getPrimaryKeyList(sqlrservercursor *cursor,
						const char *table,
						const char *wild) {

	odbccursor	*odbccur=(odbccursor *)cursor;

	// allocate the statement handle
	if (!odbccur->allocateStatementHandle()) {
		return false;
	}

	// initialize column and row counts
	odbccur->initializeColCounts();
	odbccur->initializeRowCounts();

	// various buffers/pointers
	char		catalogbuffer[1024];
	const char	*catalog=NULL;
	char		schemabuffer[1024];
	const char	*schema="";
	const char	*tablename="";
	char		**tableparts=NULL;
	uint64_t	tablepartcount=0;

	// get the current catalog (instance)
	SQLINTEGER	cataloglen=0;
	if (SQLGetConnectAttr(dbc,
				SQL_CURRENT_QUALIFIER,
				catalogbuffer,
				sizeof(catalogbuffer),
				&cataloglen)==SQL_SUCCESS) {
		catalogbuffer[cataloglen]='\0';
		catalog=catalogbuffer;
	}

	// get the current user (schema)
	SQLSMALLINT	schemalen=0;
	if (SQLGetInfo(dbc,
			SQL_USER_NAME,
			schemabuffer,
			sizeof(schemabuffer),
			&schemalen)==SQL_SUCCESS) {
		schemabuffer[schemalen]='\0';
		schema=schemabuffer;
	}

	// the table name might be in one
	// of the following formats:
	// * table
	// * schema.table
	// * catalog.schema.table
	charstring::split(table,".",true,&tableparts,&tablepartcount);

	// reset schema and catalog if necessary
	switch (tablepartcount) {
		case 3:
			catalog=tableparts[0];
			schema=tableparts[1];
			tablename=tableparts[2];
			break;
		case 2:
			// If there are 2 parts the it could mean:
			// * catalog(.defaultschama).table
			//   or
			// * (currentcatalog.)schema.table...
			// If the first part is not the same as the current
			// catalog, then we'll guess
			// (currentcatalog.)schema.table, but we don't really
			// know for sure. The app may really mean to target
			// another catalog.
			if (charstring::compare(tableparts[0],catalogbuffer)) {
				schema=tableparts[0];
			}
			tablename=tableparts[1];
			break;
		case 1:
			tablename=tableparts[0];
			break;
	}

	// get the primary key list
	erg=SQLPrimaryKeys(odbccur->stmt,
			(SQLCHAR *)catalog,SQL_NTS,
			(SQLCHAR *)schema,SQL_NTS,
			(SQLCHAR *)tablename,SQL_NTS);
	bool	retval=(erg==SQL_SUCCESS || erg==SQL_SUCCESS_WITH_INFO);

	// clean up
	for (uint64_t i=0; i<tablepartcount; i++) {
		delete[] tableparts[i];
	}
	delete[] tableparts;

	// parse the column information
	return (retval)?odbccur->handleColumns():false;
}

bool odbcconnection::getKeyAndIndexList(sqlrservercursor *cursor,
						const char *table,
						const char *wild) {

	odbccursor	*odbccur=(odbccursor *)cursor;

	// allocate the statement handle
	if (!odbccur->allocateStatementHandle()) {
		return false;
	}

	// initialize column and row counts
	odbccur->initializeColCounts();
	odbccur->initializeRowCounts();

	// various buffers/pointers
	char		catalogbuffer[1024];
	const char	*catalog=NULL;
	char		schemabuffer[1024];
	const char	*schema="";
	const char	*tablename="";
	char		**tableparts=NULL;
	uint64_t	tablepartcount=0;

	// get the current catalog (instance)
	SQLINTEGER	cataloglen=0;
	if (SQLGetConnectAttr(dbc,
				SQL_CURRENT_QUALIFIER,
				catalogbuffer,
				sizeof(catalogbuffer),
				&cataloglen)==SQL_SUCCESS) {
		catalogbuffer[cataloglen]='\0';
		catalog=catalogbuffer;
	}

	// get the current user (schema)
	SQLSMALLINT	schemalen=0;
	if (SQLGetInfo(dbc,
			SQL_USER_NAME,
			schemabuffer,
			sizeof(schemabuffer),
			&schemalen)==SQL_SUCCESS) {
		schemabuffer[schemalen]='\0';
		schema=schemabuffer;
	}

	// the table name might be in one
	// of the following formats:
	// * table
	// * schema.table
	// * catalog.schema.table
	charstring::split(table,".",true,&tableparts,&tablepartcount);

	// reset schema and catalog if necessary
	switch (tablepartcount) {
		case 3:
			catalog=tableparts[0];
			schema=tableparts[1];
			tablename=tableparts[2];
			break;
		case 2:
			// If there are 2 parts the it could mean:
			// * catalog(.defaultschama).table
			//   or
			// * (currentcatalog.)schema.table...
			// If the first part is not the same as the current
			// catalog, then we'll guess
			// (currentcatalog.)schema.table, but we don't really
			// know for sure. The app may really mean to target
			// another catalog.
			if (charstring::compare(tableparts[0],catalogbuffer)) {
				schema=tableparts[0];
			}
			tablename=tableparts[1];
			break;
		case 1:
			tablename=tableparts[0];
			break;
	}

	// set uniqueness
	SQLUSMALLINT	uniqueness=SQL_INDEX_UNIQUE;
	if (charstring::contains(wild,"all")) {
		uniqueness=SQL_INDEX_ALL;
	}

	// set accuracy
	SQLUSMALLINT	accuracy=SQL_QUICK;
	if (charstring::contains(wild,"ensure")) {
		accuracy=SQL_ENSURE;
	}

	// get the primary key list
	erg=SQLStatistics(odbccur->stmt,
			(SQLCHAR *)catalog,SQL_NTS,
			(SQLCHAR *)schema,SQL_NTS,
			(SQLCHAR *)tablename,SQL_NTS,
			uniqueness,
			accuracy);
	bool	retval=(erg==SQL_SUCCESS || erg==SQL_SUCCESS_WITH_INFO);

	// clean up
	for (uint64_t i=0; i<tablepartcount; i++) {
		delete[] tableparts[i];
	}
	delete[] tableparts;

	// parse the column information
	return (retval)?odbccur->handleColumns():false;
}

bool odbcconnection::getProcedureBindAndColumnList(
					sqlrservercursor *cursor,
					const char *procedure,
					const char *wild) {

	odbccursor	*odbccur=(odbccursor *)cursor;

	// allocate the statement handle
	if (!odbccur->allocateStatementHandle()) {
		return false;
	}

	// initialize column and row counts
	odbccur->initializeColCounts();
	odbccur->initializeRowCounts();

	// Unlike SQLColumns/SQLTables, SQLProcedureColumns wants NULL instead
	// of "" for catalog/schema, to indicate the current catalog/schema.
	// It interprets "" as meaning outside of any catalog/schema.
	const char	*catalog=NULL;
	const char	*schema=NULL;
	const char	*proc=NULL;

	// get the current catalog (instance)
	char		catalogbuffer[1024];
	SQLINTEGER	cataloglen=0;
	if (SQLGetConnectAttr(dbc,
				SQL_CURRENT_QUALIFIER,
				catalogbuffer,
				sizeof(catalogbuffer),
				&cataloglen)==SQL_SUCCESS) {
		catalogbuffer[cataloglen]='\0';
	}

	// split the procedure name and extract the parts
	char		**procparts=NULL;
	uint64_t	procpartcount=0;
	charstring::split(procedure,".",true,&procparts,&procpartcount);
	switch (procpartcount) {
		case 3:
			catalog=procparts[0];
			schema=procparts[1];
			proc=procparts[2];
			break;
		case 2:
			// If there are 2 parts the it could mean:
			// * catalog(.defaultschama).proc
			//   or
			// * (currentcatalog.)schema.proc...
			// If the first part is not the same as the
			// current catalog, then we'll guess
			// (currentcatalog.)schema.proc, but we don't really
			// know for sure.  The app may really mean to target
			// another catalog.
			if (charstring::compare(procparts[0],
						catalogbuffer)) {
				schema=procparts[0];
			}
			proc=procparts[1];

			// NOTE: Delphi was passing catalog.proc at one point,
			// and we were assuming that instead.
			//catalog=procparts[0];
			break;
		case 1:
			proc=procparts[0];
			break;
	}

	// SQLProcedureColumns takes non-const arguments, so we have to make
	// a copy of the wild parameter.
	char	*wildcopy=charstring::duplicate(wild);

	// SQLColumns interprets an empty or NULL column name as meaning
	// "all columns".  SQLProcedureColumns interprtes an empty column name
	// as meaning "no columns" and a NULL as meaning "all columns".  At
	// least with the MS SQL Server driver.  For consistency, we'll make
	// empty work the same as NULL by mapping empty to NULL here.
	if (wildcopy[0]=='\0') {
		delete[] wildcopy;
		wildcopy=NULL;
	}

	// get the column list
	erg=SQLProcedureColumns(odbccur->stmt,
			(SQLCHAR *)catalog,
			charstring::length(catalog),
			(SQLCHAR *)schema,
			charstring::length(schema),
			(SQLCHAR *)proc,
			charstring::length(proc),
			(SQLCHAR *)wildcopy,
			charstring::length(wildcopy)
			);
	bool	retval=(erg==SQL_SUCCESS || erg==SQL_SUCCESS_WITH_INFO);

	// clean up
	delete[] wildcopy;
	for (uint64_t i=0; i<procpartcount; i++) {
		delete[] procparts[i];
	}
	delete[] procparts;

	// parse the column information
	return (retval)?odbccur->handleColumns():false;
}

bool odbcconnection::getTypeInfoList(sqlrservercursor *cursor,
					const char *type,
					const char *wild) {

	odbccursor	*odbccur=(odbccursor *)cursor;

	// allocate the statement handle
	if (!odbccur->allocateStatementHandle()) {
		return false;
	}

	// initialize column and row counts
	odbccur->initializeColCounts();
	odbccur->initializeRowCounts();

	// map the string type to a number
	// FIXME: this will be slooowwww... improve it
	SQLSMALLINT	typenumber=-200;
	if (!charstring::compareIgnoringCase(type,"CHAR")) {
		typenumber=SQL_CHAR;
	} else if (!charstring::compareIgnoringCase(type,"VARCHAR")) {
		typenumber=SQL_VARCHAR;
	} else if (!charstring::compareIgnoringCase(type,"LONGVARCHAR")) {
		typenumber=SQL_LONGVARCHAR;
	} else if (!charstring::compareIgnoringCase(type,"WCHAR")) {
		typenumber=SQL_WCHAR;
	} else if (!charstring::compareIgnoringCase(type,"WVARCHAR")) {
		typenumber=SQL_WVARCHAR;
	} else if (!charstring::compareIgnoringCase(type,"WLONGVARCHAR")) {
		typenumber=SQL_WLONGVARCHAR;
	} else if (!charstring::compareIgnoringCase(type,"DECIMAL")) {
		typenumber=SQL_DECIMAL;
	} else if (!charstring::compareIgnoringCase(type,"NUMERIC")) {
		typenumber=SQL_NUMERIC;
	} else if (!charstring::compareIgnoringCase(type,"SMALLINT")) {
		typenumber=SQL_SMALLINT;
	} else if (!charstring::compareIgnoringCase(type,"INTEGER")) {
		typenumber=SQL_INTEGER;
	} else if (!charstring::compareIgnoringCase(type,"REAL")) {
		typenumber=SQL_REAL;
	} else if (!charstring::compareIgnoringCase(type,"FLOAT")) {
		typenumber=SQL_FLOAT;
	} else if (!charstring::compareIgnoringCase(type,"DOUBLE")) {
		typenumber=SQL_DOUBLE;
	} else if (!charstring::compareIgnoringCase(type,"DATE")) {
		typenumber=SQL_DATE;
	} else if (!charstring::compareIgnoringCase(type,"TIME")) {
		typenumber=SQL_TIME;
	} else if (!charstring::compareIgnoringCase(type,"TIMESTAMP")) {
		typenumber=SQL_TIMESTAMP;
	} else if (!charstring::compareIgnoringCase(type,"BIT")) {
		typenumber=SQL_BIT;
	} else if (!charstring::compareIgnoringCase(type,"TINYINT")) {
		typenumber=SQL_TINYINT;
	} else if (!charstring::compareIgnoringCase(type,"BIGINT")) {
		typenumber=SQL_BIGINT;
	} else if (!charstring::compareIgnoringCase(type,"BINARY")) {
		typenumber=SQL_BINARY;
	} else if (!charstring::compareIgnoringCase(type,"VARBINARY")) {
		typenumber=SQL_VARBINARY;
	} else if (!charstring::compareIgnoringCase(type,"LONGVARBINARY")) {
		typenumber=SQL_LONGVARBINARY;
	} else if (!charstring::compareIgnoringCase(type,"TYPE_DATE")) {
		typenumber=SQL_TYPE_DATE;
	} else if (!charstring::compareIgnoringCase(type,"TYPE_TIME")) {
		typenumber=SQL_TYPE_TIME;
	} else if (!charstring::compareIgnoringCase(type,"TYPE_TIMESTAMP")) {
		typenumber=SQL_TYPE_TIMESTAMP;
	#ifdef SQL_TYPE_UTCDATETIME
	} else if (!charstring::compareIgnoringCase(type,"TYPE_UTCDATETIME")) {
		typenumber=SQL_TYPE_UTCDATETIME;
	#endif
	#ifdef SQL_TYPE_UTCTIME
	} else if (!charstring::compareIgnoringCase(type,"TYPE_UCTTIME")) {
		typenumber=SQL_TYPE_UTCTIME;
	#endif
	} else if (!charstring::compareIgnoringCase(type,"INTERVAL_MONTH")) {
		typenumber=SQL_INTERVAL_MONTH;
	} else if (!charstring::compareIgnoringCase(type,"INTERVAL_YEAR")) {
		typenumber=SQL_INTERVAL_YEAR;
	} else if (!charstring::compareIgnoringCase(
					type,"INTERVAL_YEAR_TO_MONTH")) {
		typenumber=SQL_INTERVAL_YEAR_TO_MONTH;
	} else if (!charstring::compareIgnoringCase(type,"INTERVAL_DAY")) {
		typenumber=SQL_INTERVAL_DAY;
	} else if (!charstring::compareIgnoringCase(type,"INTERVAL_HOUR")) {
		typenumber=SQL_INTERVAL_HOUR;
	} else if (!charstring::compareIgnoringCase(type,"INTERVAL_MINUTE")) {
		typenumber=SQL_INTERVAL_MINUTE;
	} else if (!charstring::compareIgnoringCase(type,"INTERVAL_SECOND")) {
		typenumber=SQL_INTERVAL_SECOND;
	} else if (!charstring::compareIgnoringCase(
					type,"INTERVAL_DAY_TO_HOUR")) {
		typenumber=SQL_INTERVAL_DAY_TO_HOUR;
	} else if (!charstring::compareIgnoringCase(
					type,"INTERVAL_DAY_TO_MINUTE")) {
		typenumber=SQL_INTERVAL_DAY_TO_MINUTE;
	} else if (!charstring::compareIgnoringCase(
					type,"INTERVAL_DAY_TO_SECOND")) {
		typenumber=SQL_INTERVAL_DAY_TO_SECOND;
	} else if (!charstring::compareIgnoringCase(
					type,"INTERVAL_HOUR_TO_MINUTE")) {
		typenumber=SQL_INTERVAL_HOUR_TO_MINUTE;
	} else if (!charstring::compareIgnoringCase(
					type,"INTERVAL_HOUR_TO_SECOND")) {
		typenumber=SQL_INTERVAL_HOUR_TO_SECOND;
	} else if (!charstring::compareIgnoringCase(
					type,"INTERVAL_MINUTE_TO_SECOND")) {
		typenumber=SQL_INTERVAL_MINUTE_TO_SECOND;
	} else if (!charstring::compareIgnoringCase(type,"GUID")) {
		typenumber=SQL_GUID;
	} else if (!charstring::compareIgnoringCase(type,"*")) {
		typenumber=SQL_ALL_TYPES;
	}

	// remap date/time types to the appropriate odbc2/3 type,
	// just in case the client isn't well-behaved
	if (!charstring::compare(odbcversion,"2")) {
		switch (typenumber) {
			case SQL_TYPE_DATE:
				typenumber=SQL_DATE;
				break;
			case SQL_TYPE_TIME:
				typenumber=SQL_TIME;
				break;
			case SQL_TYPE_TIMESTAMP:
				typenumber=SQL_TIMESTAMP;
				break;
		}
	} else {
		switch (typenumber) {
			case SQL_DATE:
				typenumber=SQL_TYPE_DATE;
				break;
			case SQL_TIME:
				typenumber=SQL_TYPE_TIME;
				break;
			case SQL_TIMESTAMP:
				typenumber=SQL_TYPE_TIMESTAMP;
				break;
		}
	}

	// get the type list
	erg=SQLGetTypeInfo(odbccur->stmt,typenumber);
	bool	retval=(erg==SQL_SUCCESS || erg==SQL_SUCCESS_WITH_INFO);

	// parse the column information
	return (retval)?odbccur->handleColumns():false;
}

bool odbcconnection::getProcedureList(sqlrservercursor *cursor,
						const char *wild) {

	odbccursor	*odbccur=(odbccursor *)cursor;

	// allocate the statement handle
	if (!odbccur->allocateStatementHandle()) {
		return false;
	}

	// initialize column and row counts
	odbccur->initializeColCounts();
	odbccur->initializeRowCounts();

	// get the procedure list
	char		catalogbuffer[1024];
	const char	*catalog=NULL;
	char		schemabuffer[1024];
	const char	*schema="";
	const char	*procname="";
	char		**procparts=NULL;
	uint64_t	procpartcount=0;

	// get the current catalog (instance)
	SQLINTEGER	cataloglen=0;
	if (SQLGetConnectAttr(dbc,
			SQL_CURRENT_QUALIFIER,
			catalogbuffer,
			sizeof(catalogbuffer),
			&cataloglen)==SQL_SUCCESS) {
		catalogbuffer[cataloglen]='\0';
		catalog=catalogbuffer;
	}

	// get the current user (schema)
	SQLSMALLINT	schemalen=0;
	if (SQLGetInfo(dbc,
			SQL_USER_NAME,
			schemabuffer,
			sizeof(schemabuffer),
			&schemalen)==SQL_SUCCESS) {
		schemabuffer[schemalen]='\0';
		schema=schemabuffer;
	}

	// get the procedure name (or % for all procedures)
	if (charstring::isNullOrEmpty(wild)) {

		procname="%";

	} else {

		// the procedure name might be in one
		// of the following formats:
		// * procedure
		// * schema.procedure
		// * catalog.schema.procedure
		charstring::split(wild,".",true,
				&procparts,&procpartcount);

		// reset schema and catalog if necessary
		switch (procpartcount) {
			case 3:
				catalog=procparts[0];
				schema=procparts[1];
				procname=procparts[2];
				break;
			case 2:
				// If there are 2 parts the it could mean:
				// * catalog(.defaultschama).proc
				//   or
				// * (currentcatalog.)schema.proc...
				// If the first part is not the same as the
				// current catalog, then we'll guess
				// (currentcatalog.)schema.proc, but we don't
				// really know for sure.  The app may really
				// mean to target another catalog.
				if (charstring::compare(procparts[0],
							catalogbuffer)) {
					schema=procparts[0];
				}
				procname=procparts[1];
				break;
			case 1:
				procname=procparts[0];
				break;
		}
	}

	// get the procedure list
	erg=SQLProcedures(odbccur->stmt,
			(SQLCHAR *)catalog,SQL_NTS,
			(SQLCHAR *)schema,SQL_NTS,
			(SQLCHAR *)procname,SQL_NTS);
	bool	retval=(erg==SQL_SUCCESS || erg==SQL_SUCCESS_WITH_INFO);

	// clean up
	for (uint64_t i=0; i<procpartcount; i++) {
		delete[] procparts[i];
	}
	delete[] procparts;

	// parse the column information
	return (retval)?odbccur->handleColumns():false;
}

const char *odbcconnection::selectDatabaseQuery() {
	// FIXME: this won't work with every database
	return "use %s";
}

char *odbcconnection::getCurrentDatabase() {
	char	*currentdb=new char[256];
	SQLSMALLINT	currentdblen;
	SQLGetInfo(dbc,SQL_DATABASE_NAME,
			(SQLPOINTER)currentdb,
			(SQLSMALLINT)256,
			&currentdblen);
	return currentdb;
}

char *odbcconnection::getCurrentSchema() {
	char	*currentschema=new char[256];
	SQLSMALLINT	currentschemalen;
	SQLGetInfo(dbc,SQL_USER_NAME,
			(SQLPOINTER)currentschema,
			(SQLSMALLINT)256,
			&currentschemalen);
	return currentschema;
}

#if (ODBCVER >= 0x0300)
bool odbcconnection::autoCommitOn() {
	// FIXME: I'm not sure this is necessary for non-sqlserver/sap/sybase
	cont->closeAllResultSets();
	erg=SQLSetConnectAttr(dbc,SQL_ATTR_AUTOCOMMIT,
				(SQLPOINTER)SQL_AUTOCOMMIT_ON,
				sizeof(SQLINTEGER));
	return (erg==SQL_SUCCESS || erg==SQL_SUCCESS_WITH_INFO);
}

bool odbcconnection::autoCommitOff() {
	// FIXME: I'm not sure this is necessary for non-sqlserver/sap/sybase
	cont->closeAllResultSets();
	erg=SQLSetConnectAttr(dbc,SQL_ATTR_AUTOCOMMIT,
				(SQLPOINTER)SQL_AUTOCOMMIT_OFF,
				sizeof(SQLINTEGER));
	return (erg==SQL_SUCCESS || erg==SQL_SUCCESS_WITH_INFO);
}

bool odbcconnection::supportsAutoCommit() {
	return true;
}

bool odbcconnection::commit() {
	// FIXME: I'm not sure this is necessary for non-sqlserver/sap/sybase
	cont->closeAllResultSets();
	return (SQLEndTran(SQL_HANDLE_ENV,env,SQL_COMMIT)==SQL_SUCCESS);
}

bool odbcconnection::rollback() {
	// FIXME: I'm not sure this is necessary for non-sqlserver/sap/sybase
	cont->closeAllResultSets();
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
	maxbindcount=conn->cont->getConfig()->getMaxBindCount();
	outdatebind=new datebind *[maxbindcount];
	outisnullptr=new int16_t *[maxbindcount];
	#ifdef SQLBINDPARAMETER_SQLLEN
	outisnull=new SQLLEN[maxbindcount];
	#else
	outisnull=new SQLINTEGER[maxbindcount];
	#endif
	for (uint16_t i=0; i<maxbindcount; i++) {
		outdatebind[i]=NULL;
		outisnullptr[i]=NULL;
		outisnull[i]=0;
	}
	sqlnulldata=SQL_NULL_DATA;
	allocateResultSetBuffers(conn->cont->getMaxColumnCount());
	initializeColCounts();
	initializeRowCounts();
}

odbccursor::~odbccursor() {
	delete[] outdatebind;
	delete[] outisnullptr;
	delete[] outisnull;
	deallocateResultSetBuffers();
}

void odbccursor::allocateResultSetBuffers(int32_t columncount) {

	if (!columncount) {
		this->columncount=0;
		field=NULL;
		loblength=NULL;
		indicator=NULL;
		column=NULL;
	} else {
		this->columncount=columncount;
		field=new char *[columncount];
		#ifdef SQLBINDCOL_SQLLEN
		loblength=new SQLLEN[columncount];
		indicator=new SQLLEN[columncount];
		#else
		loblength=new SQLINTEGER[columncount];
		indicator=new SQLINTEGER[columncount];
		#endif
		uint32_t	maxfieldlength=conn->cont->getMaxFieldLength();
		column=new odbccolumn[columncount];
		for (int32_t i=0; i<columncount; i++) {
			field[i]=new char[maxfieldlength];
		}
	}
}

void odbccursor::deallocateResultSetBuffers() {
	if (columncount) {
		for (int32_t i=0; i<columncount; i++) {
			delete[] field[i];
		}
		delete[] column;
		delete[] field;
		delete[] loblength;
		delete[] indicator;
		columncount=0;
	}
}

bool odbccursor::prepareQuery(const char *query, uint32_t length) {

	// initialize column count
	initializeColCounts();

	// allocate the statement handle
	if (!allocateStatementHandle()) {
		return false;
	}

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
	return (erg==SQL_SUCCESS || erg==SQL_SUCCESS_WITH_INFO);
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
				int16_t *isnull) {

	uint16_t	pos=charstring::toInteger(variable+1);
	if (!pos || pos>maxbindcount) {
		return false;
	}

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
				pos,
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
				&sqlnulldata);
	} else {

		// In ODBC-2 mode, SQL Server Native Client 11.0 (at least)
		// allows a valuesize of 0, when the value is "".
		// In non-ODBC-2 mode, it throws: "Invalid precision value"
		// Using a valuesize of 1 works with all ODBC-modes.
		// Hopefully it works with all drivers.
		if (!valuesize) {
			valuesize=1;
		}

		erg=SQLBindParameter(stmt,
				pos,
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
				NULL);
	}
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		stdoutput.printf("bind failed\n");
	}
	return (erg==SQL_SUCCESS || erg==SQL_SUCCESS_WITH_INFO);
}

bool odbccursor::inputBind(const char *variable,
				uint16_t variablesize,
				int64_t *value) {

	uint16_t	pos=charstring::toInteger(variable+1);
	if (!pos || pos>maxbindcount) {
		return false;
	}

	erg=SQLBindParameter(stmt,
				pos,
				SQL_PARAM_INPUT,
				SQL_C_SBIGINT,
				SQL_BIGINT,
				0,
				0,
				value,
				sizeof(int64_t),
				NULL);
	return (erg==SQL_SUCCESS || erg==SQL_SUCCESS_WITH_INFO);
}

bool odbccursor::inputBind(const char *variable,
				uint16_t variablesize,
				double *value,
				uint32_t precision,
				uint32_t scale) {

	uint16_t	pos=charstring::toInteger(variable+1);
	if (!pos || pos>maxbindcount) {
		return false;
	}

	erg=SQLBindParameter(stmt,
				pos,
				SQL_PARAM_INPUT,
				SQL_C_DOUBLE,
				SQL_DECIMAL,
				precision,
				scale,
				value,
				sizeof(double),
				NULL);
	return (erg==SQL_SUCCESS || erg==SQL_SUCCESS_WITH_INFO);
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
				bool isnegative,
				char *buffer,
				uint16_t buffersize,
				int16_t *isnull) {

	uint16_t	pos=charstring::toInteger(variable+1);
	if (!pos || pos>maxbindcount) {
		return false;
	}

	bool	validdate=(year>=0 && month>=0 && day>=0);
	bool	validtime=(hour>=0 && minute>=0 && second>=0 && microsecond>=0);

	if (validdate && !validtime) {

		SQL_DATE_STRUCT	*ts=(SQL_DATE_STRUCT *)buffer;
		ts->year=year;
		ts->month=month;
		ts->day=day;

		erg=SQLBindParameter(stmt,
				pos,
				SQL_PARAM_INPUT,
				SQL_C_DATE,
				SQL_DATE,
				0,
				0,
				buffer,
				0,
				NULL);
	} else {

		SQL_TIMESTAMP_STRUCT	*ts=(SQL_TIMESTAMP_STRUCT *)buffer;
		ts->year=year;
		ts->month=month;
		ts->day=day;
		ts->hour=hour;
		ts->minute=minute;
		ts->second=second;
		ts->fraction=microsecond*1000;

		// FIXME: this works with the SQL Server Native Client ODBC
		// drivers, but not the old "standard" SQL Server driver
		// (seconds and fractional seconds are truncated).  The web is
		// riddled with people trying to get it to work with the old
		// driver.  None of their solutions worked for me.  There is
		// probably some magic that will work.  I'll have to find it
		// some day.
		erg=SQLBindParameter(stmt,
				pos,
				SQL_PARAM_INPUT,
				SQL_C_TIMESTAMP,
				SQL_TIMESTAMP,
				0,
				// Here, decimal digits here refers to the max
				// digits in ts->fraction.  Since ts->fraction
				// represents billionths of a second
				// (0-999999999) in ODBC, decimal digits must
				// be 9 to accomodate the full range.
				9,
				buffer,
				0,
				NULL);
	}
	return (erg==SQL_SUCCESS || erg==SQL_SUCCESS_WITH_INFO);
}

bool odbccursor::outputBind(const char *variable, 
				uint16_t variablesize,
				char *value, 
				uint32_t valuesize, 
				int16_t *isnull) {

	uint16_t	pos=charstring::toInteger(variable+1);
	if (!pos || pos>maxbindcount) {
		return false;
	}

	outdatebind[pos-1]=NULL;
	outisnullptr[pos-1]=isnull;

	// HACK:
	// Pass in an empty string, rather than the NULL that would be passed
	// in if SQL_PARAM_OUTPUT were used.
	//
	// Most apps only use the output feature and don't care what is passed
	// in.  Most of the rest of the apps want an empty string to be passed
	// in.  The rest of the apps want a particular value, and they're out
	// of luck anyway.  We'll pass in an empty string to satisfy the largest
	// number of apps that we can.
	//
	// Ultimately, we need legitimate input/output bind support.
	bytestring::zero(value,valuesize);
	outisnull[pos-1]=0;

	erg=SQLBindParameter(stmt,
				pos,
				//SQL_PARAM_OUTPUT,
				SQL_PARAM_INPUT_OUTPUT,
				SQL_C_CHAR,
				SQL_VARCHAR,
				valuesize,
				0,
				(SQLPOINTER)value,
				valuesize,
				&(outisnull[pos-1]));
	return (erg==SQL_SUCCESS || erg==SQL_SUCCESS_WITH_INFO);
}

bool odbccursor::outputBind(const char *variable,
				uint16_t variablesize,
				int64_t *value,
				int16_t *isnull) {

	uint16_t	pos=charstring::toInteger(variable+1);
	if (!pos || pos>maxbindcount) {
		return false;
	}

	outdatebind[pos-1]=NULL;
	outisnullptr[pos-1]=isnull;

	*value=0;

	erg=SQLBindParameter(stmt,
				pos,
				SQL_PARAM_OUTPUT,
				SQL_C_SBIGINT,
				SQL_BIGINT,
				0,
				0,
				value,
				sizeof(int64_t),
				&(outisnull[pos-1]));
	return (erg==SQL_SUCCESS || erg==SQL_SUCCESS_WITH_INFO);
}

bool odbccursor::outputBind(const char *variable,
				uint16_t variablesize,
				double *value,
				uint32_t *precision,
				uint32_t *scale,
				int16_t *isnull) {

	uint16_t	pos=charstring::toInteger(variable+1);
	if (!pos || pos>maxbindcount) {
		return false;
	}

	outdatebind[pos-1]=NULL;
	outisnullptr[pos-1]=isnull;

	*value=0.0;

	erg=SQLBindParameter(stmt,
				pos,
				SQL_PARAM_OUTPUT,
				SQL_C_DOUBLE,
				SQL_DOUBLE,
				0,
				0,
				value,
				sizeof(double),
				&(outisnull[pos-1]));
	return (erg==SQL_SUCCESS || erg==SQL_SUCCESS_WITH_INFO);
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
				bool *isnegative,
				char *buffer,
				uint16_t buffersize,
				int16_t *isnull) {

	uint16_t	pos=charstring::toInteger(variable+1);
	if (!pos || pos>maxbindcount) {
		return false;
	}

	datebind	*db=new datebind;
	db->year=year;
	db->month=month;
	db->day=day;
	db->hour=hour;
	db->minute=minute;
	db->second=second;
	db->microsecond=microsecond;
	db->tz=tz;
	*isnegative=false;
	db->buffer=buffer;
	outdatebind[pos-1]=db;
	outisnullptr[pos-1]=isnull;

	erg=SQLBindParameter(stmt,
				pos,
				SQL_PARAM_OUTPUT,
				SQL_C_TIMESTAMP,
				SQL_TIMESTAMP,
				0,
				0,
				buffer,
				0,
				&(outisnull[pos-1]));
	return (erg==SQL_SUCCESS || erg==SQL_SUCCESS_WITH_INFO);
}

int16_t odbccursor::nonNullBindValue() {
	return 0;
}

int16_t odbccursor::nullBindValue() {
	return SQL_NULL_DATA;
}

bool odbccursor::bindValueIsNull(uint16_t isnull) {
	return (isnull==SQL_NULL_DATA);
}

bool odbccursor::executeQuery(const char *query, uint32_t length) {

	// initialize counts
	initializeRowCounts();

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
	erg=SQLRowCount(stmt,&affectedrows);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return false;
	}

	// convert date output binds and copy out isnulls
	for (uint16_t i=0; i<getOutputBindCount(); i++) {
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
		if (outisnullptr[i]) {
			*(outisnullptr[i])=outisnull[i];
		}
	}

	return true;
}

void odbccursor::initializeColCounts() {
	ncols=0;
}

void odbccursor::initializeRowCounts() {
	row=0;
	maxrow=0;
	totalrows=0;
	affectedrows=-1;
}

bool odbccursor::handleColumns() {

	// get the column count
	erg=SQLNumResultCols(stmt,&ncols);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return false;
	}

	// allocate buffers and limit column count if necessary
	if (!conn->cont->getMaxColumnCount()) {
		allocateResultSetBuffers(ncols);
	} else if ((uint32_t)ncols>conn->cont->getMaxColumnCount()) {
		ncols=conn->cont->getMaxColumnCount();
	}

	// run through the columns
	for (SQLSMALLINT i=0; i<ncols; i++) {

		if (conn->cont->getSendColumnInfo()==SEND_COLUMN_INFO) {
#if (ODBCVER >= 0x0300)
			// column name
			erg=SQLColAttribute(stmt,i+1,SQL_DESC_LABEL,
					column[i].name,4096,
					(SQLSMALLINT *)&(column[i].namelength),
					NULL);
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return false;
			}
			column[i].namelength=charstring::length(column[i].name);

			// column length
			erg=SQLColAttribute(stmt,i+1,SQL_DESC_LENGTH,
					NULL,0,NULL,
					&(column[i].length));
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return false;
			}
	
			// column type
			erg=SQLColAttribute(stmt,i+1,SQL_DESC_TYPE,
					NULL,0,NULL,
					&(column[i].type));
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return false;
			}

			// column precision
			erg=SQLColAttribute(stmt,i+1,SQL_DESC_PRECISION,
					NULL,0,NULL,
					&(column[i].precision));
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return false;
			}

			// column scale
			erg=SQLColAttribute(stmt,i+1,SQL_DESC_SCALE,
					NULL,0,NULL,
					&(column[i].scale));
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return false;
			}

			// column nullable
			erg=SQLColAttribute(stmt,i+1,SQL_DESC_NULLABLE,
					NULL,0,NULL,
					&(column[i].nullable));
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return false;
			}

			// primary key

			// unique

			// part of key

			// unsigned number
			erg=SQLColAttribute(stmt,i+1,SQL_DESC_UNSIGNED,
					NULL,0,NULL,
					&(column[i].unsignednumber));
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return false;
			}

			// zero fill

			// binary

			// autoincrement
			erg=SQLColAttribute(stmt,i+1,SQL_DESC_AUTO_UNIQUE_VALUE,
					NULL,0,NULL,
					&(column[i].autoincrement));
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return false;
			}
#else
			// column name
			erg=SQLColAttributes(stmt,i+1,SQL_COLUMN_LABEL,
					column[i].name,4096,
					(SQLSMALLINT *)&(column[i].namelength),
					NULL);
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return false;
			}
			// FIXME: above we reset namelength to length(name)...

			// column length
			erg=SQLColAttributes(stmt,i+1,SQL_COLUMN_LENGTH,
					NULL,0,NULL,
					&(column[i].length));
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return false;
			}

			// column type
			erg=SQLColAttributes(stmt,i+1,SQL_COLUMN_TYPE,
					NULL,0,NULL,
					&(column[i].type));
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return false;
			}

			// column precision
			erg=SQLColAttributes(stmt,i+1,SQL_COLUMN_PRECISION,
					NULL,0,NULL,
					&(column[i].precision));
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return false;
			}

			// column scale
			erg=SQLColAttributes(stmt,i+1,SQL_COLUMN_SCALE,
					NULL,0,NULL,
					&(column[i].scale));
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return false;
			}

			// column nullable
			erg=SQLColAttributes(stmt,i+1,SQL_COLUMN_NULLABLE,
					NULL,0,NULL,
					&(column[i].nullable));
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return false;
			}

			// primary key

			// unique

			// part of key

			// unsigned number
			erg=SQLColAttributes(stmt,i+1,SQL_COLUMN_UNSIGNED,
					NULL,0,NULL,
					&(column[i].unsignednumber));
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
					&(column[i].autoincrement));
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return false;
			}
			#else
			column[i].autoincrement=0;
			#endif
#endif
		}

		uint32_t	maxfieldlength=conn->cont->getMaxFieldLength();

		// bind the column to a buffer
		#ifdef HAVE_SQLCONNECTW
		if (column[i].type==-9 || column[i].type==-8) {
			// bind nvarchar and nchar fields as wchar
			erg=SQLBindCol(stmt,i+1,SQL_C_WCHAR,
					field[i],maxfieldlength,
					&indicator[i]);

		} else {
			// bind the column to a buffer
			if (column[i].type==93 || column[i].type==91) {
				erg=SQLBindCol(stmt,i+1,SQL_C_BINARY,
						field[i],maxfieldlength,
						&indicator[i]);
			} else {
				erg=SQLBindCol(stmt,i+1,SQL_C_CHAR,
						field[i],maxfieldlength,
						&indicator[i]);
			}

		}
		#else
		if (column[i].type!=SQL_LONGVARCHAR &&
			column[i].type!=SQL_LONGVARBINARY) {
			erg=SQLBindCol(stmt,i+1,SQL_C_CHAR,
					field[i],maxfieldlength,
					&indicator[i]);
		}
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
	return column[i].name;
}

uint16_t odbccursor::getColumnNameLength(uint32_t i) {
	return column[i].namelength;
}

uint16_t odbccursor::getColumnType(uint32_t i) {

	switch (column[i].type) {

		// generic types...
		case SQL_CHAR:
			return CHAR_DATATYPE;
		case SQL_VARCHAR:
			return VARCHAR_DATATYPE;
		case SQL_LONGVARCHAR:
			return LONGVARCHAR_DATATYPE;
		// FIXME:
		// case SQL_WCHAR:
		// FIXME:
		// case SQL_WVARCHAR:
		// FIXME:
		// case SQL_WLONGVARCHAR:
		case SQL_DECIMAL:
			return DECIMAL_DATATYPE;
		case SQL_NUMERIC:
			return NUMERIC_DATATYPE;
		case SQL_SMALLINT:
			return SMALLINT_DATATYPE;
		case SQL_INTEGER:
			return INTEGER_DATATYPE;
		case SQL_REAL:
			return REAL_DATATYPE;
		case SQL_FLOAT:
			return FLOAT_DATATYPE;
		case SQL_DOUBLE:
			return DOUBLE_DATATYPE;
		case SQL_DATE:
		//case SQL_DATETIME:
		//	(odbc 3 dup of SQL_DATE)
			// FIXME: need parameter indicating whether
			// to map this to date or datetime.  MySQL, for example,
			// may use SQL_DATE for dates and SQL_TIMESTAMP for
			// datetimes.
			return DATETIME_DATATYPE;
		case SQL_TIME:
		//case SQL_INTERVAL:
		//	(odbc 3 dup of SQL_TIME)
			return TIME_DATATYPE;
		case SQL_TIMESTAMP:
			return TIMESTAMP_DATATYPE;
		case SQL_BIT:
			return BIT_DATATYPE;
		case SQL_TINYINT:
			return TINYINT_DATATYPE;
		case SQL_BIGINT:
			return BIGINT_DATATYPE;
		case SQL_BINARY:
			return BINARY_DATATYPE;
		case SQL_VARBINARY:
			return VARBINARY_DATATYPE;
		case SQL_LONGVARBINARY:
			return LONGVARBINARY_DATATYPE;
		case SQL_TYPE_DATE:
			// FIXME: need parameter indicating whether
			// to map this to date or datetime.  MySQL, for example,
			// may use SQL_TYPE_DATE for dates and
			// SQL_TYPE_TIMESTAMP for datetimes.
			return DATETIME_DATATYPE;
		case SQL_TYPE_TIME:
			return TIME_DATATYPE;
		case SQL_TYPE_TIMESTAMP:
			return TIMESTAMP_DATATYPE;
		// FIXME:
		// #ifdef SQL_TYPE_UTCDATETIME
		//case SQL_TYPE_UTCDATETIME:
		// FIXME:
		// #ifdef SQL_TYPE_UTCTIME
		//case SQL_TYPE_UTCTIME:
		// FIXME:
		// interval types...
		case SQL_GUID:
			return UNIQUEIDENTIFIER_DATATYPE;

		// MS SQL Server types
		case -8:
			return NCHAR_DATATYPE;
		case -9:
			return NVARCHAR_DATATYPE;
		case -10:
			return NTEXT_DATATYPE;
		case -152:
			return XML_DATATYPE;
		case -154:
			return TIME_DATATYPE;
		case -155:
			return DATETIMEOFFSET_DATATYPE;

		default:
			return UNKNOWN_DATATYPE;
	}
}

uint32_t odbccursor::getColumnLength(uint32_t i) {
	return column[i].length;
}

uint32_t odbccursor::getColumnPrecision(uint32_t i) {
	return column[i].precision;
}

uint32_t odbccursor::getColumnScale(uint32_t i) {
	return column[i].scale;
}

uint16_t odbccursor::getColumnIsNullable(uint32_t i) {
	return column[i].nullable;
}

uint16_t odbccursor::getColumnIsUnsigned(uint32_t i) {
	return column[i].unsignednumber;
}

uint16_t odbccursor::getColumnIsBinary(uint32_t i) {
	uint16_t	type=getColumnType(i);
	return (type==BINARY_DATATYPE ||
		type==LONGVARBINARY_DATATYPE ||
		type==VARBINARY_DATATYPE);
}

uint16_t odbccursor::getColumnIsAutoIncrement(uint32_t i) {
	return column[i].autoincrement;
}

bool odbccursor::noRowsToReturn() {
	// if there are no columns, then there can't be any rows either
	return (!ncols);
}

bool odbccursor::fetchRow() {

	erg=SQLFetch(stmt);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return false;
	}
	
	#ifdef HAVE_SQLCONNECTW
	//convert char and varchar data to user coding from ucs-2
	for (int i=0; i<ncols; i++) {
		if (column[i].type==-9 || column[i].type==-8) {
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
}

void odbccursor::getField(uint32_t col,
				const char **fld, uint64_t *fldlength,
				bool *blob, bool *null) {

	// handle NULLs
	if (indicator[col]==SQL_NULL_DATA) {
		*null=true;
		return;
	}

	// handle lobs
	if (column[col].type==SQL_LONGVARCHAR ||
		column[col].type==SQL_LONGVARBINARY) {
		*blob=true;
		return;
	}

	// handle normal datatypes
	*fld=field[col];
	*fldlength=indicator[col];
}

bool odbccursor::getLobFieldLength(uint32_t col, uint64_t *length) {

	// get the length of the lob

	// a valid buffer must be provided, but it's ok to fetch 0 bytes into it
	SQLCHAR	buffer[1];
	erg=SQLGetData(stmt,col+1,SQL_C_BINARY,buffer,0,&(loblength[col]));
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return false;
	}

	// copy out the length
	*length=loblength[col];

	return true;
}

bool odbccursor::getLobFieldSegment(uint32_t col,
					char *buffer, uint64_t buffersize,
					uint64_t offset, uint64_t charstoread,
					uint64_t *charsread) {

	// bail if we're attempting to start reading past the end
	if (offset>(uint64_t)loblength[col]) {
		return false;
	}

	// prevent attempts to read past the end
	if (offset+charstoread>(uint64_t)loblength[col]) {
		charstoread=charstoread-((offset+charstoread)-loblength[col]);
	}

	// read a blob segment, at most MAX_LOB_CHUNK_SIZE bytes at a time
	uint64_t	totalbytesread=0;
	SQLLEN		bytestoread=0;
	uint64_t	remainingbytestoread=charstoread;
	for (;;) {

		// figure out how many bytes to read this time
		if (remainingbytestoread<MAX_LOB_CHUNK_SIZE) {
			bytestoread=remainingbytestoread;
		} else {
			bytestoread=MAX_LOB_CHUNK_SIZE;
			remainingbytestoread=remainingbytestoread-
						MAX_LOB_CHUNK_SIZE;
		}

		// read the bytes
		SQLLEN	ind=0;
		erg=SQLGetData(stmt,col+1,
				SQL_C_BINARY,
				buffer+totalbytesread,
				bytestoread,&ind);
		if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
			return false;
		}

		// determine how many bytes were read
		uint64_t	bytesread=
			(ind>=bytestoread || ind==SQL_NO_TOTAL)?bytestoread:ind;

		// update total bytes read
		totalbytesread=totalbytesread+bytesread;

		// bail if we're done reading
		if ((SQLUINTEGER)bytesread<bytestoread ||
				totalbytesread==charstoread) {
			break;
		}
	}

	// return number of bytes/chars read
	*charsread=totalbytesread;

	return true;
}

void odbccursor::closeResultSet() {
	//SQLCloseCursor(stmt);

	for (uint16_t i=0; i<getOutputBindCount(); i++) {
		delete outdatebind[i];
		outdatebind[i]=NULL;
		outisnullptr[i]=NULL;
		outisnull[i]=0;
	}

	if (!conn->cont->getMaxColumnCount()) {
		deallocateResultSetBuffers();
	}
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrserverconnection *new_odbcconnection(
						sqlrservercontroller *cont) {
		return new odbcconnection(cont);
	}
}
