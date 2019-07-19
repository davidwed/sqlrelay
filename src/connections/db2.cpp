// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/environment.h>

#include <datatypes.h>
#include <defines.h>
#include <config.h>

#ifdef DB2_AT_RUNTIME
	#include "db2atruntime.cpp"
#else
	#include <sqlcli1.h>
#endif

#define MAX_OUT_BIND_LOB_SIZE	2097152

#define MAX_LOB_CHUNK_SIZE	2147483647

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

class db2connection;

class SQLRSERVER_DLLSPEC db2cursor : public sqlrservercursor {
	friend class db2connection;
	public:
			db2cursor(sqlrserverconnection *conn, uint16_t id);
			~db2cursor();
	private:
		void		allocateResultSetBuffers(int32_t columncount);
		void		deallocateResultSetBuffers();
		bool		open();
		bool		close();
		bool		prepareQuery(const char *query,
						uint32_t length);
		void		encodeBlob(stringbuffer *buffer,
						const char *data,
						uint32_t datasize);
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
		#if (DB2VERSION<=7)
		bool		inputBindClob(const char *variable,
						uint16_t variablesize,
						const char *value,
						uint32_t valuesize,
						int16_t *isnull);
		#endif
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
		const char	*getColumnTable(uint32_t i);
		uint16_t	getColumnTableLength(uint32_t i);
		bool		noRowsToReturn();
		bool		skipRow(bool *error);
		bool		fetchRow(bool *error);
		void		getField(uint32_t col,
					const char **fld,
					uint64_t *fldlength,
					bool *blob,
					bool *null);
		void		nextRow();
		bool		getLobFieldLength(uint32_t col,
							uint64_t *length);
		bool		getLobFieldSegment(uint32_t col,
					char *buffer, uint64_t buffersize,
					uint64_t offset, uint64_t charstoread,
					uint64_t *charsread);
		void		closeResultSet();

		SQLRETURN	erg;
		SQLHSTMT	stmt;
		SQLHSTMT	lobstmt;
		SQLSMALLINT	ncols;
		SQLINTEGER 	affectedrows;

		int32_t		columncount;
		char		**field;
		SQLINTEGER	**loblocator;
		SQLINTEGER	**loblength;
		SQLINTEGER	**indicator;
		#if (DB2VERSION>7)
		SQLUSMALLINT	*rowstat;
		#endif
		db2column	*column;

		uint16_t	maxbindcount;
		SQLINTEGER	*blobbindsize;
		datebind	**outdatebind;
		char		**outlobbind;
		SQLINTEGER 	*outlobbindlen;
		int16_t		**outisnullptr;
		SQLINTEGER	*outisnull;
		SQLINTEGER	sqlnulldata;

		uint64_t	rowgroupindex;
		uint64_t	totalinrowgroup;
		uint64_t	totalrows;
		uint64_t	rownumber;

		bool		bindformaterror;

		stringbuffer	errormsg;

		db2connection	*db2conn;
};

class SQLRSERVER_DLLSPEC db2connection : public sqlrserverconnection {
	friend class db2cursor;
	public:
			db2connection(sqlrservercontroller *cont);
	private:
		void	handleConnectString();
		bool	mustDetachBeforeLogIn();
		bool	logIn(const char **error, const char **warning);
		const char	*logInError(const char *errmsg);
		void	dbVersionSpecificTasks();
		sqlrservercursor	*newCursor(uint16_t id);
		void	deleteCursor(sqlrservercursor *curs);
		void	logOut();
		int16_t	nullBindValue();
		bool	bindValueIsNull(int16_t isnull);
		bool	autoCommitOn();
		bool	autoCommitOff();
		bool	supportsTransactionBlocks();
		bool	supportsAutoCommit();
		bool	commit();
		bool	rollback();
		void	errorMessage(char *errorbuffer,
					uint32_t errorbufferlength,
					uint32_t *errorlength,
					int64_t	*errorcode,
					bool *liveconnection);
		bool	liveConnection(SQLINTEGER nativeerror,
					const char *errorbuffer,
					SQLSMALLINT errlength);
		const char	*pingQuery();
		const char	*identify();
		const char	*dbVersion();
		const char	*dbHostNameQuery();
		const char	*getDatabaseListQuery(bool wild);
		const char	*getTableListQuery(bool wild,
						uint16_t objecttypes);
		const char	*getColumnListQuery(
					const char *table, bool wild);
		const char	*selectDatabaseQuery();
		const char	*getCurrentDatabaseQuery();
		const char	*getLastInsertIdQuery();
		const char	*setIsolationLevelQuery();
		const char	*noopQuery();
		const char	*bindFormat();

