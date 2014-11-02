// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrcontroller.h>
#include <sqlrelay/sqlrconnection.h>
#include <rudiments/bytestring.h>
#ifndef HAVE_POSTGRESQL_PQSETNOTICEPROCESSOR
	#include <rudiments/file.h>
#endif
#include <rudiments/sys.h>

#include <datatypes.h>

#include <libpq-fe.h>

class postgresqlconnection : public sqlrconnection_svr {
	friend class postgresqlcursor;
	public:
			postgresqlconnection(sqlrcontroller_svr *cont);
			~postgresqlconnection();
	private:
		void		handleConnectString();
		bool		logIn(const char **error);
		const char	*logInError(const char *errmsg);
		sqlrcursor_svr	*newCursor();
		void		deleteCursor(sqlrcursor_svr *curs);
		void		logOut();
		void		errorMessage(char *errorbuffer,
						uint32_t errorbufferlength,
						uint32_t *errorlength,
						int64_t	*errorcode,
						bool *liveconnection);
		const char	*identify();
		const char	*dbVersion();
		const char	*dbHostName();
		const char	*dbIpAddressQuery();
		const char	*dbIpAddress();
		const char	*getDatabaseListQuery(bool wild);
		const char	*getTableListQuery(bool wild);
		const char	*getColumnListQuery(const char *table,
								bool wild);
		const char	*selectDatabaseQuery();
		const char	*getCurrentDatabaseQuery();
		bool		getLastInsertId(uint64_t *id);
		const char	*getLastInsertIdQuery();
		const char	*bindFormat();

		int	datatypecount;
		int32_t	*datatypeids;
		char	**datatypenames;

		PGconn	*pgconn;

		const char	*host;
		const char	*port;
		const char	*options;
		const char	*db;
		uint16_t	typemangling;
		const char	*charset;
		char		*dbversion;
		char		*hostname;

		stringbuffer	errormessage;

#ifdef HAVE_POSTGRESQL_PQOIDVALUE
		Oid	currentoid;
#endif
		char	*lastinsertidquery;

#ifndef HAVE_POSTGRESQL_PQSETNOTICEPROCESSOR
	private:
		file	devnull;
#endif
};

class postgresqlcursor : public sqlrcursor_svr {
	friend class postgresqlconnection;
	private:
				postgresqlcursor(sqlrconnection_svr *conn);
				~postgresqlcursor();
#if defined(HAVE_POSTGRESQL_PQEXECPREPARED) && \
		defined(HAVE_POSTGRESQL_PQPREPARE)
		bool		open(uint16_t id);
		bool		prepareQuery(const char *query,
						uint32_t length);
		bool		deallocateStatement();
#endif
		bool		supportsNativeBinds();
#if defined(HAVE_POSTGRESQL_PQEXECPREPARED) && \
		defined(HAVE_POSTGRESQL_PQPREPARE)
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
#endif
		bool		executeQuery(const char *query,
						uint32_t length);
		bool		knowsRowCount();
		uint64_t	rowCount();
		uint64_t	affectedRows();
		uint32_t	colCount();
		uint16_t	columnTypeFormat();
		const char	*getColumnName(uint32_t col);
		uint16_t	getColumnType(uint32_t col);
		const char	*getColumnTypeName(uint32_t col);
		uint32_t	getColumnLength(uint32_t col);
		uint16_t	getColumnIsBinary(uint32_t col);
		bool		noRowsToReturn();
		bool		fetchRow();
		void		getField(uint32_t col,
					const char **field,
					uint64_t *fieldlength,
					bool *blob,
					bool *null);
		void		closeResultSet();

		PGresult	*pgresult;
		ExecStatusType	pgstatus;
		int		ncols;
		int		nrows;
		uint64_t	affectedrows;
		int		currentrow;

		char		typenamebuffer[6];

		postgresqlconnection	*postgresqlconn;

#if defined(HAVE_POSTGRESQL_PQEXECPREPARED) && \
		defined(HAVE_POSTGRESQL_PQPREPARE)
		bool		deallocatestatement;
		int		bindcount;
		int		bindcounter;
		char		**bindvalues;
		int		*bindlengths;
		int		*bindformats;
		char		*cursorname;
#endif
};

#ifdef HAVE_POSTGRESQL_PQSETNOTICEPROCESSOR
static void nullNoticeProcessor(void *arg, const char *message) {
}
#endif

