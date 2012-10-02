// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <odbcconnection.h>
#include <rudiments/charstring.h>

#include <config.h>

#include <datatypes.h>

#include <stdlib.h>


#ifdef HAVE_SQLCONNECTW
#include <iconv.h>
#include <wchar.h>

#define USER_CODING "UTF8"

char *buffers[200];
int nextbuf=0;

int ucslen(char* str)
{
	char *ptr=str;
	int res=0;
	while(!(*ptr==0 && *(ptr+1)==0))
	{
		res++;
		ptr+=2;
		
	}
	
	return res;
}

char *conv_to_user_coding(char *inbuf)
{
	char *outbuf;
  size_t insize = 0;
  char *wrptr;
  iconv_t cd;
	size_t avail;
	
	insize=ucslen(inbuf)*2;
	avail=insize+4;
	outbuf=(char*)malloc(avail);
	
	
	wrptr = (char *) outbuf;

	cd = iconv_open (USER_CODING, "UCS-2");
  if (cd == (iconv_t) -1)
    {
      /* Something went wrong.  */
        perror ("error in iconv_open");

      /* Terminate the output string.  */
      *outbuf = '\0';
      return outbuf;
    }
      size_t nconv;
      char *inptr = inbuf;
		
#ifdef ICONV_CONST_CHAR
		nconv = iconv (cd, (const char **)&inptr, &insize, &wrptr, &avail);
#else
		nconv = iconv (cd, &inptr, &insize, &wrptr, &avail);
#endif
      if (nconv == (size_t) -1)
        {
					printf("conv_to_user_coding: error in iconv\n");					
        }		
	
				/* Terminate the output string.  */
    					*(wrptr) = '\0';
				
				if (nconv == (size_t) -1)
				{
					printf("wrptr='%s'\n",wrptr);
				}

				
  if (iconv_close (cd) != 0)
    				perror ("iconv_close");
	
	
	return outbuf;


}

char *conv_to_ucs(char *inbuf)
{
	char *outbuf;
  size_t insize = 0;
  char *wrptr;
  iconv_t cd;
	size_t avail;
	
	insize=charstring::length(inbuf);
	avail=insize*2+4;
	
	outbuf=(char*)malloc(avail);
	
	wrptr = (char *) outbuf;

	cd = iconv_open ("UCS-2", USER_CODING);
  if(cd == (iconv_t) -1)
  {
      /* Something went wrong.  */
        perror ("error in iconv_open");

      /* Terminate the output string.  */
      *outbuf = L'\0';
      return outbuf;
  }
  size_t nconv;
  char *inptr = inbuf;
		
#ifdef ICONV_CONST_CHAR
	nconv = iconv (cd, (const char **)&inptr, &insize, &wrptr, &avail);
#else
	nconv = iconv (cd, &inptr, &insize, &wrptr, &avail);
#endif
  if (nconv == (size_t) -1)
  {
		printf("conv_to_ucs: error in iconv\n");					
  }		
	
			/* Terminate the output string.  */
  *((wchar_t *) wrptr) = L'\0';
	
	if (nconv == (size_t) -1)
	{
		printf("inbuf='%s'\n",inbuf);
	}

				
  if (iconv_close (cd) != 0)
    				perror ("error in iconv_close");
	
//		FILE *ff;
//		char fname[200];
//		sprintf(fname,"/home/orbb/temp/result_conv_to_ucs.txt_%d",charstring::length(inbuf));
//		ff=fopen(fname,"wb");
//		fwrite(outbuf,1,wrptr-outbuf,ff);
//		fclose(ff);

	return outbuf;
}
#endif



uint16_t odbcconnection::getNumberOfConnectStringVars() {
	return NUM_CONNECT_STRING_VARS;
}

void odbcconnection::handleConnectString() {
	dsn=connectStringValue("dsn");
	setUser(connectStringValue("user"));
	setPassword(connectStringValue("password"));
	const char	*autocom=connectStringValue("autocommit");
	setAutoCommitBehavior((autocom &&
		!charstring::compareIgnoringCase(autocom,"yes")));
	fakeinputbinds=
		!charstring::compare(connectStringValue("fakebinds"),"yes");
}

bool odbcconnection::logIn(bool printerrors) {

	// allocate environment handle
#if (ODBCVER >= 0x0300)
	erg=SQLAllocHandle(SQL_HANDLE_ENV,SQL_NULL_HANDLE,&env);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		SQLFreeHandle(SQL_HANDLE_ENV,env);
		return false;
	}
	erg=SQLSetEnvAttr(env,SQL_ATTR_ODBC_VERSION,
				(void *)SQL_OV_ODBC3,0);
#else
	erg=SQLAllocEnv(&env);
#endif
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
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
	SQLSetConnectAttr(dbc,SQL_LOGIN_TIMEOUT,(SQLPOINTER *)5,0);
