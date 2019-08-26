// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/environment.h>

#include <datatypes.h>
#include <defines.h>
#include <config.h>

#ifdef INFORMIX_AT_RUNTIME
	#include "informixatruntime.cpp"
#else
	#include <infxcli.h>
#endif

#define MAX_OUT_BIND_LOB_SIZE	2097152

#define MAX_LOB_CHUNK_SIZE	2147483647

struct informixcolumn {
	char		*name;
	SQLSMALLINT	namelength;
	SQLLEN		type;
	SQLLEN		precision;
	SQLLEN		scale;
	SQLLEN		flags;
	SQLLEN		primarykey;
	SQLLEN		unique;
	SQLLEN		partofkey;
	SQLLEN		unsignednumber;
	SQLLEN		zerofill;
	SQLLEN		binary;
	SQLLEN		autoincrement;
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

class informixconnection;

class SQLRSERVER_DLLSPEC informixcursor : public sqlrservercursor {
	friend class informixconnection;
	public:
			informixcursor(sqlrserverconnection *conn, uint16_t id);
			~informixcursor();
	private:
		void		allocateResultSetBuffers(int32_t columncount);
		void		deallocateResultSetBuffers();
		bool		open();
		bool		close();
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
						bool isnegative,
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
		SQLSMALLINT	ncols;
		SQLLEN 		affectedrows;

		int32_t		columncount;
		char		**field;
		SQLLEN		**loblength;
		SQLLEN		**indicator;
		informixcolumn	*column;

		uint16_t	maxbindcount;
		SQLLEN		*lobbindsize;
		datebind	**outdatebind;
		char		**outlobbind;
		SQLLEN 		*outlobbindlen;
		int16_t		**outisnullptr;
		SQLLEN		*outisnull;
		SQLLEN		sqlnulldata;
		BOOL		truevalue;

		uint64_t	rowgroupindex;
		uint64_t	totalinrowgroup;
		uint64_t	totalrows;
		uint64_t	rownumber;

		bool		noop;

		bool		bindformaterror;

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

		const char	*informixdir;
		const char	*servername;
		const char	*db;
		const char	*lang;
		stringbuffer	dsn;

		stringbuffer	errormessage;

		int32_t		maxoutbindlobsize;

		const char	*identity;

		char		dbversion[512];