postgresqlconnection::postgresqlconnection(sqlrcontroller_svr *cont) :
						sqlrconnection_svr(cont) {
	dbversion=NULL;
	datatypecount=0;
	datatypeids=NULL;
	datatypenames=NULL;
	pgconn=(PGconn *)NULL;
#ifdef HAVE_POSTGRESQL_PQOIDVALUE
	currentoid=InvalidOid;
#endif
	lastinsertidquery=NULL;
	hostname=NULL;
}

postgresqlconnection::~postgresqlconnection() {
#ifndef HAVE_POSTGRESQL_PQSETNOTICEPROCESSOR
	devnull.close();
#endif
	delete[] dbversion;
	delete[] lastinsertidquery;
	delete[] hostname;
}

void postgresqlconnection::handleConnectString() {
	host=cont->connectStringValue("host");
	port=cont->connectStringValue("port");
	options=cont->connectStringValue("options");
	db=cont->connectStringValue("db");
	cont->setUser(cont->connectStringValue("user"));
	cont->setPassword(cont->connectStringValue("password"));
	const char	*typemang=cont->connectStringValue("typemangling");
	if (!typemang ||!charstring::compareIgnoringCase(typemang,"no")) {
		typemangling=0;
	} else if (!charstring::compareIgnoringCase(typemang,"yes")) {
		typemangling=1;
	} else {
		typemangling=2;
	}
	charset=cont->connectStringValue("charset");
	const char	*lastinsertidfunc=
			cont->connectStringValue("lastinsertidfunction");
	if (lastinsertidfunc) {
		stringbuffer	liiquery;
		liiquery.append("select ");
		liiquery.append(lastinsertidfunc);
		lastinsertidquery=liiquery.detachString();
	}
	cont->setFakeInputBinds(
		!charstring::compare(
			cont->connectStringValue("fakebinds"),"yes"));
}

bool postgresqlconnection::logIn(const char **error) {

	// initialize the datatype storage buffers
	if (typemangling==2) {
		datatypecount=0;
		datatypeids=NULL;
		datatypenames=NULL;
	}

	// log in
	pgconn=PQsetdbLogin(host,port,options,NULL,db,
				cont->getUser(),cont->getPassword());

	// check the status of the login
	if (PQstatus(pgconn)==CONNECTION_BAD) {
		*error=logInError("Log in failed");
		logOut();
		return false;
	}

#ifdef HAVE_POSTGRESQL_PQSETNOTICEPROCESSOR
	// make sure that no messages get sent to the console
	PQsetNoticeProcessor(pgconn,nullNoticeProcessor,NULL);
#else
	if (devnull.open("/dev/null",O_RDONLY)) {
		devnull.duplicate(1);
		devnull.duplicate(2);
	}
#endif

#if defined(HAVE_POSTGRESQL_PQSETCLIENTENCODING)
	if (charstring::length(charset)) {
		PQsetClientEncoding(pgconn,charset);
	}
#endif

	// get the datatypes
	if (typemangling==2) {
		PGresult	*result=PQexec(pgconn,
					"select oid,typname from pg_type");
		if (result==(PGresult *)NULL) {
			*error=logInError("Get datatypes failed");
			return false;
		}

		// create the datatype storage buffers
		datatypecount=PQntuples(result);
		datatypeids=new int32_t[datatypecount];
		datatypenames=new char *[datatypecount];

		// copy the datatype ids/names into the buffers
		for (int i=0; i<datatypecount; i++) {
			datatypeids[i]=
				charstring::toInteger(PQgetvalue(result,i,0));
			datatypenames[i]=
				charstring::duplicate(PQgetvalue(result,i,1));
		}
	
		// clean up
		PQclear(result);
	}

#if defined(HAVE_POSTGRESQL_PQEXECPREPARED) && \
		defined(HAVE_POSTGRESQL_PQPREPARE)
	// don't use bind variables against older servers
	if (PQprotocolVersion(pgconn)<3) {
		cont->setFakeInputBinds(true);
	}
#endif

	return true;
}

const char *postgresqlconnection::logInError(const char *errmsg) {

	errormessage.clear();
	errormessage.append(errmsg)->append(": ");

	// get the error message from postgresql
	const char	*message=PQerrorMessage(pgconn);
	errormessage.append(message);
	return errormessage.getString();
}

sqlrcursor_svr *postgresqlconnection::newCursor() {
	return (sqlrcursor_svr *)new
			postgresqlcursor((sqlrconnection_svr *)this);
}

