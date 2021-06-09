// Copyright (c) 1999-2018 David Muse
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
#include <rudiments/sys.h>

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
	char		table[4096];
	uint16_t	tablelength;
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

struct charbind {
	char		*value;
	uint32_t	valuesize;
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
		bool		inputBindBlob(const char *variable,
						uint16_t variablesize,
						const char *value,
						uint32_t valuesize,
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
		bool		inputOutputBind(const char *variable, 
						uint16_t variablesize,
						char *value, 
						uint32_t valuesize,
						int16_t *isnull);
		bool		inputOutputBind(const char *variable, 
						uint16_t variablesize,
						int64_t *value,
						int16_t *isnull);
		bool		inputOutputBind(const char *variable,
						uint16_t variablesize,
						double *value,
						uint32_t *precision,
						uint32_t *scale,
						int16_t *isnull);
		bool		inputOutputBind(const char *variable,
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
		bool		handleColumns(bool getcolumninfo,
						bool bindcolumns);
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
		const char	*getColumnTable(uint32_t i);
		uint16_t	getColumnTableLength(uint32_t i);
		bool		noRowsToReturn();
		bool		fetchRow(bool *error);
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
		bool		nextResultSet(bool *nextresultsetavailable);
		void		closeResultSet();

		bool		columnInfoIsValidAfterPrepare();

#if (ODBCVER >= 0x0300) && defined(SQLCOLATTRIBUTE_SQLLEN)
		bool		isLob(SQLLEN type);
#else
		bool		isLob(SQLINTEGER type);
#endif

		void		setConvCharError(const char *baseerror,
						const char *detailerror);


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
		charbind	**outcharbind;
		int16_t		**outisnullptr;
		datebind	**inoutdatebind;
		charbind	**inoutcharbind;
		int16_t		**inoutisnullptr;
		#ifdef SQLBINDPARAMETER_SQLLEN
		SQLLEN		*outisnull;
		SQLLEN		*inoutisnull;
		SQLLEN		sqlnulldata;
		#else
		SQLINTEGER	*outisnull;
		SQLINTEGER	*inoutisnull;
		SQLINTEGER	sqlnulldata;
		#endif

		bool		bindformaterror;

		uint32_t	row;
		uint32_t	maxrow;
		uint32_t	totalrows;

		stringbuffer	errormsg;

		#ifdef HAVE_SQLCONNECTW
		singlylinkedlist<char *>	ucsinbindstrings;
		#endif

		bool		columninfoisvalidafterprepare;

		odbcconnection	*odbcconn;

		char	columnnamescratch[4096];
};

class SQLRSERVER_DLLSPEC odbcconnection : public sqlrserverconnection {
	friend class odbccursor;
	public:
			odbcconnection(sqlrservercontroller *cont);
			~odbcconnection();
	private:
		void		handleConnectString();
		bool		logIn(const char **error, const char **warning);
		char		*odbcDriverConnectionString(
						const char *userasc,
						const char *passwordasc);
		void		pushConnstrValue(char **pptr,
						size_t *pbuffavail,
						const char *keyword,
						const char *value);
		char		*traceFileName(const char *tracefilenameformat);
		const char	*logInError(const char *errmsg);
		sqlrservercursor	*newCursor(uint16_t id);
		void		deleteCursor(sqlrservercursor *curs);
		void		logOut();
		#if (ODBCVER>=0x0300)
		bool		autoCommitOn();
		bool		autoCommitOff();
		bool		supportsAutoCommit();
		const char	*beginTransactionQuery();
		bool		commit();
		bool		rollback();
		void		errorMessage(char *errorbuffer,
						uint32_t errorbufferlength,
						uint32_t *errorlength,
						int64_t	*errorcode,
						bool *liveconnection);
		#endif
		bool		isLiveConnection(SQLCHAR *state);
		bool		ping();
		const char	*identify();
		const char	*dbVersion();
		const char	*bindFormat();
		const char	*getLastInsertIdQuery();
		bool		getListsByApiCalls();
		bool		getDatabaseList(sqlrservercursor *cursor,
						const char *wild);
		bool		getSchemaList(sqlrservercursor *cursor,
						const char *wild);
		bool		getTableList(sqlrservercursor *cursor,
						const char *wild,
						uint16_t objecttypes);
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
		const char	*dbHostNameQuery();
		const char	*dbIpAddressQuery();


		SQLRETURN	erg;
		SQLHENV		env;
		SQLHDBC		dbc;

		const char	*driver;
		const char	*driverconnect;
		const char	*dsn;
		const char	*server;
		const char	*db;
		const char	*trace;
		const char	*tracefile;
		const char	*identity;
		const char	*odbcversion;
		const char	*lastinsertidquery;
		bool		mars;
		bool		getcolumntables;
		const char	*overrideschema;
		bool		unicode;
		const char	*ncharencoding;

		stringbuffer	errormessage;

		char		dbversion[512];

		const char	*begintxquery;
		bool		usecharforlobbind;
		SQLSMALLINT	fractionscale;
		bool		supportsfraction;
		bool		timestampfortime;
		uint32_t	maxallowedvarcharbindlength;
		uint32_t	maxvarcharbindlength;
		SQLINTEGER	*columninfonotvalidyeterror;
		bool		sqltypedatetosqlcbinary;
		bool		fetchlobsasstrings;

		#if (ODBCVER>=0x0300)
		stringbuffer	errormsg;
		#endif
};

#ifdef HAVE_SQLCONNECTW
#include <iconv.h>

void printerror(const char *error) {
	char	*err=error::getErrorString();
	stderror.printf("%s: %d - %s\n",error,error::getErrorNumber(),err);
	delete[] err;
}

size_t isFixed4Byte(const char *encoding) {
	// FIXME: support other encodings
	return (charstring::contains(encoding,"UCS4") ||
			charstring::contains(encoding,"UCS-4") ||
			charstring::contains(encoding,"UTF32") ||
			charstring::contains(encoding,"UTF-32"));
}

size_t isFixed2Byte(const char *encoding) {
	// FIXME: support other encodings
	return (charstring::contains(encoding,"UCS2") ||
			charstring::contains(encoding,"UCS-2"));
}

size_t isVariable2Byte(const char *encoding) {
	// FIXME: support other encodings
	return (charstring::contains(encoding,"UTF16") ||
			charstring::contains(encoding,"UTF-16"));
}

size_t isVariable1Byte(const char *encoding) {
	return (!isFixed4Byte(encoding) &&
		!isFixed2Byte(encoding) &&
		!isVariable2Byte(encoding));
}

// returns number of characters in the (null-terminated) string
size_t len(const char *str, const char *encoding) {

	const char	*ptr=str;
	size_t		res=0;

	if (isFixed2Byte(encoding)) {

		// skip any byte-order mark
		// FIXME: assumes encoding supports a byte order mark
		// FIXME: handle nulls
		if (*ptr==(char)0xEF &&
			*(ptr+1)==(char)0xBB &&
			*(ptr+2)==(char)0xBF) {
			ptr+=3;
		}

		while (*ptr || *(ptr+1)) {
			res++;
			ptr+=2;
		}

	} else if (isFixed4Byte(encoding)) {

		// skip any byte-order mark
		// FIXME: assumes encoding supports a byte order mark
		// FIXME: handle nulls
		if ((*ptr=='\0' && *(ptr+1)=='\0' &&
			*(ptr+2)==(char)0xFE && *(ptr+3)==(char)0xFF) ||
			(*ptr==(char)0xFF && *(ptr+1)==(char)0xFE &&
			*(ptr+2)=='\0' && *(ptr+3)=='\0')) {
			ptr+=4;
		}

		while (*ptr || *(ptr+1) || *(ptr+2) || *(ptr+3)) {
			res++;
			ptr+=4;
		}

	} else if (isVariable2Byte(encoding)) {

		size_t	offset=0;

		// look for byte-order mark and update offset if necessary
		// FIXME: assumes encoding supports a byte order mark
		// FIXME: handle nulls
		if (*ptr==(char)0xFE && *(ptr+1)==(char)0xFF) {
			ptr+=2;
		} else if (*ptr==(char)0xFF && *(ptr+1)==(char)0xFE) {
			offset=1;
			ptr+=2;
		}

		while (*ptr || *(ptr+1)) {
			res++;
			// FIXME: assumes UTF-16
			if (*(ptr+offset)>=(char)0xD8 &&
				*(ptr+offset)<=(char)0xDF) {
				ptr+=4;
			} else {
				ptr+=2;
			}
		}

	} else if (isVariable1Byte(encoding)) {

		while (*ptr) {
			res++;
			// FIXME: assumes UTF-8
			if (*ptr<192) {
				ptr++;
			} else if (*ptr<224) {
				ptr+=2;
			} else if (*ptr<240) {
				ptr+=3;
			} else {
				ptr+=4;
			}
		}
		
	} else {
		res=charstring::length(str);
	}
	return res;
}

// returns number of bytes in the (null-terminated) string
size_t size(const char *str, const char *encoding) {

	const char	*ptr=str;
	size_t		res=0;

	if (isFixed2Byte(encoding)) {

		// skip any byte-order mark
		// FIXME: assumes encoding supports a byte order mark
		// FIXME: handle nulls
		if (*ptr==(char)0xEF &&
			*(ptr+1)==(char)0xBB &&
			*(ptr+2)==(char)0xBF) {
			ptr+=3;
		}

		while (*ptr || *(ptr+1)) {
			res+=2;
			ptr+=2;
		}

	} else if (isFixed4Byte(encoding)) {

		// skip any byte-order mark
		// FIXME: assumes encoding supports a byte order mark
		// FIXME: handle nulls
		if ((*ptr=='\0' && *(ptr+1)=='\0' &&
			*(ptr+2)==(char)0xFE && *(ptr+3)==(char)0xFF) ||
			(*ptr==(char)0xFF && *(ptr+1)==(char)0xFE &&
			*(ptr+2)=='\0' && *(ptr+3)=='\0')) {
			ptr+=4;
		}

		while (*ptr || *(ptr+1) || *(ptr+2) || *(ptr+3)) {
			res+=4;
			ptr+=4;
		}

	} else if (isVariable2Byte(encoding)) {

		size_t	offset=0;

		// look for byte-order mark and update offset if necessary
		// FIXME: assumes encoding supports a byte order mark
		// FIXME: handle nulls
		if (*ptr==(char)0xFE && *(ptr+1)==(char)0xFF) {
			res+=2;
			ptr+=2;
		} else if (*ptr==(char)0xFF && *(ptr+1)==(char)0xFE) {
			offset=1;
			res+=2;
			ptr+=2;
		}

		while (*ptr || *(ptr+1)) {
			// FIXME: assumes UTF-16
			if (*(ptr+offset)>=(char)0xD8 &&
				*(ptr+offset)<=(char)0xDF) {
				res+=4;
				ptr+=4;
			} else {
				res+=2;
				ptr+=2;
			}
		}

	} else if (isVariable1Byte(encoding)) {

		while (*ptr) {
			// FIXME: assumes UTF-8
			if (*ptr<192) {
				res++;
				ptr++;
			} else if (*ptr<224) {
				res+=2;
				ptr+=2;
			} else if (*ptr<240) {
				res+=3;
				ptr+=3;
			} else {
				res+=4;
				ptr+=4;
			}
		}
		
	} else {
		res=charstring::length(str);
	}
	return res;
}

size_t nullSize(const char *encoding) {
	if (isFixed2Byte(encoding) || isVariable2Byte(encoding)) {
		return 2;
	} else if (isFixed4Byte(encoding)) {
		return 4;
	} else {
		return 1;
	}
}

size_t byteOrderMarkSize(const char *encoding) {
	if (isVariable1Byte(encoding)) {
		return 3;
	} else if (isVariable2Byte(encoding)) {
		return 2;
	} else if (isFixed4Byte(encoding)) {
		return 4;
	} else {
		return 0;
	}
}

char *convertCharset(const char *inbuf,
				size_t insize,
				const char *inenc,
				const char *outenc,
				char **error) {

	// initialize error
	if (error) {
		*error=NULL;
	}

	// get size of null terminator
	size_t	nullsize=nullSize(outenc);

	// get size of byte order mark
	size_t	bosize=byteOrderMarkSize(outenc);

	// calculate size of output buffer (in bytes)
	size_t	multiplier=4;
	if (isFixed4Byte(inenc)) {
		multiplier=1;
	} else if (isFixed2Byte(inenc)) {
		if (isFixed2Byte(inenc)) {
			multiplier=1;
		} else if (isFixed4Byte(inenc)) {
			multiplier=2;
		}
	}
	size_t	outsize=len(inbuf,inenc)*multiplier+bosize+nullsize;

	// allocate the output buffer
	char	*outbuf=new char[outsize];

	// capture initial buffer positions and sizes
	const char	*inptr=inbuf;
	char		*outptr=outbuf;
	size_t		insizebefore=insize;
	size_t		outsizebefore=outsize;

	// open converter
	// FIXME: reuse this rather than re-creating it over and over
	iconv_t	cd=iconv_open(outenc,inenc);
	if (cd==(iconv_t)-1) {
		if (error) {
			char	*err=error::getErrorString();
			charstring::printf(error,"iconv_open(): %s",err);
			delete[] err;
		}
		// null-terminate the output
		bytestring::zero(outptr,nullsize);
		return outbuf;
	}

	// convert
	if (iconv(cd,
			#ifndef ICONV_CONST_CHAR
			(char **)
			#endif
			&inptr,
			&insize,
			&outptr,
			&outsize)==(size_t)-1) {
		if (error) {
			char	*err=error::getErrorString();
			charstring::printf(error,
				"iconv(): %s (in=%ld/%ld out=%ld/%ld)",
				err,insizebefore,insize,outsizebefore,outsize);
			delete[] err;
		}
	}

	// SQL Server doesn't like UTF-16 values to have a byte-order mark
	// (and it wants them to be big-endian)
	// FIXME: make this configurable somehow...
	if (isVariable2Byte(outenc) &&
		((outbuf[0]==(char)0xFF && outbuf[1]==(char)0xFE) ||
		(outbuf[0]==(char)0xFE && outbuf[1]==(char)0xFF))) {
		bytestring::copyWithOverlap(outbuf,outbuf+2,outptr-outbuf-2);
		outptr-=2;
	}

	// null-terminate the output
	bytestring::zero(outptr,nullsize);

	// close converter
	if (iconv_close(cd)!=0) {
		// don't override any previously set error here
		if (error && !*error) {
			char	*err=error::getErrorString();
			charstring::printf(error,"iconv_open(): %s",err);
			delete[] err;
		}
	}
	return outbuf;
}

char *convertCharset(const char *inbuf,
				const char *inenc,
				const char *outenc,
				char **error) {
	return convertCharset(inbuf,size(inbuf,inenc),inenc,outenc,error);
}
#endif

odbcconnection::odbcconnection(sqlrservercontroller *cont) :
					sqlrserverconnection(cont) {
	driver=NULL;
	driverconnect=NULL;
	dsn=NULL;
	server=NULL;
	db=NULL;
	trace=NULL;
	tracefile=NULL;
	identity=NULL;
	odbcversion=NULL;
	lastinsertidquery=NULL;
	mars=false;
	getcolumntables=false;
	overrideschema=NULL;
	unicode=true;
	ncharencoding=NULL;
	columninfonotvalidyeterror=NULL;
}

odbcconnection::~odbcconnection() {
	delete[] columninfonotvalidyeterror;
}

void odbcconnection::handleConnectString() {

	sqlrserverconnection::handleConnectString();

	driver=cont->getConnectStringValue("driver");
	driverconnect=cont->getConnectStringValue("driverconnect");
	dsn=cont->getConnectStringValue("dsn");
	server=cont->getConnectStringValue("server");
	db=cont->getConnectStringValue("db");

	trace=cont->getConnectStringValue("trace");
	tracefile=cont->getConnectStringValue("tracefile");

	identity=cont->getConnectStringValue("identity");

	odbcversion=cont->getConnectStringValue("odbcversion");

	lastinsertidquery=cont->getConnectStringValue("lastinsertidquery");

	mars=charstring::isYes(cont->getConnectStringValue("mars"));
	getcolumntables=charstring::isYes(
			cont->getConnectStringValue("getcolumntables"));
	const char	*os=cont->getConnectStringValue("overrideschema");
	if (!charstring::isNullOrEmpty(os)) {
		overrideschema=os;
	}

	unicode=!charstring::isNo(cont->getConnectStringValue("unicode"));
	ncharencoding=cont->getConnectStringValue("ncharencoding");
	if (charstring::isNullOrEmpty(ncharencoding)) {
		ncharencoding="UCS-2//TRANSLIT";
	}

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
		#else
		SQLFreeEnv(env);
		#endif
		return false;
	}

#if (ODBCVER >= 0x0300)
	// trace paramters may have been set in the DSN,
	// but we can also override them here...
	if (!charstring::isNullOrEmpty(tracefile)) {
		// FIXME: does this need to persist?
		char	*tracefilename=traceFileName(tracefile);
		erg=SQLSetConnectAttr(dbc,
				SQL_ATTR_TRACEFILE,
				(SQLPOINTER *)tracefilename,
				SQL_NTS);
		delete[] tracefilename;
	}
	if (charstring::isYes(trace)) {
		erg=SQLSetConnectAttr(dbc,
				SQL_ATTR_TRACE,
				(SQLPOINTER *)SQL_OPT_TRACE_ON,
				0);
	} else if (charstring::isNo(trace)) {
		erg=SQLSetConnectAttr(dbc,
				SQL_ATTR_TRACE,
				(SQLPOINTER *)SQL_OPT_TRACE_OFF,
				0);
	}

	// set the initial db
	if (!charstring::isNullOrEmpty(db)) {
		erg = SQLSetConnectAttr(dbc,SQL_ATTR_CURRENT_CATALOG,
					(SQLPOINTER *)db,SQL_NTS);
		if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
			*error="Failed to set database";
			SQLFreeHandle(SQL_HANDLE_DBC,dbc);
			SQLFreeHandle(SQL_HANDLE_ENV,env);
			return false;
		}
	}