		stringbuffer	errormsg;
};

informixconnection::informixconnection(sqlrservercontroller *cont) :
					sqlrserverconnection(cont) {

	maxoutbindlobsize=MAX_OUT_BIND_LOB_SIZE;
	identity=NULL;
}

void informixconnection::handleConnectString() {

	sqlrserverconnection::handleConnectString();

	// get informix dir
	informixdir=cont->getConnectStringValue("informixdir");

	// get dsn components
	servername=cont->getConnectStringValue("servername");
	if (charstring::isNullOrEmpty(servername)) {
		servername=environment::getValue("INFORMIXSERVER");
	}
	db=cont->getConnectStringValue("db");

	// build dsn
	dsn.clear();
	if (!charstring::isNullOrEmpty(servername)) {
		dsn.append("Servername=")->append(servername);
	}
	if (!charstring::isNullOrEmpty(db)) {
		if (dsn.getStringLength()) {
			dsn.append(";");
		}
		dsn.append("Database=")->append(db);
	}
	const char	*user=cont->getUser();
	if (!charstring::isNullOrEmpty(user)) {
		if (dsn.getStringLength()) {
			dsn.append(";");
		}
		dsn.append("LogonID=")->append(user);
	}
	const char	*pass=cont->getPassword();
	if (!charstring::isNullOrEmpty(pass)) {
		if (dsn.getStringLength()) {
			dsn.append(";");
		}
		dsn.append("pwd=")->append(pass);
	}

	// get other parameters
	lang=cont->getConnectStringValue("lang");

	// multi-row fetch doesn't work with clobs/blobs because you're already
	// on a different row when SQLGetData is called to get the data for the
	// clob/blob on the first row, so override it to 1
	cont->setFetchAtOnce(1);

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

	#ifdef INFORMIX_AT_RUNTIME
	if (!loadLibraries(&errormessage)) {
		*error=errormessage.getString();
		return NULL;
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

bool informixconnection::supportsAutoCommit() {
	return true;
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
	return "select dbinfo('dbhostname') from sysmaster:sysdual";
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

const char *informixconnection::getTableListQuery(bool wild,
						uint16_t objecttypes) {
	return (wild)?
		"select distinct "
		"	dbname as table_cat, "
		"	owner as table_schem, "
		"	tabname as table_name, "
		"	'TABLE' as table_type, "
		"	'' as remarks, "
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
		"	dbname, "
		"	owner, "
		"	tabname":

		"select distinct "
		"	dbname as table_cat, "
		"	owner as table_schem, "
		"	tabname as table_name, "
		"	'TABLE' as table_type, "
		"	'' as remarks, "
		"	'' as extra "
		"from "
		"	systables "
		"where "
		"	tabid > 99 "
		"	and "
		"	tabtype in ('T','S','P','V') "
		"order by "
		"	dbname, "
		"	owner, "
		"	tabname";
}

const char *informixconnection::getColumnListQuery(
					const char *table, bool wild) {

	// informix has the most ridiculous column info...
	// * if coltype > 256 then nulls are not allowed
	// * coltype mod 256 is the actual column type,
	// 	but it's a number that has to be decoded
	// * for decimal and money:
	//  * collength/256 is the precision
	//  * collength mod 256 is the scale
	// * the length of datetimes can vary widely depending on the interval
	//   but 8 is the max (I think)
	// * text and byte types can store 2^31 bytes but
	//   collength is given as 56
	// * clob and blob types can store any number of bytes but
	//   collength is given as 72
	// * boolean, clob and blob columns are all given as type 41, but
	//   blobs have an extended_id of 10 and clobs 11
	// * the default value for integer, float and date/time types has some
	//   qualifiers prepended to it

#define COLTYPE \
		"	decode(mod(coltype,256), " \
		"		41, " \
		"		decode(extended_id, " \
		"			10,'clob', " \
		"			11,'blob', " \
		"			'boolean'), " \
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

#define COLLENGTH \
		"	decode(mod(coltype,256), " \
		"		5,floor(collength/256), " \
		"		8,floor(collength/256), " \
		"		10,8, " \
		"		12,2147483648, "\
		"		11,2147483648, "\
		"		collength) as length, "

#define COLPREC \
		"	decode(mod(coltype,256), " \
		"		5,floor(collength/256), " \
		"		8,floor(collength/256), " \
		"		10,8, " \
		"		12,2147483648, "\
		"		11,2147483648, "\
		"		collength) as precision, "

#define COLSCALE \
		"	decode(mod(coltype,256), " \
		"		5,mod(collength,256), " \
		"		8,mod(collength,256), " \
		"		0) as scale, "

#define COLNULLS \
		"	case when (coltype<256) then " \
		"'YES' else 'NO' end as nulls, "

#define COLDEFAULT \
		"	decode(mod(coltype,256), " \
		"		41,sysdefaults.default, " \
		"		0,sysdefaults.default, " \
		"		15,sysdefaults.default, " \
		"		13,sysdefaults.default, " \
		"		16,sysdefaults.default, " \
		"		40,sysdefaults.default, " \
		"		substr(sysdefaults.default," \
		"			charindex(' '," \
		"			sysdefaults.default)+1)) as default, "

	// FIXME: primary key can be gotten from:
	// sysconstriants.constrtype  (which should be 'P')
	// where
	//     sysindexes.tabid = syscolumns.tabid
	//     and
	//     sysindexes.part1 = syscolumns.colno
	//     and
	//     sysconstraints.idxname=sysindexes.idxname
	// other constrtype values: R (reference/foreign) and U (unique)
	// but it's not clear how to get them...

	return (wild)?
		"select "
		"	colname, "
		COLTYPE
		COLLENGTH
		COLPREC
		COLSCALE
		COLNULLS
		"	'' as key, "
		COLDEFAULT
		"	'' as extra, "
		"	'' as extra2 "
		"from "
		"	systables, "
		"	syscolumns "
		"	left outer join sysdefaults on "
		"		sysdefaults.tabid=syscolumns.tabid "
		"		and "
		"		sysdefaults.colno=syscolumns.colno "
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
		COLLENGTH
		COLPREC
		COLSCALE
		COLNULLS
		"	'' as key, "
		COLDEFAULT
		"	'' as extra, "
		"	'' as extra2 "
		"from "
		"	systables, "
		"	syscolumns "
		"	left outer join sysdefaults on "
		"		sysdefaults.tabid=syscolumns.tabid "
		"		and "
		"		sysdefaults.colno=syscolumns.colno "
		"where "
		"	upper(systables.tabname)=upper('%s') "
		"	and "
		"	syscolumns.tabid=systables.tabid "
		"order by "
		"	syscolumns.colno";
}

const char *informixconnection::bindFormat() {
	return "?";
}

const char *informixconnection::selectDatabaseQuery() {
	return "database %s";
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

const char *informixconnection::noopQuery() {
        return "noop";
}

informixcursor::informixcursor(sqlrserverconnection *conn, uint16_t id) :
						sqlrservercursor(conn,id) {
	informixconn=(informixconnection *)conn;
	stmt=0;
	maxbindcount=conn->cont->getConfig()->getMaxBindCount();
	lobbindsize=new SQLLEN[maxbindcount];
	outdatebind=new datebind *[maxbindcount];
	outlobbind=new char *[maxbindcount];
	outlobbindlen=new SQLLEN[maxbindcount];
	outisnullptr=new int16_t *[maxbindcount];
	outisnull=new SQLLEN[maxbindcount];
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
	truevalue=SQL_TRUE;
}

informixcursor::~informixcursor() {
	delete[] lobbindsize;
	delete[] outdatebind;
	delete[] outlobbind;
	delete[] outlobbindlen;
	delete[] outisnullptr;
	delete[] outisnull;
	deallocateResultSetBuffers();
}

void informixcursor::allocateResultSetBuffers(int32_t columncount) {

	if (!columncount) {
		this->columncount=0;
		field=NULL;
		loblength=NULL;
		indicator=NULL;
		column=NULL;
	} else {
		this->columncount=columncount;
		field=new char *[columncount];
		loblength=new SQLLEN *[columncount];
		indicator=new SQLLEN *[columncount];
		column=new informixcolumn[columncount];
		uint32_t	fetchatonce=conn->cont->getFetchAtOnce();
		int32_t		maxfieldlength=conn->cont->getMaxFieldLength();
		for (int32_t i=0; i<columncount; i++) {
			column[i].name=new char[4096];
			field[i]=new char[fetchatonce*maxfieldlength];
			loblength[i]=new SQLLEN[fetchatonce];
			indicator[i]=new SQLLEN[fetchatonce];
		}
	}
}

void informixcursor::deallocateResultSetBuffers() {
	if (columncount) {
		for (int32_t i=0; i<columncount; i++) {
			delete[] column[i].name;
			delete[] field[i];
			delete[] loblength[i];
			delete[] indicator[i];
		}
		delete[] column;
		delete[] field;
		delete[] loblength;
		delete[] indicator;
		columncount=0;
	}
}

bool informixcursor::open() {

	if (!stmt) {

		// allocate the cursor
		erg=SQLAllocHandle(SQL_HANDLE_STMT,informixconn->dbc,&stmt);
		if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
			return false;
		}

		// set the row array size
		erg=SQLSetStmtAttr(stmt,SQL_ATTR_ROW_ARRAY_SIZE,
				(SQLPOINTER)conn->cont->getFetchAtOnce(),0);
		if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
			return false;
		}

		// enable smart-large-object automation for non-selects
		erg=SQLSetStmtAttr(stmt,SQL_INFX_ATTR_LO_AUTOMATIC,
						(SQLPOINTER)truevalue,0);
		if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
			return false;
		}
	}
	return true;
}

bool informixcursor::close() {

	if (stmt) {
		SQLFreeHandle(SQL_HANDLE_STMT,stmt);
		stmt=0;
	}
	return true;
}

bool informixcursor::prepareQuery(const char *query, uint32_t length) {

	bindformaterror=false;

	// FIXME: we shouldn't have to do this, but the tests crash in
	// multiple locations if we don't...
	if (!close() || !open()) {
		return false;
	}

	// initialize column count
	ncols=0;

	// handle noops
	noop=!charstring::compare(query,"noop");
	if (noop) {
		return true;
	}

	// prepare the query
	erg=SQLPrepare(stmt,(SQLCHAR *)query,length);
	return (erg==SQL_SUCCESS || erg==SQL_SUCCESS_WITH_INFO);
}

bool informixcursor::inputBind(const char *variable,
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
				// the parameter below must be set
				// for informix, unlike db2 and odbc
				valuesize,
				0,
				(SQLPOINTER)value,
				valuesize,
				NULL);
	}
	return (erg==SQL_SUCCESS || erg==SQL_SUCCESS_WITH_INFO);
}