void postgresqlconnection::deleteCursor(sqlrcursor_svr *curs) {
	delete (postgresqlcursor *)curs;
}

void postgresqlconnection::logOut() {

#ifndef HAVE_POSTGRESQL_PQSETNOTICEPROCESSOR
	devnull.close();
#endif

	if (pgconn) {
		PQfinish(pgconn);
		pgconn=NULL;
	}

	if (typemangling==2) {

		// delete the datatype storage buffers
		for (int i=0; i<datatypecount; i++) {
			delete[] datatypenames[i];
		}
		delete[] datatypeids;
		delete[] datatypenames;

		// re-initialize the datatype storage buffers
		datatypecount=0;
		datatypeids=NULL;
		datatypenames=NULL;
	}
}

void postgresqlconnection::errorMessage(char *errorbuffer,
					uint32_t errorbufferlength,
					uint32_t *errorlength,
					int64_t *errorcode,
					bool *liveconnection) {
	const char	*errorstring=PQerrorMessage(pgconn);
	*errorlength=charstring::length(errorstring);
	charstring::safeCopy(errorbuffer,errorbufferlength,
					errorstring,*errorlength);
	// FIXME: set this
	*errorcode=0;
	*liveconnection=(PQstatus(pgconn)==CONNECTION_OK);
}

const char *postgresqlconnection::identify() {
	return "postgresql";
}

const char *postgresqlconnection::dbVersion() {
	delete[] dbversion;
#if defined(HAVE_POSTGRESQL_PQSERVERVERSION)
	dbversion=charstring::parseNumber((uint64_t)PQserverVersion(pgconn));
#else
#if defined(HAVE_POSTGRESQL_PQPARAMETERSTATUS)
	dbversion=charstring::duplicate(PQparameterStatus(pgconn,
							"server_version"));
#else
	PGresult	*result=PQexec(pgconn,"select version()");
	if (result==(PGresult *)NULL) {
		return NULL;
	}

	const char	*versionstring=PQgetvalue(result,0,0);
	char		**list;
	uint64_t	listlength;
	charstring::split(versionstring," ",true,&list,&listlength);
	if (listlength>=2) {
		dbversion=list[1];
		list[1]=NULL;
	}
	for (uint64_t i=0; i<listlength; i++) {
		delete[] list[i];
	}
	delete[] list;

	PQclear(result);
#endif
	char		**parts;
	uint64_t	partslength;
	charstring::split(dbversion,".",true,&parts,&partslength);
	if (partslength==3) {
		int64_t	minor=charstring::toInteger(parts[1]);
		int64_t	patch=charstring::toInteger(parts[2]);
		charstring::printf(dbversion,
					charstring::length(dbversion)+1,
					"%s%02lld%02lld",
					parts[0],
					(long long)minor,(long long)patch);
	}
	for (uint64_t i=0; i<partslength; i++) {
		delete[] parts[i];
	}
	delete[] parts;
#endif
	return dbversion;
}

const char *postgresqlconnection::dbHostName() {
	const char	*dbhostname=sqlrconnection_svr::dbHostName();
	if (charstring::length(dbhostname)) {
		return dbhostname;
	}
	if (!hostname) {
		hostname=sys::getHostName();
	}
	return hostname;
}

const char *postgresqlconnection::dbIpAddressQuery() {
	return "select inet_server_addr()";
}

const char *postgresqlconnection::dbIpAddress() {
	const char	*ipaddress=sqlrconnection_svr::dbIpAddress();
	return (charstring::length(ipaddress))?ipaddress:"127.0.0.1";
}

const char *postgresqlconnection::getDatabaseListQuery(bool wild) {
	return (wild)?
		"select "
		"	datname "
		"from "
		"	pg_database "
		"where "
		"	datname like '%s' "
		"order by "
		"	datname":

		"select "
		"	datname "
		"from "
		"	pg_database "
		"order by "
		"	datname";
}

const char *postgresqlconnection::getTableListQuery(bool wild) {
	return (wild)?
		"select "
		"	table_name "
		"from "
		"	information_schema.tables "
		"where "
		"	table_schema = 'public' "
		"	and "
		"	table_name like '%s' "
		"order by "
		"	table_name":

		"select "
		"	table_name "
		"from "
		"	information_schema.tables "
		"where "
		"	table_schema = 'public' "
		"order by "
		"	table_name";
}