#endif

	// connect to the database
	char *user_asc=(char*)getUser();
	char *password_asc=(char*)getPassword();
	char *dsn_asc=(char*)dsn;
#ifdef HAVE_SQLCONNECTW
	char *user_ucs=(char*)conv_to_ucs(user_asc);
	char *password_ucs=(char*)conv_to_ucs(password_asc);
	char *dsn_ucs=(char*)conv_to_ucs(dsn_asc);
	erg=SQLConnectW(dbc,(SQLWCHAR *)dsn_ucs,SQL_NTS,
				(SQLWCHAR *)user_ucs,SQL_NTS,
				(SQLWCHAR *)password_ucs,SQL_NTS);
				
	if(user_ucs)free(user_ucs);
	if(password_ucs)free(password_ucs);
	if(dsn_ucs)free(dsn_ucs);
#else
	erg=SQLConnect(dbc,(SQLCHAR *)dsn_asc,SQL_NTS,
				(SQLCHAR *)user_asc,SQL_NTS,
				(SQLCHAR *)password_asc,SQL_NTS);
#endif
	
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
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

sqlrcursor_svr *odbcconnection::initCursor() {
	return (sqlrcursor_svr *)new odbccursor((sqlrconnection_svr *)this);
}

void odbcconnection::deleteCursor(sqlrcursor_svr *curs) {
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

bool odbcconnection::getDatabaseList(sqlrcursor_svr *cursor, const char *wild) {
	return getDatabaseOrTableList(cursor,wild,false);
}

bool odbcconnection::getTableList(sqlrcursor_svr *cursor, const char *wild) {
	return getDatabaseOrTableList(cursor,wild,true);
}

bool odbcconnection::getDatabaseOrTableList(sqlrcursor_svr *cursor,
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

bool odbcconnection::getColumnList(sqlrcursor_svr *cursor,
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
#endif

bool odbcconnection::setIsolationLevel(const char *isolevel) {
	// FIXME: do nothing for now.  see task #422
	return true;
}

odbccursor::odbccursor(sqlrconnection_svr *conn) : sqlrcursor_svr(conn) {
	errormsg=NULL;
	odbcconn=(odbcconnection *)conn;
	stmt=NULL;
	for (uint16_t i=0; i<MAXVAR; i++) {
		outdatebind[i]=NULL;
	}
}

odbccursor::~odbccursor() {
	if (errormsg) {
		delete errormsg;
	}
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
	while(nextbuf>0)
	{
		nextbuf--;
		if(buffers[nextbuf])free(buffers[nextbuf]);
	}
	char *query_ucs=conv_to_ucs((char*)query);
	erg=SQLPrepareW(stmt,(SQLWCHAR *)query_ucs,SQL_NTS);
	if(query_ucs)free(query_ucs);
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

bool odbccursor::inputBindString(const char *variable,
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
		erg=SQLBindParameter(stmt,
				charstring::toInteger(variable+1),
				SQL_PARAM_INPUT,
				#ifdef HAVE_SQLCONNECTW
				SQL_C_WCHAR,
				#else
				SQL_C_CHAR,
				#endif
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

bool odbccursor::inputBindInteger(const char *variable,
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

bool odbccursor::inputBindDouble(const char *variable,
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

bool odbccursor::inputBindDate(const char *variable,
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

bool odbccursor::outputBindString(const char *variable, 
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

bool odbccursor::outputBindInteger(const char *variable,
						uint16_t variablesize,
						int64_t *value,
						int16_t *isnull) {

	outdatebind[outbindcount]=NULL;

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

bool odbccursor::outputBindDouble(const char *variable,
						uint16_t variablesize,
						double *value,
						uint32_t *precision,
						uint32_t *scale,
						int16_t *isnull) {

	outdatebind[outbindcount]=NULL;

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

bool odbccursor::outputBindDate(const char *variable,
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

bool odbccursor::executeQuery(const char *query, uint32_t length,
							bool execute) {

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
	for (uint16_t i=0; i<MAXVAR; i++) {
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

		if (conn->sendColumnInfo()) {
#if (ODBCVER >= 0x0300)
			// column name

		
			erg=SQLColAttribute(stmt,i+1,SQL_DESC_LABEL,
					col[i].name,MAX_ITEM_BUFFER_SIZE,
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
					col[i].name,MAX_ITEM_BUFFER_SIZE,
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
		if(col[i].type==-9 || col[i].type==-8)
		{
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

		}
		else
		{
			// bind the column to a buffer
			if(col[i].type==93 || col[i].type==91)
			{
				erg=SQLBindCol(stmt,i+1,SQL_C_BINARY,
						field[i],MAX_ITEM_BUFFER_SIZE,
						#ifdef SQLBINDCOL_SQLLEN
						(SQLLEN *)&indicator[i]
						#else
						(SQLINTEGER *)&indicator[i]
						#endif
						);
			}
			else
			{
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

void odbccursor::errorMessage(const char **errorstring,
				int64_t *errornumber,
				bool *liveconnection) {

	SQLCHAR		error[501];
	SQLCHAR		state[10];
	SQLINTEGER	nativeerrnum;
	SQLSMALLINT	errnum;

	// need to use SQLGetDiagRec and SQLGetDiagField here...
	SQLError(odbcconn->env,odbcconn->dbc,stmt,state,&nativeerrnum,
							error,500,&errnum);
	if (errormsg) {
		delete errormsg;
	}
	errormsg=new stringbuffer();
	errormsg->append((const char *)error);

	*liveconnection=true;

	// set return values
	*errorstring=errormsg->getString();
	*errornumber=errnum;
}

bool odbccursor::knowsRowCount() {
	return false;
}

uint64_t odbccursor::rowCount() {
	return 0;
}

bool odbccursor::knowsAffectedRows() {
	return true;
}

uint64_t odbccursor::affectedRows() {
	return affectedrows;
}

uint32_t odbccursor::colCount() {
	return ncols;
}

const char * const * odbccursor::columnNames() {
	for (SQLSMALLINT i=0; i<ncols; i++) {
		columnnames[i]=col[i].name;
	}
	return columnnames;
}

uint16_t odbccursor::columnTypeFormat() {
	return (uint16_t)COLUMN_TYPE_IDS;
}

void odbccursor::returnColumnInfo() {

	// a useful variable
	uint16_t	type;

	// for each column...
	for (SQLSMALLINT i=0; i<ncols; i++) {

		uint16_t	binary=0;
		if (col[i].type==SQL_BIGINT) {
			type=BIGINT_DATATYPE;
		} else if (col[i].type==SQL_BINARY) {
			type=BINARY_DATATYPE;
			binary=1;
		} else if (col[i].type==SQL_BIT) {
			type=BIT_DATATYPE;
		} else if (col[i].type==SQL_CHAR) {
			type=CHAR_DATATYPE;
		} else if (col[i].type==SQL_DATE) {
			type=DATE_DATATYPE;
		} else if (col[i].type==SQL_DECIMAL) {
			type=DECIMAL_DATATYPE;
		} else if (col[i].type==SQL_DOUBLE) {
			type=DOUBLE_DATATYPE;
		} else if (col[i].type==SQL_FLOAT) {
			type=FLOAT_DATATYPE;
		} else if (col[i].type==SQL_INTEGER) {
			type=INTEGER_DATATYPE;
		} else if (col[i].type==SQL_LONGVARBINARY) {
			type=LONGVARBINARY_DATATYPE;
			binary=1;
		} else if (col[i].type==SQL_LONGVARCHAR) {
			type=LONGVARCHAR_DATATYPE;
		} else if (col[i].type==SQL_NUMERIC) {
			type=NUMERIC_DATATYPE;
		} else if (col[i].type==SQL_REAL) {
			type=REAL_DATATYPE;
		} else if (col[i].type==SQL_SMALLINT) {
			type=SMALLINT_DATATYPE;
		} else if (col[i].type==SQL_TIME) {
			type=TIME_DATATYPE;
		} else if (col[i].type==SQL_TIMESTAMP) {
			type=TIMESTAMP_DATATYPE;
		} else if (col[i].type==SQL_TINYINT) {
			type=TINYINT_DATATYPE;
		} else if (col[i].type==SQL_VARBINARY) {
			type=VARBINARY_DATATYPE;
			binary=1;
		} else if (col[i].type==SQL_VARCHAR) {
			type=VARCHAR_DATATYPE;
		} else {
			type=UNKNOWN_DATATYPE;
		}

		// send column definition
		conn->sendColumnDefinition(col[i].name,col[i].namelength,type,
						col[i].length,col[i].precision,
						col[i].scale,col[i].nullable,
						0,0,0,
						col[i].unsignednumber,0,binary,
						col[i].autoincrement);


	}
}

bool odbccursor::noRowsToReturn() {
	// if there are no columns, then there can't be any rows either
	return (!ncols);
}

bool odbccursor::skipRow() {
	return fetchRow();
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
	for (int i=0; i<ncols; i++)
	{
		
		
		if(col[i].type==-9 || col[i].type==-8)
		{
			if(indicator[i]!=-1 && field[i])
			{
				char *u=conv_to_user_coding(field[i]);
				int len=charstring::length(u);
				charstring::copy(field[i],u);
				indicator[i]=len;
			
				if(u)free(u);			
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

void odbccursor::cleanUpData(bool freeresult, bool freebinds) {

	if (freebinds) {
		for (uint16_t i=0; i<MAXVAR; i++) {
			delete outdatebind[i];
			outdatebind[i]=NULL;
		}
	}
}