	// set the connect timeout
	uint64_t	connecttimeout=cont->getConnectTimeout();
	if (connecttimeout) {
		erg=SQLSetConnectAttr(dbc,SQL_LOGIN_TIMEOUT,
					(SQLPOINTER *)connecttimeout,0);
		if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
			*error="Failed to set connect timeout";
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
	const char	*userasc=cont->getUser();
	const char	*passwordasc=cont->getPassword();

	if (!charstring::isNullOrEmpty(driver)) {

		char	*sqlconnectdriverstring=
			odbcDriverConnectionString(userasc,passwordasc);

		// These values are useful to look at from the debugger,
		// it is not a good security practice to directly log
		// the string, because it might contain a plaintext password.
		SQLCHAR		outconnectionstring[2048];
		SQLSMALLINT	outconnectionstringlen;
		erg=SQLDriverConnect(dbc,
				(SQLHWND)NULL,
				(SQLCHAR *)sqlconnectdriverstring,
				(SQLSMALLINT)charstring::length(
						sqlconnectdriverstring),
				outconnectionstring,
				(SQLSMALLINT)sizeof(outconnectionstring),
				&outconnectionstringlen,
				(SQLSMALLINT)SQL_DRIVER_NOPROMPT);

		delete[] sqlconnectdriverstring;

	} else {

		const char	*dsnasc=dsn;

		#ifdef HAVE_SQLCONNECTW
		if (unicode) {
			char	*dsnucs=(dsnasc)?
					convertCharset(dsnasc,
							"UTF-8",
							"UCS-2//TRANSLIT",
							NULL):NULL;
			char	*userucs=(userasc)?
					convertCharset(userasc,
							"UTF-8",
							"UCS-2//TRANSLIT",
							NULL):NULL;
			char	*passworducs=(passwordasc)?
					convertCharset(passwordasc,
							"UTF-8",
							"UCS-2//TRANSLIT",
							NULL):NULL;
			erg=SQLConnectW(dbc,(SQLWCHAR *)dsnucs,SQL_NTS,
					(SQLWCHAR *)userucs,SQL_NTS,
					(SQLWCHAR *)passworducs,SQL_NTS);
			delete[] dsnucs;
			delete[] userucs;
			delete[] passworducs;
		} else {
		#endif
			erg=SQLConnect(dbc,(SQLCHAR *)dsnasc,SQL_NTS,
					(SQLCHAR *)userasc,SQL_NTS,
					(SQLCHAR *)passwordasc,SQL_NTS);
		#ifdef HAVE_SQLCONNECTW
		}
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

	// get the type of database
	char		dbmsnamebuffer[1024];
	dbmsnamebuffer[0]='\0';
	SQLSMALLINT	dbmsnamelen=0;
	if (SQLGetInfo(dbc,
			SQL_DBMS_NAME,
			dbmsnamebuffer,
			sizeof(dbmsnamebuffer),
			&dbmsnamelen)==SQL_SUCCESS) {
		dbmsnamebuffer[dbmsnamelen]='\0';
	}

	// set some default params
	begintxquery=sqlrserverconnection::beginTransactionQuery();
	usecharforlobbind=true;
	// When binding dates using SQLBindParameter, the "decimal
	// digits" parameter refers to the number of digits in the
	// "fraction" part of the date.  Since that is in nanoseconds
	// (billionths of a second (0-999999999)) in ODBC, the
	// "decimal digits" parameter must be 9 to accomodate the
	// full range.
	fractionscale=9;
	supportsfraction=true;
	timestampfortime=true;
	maxallowedvarcharbindlength=0;
	maxvarcharbindlength=0;
	columninfonotvalidyeterror=NULL;
	sqltypedatetosqlcbinary=true;
	fetchlobsasstrings=false;

	// override some default params based on the db-type
	if (!charstring::compare(dbmsnamebuffer,"Teradata")) {
		begintxquery="BT";
		usecharforlobbind=false;
		// See below...  Teradata only supports 6 digits though.
		fractionscale=6;
		// Well... Teradata theoretically supports 6 digits of
		// fractional seconds, but any attempt to actually bind
		// fractional seconds results in "[Teradata][Support] (40520)
		// Datetime field overflow resulting from invalid datetime."
		supportsfraction=false;
		// Teradata doesn't like it if you bind a SQL_TIMESTAMP_STRUCT
		// to a TIME datatype.
		timestampfortime=false;
	} else if (!charstring::compare(dbmsnamebuffer,
						"Microsoft SQL Server",20)) {
		// SQL Server defines a varchar/nvarchar as 4000 characters
		// long, but you can actually store up to 2Gb in them.
		// However, if you send a valuesize > 4000 characters during
		// a bind, then something in the chain doesn't like it.  To
		// work around this, you have to send 0.  Go figure...
		maxallowedvarcharbindlength=4000;
		maxvarcharbindlength=0;

		// With MS SQL Server, there are various cases where column
		// metadata can't be fetched until post-execute.  For example:
		//
		// sqlrsh commands like:
		//	inputbind 1 = 'hello';
		//	select ?;
		// fail with:
		//	11521:
		//	[Microsoft][ODBC Driver 17 for SQL Server][SQL Server]
		//	The metadata could not be determined because statement
		//	'select @P1' uses an undeclared parameter in a context
		//	that affects its metadata.
		// Sored procedures that optionally execute selects which
		// return different numbers of columns fail with:
		// 	11512:
		// 	[Microsoft][ODBC Driver 17 for SQL Server][SQL Server]
		// 	The metadata could not be determined because the
		// 	statement '...some select query...' is not compatible
		// 	with the statement '...some other select query...' in
		// 	procedure '...some procedure...'.
		// So, in cases like this we can catch the error and defer
		// getting/sending column info until later.
		// Basically, it's error codes 11509-11530 but there could be
		// others too...
		columninfonotvalidyeterror=new SQLINTEGER[23];
		columninfonotvalidyeterror[0]=11509;
		columninfonotvalidyeterror[1]=11510;
		columninfonotvalidyeterror[2]=11511;
		columninfonotvalidyeterror[3]=11512;
		columninfonotvalidyeterror[4]=11513;
		columninfonotvalidyeterror[5]=11514;
		columninfonotvalidyeterror[6]=11515;
		columninfonotvalidyeterror[7]=11516;
		columninfonotvalidyeterror[8]=11517;
		columninfonotvalidyeterror[9]=11518;
		columninfonotvalidyeterror[10]=11519;
		columninfonotvalidyeterror[11]=11520;
		columninfonotvalidyeterror[12]=11521;
		columninfonotvalidyeterror[13]=11522;
		columninfonotvalidyeterror[14]=11523;
		columninfonotvalidyeterror[15]=11524;
		columninfonotvalidyeterror[16]=11525;
		columninfonotvalidyeterror[17]=11526;
		columninfonotvalidyeterror[18]=11527;
		columninfonotvalidyeterror[19]=11528;
		columninfonotvalidyeterror[20]=11529;
		columninfonotvalidyeterror[21]=11530;
		columninfonotvalidyeterror[22]=0;

		// SQL Server doesn't like for you to convert SQL_TYPE_DATE
		// to SQL_C_BINARY
		sqltypedatetosqlcbinary=false;

		// SQL Server has trouble mixing SQLBindCol and SQLGetData.
		// If you SQLBindCol a column (eg. column 4) then you can't use
		// SQLGetData to fetch an earlier column (eg. column 3).
		// A workaround is to use SQLBindCol in all cases and fetch
		// LOBs as strings.
		fetchlobsasstrings=true;

		// SQL Server likes "BEGIN TRANSACTION" to begin transactions.
		begintxquery="BEGIN TRANSACTION";
	}

	return true;
}

char *odbcconnection::traceFileName(const char *tracefilenameformat) {

	// This would be a good candidate for promotion to rudiments,
	// These format operators are enough to provide a unique log file
	// name, per-process:
	// %p means PID
	// %t means a timestamp.
	// %h means the hostname.
	// If any of these appears more than once then the output filename
	// may be truncated.

	pid_t	pid=process::getProcessId();

	datetime dt;
	dt.getSystemDateAndTime();
	time_t	now=dt.getEpoch();

	char	*hostname=sys::getHostName();

	size_t	tracefilenamebuffersize=charstring::length(tracefilenameformat);
	tracefilenamebuffersize+=charstring::integerLength((int64_t)pid);
	tracefilenamebuffersize+=charstring::integerLength((int64_t)now);
	tracefilenamebuffersize+=charstring::length(hostname);
	tracefilenamebuffersize+=1;

	char		*tracefilename=new char[tracefilenamebuffersize];
	char		*outptr=tracefilename;
	size_t		outptrsize=tracefilenamebuffersize-1;
	const char	*ptr=tracefilenameformat;
	*outptr=0;
	while (*ptr && (outptrsize>0)) {
		if (*ptr=='%') {
			char	*insertstring=NULL;
			int64_t	insertnumber=0;
			ptr++;
			if (*ptr=='p') {
				insertnumber=pid;
			} else if (*ptr=='t') {
				insertnumber=now;
			} else if (*ptr=='h') {
				insertstring=hostname;
			}
			if (insertstring!=NULL) {
				charstring::printf(
					outptr,outptrsize,"%s",insertstring);
			} else {
				charstring::printf(
					outptr,outptrsize,"%ld",insertnumber);
			}
			ptr++;
			size_t	outptrinc=charstring::length(outptr);
			outptrsize-=outptrinc;
			outptr+=outptrinc;
		} else {
			*outptr++=*ptr++;
			outptrsize--;
			*outptr=0;
		}
	}
	delete[] hostname;
	return tracefilename;
}

char *odbcconnection::odbcDriverConnectionString(const char *userasc,
						const char *passwordasc) {

	// FIXME: use a stringbuffer
	size_t	buffsize=1024;
	size_t	buffavail=buffsize;
	char	*buff=new char[buffsize];
	char	*ptr=buff;

	// At least with unixODBC, we find that if the DSN is not the first
	// field, there will be an SQLDriverConnect error of:
	//
	//	state 08001
	//	errnum 0
	//	message [unixODBC][Microsoft][ODBC Driver 11 for SQL Server]Neither DSN nor SERVER keyword supplied
	//
	// If DSN is specified then the DRIVER seems to be ignored. This makes
	// sense actually.

	if (!charstring::isNullOrEmpty(dsn)) {
		pushConnstrValue(&ptr,&buffavail,"DSN",dsn);
	}
	if (!charstring::isNullOrEmpty(driver)) {
		pushConnstrValue(&ptr,&buffavail,"DRIVER",driver);
	}
	if (!charstring::isNullOrEmpty(driverconnect)) {
		// we push this extra info right after the DSN or DRIVER
		// so that we can clearly see it in the unixODBC trace which
		// tends to truncate at about 130 characters.
		unsigned char	*rawdriverconnect=
				charstring::base64Decode(driverconnect);
		pushConnstrValue(&ptr,&buffavail,NULL,
				(const char *)rawdriverconnect);
		delete[] rawdriverconnect;
	}
	if (!(charstring::isNullOrEmpty(server) ||
			charstring::contains(buff,";SERVER="))) {
		pushConnstrValue(&ptr,&buffavail,"SERVER",server);
	}
	if (!(charstring::isNullOrEmpty(userasc) ||
			charstring::contains(buff,";UID="))) {
		pushConnstrValue(&ptr,&buffavail,"UID",userasc);
	}
	if (!(charstring::isNullOrEmpty(passwordasc) ||
			charstring::contains(buff,";PWD="))) {
		pushConnstrValue(&ptr,&buffavail,"PWD",passwordasc);
	}
	if (!charstring::contains(buff, ";WSID=")) {
		pushConnstrValue(&ptr,&buffavail,"WSID",sys::getHostName());
	}
	if (!charstring::contains(buff, ";APP=")) {
		// FIXME: use one of the SQLR macros here...
		pushConnstrValue(&ptr,&buffavail,
					"APP","SQLRelay-" SQLR_VERSION);
	}
	return buff;
}

void odbcconnection::pushConnstrValue(char **pptr, size_t *pbuffavail,
				const char *keyword, const char *value) {

	const char	*openbracket="";
	const char	*closebracket="";
	char		*ptr=*pptr;
	size_t		buffavail=*pbuffavail;
	if (charstring::contains(value,';')) {
		openbracket="{";
		closebracket="}";
	}
	if (keyword == NULL) {
		// here we are just going to push a raw value.
		// With an extra semicolon just in case.
		charstring::printf(ptr,buffavail,"%s;",value);
	} else {
		charstring::printf(ptr,buffavail,"%s=%s%s%s;",
				keyword,openbracket,value,closebracket);
	}
	size_t	ptrinc=charstring::length(ptr);
	ptr+=ptrinc;
	buffavail-=ptrinc;
	*pptr=ptr;
	*pbuffavail=buffavail;
}

const char *odbcconnection::logInError(const char *errmsg) {

	errormessage.clear();
	if (errmsg) {
		errormessage.append(errmsg)->append(": ");
	}

	// get the error message
	SQLCHAR		state[SQL_SQLSTATE_SIZE+1];
	SQLINTEGER	nativeerrnum;
	SQLCHAR		errorbuffer[1024];
	SQLSMALLINT	errlength;

	bytestring::zero(state,sizeof(state));

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
	dbc=NULL;
	env=NULL;
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
	// FIXME: not true for all db's
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

	odbccursor	*odbccur=(odbccursor *)cursor;

	// allocate the statement handle
	if (!odbccur->allocateStatementHandle()) {
		return false;
	}

	if (getcolumntables) {
		SQLSetStmtAttr(odbccur->stmt,SQL_ATTR_CURSOR_TYPE,
				(SQLPOINTER)SQL_CURSOR_STATIC,
				SQL_IS_INTEGER);
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
	return (retval)?odbccur->handleColumns(true,true):false;
}

bool odbcconnection::getSchemaList(sqlrservercursor *cursor,
						const char *wild) {

	odbccursor	*odbccur=(odbccursor *)cursor;

	// allocate the statement handle
	if (!odbccur->allocateStatementHandle()) {
		return false;
	}

	if (getcolumntables) {
		SQLSetStmtAttr(odbccur->stmt,SQL_ATTR_CURSOR_TYPE,
				(SQLPOINTER)SQL_CURSOR_STATIC,
				SQL_IS_INTEGER);
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
	return (retval)?odbccur->handleColumns(true,true):false;
}

bool odbcconnection::getTableList(sqlrservercursor *cursor,
					const char *wild,
					uint16_t objecttypes) {

	odbccursor	*odbccur=(odbccursor *)cursor;

	// allocate the statement handle
	if (!odbccur->allocateStatementHandle()) {
		return false;
	}

	if (getcolumntables) {
		SQLSetStmtAttr(odbccur->stmt,SQL_ATTR_CURSOR_TYPE,
				(SQLPOINTER)SQL_CURSOR_STATIC,
				SQL_IS_INTEGER);
	}

	// initialize column and row counts
	odbccur->initializeColCounts();
	odbccur->initializeRowCounts();

	// various buffers/pointers
	char		catalogbuffer[1024];
	const char	*catalog=NULL;
	char		schemabuffer[1024];
	const char	*schema="";
	// FIXME: should this be SQL_ALL_TABLES?
	const char	*table="%";

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
	if (overrideschema) {
		schema=overrideschema;
	} else {
		SQLSMALLINT	schemalen=0;
		if (SQLGetInfo(dbc,
				SQL_USER_NAME,
				schemabuffer,
				sizeof(schemabuffer),
				&schemalen)==SQL_SUCCESS) {
			schemabuffer[schemalen]='\0';
			schema=schemabuffer;
		}
	}

	// split the object name
	if (!charstring::isNullOrEmpty(wild)) {
		cont->splitObjectName(catalogbuffer,schemabuffer,wild,
						&catalog,&schema,&table);
	}

	stringbuffer	tabletype;
	if (objecttypes&DB_OBJECT_TABLE) {
		tabletype.append("TABLE");
	}
	if (objecttypes&DB_OBJECT_VIEW) {
		if (tabletype.getSize()) {
			tabletype.append(',');
		}
		tabletype.append("VIEW");
	}
	if (objecttypes&DB_OBJECT_ALIAS) {
		if (tabletype.getSize()) {
			tabletype.append(',');
		}
		tabletype.append("ALIAS");
	}
	if (objecttypes&DB_OBJECT_SYNONYM) {
		if (tabletype.getSize()) {
			tabletype.append(',');
		}
		tabletype.append("SYNONYM");
	}

	// get the table list
	erg=SQLTables(odbccur->stmt,
			(SQLCHAR *)catalog,SQL_NTS,
			(SQLCHAR *)schema,SQL_NTS,
			(SQLCHAR *)table,SQL_NTS,
			(SQLCHAR *)tabletype.getString(),SQL_NTS);
	bool	retval=(erg==SQL_SUCCESS || erg==SQL_SUCCESS_WITH_INFO);

	// parse the column information
	return (retval)?odbccur->handleColumns(true,true):false;
}

bool odbcconnection::getTableTypeList(sqlrservercursor *cursor,
					const char *wild) {

	odbccursor	*odbccur=(odbccursor *)cursor;

	// allocate the statement handle
	if (!odbccur->allocateStatementHandle()) {
		return false;
	}

	if (getcolumntables) {
		SQLSetStmtAttr(odbccur->stmt,SQL_ATTR_CURSOR_TYPE,
				(SQLPOINTER)SQL_CURSOR_STATIC,
				SQL_IS_INTEGER);
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
	return (retval)?odbccur->handleColumns(true,true):false;
}

bool odbcconnection::getColumnList(sqlrservercursor *cursor,
					const char *table,
					const char *wild) {

	odbccursor	*odbccur=(odbccursor *)cursor;

	// allocate the statement handle
	if (!odbccur->allocateStatementHandle()) {
		return false;
	}

	if (getcolumntables) {
		SQLSetStmtAttr(odbccur->stmt,SQL_ATTR_CURSOR_TYPE,
				(SQLPOINTER)SQL_CURSOR_STATIC,
				SQL_IS_INTEGER);
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
	if (overrideschema) {
		schema=overrideschema;
	} else {
		SQLSMALLINT	schemalen=0;
		if (SQLGetInfo(dbc,
				SQL_USER_NAME,
				schemabuffer,
				sizeof(schemabuffer),
				&schemalen)==SQL_SUCCESS) {
			schemabuffer[schemalen]='\0';
			schema=schemabuffer;
		}
	}

	// split the table name
	cont->splitObjectName(catalogbuffer,schemabuffer,table,
						&catalog,&schema,&tablename);

	// use % if wild was empty
	wild=(!charstring::isNullOrEmpty(wild))?wild:"%";
		
	// get the column list
	erg=SQLColumns(odbccur->stmt,
			(SQLCHAR *)catalog,SQL_NTS,
			(SQLCHAR *)schema,SQL_NTS,
			(SQLCHAR *)tablename,SQL_NTS,
			(SQLCHAR *)wild,SQL_NTS);
	bool	retval=(erg==SQL_SUCCESS || erg==SQL_SUCCESS_WITH_INFO);

	// parse the column information
	return (retval)?odbccur->handleColumns(true,true):false;
}

bool odbcconnection::getPrimaryKeyList(sqlrservercursor *cursor,
						const char *table,
						const char *wild) {

	odbccursor	*odbccur=(odbccursor *)cursor;

	// allocate the statement handle
	if (!odbccur->allocateStatementHandle()) {
		return false;
	}

	if (getcolumntables) {
		SQLSetStmtAttr(odbccur->stmt,SQL_ATTR_CURSOR_TYPE,
				(SQLPOINTER)SQL_CURSOR_STATIC,
				SQL_IS_INTEGER);
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
	if (overrideschema) {
		schema=overrideschema;
	} else {
		SQLSMALLINT	schemalen=0;
		if (SQLGetInfo(dbc,
				SQL_USER_NAME,
				schemabuffer,
				sizeof(schemabuffer),
				&schemalen)==SQL_SUCCESS) {
			schemabuffer[schemalen]='\0';
			schema=schemabuffer;
		}
	}

	// split the table name
	cont->splitObjectName(catalogbuffer,schemabuffer,table,
						&catalog,&schema,&tablename);

	// get the primary key list
	erg=SQLPrimaryKeys(odbccur->stmt,
			(SQLCHAR *)catalog,SQL_NTS,
			(SQLCHAR *)schema,SQL_NTS,
			(SQLCHAR *)tablename,SQL_NTS);
	bool	retval=(erg==SQL_SUCCESS || erg==SQL_SUCCESS_WITH_INFO);

	// parse the column information
	return (retval)?odbccur->handleColumns(true,true):false;
}

bool odbcconnection::getKeyAndIndexList(sqlrservercursor *cursor,
						const char *table,
						const char *wild) {

	odbccursor	*odbccur=(odbccursor *)cursor;

	// allocate the statement handle
	if (!odbccur->allocateStatementHandle()) {
		return false;
	}

	if (getcolumntables) {
		SQLSetStmtAttr(odbccur->stmt,SQL_ATTR_CURSOR_TYPE,
				(SQLPOINTER)SQL_CURSOR_STATIC,
				SQL_IS_INTEGER);
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
	if (overrideschema) {
		schema=overrideschema;
	} else {
		SQLSMALLINT	schemalen=0;
		if (SQLGetInfo(dbc,
				SQL_USER_NAME,
				schemabuffer,
				sizeof(schemabuffer),
				&schemalen)==SQL_SUCCESS) {
			schemabuffer[schemalen]='\0';
			schema=schemabuffer;
		}
	}

	// split the table name
	cont->splitObjectName(catalogbuffer,schemabuffer,table,
						&catalog,&schema,&tablename);

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

	// parse the column information
	return (retval)?odbccur->handleColumns(true,true):false;
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

	if (getcolumntables) {
		SQLSetStmtAttr(odbccur->stmt,SQL_ATTR_CURSOR_TYPE,
				(SQLPOINTER)SQL_CURSOR_STATIC,
				SQL_IS_INTEGER);
	}

	// initialize column and row counts
	odbccur->initializeColCounts();
	odbccur->initializeRowCounts();

	// Unlike SQLColumns/SQLTables, SQLProcedureColumns wants NULL instead
	// of "" for catalog/schema, to indicate the current catalog/schema.
	// It interprets "" as meaning outside of any catalog/schema.
	char		catalogbuffer[1024];
	const char	*catalog=NULL;
	char		schemabuffer[1024];
	const char	*schema=NULL;
	const char	*proc=NULL;

	// get the current catalog (instance)
	SQLINTEGER	cataloglen=0;
	if (SQLGetConnectAttr(dbc,
				SQL_CURRENT_QUALIFIER,
				catalogbuffer,
				sizeof(catalogbuffer),
				&cataloglen)==SQL_SUCCESS) {
		catalogbuffer[cataloglen]='\0';
	}

	// get the current user (schema)
	if (overrideschema) {
		schema=overrideschema;
	} else {
		SQLSMALLINT	schemalen=0;
		if (SQLGetInfo(dbc,
				SQL_USER_NAME,
				schemabuffer,
				sizeof(schemabuffer),
				&schemalen)==SQL_SUCCESS) {
			schemabuffer[schemalen]='\0';
			schema=schemabuffer;
		}
	}

	// split the procedure name
	cont->splitObjectName(catalogbuffer,schemabuffer,procedure,
						&catalog,&schema,&proc);

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

	// parse the column information
	return (retval)?odbccur->handleColumns(true,true):false;
}

bool odbcconnection::getTypeInfoList(sqlrservercursor *cursor,
					const char *type,
					const char *wild) {

	odbccursor	*odbccur=(odbccursor *)cursor;

	// allocate the statement handle
	if (!odbccur->allocateStatementHandle()) {
		return false;
	}

	if (getcolumntables) {
		SQLSetStmtAttr(odbccur->stmt,SQL_ATTR_CURSOR_TYPE,
				(SQLPOINTER)SQL_CURSOR_STATIC,
				SQL_IS_INTEGER);
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
	return (retval)?odbccur->handleColumns(true,true):false;
}

bool odbcconnection::getProcedureList(sqlrservercursor *cursor,
						const char *wild) {

	odbccursor	*odbccur=(odbccursor *)cursor;

	// allocate the statement handle
	if (!odbccur->allocateStatementHandle()) {
		return false;
	}

	if (getcolumntables) {
		SQLSetStmtAttr(odbccur->stmt,SQL_ATTR_CURSOR_TYPE,
				(SQLPOINTER)SQL_CURSOR_STATIC,
				SQL_IS_INTEGER);
	}

	// initialize column and row counts
	odbccur->initializeColCounts();
	odbccur->initializeRowCounts();

	// get the procedure list
	char		catalogbuffer[1024];
	const char	*catalog=NULL;
	char		schemabuffer[1024];
	const char	*schema="";
	const char	*procname="%";

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
	if (overrideschema) {
		schema=overrideschema;
	} else {
		SQLSMALLINT	schemalen=0;
		if (SQLGetInfo(dbc,
				SQL_USER_NAME,
				schemabuffer,
				sizeof(schemabuffer),
				&schemalen)==SQL_SUCCESS) {
			schemabuffer[schemalen]='\0';
			schema=schemabuffer;
		}
	}

	// split the procedure name
	if (!charstring::isNullOrEmpty(wild)) {
		cont->splitObjectName(catalogbuffer,schemabuffer,wild,
						&catalog,&schema,&procname);
	}

	// get the procedure list
	erg=SQLProcedures(odbccur->stmt,
			(SQLCHAR *)catalog,SQL_NTS,
			(SQLCHAR *)schema,SQL_NTS,
			(SQLCHAR *)procname,SQL_NTS);
	bool	retval=(erg==SQL_SUCCESS || erg==SQL_SUCCESS_WITH_INFO);

	// parse the column information
	return (retval)?odbccur->handleColumns(true,true):false;
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

const char *odbcconnection::beginTransactionQuery() {
	return begintxquery;
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
	SQLCHAR		state[SQL_SQLSTATE_SIZE+1];
	SQLINTEGER	nativeerrnum;
	SQLSMALLINT	errlength;

	bytestring::zero(state,sizeof(state));

	SQLGetDiagRec(SQL_HANDLE_DBC,dbc,1,state,&nativeerrnum,
				(SQLCHAR *)errorbuffer,errorbufferlength,
				&errlength);

	// set return values
	*errorlength=errlength;
	*errorcode=nativeerrnum;
	*liveconnection=isLiveConnection(state);
}
#endif

bool odbcconnection::isLiveConnection(SQLCHAR *state) {
	// TODO: Gain access to the dbc, and in ODBC 3.5 see if
	// SQL_ATTR_CONNECTION_DEAD is SQL_CD_TRUE.
	return bytestring::compare("08S01",state,5) &&
		bytestring::compare("08003",state,5);
}

bool odbcconnection::setIsolationLevel(const char *isolevel) {
	// FIXME: do nothing for now.  see task #422
	return true;
}

const char *odbcconnection::dbHostNameQuery() {
	// FIXME: only works with MS SQL Server
	return "SELECT cast(@@SERVERNAME as varchar(64))";
}

const char *odbcconnection::dbIpAddressQuery() {
	// FIXME: only works with MS SQL Server
	return "SELECT CAST(SERVERPROPERTY('ComputerNamePhysicalNetBIOS') as varchar(64))";
}

odbccursor::odbccursor(sqlrserverconnection *conn, uint16_t id) :
						sqlrservercursor(conn,id) {
	odbcconn=(odbcconnection *)conn;
	stmt=NULL;
	maxbindcount=conn->cont->getConfig()->getMaxBindCount();
	outdatebind=new datebind *[maxbindcount];
	outcharbind=new charbind *[maxbindcount];
	outisnullptr=new int16_t *[maxbindcount];
	inoutdatebind=new datebind *[maxbindcount];
	inoutcharbind=new charbind *[maxbindcount];
	inoutisnullptr=new int16_t *[maxbindcount];
	#ifdef SQLBINDPARAMETER_SQLLEN
	outisnull=new SQLLEN[maxbindcount];
	inoutisnull=new SQLLEN[maxbindcount];
	#else
	outisnull=new SQLINTEGER[maxbindcount];
	inoutisnull=new SQLINTEGER[maxbindcount];
	#endif
	for (uint16_t i=0; i<maxbindcount; i++) {
		outdatebind[i]=NULL;
		outcharbind[i]=NULL;
		outisnullptr[i]=NULL;
		outisnull[i]=0;
		inoutdatebind[i]=NULL;
		inoutcharbind[i]=NULL;
		inoutisnullptr[i]=NULL;
		inoutisnull[i]=0;
	}
	sqlnulldata=SQL_NULL_DATA;
	bindformaterror=false;
	allocateResultSetBuffers(conn->cont->getMaxColumnCount());
	initializeColCounts();
	initializeRowCounts();
}

odbccursor::~odbccursor() {
	delete[] outdatebind;
	delete[] outcharbind;
	delete[] outisnullptr;
	delete[] outisnull;
	delete[] inoutdatebind;
	delete[] inoutcharbind;
	delete[] inoutisnullptr;
	delete[] inoutisnull;
	#ifdef HAVE_SQLCONNECTW
	ucsinbindstrings.clearAndArrayDelete();
	#endif
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

	bindformaterror=false;

	// initialize column count
	initializeColCounts();

	// allocate the statement handle
	if (!allocateStatementHandle()) {
		return false;
	}

	if (odbcconn->getcolumntables && !getExecuteDirect()) {

		// MS SQL Server only returns column table names when using a
		// server cursor or when the query contains a FOR BROWSE clause.
		//
		// Some apps need the table name.
		//
		// Setting the cursor type to static appears to be the least
		// invasive way of influencing the server to use a server cursor
		// and thus return column names.
		//
		// (see more below)
		SQLSetStmtAttr(stmt,SQL_ATTR_CURSOR_TYPE,
				(SQLPOINTER)SQL_CURSOR_STATIC,
				SQL_IS_INTEGER);
	}

	// prepare the query...

	#ifdef HAVE_SQLCONNECTW
	if (odbcconn->unicode) {

		ucsinbindstrings.clearAndArrayDelete();

		if (getExecuteDirect()) {
			return true;
		}

		char	*err=NULL;
		char	*queryucs=convertCharset(query,length,
						"UTF-8","UCS-2//TRANSLIT",
						&err);
		if (err) {
			delete[] queryucs;
			setConvCharError("prepare query",err);
			return false;
		}
		erg=SQLPrepareW(stmt,(SQLWCHAR *)queryucs,SQL_NTS);
		delete[] queryucs;
	} else {
	#endif
		if (getExecuteDirect()) {
			return true;
		}
		erg=SQLPrepare(stmt,(SQLCHAR *)query,length);
	#ifdef HAVE_SQLCONNECTW
	}
	#endif

	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return false;
	}

	if (!handleColumns(true,false)) {
		return false;
	}

	if (odbcconn->getcolumntables) {

		// (continued from above)
		//
		// If we want column table names then we had to do something
		// like use a static cursor above.  However, if we actually
		// execute with a static cursor, then performance is really
		// bad if there are a decent number of rows.
		//
		// To work around, we'll grab the column info, reallocate the
		// statment handle, letting its cursor type default to a
		// forward-only cursor, and re-prepare it.
		//
		// This is generally faster than fetching from a static cursor.
		// It won't be for complex queries that return small result
		// sets, but we'll hope that isn't the case.
		//
		// Arguably this should be controlled by a directive on a
		// query-by-query basis like execute-direct is.
		if (!allocateStatementHandle()) {
			return false;
		}

		#ifdef HAVE_SQLCONNECTW
		if (odbcconn->unicode) {

			ucsinbindstrings.clearAndArrayDelete();

			char *queryucs=convertCharset(query,length,
							"UTF-8",
							"UCS-2//TRANSLIT",
							NULL);
			erg=SQLPrepareW(stmt,(SQLWCHAR *)queryucs,SQL_NTS);
			delete[] queryucs;
		} else {
		#endif
			erg=SQLPrepare(stmt,(SQLCHAR *)query,length);
		#ifdef HAVE_SQLCONNECTW
		}
		#endif
	}

	return (erg==SQL_SUCCESS || erg==SQL_SUCCESS_WITH_INFO);
}

bool odbccursor::allocateStatementHandle() {

	if (stmt) {
		#if (ODBCVER >= 0x0300)
		SQLFreeHandle(SQL_HANDLE_STMT,stmt);
		#else
		SQLFreeStmt(stmt,SQL_DROP);
		#endif
		stmt=NULL;
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
		bindformaterror=true;
		return false;
	}

	if (*isnull==SQL_NULL_DATA) {

		// We used to do something like this to bind a NULL:
		//
		//	erg=SQLBindParameter(stmt,
		//			pos,
		//			SQL_PARAM_INPUT,
		//			SQL_C_BINARY,
		//			SQL_CHAR,
		//			1,		// in characters
		//			0,
		//			val,
		//			bufferlength,	// in bytes
		//			&sqlnulldata);
		//
		// (code to set val and bufferlength used to be above this if)
		// (see #975 and the next "see #975" comment below
		// for why there was a 1 for the 6th (ColumnSize) parameter)
		//
		// However, with ODBC Driver 17 for SQL Server (and possibly
		// other versions, and other drivers) the above fails.
		//
		// It's not exactly clear what fails.  It's experimentally
		// verifiable that, in this case, val="" and bufferlength=0.
		// Somehow though, val gets interpreted as "*".
		//
		// It's also seemingly random what works.  Eg.
		//
		// This works:
		//
		//	select case when :foo is null then 'foo-null'
		//	else 'foo-not-null' end as foo;
		//
		// It returns 'foo-null'
		//
		// However, this fails:
		// 
		//	select isnull(:foo, 99) as foo;
		//
		// It returns '*'
		//
		// Similarly, this also fails:
		// 
		//	select cast(isnull(:foo, 99) as int) as foo;
		//
		// It throws "Conversion failed when converting the varchar"
		// "value '*' to data type int." as that '*' can't be converted
		// to an int.
		// 	
		// It's strange.
		//
		// Setting the ColumnSize to 0 seems like an intuitive fix, but
		// if ParameterType is SQL_CHAR, then we can't do that (see the
		// "see #975" comment below for more detail on that).
		//
		// However, if we use a ParameterType of SQL_VARCHAR, then
		// we can set ParameterValuePtr to NULL, ColumnSize to 0, and
		// BufferLength to 0, and then it works.  Apparently a
		// ColumnSize of 0 is allowed with SQL_VARCHARs and
		// SQL_VARBINARYs, but not SQL_LONGVARCHARs or
		// SQL_LONGVARBINARYs as it turns out.
		//
		// see #6232 for more detail
		//
		//
		// NOTE: the 4th (ValueType) parameter must be
		// SQL_C_BINARY (as opposed to SQL_C_WCHAR or SQL_C_CHAR)
		// for this to work with blobs
		erg=SQLBindParameter(stmt,
				pos,
				SQL_PARAM_INPUT,
				SQL_C_BINARY,
				SQL_VARCHAR,
				0,		// in characters
				0,
				NULL,
				0,		// in bytes
				&sqlnulldata);

	} else {

		SQLPOINTER	val=NULL;
		SQLSMALLINT	valtype=SQL_C_CHAR;
		SQLSMALLINT	paramtype=SQL_CHAR;
		SQLLEN		bufferlength=valuesize;
		#ifdef HAVE_SQLCONNECTW
		if (odbcconn->unicode) {

			const char	*encoding=odbcconn->ncharencoding;
			char	*err=NULL;
			char	*valueucs=convertCharset(
						value,valuesize,
						"UTF-8",encoding,
						&err);
			if (err) {
				delete[] valueucs;
				setConvCharError("input bind",err);
				return false;
			}
			valuesize=len(valueucs,encoding);
			bufferlength=size(valueucs,encoding);
			ucsinbindstrings.append(valueucs);
			val=(SQLPOINTER)valueucs;
			valtype=SQL_C_WCHAR;
			paramtype=SQL_WVARCHAR;
		} else {
		#endif
			val=(SQLPOINTER)value;
		#ifdef HAVE_SQLCONNECTW
		}
		#endif

		if (!valuesize) {
			// see #975
			// When binding an empty string, it's intuitive to set
			// ColumnSize to 0, as ParameterValuePtr is "" and, as
			// such, contains 0 characters.  This works with most
			// drivers.
			//
			// However, with SQL Server Native Client ODBC Driver
			// version 11.0 (and possibly later versions)...
			//
			// In ODBC-2 mode, it allows a ColumnSize of 0, but in
			// non-ODBC-2 mode, it throws: "Invalid precision value"
			//
			// Setting ColumnSize to 1 appears to work around this,
			// and works in all ODBC-modes.  BufferLength appears
			// to override ColumnSize, so it's ok that the string
			// is actually 0 characters long, even though we declare
			// it to be 1.
			//
			// This workaound doesn't appear to break any other
			// ODBC drivers either, so we'll go with it for now.
			//
			// NOTE: per the "see #6232" comment above...
			// ColumnSize of 0 may only be invalid for a
			// ParameterType of SQL_CHAR.  Using SQL_VARCHAR
			// instead might be another solution to this problem.
			valuesize=1;
		} else if (odbcconn->maxallowedvarcharbindlength &&
			valuesize>odbcconn->maxallowedvarcharbindlength) {
			valuesize=odbcconn->maxvarcharbindlength;
		}

		erg=SQLBindParameter(stmt,
				pos,
				SQL_PARAM_INPUT,
				valtype,
				paramtype,
				valuesize,	// in characters
				0,
				val,
				bufferlength,	// in bytes
				NULL);
	}
	return (erg==SQL_SUCCESS || erg==SQL_SUCCESS_WITH_INFO);
}

bool odbccursor::inputBind(const char *variable,
				uint16_t variablesize,
				int64_t *value) {

	uint16_t	pos=charstring::toInteger(variable+1);
	if (!pos || pos>maxbindcount) {
		bindformaterror=true;
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
		bindformaterror=true;
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
		bindformaterror=true;
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

	} else if (!validdate && validtime && !odbcconn->timestampfortime) {

		SQL_TIME_STRUCT	*ts=(SQL_TIME_STRUCT *)buffer;
		ts->hour=hour;
		ts->minute=minute;
		ts->second=second;

		erg=SQLBindParameter(stmt,
				pos,
				SQL_PARAM_INPUT,
				SQL_C_TIME,
				SQL_TIME,
				0,
				odbcconn->fractionscale,
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
		if (odbcconn->supportsfraction) {
			if (odbcconn->fractionscale==9) {
				ts->fraction=microsecond*1000;
			} else if (odbcconn->fractionscale==6) {
				ts->fraction=microsecond;
			}
		} else {
			ts->fraction=0;
		}

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
				odbcconn->fractionscale,
				buffer,
				0,
				NULL);
	}
	return (erg==SQL_SUCCESS || erg==SQL_SUCCESS_WITH_INFO);
}

bool odbccursor::inputBindBlob(const char *variable,
						uint16_t variablesize,
						const char *value,
						uint32_t valuesize,
						int16_t *isnull) {

	// FIXME: This code is known to work with SQL Server...
	if (odbcconn->usecharforlobbind) {
		return sqlrservercursor::inputBindBlob(
						variable,
						variablesize,
						value,
						valuesize,
						isnull);
	}

	// FIXME: This code is known to work with Teradata...
	// (Ideally we should get one body of code working for all dbs)
	uint16_t	pos=charstring::toInteger(variable+1);
	if (!pos || pos>maxbindcount) {
		bindformaterror=true;
		return false;
	}

	erg=SQLBindParameter(stmt,
				pos,
				SQL_PARAM_INPUT,
				SQL_C_BINARY,
				SQL_BINARY,
				valuesize,
				0,
				(SQLPOINTER)value,
				0,
				NULL);
	return (erg==SQL_SUCCESS || erg==SQL_SUCCESS_WITH_INFO);
}

bool odbccursor::outputBind(const char *variable, 
				uint16_t variablesize,
				char *value, 
				uint32_t valuesize, 
				int16_t *isnull) {

	uint16_t	pos=charstring::toInteger(variable+1);
	if (!pos || pos>maxbindcount) {
		bindformaterror=true;
		return false;
	}

	SQLLEN		bufferlength=valuesize;

	if (odbcconn->maxallowedvarcharbindlength &&
		valuesize>odbcconn->maxallowedvarcharbindlength) {
		valuesize=odbcconn->maxvarcharbindlength;
		// FIXME: should bufferlength also be reduced here
	}

	charbind	*cb=new charbind;
	cb->value=value;
	cb->valuesize=valuesize;

	outdatebind[pos-1]=NULL;
	outcharbind[pos-1]=cb;
	outisnullptr[pos-1]=isnull;

	#ifdef HAVE_SQLCONNECTW
	if (odbcconn->unicode) {

		erg=SQLBindParameter(stmt,
				pos,
				SQL_PARAM_OUTPUT,
				SQL_C_WCHAR,
				SQL_WVARCHAR,
				valuesize,		// in characters
				0,
				(SQLPOINTER)value,
				bufferlength,		// in bytes
				&(outisnull[pos-1]));

	} else {
	#endif

		erg=SQLBindParameter(stmt,
				pos,
				SQL_PARAM_OUTPUT,
				SQL_C_CHAR,
				SQL_VARCHAR,
				valuesize,		// in characters
				0,
				(SQLPOINTER)value,
				bufferlength,		// in bytes
				&(outisnull[pos-1]));

	#ifdef HAVE_SQLCONNECTW
	}
	#endif

	return (erg==SQL_SUCCESS || erg==SQL_SUCCESS_WITH_INFO);
}

bool odbccursor::outputBind(const char *variable,
				uint16_t variablesize,
				int64_t *value,
				int16_t *isnull) {

	uint16_t	pos=charstring::toInteger(variable+1);
	if (!pos || pos>maxbindcount) {
		bindformaterror=true;
		return false;
	}

	outdatebind[pos-1]=NULL;
	outcharbind[pos-1]=NULL;
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
		bindformaterror=true;
		return false;
	}

	outdatebind[pos-1]=NULL;
	outcharbind[pos-1]=NULL;
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
		bindformaterror=true;
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
	outcharbind[pos-1]=NULL;
	outisnullptr[pos-1]=isnull;

	erg=SQLBindParameter(stmt,
				pos,
				SQL_PARAM_OUTPUT,
				SQL_C_TIMESTAMP,
				SQL_TIMESTAMP,
				// FIXME: shouldn't these be 29,9
				// like an input/output bind?
				0,
				0,
				buffer,
				0,
				&(outisnull[pos-1]));
	return (erg==SQL_SUCCESS || erg==SQL_SUCCESS_WITH_INFO);
}

bool odbccursor::inputOutputBind(const char *variable, 
				uint16_t variablesize,
				char *value, 
				uint32_t valuesize, 
				int16_t *isnull) {

	uint16_t	pos=charstring::toInteger(variable+1);
	if (!pos || pos>maxbindcount) {
		bindformaterror=true;
		return false;
	}

	SQLSMALLINT	valtype=SQL_C_CHAR;
	SQLSMALLINT	paramtype=SQL_CHAR;
	SQLLEN		bufferlength=valuesize;
	#ifdef HAVE_SQLCONNECTW
	if (odbcconn->unicode) {

		const char	*encoding=odbcconn->ncharencoding;
		char	*err=NULL;
		char	*valueucs=convertCharset(
					value,size(value,"UTF-8"),
					"UTF-8",encoding,
					&err);
		if (err) {
			delete[] valueucs;
			setConvCharError("input-output bind",err);
			return false;
		}
		size_t	sizetocopy=size(valueucs,encoding)+nullSize(encoding);
		if (sizetocopy<=valuesize) {
			bytestring::copy(value,valueucs,sizetocopy);
		} else {
			bytestring::copy(value,valueucs,valuesize);
			bytestring::zero(value+valuesize-nullSize(encoding),
							nullSize(encoding));
		}
		delete[] valueucs;
		valtype=SQL_C_WCHAR;
		paramtype=SQL_WVARCHAR;
	}
	#endif

	charbind	*cb=new charbind;
	cb->value=value;
	cb->valuesize=valuesize;

	inoutdatebind[pos-1]=NULL;
	inoutcharbind[pos-1]=cb;
	inoutisnullptr[pos-1]=isnull;

	inoutisnull[pos-1]=(*isnull==SQL_NULL_DATA)?
				sqlnulldata:charstring::length(value);

	// FIXME: original code...
	/*erg=SQLBindParameter(stmt,
				pos,
				SQL_PARAM_INPUT_OUTPUT,
				SQL_C_CHAR,
				SQL_VARCHAR,
				valuesize,
				0,
				(SQLPOINTER)value,
				valuesize,
				&(inoutisnull[pos-1]));*/

	if (*isnull==SQL_NULL_DATA) {
		// the 4th parameter (ValueType) must by
		// SQL_C_BINARY (as opposed to SQL_C_WCHAR or SQL_C_CHAR)
		// for this to work with blobs
		erg=SQLBindParameter(stmt,
				pos,
				SQL_PARAM_INPUT_OUTPUT,
				SQL_C_BINARY,
				SQL_CHAR,
				1,
				0,
				(SQLPOINTER)value,
				bufferlength,
				&(inoutisnull[pos-1]));
	} else {

		if (!valuesize) {
			// In ODBC-2 mode, SQL Server Native Client 11.0
			// (at least) allows a valuesize of 0, when the value
			// is "".  In non-ODBC-2 mode, it throws:
			// "Invalid precision value" Using a valuesize of 1
			// works with all ODBC-modes.  Hopefully it works with
			// all drivers.
			valuesize=1;
		} else if (odbcconn->maxallowedvarcharbindlength &&
			valuesize>odbcconn->maxallowedvarcharbindlength) {
			valuesize=odbcconn->maxvarcharbindlength;
			// FIXME: should bufferlength also be reduced here
		}

		erg=SQLBindParameter(stmt,
				pos,
				SQL_PARAM_INPUT_OUTPUT,
				valtype,
				paramtype,
				valuesize,	// in characters
				0,
				(SQLPOINTER)value,
				bufferlength,	// in bytes
				&(inoutisnull[pos-1]));
	}
	return (erg==SQL_SUCCESS || erg==SQL_SUCCESS_WITH_INFO);
}

bool odbccursor::inputOutputBind(const char *variable, 
				uint16_t variablesize,
				int64_t *value,
				int16_t *isnull) {

	uint16_t	pos=charstring::toInteger(variable+1);
	if (!pos || pos>maxbindcount) {
		bindformaterror=true;
		return false;
	}

	inoutdatebind[pos-1]=NULL;
	inoutcharbind[pos-1]=NULL;
	inoutisnullptr[pos-1]=isnull;

	inoutisnull[pos-1]=(*isnull==SQL_NULL_DATA)?
				sqlnulldata:sizeof(int64_t);

	erg=SQLBindParameter(stmt,
				pos,
				SQL_PARAM_INPUT_OUTPUT,
				SQL_C_SBIGINT,
				SQL_BIGINT,
				0,
				0,
				value,
				sizeof(int64_t),
				&(inoutisnull[pos-1]));
	return (erg==SQL_SUCCESS || erg==SQL_SUCCESS_WITH_INFO);
}

bool odbccursor::inputOutputBind(const char *variable,
				uint16_t variablesize,
				double *value,
				uint32_t *precision,
				uint32_t *scale,
				int16_t *isnull) {

	uint16_t	pos=charstring::toInteger(variable+1);
	if (!pos || pos>maxbindcount) {
		bindformaterror=true;
		return false;
	}

	inoutdatebind[pos-1]=NULL;
	inoutcharbind[pos-1]=NULL;
	inoutisnullptr[pos-1]=isnull;

	erg=SQLBindParameter(stmt,
				pos,
				SQL_PARAM_INPUT_OUTPUT,
				SQL_C_DOUBLE,
				SQL_DOUBLE,
				*precision,
				*scale,
				value,
				sizeof(double),
				&(outisnull[pos-1]));
	return (erg==SQL_SUCCESS || erg==SQL_SUCCESS_WITH_INFO);
}

bool odbccursor::inputOutputBind(const char *variable,
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
		bindformaterror=true;
		return false;
	}

	SQL_TIMESTAMP_STRUCT	*ts=(SQL_TIMESTAMP_STRUCT *)buffer;
	ts->year=*year;
	ts->month=*month;
	ts->day=*day;
	ts->hour=*hour;
	ts->minute=*minute;
	ts->second=*second;
	ts->fraction=(*microsecond)*1000;

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

	inoutdatebind[pos-1]=db;
	inoutcharbind[pos-1]=NULL;
	inoutisnullptr[pos-1]=isnull;

	erg=SQLBindParameter(stmt,
				pos,
				SQL_PARAM_INPUT_OUTPUT,
				SQL_C_TIMESTAMP,
				SQL_TIMESTAMP,
				29,
				9,
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
	return ((int16_t)isnull==SQL_NULL_DATA);
}

bool odbccursor::executeQuery(const char *query, uint32_t length) {

	// initialize counts
	initializeRowCounts();

	// query timeout is an odbc-driver level read timeout but with
	// special cleanup handling in better drivers to tell the server
	// to stop executing the query after the client read timeout...
	// * init from query timeout specified in the connect string
	// * override with query timeout set via a directive
	// * if it's still > 0 then actually set the timeout
	uint64_t	statementquerytimeout=conn->cont->getQueryTimeout();
	if (getQueryTimeout()>0) {
		statementquerytimeout=getQueryTimeout();
	}
	if (statementquerytimeout>0) {
		erg=SQLSetStmtAttr(stmt,SQL_ATTR_QUERY_TIMEOUT,
					(SQLPOINTER)statementquerytimeout,
					SQL_IS_UINTEGER);
		// FIXME: do we care if this fails?
	}

	// execute the query
	if (getExecuteDirect()) {
		#ifdef HAVE_SQLCONNECTW
		if (odbcconn->unicode) {
			char	*err=NULL;
			char	*queryucs=convertCharset(query,length,
							"UTF-8",
							"UCS-2//TRANSLIT",
							&err);
			if (err) {
				delete[] queryucs;
				setConvCharError("execute query",err);
				return false;
			}
			erg=SQLExecDirectW(stmt,(SQLWCHAR *)queryucs,SQL_NTS);
			delete[] queryucs;
		} else {
		#endif
			erg=SQLExecDirect(stmt,(SQLCHAR *)query,length);
		#ifdef HAVE_SQLCONNECTW
		}
		#endif
	} else {
		erg=SQLExecute(stmt);
	}

	#ifdef HAVE_SQLCONNECTW
		// free buffers used to convert string-binds to unicode
		ucsinbindstrings.clearAndArrayDelete();
	#endif

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

	// if we're not exec-direct'ing, and if column info is valid after
	// prepare, then we must have already done the first half of this in
	// prepareQuery()
	if (!handleColumns(getExecuteDirect() ||
			!columninfoisvalidafterprepare,true)) {
		return false;
	}

	// get the row count
	erg=SQLRowCount(stmt,&affectedrows);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return false;
	}

	// FIXME:
	// Data isn't written to the output bind buffers (at least with the
	// Microsoft ODBC Driver) until SQLMoreResults() returns SQL_NO_DATA. 
	// So if there are any output results pending, this work is being done
	// too soon.

	// convert date output binds and copy out isnulls
	//for (uint16_t i=0; i<getOutputBindCount(); i++) {
	// FIXME: inefficient
	for (uint16_t i=0; i<maxbindcount; i++) {
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
		#ifdef HAVE_SQLCONNECTW
		if (odbcconn->unicode && outcharbind[i]) {
			// convert wchar output binds to user coding
			char		*value=outcharbind[i]->value;
			uint32_t	valuesize=outcharbind[i]->valuesize;
			char		*err=NULL;
			char		*u=convertCharset(value,
						odbcconn->ncharencoding,
						"UTF-8",&err);
			if (err) {
				delete[] u;
				setConvCharError("output bind",err);
				return false;
			}
			size_t	s=size(u,"UTF-8");
			if (s>=valuesize) {
				// FIXME: this could make s<0
				s=valuesize-nullSize("UTF-8");
			}
			bytestring::zero(value+s,nullSize("UTF-8"));
			bytestring::copy(value,u,s);
			delete[] u;
		}
		#endif
		if (outisnullptr[i]) {
			*(outisnullptr[i])=outisnull[i];
			// FIXME: get these to work
			/*if (outisnull[i]==SQL_NO_TOTAL) {
				// This is most likely caused by the fact that
				// we should be using SQL_C_WCHAR and SQL_WCHAR
				// instead of forcing ODBC to do the conversion
				// for us.   In a work-around we just kludge
				// away the space with padding.
				// Work-around in SQL: return only varchar, not 
				// varchar.
				char	*valuep=sb->value;
				for (int k=(sb->BufferLength-2);
					k>=0 && valuep[k]==' ';k--) {
					valuep[k]=0;
				}
			} else if (outisnull[i]>=0 &&
					outisnull[i]<sb->BufferLength) {
				// forcibly null-terminate the buffer
				sb->value[outisnull[i]]=0;
			}*/
		}
	}
	//for (uint16_t i=0; i<getInputOutputBindCount(); i++) {
	// FIXME: inefficient
	for (uint16_t i=0; i<maxbindcount; i++) {
		if (inoutdatebind[i]) {
			datebind	*db=inoutdatebind[i];
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
		#ifdef HAVE_SQLCONNECTW
		if (odbcconn->unicode && inoutcharbind[i]) {
			// convert wchar output binds to user coding
			char		*value=inoutcharbind[i]->value;
			uint32_t	valuesize=inoutcharbind[i]->valuesize;
			char		*err=NULL;
			char		*u=convertCharset(value,
						odbcconn->ncharencoding,
						"UTF-8",&err);
			if (err) {
				delete[] u;
				setConvCharError("input-output bind",err);
				return false;
			}
			size_t	s=size(u,"UTF-8");
			if (s>=valuesize) {
				// FIXME: this could make s<0
				s=valuesize-nullSize("UTF-8");
			}
			bytestring::zero(value+s,nullSize("UTF-8"));
			bytestring::copy(value,u,s);
			delete[] u;
		}
		#endif
		if (inoutisnullptr[i]) {
			*(inoutisnullptr[i])=inoutisnull[i];
		}
	}

	return true;
}

void odbccursor::initializeColCounts() {
	ncols=0;
	columninfoisvalidafterprepare=true;
}

void odbccursor::initializeRowCounts() {
	row=0;
	maxrow=0;
	totalrows=0;
	affectedrows=-1;
}

bool odbccursor::handleColumns(bool getcolumninfo, bool bindcolumns) {

	// get the column count
	erg=SQLNumResultCols(stmt,&ncols);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {

		// column info may not be valid until post-execute for
		// particular queries
		// (eg. "select ?" with a bind value in MS SQL Server, or a
		// stored procedure that optionally executes selects with
		// different numbers of columns)
		if (odbcconn->columninfonotvalidyeterror) {
			SQLCHAR		state[SQL_SQLSTATE_SIZE+1];
			SQLINTEGER	nativeerrnum=0;
			SQLSMALLINT	errlength=0;
			bytestring::zero(state,sizeof(state));
			SQLGetDiagRec(SQL_HANDLE_STMT,stmt,1,
							state,&nativeerrnum,
							NULL,0,&errlength);
			for (SQLINTEGER *ptr=
					odbcconn->columninfonotvalidyeterror;
					*ptr; ptr++) {
				if (nativeerrnum==*ptr) {
					columninfoisvalidafterprepare=false;
					erg=SQL_SUCCESS;
					return true;
				}
			}
		}
		return false;
	}

	// limit column count if necessary
	uint32_t	maxcolumncount=conn->cont->getMaxColumnCount();
	if (maxcolumncount && (uint32_t)ncols>maxcolumncount) {
		ncols=maxcolumncount;
	}

	if (getcolumninfo) {

		// allocate buffers if necessary
		if (!maxcolumncount) {
			allocateResultSetBuffers(ncols);
		}

		// run through the columns
		for (SQLSMALLINT i=0; i<ncols; i++) {

			if (conn->cont->getSendColumnInfo()==SEND_COLUMN_INFO) {
#if (ODBCVER >= 0x0300)
				// column name
				erg=SQLColAttribute(stmt,i+1,SQL_DESC_LABEL,
						column[i].name,4096,
						(SQLSMALLINT *)
						&(column[i].namelength),
						NULL);
				if (erg!=SQL_SUCCESS &&
					erg!=SQL_SUCCESS_WITH_INFO) {
					return false;
				}
				column[i].namelength=
					charstring::length(column[i].name);

				// column length
				erg=SQLColAttribute(stmt,i+1,SQL_DESC_LENGTH,
						NULL,0,NULL,
						&(column[i].length));
				if (erg!=SQL_SUCCESS &&
					erg!=SQL_SUCCESS_WITH_INFO) {
					return false;
				}
	
				// column type
				erg=SQLColAttribute(stmt,i+1,SQL_DESC_TYPE,
						NULL,0,NULL,
						&(column[i].type));
				if (erg!=SQL_SUCCESS &&
					erg!=SQL_SUCCESS_WITH_INFO) {
					return false;
				}

				// column precision
				erg=SQLColAttribute(stmt,i+1,SQL_DESC_PRECISION,
						NULL,0,NULL,
						&(column[i].precision));
				// Some drivers (Redshift) like to return -1
				// for the precision of some (TEXT/NTEXT)
				// columns.  This wreaks havoc on the client
				// side, as the value is interpreted as 2^32-1.
				// Override the -1 with the length.
				if (column[i].precision==-1) {
					column[i].precision=column[i].length;
				}
				if (erg!=SQL_SUCCESS &&
					erg!=SQL_SUCCESS_WITH_INFO) {
					return false;
				}

				// column scale
				erg=SQLColAttribute(stmt,i+1,SQL_DESC_SCALE,
						NULL,0,NULL,
						&(column[i].scale));
				if (erg!=SQL_SUCCESS &&
					erg!=SQL_SUCCESS_WITH_INFO) {
					return false;
				}

				// column nullable
				erg=SQLColAttribute(stmt,i+1,SQL_DESC_NULLABLE,
						NULL,0,NULL,
						&(column[i].nullable));
				if (erg!=SQL_SUCCESS &&
					erg!=SQL_SUCCESS_WITH_INFO) {
					return false;
				}

				// primary key

				// unique

				// part of key

				// unsigned number
				erg=SQLColAttribute(stmt,i+1,SQL_DESC_UNSIGNED,
						NULL,0,NULL,
						&(column[i].unsignednumber));
				if (erg!=SQL_SUCCESS &&
					erg!=SQL_SUCCESS_WITH_INFO) {
					return false;
				}

				// zero fill

				// binary

				// autoincrement
				erg=SQLColAttribute(stmt,i+1,
						SQL_DESC_AUTO_UNIQUE_VALUE,
						NULL,0,NULL,
						&(column[i].autoincrement));
				if (erg!=SQL_SUCCESS &&
					erg!=SQL_SUCCESS_WITH_INFO) {
					return false;
				}

				// table name
				erg=SQLColAttribute(stmt,i+1,
						SQL_DESC_BASE_TABLE_NAME,
						column[i].table,4096,
						(SQLSMALLINT *)
						&(column[i].tablelength),
						NULL);
				if (erg!=SQL_SUCCESS &&
					erg!=SQL_SUCCESS_WITH_INFO) {
					return false;
				}
				// Some databases (Hive) like to return
				// columns as table.column.
				// If the column name was table.column then
				// split it and override the table name.
				char	*dot=charstring::findFirst(
							column[i].name,'.');
				if (dot) {
					char	*col=dot+1;
					*dot='\0';
					charstring::copy(column[i].table,
								column[i].name);
					charstring::copy(columnnamescratch,col);
					charstring::copy(column[i].name,
							columnnamescratch);
					column[i].namelength=
						charstring::length(
							column[i].name);
				}
				column[i].tablelength=
					charstring::length(column[i].table);

#else
				// column name
				erg=SQLColAttributes(stmt,i+1,
						SQL_COLUMN_LABEL,
						column[i].name,4096,
						(SQLSMALLINT *)
						&(column[i].namelength),
						NULL);
				if (erg!=SQL_SUCCESS &&
					erg!=SQL_SUCCESS_WITH_INFO) {
					return false;
				}
				// FIXME: above we reset namelength
				// to length(name)...

				// column length
				erg=SQLColAttributes(stmt,i+1,
						SQL_COLUMN_LENGTH,
						NULL,0,NULL,
						&(column[i].length));
				if (erg!=SQL_SUCCESS &&
					erg!=SQL_SUCCESS_WITH_INFO) {
					return false;
				}

				// column type
				erg=SQLColAttributes(stmt,i+1,
						SQL_COLUMN_TYPE,
						NULL,0,NULL,
						&(column[i].type));
				if (erg!=SQL_SUCCESS &&
					erg!=SQL_SUCCESS_WITH_INFO) {
					return false;
				}

				// column precision
				erg=SQLColAttributes(stmt,i+1,
						SQL_COLUMN_PRECISION,
						NULL,0,NULL,
						&(column[i].precision));
				if (erg!=SQL_SUCCESS &&
					erg!=SQL_SUCCESS_WITH_INFO) {
					return false;
				}

				// column scale
				erg=SQLColAttributes(stmt,i+1,
						SQL_COLUMN_SCALE,
						NULL,0,NULL,
						&(column[i].scale));
				if (erg!=SQL_SUCCESS &&
					erg!=SQL_SUCCESS_WITH_INFO) {
					return false;
				}

				// column nullable
				erg=SQLColAttributes(stmt,i+1,
						SQL_COLUMN_NULLABLE,
						NULL,0,NULL,
						&(column[i].nullable));
				if (erg!=SQL_SUCCESS &&
					erg!=SQL_SUCCESS_WITH_INFO) {
					return false;
				}

				// primary key

				// unique

				// part of key

				// unsigned number
				erg=SQLColAttributes(stmt,i+1,
						SQL_COLUMN_UNSIGNED,
						NULL,0,NULL,
						&(column[i].unsignednumber));
				if (erg!=SQL_SUCCESS &&
					erg!=SQL_SUCCESS_WITH_INFO) {
					return false;
				}

				// zero fill

				// binary

				// autoincrement
				#ifdef SQL_DESC_AUTO_UNIQUE_VALUE
				erg=SQLColAttributes(stmt,i+1,
						SQL_COLUMN_AUTO_INCREMENT,
						NULL,0,NULL,
						&(column[i].autoincrement));
				if (erg!=SQL_SUCCESS &&
					erg!=SQL_SUCCESS_WITH_INFO) {
					return false;
				}
				#else
				column[i].autoincrement=0;
				#endif

				// table name
				erg=SQLColAttributes(stmt,i+1,
						SQL_COLUMN_TABLE_NAME,
						column[i].table,4096,
						(SQLSMALLINT *)
						&(column[i].tablelength),
						NULL);
				if (erg!=SQL_SUCCESS &&
					erg!=SQL_SUCCESS_WITH_INFO) {
					return false;
				}
				// Some databases (Hive) like to return
				// columns as table.column.
				// If the column name was table.column then
				// split it and override the table name.
				char	*dot=charstring::findFirst(
							column[i].name,'.');
				if (dot) {
					char	*col=dot+1;
					*dot='\0';
					charstring::copy(column[i].table,
								column[i].name);
					charstring::copy(columnnamescratch,col);
					charstring::copy(column[i].name,
							columnnamescratch);
					column[i].namelength=
						charstring::length(
							column[i].name);
				}
				column[i].tablelength=
					charstring::length(column[i].table);
#endif
			}
		}
	}

	if (bindcolumns) {

		// allocate buffers if necessary
		/*if (!maxcolumncount) {
			allocateResultSetBuffers(ncols);
		}*/

		uint32_t	maxfieldlength=conn->cont->getMaxFieldLength();

		// run through the columns
		for (SQLSMALLINT i=0; i<ncols; i++) {

			// bind the column to a buffer
			#ifdef HAVE_SQLCONNECTW
			if (odbcconn->unicode) {
				if (column[i].type==SQL_WVARCHAR ||
					column[i].type==SQL_WCHAR) {
					erg=SQLBindCol(stmt,i+1,SQL_C_WCHAR,
							field[i],maxfieldlength,
							&(indicator[i]));
				} else if (column[i].type==SQL_TYPE_TIMESTAMP ||
					(odbcconn->sqltypedatetosqlcbinary &&
					column[i].type==SQL_TYPE_DATE)) {
					erg=SQLBindCol(stmt,i+1,SQL_C_BINARY,
							field[i],maxfieldlength,
							&(indicator[i]));
				} else if (!isLob(column[i].type)) {
					erg=SQLBindCol(stmt,i+1,SQL_C_CHAR,
							field[i],maxfieldlength,
							&(indicator[i]));
				}
			} else {
			#endif
				if (!isLob(column[i].type)) {
					erg=SQLBindCol(stmt,i+1,SQL_C_CHAR,
							field[i],maxfieldlength,
							&(indicator[i]));
				}
			#ifdef HAVE_SQLCONNECTW
			}
			#endif
		
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return false;
			}
		}
	}

	return true;
}

void odbccursor::errorMessage(char *errorbuffer,
					uint32_t errorbufferlength,
					uint32_t *errorlength,
					int64_t *errorcode,
					bool *liveconnection) {
	if (bindformaterror) {
		// handle bind format errors
		*errorlength=charstring::length(
				SQLR_ERROR_INVALIDBINDVARIABLEFORMAT_STRING);
		charstring::safeCopy(errorbuffer,
				errorbufferlength,
				SQLR_ERROR_INVALIDBINDVARIABLEFORMAT_STRING,
				*errorlength);
		*errorcode=SQLR_ERROR_INVALIDBINDVARIABLEFORMAT;
		*liveconnection=true;
		return;
	}

	SQLCHAR		state[SQL_SQLSTATE_SIZE+1];
	SQLINTEGER	nativeerrnum;
	SQLSMALLINT	errlength;

	bytestring::zero(state,sizeof(state));

	SQLGetDiagRec(SQL_HANDLE_STMT,stmt,1,state,&nativeerrnum,
				(SQLCHAR *)errorbuffer,errorbufferlength,
				&errlength);

	// set return values
	*errorlength=errlength;
	*errorcode=nativeerrnum;
	*liveconnection=odbcconn->isLiveConnection(state);
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
		case SQL_WCHAR:
			return NCHAR_DATATYPE;
		case SQL_WVARCHAR:
			return NVARCHAR_DATATYPE;
		case SQL_WLONGVARCHAR:
			return NTEXT_DATATYPE;
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
		case -150:
			// FIXME:
			// this is "sql_variant"
			// is there a better type to map it to?
			return VARCHAR_DATATYPE;
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

const char *odbccursor::getColumnTable(uint32_t i) {
	return column[i].table;
}

uint16_t odbccursor::getColumnTableLength(uint32_t i) {
	return column[i].tablelength;
}

bool odbccursor::noRowsToReturn() {
	// if there are no columns, then there can't be any rows either
	return (!ncols);
}

bool odbccursor::fetchRow(bool *error) {

	*error=false;
	erg=SQLFetch(stmt);
	if (erg==SQL_ERROR) {
		*error=true;
		return false;
	}
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return false;
	}
	
	#ifdef HAVE_SQLCONNECTW
	if (odbcconn->unicode) {
		// convert wvarchar/wchar fields to user coding
		uint32_t	maxfieldlength=conn->cont->getMaxFieldLength();
		for (int i=0; i<ncols; i++) {
			if (column[i].type==SQL_WVARCHAR ||
					column[i].type==SQL_WCHAR) {
				if (indicator[i]!=SQL_NULL_DATA && field[i]) {
					char	*err=NULL;
					char	*u=convertCharset(
						field[i],
						odbcconn->ncharencoding,
						"UTF-8",&err);
					if (err) {
						delete[] u;
						setConvCharError("fetch",err);
						return false;
					}
					size_t	s=size(u,"UTF-8");
					if (s>=maxfieldlength) {
						// FIXME: this could make s<0
						s=maxfieldlength-
							nullSize("UTF-8");
					}
					bytestring::zero(field[i]+s,
							nullSize("UTF-8"));
					bytestring::copy(field[i],u,s);
					indicator[i]=s;
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
	if (isLob(column[col].type)) {
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

	// FIXME: SQL Server XML types reliably return SQL_NO_TOTAL, so for now
	// we aren't handling them as LOBs.  Is there some other way we can
	// determine the length?
	if (loblength[col]==SQL_NO_TOTAL) {
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

bool odbccursor::nextResultSet(bool *nextresultsetavailable) {
	*nextresultsetavailable=false;
	return true;
}

void odbccursor::closeResultSet() {

	if (stmt) {
		SQLCloseCursor(stmt);
		// The msdn.microsoft.com documentation says that this call
		// is equivalent to SQLFreeStmt with SQL_CLOSE.  If so, then
		// we should be able to set stmt to NULL here.  But, if we do,
		// then we get SQLExecute.c][170]Error: SQL_INVALID_HANDLE.
		// So apparently the microsoft documentation is wrong.
	}

	for (uint16_t i=0; i<getOutputBindCount(); i++) {
		delete outdatebind[i];
	}

	for (uint16_t i=0; i<getOutputBindCount(); i++) {
		delete outcharbind[i];
	}

	for (uint16_t i=0; i<getInputOutputBindCount(); i++) {
		delete inoutdatebind[i];
	}

	for (uint16_t i=0; i<getInputOutputBindCount(); i++) {
		delete inoutcharbind[i];
	}

	// FIXME: inefficient, but there appears to be a case where
	// closeResultSet isn't called, and stale ptrs get left lingering
	// around...
	for (uint16_t i=0; i<maxbindcount; i++) {
		outdatebind[i]=NULL;
		outcharbind[i]=NULL;
		outisnullptr[i]=NULL;
		outisnull[i]=0;
		inoutdatebind[i]=NULL;
		inoutcharbind[i]=NULL;
		inoutisnullptr[i]=NULL;
		inoutisnull[i]=0;
	}

	if (!conn->cont->getMaxColumnCount()) {
		deallocateResultSetBuffers();
	}

	// NOTE: this is a bit of a kludge.
	//
	// ncols is reset at the beginning of prepareQuery, and other methods,
	// but, since we rely on it to decide whether there are rows to return,
	// it really needs to be reset here.
	//
	// If sqlrservercontroller intercepts the query (eg. if it's a begin,
	// commit, rollback, etc.) then prepareQuery() will never be called,
	// and this won't be reset.  If it was > 0 from the previous query,
	// then a begin (for example) will think that it has rows to return,
	// and the subsequent SQLFetch will fail with a
	// "function sequence error".  We can avoid that by setting ncols=0
	// here, which will cause noRowsToReturn() to return false by default,
	// and avoid the fetch.
	//
	// Arguably, other things should be reset here too
	// (columninfoisvalidafterprepare, various row counts, etc.) but this
	// is the critical one for now, so we'll sort that out later.
	ncols=0;
}

bool odbccursor::columnInfoIsValidAfterPrepare() {
	return columninfoisvalidafterprepare;
}

#if (ODBCVER >= 0x0300) && defined(SQLCOLATTRIBUTE_SQLLEN)
bool odbccursor::isLob(SQLLEN type) {
#else
bool odbccursor::isLob(SQLINTEGER type) {
#endif

	if (odbcconn->fetchlobsasstrings) {
		return false;
	}

	// FIXME: -152 (SQL Server XML) types are kind-of also LOBs, but
	// attempts to get their lengths reliably result in SQL_NO_TOTAL.
	// We don't (currently) have a way of determining their lengths,
	// so, for now, we'll handle them as non-LOBs.
	return (type==SQL_LONGVARCHAR ||
		type==SQL_LONGVARBINARY ||
		type==SQL_WLONGVARCHAR);
}

void odbccursor::setConvCharError(const char *baseerror,
						const char *detailerror) {

	stringbuffer	err;
	err.append(baseerror)->append(": ")->append(detailerror);
	conn->cont->setError(this,err.getString(),
				SQLR_ERROR_CHARACTER_CONVERSION_FAILED,true);
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrserverconnection *new_odbcconnection(
						sqlrservercontroller *cont) {
		return new odbcconnection(cont);
	}
}