const char *postgresqlconnection::getColumnListQuery(const char *table,
								bool wild) {
	return (wild)?
		"select "
		"	column_name, "
		"	data_type, "
		"	character_maximum_length, "
		"	numeric_precision, "
		"	numeric_scale, "
		"	is_nullable, "
		"	'' as key, "
		"	column_default, "
		"	'' as extra "
		"from "
		"	information_schema.columns "
		"where "
		"	table_name='%s' "
		"	and "
		"	column_name like '%s' "
		"order by "
		"	ordinal_position":

		"select "
		"	column_name, "
		"	data_type, "
		"	character_maximum_length, "
		"	numeric_precision, "
		"	numeric_scale, "
		"	is_nullable, "
		"	'' as key, "
		"	column_default, "
		"	'' as extra "
		"from "
		"	information_schema.columns "
		"where "
		"	table_name='%s' "
		"order by "
		"	ordinal_position";
}

const char *postgresqlconnection::selectDatabaseQuery() {
	return "use %s";
}

const char *postgresqlconnection::getCurrentDatabaseQuery() {
	return "select current_database()";
}

bool postgresqlconnection::getLastInsertId(uint64_t *id) {
#ifdef HAVE_POSTGRESQL_PQOIDVALUE
	if (lastinsertidquery) {
		return sqlrconnection_svr::getLastInsertId(id);
	}
	*id=(currentoid!=InvalidOid)?currentoid:0;
	return true;
#else
	return false;
#endif
}

const char *postgresqlconnection::getLastInsertIdQuery() {
	return lastinsertidquery;
}

const char *postgresqlconnection::bindFormat() {
#if defined(HAVE_POSTGRESQL_PQEXECPREPARED) && \
		defined(HAVE_POSTGRESQL_PQPREPARE)
	return "$1";
#else
	return sqlrconnection_svr::bindFormat();
#endif
}

postgresqlcursor::postgresqlcursor(sqlrconnection_svr *conn) :
						sqlrcursor_svr(conn) {
	postgresqlconn=(postgresqlconnection *)conn;
	pgresult=NULL;
#if defined(HAVE_POSTGRESQL_PQEXECPREPARED) && \
		defined(HAVE_POSTGRESQL_PQPREPARE)
	deallocatestatement=false;
	cursorname=NULL;
	bindcounter=0;
	bindcount=0;
	bindvalues=NULL;
	bindlengths=NULL;
	bindformats=NULL;
#endif
}

postgresqlcursor::~postgresqlcursor() {
#if defined(HAVE_POSTGRESQL_PQEXECPREPARED) && \
		defined(HAVE_POSTGRESQL_PQPREPARE)
	deallocateStatement();
	delete[] cursorname;

	for (uint16_t i=0; i<bindcounter; i++) {
		delete[] bindvalues[i];
	}
	delete[] bindvalues;
	delete[] bindlengths;
	delete[] bindformats;
#endif
}

#if defined(HAVE_POSTGRESQL_PQEXECPREPARED) && \
		defined(HAVE_POSTGRESQL_PQPREPARE)
bool postgresqlcursor::open(uint16_t id) {
	size_t	cursornamelen=6+charstring::integerLength(id)+1;
	cursorname=new char[cursornamelen];
	charstring::printf(cursorname,cursornamelen,"cursor%d",id);
	return true;
}

bool postgresqlcursor::deallocateStatement() {
	if (deallocatestatement) {
		stringbuffer	rmquery;
		rmquery.append("deallocate ")->append(cursorname);
		pgresult=PQexec(postgresqlconn->pgconn,rmquery.getString());
		if (pgresult==(PGresult *)NULL) {
			return false;
		}
		PQclear(pgresult);
		deallocatestatement=true;
	}
	return true;
}

bool postgresqlcursor::prepareQuery(const char *query, uint32_t length) {

	// store inbindcount here, otherwise if rebinding/reexecution occurs and
	// the client tries to bind more variables than were defined when the
	// query was prepared, it would cause the inputBind methods to attempt
	// to address beyond the end of the various arrays
	bindcount=inbindcount;

	// reset bind counter
	bindcounter=0;

	if (bindcount) {

		// clear bind arrays
		delete[] bindvalues;
		delete[] bindlengths;
		delete[] bindformats;

		// create new bind arrays
		bindvalues=new char *[bindcount];
		bindlengths=new int[bindcount];
		bindformats=new int[bindcount];

		// initialize bind arrays
		for (int i=0; i<bindcount; i++) {
			bindvalues[i]=NULL;
			bindlengths[i]=0;
			bindformats[i]=0;
		}
	}

	// remove this named statement, if it exists already
	if (!deallocateStatement()) {
		return false;
	}

	// prepare the query
	pgresult=PQprepare(postgresqlconn->pgconn,cursorname,query,0,NULL);

	// handle a failed query
	if (pgresult==(PGresult *)NULL) {
		return false;
	}

	// handle errors
	pgstatus=PQresultStatus(pgresult);
	if (pgstatus==PGRES_BAD_RESPONSE ||
		pgstatus==PGRES_NONFATAL_ERROR ||
		pgstatus==PGRES_FATAL_ERROR) {
		return false;
	}
	deallocatestatement=true;

	return true;
}