bool informixcursor::inputBind(const char *variable,
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

bool informixcursor::inputBind(const char *variable,
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

bool informixcursor::inputBindBlob(const char *variable,
					uint16_t variablesize,
					const char *value,
					uint32_t valuesize,
					int16_t *isnull) {

	uint16_t	pos=charstring::toInteger(variable+1);
	if (!pos || pos>maxbindcount) {
		bindformaterror=true;
		return false;
	}

	lobbindsize[pos-1]=valuesize;
	erg=SQLBindParameter(stmt,
				pos,
				SQL_PARAM_INPUT,
				SQL_C_BINARY,
				SQL_LONGVARBINARY,
				valuesize,
				0,
				(SQLPOINTER)value,
				valuesize,
				&(lobbindsize[pos-1]));
	return (erg==SQL_SUCCESS || erg==SQL_SUCCESS_WITH_INFO);
}

bool informixcursor::inputBindClob(const char *variable,
					uint16_t variablesize,
					const char *value,
					uint32_t valuesize,
					int16_t *isnull) {

	uint16_t	pos=charstring::toInteger(variable+1);
	if (!pos || pos>maxbindcount) {
		bindformaterror=true;
		return false;
	}

	lobbindsize[pos-1]=valuesize;
	erg=SQLBindParameter(stmt,
				pos,
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
				&(lobbindsize[pos-1]));
	return (erg==SQL_SUCCESS || erg==SQL_SUCCESS_WITH_INFO);
}

bool informixcursor::outputBind(const char *variable, 
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

bool informixcursor::outputBind(const char *variable,
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

bool informixcursor::outputBind(const char *variable,
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

bool informixcursor::outputBindBlob(const char *variable, 
					uint16_t variablesize,
					uint16_t index,
					int16_t *isnull) {

	uint16_t	pos=charstring::toInteger(variable+1);
	if (!pos || pos>maxbindcount) {
		bindformaterror=true;
		return false;
	}

	outlobbind[index]=new char[informixconn->maxoutbindlobsize];
	outlobbindlen[index]=SQL_NULL_DATA;

	erg=SQLBindParameter(stmt,
				pos,
				SQL_PARAM_OUTPUT,
				SQL_C_BINARY,
				SQL_LONGVARBINARY,
				informixconn->maxoutbindlobsize,
				0,
				outlobbind[index],
				informixconn->maxoutbindlobsize,
				&outlobbindlen[index]);
	return (erg==SQL_SUCCESS || erg==SQL_SUCCESS_WITH_INFO);
}

bool informixcursor::outputBindClob(const char *variable, 
					uint16_t variablesize,
					uint16_t index,
					int16_t *isnull) {

	uint16_t	pos=charstring::toInteger(variable+1);
	if (!pos || pos>maxbindcount) {
		bindformaterror=true;
		return false;
	}

	outlobbind[index]=new char[informixconn->maxoutbindlobsize];
	outlobbindlen[index]=SQL_NULL_DATA;

	erg=SQLBindParameter(stmt,
				pos,
				SQL_PARAM_OUTPUT,
				SQL_C_CHAR,
				SQL_LONGVARCHAR,
				informixconn->maxoutbindlobsize,
				0,
				outlobbind[index],
				informixconn->maxoutbindlobsize,
				&outlobbindlen[index]);
	return (erg==SQL_SUCCESS || erg==SQL_SUCCESS_WITH_INFO);
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

	// initialize row counts
	rowgroupindex=0;
	totalinrowgroup=0;
	totalrows=0;

	// handle noops
	if (noop) {
		return true;
	}

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
	int32_t	maxcolumncount=conn->cont->getMaxColumnCount();
	if (!maxcolumncount) {
		allocateResultSetBuffers(ncols);
	} else if (ncols>maxcolumncount) {
		ncols=maxcolumncount;
	}

	// run through the columns
	for (SQLSMALLINT i=0; i<ncols; i++) {

		if (conn->cont->getSendColumnInfo()==SEND_COLUMN_INFO) {

			// column name
			erg=SQLColAttribute(stmt,i+1,SQL_COLUMN_LABEL,
					column[i].name,4096,
					&(column[i].namelength),
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

			// informix doesn't support column length,
			// so we'll just use the precision

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
			// Informix doesn't support SQL_COLUMN_NULLABLE.
			// Nullability is just part of the "flags".
			erg=SQLColAttribute(stmt,i+1,SQL_INFX_ATTR_FLAGS,
					NULL,0,NULL,&(column[i].flags));
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

		if (column[i].type==SQL_LONGVARBINARY ||
			column[i].type==SQL_INFX_UDT_BLOB) {
			erg=SQLBindCol(stmt,i+1,SQL_C_BINARY,
					field[i],
					conn->cont->getMaxFieldLength(),
					indicator[i]);
		} else {
			erg=SQLBindCol(stmt,i+1,SQL_C_CHAR,
					field[i],
					conn->cont->getMaxFieldLength(),
					indicator[i]);
		}
		if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
			return false;
		}
	}

	// get the row count
	erg=SQLRowCount(stmt,&affectedrows);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		// This might fail for queries like "database xxx", so we'll
		// tolarate failure and just set affectedrows to 0.  This
		// seems to be ok.
		affectedrows=0;
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

void informixcursor::errorMessage(char *errorbuffer,
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
		#ifdef SQL_DECFLOAT
		case SQL_DECFLOAT:
		#endif
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
	// informix doesn't support column length,
	// so we'll just use the precision
	return column[i].precision;
}

uint32_t informixcursor::getColumnPrecision(uint32_t i) {
	return column[i].precision;
}

uint32_t informixcursor::getColumnScale(uint32_t i) {
	return column[i].scale;
}

uint16_t informixcursor::getColumnIsNullable(uint32_t i) {
	return ISNULLABLE(column[i].flags);
}

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

const char *informixcursor::getColumnTable(uint32_t i) {
	return column[i].table;
}

uint16_t informixcursor::getColumnTableLength(uint32_t i) {
	return column[i].tablelength;
}

bool informixcursor::noRowsToReturn() {
	// if there are no columns, then there can't be any rows either
	return (ncols)?false:true;
}

bool informixcursor::skipRow(bool *error) {
	if (fetchRow(error)) {
		rowgroupindex++;
		return true;
	}
	return false;
}

bool informixcursor::fetchRow(bool *error) {

	*error=false;

	if (noop) {
		return false;
	}

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
	*fld=&field[col][rowgroupindex*conn->cont->getMaxFieldLength()];
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
	SQLRSERVER_DLLSPEC sqlrserverconnection *new_informixconnection(
						sqlrservercontroller *cont) {
		return new informixconnection(cont);
	}
}
