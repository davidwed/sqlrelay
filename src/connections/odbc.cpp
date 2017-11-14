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
#include <string.h>         // I am calling memset, memcmp
#include <stdlib.h>

// note that sqlrserver.h must be included after sqltypes.h to
// get around a problem with CHAR/xmlChar in gnome-xml
#include <sqlrelay/sqlrserver.h>
#include <rudiments/charstring.h>
#include <rudiments/error.h>
#include <rudiments/stdio.h>
#include <rudiments/process.h>
#include <rudiments/randomnumber.h>
#include <rudiments/sys.h>

#include <datatypes.h>
#include <defines.h>

#ifdef HAVE_IODBC
	#include <iodbcinst.h>
#endif

#define FETCH_AT_ONCE		10
#define MAX_SELECT_LIST_SIZE	400
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


// here we use the types declared for SQLBindParameter, but also include
// the types we want to use for communicating with the rest of SQLRelay.
struct stringbind {
  char *value;
  uint64_t valuesize;
  short *isnull;
  SQLUSMALLINT ParameterNumber;
  SQLSMALLINT InputOutputType;
  SQLSMALLINT ValueType;
  SQLSMALLINT ParameterType;
  SQLULEN ColumnSize;
  SQLPOINTER ParameterValuePtr;
  SQLLEN  BufferLength;
  SQLLEN StrLen_or_IndPtr;
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
						bool isnegative,
						char *buffer,
						uint16_t buffersize,
						int16_t *isnull);
		bool		outputBind(const char *variable, 
						uint16_t variablesize,
						char *value, 
						uint32_t valuesize,
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
						bool *isnegative,
						char *buffer,
						uint16_t buffersize,
						int16_t *isnull);
		short		nonNullBindValue();
		short		nullBindValue();
		bool		bindValueIsNull(short isnull);
		bool		executeQuery(const char *query,
						uint32_t length);
		bool            nextResultSet(bool *next_result_set_available);
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
		
		uint16_t        outbind_count;
		uint16_t        allocate_outbind_index();
                void            reset_outbind_index();
                void            fixup_output_binds();
		datebind	**outdatebind;
		stringbind      **outstringbind;

		uint32_t	row;
		uint32_t	maxrow;
		uint32_t	totalrows;
		uint32_t	rownumber;

		stringbuffer	errormsg;

		odbcconnection	*odbcconn;
		uint64_t	query_timeout;
		bool            execute_direct;
		bool            execute_rpc;
		void initialize_directives();
		void parse_directive(const char *directive_start, uint32_t length);
		void parse_directives(const char *query, uint32_t length);
};

class SQLRSERVER_DLLSPEC odbcconnection : public sqlrserverconnection {
	friend class odbccursor;
	public:
			odbcconnection(sqlrservercontroller *cont);
	private:
		bool	        mustDetachBeforeLogIn();
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
		const char	*bindFormat();

		const char	*getLastInsertIdQuery();
		bool		supportsTransactionBlocks();

                char *odbc_driver_connection_string(const char*, const char*);

		SQLRETURN	erg;
		SQLHENV		env;
		SQLHDBC		dbc;

                const char      *odbc_driver;
                const char      *odbc_server;
		const char	*dsn;
		const char      *extra_driverconnect;
                const char      *initial_db;
                const char      *trace_file;
                int             trace_flag;
                bool            detach_before_login;
		bool            execute_direct;

		const char	*lastinsertidquery;
                
		uint64_t	timeout;
		uint64_t	query_timeout;

		const char	*identity;

		const char	*odbcversion;

		stringbuffer	errormessage;

		char		dbversion[512];

#if (ODBCVER>=0x0300)
		stringbuffer	errormsg;
#endif
		const char* getCurrentDatabaseQuery();
		const char* dbHostNameQuery();
		const char* dbIpAddressQuery();
};

#define FIXED_BUFFER_COUNT 512

#ifdef SQLBINDPARAMETER_SQLLEN
SQLLEN
#else
SQLINTEGER
#endif
* indptrs[FIXED_BUFFER_COUNT];
int nextindptr=0;

#ifdef HAVE_SQLCONNECTW
#include <iconv.h>
#include <wchar.h>

#define USER_CODING "UTF8"