bool postgresqlcursor::inputBind(const char *variable, 
					uint16_t variablesize,
					const char *value, 
					uint32_t valuesize,
					int16_t *isnull) {

	int32_t	pos=charstring::toInteger(variable+1)-1;

	// ignore attempts to bind beyond the number of
	// variables defined when the query was prepared
	if (pos>=bindcount || bindcounter>=bindcount) {
		return true;
	}

	if (*isnull) {
		bindvalues[pos]=NULL;
		bindlengths[pos]=0;
	} else {
		bindvalues[pos]=charstring::duplicate(value,valuesize);
		bindlengths[pos]=valuesize;
	}
	bindformats[pos]=0;
	bindcounter++;
	return true;
}

bool postgresqlcursor::inputBind(const char *variable, 
					uint16_t variablesize,
					int64_t *value) {

	int32_t	pos=charstring::toInteger(variable+1)-1;

	// ignore attempts to bind beyond the number of
	// variables defined when the query was prepared
	if (pos>=bindcount || bindcounter>=bindcount) {
		return true;
	}

	bindvalues[pos]=charstring::parseNumber(*value);
	bindlengths[pos]=charstring::length(bindvalues[pos]);
	bindformats[pos]=0;
	bindcounter++;
	return true;
}

bool postgresqlcursor::inputBind(const char *variable, 
					uint16_t variablesize,
					double *value,
					uint32_t precision,
					uint32_t scale) {

	int32_t	pos=charstring::toInteger(variable+1)-1;

	// ignore attempts to bind beyond the number of
	// variables defined when the query was prepared
	if (pos>=bindcount || bindcounter>=bindcount) {
		return true;
	}

	bindvalues[pos]=charstring::parseNumber(*value,precision,scale);
	bindlengths[pos]=charstring::length(bindvalues[pos]);
	bindformats[pos]=0;
	bindcounter++;
	return true;
}

bool postgresqlcursor::inputBindBlob(const char *variable, 
					uint16_t variablesize,
					const char *value, 
					uint32_t valuesize,
					int16_t *isnull) {

	int32_t	pos=charstring::toInteger(variable+1)-1;

	// ignore attempts to bind beyond the number of
	// variables defined when the query was prepared
	if (pos>=bindcount || bindcounter>=bindcount) {
		return true;
	}

	if (*isnull) {
		bindvalues[pos]=NULL;
		bindlengths[pos]=0;
	} else {
		bindvalues[pos]=static_cast<char *>
				(bytestring::duplicate(value,valuesize));
		bindlengths[pos]=valuesize;
	}
	bindformats[pos]=1;
	bindcounter++;
	return true;
}

bool postgresqlcursor::inputBindClob(const char *variable, 
					uint16_t variablesize,
					const char *value, 
					uint32_t valuesize,
					int16_t *isnull) {

	int32_t	pos=charstring::toInteger(variable+1)-1;

	// ignore attempts to bind beyond the number of
	// variables defined when the query was prepared
	if (pos>=bindcount || bindcounter>=bindcount) {
		return true;
	}

	if (*isnull) {
		bindvalues[pos]=NULL;
		bindlengths[pos]=0;
	} else {
		bindvalues[pos]=charstring::duplicate(value,valuesize);
		bindlengths[pos]=valuesize;
	}
	bindformats[pos]=0;
	bindcounter++;
	return true;
}
#endif

bool postgresqlcursor::supportsNativeBinds() {
#if defined(HAVE_POSTGRESQL_PQEXECPREPARED) && \
		defined(HAVE_POSTGRESQL_PQPREPARE)
	return true;
#else
	return false;
#endif
}

