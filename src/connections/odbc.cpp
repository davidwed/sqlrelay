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

#include <datatypes.h>
#include <defines.h>

#ifdef HAVE_IODBC
	#include <iodbcinst.h>
#endif

#define FETCH_AT_ONCE		10
#define MAX_COLUMN_COUNT	256
#define MAX_FIELD_LENGTH	32768

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

class SQLRSERVER_DLLSPEC odbccursor : public sqlrservercursor {
	friend class odbcconnection;
	private:
				odbccursor(sqlrserverconnection *conn,
							uint16_t id);
				~odbccursor();
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
		void		nextRow();
		void		closeResultSet();


		SQLRETURN	erg;
		SQLHSTMT	stmt;
		SQLSMALLINT	ncols;
		SQLINTEGER 	affectedrows;

// this code is here in case unixodbc ever 
// successfully supports array fetches

/*#ifdef HAVE_UNIXODBC
		char		field[MAX_COLUMN_COUNT]
					[FETCH_AT_ONCE]
					[MAX_FIELD_LENGTH];
		SQLINTEGER	indicator[MAX_COLUMN_COUNT]
						[FETCH_AT_ONCE];
#else*/
		char		field[MAX_COLUMN_COUNT]
					[MAX_FIELD_LENGTH];
		SQLINTEGER	indicator[MAX_COLUMN_COUNT];
//#endif
		odbccolumn 	col[MAX_COLUMN_COUNT];

		uint16_t	maxbindcount;
		datebind	**outdatebind;

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
		bool		getTableList(sqlrservercursor *cursor,
						const char *wild);
		bool		getDatabaseOrTableList(
						sqlrservercursor *cursor,
						const char *wild,
						bool table);
		bool		getColumnList(sqlrservercursor *cursor,
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
		bool		setIsolationLevel(const char *isolevel);

		SQLRETURN	erg;
		SQLHENV		env;
		SQLHDBC		dbc;

		const char	*dsn;
		uint64_t	timeout;

		const char	*identity;

		const char	*odbcversion;

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

	// unixodbc doesn't support array fetches
	cont->setFetchAtOnce(1);

	// this module doesn't support dynamic max-column-count/max-field-length
	cont->setMaxColumnCount(MAX_COLUMN_COUNT);
	cont->setMaxFieldLength(MAX_FIELD_LENGTH);
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
	return getDatabaseOrTableList(cursor,wild,false);
}

bool odbcconnection::getTableList(sqlrservercursor *cursor,
					const char *wild) {
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

	// initialize column and row counts
	odbccur->initializeColCounts();
	odbccur->initializeRowCounts();

	// get the table/database list
	char		catalogbuffer[1024];
	const char	*catalog=NULL;
	char		schemabuffer[1024];
	const char	*schema="";
	const char	*tablename="";
	const char	*tabletype=NULL;
	char		**tableparts=NULL;
	uint64_t	tablepartcount=0;

	if (table) {

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

			tablename="%";

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
					tablename=tableparts[2];
					break;
				case 2:
					// If there are 2 parts the it could
					// mean:
					// * catalog(.defaultschama).proc
					//   or
					// * (currentcatalog.)schema.proc...
					// We'll guess schema.proc, but we
					// don't know for sure.
					schema=tableparts[0];
					tablename=tableparts[1];
					break;
				case 1:
					tablename=tableparts[0];
					break;
			}
		}
		
