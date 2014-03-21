// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrcontroller.h>
#include <sqlrconnection.h>
#include <rudiments/environment.h>
#include <rudiments/process.h>

#include <datatypes.h>
#include <config.h>

#include <sqlcli1.h>

#define FETCH_AT_ONCE		10
#define MAX_SELECT_LIST_SIZE	256
#define MAX_ITEM_BUFFER_SIZE	4096

struct db2column {
	char		*name;
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

class db2connection;

class db2cursor : public sqlrcursor_svr {
	friend class db2connection;
	public:
			db2cursor(sqlrconnection_svr *conn);
			~db2cursor();
	private:
		void		allocateResultSetBuffers(
						int32_t selectlistsize);
		void		deallocateResultSetBuffers();
		bool		prepareQuery(const char *query,
						uint32_t length);
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
						uint16_t valuesize,
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
						char *buffer,
						uint16_t buffersize,
						int16_t *isnull);
		bool		outputBindBlob(const char *variable, 
						uint16_t variablesize,
						uint16_t index,
						int16_t *isnull);
		bool		outputBindClob(const char *variable, 
						uint16_t variablesize,
						uint16_t index,
						int16_t *isnull);
		bool		getLobOutputBindLength(uint16_t index,
							uint64_t *length);
		bool		getLobOutputBindSegment(uint16_t index,
							char *buffer,
							uint64_t buffersize,
							uint64_t offset,
							uint64_t charstoread,
							uint64_t *charsread);
		bool		executeQuery(const char *query,
						uint32_t length);
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
		bool		skipRow();
		bool		fetchRow();
		void		getField(uint32_t col,
					const char **fld,
					uint64_t *fldlength,
					bool *blob,
					bool *null);
		void		nextRow();
		void		cleanUpData();

		SQLRETURN	erg;
		SQLHSTMT	stmt;
		SQLSMALLINT	ncols;
		SQLINTEGER 	affectedrows;

		int32_t		selectlistsize;
		char		**field;
		SQLINTEGER	**indicator;
		#if (DB2VERSION>7)
		SQLUSMALLINT	*rowstat;
		#endif
		db2column	*col;

		datebind	**outdatebind;
		char		**outlobbind;
		SQLINTEGER 	*outlobbindlen;

		uint64_t	rowgroupindex;
		uint64_t	totalinrowgroup;
		uint64_t	totalrows;
		uint64_t	rownumber;

		stringbuffer	errormsg;

		db2connection	*db2conn;
};

class db2connection : public sqlrconnection_svr {
	friend class db2cursor;
	public:
			db2connection(sqlrcontroller_svr *cont);
	private:
		void	handleConnectString();
		bool	logIn(const char **error);
		const char	*logInError(const char *errmsg);
		sqlrcursor_svr	*initCursor();
		void	deleteCursor(sqlrcursor_svr *curs);
		void	logOut();
		int16_t	nullBindValue();
		bool	bindValueIsNull(int16_t isnull);
		bool	autoCommitOn();
		bool	autoCommitOff();
		bool	supportsTransactionBlocks();
		bool	commit();
		bool	rollback();
		void	errorMessage(char *errorbuffer,
					uint32_t errorbufferlength,
					uint32_t *errorlength,
					int64_t	*errorcode,
					bool *liveconnection);
		bool	liveConnection(SQLINTEGER nativeerror,
					SQLSMALLINT errlength);
		const char	*pingQuery();
		const char	*identify();
		const char	*dbVersion();
		const char	*dbHostNameQuery();
		const char	*getDatabaseListQuery(bool wild);
		const char	*getTableListQuery(bool wild);
		const char	*getColumnListQuery(const char *table,
								bool wild);
		const char	*selectDatabaseQuery();
		const char	*getCurrentDatabaseQuery();
		const char	*getLastInsertIdQuery();
		const char	*setIsolationLevelQuery();
		const char	*bindFormat();

		SQLHENV		env;
		SQLRETURN	erg;
		SQLHDBC		dbc;