/* deus ex machina. This fixed size should be dynamic or at least checked. */
char *buffers[FIXED_BUFFER_COUNT];
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

        // insize is the number of unicode code points times 2.
        // now a full 16 bit codepoint might generate 3 bytes in
        // the output utf, so this conversion could make things
        // bigger. production running has shown that
        // it is possible to get errno = E2BIG if we do not
        // have enough space, and that is eventually fatal.
        // One more byte for zero termination.
	
	size_t	insize=ucslen(inbuf)*2;
	size_t	avail=(insize/2)*3 + 1;
	char	*outbuf=new char[avail];
	char	*wrptr=outbuf;
	size_t insize_before = insize;
	size_t avail_before = avail;

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
	  stdoutput.printf("conv_to_user_coding: error in iconv errno = %d insize=%ld/%ld avail=%ld/%ld before/after.\n", errno,
			   insize_before, insize, avail_before, avail
			   );
	}		
	
	/* Terminate the output string. */
	*(wrptr)='\0';
				
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
        lastinsertidquery=NULL;
}

static int boolean_flag(const char *value, int default_value) {
  if (value == NULL) {
    return(default_value);
  } else if (charstring::compareIgnoringCase(value,"yes") == 0) {
    return(1);
  } else if (charstring::compareIgnoringCase(value,"no") == 0) {
    return(0);
  } else {
    return(default_value);
  }
}

void odbcconnection::handleConnectString() {
        odbc_driver = cont->getConnectStringValue("driver");
        odbc_server = cont->getConnectStringValue("server");
        initial_db=cont->getConnectStringValue("db");
        trace_file=cont->getConnectStringValue("tracefile");
        trace_flag = boolean_flag(cont->getConnectStringValue("trace"), -1);
	dsn=cont->getConnectStringValue("dsn");
	extra_driverconnect=cont->getConnectStringValue("driverconnect");
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
	const char	*qto=cont->getConnectStringValue("querytimeout");
	if (!charstring::length(qto)) {
		// this means no default query timeout, wait forever for server to respond.
		query_timeout=0;
	} else {
		query_timeout=charstring::toInteger(qto);
	}

	identity=cont->getConnectStringValue("identity");

	odbcversion=cont->getConnectStringValue("odbcversion");

        detach_before_login = boolean_flag(cont->getConnectStringValue("detachbeforelogin"), 0);
        lastinsertidquery = cont->getConnectStringValue("lastinsertidquery");
        execute_direct = boolean_flag(cont->getConnectStringValue("execdirect"), 1);
}


/* This would be a good candidate for promotion to rudiments,
   because a single shared trace/log file of any kind
   is often very frustrating, with high cognitive cost to deal with.
   These format operators are enough to provide a unique log file
   name per process even when the log 
   It is a specialized format language where
   %p means PID
   %t means a timestamp.
   %h means the hostname.
   If any of these appears more than once then the output filename
   may be truncated.
*/
   
char *trace_file_format(const char *trace_file_fmt) {
  pid_t	pid = process::getProcessId();
  datetime dt;
  dt.getSystemDateAndTime();
  time_t now = dt.getEpoch();
  char *hostname = sys::getHostName();
  size_t trace_filename_buffersize = charstring::length(trace_file_fmt);
  trace_filename_buffersize += charstring::integerLength((int64_t) pid);
  trace_filename_buffersize += charstring::integerLength((int64_t) now);
  trace_filename_buffersize += charstring::length(hostname);
  trace_filename_buffersize += 1;
  char *trace_filename = new char[trace_filename_buffersize];
  char *outptr = trace_filename;
  size_t outptr_size = trace_filename_buffersize - 1;
  const char *ptr = trace_file_fmt;
  *outptr = 0;
  while (*ptr && (outptr_size > 0)) {
    if (*ptr == '%') {
      char *insert_string = NULL;
      int64_t insert_number = 0;
      ++ptr;
      if (*ptr == 'p') {
        insert_number = pid;
      } else if (*ptr == 't') {
        insert_number = now;
      } else if (*ptr == 'h') {
        insert_string = hostname;
      }
      if (insert_string != NULL) {
        charstring::printf(outptr, outptr_size, "%s", insert_string);
      } else {
        charstring::printf(outptr, outptr_size, "%ld", insert_number);
      }
      ++ptr;
      size_t outptr_inc = charstring::length(outptr);
      outptr_size -= outptr_inc;
      outptr += outptr_inc;
    } else {
      *outptr++ = *ptr++;
      outptr_size--;
      *outptr = 0;
    }
  }
  delete[] hostname;
  return trace_filename;
}