		SQLHENV		env;
		SQLRETURN	erg;
		SQLHDBC		dbc;

		const char	*db2path;
		const char	*server;
		const char	*lang;

		stringbuffer	errormessage;

		int32_t		maxoutbindlobsize;

		const char	*identity;

		char		dbversion[512];
		uint16_t	dbmajorversion;

		const char	*gettablelistquery;
		const char	*gettablelistquerywild;
		const char	*dbhostnamequery;

		stringbuffer	errormsg;
};

db2connection::db2connection(sqlrservercontroller *cont) :
					sqlrserverconnection(cont) {

	maxoutbindlobsize=MAX_OUT_BIND_LOB_SIZE;
	identity=NULL;
}

void db2connection::handleConnectString() {

	sqlrserverconnection::handleConnectString();

	db2path=cont->getConnectStringValue("db2path");

	// override legacy "server" parameter with modern "db" parameter
	server=cont->getConnectStringValue("server");
	const char	*tmp=cont->getConnectStringValue("db");
	if (!charstring::isNullOrEmpty(tmp)) {
		server=tmp;
	}

	identity=cont->getConnectStringValue("identity");

	lang=cont->getConnectStringValue("lang");

	maxoutbindlobsize=charstring::toInteger(
			cont->getConnectStringValue("maxoutbindlobsize"));
	if (maxoutbindlobsize<1) {
		maxoutbindlobsize=MAX_OUT_BIND_LOB_SIZE;
	}
}

bool db2connection::mustDetachBeforeLogIn() {
	#if (DB2VERSION>7)
	return false;
	#else
	return true;
	#endif
}

bool db2connection::logIn(const char **error, const char **warning) {

	// set the LANG environment variable
	if (charstring::length(lang) && !environment::setValue("LANG",lang)) {
		*error="Failed to set LANG environment variable";
		return false;
	}

	#ifdef DB2_AT_RUNTIME
	if (!loadLibraries(&errormessage,db2path)) {
		*error=errormessage.getString();
		return false;
	}
	#endif

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
	uint32_t	connecttimeout=cont->getConnectTimeout();
	if (connecttimeout) {
		erg=SQLSetConnectAttr(dbc,
				#ifdef SQL_ATTR_LOGIN_TIMEOUT
				SQL_ATTR_LOGIN_TIMEOUT,
				#else
				SQL_LOGIN_TIMEOUT,
				#endif
				(SQLPOINTER)connecttimeout,0);
		if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
			*error="Failed to set connect timeout";
			SQLFreeHandle(SQL_HANDLE_DBC,dbc);
			SQLFreeHandle(SQL_HANDLE_ENV,env);
			return false;
		}
	}

	// connect to the database
	erg=SQLConnect(dbc,(SQLCHAR *)server,SQL_NTS,
				(SQLCHAR *)cont->getUser(),SQL_NTS,
				(SQLCHAR *)cont->getPassword(),SQL_NTS);
	if (erg==SQL_SUCCESS_WITH_INFO) {
		*warning=logInError(NULL);
	} else if (erg!=SQL_SUCCESS) {
		*error=logInError("SQLConnect failed");
		SQLFreeHandle(SQL_HANDLE_DBC,dbc);
		SQLFreeHandle(SQL_HANDLE_ENV,env);
		return false;
	}

	dbVersionSpecificTasks();

	return true;
}

