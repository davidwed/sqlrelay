// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <postgresqlconnection.h>
#include <rudiments/rawbuffer.h>

#include <stdio.h>
#include <stdlib.h>

#include <datatypes.h>

#ifndef HAVE_POSTGRESQL_PQSETNOTICEPROCESSOR
postgresqlconnection::postgresqlconnection() {
	datatypecount=0;
	datatypeids=NULL;
	datatypenames=NULL;
	pgconn=(PGconn *)NULL;
}

postgresqlconnection::~postgresqlconnection() {
	devnull.close();
}
#else
static void nullNoticeProcessor(void *arg, const char *message) {
}
#endif

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
	typemangling=0;
	if (typemang) {
		if (!charstring::compareIgnoringCase(typemang,"yes")) {
			typemangling=1;
		} else {
			typemangling=2;
		}
	}
#ifdef HAVE_POSTGRESQL_PQEXECPARAMS
	fakebinds=!charstring::compare(connectStringValue("fakebinds"),"yes");
#endif
}

bool postgresqlconnection::logIn() {

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

	return true;
}

sqlrcursor *postgresqlconnection::initCursor() {
	return (sqlrcursor *)new postgresqlcursor((sqlrconnection *)this);
}

void postgresqlconnection::deleteCursor(sqlrcursor *curs) {
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

postgresqlcursor::postgresqlcursor(sqlrconnection *conn) :
						sqlrcursor(conn) {
	postgresqlconn=(postgresqlconnection *)conn;
	pgresult=NULL;
#ifdef HAVE_POSTGRESQL_PQEXECPARAMS
	deallocatestatement=false;
	cursorname=NULL;
	bindcounter=0;
	bindformats=NULL;
	bindvalues=NULL;
	bindlengths=NULL;
#endif
}

#ifdef HAVE_POSTGRESQL_PQEXECPARAMS
postgresqlcursor::~postgresqlcursor() {
	delete[] cursorname;
}

bool postgresqlcursor::openCursor(uint16_t id) {
	size_t	cursornamelen=6+charstring::integerLength(id)+1;
	cursorname=new char[cursornamelen];
	snprintf(cursorname,cursornamelen,"cursor%d",id);
	return true;
}

bool postgresqlcursor::prepareQuery(const char *query, uint32_t length) {

	if (postgresqlconn->fakebinds) {
		return true;
	}

	// store inbindcount here, otherwise if rebinding/reexecution occurs and
	// the client tries to bind more variables than were defined when the
	// query was prepared, it would cause the inputBind methods to attempt
	// to address beyond the end of the various arrays
	bindcount=inbindcount;

	if (!bindcount) {
		return true;
	}

	// reset bind counter
	bindcounter=0;

	// clear bind arrays
	delete[] bindvalues;
	delete[] bindlengths;
	delete[] bindformats;
	bindvalues=NULL;
	bindlengths=NULL;
	bindformats=NULL;

	// create new bind arrays
	if (bindcount) {
		bindvalues=new char *[bindcount];
		bindlengths=new int[bindcount];
		bindformats=new int[bindcount];
	}

	// remove this named statement, if it exists already
	if (deallocatestatement) {
		stringbuffer	rmquery;
		rmquery.append("deallocate ")->append(cursorname);
		pgresult=PQexec(postgresqlconn->pgconn,rmquery.getString());
		if (pgresult==(PGresult *)NULL) {
			return false;
		}
		PQclear(pgresult);
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
		// FIXME: do I need to do a PQclear here?
		return false;
	}
	deallocatestatement=true;
	return true;
}

bool postgresqlcursor::inputBindString(const char *variable, 
						uint16_t variablesize,
						const char *value, 
						uint16_t valuesize,
						int16_t *isnull) {

	if (postgresqlconn->fakebinds) {
		return true;
	}

	// don't attempt to bind beyond the number of
	// variables defined when the query was prepared
	if (bindcounter>bindcount) {
		return false;
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

	if (postgresqlconn->fakebinds) {
		return true;
	}

	// don't attempt to bind beyond the number of
	// variables defined when the query was prepared
	if (bindcounter>bindcount) {
		return false;
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

	if (postgresqlconn->fakebinds) {
		return true;
	}

	// don't attempt to bind beyond the number of
	// variables defined when the query was prepared
	if (bindcounter>bindcount) {
		return false;
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

	if (postgresqlconn->fakebinds) {
		return true;
	}

	// don't attempt to bind beyond the number of
	// variables defined when the query was prepared
	if (bindcounter>bindcount) {
		return false;
	}

	if (*isnull) {
		bindvalues[bindcounter]=NULL;
		bindlengths[bindcounter]=0;
	} else {
		bindvalues[bindcounter]=static_cast<char *>
					(rawbuffer::duplicate(value,valuesize));
		bindlengths[bindcounter]=valuesize;
	}
	bindformats[bindcounter]=0;
	bindcounter++;
	return true;
}

bool postgresqlcursor::inputBindClob(const char *variable, 
						uint16_t variablesize,
						const char *value, 
						uint32_t valuesize,
						int16_t *isnull) {

	if (postgresqlconn->fakebinds) {
		return true;
	}

	// don't attempt to bind beyond the number of
	// variables defined when the query was prepared
	if (bindcounter>bindcount) {
		return false;
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

bool postgresqlcursor::executeQuery(const char *query, uint32_t length,
							bool execute) {

	// initialize the counts
	ncols=0;
	nrows=0;
	currentrow=-1;

#ifdef HAVE_POSTGRESQL_PQEXECPARAMS
	if (postgresqlconn->fakebinds) {
#endif
		// fake binds
		const char	*queryptr=query;
		stringbuffer	*newquery=fakeInputBinds(query);
		if (newquery) {
			queryptr=newquery->getString();
		}

		pgresult=PQexec(postgresqlconn->pgconn,queryptr);

		if (newquery) {
			delete newquery;
		}
#ifdef HAVE_POSTGRESQL_PQEXECPARAMS
	} else {
		if (bindcount) {
			// execute the query
			pgresult=PQexecPrepared(postgresqlconn->pgconn,
						cursorname,
						bindcount,bindvalues,
						bindlengths,bindformats,0);
			// reset bind counter
			bindcounter=0;
		} else {
			pgresult=PQexec(postgresqlconn->pgconn,query);
		}
	}
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
		// FIXME: do I need to do a PQclear here?
		return false;
	}

	//checkForTempTable(query,length);

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

	return true;
}

const char *postgresqlcursor::getErrorMessage(bool *liveconnection) {
	*liveconnection=(PQstatus(postgresqlconn->pgconn)==CONNECTION_OK);
	return PQerrorMessage(postgresqlconn->pgconn);
}

void postgresqlcursor::returnRowCounts() {
	conn->sendRowCounts(true,nrows,true,affectedrows);
}

void postgresqlcursor::returnColumnCount() {
	conn->sendColumnCount(ncols);
}

void postgresqlcursor::returnColumnInfo() {

	if (postgresqlconn->typemangling==1) {
		conn->sendColumnTypeFormat(COLUMN_TYPE_IDS);
	} else {
		conn->sendColumnTypeFormat(COLUMN_TYPE_NAMES);
	}

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
			if ((int32_t)pgfieldtype==23) {
				type=INT_DATATYPE;
			} else if ((int32_t)pgfieldtype==701) {
				type=FLOAT_DATATYPE;
			} else if ((int32_t)pgfieldtype==700) {
				type=REAL_DATATYPE;
			} else if ((int32_t)pgfieldtype==21) {
				type=SMALLINT_DATATYPE;
			} else if ((int32_t)pgfieldtype==1042) {
				type=CHAR_DATATYPE;
			} else if ((int32_t)pgfieldtype==1043) {
				type=VARCHAR_DATATYPE;
			} else if ((int32_t)pgfieldtype==25) {
				type=TEXT_DATATYPE;
			} else if ((int32_t)pgfieldtype==1082) {
				type=DATE_DATATYPE;
			} else if ((int32_t)pgfieldtype==1083) {
				type=TIME_DATATYPE;
			} else if ((int32_t)pgfieldtype==1296 || 
					(int32_t)pgfieldtype==1184) {
				type=TIMESTAMP_DATATYPE;
			} else {
				type=UNKNOWN_DATATYPE;
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

void postgresqlcursor::returnRow() {

	// send the row back
	for (int32_t col=0; col<ncols; col++) {

		// get the row
		if (PQgetisnull(pgresult,currentrow,col)) {
			conn->sendNullField();
		} else {
			conn->sendField(PQgetvalue(pgresult,currentrow,col),
				PQgetlength(pgresult,currentrow,col));
		}
	}
}


void postgresqlcursor::cleanUpData(bool freeresult, bool freebinds) {
	if (freeresult && pgresult) {
		PQclear(pgresult);
		pgresult=(PGresult *)NULL;
	}
}