void push_connstr_value(char **pptr, size_t *pbuffavail, const char *keyword, const char *value) {
  const char *open_bracket = "";
  const char *close_bracket = "";
  char *ptr = *pptr;
  size_t buffavail = *pbuffavail;
  if (charstring::contains(value, ';')) {
    open_bracket = "{";
    close_bracket = "}";
  }
  if (keyword == NULL) {
    // here we are just going to push a raw value. With an extra semicolon just in case.
    charstring::printf(ptr, buffavail, "%s;", value);
  } else {
    charstring::printf(ptr, buffavail, "%s=%s%s%s;", keyword, open_bracket, value, close_bracket);
  }
  size_t ptr_inc = charstring::length(ptr);
  ptr += ptr_inc;
  buffavail -= ptr_inc;
  *pptr = ptr;
  *pbuffavail = buffavail;
}

char *odbcconnection::odbc_driver_connection_string(const char *user_asc, const char *password_asc) {
  size_t buffsize = 1024;
  size_t buffavail = buffsize;
  char *buff = new char[buffsize];
  char *ptr = buff;

  /* At least with unixODBC we find that if the DSN is not the first field, there will
     be an SQLDriverConnect error of
     state 08001
     errnum 0
     message [unixODBC][Microsoft][ODBC Driver 11 for SQL Server]Neither DSN nor SERVER keyword supplied

     If DSN is specified then the DRIVER seems to be ignored. This makes sense actually.
  */

  if (!charstring::isNullOrEmpty(dsn)) {
    push_connstr_value(&ptr, &buffavail, "DSN", dsn);
  }

  if (!charstring::isNullOrEmpty(odbc_driver)) {
    push_connstr_value(&ptr, &buffavail, "DRIVER", odbc_driver);
  }

  if (!charstring::isNullOrEmpty(extra_driverconnect)) {
    // we push this extra info right after the DSN or DRIVER
    // so that we can clearly see it in the unixODBC trace which
    // tends to truncate at about 130 characters.
    unsigned char *raw_driverconnect = charstring::base64Decode(extra_driverconnect);
    push_connstr_value(&ptr, &buffavail, NULL, (const char *) raw_driverconnect);
    delete[] raw_driverconnect;
  }
  if (!(charstring::isNullOrEmpty(odbc_server) || charstring::contains(buff, ";SERVER="))) {
    push_connstr_value(&ptr, &buffavail, "SERVER", odbc_server);
  }
  if (!(charstring::isNullOrEmpty(user_asc) || charstring::contains(buff, ";UID="))) {
    push_connstr_value(&ptr, &buffavail, "UID", user_asc);
  }
  if (!(charstring::isNullOrEmpty(password_asc) || charstring::contains(buff, ";PWD="))) {
    push_connstr_value(&ptr, &buffavail, "PWD", password_asc);
  }
  if (!charstring::contains(buff, ";WSID=")) {
    push_connstr_value(&ptr, &buffavail, "WSID", sys::getHostName());
  }
  if (!charstring::contains(buff, ";APP=")) {
    push_connstr_value(&ptr, &buffavail, "APP", "SQLRelay-"SQLR_VERSION);
  }
  return buff;
}

bool odbcconnection::mustDetachBeforeLogIn() {
  return detach_before_login;
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

	// allocate connection handlne
#if (ODBCVER >= 0x0300)
	erg=SQLAllocHandle(SQL_HANDLE_DBC,env,&dbc);
#else
	erg=SQLAllocConnect(env,&dbc);
#endif
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		*error="Failed to allocate connection handle";
#if (ODBCVER >= 0x0300)
		SQLFreeHandle(SQL_HANDLE_ENV,env);
#else
		SQLFreeEnv(env);
#endif
		return false;
	}

#if (ODBCVER >= 0x0300)
        char *trace_filename = NULL;
        if (!charstring::isNullOrEmpty(trace_file)) {
          trace_filename = trace_file_format(trace_file);
          erg = SQLSetConnectAttr(dbc,
                                  SQL_ATTR_TRACEFILE,
                                  (SQLPOINTER *)trace_filename,
                                  SQL_NTS);
        }
        if (trace_flag != -1) {
          erg = SQLSetConnectAttr(dbc,
                                  SQL_ATTR_TRACE,
                                  (SQLPOINTER *) ((trace_flag == 1) ? SQL_OPT_TRACE_ON : SQL_OPT_TRACE_OFF),
                                  0);
        }
        if (trace_filename != NULL) {
          delete[] trace_filename;
          trace_filename = NULL;
        }
#endif

	// set the connect timeout
#if (ODBCVER >= 0x0300)
	if (timeout) {
                erg = SQLSetConnectAttr(dbc,SQL_LOGIN_TIMEOUT,
					(SQLPOINTER *)timeout,0);
		if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
			*error="Failed to set timeout";
			SQLFreeHandle(SQL_HANDLE_DBC,dbc);
			SQLFreeHandle(SQL_HANDLE_ENV,env);
			return false;
		}
	}
