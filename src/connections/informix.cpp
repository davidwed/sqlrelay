// Copyright (c) 1999-2015  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/environment.h>

#include <datatypes.h>
#include <defines.h>
#include <config.h>

#include <infxcli.h>

// column nullability doesn't work with SQLColAttribute
#define USE_NULLABLE 0

// multi-row fetch doesn't work with clobs/blobs because you're already on a
// different row when SQLGetData is called to get the data for the clob/blob
// on the first row
#define FETCH_AT_ONCE		1
#define MAX_SELECT_LIST_SIZE	256
#define MAX_ITEM_BUFFER_SIZE	32768	
#define MAX_OUT_BIND_LOB_SIZE	2097152

#define MAX_LOB_CHUNK_SIZE	2147483647

struct informixcolumn {
	char		*name;
	uint16_t	namelength;
	// SQLColAttribute requires that these are signed, 32 bit integers
	int32_t		type;
	int32_t		length;
	int32_t		precision;
	int32_t		scale;
	#if USE_NULLABLE == 1
	int32_t		nullable;
	#endif
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

class informixconnection;

class SQLRSERVER_DLLSPEC informixcursor : public sqlrservercursor {
	friend class informixconnection;
	public:
			informixcursor(sqlrserverconnection *conn, uint16_t id);
			~informixcursor();
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
		bool		inputBindClob(const char *variable,
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
		#if USE_NULLABLE == 1
		uint16_t	getColumnIsNullable(uint32_t i);
		#endif
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
		SQLLEN 		affectedrows;

		int32_t		selectlistsize;
		char		**field;
		SQLLEN		**loblength;
		SQLLEN		**indicator;
		informixcolumn	*column;

		uint16_t	maxbindcount;
		SQLLEN		*lobbindsize;
		datebind	**outdatebind;
		char		**outlobbind;
		SQLLEN 		*outlobbindlen;
		SQLLEN		sqlnulldata;
		BOOL		truevalue;

		uint64_t	rowgroupindex;
		uint64_t	totalinrowgroup;
		uint64_t	totalrows;
		uint64_t	rownumber;

		stringbuffer	errormsg;

		informixconnection	*informixconn;
};

class SQLRSERVER_DLLSPEC informixconnection : public sqlrserverconnection {
	friend class informixcursor;
	public:
			informixconnection(sqlrservercontroller *cont);
	private:
		void	handleConnectString();
		bool	logIn(const char **error, const char **warning);
		const char	*logInError(const char *errmsg);
		sqlrservercursor	*newCursor(uint16_t id);
		void	deleteCursor(sqlrservercursor *curs);
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
					const char *errorbuffer,
					SQLSMALLINT errlength);
		const char	*pingQuery();
		const char	*identify();
		const char	*dbVersion();
		const char	*dbHostNameQuery();
		const char	*getDatabaseListQuery(bool wild);
		const char	*getTableListQuery(bool wild);
		const char	*getColumnListQuery(
					const char *table, bool wild);
		const char	*selectDatabaseQuery();
		const char	*getCurrentDatabaseQuery();
		const char	*getLastInsertIdQuery();
		const char	*setIsolationLevelQuery();
		const char	*bindFormat();

		SQLHENV		env;
		SQLRETURN	erg;
		SQLHDBC		dbc;

		const char	*informixdir;
		const char	*servername;
		const char	*database;
		const char	*lang;
		uint32_t	timeout;
		stringbuffer	dsn;

		stringbuffer	errormessage;

		uint32_t	fetchatonce;
		int32_t		maxselectlistsize;
		int32_t		maxitembuffersize;
		int32_t		maxoutbindlobsize;

		const char	*identity;

		char		dbversion[512];