		const char	*server;
		const char	*lang;
		uint32_t	timeout;

		stringbuffer	errormessage;

		uint32_t	fetchatonce;
		int32_t		maxselectlistsize;
		int32_t		maxitembuffersize;

		char		dbversion[512];

		stringbuffer	errormsg;
};

db2connection::db2connection(sqlrcontroller_svr *cont) :
					sqlrconnection_svr(cont) {

	fetchatonce=FETCH_AT_ONCE;
	maxselectlistsize=MAX_SELECT_LIST_SIZE;
	maxitembuffersize=MAX_ITEM_BUFFER_SIZE;
}

void db2connection::handleConnectString() {

	// override legacy "server" parameter with modern "db" parameter
	server=cont->connectStringValue("server");
	const char	*tmp=cont->connectStringValue("db");
	if (tmp && tmp[0]) {
		server=tmp;
	}

	cont->setUser(cont->connectStringValue("user"));
	cont->setPassword(cont->connectStringValue("password"));
	const char	*autocom=cont->connectStringValue("autocommit");
	cont->setAutoCommitBehavior((autocom &&
		!charstring::compareIgnoringCase(autocom,"yes")));
	lang=cont->connectStringValue("lang");
	cont->setFakeTransactionBlocksBehavior(
		!charstring::compare(
			cont->connectStringValue("faketransactionblocks"),
			"yes"));
	cont->fakeinputbinds=
		!charstring::compare(
			cont->connectStringValue("fakebinds"),
			"yes");

	const char	*to=cont->connectStringValue("timeout");
	if (!charstring::length(to)) {
		// for back-compatibility
		timeout=5;
	} else {
		timeout=charstring::toInteger(to);
	}

	fetchatonce=charstring::toUnsignedInteger(
				cont->connectStringValue("fetchatonce"));
	if (!fetchatonce) {
		fetchatonce=FETCH_AT_ONCE;
	}
	maxselectlistsize=charstring::toInteger(
				cont->connectStringValue("maxselectlistsize"));
	if (!maxselectlistsize) {
		maxselectlistsize=MAX_SELECT_LIST_SIZE;
	}
	maxitembuffersize=charstring::toInteger(
				cont->connectStringValue("maxitembuffersize"));
	if (!maxitembuffersize) {
		maxitembuffersize=MAX_ITEM_BUFFER_SIZE;
	}
}

bool db2connection::logIn(const char **error) {

	// set the LANG environment variable
	if (charstring::length(lang) && !environment::setValue("LANG",lang)) {
		*error="Failed to set LANG environment variable";
		return false;
	}

	// allocate environment handle
	erg=SQLAllocHandle(SQL_HANDLE_ENV,SQL_NULL_HANDLE,&env);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		*error="Failed to allocate environment handle";
		SQLFreeHandle(SQL_HANDLE_ENV,env);
		return false;
	}

	// allocate connection handle
	erg=SQLAllocHandle(SQL_HANDLE_DBC,env,&dbc);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		*error="Failed to allocate connection handle";
		SQLFreeHandle(SQL_HANDLE_DBC,dbc);
		SQLFreeHandle(SQL_HANDLE_ENV,env);
		return false;
	}

	// set the connect timeout
	if (timeout) {
		erg=SQLSetConnectAttr(dbc,
				#ifdef SQL_ATTR_LOGIN_TIMEOUT
				SQL_ATTR_LOGIN_TIMEOUT,
				#else
				SQL_LOGIN_TIMEOUT,
				#endif
				(SQLPOINTER *)timeout,0);
		if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
			*error="Failed to set timeout";
			SQLFreeHandle(SQL_HANDLE_DBC,dbc);
			SQLFreeHandle(SQL_HANDLE_ENV,env);
			return false;
		}
	}

	// connect to the database
	erg=SQLConnect(dbc,(SQLCHAR *)server,SQL_NTS,
				(SQLCHAR *)cont->getUser(),SQL_NTS,
				(SQLCHAR *)cont->getPassword(),SQL_NTS);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		*error=logInError("SQLConnect failed");
		SQLFreeHandle(SQL_HANDLE_DBC,dbc);
		SQLFreeHandle(SQL_HANDLE_ENV,env);
		return false;
	}
	return true;
}