#endif

#if (ODBCVER >= 0x0300)
        if (!charstring::isNullOrEmpty(initial_db)) {
		erg = SQLSetConnectAttr(dbc,SQL_ATTR_CURRENT_CATALOG,
					(SQLPOINTER *)initial_db,SQL_NTS);
		if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
			*error="Failed to set database";
			SQLFreeHandle(SQL_HANDLE_DBC,dbc);
			SQLFreeHandle(SQL_HANDLE_ENV,env);
			return false;
                }
        }
#endif
          

	// connect to the database
	char *user_asc=(char*)cont->getUser();
	char *password_asc=(char*)cont->getPassword();

        if (!charstring::isNullOrEmpty(odbc_driver)) {
          char *sqlconnectdriver_string = odbc_driver_connection_string(user_asc, password_asc);
          // These values are useful to look at from the debugger,
          // it is not a good security practice to directly log
          // the string, because it might contain a plaintext password.
          SQLCHAR OutConnectionString[2048];
          SQLSMALLINT OutConnectionStringLen;
          erg = SQLDriverConnect(dbc,
                                 (SQLHWND) NULL,
                                 (SQLCHAR *) sqlconnectdriver_string,
                                 (SQLSMALLINT) charstring::length(sqlconnectdriver_string),
                                 OutConnectionString,
                                 (SQLSMALLINT) sizeof(OutConnectionString),
                                 &OutConnectionStringLen,
                                 (SQLSMALLINT) SQL_DRIVER_NOPROMPT);
          if (sqlconnectdriver_string) {
            delete[] sqlconnectdriver_string;
          }
        } else {

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
        }
	
	if (erg==SQL_SUCCESS_WITH_INFO) {
		*warning=logInError(NULL);
	} else if (erg!=SQL_SUCCESS) {
		*error=logInError("SQLConnect failed");
#if (ODBCVER >= 0x0300)
		SQLFreeHandle(SQL_HANDLE_DBC,dbc);
		SQLFreeHandle(SQL_HANDLE_ENV,env);
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
	SQLCHAR		state[SQL_SQLSTATE_SIZE+1];
	SQLINTEGER	nativeerrnum;
	SQLCHAR		errorbuffer[1024];
	SQLSMALLINT	errlength;

        memset(state, 0, sizeof(state));

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
        dbc = NULL;
        env = NULL;
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


const char *odbcconnection::bindFormat() {
	return "?";
}


const char *odbcconnection::getLastInsertIdQuery() {
  return lastinsertidquery;
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
		tablename=(!charstring::isNullOrEmpty(wild))?wild:"%";
	} else {
		catalog=((!charstring::isNullOrEmpty(wild))?
						wild:SQL_ALL_CATALOGS);
	}
	erg=SQLTables(odbccur->stmt,
			(SQLCHAR *)catalog,SQL_NTS,
			(SQLCHAR *)schema,SQL_NTS,
			(SQLCHAR *)tablename,SQL_NTS,
			NULL,SQL_NTS);
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


static bool is_liveconnection(SQLCHAR *state, SQLINTEGER nativeerrnum) {
  /* TODO: Make this a method, and gain access to the dbc, and if ODBC 3.5
     see if SQL_ATTR_CONNECTION_DEAD is SQL_CD_TRUE.
  */
  if (memcmp("08S01", state, 5) == 0) {
    return false;
  } else if (memcmp("08003", state, 5) == 0) {
    return false;
  } else {
    return true;
  }
}


bool odbcconnection::supportsTransactionBlocks() {
	return false;
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
	SQLCHAR		state[SQL_SQLSTATE_SIZE+1];
	SQLINTEGER	nativeerrnum;
	SQLSMALLINT	errlength;

        memset(state, 0, sizeof(state));

	SQLGetDiagRec(SQL_HANDLE_DBC,dbc,1,state,&nativeerrnum,
                      (SQLCHAR *)errorbuffer,errorbufferlength,
                      &errlength);

	// set return values
	*errorlength=errlength;
	*errorcode=nativeerrnum;
	*liveconnection = is_liveconnection(state, nativeerrnum);
}
#endif

bool odbcconnection::setIsolationLevel(const char *isolevel) {
	// FIXME: do nothing for now.  see task #422
	return true;
}

const char * odbcconnection::getCurrentDatabaseQuery() {
  return "SELECT cast(DB_NAME() as varchar(64))";
}

const char * odbcconnection::dbHostNameQuery() {
  return "SELECT cast(@@SERVERNAME as varchar(64))";
}

const char * odbcconnection::dbIpAddressQuery() {
  return "SELECT CAST(SERVERPROPERTY('ComputerNamePhysicalNetBIOS') as varchar(64))";
}

odbccursor::odbccursor(sqlrserverconnection *conn, uint16_t id) :
						sqlrservercursor(conn,id) {
	odbcconn=(odbcconnection *)conn;
	stmt=NULL;
	outbind_count = 0;
	outdatebind=new datebind *[conn->cont->getConfig()->getMaxBindCount()];
	for (uint16_t i=0; i<conn->cont->getConfig()->getMaxBindCount(); i++) {
		outdatebind[i]=NULL;
	}
	outstringbind=new stringbind *[conn->cont->getConfig()->getMaxBindCount()];
	for (uint16_t i=0; i<conn->cont->getConfig()->getMaxBindCount(); i++) {
		outstringbind[i]=NULL;
	}
	query_timeout = 0;
	execute_direct = odbcconn->execute_direct;
	execute_rpc = false;
}

odbccursor::~odbccursor() {
	delete[] outdatebind;
	delete[] outstringbind;
        stmt = NULL;
}

void odbccursor::initialize_directives() {
  query_timeout = 0;
  execute_direct = odbcconn->execute_direct;
  execute_rpc = false;
}

#define KEYWORD_SQLEXECDIRECT "sqlexecdirect"
#define KEYWORD_QUERYTIMEOUT "querytimeout:"
#define KEYWORD_SQLPREPARE "sqlprepare"
#define KEYWORD_SQLRELAY_CRASH "sqlrelay-crash"
#define KEYWORD_SQLRELAY_CRASH_ARG "sqlrelay-crash:"
#define MARKER_ODBC_RPC '{'

void crashme_test(int32_t iargument) {
  char *some_ptr = (char *) NULL;
  if (iargument == 0) {
    some_ptr[0] = 1;
  } else if (iargument == 1) {
    free(some_ptr);
  } else if (iargument == 2) {
    some_ptr = (char *) malloc(100);
    free(some_ptr);
    free(some_ptr);
  } else if (iargument == 3) {
    some_ptr[0] = (memchr(some_ptr, 25, 30)) ? 1 : 2;
  } else if (iargument == 4) {
    some_ptr[0] = some_ptr[10];
  }
}

void odbccursor::parse_directive(const char *directive_start, uint32_t length) {
  uint32_t clean_length = length;
  int32_t argument_size;
  const char *argument;
  int32_t iargument = 0;
  if ((clean_length > 0) && directive_start[clean_length-1] == '\r') {
    --clean_length;
  }
  if (clean_length == 0) {
    return;
  }
  // Note: These are not intended to be human friendly declarations,
  // just very strict and simple formats for a code generator to emit.
  if (strncmp(directive_start, KEYWORD_SQLEXECDIRECT, clean_length) == 0) {
    execute_direct = true;
    return;
  }
  if (strncmp(directive_start, KEYWORD_SQLPREPARE, clean_length) == 0) {
    execute_direct = false;
  }
  if (strncmp(directive_start, KEYWORD_SQLRELAY_CRASH, clean_length) == 0) {
    crashme_test(0);
    return;
  }
  if ((clean_length > strlen(KEYWORD_SQLRELAY_CRASH_ARG)) &&
      (strncmp(directive_start, KEYWORD_SQLRELAY_CRASH_ARG, strlen(KEYWORD_SQLRELAY_CRASH_ARG)) == 0)) {
    argument_size = clean_length - strlen(KEYWORD_SQLRELAY_CRASH_ARG);
    argument = &directive_start[strlen(KEYWORD_SQLRELAY_CRASH_ARG)];
    if (charstring::isInteger(argument, argument_size)) {
      iargument = charstring::toInteger(argument);
    }
    crashme_test(iargument);
    return;
  }
  if ((clean_length > strlen(KEYWORD_QUERYTIMEOUT)) &&
      (strncmp(directive_start, KEYWORD_QUERYTIMEOUT, strlen(KEYWORD_QUERYTIMEOUT)) == 0)) {
    argument_size = clean_length - strlen(KEYWORD_QUERYTIMEOUT);
    argument = &directive_start[strlen(KEYWORD_QUERYTIMEOUT)];
    if (charstring::isInteger(argument, argument_size)) {
      // well, I know that the directive is always zero terminated someplace,
      // and I already know this it appears to be an integer, so
      // let it rip even though we would like to use the argument_size.
      query_timeout = charstring::toInteger(argument);
    }
    return;
  }
}

void odbccursor::parse_directives(const char *query, uint32_t length) {
  const char *line_start = NULL;
  const char *line_end = NULL;

  if (query[0] == MARKER_ODBC_RPC) {
    execute_direct = true;
    execute_rpc = true;
  }
  line_start = &query[0];
  for (uint32_t i=0; (i < length); ++i) {
    if ((query[i] == '\n') || !((i + 1) < length)) {
      if (query[i] == '\n') {
	line_end = &query[i];
      } else {
	line_end = &query[i+1];
      }
      if (((line_end - line_start) > 2) &&
	  (line_start[0] == '-') &&
	  (line_start[1] == '-')) {
	parse_directive(&line_start[2], line_end - line_start - 2);
      }
      line_start = &query[i+1];
    }
  }
}


bool odbccursor::prepareQuery(const char *query, uint32_t length) {

	// initialize column count
	initializeColCounts();
	initialize_directives();
	parse_directives(query, length);
	reset_outbind_index();

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

	//free allocated indptrs
	while (nextindptr>0) {
		nextindptr--;
		if (indptrs[nextindptr]) {
			delete[] indptrs[nextindptr];
		}
	}

#ifdef HAVE_SQLCONNECTW
	//free allocated buffers
	while (nextbuf>0) {
		nextbuf--;
		if (buffers[nextbuf]) {
			delete[] buffers[nextbuf];
		}
	}

	if (execute_direct) {
	  return true;
	}
	
	char *query_ucs=conv_to_ucs((char*)query);
	erg=SQLPrepareW(stmt,(SQLWCHAR *)query_ucs,SQL_NTS);
	if (query_ucs) {
		delete[] query_ucs;
	}
#else
	if (execute_direct) {
	  return true;
	}
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
                stmt = NULL;
#else
		SQLFreeStmt(stmt,SQL_DROP);
                stmt = NULL;
#endif
	}
#if (ODBCVER >= 0x0300)
	erg=SQLAllocHandle(SQL_HANDLE_STMT,odbcconn->dbc,&stmt);
#else
	erg=SQLAllocStmt(odbcconn->dbc,&stmt);
#endif
	return (erg==SQL_SUCCESS || erg==SQL_SUCCESS_WITH_INFO);
}