		stringbuffer	errormsg;
};

informixconnection::informixconnection(sqlrservercontroller *cont) :
					sqlrserverconnection(cont) {

	fetchatonce=FETCH_AT_ONCE;
	maxselectlistsize=MAX_SELECT_LIST_SIZE;
	maxitembuffersize=MAX_ITEM_BUFFER_SIZE;
	maxoutbindlobsize=MAX_OUT_BIND_LOB_SIZE;
	identity=NULL;
}

void informixconnection::handleConnectString() {

	// get informix dir
	informixdir=cont->getConnectStringValue("informixdir");

	// get dsn components
	servername=cont->getConnectStringValue("servername");
	if (!servername || !servername[0]) {
		servername=environment::getValue("INFORMIXSERVER");
	}
	database=cont->getConnectStringValue("database");
	cont->setUser(cont->getConnectStringValue("user"));
	cont->setPassword(cont->getConnectStringValue("password"));

	// build dsn
	dsn.clear();
	if (servername && servername[0]) {
		dsn.append("Servername=")->append(servername);
	}
	if (database && database[0]) {
		if (dsn.getStringLength()) {
			dsn.append(";");
		}
		dsn.append("Database=")->append(database);
	}
	const char	*user=cont->getUser();
	if (user && user[0]) {
		if (dsn.getStringLength()) {
			dsn.append(";");
		}
		dsn.append("LogonID=")->append(user);
	}
	const char	*pass=cont->getPassword();
	if (pass && pass[0]) {
		if (dsn.getStringLength()) {
			dsn.append(";");
		}
		dsn.append("pwd=")->append(pass);
	}

	// get other parameters
	const char	*autocom=cont->getConnectStringValue("autocommit");
	cont->setAutoCommitBehavior((autocom &&
		!charstring::compareIgnoringCase(autocom,"yes")));
	lang=cont->getConnectStringValue("lang");
	cont->setFakeTransactionBlocksBehavior(
		!charstring::compare(
			cont->getConnectStringValue("faketransactionblocks"),
			"yes"));
	if (!charstring::compare(
			cont->getConnectStringValue("fakebinds"),"yes")) {
		cont->fakeInputBinds();
	}

	timeout=charstring::toInteger(cont->getConnectStringValue("timeout"));

	// multi-row fetch doesn't work with clobs/blobs because you're already
	// on a different row when SQLGetData is called to get the data for the
	// clob/blob on the first row
	/*fetchatonce=charstring::toUnsignedInteger(
				cont->getConnectStringValue("fetchatonce"));
	if (fetchatonce<1) {
		fetchatonce=FETCH_AT_ONCE;
	}*/

	maxselectlistsize=charstring::toInteger(
			cont->getConnectStringValue("maxselectlistsize"));
	if (!maxselectlistsize || maxselectlistsize<-1) {
		maxselectlistsize=MAX_SELECT_LIST_SIZE;
	}
	maxitembuffersize=charstring::toInteger(
			cont->getConnectStringValue("maxitembuffersize"));
	if (maxitembuffersize<1) {
		maxitembuffersize=MAX_ITEM_BUFFER_SIZE;
	}
	maxoutbindlobsize=charstring::toInteger(
			cont->getConnectStringValue("maxoutbindlobsize"));
	if (maxoutbindlobsize<1) {
		maxoutbindlobsize=MAX_OUT_BIND_LOB_SIZE;
	}
	identity=cont->getConnectStringValue("identity");
}

bool informixconnection::logIn(const char **error, const char **warning) {

	// set the INFORMIX environment variable
	if (charstring::length(informixdir) &&
		!environment::setValue("INFORMIXDIR",informixdir)) {
		*error="Failed to set INFORMIXDIR environment variable";
		return false;
	}

	// set the LANG environment variable
	if (charstring::length(lang) &&
		!environment::setValue("LANG",lang)) {
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
				(SQLPOINTER)timeout,0);
		if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
			*error="Failed to set timeout";
			SQLFreeHandle(SQL_HANDLE_DBC,dbc);
			SQLFreeHandle(SQL_HANDLE_ENV,env);
			return false;
		}
	}

	// connect to the database
	erg=SQLDriverConnect(dbc,NULL,
				(SQLCHAR *)dsn.getString(),
				dsn.getStringLength(),
				NULL,0,NULL,SQL_DRIVER_COMPLETE);
	if (erg==SQL_SUCCESS_WITH_INFO) {
		*warning=logInError(NULL);
	} else if (erg!=SQL_SUCCESS) {
		*error=logInError("SQLConnect failed");
		SQLFreeHandle(SQL_HANDLE_DBC,dbc);
		SQLFreeHandle(SQL_HANDLE_ENV,env);
		return false;
	}

	// get db version
	SQLSMALLINT	dbversionlen;
	SQLGetInfo(dbc,SQL_DBMS_VER,
			(SQLPOINTER)dbversion,
			(SQLSMALLINT)sizeof(dbversion),
			&dbversionlen);

	return true;
}