bool postgresqlcursor::executeQuery(const char *query, uint32_t length) {

	// initialize the counts
	ncols=0;
	nrows=0;
	currentrow=-1;

#if defined(HAVE_POSTGRESQL_PQEXECPREPARED) && \
		defined(HAVE_POSTGRESQL_PQPREPARE)
	if (bindcounter) {
		// execute the query
		pgresult=PQexecPrepared(postgresqlconn->pgconn,
					cursorname,
					bindcounter,bindvalues,
					bindlengths,bindformats,0);
		// reset bind counter
		bindcounter=0;
	} else {
		pgresult=PQexec(postgresqlconn->pgconn,query);
	}
#else
	pgresult=PQexec(postgresqlconn->pgconn,query);
#endif

	// handle a failed query
	if (pgresult==(PGresult *)NULL) {
		return false;
	}

	// handle errors
	ExecStatusType	pgstatus=PQresultStatus(pgresult);
	if (pgstatus==PGRES_BAD_RESPONSE ||
		pgstatus==PGRES_NONFATAL_ERROR ||
		pgstatus==PGRES_FATAL_ERROR) {
		return false;
	}

	checkForTempTable(query,length);

	// get the col count
	ncols=PQnfields(pgresult);

	// get the row count
	nrows=PQntuples(pgresult);

	// get the affected row count
	const char	*affrows=PQcmdTuples(pgresult);
	affectedrows=0;
	if (affrows && affrows[0]) {
		affectedrows=charstring::toInteger(affrows);
	}

#ifdef HAVE_POSTGRESQL_PQOIDVALUE
	// get the oid of the inserted row (if this was an insert)
	Oid	coid=PQoidValue(pgresult);
	if (coid!=InvalidOid) {
		postgresqlconn->currentoid=coid;
	}
#endif

	return true;
}

bool postgresqlcursor::knowsRowCount() {
	return true;
}

uint64_t postgresqlcursor::rowCount() {
	return nrows;
}

uint64_t postgresqlcursor::affectedRows() {
	return affectedrows;
}

uint32_t postgresqlcursor::colCount() {
	return ncols;
}

uint16_t postgresqlcursor::columnTypeFormat() {
	if (postgresqlconn->typemangling==1) {
		return (uint16_t)COLUMN_TYPE_IDS;
	} else {
		return (uint16_t)COLUMN_TYPE_NAMES;
	}
}

const char *postgresqlcursor::getColumnName(uint32_t col) {
	return PQfname(pgresult,col);
}