const char *db2connection::logInError(const char *errmsg) {

	errormessage.clear();
	errormessage.append(errmsg)->append(": ");

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

sqlrcursor_svr *db2connection::initCursor() {
	return (sqlrcursor_svr *)new db2cursor((sqlrconnection_svr *)this);
}

void db2connection::deleteCursor(sqlrcursor_svr *curs) {
	delete (db2cursor *)curs;
}

void db2connection::logOut() {
	SQLDisconnect(dbc);
	SQLFreeHandle(SQL_HANDLE_DBC,dbc);
	SQLFreeHandle(SQL_HANDLE_ENV,env);
}

int16_t db2connection::nullBindValue() {
	return SQL_NULL_DATA;
}

bool db2connection::bindValueIsNull(int16_t isnull) {
	if (isnull==SQL_NULL_DATA) {
		return true;
	}
	return false;
}

bool db2connection::autoCommitOn() {
	return (SQLSetConnectAttr(dbc,SQL_ATTR_AUTOCOMMIT,
				(SQLPOINTER)SQL_AUTOCOMMIT_ON,
				sizeof(SQLINTEGER))==SQL_SUCCESS);
}

bool db2connection::autoCommitOff() {
	return (SQLSetConnectAttr(dbc,SQL_ATTR_AUTOCOMMIT,
				(SQLPOINTER)SQL_AUTOCOMMIT_OFF,
				sizeof(SQLINTEGER))==SQL_SUCCESS);
}

bool db2connection::supportsTransactionBlocks() {
	return false;
}

bool db2connection::commit() {
	return (SQLEndTran(SQL_HANDLE_ENV,env,SQL_COMMIT)==SQL_SUCCESS);
}

bool db2connection::rollback() {
	return (SQLEndTran(SQL_HANDLE_ENV,env,SQL_ROLLBACK)==SQL_SUCCESS);
}

void db2connection::errorMessage(char *errorbuffer,
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
	*liveconnection=liveConnection(nativeerrnum,errlength);
}

bool db2connection::liveConnection(SQLINTEGER nativeerrnum,
					SQLSMALLINT errlength) {

	// When the DB goes down, DB2 first reports one error:
	// 	[IBM][CLI Driver] SQL1224N  A database agent could not be
	//	started to service a request, or was terminated as a result of
	//	a database system shutdown or a force command.  SQLSTATE=55032
	//	(in this case nativeerrnum==-1224 and errlength==184)
	// then upon repeated attempts to run a query, it reports:
	//	[IBM][CLI Driver] CLI0106E  Connection is closed. SQLSTATE=08003
	//	(in this case nativeerrnum==-99999 and errlength==64)
	// here's another one for -1224
	//	[IBM][CLI Driver] SQL1224N  The database manager is not able to
	//	 accept new requests, has terminated all requests in progress,
	//	or has terminated your particular request due to a problem with
	//	your request.  SQLSTATE=55032
	// We need to catch it too.
	// When a "force application" command forcibly kills a connection on
	// the server side, the DB2 client reports:
	//	[IBM][CLI Driver] SQL30081N  A communication error has been
	//	detected...
	//	(in this case nativeerrnum==-30081 and the error length is
	//	variable depending on the host name/ip address of the server
	//	and other things, so we'll only test for the error number)
	return !((nativeerrnum==-1224 && errlength==184) ||
		(nativeerrnum==-99999 && errlength==64) ||
		(nativeerrnum==-1224 && errlength==220) ||
		(nativeerrnum==-30081));
}


const char *db2connection::pingQuery() {
	return "values 1";
}

const char *db2connection::identify() {
	return "db2";
}

const char *db2connection::dbVersion() {
	SQLSMALLINT	dbversionlen;
	SQLGetInfo(dbc,SQL_DBMS_VER,
			(SQLPOINTER)dbversion,
			(SQLSMALLINT)sizeof(dbversion),
			&dbversionlen);
	return dbversion;
}

const char *db2connection::dbHostNameQuery() {
	return "select "
		"	host_name "
		"from "
		"	table(sysproc.env_get_sys_info())";
}

const char *db2connection::getDatabaseListQuery(bool wild) {
	return (wild)?
		"select "
		"	schemaname "
		"from "
		"	syscat.schemata "
		"where "
		"	schemaname like '%s'":

		"select "
		"	schemaname "
		"from "
		"	syscat.schemata ";
}

const char *db2connection::getTableListQuery(bool wild) {
	return (wild)?
		"select "
		"	tabname "
		"from "
		"	syscat.tables "
		"where "
		"	ownertype='U' "
		"	and "
		"	tabschema!='SYSTOOLS' "
		"	and "
		"	type in ('T','U','V','W') "
		"	and "
		"	tabname like '%s' "
		"order by "
		"	tabname":

		"select "
		"	tabname "
		"from "
		"	syscat.tables "
		"where "
		"	ownertype='U' "
		"	and "
		"	tabschema!='SYSTOOLS' "
		"	and "
		"	type in ('T','U','V','W') "
		"order by "
		"	tabname";
}

const char *db2connection::getColumnListQuery(const char *table, bool wild) {
	return (wild)?
		"select "
		"	colname, "
		"	typename, "
		"	length, "
		"	length as precision, "
		"	scale, "
		"	nulls, "
		"	keyseq as key, "
		"	default, "
		"	'' as extra "
		"from "
		"	syscat.columns "
		"where "
		"	upper(tabame)=upper('%s') "
		"	and "
		"	colname like '%s' "
		"order by "
		"	colno":

		"select "
		"	colname, "
		"	typename, "
		"	length, "
		"	length as precision, "
		"	scale, "
		"	nulls, "
		"	keyseq as key, "
		"	default, "
		"	'' as extra "
		"from "
		"	syscat.columns "
		"where "
		"	upper(tabname)=upper('%s') "
		"order by "
		"	colno";
}

const char *db2connection::bindFormat() {
	return "?";
}

const char *db2connection::selectDatabaseQuery() {
	return "set schema %s";
}

const char *db2connection::getCurrentDatabaseQuery() {
	return "values current schema";
}

const char *db2connection::getLastInsertIdQuery() {
	return "values identity_val_local()";
}

const char *db2connection::setIsolationLevelQuery() {
        return "set current isolation %s";
}

db2cursor::db2cursor(sqlrconnection_svr *conn) : sqlrcursor_svr(conn) {
	db2conn=(db2connection *)conn;
	stmt=0;
	outdatebind=new datebind *[conn->cont->maxbindcount];
	outlobbind=new char *[conn->cont->maxbindcount];
	outlobbindlen=new SQLINTEGER[conn->cont->maxbindcount];
	for (uint16_t i=0; i<conn->cont->maxbindcount; i++) {
		outdatebind[i]=NULL;
		outlobbind[i]=NULL;
		outlobbindlen[i]=0;
	}
	allocateResultSetBuffers(db2conn->maxselectlistsize);
}

db2cursor::~db2cursor() {
	delete[] outdatebind;
	delete[] outlobbind;
	delete[] outlobbindlen;
	deallocateResultSetBuffers();
}

void db2cursor::allocateResultSetBuffers(int32_t selectlistsize) {

	if (selectlistsize==-1) {
		this->selectlistsize=0;
		field=NULL;
		indicator=NULL;
		#if (DB2VERSION>7)
		rowstat=NULL;
		#endif
		col=NULL;
	} else {
		this->selectlistsize=selectlistsize;
		field=new char *[selectlistsize];
		indicator=new SQLINTEGER *[selectlistsize];
		#if (DB2VERSION>7)
		rowstat=new SQLUSMALLINT[db2conn->fetchatonce];
		#endif
		col=new db2column[selectlistsize];
		for (int32_t i=0; i<selectlistsize; i++) {
			col[i].name=new char[db2conn->maxitembuffersize];
			field[i]=new char[db2conn->fetchatonce*
						db2conn->maxitembuffersize];
			indicator[i]=new SQLINTEGER[db2conn->fetchatonce];
		}
	}
}

void db2cursor::deallocateResultSetBuffers() {
	if (selectlistsize) {
		for (int32_t i=0; i<selectlistsize; i++) {
			delete[] col[i].name;
			delete[] field[i];
			delete[] indicator[i];
		}
		delete[] col;
		delete[] field;
		delete[] indicator;
		#if (DB2VERSION>7)
		delete[] rowstat;
		#endif
		selectlistsize=0;
	}
}

bool db2cursor::prepareQuery(const char *query, uint32_t length) {

	if (stmt) {
		SQLFreeHandle(SQL_HANDLE_STMT,stmt);
	}

	// allocate the cursor
	erg=SQLAllocHandle(SQL_HANDLE_STMT,db2conn->dbc,&stmt);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return false;
	}

	// set the row array size
	erg=SQLSetStmtAttr(stmt,SQL_ATTR_ROW_ARRAY_SIZE,
				(SQLPOINTER)db2conn->fetchatonce,0);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return false;
	}

	#if (DB2VERSION>7)
	if (db2conn->maxselectlistsize!=-1) {

		// set the row status ptr
		// (only do this here if we're not
		// dynamically allocating row buffers)
		erg=SQLSetStmtAttr(stmt,SQL_ATTR_ROW_STATUS_PTR,
					(SQLPOINTER)rowstat,0);
		if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
			return false;
		}
	}
	#endif

	// prepare the query
	erg=SQLPrepare(stmt,(SQLCHAR *)query,length);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return false;
	}
	return true;
}