const char *informixconnection::logInError(const char *errmsg) {

	errormessage.clear();
	if (errmsg) {
		errormessage.append(errmsg)->append(": ");
	}

	// get the error message from informix
	SQLCHAR		state[10];
	SQLINTEGER	nativeerrnum;
	SQLCHAR		errorbuffer[1024];
	SQLSMALLINT	errlength;

	SQLGetDiagRec(SQL_HANDLE_DBC,dbc,1,state,&nativeerrnum,
					errorbuffer,1024,&errlength);
	errormessage.append(errorbuffer,errlength);
	return errormessage.getString();
}

sqlrservercursor *informixconnection::newCursor(uint16_t id) {
	return (sqlrservercursor *)new informixcursor(
					(sqlrserverconnection *)this,id);
}

void informixconnection::deleteCursor(sqlrservercursor *curs) {
	delete (informixcursor *)curs;
}

void informixconnection::logOut() {
	SQLDisconnect(dbc);
	SQLFreeHandle(SQL_HANDLE_DBC,dbc);
	SQLFreeHandle(SQL_HANDLE_ENV,env);
}

int16_t informixconnection::nullBindValue() {
	return SQL_NULL_DATA;
}

bool informixconnection::bindValueIsNull(int16_t isnull) {
	if (isnull==SQL_NULL_DATA) {
		return true;
	}
	return false;
}

bool informixconnection::autoCommitOn() {
	return (SQLSetConnectAttr(dbc,SQL_ATTR_AUTOCOMMIT,
				(SQLPOINTER)SQL_AUTOCOMMIT_ON,
				sizeof(SQLINTEGER))==SQL_SUCCESS);
}

bool informixconnection::autoCommitOff() {
	return (SQLSetConnectAttr(dbc,SQL_ATTR_AUTOCOMMIT,
				(SQLPOINTER)SQL_AUTOCOMMIT_OFF,
				sizeof(SQLINTEGER))==SQL_SUCCESS);
}

bool informixconnection::supportsTransactionBlocks() {
	return false;
}

bool informixconnection::commit() {
	return (SQLEndTran(SQL_HANDLE_ENV,env,SQL_COMMIT)==SQL_SUCCESS);
}

bool informixconnection::rollback() {
	return (SQLEndTran(SQL_HANDLE_ENV,env,SQL_ROLLBACK)==SQL_SUCCESS);
}