const char *db2connection::logInError(const char *errmsg) {

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

void db2connection::dbVersionSpecificTasks() {

	// get the db version
	SQLSMALLINT	dbversionlen;
	SQLGetInfo(dbc,SQL_DBMS_VER,
			(SQLPOINTER)dbversion,
			(SQLSMALLINT)sizeof(dbversion),
			&dbversionlen);
	dbmajorversion=charstring::toInteger(dbversion);

	// set queries to use based on version
	if (dbmajorversion>7) {

		gettablelistquery=
			"select distinct "
			"	NULL as table_cat, "
			"	tabschema as table_schem, "
			"	tabname as table_name, "
			"	'TABLE' as table_type, "
			"	NULL as remarks, "
			"	NULL as extra "
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
			"	tabschema, "
			"	tabname";

		gettablelistquerywild=
			"select distinct "
			"	NULL as table_cat, "
			"	tabschema as table_schem, "
			"	tabname as table_name, "
			"	'TABLE' as table_type, "
			"	NULL as remarks, "
			"	NULL as extra "
			"from "
			"	syscat.tables "
			"where "
			"	ownertype='U' "
			"	and "
			"	tabschema!='SYSTOOLS' "
			"	and "
			"	type in ('T','U','V','W') "
			"order by "
			"	tabschema, "
			"	tabname";

		dbhostnamequery=
			"select "
			"	host_name "
			"from "
			"	table(sysproc.env_get_sys_info())";
	} else {

		gettablelistquery=
			"select distinct "
			"	NULL as table_cat, "
			"	tabschema as table_schem, "
			"	tabname as table_name, "
			"	'TABLE' as table_type, "
			"	NULL as remarks, "
			"	NULL as extra "
			"from "
			"	syscat.tables "
			"where "
			"	definer!='SYSIBM' "
			"	and "
			"	tabschema!='SYSTOOLS' "
			"	and "
			"	type in ('T','U','V','W') "
			"	and "
			"	tabname like '%s' "
			"order by "
			"	tabschema, "
			"	tabname";

		gettablelistquerywild=
			"select distinct "
			"	NULL as table_cat, "
			"	tabschema as table_schem, "
			"	tabname as table_name, "
			"	'TABLE' as table_type, "
			"	NULL as remarks, "
			"	NULL as extra "
			"from "
			"	syscat.tables "
			"where "
			"	definer!='SYSIBM' "
			"	and "
			"	tabschema!='SYSTOOLS' "
			"	and "
			"	type in ('T','U','V','W') "
			"order by "
			"	tabschema, "
			"	tabname";

		// there is no obvious way to get this prior to 8.0
		dbhostnamequery=
			"select "
			"	NULL "
			"from "
			"	sysibm.sysdummy1";
	}
}

sqlrservercursor *db2connection::newCursor(uint16_t id) {
	return (sqlrservercursor *)new db2cursor((sqlrserverconnection *)this,id);
}

void db2connection::deleteCursor(sqlrservercursor *curs) {
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

bool db2connection::supportsAutoCommit() {
	return true;
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
	*liveconnection=liveConnection(nativeerrnum,errorbuffer,errlength);
}

bool db2connection::liveConnection(SQLINTEGER nativeerrnum,
					const char *errorbuffer,
					SQLSMALLINT errlength) {

	// When the DB goes down, DB2 first reports one error:
	// 	[IBM][CLI Driver] SQL1224N  A database agent could not be
	//	started to service a request, or was terminated as a result of
	//	a database system shutdown or a force command.  SQLSTATE=55032
	//	(in this case nativeerrnum==-1224 and errlength==184)
	// then upon repeated attempts to run a query, it reports:
	//	[IBM][CLI Driver] CLI0106E  Connection is closed. SQLSTATE=08003
	//	(in this case nativeerrnum==-99999 and errlength==64)
	//	(unforutnately other errors have the same error number and
	//	length, such as "Invalid cursor state." so we have to
	//	discriminate a bit for this one)
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
		(nativeerrnum==-99999 && errlength==64 &&
		charstring::contains(errorbuffer,"Connection is closed")) ||
		(nativeerrnum==-1224 && errlength==220) ||
		(nativeerrnum==-30081));
}


const char *db2connection::pingQuery() {
	return "values 1";
}

const char *db2connection::identify() {
	return (identity)?identity:"db2";
}

const char *db2connection::dbVersion() {
	return dbversion;
}

const char *db2connection::dbHostNameQuery() {
	return dbhostnamequery;
}

const char *db2connection::getDatabaseListQuery(bool wild) {
	return (wild)?
		"select "
		"	schemaname, "
		"	NULL "
		"from "
		"	syscat.schemata "
		"where "
		"	schemaname like '%s'":

		"select "
		"	schemaname, "
		"	NULL "
		"from "
		"	syscat.schemata ";
}

const char *db2connection::getTableListQuery(bool wild, uint16_t objecttypes) {
	return (wild)?gettablelistquery:gettablelistquerywild;
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
		"	'' as extra, "
		"	NULL "
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
		"	'' as extra, "
		"	NULL "
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

const char *db2connection::noopQuery() {
	return "begin end;";
}

db2cursor::db2cursor(sqlrserverconnection *conn, uint16_t id) :
						sqlrservercursor(conn,id) {
	db2conn=(db2connection *)conn;
	stmt=0;
	lobstmt=0;
	maxbindcount=conn->cont->getConfig()->getMaxBindCount();
	blobbindsize=new SQLINTEGER[maxbindcount];
	outdatebind=new datebind *[maxbindcount];
	outlobbind=new char *[maxbindcount];
	outlobbindlen=new SQLINTEGER[maxbindcount];
	outisnullptr=new int16_t *[maxbindcount];
	outisnull=new SQLINTEGER[maxbindcount];
	for (uint16_t i=0; i<maxbindcount; i++) {
		outdatebind[i]=NULL;
		outlobbind[i]=NULL;
		outlobbindlen[i]=0;
		outisnullptr[i]=NULL;
		outisnull[i]=0;
	}
	sqlnulldata=SQL_NULL_DATA;
	bindformaterror=false;
	allocateResultSetBuffers(conn->cont->getMaxColumnCount());
}

db2cursor::~db2cursor() {
	delete[] blobbindsize;
	delete[] outdatebind;
	delete[] outlobbind;
	delete[] outlobbindlen;
	delete[] outisnullptr;
	delete[] outisnull;
	deallocateResultSetBuffers();
}

void db2cursor::allocateResultSetBuffers(int32_t columncount) {

	if (!columncount) {
		this->columncount=0;
		field=NULL;
		loblocator=NULL;
		loblength=NULL;
		indicator=NULL;
		#if (DB2VERSION>7)
		rowstat=NULL;
		#endif
		column=NULL;
	} else {
		this->columncount=columncount;
		field=new char *[columncount];
		loblocator=new SQLINTEGER *[columncount];
		loblength=new SQLINTEGER *[columncount];
		indicator=new SQLINTEGER *[columncount];
		uint32_t	fetchatonce=conn->cont->getFetchAtOnce();
		uint32_t	maxfieldlength=conn->cont->getMaxFieldLength();
		#if (DB2VERSION>7)
		rowstat=new SQLUSMALLINT[fetchatonce];
		#endif
		column=new db2column[columncount];
		for (int32_t i=0; i<columncount; i++) {
			column[i].name=new char[4096];
			field[i]=new char[fetchatonce*maxfieldlength];
			loblocator[i]=new SQLINTEGER[fetchatonce];
			loblength[i]=new SQLINTEGER[fetchatonce];
			indicator[i]=new SQLINTEGER[fetchatonce];
		}
	}
}

void db2cursor::deallocateResultSetBuffers() {
	if (columncount) {
		for (int32_t i=0; i<columncount; i++) {
			delete[] column[i].name;
			delete[] field[i];
			delete[] loblocator[i];
			delete[] loblength[i];
			delete[] indicator[i];
		}
		delete[] column;
		delete[] field;
		delete[] loblocator;
		delete[] loblength;
		delete[] indicator;
		#if (DB2VERSION>7)
		delete[] rowstat;
		#endif
		columncount=0;
	}
}

bool db2cursor::open() {

	if (!stmt) {

		// allocate the cursor
		erg=SQLAllocHandle(SQL_HANDLE_STMT,db2conn->dbc,&stmt);
		if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
			return false;
		}

		// set the row array size
		erg=SQLSetStmtAttr(stmt,SQL_ATTR_ROW_ARRAY_SIZE,
				(SQLPOINTER)conn->cont->getFetchAtOnce(),0);
		if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
			return false;
		}

		#if (DB2VERSION>7)
		if (conn->cont->getMaxColumnCount()) {

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
	}

	if (!lobstmt) {

		// allocate a lob handle, in case we need it
		erg=SQLAllocHandle(SQL_HANDLE_STMT,db2conn->dbc,&lobstmt);
		if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
			return false;
		}
	}

	return true;
}

