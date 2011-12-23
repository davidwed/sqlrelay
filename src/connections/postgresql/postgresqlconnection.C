// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <postgresqlconnection.h>
#include <rudiments/rawbuffer.h>

#include <stdio.h>
#include <stdlib.h>

#include <datatypes.h>

#ifdef HAVE_POSTGRESQL_PQSETNOTICEPROCESSOR
static void nullNoticeProcessor(void *arg, const char *message) {
}
#endif

postgresqlconnection::postgresqlconnection() : sqlrconnection_svr() {
	dbversion=NULL;
	datatypecount=0;
	datatypeids=NULL;
	datatypenames=NULL;
	pgconn=(PGconn *)NULL;
	currentoid=InvalidOid;
	lastinsertidquery=NULL;
}

postgresqlconnection::~postgresqlconnection() {
#ifndef HAVE_POSTGRESQL_PQSETNOTICEPROCESSOR
	devnull.close();
#endif
	delete[] dbversion;
	delete[] lastinsertidquery;
}

uint16_t postgresqlconnection::getNumberOfConnectStringVars() {
	return NUM_CONNECT_STRING_VARS;
}

void postgresqlconnection::handleConnectString() {
	host=connectStringValue("host");
	port=connectStringValue("port");
	options=connectStringValue("options");
	db=connectStringValue("db");
	setUser(connectStringValue("user"));
	setPassword(connectStringValue("password"));
	const char	*typemang=connectStringValue("typemangling");
	if (!typemang ||!charstring::compareIgnoringCase(typemang,"no")) {
		typemangling=0;
	} else if (!charstring::compareIgnoringCase(typemang,"yes")) {
		typemangling=1;
	} else {
		typemangling=2;
	}
	charset=connectStringValue("charset");
	const char	*lastinsertidfunc=
			connectStringValue("lastinsertidfunction");
	if (lastinsertidfunc) {
		stringbuffer	liiquery;
		liiquery.append("select ");
		liiquery.append(lastinsertidfunc);
		lastinsertidquery=liiquery.detachString();
	}
}

bool postgresqlconnection::logIn(bool printerrors) {

	// initialize the datatype storage buffers
	if (typemangling==2) {
		datatypecount=0;
		datatypeids=NULL;
		datatypenames=NULL;
	}

	// log in
	pgconn=PQsetdbLogin(host,port,options,NULL,db,getUser(),getPassword());

	// check the status of the login
	if (PQstatus(pgconn)==CONNECTION_BAD) {
		logOut();
		return false;
	}

#ifdef HAVE_POSTGRESQL_PQSETNOTICEPROCESSOR
	// make sure that no messages get sent to the console
	PQsetNoticeProcessor(pgconn,nullNoticeProcessor,NULL);
#else
	if (devnull.open("/dev/null",O_RDONLY)) {
		devnull.duplicate(STDOUT_FILENO);
		devnull.duplicate(STDERR_FILENO);
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
		setFakeInputBinds(true);
	}
#endif

	return true;
}

sqlrcursor_svr *postgresqlconnection::initCursor() {
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
		return false;
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
		snprintf(dbversion,charstring::length(dbversion)+1,
				"%s%02lld%02lld",
				parts[0],(long long)minor,(long long)patch);
	}
	for (uint64_t i=0; i<partslength; i++) {
		delete[] parts[i];
	}
	delete[] parts;
#endif
	return dbversion;
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