void informixconnection::errorMessage(char *errorbuffer,
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

bool informixconnection::liveConnection(SQLINTEGER nativeerrnum,
					const char *errorbuffer,
					SQLSMALLINT errlength) {

	// When the DB goes down, Informix reports:
	// -11020: [Informix][Informix ODBC Driver]Communication link failure.
	// (if there are other errors then I haven't seen them yet)
	return nativeerrnum!=-11020;
}


const char *informixconnection::pingQuery() {
	return "select 1 from sysmaster:sysdual";
}

const char *informixconnection::identify() {
	return (identity)?identity:"informix";
}

const char *informixconnection::dbVersion() {
	return dbversion;
}

const char *informixconnection::dbHostNameQuery() {
	return "select dbinfo('dbname') from sysmaster:sysdual";
	//return "select os_nodename from sysmaster:sysmachineinfo";
}

const char *informixconnection::getDatabaseListQuery(bool wild) {
	return (wild)?
		"select "
		"	name, "
		"	'' as extra "
		"from "
		"	sysmaster:sysdatabases "
		"where "
		"	name like '%s'":

		"select "
		"	name, "
		"	'' as extra "
		"from "
		"	sysmaster:sysdatabases ";
}

const char *informixconnection::getTableListQuery(bool wild) {
	return (wild)?
		"select distinct "
		"	tabname, "
		"	'' as extra "
		"from "
		"	systables "
		"where "
		"	tabid > 99 "
		"	and "
		"	tabname like '%s' "
		"	and "
		"	tabtype in ('T','S','P','V') "
		"order by "
		"	tabname":

		"select distinct "
		"	tabname, "
		"	'' as extra "
		"from "
		"	systables "
		"where "
		"	tabid > 99 "
		"	and "
		"	tabtype in ('T','S','P','V') "
		"order by "
		"	tabname";
}

const char *informixconnection::getColumnListQuery(
					const char *table, bool wild) {

#define COLTYPE \
		"	decode(coltype, " \
		"		41,'boolean', " \
		"		1,'smallint', " \
		"		2,'int', " \
		"		52,'bigint', " \
		"		17,'int8', " \
		"		5,'decimal', " \
		"		8,'money', " \
		"		4,'smallfloat', " \
		"		3,'float', " \
		"		0,'char', " \
		"		15,'nchar', " \
		"		13,'varchar', " \
		"		16,'nvarchar', " \
		"		40,'lvarchar', " \
		"		7,'date', " \
		"		10,'datetime', " \
		"		12,'text', " \
		"		11,'byte', " \
		"		'unknown') as coltype, "

	// FIXME: collength is impossibly large
	// see col_cnvrt(), fix_nm(), and fix_dt() in dbdiff2 at
	// http://www.iiug.org/library/faqs/informix-faq/dbdiff2.4gl.txt
	return (wild)?
		"select "
		"	colname, "
		COLTYPE
		"	'' as length, "
		"	'' as precision, "
		"	'' as scale, "
		"	'' as nulls, "
		"	'' as key, "
		"	'' as default, "
		"	'' as extra, "
		"	'' as extra2 "
		"from "
		"	systables, "
		"	syscolumns "
		"where "
		"	upper(systables.tabname)=upper('%s') "
		"	and "
		"	syscolumns.tabid=systables.tabid "
		"	and "
		"	syscolumns.colname like '%s' "
		"order by "
		"	colno":

		"select "
		"	colname, "
		COLTYPE
		"	'' as length, "
		"	'' as precision, "
		"	'' as scale, "
		"	'' as nulls, "
		"	'' as key, "
		"	'' as default, "
		"	'' as extra, "
		"	'' as extra2 "
		"from "
		"	systables, "
		"	syscolumns "
		"where "
		"	upper(systables.tabname)=upper('%s') "
		"	and "
		"	syscolumns.tabid=systables.tabid "
		"order by "
		"	colno";
}

const char *informixconnection::bindFormat() {
	return "?";
}

const char *informixconnection::selectDatabaseQuery() {
	// FIXME: this doesn't actually work
	return "set connection '%s'";
}

const char *informixconnection::getCurrentDatabaseQuery() {
	return "select dbinfo('dbname') from sysmaster:sysdual";
}

const char *informixconnection::getLastInsertIdQuery() {
	return "select dbinfo('sqlca.sqlerrd1') from sysmaster:sysdual";
	//return "select dbinfo('serial8') from sysmaster:sysdual";
	//return "select dbinfo('bigserial') from sysmaster:sysdual";
}

const char *informixconnection::setIsolationLevelQuery() {
        return "set isolation %s";
}

informixcursor::informixcursor(sqlrserverconnection *conn, uint16_t id) :
						sqlrservercursor(conn,id) {
	informixconn=(informixconnection *)conn;
	stmt=0;
	maxbindcount=conn->cont->cfgfl->getMaxBindCount();
	lobbindsize=new SQLLEN[maxbindcount];
	outdatebind=new datebind *[maxbindcount];
	outlobbind=new char *[maxbindcount];
	outlobbindlen=new SQLLEN[maxbindcount];
	for (uint16_t i=0; i<maxbindcount; i++) {
		outdatebind[i]=NULL;
		outlobbind[i]=NULL;
		outlobbindlen[i]=0;
	}
	sqlnulldata=SQL_NULL_DATA;
	allocateResultSetBuffers(informixconn->maxselectlistsize);
	truevalue=SQL_TRUE;
}

informixcursor::~informixcursor() {
	delete[] lobbindsize;
	delete[] outdatebind;
	delete[] outlobbind;
	delete[] outlobbindlen;
	deallocateResultSetBuffers();
}

void informixcursor::allocateResultSetBuffers(int32_t selectlistsize) {

	if (selectlistsize==-1) {
		this->selectlistsize=0;
		field=NULL;
		loblength=NULL;
		indicator=NULL;
		column=NULL;
	} else {
		this->selectlistsize=selectlistsize;
		field=new char *[selectlistsize];
		loblength=new SQLLEN *[selectlistsize];
		indicator=new SQLLEN *[selectlistsize];
		column=new informixcolumn[selectlistsize];
		for (int32_t i=0; i<selectlistsize; i++) {
			column[i].name=new char[4096];
			field[i]=new char[informixconn->fetchatonce*
					informixconn->maxitembuffersize];
			loblength[i]=new SQLLEN[informixconn->fetchatonce];
			indicator[i]=new SQLLEN[informixconn->fetchatonce];
		}
	}
}

void informixcursor::deallocateResultSetBuffers() {
	if (selectlistsize) {
		for (int32_t i=0; i<selectlistsize; i++) {
			delete[] column[i].name;
			delete[] field[i];
			delete[] loblength[i];
			delete[] indicator[i];
		}
		delete[] column;
		delete[] field;
		delete[] loblength;
		delete[] indicator;
		selectlistsize=0;
	}
}

bool informixcursor::prepareQuery(const char *query, uint32_t length) {

	if (stmt) {
		SQLFreeHandle(SQL_HANDLE_STMT,stmt);
	}

	// allocate the cursor
	erg=SQLAllocHandle(SQL_HANDLE_STMT,informixconn->dbc,&stmt);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return false;
	}

	// set the row array size
	erg=SQLSetStmtAttr(stmt,SQL_ATTR_ROW_ARRAY_SIZE,
				(SQLPOINTER)informixconn->fetchatonce,0);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return false;
	}

	// enable smart-large-object automation for non-selects
	erg=SQLSetStmtAttr(stmt,SQL_INFX_ATTR_LO_AUTOMATIC,
					(SQLPOINTER)truevalue,0);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return false;
	}

	// prepare the query
	erg=SQLPrepare(stmt,(SQLCHAR *)query,length);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return false;
	}
	return true;
}