bool db2cursor::close() {

	if (stmt) {
		// free stmt handle
		SQLFreeHandle(SQL_HANDLE_STMT,stmt);
		stmt=0;
	}

	if (lobstmt) {
		// free lob handle
		SQLFreeHandle(SQL_HANDLE_STMT,lobstmt);
		lobstmt=0;
	}

	return true;
}

bool db2cursor::prepareQuery(const char *query, uint32_t length) {

	bindformaterror=false;

	// initialize column count
	ncols=0;

	// prepare the query
	erg=SQLPrepare(stmt,(SQLCHAR *)query,length);
	return (erg==SQL_SUCCESS || erg==SQL_SUCCESS_WITH_INFO);
}

void db2cursor::encodeBlob(stringbuffer *buffer,
					const char *data, uint32_t datasize) {

	// db2 sort-of follows the SQL Standard:
	// X'...' where ... is the blob data and each byte of blob data is
	// converted to two hex characters..
	// eg: hello -> X'68656C6C6F'
	// but db2 also requires that the blob() function be used

	buffer->append("blob(X\'");
	for (uint32_t i=0; i<datasize; i++) {
		buffer->append(conn->cont->asciiToHex(data[i]));
	}
	buffer->append("\')");
}

bool db2cursor::inputBind(const char *variable,
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
		// the 4th parameter (ValueType) must by
		// SQL_C_BINARY for this to work with blobs
		erg=SQLBindParameter(stmt,
				pos,
				SQL_PARAM_INPUT,
				SQL_C_BINARY,
				SQL_CHAR,
				0,
				0,
				(SQLPOINTER)value,
				valuesize,
				&sqlnulldata);
	} else {
		erg=SQLBindParameter(stmt,
				pos,
				SQL_PARAM_INPUT,
				SQL_C_CHAR,
				SQL_CHAR,
				0,
				0,
				(SQLPOINTER)value,
				valuesize,
				NULL);
	}
	return (erg==SQL_SUCCESS || erg==SQL_SUCCESS_WITH_INFO);
}