// Yet another definition missing from unixODBC. Of course it is in the Microsoft ODBC SDK
// A copy of which is in https://github.com/Microsoft/msphpsql under source/shared/msodbcsql.h
// In looking for that I also found a nice looking C++ data access library https://github.com/SOCI/soci

#ifndef SQL_SS_LENGTH_UNLIMITED
#define SQL_SS_LENGTH_UNLIMITED 0
#endif

bool odbccursor::inputBind(const char *variable,
				uint16_t variablesize,
				const char *value,
				uint32_t valuesize,
				short *isnull) {

        uint32_t bufferlength = valuesize;
	#ifdef SQLBINDPARAMETER_SQLLEN
        SQLLEN *
        #else
        SQLINTEGER *
        #endif
          ind_value_null_ptr;

	#ifdef HAVE_SQLCONNECTW
	char *value_ucs=conv_to_ucs((char*)value);
	bufferlength=ucslen(value_ucs)*2;
	buffers[nextbuf]=value_ucs;
	nextbuf++;
	#endif
						
	if (*isnull==SQL_NULL_DATA) {
		// the 4th parameter (ValueType) must by
		// SQL_C_BINARY for this to work with blobs
                // Above comment may be obsolete.
                ind_value_null_ptr = new

	#ifdef SQLBINDPARAMETER_SQLLEN
        SQLLEN
        #else
        SQLINTEGER
        #endif
                  [1];
                ind_value_null_ptr[0] = SQL_NULL_DATA;
                indptrs[nextindptr++] = ind_value_null_ptr;
		erg=SQLBindParameter(stmt,
				charstring::toInteger(variable+1),
				SQL_PARAM_INPUT,
                                SQL_C_BINARY,
				SQL_WVARCHAR,
				0,
				0,
                                NULL,
                                0,
                                ind_value_null_ptr
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
				SQL_WVARCHAR,
				// This is like saying nvarchar(4000) vs nvarchar(max)
				// In T-SQL there is NO such type as nvarchar(5000), for example.
			        (bufferlength > 8000) ? SQL_SS_LENGTH_UNLIMITED : bufferlength / 2,
				0,
				#ifdef HAVE_SQLCONNECTW
				(SQLPOINTER)value_ucs,
				#else
				(SQLPOINTER)value,
				#endif
				bufferlength,
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
				SQL_C_SBIGINT,
				SQL_BIGINT,
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
				bool isnegative,
				char *buffer,
				uint16_t buffersize,
				int16_t *isnull) {

	bool	validdate=(year>=0 && month>=0 && day>=0);
	bool	validtime=(hour>=0 && minute>=0 && second>=0 && microsecond>=0);

	if (validdate && !validtime) {

		SQL_DATE_STRUCT	*ts=(SQL_DATE_STRUCT *)buffer;
		ts->year=year;
		ts->month=month;
		ts->day=day;

		erg=SQLBindParameter(stmt,
				charstring::toInteger(variable+1),
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
	}

	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return false;
	}
	return true;
}



uint16_t odbccursor::allocate_outbind_index() {
  uint16_t current_outbind_count = outbind_count;
  ++outbind_count;
  return current_outbind_count;
}


void odbccursor::reset_outbind_index() {
  for (uint16_t i=0; i<conn->cont->getConfig()->getMaxBindCount(); i++) {
    if (outdatebind[i] != NULL) {delete outdatebind[i];}
    outdatebind[i]=NULL;
  }
  for (uint16_t i=0; i<conn->cont->getConfig()->getMaxBindCount(); i++) {
    if (outstringbind[i] != NULL) {delete outstringbind[i];}
    outstringbind[i]=NULL;
  }
  outbind_count = 0;
}

bool odbccursor::outputBind(const char *variable, 
				uint16_t variablesize,
				char *value, 
				uint32_t valuesize, 
				short *isnull) {
        uint16_t output_bind_index = allocate_outbind_index();
        outdatebind[output_bind_index] = NULL;

	stringbind *sb=new stringbind;
	sb->value = value;
	sb->valuesize = valuesize;
	sb->isnull = isnull;
	sb->ParameterNumber = charstring::toInteger(variable+1);
	sb->InputOutputType = SQL_PARAM_OUTPUT;
	sb->ValueType = SQL_C_CHAR;
	sb->ParameterType = SQL_VARCHAR;
	sb->ColumnSize = (valuesize > 8000) ? SQL_SS_LENGTH_UNLIMITED : valuesize;
	// TODO: handle wide characters here.
	sb->ParameterValuePtr = value;
	sb->BufferLength = valuesize;
	sb->StrLen_or_IndPtr = 0;
	outstringbind[output_bind_index] = sb;

	erg=SQLBindParameter(stmt,
			     sb->ParameterNumber,
			     sb->InputOutputType,
			     sb->ValueType,
			     sb->ParameterType,
			     sb->ColumnSize,
			     (SQLSMALLINT) 0,
			     sb->ParameterValuePtr,
			     sb->BufferLength,
			     &sb->StrLen_or_IndPtr);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return false;
	}
	return true;
}

bool odbccursor::outputBind(const char *variable,
				uint16_t variablesize,
				int64_t *value,
				int16_t *isnull) {

        uint16_t output_bind_index = allocate_outbind_index();
	outdatebind[output_bind_index]=NULL;
	outstringbind[output_bind_index]=NULL;

	*value=0;

	erg=SQLBindParameter(stmt,
				charstring::toInteger(variable+1),
				SQL_PARAM_OUTPUT,
				SQL_C_SBIGINT,
				SQL_BIGINT,
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

        uint16_t output_bind_index = allocate_outbind_index();
	outdatebind[output_bind_index]=NULL;
	outstringbind[output_bind_index]=NULL;

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
				bool *isnegative,
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
	*isnegative=false;
	db->buffer=buffer;
        uint16_t output_bind_index = allocate_outbind_index();
	outdatebind[output_bind_index]=db;
	outstringbind[output_bind_index]=NULL;

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

void odbccursor::fixup_output_binds() {
	// Here we "fix up" some output binds. In fact the data does not really become
        // available, at least with the Microsoft ODBC Driver, until
        // SQLMoreResults would have returned SQL_NO_DATA
        // So if there are any output results pending this work is being done too soon.

	// convert date output binds
	for (uint16_t i=0; i<conn->cont->getConfig()->getMaxBindCount(); i++) {
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
		if (outstringbind[i]) {
		  stringbind *sb=outstringbind[i];
		  if (sb->StrLen_or_IndPtr == SQL_NULL_DATA) {
		    *(sb->isnull) = SQL_NULL_DATA;
		  } else if (sb->StrLen_or_IndPtr == -4) {
		    // this is SQL_NO_TOTAL and is most likely caused by the fact
		    // that we should be using SQL_C_WCHAR and SQL_WCHAR instead
		    // of forcing ODBC to do the conversion for us.
		    // In a work-around we just kludge away the space with padding,
		    // because right now we need to ship a build.
		    // Work-around in SQL: return only varchar, not nvarchar
		    char *valuep = sb->value;
		    for (int k=(sb->BufferLength - 2); k >= 0 && valuep[k] == ' '; --k) {
		      valuep[k] = 0;
		    }
		  } else if ((sb->StrLen_or_IndPtr >= 0) && (sb->StrLen_or_IndPtr < sb->BufferLength)) {
		    // so I think this is a real length. We have not
		    // verified this in a test yet, but it can't hurt, can it?
		    sb->value[sb->StrLen_or_IndPtr] = 0;
		  }
		}
	}
}

bool odbccursor::executeQuery(const char *query, uint32_t length) {

	// initialize counts
	initializeRowCounts();

	// query timeout is an odbc-driver level read timeout
	// but with special cleanup handling in better drivers to tell the server
	// to stop executing the query after the client read timeout.

	uint64_t statement_query_timeout = odbcconn->query_timeout;
	if (query_timeout > 0) {
	  statement_query_timeout = query_timeout;
	}

	if (statement_query_timeout > 0) {
	  erg = SQLSetStmtAttr(stmt, SQL_ATTR_QUERY_TIMEOUT, (SQLPOINTER) statement_query_timeout, SQL_IS_UINTEGER);
	}

	// execute the query
	if (execute_direct) {
#ifdef HAVE_SQLCONNECTW
	  char *query_ucs=conv_to_ucs((char*)query);
	  erg=SQLExecDirectW(stmt,(SQLWCHAR *)query_ucs,SQL_NTS);
	  if (query_ucs) {
	    delete[] query_ucs;
	  }
#else
          erg=SQLExecDirect(stmt,(SQLCHAR *)query,length);
#endif
	} else {
	  erg=SQLExecute(stmt);
	}
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
        
        fixup_output_binds();

	return true;
}

bool odbccursor::nextResultSet(bool *next_result_set_available) {
        *next_result_set_available = false;
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
		if (col[i].type==SQL_WVARCHAR || col[i].type==SQL_WCHAR) {
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
			if (col[i].type==SQL_TYPE_TIMESTAMP || col[i].type==SQL_TYPE_DATE) {
				erg=SQLBindCol(stmt,i+1,SQL_C_CHAR,
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
	SQLCHAR		state[SQL_SQLSTATE_SIZE+1];
	SQLINTEGER	nativeerrnum;
	SQLSMALLINT	errlength;

        memset(state, 0, sizeof(state));
        
        SQLGetDiagRec(SQL_HANDLE_STMT,stmt,1,state,&nativeerrnum,
                      (SQLCHAR *)errorbuffer,errorbufferlength,
                      &errlength);

	// set return values
	*errorlength=errlength;
	*errorcode=nativeerrnum;
	*liveconnection= is_liveconnection(state, nativeerrnum);
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
		case SQL_WCHAR:
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
	        case SQL_WVARCHAR:
	        case SQL_GUID:
	        case SQL_WLONGVARCHAR:
	        case -150: // sql_variant
	        case -152: // xml
			return VARCHAR_DATATYPE;
	        case -155: // datetimeoffset
	        case SQL_DATETIME:
		        return DATETIME_DATATYPE;
	        case -154: // time
		        return TIME_DATATYPE;
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
		if (col[i].type==SQL_WVARCHAR || col[i].type==SQL_WCHAR) {
			if (indicator[i]!=-1 && field[i]) {
				char *u=conv_to_user_coding(field[i]);
				int len=charstring::length(u);
				if (len >= sizeof(field[i])) {
				  len = sizeof(field[i]) - 1;
				}
				charstring::copy(field[i],u,len);
				field[i][len]=0;
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
        if (stmt != NULL) {
	  SQLCloseCursor(stmt);
          // The msdn.microsoft.com documentation says that
          // this function is equivalent to SQLFreeStmt with SQL_CLOSE
          // But then we would want to set stmt to NULL here.
          // But then we get SQLExecute.c][170]Error: SQL_INVALID_HANDLE
          // So obviously the microsoft documentation is wrong.
         }

        reset_outbind_index();
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrserverconnection *new_odbcconnection(
						sqlrservercontroller *cont) {
		return new odbcconnection(cont);
	}
}