bool informixcursor::inputBind(const char *variable,
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
				&sqlnulldata);
	} else {
		erg=SQLBindParameter(stmt,
				charstring::toInteger(variable+1),
				SQL_PARAM_INPUT,
				SQL_C_CHAR,
				SQL_CHAR,
				// the parameter below must be set
				// for informix, unlike db2 and odbc
				valuesize,
				0,
				(SQLPOINTER)value,
				valuesize,
				(SQLLEN *)NULL);
	}
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return false;
	}
	return true;
}

bool informixcursor::inputBind(const char *variable,
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
				(SQLLEN *)NULL);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return false;
	}
	return true;
}

bool informixcursor::inputBind(const char *variable,
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
				(SQLLEN *)NULL);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return false;
	}
	return true;
}

bool informixcursor::inputBind(const char *variable,
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
				(SQLLEN *)NULL);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return false;
	}
	return true;
}

bool informixcursor::inputBindBlob(const char *variable,
					uint16_t variablesize,
					const char *value,
					uint32_t valuesize,
					int16_t *isnull) {

	lobbindsize[inbindcount]=valuesize;
	erg=SQLBindParameter(stmt,
				charstring::toInteger(variable+1),
				SQL_PARAM_INPUT,
				SQL_C_BINARY,
				SQL_LONGVARBINARY,
				valuesize,
				0,
				(SQLPOINTER)value,
				valuesize,
				&(lobbindsize[inbindcount]));
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return false;
	}
	return true;
}

bool informixcursor::inputBindClob(const char *variable,
					uint16_t variablesize,
					const char *value,
					uint32_t valuesize,
					int16_t *isnull) {

	lobbindsize[inbindcount]=valuesize;
	erg=SQLBindParameter(stmt,
				charstring::toInteger(variable+1),
				SQL_PARAM_INPUT,
				// SQL_C_CHAR works as expected with TEXT
				// columns, but when used with clobs it ends up
				// putting a null terminator at position
				// "valuesize".  With SQL_C_BINARY, it doesn't.
				SQL_C_BINARY,
				SQL_LONGVARCHAR,
				valuesize,
				0,
				(SQLPOINTER)value,
				valuesize,
				&(lobbindsize[inbindcount]));
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return false;
	}
	return true;
}