bool db2cursor::inputBind(const char *variable,
					uint16_t variablesize,
					const char *value,
					uint32_t valuesize,
					int16_t *isnull) {

	if (*isnull==SQL_NULL_DATA) {
		// the 4th parameter (ValueType) must by
		// SQL_C_BINARY for this to work with blobs
		erg=SQLBindParameter(stmt,
				charstring::toInteger(variable+1),
				SQL_PARAM_INPUT,
				SQL_C_BINARY,
				SQL_CHAR,
				0,
				0,
				(SQLPOINTER)value,
				valuesize,
				(SQLINTEGER *)isnull);
	} else {
		erg=SQLBindParameter(stmt,
				charstring::toInteger(variable+1),
				SQL_PARAM_INPUT,
				SQL_C_CHAR,
				SQL_CHAR,
				0,
				0,
				(SQLPOINTER)value,
				valuesize,
				(SQLINTEGER *)NULL);
	}
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return false;
	}
	return true;
}

bool db2cursor::inputBind(const char *variable,
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
				(SQLINTEGER *)NULL);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return false;
	}
	return true;
}

bool db2cursor::inputBind(const char *variable,
					uint16_t variablesize,
					double *value,
					uint32_t precision,
					uint32_t scale) {

	erg=SQLBindParameter(stmt,
				charstring::toInteger(variable+1),
				SQL_PARAM_INPUT,
				SQL_C_DOUBLE,
				SQL_DOUBLE,
				precision,
				scale,
				value,
				sizeof(double),
				(SQLINTEGER *)NULL);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return false;
	}
	return true;
}