		// get tables and views
		tabletype="TABLE,VIEW";

	} else {
		catalog=((!charstring::isNullOrEmpty(wild))?
						wild:SQL_ALL_CATALOGS);
	}

	// get the tables/databases
	erg=SQLTables(odbccur->stmt,
			(SQLCHAR *)catalog,SQL_NTS,
			(SQLCHAR *)schema,SQL_NTS,
			(SQLCHAR *)tablename,SQL_NTS,
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

	// get the table/database list
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
			// * catalog(.defaultschama).proc
			//   or
			// * (currentcatalog.)schema.proc...
			// We'll guess schema.proc, but we don't know for sure.
			schema=tableparts[0];
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
			// We'll guess schema.proc, but we don't know for sure.
			schema=procparts[0];
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
				// We'll guess schema.proc, but we don't know
				// for sure.
				schema=procparts[0];
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

bool odbcconnection::supportsAutoCommit() {
	return true;
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
	maxbindcount=conn->cont->getConfig()->getMaxBindCount();
	outdatebind=new datebind *[maxbindcount];
	for (uint16_t i=0; i<maxbindcount; i++) {
		outdatebind[i]=NULL;
	}
}

odbccursor::~odbccursor() {
	delete[] outdatebind;
}

bool odbccursor::prepareQuery(const char *query, uint32_t length) {

	// initialize column count
	initializeColCounts();

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
				#ifdef SQLBINDPARAMETER_SQLLEN
				(SQLLEN *)isnull
				#else
				(SQLINTEGER *)isnull
				#endif
				);
	} else {
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

	uint16_t	pos=charstring::toInteger(variable+1);
	if (!pos || pos>maxbindcount) {
		return false;
	}

	erg=SQLBindParameter(stmt,
				pos,
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
				#ifdef SQLBINDPARAMETER_SQLLEN
				(SQLLEN *)NULL
				#elif defined(SQLBINDPARAMETER_SQLLEN)
				(unsigned long *)NULL
				#else
				(SQLINTEGER *)NULL
				#endif
				);
	} else {

		SQL_TIMESTAMP_STRUCT	*ts=(SQL_TIMESTAMP_STRUCT *)buffer;
		ts->year=year;
		ts->month=month;
		ts->day=day;
		ts->hour=hour;
		ts->minute=minute;
		ts->second=second;
		ts->fraction=microsecond*1000;

		erg=SQLBindParameter(stmt,
				pos,
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
	}

	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return false;
	}
	return true;
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

	erg=SQLBindParameter(stmt,
				pos,
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

	uint16_t	pos=charstring::toInteger(variable+1);
	if (!pos || pos>maxbindcount) {
		return false;
	}

	outdatebind[pos-1]=NULL;

	*value=0;

	erg=SQLBindParameter(stmt,
				pos,
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

	uint16_t	pos=charstring::toInteger(variable+1);
	if (!pos || pos>maxbindcount) {
		return false;
	}

	outdatebind[pos-1]=NULL;

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

	erg=SQLBindParameter(stmt,
				pos,
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
#ifdef SQLROWCOUNT_SQLLEN
	erg=SQLRowCount(stmt,(SQLLEN *)&affectedrows);
#else
	erg=SQLRowCount(stmt,&affectedrows);
#endif
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return false;
	}

	// convert date output binds
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
}

bool odbccursor::handleColumns() {

	// get the column count
	erg=SQLNumResultCols(stmt,&ncols);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return false;
	}
	if (ncols>MAX_COLUMN_COUNT) {
		ncols=MAX_COLUMN_COUNT;
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
			// bind nvarchar and nchar fields as wchar
			erg=SQLBindCol(stmt,i+1,SQL_C_WCHAR,
					field[i],MAX_FIELD_LENGTH,
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
						field[i],MAX_FIELD_LENGTH,
						#ifdef SQLBINDCOL_SQLLEN
						(SQLLEN *)&indicator[i]
						#else
						(SQLINTEGER *)&indicator[i]
						#endif
						);
			} else {
				erg=SQLBindCol(stmt,i+1,SQL_C_CHAR,
						field[i],MAX_FIELD_LENGTH,
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
				field[i],MAX_FIELD_LENGTH,
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
	SQLCloseCursor(stmt);

	for (uint16_t i=0; i<getOutputBindCount(); i++) {
		delete outdatebind[i];
		outdatebind[i]=NULL;
	}
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrserverconnection *new_odbcconnection(
						sqlrservercontroller *cont) {
		return new odbcconnection(cont);
	}
}