bool informixcursor::outputBind(const char *variable, 
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
				(SQLLEN *)isnull);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return false;
	}
	return true;
}

bool informixcursor::outputBind(const char *variable,
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
				(SQLLEN *)isnull);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return false;
	}
	return true;
}

bool informixcursor::outputBind(const char *variable,
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
				(SQLLEN *)isnull);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return false;
	}
	return true;
}

bool informixcursor::outputBind(const char *variable,
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
				(SQLLEN *)isnull);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return false;
	}
	return true;
}

bool informixcursor::outputBindBlob(const char *variable, 
					uint16_t variablesize,
					uint16_t index,
					int16_t *isnull) {

	outlobbind[index]=new char[informixconn->maxoutbindlobsize];
	outlobbindlen[index]=SQL_NULL_DATA;

	erg=SQLBindParameter(stmt,
				charstring::toInteger(variable+1),
				SQL_PARAM_OUTPUT,
				SQL_C_BINARY,
				SQL_LONGVARBINARY,
				informixconn->maxoutbindlobsize,
				0,
				outlobbind[index],
				informixconn->maxoutbindlobsize,
				&outlobbindlen[index]);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return false;
	}
	return true;
}

bool informixcursor::outputBindClob(const char *variable, 
					uint16_t variablesize,
					uint16_t index,
					int16_t *isnull) {

	outlobbind[index]=new char[informixconn->maxoutbindlobsize];
	outlobbindlen[index]=SQL_NULL_DATA;

	erg=SQLBindParameter(stmt,
				charstring::toInteger(variable+1),
				SQL_PARAM_OUTPUT,
				SQL_C_CHAR,
				SQL_LONGVARCHAR,
				informixconn->maxoutbindlobsize,
				0,
				outlobbind[index],
				informixconn->maxoutbindlobsize,
				&outlobbindlen[index]);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return false;
	}
	return true;
}

bool informixcursor::getLobOutputBindLength(uint16_t index, uint64_t *length) {
	if (outlobbindlen[index]>informixconn->maxoutbindlobsize) {
		outlobbindlen[index]=informixconn->maxoutbindlobsize;
	}
	*length=outlobbindlen[index];
	return true;
}

bool informixcursor::getLobOutputBindSegment(uint16_t index,
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

bool informixcursor::executeQuery(const char *query, uint32_t length) {

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
	if (informixconn->maxselectlistsize==-1) {
		allocateResultSetBuffers(ncols);
	} else if (ncols>informixconn->maxselectlistsize) {
		ncols=informixconn->maxselectlistsize;
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
			// SQL_COLUMN_LENGTH isn't reliable in informix.  It
			// usually returns -1 or 0.  Just copy the result of
			// SQL_COLUMN_PRECISION below...

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
			column[i].length=column[i].precision;

			// column scale
			erg=SQLColAttribute(stmt,i+1,SQL_COLUMN_SCALE,
					NULL,0,NULL,&(column[i].scale));
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return false;
			}

			// column nullable
			// FIXME: informix doesn't appear to support this with
			// SQLColAttribute, try SQLColAttributes
			// or try FDNULLABLE/ISNULLABLE
			#if USE_NULLABLE == 1
			erg=SQLColAttribute(stmt,i+1,SQL_COLUMN_NULLABLE,
					NULL,0,NULL,&(column[i].nullable));
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return false;
			}
			#endif

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
		}

		if (column[i].type==SQL_LONGVARBINARY ||
			column[i].type==SQL_INFX_UDT_BLOB) {
			erg=SQLBindCol(stmt,i+1,SQL_C_BINARY,
					field[i],
					informixconn->maxitembuffersize,
					indicator[i]);
		} else {
			erg=SQLBindCol(stmt,i+1,SQL_C_CHAR,
					field[i],
					informixconn->maxitembuffersize,
					indicator[i]);
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

	// convert date output binds
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
	}
	
	return true;
}