bool db2cursor::inputBind(const char *variable,
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
				SQL_C_LONG,
				SQL_INTEGER,
				0,
				0,
				value,
				sizeof(int64_t),
				NULL);
	return (erg==SQL_SUCCESS || erg==SQL_SUCCESS_WITH_INFO);
}

bool db2cursor::inputBind(const char *variable,
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
				SQL_DOUBLE,
				precision,
				scale,
				value,
				sizeof(double),
				NULL);
	return (erg==SQL_SUCCESS || erg==SQL_SUCCESS_WITH_INFO);
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
				NULL);
	}
	return (erg==SQL_SUCCESS || erg==SQL_SUCCESS_WITH_INFO);
}

bool db2cursor::inputBindBlob(const char *variable,
					uint16_t variablesize,
					const char *value,
					uint32_t valuesize,
					int16_t *isnull) {

	uint16_t	pos=charstring::toInteger(variable+1);
	if (!pos || pos>maxbindcount) {
		bindformaterror=true;
		return false;
	}

	blobbindsize[pos-1]=valuesize;
	erg=SQLBindParameter(stmt,
				pos,
				SQL_PARAM_INPUT,
				SQL_C_BINARY,
				SQL_BLOB,
				valuesize,
				0,
				(SQLPOINTER)value,
				valuesize,
				&(blobbindsize[pos-1]));
	return (erg==SQL_SUCCESS || erg==SQL_SUCCESS_WITH_INFO);
}

#if (DB2VERSION<=7)
// The default implementeation of inputBindClob() just calls inputBind().
// That works fine for versions > 7 but does not work with 7 or less.
// Conversely, SQL_CLOB doesn't work with some versions > 7 so we can't use
// this code with versions > 7.
bool db2cursor::inputBindClob(const char *variable,
					uint16_t variablesize,
					const char *value,
					uint32_t valuesize,
					int16_t *isnull) {

	uint16_t	pos=charstring::toInteger(variable+1);
	if (!pos || pos>maxbindcount) {
		bindformaterror=true;
		return false;
	}

	erg=SQLBindParameter(stmt,
				pos,
				SQL_PARAM_INPUT,
				SQL_C_CHAR,
				SQL_CLOB,
				0,
				0,
				(SQLPOINTER)value,
				valuesize,
				NULL);
	return (erg==SQL_SUCCESS || erg==SQL_SUCCESS_WITH_INFO);
}
#endif

