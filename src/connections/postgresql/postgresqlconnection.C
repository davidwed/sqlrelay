// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <postgresqlconnection.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdlib.h>

#include <datatypes.h>

static void	nullNoticeProcessor(void *arg, const char *message) {
}

int	postgresqlconnection::getNumberOfConnectStringVars() {
	return NUM_CONNECT_STRING_VARS;
}

void	postgresqlconnection::handleConnectString() {
	host=connectStringValue("host");
	port=connectStringValue("port");
	options=connectStringValue("options");
	tty=connectStringValue("tty");
	db=connectStringValue("db");
	setUser(connectStringValue("user"));
	setPassword(connectStringValue("password"));
	char	*typemang=connectStringValue("typemangling");
	typemangling=0;
	if (typemang) {
		if (!strcasecmp(typemang,"yes")) {
			typemangling=1;
		} else {
			typemangling=2;
		}
	}
}

int	postgresqlconnection::logIn() {

	// initialize the datatype storage buffers
	if (typemangling==2) {
		datatypecount=0;
		datatypeids=NULL;
		datatypenames=NULL;
	}
			
	// log in
	pgconn=PQsetdbLogin(host,port,options,tty,db,getUser(),getPassword());

	// check the status of the login
	if (PQstatus(pgconn)==CONNECTION_BAD) {
		logOut();
		return 0;
	}

	// make sure that no messages get sent to the console
	PQsetNoticeProcessor(pgconn,nullNoticeProcessor,NULL);

	// get the datatypes
	if (typemangling==2) {
		PGresult	*result=PQexec(pgconn,
					"select oid,typname from pg_type");
		if (result==(PGresult *)NULL) {
			return 0;
		}

		// create the datatype storage buffers
		datatypecount=PQntuples(result);
		datatypeids=new long[datatypecount];
		datatypenames=new char *[datatypecount];

		// copy the datatype ids/names into the buffers
		for (int i=0; i<datatypecount; i++) {
			datatypeids[i]=atoi(PQgetvalue(result,i,0));
			datatypenames[i]=strdup(PQgetvalue(result,i,1));
		}
	
		// clean up
		PQclear(result);
	}

	return 1;
}

sqlrcursor	*postgresqlconnection::initCursor() {
	return (sqlrcursor *)new postgresqlcursor((sqlrconnection *)this);
}

void	postgresqlconnection::deleteCursor(sqlrcursor *curs) {
	delete (postgresqlcursor *)curs;
}