void informixcursor::errorMessage(char *errorbuffer,
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
	// leave it to informix to have negative numbers for error codes...
	// the best we can do for now is turn it into a positive number
	*errorcode=-nativeerrnum;
	*liveconnection=informixconn->liveConnection(nativeerrnum,
							errorbuffer,errlength);
}

uint64_t informixcursor::affectedRows() {
	return affectedrows;
}

uint32_t informixcursor::colCount() {
	return ncols;
}

const char *informixcursor::getColumnName(uint32_t i) {
	return column[i].name;
}

uint16_t informixcursor::getColumnNameLength(uint32_t i) {
	return column[i].namelength;
}

uint16_t informixcursor::getColumnType(uint32_t i) {
	switch (column[i].type) {
		case SQL_CHAR:
			// SQL_CHAR is returned for char and nchar
			// FIXME: is there some way to distinguish them?
			return CHAR_DATATYPE;
		case SQL_NUMERIC:
			return NUMERIC_DATATYPE;
		case SQL_DECIMAL:
			// SQL_DECIMAL is returned for decimal and money
			// FIXME: is there some way to distinguish them?
			return DECIMAL_DATATYPE;
		case SQL_INTEGER:
			return INTEGER_DATATYPE;
		case SQL_SMALLINT:
			return SMALLINT_DATATYPE;
		case SQL_FLOAT:
			return FLOAT_DATATYPE;
		case SQL_REAL:
			// SQL_REAL is returned for smallfloat
			return SMALLFLOAT_DATATYPE;
		case SQL_DOUBLE:
			// SQL_DOUBLE is returned for float
			return FLOAT_DATATYPE;
		case SQL_DATETIME:
			// SQL_DATETIME is returned for date
			return DATE_DATATYPE;
		case SQL_VARCHAR:
			return VARCHAR_DATATYPE;
		case SQL_WCHAR:
		case SQL_WVARCHAR:
		case SQL_WLONGVARCHAR:
		case SQL_DECFLOAT:
			// I don't think informix actually supports these,
			// but they're defined in infxsql.h
			return UNKNOWN_DATATYPE;
		case SQL_TIME:
			// I don't think informix actually supports this,
			// but it's defined in sqlext.h
			return TIME_DATATYPE;
		case SQL_TIMESTAMP:
			// SQL_TIMESTAMP is returned for datetime
			return DATETIME_DATATYPE;
		case SQL_LONGVARCHAR:
			// SQL_LONGVARCHAR is returned for text
			return TEXT_DATATYPE;
		case SQL_BINARY:
			// I don't think informix actually supports this,
			// but it's defined in sqlext.h
			return BINARY_DATATYPE;
		case SQL_VARBINARY:
			// I don't think informix actually supports this,
			// but it's defined in sqlext.h
			return VARBINARY_DATATYPE;
		case SQL_LONGVARBINARY:
			// SQL_LONGVARBINARY is returned for byte
			return BYTE_DATATYPE;
		case SQL_BIGINT:
			// SQL_BIGINT is returned for int8's
			return INT8_DATATYPE;
		case SQL_TINYINT:
			// I don't think informix actually supports this,
			// but it's defined in sqlext.h
			return TINYINT_DATATYPE;
		case SQL_BIT:
			// SQL_BIT is returned for boolean
			return BOOLEAN_DATATYPE;
		case SQL_INFX_UDT_FIXED:
		case SQL_INFX_UDT_VARYING:
			// not sure what these are...
			return UNKNOWN_DATATYPE;
		case SQL_INFX_UDT_BLOB:
			return BLOB_DATATYPE;
		case SQL_INFX_UDT_CLOB:
			return CLOB_DATATYPE;
		case SQL_INFX_UDT_LVARCHAR:
		case SQL_INFX_RC_ROW:
		case SQL_INFX_RC_COLLECTION:
		case SQL_INFX_RC_LIST:
		case SQL_INFX_RC_SET:
		case SQL_INFX_RC_MULTISET:
		case SQL_INFX_UNSUPPORTED:
		case SQL_INFX_C_SMARTLOB_LOCATOR:
		case SQL_INFX_QUALIFIER:
			// not sure what these are...
			return UNKNOWN_DATATYPE;
		case SQL_INFX_DECIMAL:
			return DECIMAL_DATATYPE;
		case SQL_INFX_BIGINT:
			// SQL_INFX_BIGINT is returned for bigint's
			return BIGINT_DATATYPE;
		default:
			return UNKNOWN_DATATYPE;
	}
}