bool db2cursor::outputBind(const char *variable, 
					uint16_t variablesize,
					char *value, 
					uint32_t valuesize, 
					int16_t *isnull) {

	uint16_t	pos=charstring::toInteger(variable+1);
	if (!pos || pos>maxbindcount) {
		bindformaterror=true;
		return false;
	}

	outdatebind[pos-1]=NULL;
	outisnullptr[pos-1]=isnull;

	erg=SQLBindParameter(stmt,
				pos,
				SQL_PARAM_OUTPUT,
				SQL_C_CHAR,
				SQL_CHAR,
				0,
				0,
				value,
				valuesize,
				&(outisnull[pos-1])
				);
	return (erg==SQL_SUCCESS || erg==SQL_SUCCESS_WITH_INFO);
}

bool db2cursor::outputBind(const char *variable,
					uint16_t variablesize,
					int64_t *value,
					int16_t *isnull) {

	uint16_t	pos=charstring::toInteger(variable+1);
	if (!pos || pos>maxbindcount) {
		bindformaterror=true;
		return false;
	}

	outdatebind[pos-1]=NULL;
	outisnullptr[pos-1]=isnull;

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
				&(outisnull[pos-1])
				);
	return (erg==SQL_SUCCESS || erg==SQL_SUCCESS_WITH_INFO);
}

bool db2cursor::outputBind(const char *variable,
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
				&(outisnull[pos-1])
				);
	return (erg==SQL_SUCCESS || erg==SQL_SUCCESS_WITH_INFO);
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
				&(outisnull[pos-1])
				);
	return (erg==SQL_SUCCESS || erg==SQL_SUCCESS_WITH_INFO);
}

bool db2cursor::outputBindBlob(const char *variable, 
					uint16_t variablesize,
					uint16_t index,
					int16_t *isnull) {

	uint16_t	pos=charstring::toInteger(variable+1);
	if (!pos || pos>maxbindcount) {
		bindformaterror=true;
		return false;
	}

	outlobbind[index]=new char[db2conn->maxoutbindlobsize];

	// FIXME: Ideally we'd bind a lob locator like we are for columns,
	// but I can't seem to get that working.
	erg=SQLBindParameter(stmt,
				pos,
				SQL_PARAM_OUTPUT,
				SQL_C_BINARY,
				SQL_BLOB,
				0,
				0,
				outlobbind[index],
				db2conn->maxoutbindlobsize,
				&outlobbindlen[index]);
	return (erg==SQL_SUCCESS || erg==SQL_SUCCESS_WITH_INFO);
}

bool db2cursor::outputBindClob(const char *variable, 
					uint16_t variablesize,
					uint16_t index,
					int16_t *isnull) {

	uint16_t	pos=charstring::toInteger(variable+1);
	if (!pos || pos>maxbindcount) {
		bindformaterror=true;
		return false;
	}

	outlobbind[index]=new char[db2conn->maxoutbindlobsize];

	// FIXME: Ideally we'd bind a lob locator like we are for columns,
	// but I can't seem to get that working.
	//
	// SQL_CHAR is used instead of SQL_CLOB for versions > 7.
	// Some versions of DB2 don't have SQL_CLOB, but all have SQL_CHAR.
	// SQL_CHAR works on versions > 7 but does not work on versions <= 7
	erg=SQLBindParameter(stmt,
				pos,
				SQL_PARAM_OUTPUT,
				SQL_C_CHAR,
				#if (DB2VERSION>7)
				SQL_CHAR,
				#else
				SQL_CLOB,
				#endif
				0,
				0,
				outlobbind[index],
				db2conn->maxoutbindlobsize,
				&outlobbindlen[index]);
	return (erg==SQL_SUCCESS || erg==SQL_SUCCESS_WITH_INFO);
}

bool db2cursor::getLobOutputBindLength(uint16_t index, uint64_t *length) {
	if (outlobbindlen[index]>db2conn->maxoutbindlobsize) {
		outlobbindlen[index]=db2conn->maxoutbindlobsize;
	}
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
	bytestring::copy(buffer,outlobbind[index]+offset,charstoread);
	*charsread=charstoread;
	return true;
}