uint16_t postgresqlcursor::getColumnType(uint32_t col) {
	// Types are strange in POSTGRESQL, there are no actual
	// types, only internal numbers that correspond to 
	// types which are defined in a database table 
	// somewhere.
	// If typemangling is turned on, translate to standard
	// types, otherwise return the type number.
	switch (PQftype(pgresult,col)) {
		case 16: //bool
			return BOOL_DATATYPE;
		case 17: //bytea
			return BYTEA_DATATYPE;
		case 18: //char
			return CHAR_DATATYPE;
		case 19: //name
			return NAME_DATATYPE;
		case 20: //int8
			return INT8_DATATYPE;
		case 21: //int2
			return INT2_DATATYPE;
		case 22: //int2vector
			return INT2VECTOR_DATATYPE;
		case 23: //int4
			return INT4_DATATYPE;
		case 24: //regproc
			return REGPROC_DATATYPE;
		case 25: //text
			return TEXT_DATATYPE;
		case 26: //oid
			return OID_DATATYPE;
		case 27: //tid
			return TID_DATATYPE;
		case 28: //xid
			return XID_DATATYPE;
		case 29: //cid
			return CID_DATATYPE;
		case 30: //oidvector
			return OIDVECTOR_DATATYPE;
		case 71: //pg_type
			return PG_TYPE_DATATYPE;
		case 75: //pg_attribute
			return PG_ATTRIBUTE_DATATYPE;
		case 81: //pg_proc
			return PG_PROC_DATATYPE;
		case 83: //pg_class
			return PG_CLASS_DATATYPE;
		case 210: //smgr
			return SMGR_DATATYPE;
		case 600: //point
			return POINT_DATATYPE;
		case 601: //lseg
			return LSEG_DATATYPE;
		case 602: //path
			return PATH_DATATYPE;
		case 603: //box
			return BOX_DATATYPE;
		case 604: //polygon
			return POLYGON_DATATYPE;
		case 628: //line
			return LINE_DATATYPE;
		case 629: //_line
			return _LINE_DATATYPE;
		case 651: //_cidr
			return _CIDR_DATATYPE;
		case 700: //float4
			return FLOAT4_DATATYPE;
		case 701: //float8
			return FLOAT8_DATATYPE;
		case 702: //abstime
			return ABSTIME_DATATYPE;
		case 703: //reltime
			return RELTIME_DATATYPE;
		case 704: //tinterval
			return TINTERVAL_DATATYPE;
		case 718: //circle
			return CIRCLE_DATATYPE;
		case 719: //_circle
			return _CIRCLE_DATATYPE;
		case 790: //money
			return MONEY_DATATYPE;
		case 791: //_money
			return _MONEY_DATATYPE;
		case 829: //macaddr
			return MACADDR_DATATYPE;
		case 869: //inet
			return INET_DATATYPE;
		case 650: //cidr
			return CIDR_DATATYPE;
		case 1000: //_bool
			return _BOOL_DATATYPE;
		case 1001: //_bytea
			return _BYTEA_DATATYPE;
		case 1002: //_char
			return _CHAR_DATATYPE;
		case 1003: //_name
			return _NAME_DATATYPE;
		case 1005: //_int2
			return _INT2_DATATYPE;
		case 1006: //_int2vector
			return _INT2VECTOR_DATATYPE;
		case 1007: //_int4
			return _INT4_DATATYPE;
		case 1008: //_regproc
			return _REGPROC_DATATYPE;
		case 1009: //_text
			return _TEXT_DATATYPE;
		case 1010: //_tid
			return _TID_DATATYPE;
		case 1011: //_xid
			return _XID_DATATYPE;
		case 1012: //_cid
			return _CID_DATATYPE;
		case 1013: //_oidvector
			return _OIDVECTOR_DATATYPE;
		case 1014: //_bpchar
			return _BPCHAR_DATATYPE;
		case 1015: //_varchar
			return _VARCHAR_DATATYPE;
		case 1016: //_int8
			return _INT8_DATATYPE;
		case 1017: //_point
			return _POINT_DATATYPE;
		case 1018: //_lseg
			return _LSEG_DATATYPE;
		case 1019: //_path
			return _PATH_DATATYPE;
		case 1020: //_box
			return _BOX_DATATYPE;
		case 1021: //_float4
			return _FLOAT4_DATATYPE;
		case 1022: //_float8
			return _FLOAT8_DATATYPE;
		case 1023: //_abstime
			return _ABSTIME_DATATYPE;
		case 1024: //_reltime
			return _RELTIME_DATATYPE;
		case 1025: //_tinterval
			return _TINTERVAL_DATATYPE;
		case 1027: //_polygon
			return _POLYGON_DATATYPE;
		case 1028: //_oid
			return _OID_DATATYPE;
		case 1033: //aclitem
			return ACLITEM_DATATYPE;
		case 1034: //_aclitem
			return _ACLITEM_DATATYPE;
		case 1040: //_macaddr
			return _MACADDR_DATATYPE;
		case 1041: //_inet
			return _INET_DATATYPE;
		case 1042: //bpchar
			return BPCHAR_DATATYPE;
		case 1043: //varchar
			return VARCHAR_DATATYPE;
		case 1082: //date
			return DATE_DATATYPE;
		case 1083: //time
			return TIME_DATATYPE;
		case 1114: //timestamp
		case 1296:
			return TIMESTAMP_DATATYPE;
		case 1115: //_timestamp
			return _TIMESTAMP_DATATYPE;
		case 1182: //_date
			return _DATE_DATATYPE;
		case 1183: //_time
			return _TIME_DATATYPE;
		case 1184: //timestamptz
			return TIMESTAMPTZ_DATATYPE;
		case 1185: //_timestamptz
			return _TIMESTAMPTZ_DATATYPE;
		case 1186: //interval
			return INTERVAL_DATATYPE;
		case 1187: //_interval
			return _INTERVAL_DATATYPE;
		case 1231: //_numeric
			return _NUMERIC_DATATYPE;
		case 1266: //timetz
			return TIMETZ_DATATYPE;
		case 1270: //_timetz
			return _TIMETZ_DATATYPE;
		case 1560: //bit
			return BIT_DATATYPE;
		case 1561: //_bit
			return _BIT_DATATYPE;
		case 1562: //varbit
			return VARBIT_DATATYPE;
		case 1563: //_varbit
			return _VARBIT_DATATYPE;
		case 1700: //numeric
			return NUMERIC_DATATYPE;
		case 1790: //refcursor
			return REFCURSOR_DATATYPE;
		case 2201: //_refcursor
			return _REFCURSOR_DATATYPE;
		case 2202: //regprocedure
			return REGPROCEDURE_DATATYPE;
		case 2203: //regoper
			return REGOPER_DATATYPE;
		case 2204: //regoperator
			return REGOPERATOR_DATATYPE;
		case 2205: //regclass
			return REGCLASS_DATATYPE;
		case 2206: //regtype
			return REGTYPE_DATATYPE;
		case 2207: //_regprocedure
			return _REGPROCEDURE_DATATYPE;
		case 2208: //_regoper
			return _REGOPER_DATATYPE;
		case 2209: //_regoperator
			return _REGOPERATOR_DATATYPE;
		case 2210: //_regclass
			return _REGCLASS_DATATYPE;
		case 2211: //_regtype
			return _REGTYPE_DATATYPE;
		case 2249: //record
			return RECORD_DATATYPE;
		case 2275: //cstring
			return CSTRING_DATATYPE;
		case 2276: //any
			return ANY_DATATYPE;
		case 2277: //anyarray
			return ANYARRAY_DATATYPE;
		case 2278: //void
			return VOID_DATATYPE;
		case 2279: //trigger
			return TRIGGER_DATATYPE;
		case 2280: //language_handler
			return LANGUAGE_HANDLER_DATATYPE;
		case 2281: //internal
			return INTERNAL_DATATYPE;
		case 2282: //opaque
			return OPAQUE_DATATYPE;
		case 2283: //anyelement
			return ANYELEMENT_DATATYPE;
		case 705: //unknown
		default:
			return UNKNOWN_DATATYPE;
	}
}