const char *postgresqlconnection::getColumnListQuery(bool wild) {
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

bool postgresqlconnection::getLastInsertId(uint64_t *id, char **error) {
	if (lastinsertidquery) {
		return sqlrconnection_svr::getLastInsertId(id,error);
	}
	*id=(currentoid!=InvalidOid)?currentoid:0;
	return true;
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
	columnnames=NULL;
}

postgresqlcursor::~postgresqlcursor() {
#if defined(HAVE_POSTGRESQL_PQEXECPREPARED) && \
		defined(HAVE_POSTGRESQL_PQPREPARE)
	deallocateStatement();
	delete[] cursorname;
#endif
	delete[] columnnames;

	for (uint16_t i=0; i<bindcount; i++) {
		delete[] bindvalues[i];
	}
	delete[] bindvalues;
	delete[] bindlengths;
	delete[] bindformats;
}

#if defined(HAVE_POSTGRESQL_PQEXECPREPARED) && \
		defined(HAVE_POSTGRESQL_PQPREPARE)
bool postgresqlcursor::openCursor(uint16_t id) {
	size_t	cursornamelen=6+charstring::integerLength(id)+1;
	cursorname=new char[cursornamelen];
	snprintf(cursorname,cursornamelen,"cursor%d",id);
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

bool postgresqlcursor::inputBindString(const char *variable, 
						uint16_t variablesize,
						const char *value, 
						uint32_t valuesize,
						int16_t *isnull) {

	// ignore attempts to bind beyond the number of
	// variables defined when the query was prepared
	if (bindcounter>=bindcount) {
		return true;
	}

	if (*isnull) {
		bindvalues[bindcounter]=NULL;
		bindlengths[bindcounter]=0;
	} else {
		bindvalues[bindcounter]=charstring::duplicate(value,valuesize);
		bindlengths[bindcounter]=valuesize;
	}
	bindformats[bindcounter]=0;
	bindcounter++;
	return true;
}

bool postgresqlcursor::inputBindInteger(const char *variable, 
						uint16_t variablesize,
						int64_t *value) {

	// ignore attempts to bind beyond the number of
	// variables defined when the query was prepared
	if (bindcounter>=bindcount) {
		return true;
	}

	bindvalues[bindcounter]=charstring::parseNumber(*value);
	bindlengths[bindcounter]=charstring::length(bindvalues[bindcounter]);
	bindformats[bindcounter]=0;
	bindcounter++;
	return true;
}

bool postgresqlcursor::inputBindDouble(const char *variable, 
						uint16_t variablesize,
						double *value,
						uint32_t precision,
						uint32_t scale) {

	// ignore attempts to bind beyond the number of
	// variables defined when the query was prepared
	if (bindcounter>=bindcount) {
		return true;
	}

	bindvalues[bindcounter]=charstring::parseNumber(*value,precision,scale);
	bindlengths[bindcounter]=charstring::length(bindvalues[bindcounter]);
	bindformats[bindcounter]=0;
	bindcounter++;
	return true;
}

bool postgresqlcursor::inputBindBlob(const char *variable, 
						uint16_t variablesize,
						const char *value, 
						uint32_t valuesize,
						int16_t *isnull) {

	// ignore attempts to bind beyond the number of
	// variables defined when the query was prepared
	if (bindcounter>=bindcount) {
		return true;
	}

	if (*isnull) {
		bindvalues[bindcounter]=NULL;
		bindlengths[bindcounter]=0;
	} else {
		bindvalues[bindcounter]=static_cast<char *>
					(rawbuffer::duplicate(value,valuesize));
		bindlengths[bindcounter]=valuesize;
	}
	bindformats[bindcounter]=1;
	bindcounter++;
	return true;
}

bool postgresqlcursor::inputBindClob(const char *variable, 
						uint16_t variablesize,
						const char *value, 
						uint32_t valuesize,
						int16_t *isnull) {

	// ignore attempts to bind beyond the number of
	// variables defined when the query was prepared
	if (bindcounter>=bindcount) {
		return true;
	}

	if (*isnull) {
		bindvalues[bindcounter]=NULL;
		bindlengths[bindcounter]=0;
	} else {
		bindvalues[bindcounter]=charstring::duplicate(value,valuesize);
		bindlengths[bindcounter]=valuesize;
	}
	bindformats[bindcounter]=0;
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

bool postgresqlcursor::executeQuery(const char *query, uint32_t length,
							bool execute) {

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
	char	*affrows=PQcmdTuples(pgresult);
	affectedrows=0;
	if (affrows && affrows[0]) {
		affectedrows=charstring::toInteger(affrows);
	}

	// get the oid of the inserted row (if this was an insert)
	Oid	coid=PQoidValue(pgresult);
	if (coid!=InvalidOid) {
		postgresqlconn->currentoid=coid;
	}

	return true;
}

const char *postgresqlcursor::errorMessage(bool *liveconnection) {
	*liveconnection=(PQstatus(postgresqlconn->pgconn)==CONNECTION_OK);
	return PQerrorMessage(postgresqlconn->pgconn);
}

bool postgresqlcursor::knowsRowCount() {
	return true;
}

uint64_t postgresqlcursor::rowCount() {
	return nrows;
}

bool postgresqlcursor::knowsAffectedRows() {
	return true;
}

uint64_t postgresqlcursor::affectedRows() {
	return affectedrows;
}

uint32_t postgresqlcursor::colCount() {
	return ncols;
}

const char * const * postgresqlcursor::columnNames() {
	columnnames=new char *[ncols];
	for (int32_t i=0; i<ncols; i++) {
		columnnames[i]=PQfname(pgresult,i);
	}
	return columnnames;
}

uint16_t postgresqlcursor::columnTypeFormat() {
	if (postgresqlconn->typemangling==1) {
		return (uint16_t)COLUMN_TYPE_IDS;
	} else {
		return (uint16_t)COLUMN_TYPE_NAMES;
	}
}

void postgresqlcursor::returnColumnInfo() {

	// some useful variables
	Oid		pgfieldtype;
	uint16_t	type;
	char		*typestring;
	if (!postgresqlconn->typemangling) {
		typestring=new char[6];
	}
	char		*name;
	int32_t		size;

	// is this binary data (all columns will contain
	// binary data if it is)
	int16_t	binary=PQbinaryTuples(pgresult);

	// for each column...
	for (int32_t i=0; i<ncols; i++) {

		// Types are strange in POSTGRESQL, there are no actual
		// types, only internal numbers that correspond to 
		// types which are defined in a database table 
		// somewhere.
		// If typemangling is turned on, translate to standard
		// types, otherwise return the type number.
		pgfieldtype=PQftype(pgresult,i);
		if (!postgresqlconn->typemangling) {
			snprintf(typestring,6,"%d",(int32_t)pgfieldtype);
		} else if (postgresqlconn->typemangling==1) {
			switch ((int32_t)pgfieldtype) {
				case 16: //bool
					type=BOOL_DATATYPE;
					break;
				case 17: //bytea
					type=BYTEA_DATATYPE;
					break;
				case 18: //char
					type=CHAR_DATATYPE;
					break;
				case 19: //name
					type=NAME_DATATYPE;
					break;
				case 20: //int8
					type=INT8_DATATYPE;
					break;
				case 21: //int2
					type=INT2_DATATYPE;
					break;
				case 22: //int2vector
					type=INT2VECTOR_DATATYPE;
					break;
				case 23: //int4
					type=INT4_DATATYPE;
					break;
				case 24: //regproc
					type=REGPROC_DATATYPE;
					break;
				case 25: //text
					type=TEXT_DATATYPE;
					break;
				case 26: //oid
					type=OID_DATATYPE;
					break;
				case 27: //tid
					type=TID_DATATYPE;
					break;
				case 28: //xid
					type=XID_DATATYPE;
					break;
				case 29: //cid
					type=CID_DATATYPE;
					break;
				case 30: //oidvector
					type=OIDVECTOR_DATATYPE;
					break;
				case 71: //pg_type
					type=PG_TYPE_DATATYPE;
					break;
				case 75: //pg_attribute
					type=PG_ATTRIBUTE_DATATYPE;
					break;
				case 81: //pg_proc
					type=PG_PROC_DATATYPE;
					break;
				case 83: //pg_class
					type=PG_CLASS_DATATYPE;
					break;
				case 210: //smgr
					type=SMGR_DATATYPE;
					break;
				case 600: //point
					type=POINT_DATATYPE;
					break;
				case 601: //lseg
					type=LSEG_DATATYPE;
					break;
				case 602: //path
					type=PATH_DATATYPE;
					break;
				case 603: //box
					type=BOX_DATATYPE;
					break;
				case 604: //polygon
					type=POLYGON_DATATYPE;
					break;
				case 628: //line
					type=LINE_DATATYPE;
					break;
				case 629: //_line
					type=_LINE_DATATYPE;
					break;
				case 651: //_cidr
					type=_CIDR_DATATYPE;
					break;
				case 700: //float4
					type=FLOAT4_DATATYPE;
					break;
				case 701: //float8
					type=FLOAT8_DATATYPE;
					break;
				case 702: //abstime
					type=ABSTIME_DATATYPE;
					break;
				case 703: //reltime
					type=RELTIME_DATATYPE;
					break;
				case 704: //tinterval
					type=TINTERVAL_DATATYPE;
					break;
				case 718: //circle
					type=CIRCLE_DATATYPE;
					break;
				case 719: //_circle
					type=_CIRCLE_DATATYPE;
					break;
				case 790: //money
					type=MONEY_DATATYPE;
					break;
				case 791: //_money
					type=_MONEY_DATATYPE;
					break;
				case 829: //macaddr
					type=MACADDR_DATATYPE;
					break;
				case 869: //inet
					type=INET_DATATYPE;
					break;
				case 650: //cidr
					type=CIDR_DATATYPE;
					break;
				case 1000: //_bool
					type=_BOOL_DATATYPE;
					break;
				case 1001: //_bytea
					type=_BYTEA_DATATYPE;
					break;
				case 1002: //_char
					type=_CHAR_DATATYPE;
					break;
				case 1003: //_name
					type=_NAME_DATATYPE;
					break;
				case 1005: //_int2
					type=_INT2_DATATYPE;
					break;
				case 1006: //_int2vector
					type=_INT2VECTOR_DATATYPE;
					break;
				case 1007: //_int4
					type=_INT4_DATATYPE;
					break;
				case 1008: //_regproc
					type=_REGPROC_DATATYPE;
					break;
				case 1009: //_text
					type=_TEXT_DATATYPE;
					break;
				case 1010: //_tid
					type=_TID_DATATYPE;
					break;
				case 1011: //_xid
					type=_XID_DATATYPE;
					break;
				case 1012: //_cid
					type=_CID_DATATYPE;
					break;
				case 1013: //_oidvector
					type=_OIDVECTOR_DATATYPE;
					break;
				case 1014: //_bpchar
					type=_BPCHAR_DATATYPE;
					break;
				case 1015: //_varchar
					type=_VARCHAR_DATATYPE;
					break;
				case 1016: //_int8
					type=_INT8_DATATYPE;
					break;
				case 1017: //_point
					type=_POINT_DATATYPE;
					break;
				case 1018: //_lseg
					type=_LSEG_DATATYPE;
					break;
				case 1019: //_path
					type=_PATH_DATATYPE;
					break;
				case 1020: //_box
					type=_BOX_DATATYPE;
					break;
				case 1021: //_float4
					type=_FLOAT4_DATATYPE;
					break;
				case 1022: //_float8
					type=_FLOAT8_DATATYPE;
					break;
				case 1023: //_abstime
					type=_ABSTIME_DATATYPE;
					break;
				case 1024: //_reltime
					type=_RELTIME_DATATYPE;
					break;
				case 1025: //_tinterval
					type=_TINTERVAL_DATATYPE;
					break;
				case 1027: //_polygon
					type=_POLYGON_DATATYPE;
					break;
				case 1028: //_oid
					type=_OID_DATATYPE;
					break;
				case 1033: //aclitem
					type=ACLITEM_DATATYPE;
					break;
				case 1034: //_aclitem
					type=_ACLITEM_DATATYPE;
					break;
				case 1040: //_macaddr
					type=_MACADDR_DATATYPE;
					break;
				case 1041: //_inet
					type=_INET_DATATYPE;
					break;
				case 1042: //bpchar
					type=BPCHAR_DATATYPE;
					break;
				case 1043: //varchar
					type=VARCHAR_DATATYPE;
					break;
				case 1082: //date
					type=DATE_DATATYPE;
					break;
				case 1083: //time
					type=TIME_DATATYPE;
					break;
				case 1114: //timestamp
				case 1296:
					type=TIMESTAMP_DATATYPE;
					break;
				case 1115: //_timestamp
					type=_TIMESTAMP_DATATYPE;
					break;
				case 1182: //_date
					type=_DATE_DATATYPE;
					break;
				case 1183: //_time
					type=_TIME_DATATYPE;
					break;
				case 1184: //timestamptz
					type=TIMESTAMPTZ_DATATYPE;
					break;
				case 1185: //_timestamptz
					type=_TIMESTAMPTZ_DATATYPE;
					break;
				case 1186: //interval
					type=INTERVAL_DATATYPE;
					break;
				case 1187: //_interval
					type=_INTERVAL_DATATYPE;
					break;
				case 1231: //_numeric
					type=_NUMERIC_DATATYPE;
					break;
				case 1266: //timetz
					type=TIMETZ_DATATYPE;
					break;
				case 1270: //_timetz
					type=_TIMETZ_DATATYPE;
					break;
				case 1560: //bit
					type=BIT_DATATYPE;
					break;
				case 1561: //_bit
					type=_BIT_DATATYPE;
					break;
				case 1562: //varbit
					type=VARBIT_DATATYPE;
					break;
				case 1563: //_varbit
					type=_VARBIT_DATATYPE;
					break;
				case 1700: //numeric
					type=NUMERIC_DATATYPE;
					break;
				case 1790: //refcursor
					type=REFCURSOR_DATATYPE;
					break;
				case 2201: //_refcursor
					type=_REFCURSOR_DATATYPE;
					break;
				case 2202: //regprocedure
					type=REGPROCEDURE_DATATYPE;
					break;
				case 2203: //regoper
					type=REGOPER_DATATYPE;
					break;
				case 2204: //regoperator
					type=REGOPERATOR_DATATYPE;
					break;
				case 2205: //regclass
					type=REGCLASS_DATATYPE;
					break;
				case 2206: //regtype
					type=REGTYPE_DATATYPE;
					break;
				case 2207: //_regprocedure
					type=_REGPROCEDURE_DATATYPE;
					break;
				case 2208: //_regoper
					type=_REGOPER_DATATYPE;
					break;
				case 2209: //_regoperator
					type=_REGOPERATOR_DATATYPE;
					break;
				case 2210: //_regclass
					type=_REGCLASS_DATATYPE;
					break;
				case 2211: //_regtype
					type=_REGTYPE_DATATYPE;
					break;
				case 2249: //record
					type=RECORD_DATATYPE;
					break;
				case 2275: //cstring
					type=CSTRING_DATATYPE;
					break;
				case 2276: //any
					type=ANY_DATATYPE;
					break;
				case 2277: //anyarray
					type=ANYARRAY_DATATYPE;
					break;
				case 2278: //void
					type=VOID_DATATYPE;
					break;
				case 2279: //trigger
					type=TRIGGER_DATATYPE;
					break;
				case 2280: //language_handler
					type=LANGUAGE_HANDLER_DATATYPE;
					break;
				case 2281: //internal
					type=INTERNAL_DATATYPE;
					break;
				case 2282: //opaque
					type=OPAQUE_DATATYPE;
					break;
				case 2283: //anyelement
					type=ANYELEMENT_DATATYPE;
					break;
				case 705: //unknown
				default:
					type=UNKNOWN_DATATYPE;
					break;
			}
		} else if (postgresqlconn->typemangling==2) {
			for (int i=0; i<postgresqlconn->datatypecount; i++) {

				if ((int32_t)pgfieldtype==
					postgresqlconn->datatypeids[i]) {
					typestring=postgresqlconn->
							datatypenames[i];
				}
			}
		}

		// send column definition
		name=PQfname(pgresult,i);
		size=PQfsize(pgresult,i);
#ifdef HAVE_POSTGRESQL_PQFMOD
		if (size<0) {
			size=PQfmod(pgresult,i);
		}
#endif
		if (size<0) {
			size=0;
		}

		if (postgresqlconn->typemangling==1) {
			conn->sendColumnDefinition(name,
						charstring::length(name),
						type,size,0,0,0,0,0,
						0,0,0,binary,0);
		} else {
			conn->sendColumnDefinitionString(name,
						charstring::length(name),
						typestring,
						charstring::length(typestring),
						size,
						0,0,0,0,0,
						0,0,0,binary,0);
		}
	}
	if (!postgresqlconn->typemangling) {
		delete[] typestring;
	}
}

bool postgresqlcursor::noRowsToReturn() {
	return (!nrows);
}

bool postgresqlcursor::skipRow() {
	return fetchRow();
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

void postgresqlcursor::cleanUpData(bool freeresult, bool freebinds) {

	if (freebinds) {
		for (uint16_t i=0; i<bindcount; i++) {
			delete[] bindvalues[i];
			bindvalues[i]=NULL;
		}
	}

	if (freeresult && pgresult) {
		PQclear(pgresult);
		pgresult=(PGresult *)NULL;
	}
	delete[] columnnames;
	columnnames=NULL;
}