bool db2cursor::inputBind(const char *variable,
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
				(SQLINTEGER *)NULL);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return false;
	}
	return true;
}

bool db2cursor::inputBindBlob(const char *variable,
					uint16_t variablesize,
					const char *value,
					uint32_t valuesize,
					int16_t *isnull) {

	erg=SQLBindParameter(stmt,
				charstring::toInteger(variable+1),
				SQL_PARAM_INPUT,
				SQL_C_BINARY,
				SQL_BLOB,
				0,
				0,
				(SQLPOINTER)value,
				valuesize,
				(SQLINTEGER *)NULL);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return false;
	}
	return true;
}

bool db2cursor::outputBind(const char *variable, 
					uint16_t variablesize,
					char *value, 
					uint16_t valuesize, 
					int16_t *isnull) {

	outdatebind[outbindcount]=NULL;

	erg=SQLBindParameter(stmt,
				charstring::toInteger(variable+1),
				SQL_PARAM_OUTPUT,
				SQL_C_CHAR,
				SQL_CHAR,
				0,
				0,
				value,
				valuesize,
				(SQLINTEGER *)isnull);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return false;
	}
	return true;
}

bool db2cursor::outputBind(const char *variable,
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
				(SQLINTEGER *)isnull);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return false;
	}
	return true;
}