const char *postgresqlcursor::getColumnTypeName(uint32_t col) {
	// Types are strange in POSTGRESQL, there are no actual
	// types, only internal numbers that correspond to 
	// types which are defined in a database table 
	// somewhere.
	// typemangling=0 means return the internal number as a string
	// typemangling=1 means translate to a standard datatype
	// 			(handled by getColumnType above)
	// typemangling=2 means return the name as a string
	Oid	pgfieldtype=PQftype(pgresult,col);
	if (!postgresqlconn->typemangling) {
		charstring::printf(typenamebuffer,sizeof(typenamebuffer),
						"%d",(int32_t)pgfieldtype);
		return typenamebuffer;
	} else {
		for (int i=0; i<postgresqlconn->datatypecount; i++) {
			if ((int32_t)pgfieldtype==
				postgresqlconn->datatypeids[i]) {
				return postgresqlconn->datatypenames[i];
			}
		}
	}
	return NULL;
}

uint32_t postgresqlcursor::getColumnLength(uint32_t col) {
	int32_t	size=PQfsize(pgresult,col);
#ifdef HAVE_POSTGRESQL_PQFMOD
	if (size<0) {
		size=PQfmod(pgresult,col);
	}
#endif
	if (size<0) {
		size=0;
	}
	return size;
}

uint16_t postgresqlcursor::getColumnIsBinary(uint32_t col) {
	// is this binary data (all columns will contain binary data if it is)
	int16_t	binary=false;
#ifdef HAVE_POSTGRESQL_PQBINARYTUPLES
	binary=PQbinaryTuples(pgresult);
#endif
	return binary;
}

bool postgresqlcursor::noRowsToReturn() {
	return (!nrows);
}

bool postgresqlcursor::fetchRow() {
	if (currentrow<nrows-1) {
		currentrow++;
		return true;
	}
	return false;
}

void postgresqlcursor::getField(uint32_t col,
				const char **field, uint64_t *fieldlength,
				bool *blob, bool *null) {

	// handle NULLs
	if (PQgetisnull(pgresult,currentrow,col)) {
		*null=true;
		return;
	}

	// handle normal datatypes
	*field=PQgetvalue(pgresult,currentrow,col);
	*fieldlength=PQgetlength(pgresult,currentrow,col);
}

void postgresqlcursor::closeResultSet() {

#if defined(HAVE_POSTGRESQL_PQEXECPREPARED) && \
		defined(HAVE_POSTGRESQL_PQPREPARE)
	for (uint16_t i=0; i<bindcounter; i++) {
		delete[] bindvalues[i];
		bindvalues[i]=NULL;
	}
#endif

	if (pgresult) {
		PQclear(pgresult);
		pgresult=(PGresult *)NULL;
	}
}

extern "C" {
	sqlrconnection_svr *new_postgresqlconnection(sqlrcontroller_svr *cont) {
		return new postgresqlconnection(cont);
	}
}