bool db2cursor::executeQuery(const char *query, uint32_t length) {

	// initialize row counts
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
	if (!conn->cont->getMaxColumnCount()) {

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

	} else if ((uint32_t)ncols>conn->cont->getMaxColumnCount()) {
		ncols=conn->cont->getMaxColumnCount();
	}

	// run through the columns
	for (SQLSMALLINT i=0; i<ncols; i++) {

		if (conn->cont->getSendColumnInfo()==SEND_COLUMN_INFO) {

			// column name
			erg=SQLColAttribute(stmt,i+1,SQL_COLUMN_LABEL,
					column[i].name,4096,
					(SQLSMALLINT *)&(column[i].namelength),
					NULL);
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return false;
			}

			// column length
			erg=SQLColAttribute(stmt,i+1,SQL_COLUMN_LENGTH,
					NULL,0,NULL,&(column[i].length));
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return false;
			}

			// column type
			erg=SQLColAttribute(stmt,i+1,SQL_COLUMN_TYPE,
					NULL,0,NULL,&(column[i].type));
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return false;
			}

			// column precision
			erg=SQLColAttribute(stmt,i+1,SQL_COLUMN_PRECISION,
					NULL,0,NULL,&(column[i].precision));
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return false;
			}

			// column scale
			erg=SQLColAttribute(stmt,i+1,SQL_COLUMN_SCALE,
					NULL,0,NULL,&(column[i].scale));
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return false;
			}

			// column nullable
			erg=SQLColAttribute(stmt,i+1,SQL_COLUMN_NULLABLE,
					NULL,0,NULL,&(column[i].nullable));
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return false;
			}

			// primary key

			// unique

			// part of key

			// unsigned number
			erg=SQLColAttribute(stmt,i+1,SQL_COLUMN_UNSIGNED,
				NULL,0,NULL,&(column[i].unsignednumber));
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return false;
			}

			// zero fill

			// binary

			// autoincrement
			erg=SQLColAttribute(stmt,i+1,
				SQL_COLUMN_AUTO_INCREMENT,
				NULL,0,NULL,&(column[i].autoincrement));
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return false;
			}

			// table name
			erg=SQLColAttribute(stmt,i+1,
				SQL_COLUMN_TABLE_NAME,
				column[i].table,4096,
				(SQLSMALLINT *)&(column[i].tablelength),
				NULL);
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return false;
			}
			column[i].tablelength=
				charstring::length(column[i].table);
		}

		// bind the column to a lob locator or buffer
		switch (column[i].type) {
			case SQL_CLOB:
				erg=SQLBindCol(stmt,i+1,SQL_C_CLOB_LOCATOR,
						loblocator[i],0,
						indicator[i]);
				break;
			case SQL_BLOB:
				erg=SQLBindCol(stmt,i+1,SQL_C_BLOB_LOCATOR,
						loblocator[i],0,
						indicator[i]);
				break;
			default:
				erg=SQLBindCol(stmt,i+1,SQL_C_CHAR,
					field[i],
					conn->cont->getMaxFieldLength(),
					indicator[i]);
				break;
		}
		if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
			return false;
		}
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

void db2cursor::errorMessage(char *errorbuffer,
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

	SQLCHAR		state[10];
	SQLINTEGER	nativeerrnum;
	SQLSMALLINT	errlength;

	SQLGetDiagRec(SQL_HANDLE_STMT,stmt,1,state,&nativeerrnum,
				(SQLCHAR *)errorbuffer,errorbufferlength,
				&errlength);

	// set return values
	*errorlength=errlength;
	*errorcode=nativeerrnum;
	*liveconnection=db2conn->liveConnection(nativeerrnum,
						errorbuffer,errlength);
}

uint64_t db2cursor::affectedRows() {
	return affectedrows;
}

uint32_t db2cursor::colCount() {
	return ncols;
}

const char *db2cursor::getColumnName(uint32_t i) {
	return column[i].name;
}

uint16_t db2cursor::getColumnNameLength(uint32_t i) {
	return column[i].namelength;
}