bool db2cursor::outputBind(const char *variable,
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
				(SQLINTEGER *)isnull);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return false;
	}
	return true;
}

bool db2cursor::outputBind(const char *variable,
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
				(SQLINTEGER *)isnull);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return false;
	}
	return true;
}

bool db2cursor::outputBindBlob(const char *variable, 
					uint16_t variablesize,
					uint16_t index,
					int16_t *isnull) {

	outlobbind[index]=new char[db2conn->maxitembuffersize];

	erg=SQLBindParameter(stmt,
				charstring::toInteger(variable+1),
				SQL_PARAM_OUTPUT,
				SQL_C_BINARY,
				SQL_BLOB,
				0,
				0,
				outlobbind[index],
				db2conn->maxitembuffersize,
				&outlobbindlen[index]);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return false;
	}
	return true;
}

bool db2cursor::outputBindClob(const char *variable, 
					uint16_t variablesize,
					uint16_t index,
					int16_t *isnull) {

	outlobbind[index]=new char[db2conn->maxitembuffersize];

	erg=SQLBindParameter(stmt,
				charstring::toInteger(variable+1),
				SQL_PARAM_OUTPUT,
				SQL_C_CHAR,
				SQL_CHAR,
				0,
				0,
				outlobbind[index],
				db2conn->maxitembuffersize,
				&outlobbindlen[index]);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return false;
	}
	return true;
}

bool db2cursor::getLobOutputBindLength(uint16_t index, uint64_t *length) {
	*length=outlobbindlen[index];
	return true;
}

bool db2cursor::getLobOutputBindSegment(uint16_t index,
					char *buffer, uint64_t buffersize,
					uint64_t offset, uint64_t charstoread,
					uint64_t *charsread) {
	uint64_t	len=outlobbindlen[index];
	if (offset>len) {
		return false;
	}
	if (offset+charstoread>len) {
		charstoread=charstoread-((offset+charstoread)-len);
	}
	rawbuffer::copy(buffer,outlobbind[index]+offset,charstoread);
	*charsread=charstoread;
	return true;
}