uint32_t informixcursor::getColumnLength(uint32_t i) {
	return column[i].length;
}

uint32_t informixcursor::getColumnPrecision(uint32_t i) {
	return column[i].precision;
}

uint32_t informixcursor::getColumnScale(uint32_t i) {
	return column[i].scale;
}

#if USE_NULLABLE == 1
uint16_t informixcursor::getColumnIsNullable(uint32_t i) {
	return column[i].nullable;
}
#endif

uint16_t informixcursor::getColumnIsUnsigned(uint32_t i) {
	return column[i].unsignednumber;
}

uint16_t informixcursor::getColumnIsBinary(uint32_t i) {
	uint16_t	type=getColumnType(i);
	return (type==BINARY_DATATYPE ||
		type==LONGVARBINARY_DATATYPE ||
		type==VARBINARY_DATATYPE ||
		type==GRAPHIC_DATATYPE ||
		type==VARGRAPHIC_DATATYPE ||
		type==LONGVARGRAPHIC_DATATYPE ||
		type==BLOB_DATATYPE);
}

uint16_t informixcursor::getColumnIsAutoIncrement(uint32_t i) {
	return column[i].autoincrement;
}

bool informixcursor::noRowsToReturn() {
	// if there are no columns, then there can't be any rows either
	return (ncols)?false:true;
}

bool informixcursor::skipRow() {
	if (fetchRow()) {
		rowgroupindex++;
		return true;
	}
	return false;
}

bool informixcursor::fetchRow() {

	if (rowgroupindex==informixconn->fetchatonce) {
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
		SQLGetStmtAttr(stmt,SQL_ATTR_ROW_NUMBER,
				(SQLPOINTER)&rownumber,0,NULL);

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

void informixcursor::getField(uint32_t col,
				const char **fld, uint64_t *fldlength,
				bool *blob, bool *null) {

	// handle NULLs
	if (indicator[col][rowgroupindex]==SQL_NULL_DATA) {
		*null=true;
		return;
	}

	// handle lobs
	if (column[col].type==SQL_INFX_UDT_CLOB ||
		column[col].type==SQL_INFX_UDT_BLOB) {
		*blob=true;
		return;
	}

	// handle normal datatypes
	*fld=&field[col][rowgroupindex*informixconn->maxitembuffersize];
	*fldlength=indicator[col][rowgroupindex];
}

void informixcursor::nextRow() {
	rowgroupindex++;
}

bool informixcursor::getLobFieldLength(uint32_t col, uint64_t *length) {

	// get the length of the lob

	// a valid buffer must be provided, but it's ok to fetch 0 bytes into it
	SQLCHAR	buffer[1];
	erg=SQLGetData(stmt,col+1,SQL_C_BINARY,buffer,0,
					&(loblength[col][rowgroupindex]));
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return false;
	}

	// copy out the length
	*length=loblength[col][rowgroupindex];

	return true;
}

bool informixcursor::getLobFieldSegment(uint32_t col,
					char *buffer, uint64_t buffersize,
					uint64_t offset, uint64_t charstoread,
					uint64_t *charsread) {

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
		erg=SQLGetData(stmt,col+1,SQL_C_BINARY,
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

void informixcursor::closeResultSet() {

	// informix doesn't like to close a null stmt
	if (stmt) {
		SQLCloseCursor(stmt);
	}

	for (uint16_t i=0; i<maxbindcount; i++) {
		delete outdatebind[i];
		outdatebind[i]=NULL;
		delete outlobbind[i];
		outlobbind[i]=NULL;
		outlobbindlen[i]=0;
	}

	if (informixconn->maxselectlistsize==-1) {
		deallocateResultSetBuffers();
	}
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrserverconnection *new_informixconnection(
						sqlrservercontroller *cont) {
		return new informixconnection(cont);
	}
}
