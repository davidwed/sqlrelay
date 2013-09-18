// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrcontroller.h>
#include <sqlrconnection.h>
#include <rudiments/environment.h>

#include <datatypes.h>
#include <config.h>

#include <sqlcli1.h>

#define FETCH_AT_ONCE		10
#define MAX_SELECT_LIST_SIZE	256
#define MAX_ITEM_BUFFER_SIZE	4096

struct db2column {
	char		name[MAX_ITEM_BUFFER_SIZE];
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
		char		field[MAX_SELECT_LIST_SIZE]
					[FETCH_AT_ONCE]
					[MAX_ITEM_BUFFER_SIZE];
		SQLINTEGER	indicator[MAX_SELECT_LIST_SIZE]
						[FETCH_AT_ONCE];
#if (DB2VERSION>7)
		SQLUSMALLINT	rowstat[FETCH_AT_ONCE];
#endif
		db2column	col[MAX_SELECT_LIST_SIZE];

		datebind	**outdatebind;

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
					SQLSMALLINT errnum);
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

		char		dbversion[512];

		stringbuffer	errormsg;
};

db2connection::db2connection(sqlrcontroller_svr *cont) :
					sqlrconnection_svr(cont) {
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
}

bool db2connection::logIn(const char **error) {

	// set the LANG environment variable
	if (charstring::length(lang)) {
		if (!environment::setValue("LANG",lang)) {
			*error="Failed to set LANG environment variable.";
			return false;
		}
	}

	// allocate environment handle
	erg=SQLAllocHandle(SQL_HANDLE_ENV,SQL_NULL_HANDLE,&env);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		SQLFreeHandle(SQL_HANDLE_ENV,env);
		return false;
	}

	// allocate connection handle
	erg=SQLAllocHandle(SQL_HANDLE_DBC,env,&dbc);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		SQLFreeHandle(SQL_HANDLE_DBC,dbc);
		SQLFreeHandle(SQL_HANDLE_ENV,env);
		return false;
	}

	// set the connect timeout
	SQLSetConnectAttr(dbc,SQL_LOGIN_TIMEOUT,(SQLPOINTER *)5,0);

	// connect to the database
	erg=SQLConnect(dbc,(SQLCHAR *)server,SQL_NTS,
				(SQLCHAR *)cont->getUser(),SQL_NTS,
				(SQLCHAR *)cont->getPassword(),SQL_NTS);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		SQLFreeHandle(SQL_HANDLE_DBC,dbc);
		SQLFreeHandle(SQL_HANDLE_ENV,env);
		return false;
	}
	return true;
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
	// here's another one
	//	[IBM][CLI Driver] SQL1224N  The database manager is not able to
	//	 accept new requests, has terminated all requests in progress,
	//	or has terminated your particular request due to a problem with
	//	your request.  SQLSTATE=55032
	// We need to catch both...
	return !((nativeerrnum==-1224 && errlength==184) ||
		(nativeerrnum==-99999 && errlength==64) ||
		(nativeerrnum==-1224 && errlength==220));
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
	for (uint16_t i=0; i<conn->cont->maxbindcount; i++) {
		outdatebind[i]=NULL;
	}
}

db2cursor::~db2cursor() {
	delete[] outdatebind;
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
				(SQLPOINTER)FETCH_AT_ONCE,0);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return false;
	}

#if (DB2VERSION>7)
	// set the row status ptr
	erg=SQLSetStmtAttr(stmt,SQL_ATTR_ROW_STATUS_PTR,
				(SQLPOINTER)rowstat,0);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return false;
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
		erg=SQLBindParameter(stmt,
				charstring::toInteger(variable+1),
				SQL_PARAM_INPUT,
				SQL_C_CHAR,
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
	if (ncols>MAX_SELECT_LIST_SIZE) {
		ncols=MAX_SELECT_LIST_SIZE;
	}

	// run through the columns
	for (SQLSMALLINT i=0; i<ncols; i++) {

		if (conn->cont->sendColumnInfo()) {

			// column name
			erg=SQLColAttribute(stmt,i+1,SQL_COLUMN_LABEL,
					col[i].name,MAX_ITEM_BUFFER_SIZE,
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
				field[i],MAX_ITEM_BUFFER_SIZE,
				(SQLINTEGER *)&indicator[i]);
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

	if (rowgroupindex==FETCH_AT_ONCE) {
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
		// the row status buffer appears to work.
		int32_t	index=0;
		while (index<FETCH_AT_ONCE) {
			index++;
		}
		index=0;
		while (index<FETCH_AT_ONCE &&
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
	*fld=field[col][rowgroupindex];
	*fldlength=indicator[col][rowgroupindex];
}

void db2cursor::nextRow() {
	rowgroupindex++;
}

void db2cursor::cleanUpData() {
	for (uint16_t i=0; i<conn->cont->maxbindcount; i++) {
		delete outdatebind[i];
		outdatebind[i]=NULL;
	}
}

extern "C" {
	sqlrconnection_svr *new_db2connection(sqlrcontroller_svr *cont) {
		return new db2connection(cont);
	}
}