bool db2cursor::executeQuery(const char *query, uint32_t length) {

	// initialize counts
	ncols=0;
	rowgroupindex=0;
	totalinrowgroup=0;
	totalrows=0;

	// execute the query
	erg=SQLExecute(stmt);
	if (erg!=SQL_SUCCESS &&
		erg!=SQL_SUCCESS_WITH_INFO &&
		erg!=SQL_NO_DATA) {
		return false;
	}

	checkForTempTable(query,length);

	// get the column count
	erg=SQLNumResultCols(stmt,&ncols);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return false;
	}

	// allocate buffers and limit column count if necessary
	if (db2conn->maxselectlistsize==-1) {

		allocateResultSetBuffers(ncols);

		#if (DB2VERSION>7)
		// set the row status ptr
		// (only do this here if we're
		// dynamically allocating row buffers)
		erg=SQLSetStmtAttr(stmt,SQL_ATTR_ROW_STATUS_PTR,
					(SQLPOINTER)rowstat,0);
		if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
			return false;
		}
		#endif

	} else if (ncols>db2conn->maxselectlistsize) {
		ncols=db2conn->maxselectlistsize;
	}

	// run through the columns
	for (SQLSMALLINT i=0; i<ncols; i++) {

		if (conn->cont->sendColumnInfo()) {

			// column name
			erg=SQLColAttribute(stmt,i+1,SQL_COLUMN_LABEL,
					col[i].name,db2conn->maxitembuffersize,
					(SQLSMALLINT *)&(col[i].namelength),
					NULL);
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return false;
			}

			// column length
			erg=SQLColAttribute(stmt,i+1,SQL_COLUMN_LENGTH,
					NULL,0,NULL,&(col[i].length));
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return false;
			}

			// column type
			erg=SQLColAttribute(stmt,i+1,SQL_COLUMN_TYPE,
					NULL,0,NULL,&(col[i].type));
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return false;
			}

			// column precision
			erg=SQLColAttribute(stmt,i+1,SQL_COLUMN_PRECISION,
					NULL,0,NULL,&(col[i].precision));
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return false;
			}

			// column scale
			erg=SQLColAttribute(stmt,i+1,SQL_COLUMN_SCALE,
					NULL,0,NULL,&(col[i].scale));
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return false;
			}

			// column nullable
			erg=SQLColAttribute(stmt,i+1,SQL_COLUMN_NULLABLE,
					NULL,0,NULL,&(col[i].nullable));
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return false;
			}

			// primary key

			// unique

			// part of key

			// unsigned number
			erg=SQLColAttribute(stmt,i+1,SQL_COLUMN_UNSIGNED,
					NULL,0,NULL,&(col[i].unsignednumber));
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return false;
			}

			// zero fill

			// binary

			// autoincrement
			erg=SQLColAttribute(stmt,i+1,
					SQL_COLUMN_AUTO_INCREMENT,
					NULL,0,NULL,&(col[i].autoincrement));
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return false;
			}
		}

		// bind the column to a buffer
		erg=SQLBindCol(stmt,i+1,SQL_C_CHAR,
				field[i],db2conn->maxitembuffersize,
				(SQLINTEGER *)indicator[i]);
		if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
			return false;
		}
	}

	// get the row count
	erg=SQLRowCount(stmt,&affectedrows);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return false;
	}

	// convert date output binds
	for (uint16_t i=0; i<conn->cont->maxbindcount; i++) {
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

void db2cursor::errorMessage(char *errorbuffer,
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
	*liveconnection=db2conn->liveConnection(nativeerrnum,errlength);
}

uint64_t db2cursor::affectedRows() {
	return affectedrows;
}

uint32_t db2cursor::colCount() {
	return ncols;
}

const char *db2cursor::getColumnName(uint32_t i) {
	return col[i].name;
}

uint16_t db2cursor::getColumnNameLength(uint32_t i) {
	return col[i].namelength;
}

uint16_t db2cursor::getColumnType(uint32_t i) {
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
		// DB2 has more datatypes than ODBC...
		case SQL_GRAPHIC:
			return GRAPHIC_DATATYPE;
		case SQL_VARGRAPHIC:
			return VARGRAPHIC_DATATYPE;
		case SQL_LONGVARGRAPHIC:
			return LONGVARGRAPHIC_DATATYPE;
		case SQL_BLOB:
			return BLOB_DATATYPE;
		case SQL_CLOB:
			return CLOB_DATATYPE;
		case SQL_DBCLOB:
			return DBCLOB_DATATYPE;
		case SQL_DATALINK:
			return DATALINK_DATATYPE;
		case SQL_USER_DEFINED_TYPE:
			return USER_DEFINED_TYPE_DATATYPE;
		default:
			return UNKNOWN_DATATYPE;
	}
}

uint32_t db2cursor::getColumnLength(uint32_t i) {
	return col[i].length;
}

uint32_t db2cursor::getColumnPrecision(uint32_t i) {
	return col[i].precision;
}

uint32_t db2cursor::getColumnScale(uint32_t i) {
	return col[i].scale;
}

uint16_t db2cursor::getColumnIsNullable(uint32_t i) {
	return col[i].nullable;
}