void	postgresqlconnection::logOut() {

	PQfinish(pgconn);

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

int	postgresqlconnection::commit() {
	if (!sqlrconnection::commit()) {
		return 0;
	}
	return 1;
}

int	postgresqlconnection::rollback() {
	if (!sqlrconnection::rollback()) {
		return 0;
	}
	return 1;
}

int	postgresqlconnection::ping() {
	if (PQstatus(pgconn)==CONNECTION_OK) {
		return 1;
	}
	return 0;
}

char	*postgresqlconnection::identify() {
	return "postgresql";
}

postgresqlcursor::postgresqlcursor(sqlrconnection *conn) : sqlrcursor(conn) {
	postgresqlconn=(postgresqlconnection *)conn;
	ddlquery=0;
	pgresult=(PGresult *)NULL;
}

int	postgresqlcursor::executeQuery(const char *query, long length,
						unsigned short execute) {

	checkForTempTable(query,length);

	// initialize the counts
	ncols=0;
	nrows=0;
	currentrow=-1;

	// fake binds
	stringbuffer	*newquery=fakeInputBinds(query);
	char	*queryptr=(char *)query;
	if (newquery) {
		queryptr=newquery->getString();
	}

	// execute the query
	pgresult=PQexec(postgresqlconn->pgconn,queryptr);
	if (newquery) {
		delete newquery;
	}

	// handle a failed query
	if (pgresult==(PGresult *)NULL) {
		return 0;
	}

	// handle errors
	ExecStatusType	pgstatus=PQresultStatus(pgresult);
	if (pgstatus==PGRES_BAD_RESPONSE || pgstatus==PGRES_NONFATAL_ERROR ||
		pgstatus==PGRES_FATAL_ERROR) {
		// FIXME: do I need to do a PQclear here?
		return 0;
	}

	// get the col count
	ncols=PQnfields(pgresult);

	// get the row count
	nrows=PQntuples(pgresult);

	// get the affected row count
	char	*affrows=PQcmdTuples(pgresult);
	affectedrows=0;
	if (affrows && affrows[0]) {
		affectedrows=atol(affrows);
	}

	return 1;
}

char	*postgresqlcursor::getErrorMessage(int *liveconnection) {
	*liveconnection=(PQstatus(postgresqlconn->pgconn)==CONNECTION_OK);
	return PQerrorMessage(postgresqlconn->pgconn);
}

void	postgresqlcursor::returnRowCounts() {

	// send row counts (affected rows unknown in postgresql)
	conn->sendRowCounts((long)nrows,(long)affectedrows);
}

void	postgresqlcursor::returnColumnCount() {
	conn->sendColumnCount(ncols);
}

void	postgresqlcursor::returnColumnInfo() {

	if (postgresqlconn->typemangling==1) {
		conn->sendColumnTypeFormat(COLUMN_TYPE_IDS);
	} else {
		conn->sendColumnTypeFormat(COLUMN_TYPE_NAMES);
	}

	// some useful variables
	Oid	pgfieldtype;
	unsigned short	type;
	char	*typestring;
	if (!postgresqlconn->typemangling) {
		typestring=new char[6];
	}
	char		*name;
	int		size;

	// for each column...
	for (int i=0; i<ncols; i++) {


		// Types are strange in POSTGRESQL, there are no actual
		// types, only internal numbers that correspond to 
		// types which are defined in a database table 
		// somewhere.
		// If typemangling is turned on, translate to standard types, 
		// otherwise return the type number.
		pgfieldtype=PQftype(pgresult,i);
		if (!postgresqlconn->typemangling) {
			sprintf(typestring,"%d",(int)pgfieldtype);
		} else if (postgresqlconn->typemangling==1) {
			if ((int)pgfieldtype==23) {
				type=INT_DATATYPE;
			} else if ((int)pgfieldtype==701) {
				type=FLOAT_DATATYPE;
			} else if ((int)pgfieldtype==700) {
				type=REAL_DATATYPE;
			} else if ((int)pgfieldtype==21) {
				type=SMALLINT_DATATYPE;
			} else if ((int)pgfieldtype==1042) {
				type=CHAR_DATATYPE;
			} else if ((int)pgfieldtype==1043) {
				type=VARCHAR_DATATYPE;
			} else if ((int)pgfieldtype==25) {
				type=TEXT_DATATYPE;
			} else if ((int)pgfieldtype==1082) {
				type=DATE_DATATYPE;
			} else if ((int)pgfieldtype==1083) {
				type=TIME_DATATYPE;
			} else if ((int)pgfieldtype==1296 || 
					(int)pgfieldtype==1184) {
				type=TIMESTAMP_DATATYPE;
			} else {
				type=UNKNOWN_DATATYPE;
			}
		} else if (postgresqlconn->typemangling==2) {
			for (int i=0; i<postgresqlconn->datatypecount; i++) {
				if ((long)pgfieldtype==
					postgresqlconn->datatypeids[i]) {
					typestring=postgresqlconn->
							datatypenames[i];
				}
			}
		}

		// send column definition
		name=PQfname(pgresult,i);
		size=PQfsize(pgresult,i);
		if (size<0) {
			size=PQfmod(pgresult,i);
		}
		if (size<0) {
			size=0;
		}

		if (postgresqlconn->typemangling==1) {
			conn->sendColumnDefinition(name,strlen(name),
							type,size,0,0,0,0,0);
		} else {
			conn->sendColumnDefinitionString(name,strlen(name),
					typestring,strlen(typestring),size,
								0,0,0,0,0);
		}
	}
}

int	postgresqlcursor::noRowsToReturn() {
	if (!nrows) {
		return 1;
	}
	return 0;
}

int	postgresqlcursor::skipRow() {
	if (fetchRow()) {
		return 1;
	}
	return 0;
}

int	postgresqlcursor::fetchRow() {
	currentrow++;
	if (currentrow==nrows) {
		return 0;
	}
	return 1;
}

void	postgresqlcursor::returnRow() {

	// send the row back
	for (int col=0; col<ncols; col++) {

		// get the row
		if (PQgetisnull(pgresult,currentrow,col)) {
			conn->sendNullField();
		} else {
			conn->sendField(PQgetvalue(pgresult,currentrow,col),
				PQgetlength(pgresult,currentrow,col));
		}
	}
}


void	postgresqlcursor::cleanUpData() {
	if (pgresult) {
		PQclear(pgresult);
		pgresult=(PGresult *)NULL;
	}
}