uint16_t db2cursor::getColumnType(uint32_t i) {
	switch (column[i].type) {
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
	return column[i].length;
}

uint32_t db2cursor::getColumnPrecision(uint32_t i) {
	return column[i].precision;
}

uint32_t db2cursor::getColumnScale(uint32_t i) {
	return column[i].scale;
}

uint16_t db2cursor::getColumnIsNullable(uint32_t i) {
	return column[i].nullable;
}

uint16_t db2cursor::getColumnIsUnsigned(uint32_t i) {
	return column[i].unsignednumber;
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
	return column[i].autoincrement;
}

const char *db2cursor::getColumnTable(uint32_t i) {
	return column[i].table;
}

uint16_t db2cursor::getColumnTableLength(uint32_t i) {
	return column[i].tablelength;
}

bool db2cursor::noRowsToReturn() {
	// if there are no columns, then there can't be any rows either
	return (ncols)?false:true;
}

bool db2cursor::skipRow(bool *error) {
	if (fetchRow(error)) {
		rowgroupindex++;
		return true;
	}
	return false;
}

bool db2cursor::fetchRow(bool *error) {

	*error=false;

	if (rowgroupindex==conn->cont->getFetchAtOnce()) {
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
		if (result==SQL_ERROR) {
			*error=true;
			return false;
		}
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
		while (index<conn->cont->getFetchAtOnce() &&
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

	// handle blobs
	if (column[col].type==SQL_CLOB || column[col].type==SQL_BLOB) {
		*blob=true;
		return;
	}

	// handle normal datatypes
	*fld=&field[col][rowgroupindex*conn->cont->getMaxFieldLength()];
	*fldlength=indicator[col][rowgroupindex];
}

void db2cursor::nextRow() {
	rowgroupindex++;
}

bool db2cursor::getLobFieldLength(uint32_t col, uint64_t *length) {

	// get the length of the lob
	SQLINTEGER	ind;
	SQLSMALLINT	locatortype=(column[col].type==SQL_CLOB)?
						SQL_C_CLOB_LOCATOR:
						SQL_C_BLOB_LOCATOR;
	erg=SQLGetLength(lobstmt,locatortype,
				loblocator[col][rowgroupindex],
				&loblength[col][rowgroupindex],&ind);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return false;
	}

	// copy out the length
	*length=loblength[col][rowgroupindex];

	return true;
}

bool db2cursor::getLobFieldSegment(uint32_t col,
					char *buffer, uint64_t buffersize,
					uint64_t offset, uint64_t charstoread,
					uint64_t *charsread) {

	// Usually, methods to fetch lob segments return an error, or at least
	// return that fewer characters were read than attempted, when we try
	// to read past the end.  SQLGetSubString does not.  It allows you to
	// read past the end and just returns a bunch of nulls if you do and
	// doesn't indicate that anything odd has happened.  So we have to
	// detect attempts to read past the end ourselves.

	// bail if we're attempting to start reading past the end
	if (offset>(uint64_t)loblength[col][rowgroupindex]) {
		return false;
	}

	// prevent attempts to read past the end
	if (offset+charstoread>(uint64_t)loblength[col][rowgroupindex]) {
		charstoread=charstoread-
			((offset+charstoread)-loblength[col][rowgroupindex]);
	}

	// read a blob segment, at most MAX_LOB_CHUNK_SIZE bytes at a time
	uint64_t	totalbytesread=0;
	SQLUINTEGER	bytestoread=0;
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
		SQLINTEGER	bytesread=0;
		SQLINTEGER	ind=0;
		SQLSMALLINT	locatortype=(column[col].type==SQL_CLOB)?
							SQL_C_CLOB_LOCATOR:
							SQL_C_BLOB_LOCATOR;
		SQLSMALLINT	targettype=(column[col].type==SQL_CLOB)?
							SQL_C_CHAR:
							SQL_C_BINARY;
		erg=SQLGetSubString(lobstmt,locatortype,
					loblocator[col][rowgroupindex],
					offset+1,bytestoread,
					targettype,buffer+totalbytesread,
					buffersize-totalbytesread,
					&bytesread,&ind);
		if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
			return false;
		}

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

void db2cursor::closeResultSet() {
	SQLCloseCursor(stmt);

	for (uint16_t i=0; i<getOutputBindCount(); i++) {
		delete outdatebind[i];
		outdatebind[i]=NULL;
		delete outlobbind[i];
		outlobbind[i]=NULL;
		outlobbindlen[i]=0;
		outisnullptr[i]=NULL;
		outisnull[i]=0;
	}

	if (!conn->cont->getMaxColumnCount()) {
		deallocateResultSetBuffers();
	}
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrserverconnection *new_db2connection(
						sqlrservercontroller *cont) {
		return new db2connection(cont);
	}
}