uint16_t db2cursor::getColumnIsUnsigned(uint32_t i) {
	return col[i].unsignednumber;
}

uint16_t db2cursor::getColumnIsBinary(uint32_t i) {
	uint16_t	type=getColumnType(i);
	return (type==BINARY_DATATYPE ||
		type==LONGVARBINARY_DATATYPE ||
		type==VARBINARY_DATATYPE ||
		type==GRAPHIC_DATATYPE ||
		type==VARGRAPHIC_DATATYPE ||
		type==LONGVARGRAPHIC_DATATYPE ||
		type==BLOB_DATATYPE);
}

uint16_t db2cursor::getColumnIsAutoIncrement(uint32_t i) {
	return col[i].autoincrement;
}

bool db2cursor::noRowsToReturn() {
	// if there are no columns, then there can't be any rows either
	return (ncols)?false:true;
}

bool db2cursor::skipRow() {
	if (fetchRow()) {
		rowgroupindex++;
		return true;
	}
	return false;
}

bool db2cursor::fetchRow() {

	if (rowgroupindex==db2conn->fetchatonce) {
		rowgroupindex=0;
	}
	if (rowgroupindex>0 && rowgroupindex==totalinrowgroup) {
		return false;
	}
	if (!rowgroupindex) {

		// SQLFetchScroll should return SQL_SUCCESS or
		// SQL_SUCCESS_WITH_INFO if it successfully fetched a group of
		// rows, otherwise we're at the end of the result and there are
		// no more rows to fetch.
		SQLRETURN	result=SQLFetchScroll(stmt,SQL_FETCH_NEXT,0);
		if (result!=SQL_SUCCESS && result!=SQL_SUCCESS_WITH_INFO) {
			// there are no more rows to be fetched
			return false;
		}

		// Determine the current rownumber
		#if (DB2VERSION>7)
		// An apparant bug in version 8.1 causes the
		// SQL_ATTR_ROW_NUMBER to always be 1, running through
		// the row status buffer appears to work though.
		uint32_t	index=0;
		while (index<db2conn->fetchatonce &&
			(rowstat[index]==SQL_ROW_SUCCESS ||
			rowstat[index]==SQL_ROW_SUCCESS_WITH_INFO)) {
			index++;
		}
		rownumber=totalrows+index;
		#else
		SQLGetStmtAttr(stmt,SQL_ATTR_ROW_NUMBER,
				(SQLPOINTER)&rownumber,0,NULL);
		#endif

		// In the event that there's a bug in SQLFetchScroll and it
		// returns SQL_SUCCESS or SQL_SUCCESS_WITH_INFO even if we were
		// at the end of the result set and there were no more rows to
		// fetch, this will also catch the end of the result set.
		// I think there was a bug like that in DB2 version 7.2.
		if (rownumber==totalrows) {
			return false;
		}
		totalinrowgroup=rownumber-totalrows;
		totalrows=rownumber;
	}
	return true;
}

void db2cursor::getField(uint32_t col,
				const char **fld, uint64_t *fldlength,
				bool *blob, bool *null) {

	// handle NULLs
	if (indicator[col][rowgroupindex]==SQL_NULL_DATA) {
		*null=true;
		return;
	}

	// handle normal datatypes
	*fld=&field[col][rowgroupindex*db2conn->maxitembuffersize];
	*fldlength=indicator[col][rowgroupindex];
}

void db2cursor::nextRow() {
	rowgroupindex++;
}

void db2cursor::cleanUpData() {
	for (uint16_t i=0; i<conn->cont->maxbindcount; i++) {
		delete outdatebind[i];
		outdatebind[i]=NULL;
		delete outlobbind[i];
		outlobbind[i]=NULL;
		outlobbindlen[i]=0;
	}

	if (db2conn->maxselectlistsize==-1) {
		deallocateResultSetBuffers();
	}
}

extern "C" {
	sqlrconnection_svr *new_db2connection(sqlrcontroller_svr *cont) {
		return new db2connection(cont);
	}
}
