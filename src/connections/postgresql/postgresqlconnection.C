// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <postgresqlconnection.h>

#include <stdlib.h>

#include <datatypes.h>

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
	if (typemang && !strcasecmp(typemang,"yes")) {
		typemangling=1;
	}
}

int	postgresqlconnection::logIn() {
			
	// log in
	pgconn=PQsetdbLogin(host,port,options,tty,db,getUser(),getPassword());

	// check the status of the login
	if (PQstatus(pgconn)==CONNECTION_BAD) {
		logOut();
		return 0;
	}

	int	devnull;
	if ((devnull=open("/dev/null",O_RDONLY))>0) {
		dup2(devnull,STDOUT_FILENO);
		dup2(devnull,STDERR_FILENO);
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

	// some useful variables
	Oid	pgfieldtype;
	int	type;
	char	*name;
	int	size;

	// for each column...
	for (int i=0; i<ncols; i++) {


		// Types are strange in POSTGRESQL, there are no actual
		// types, only internal numbers that correspond to 
		// types which are defined in a database table 
		// somewhere.
		// If typemangling is turned on, translate to standard types, 
		// otherwise return the type number.
		pgfieldtype=PQftype(pgresult,i);
		if (postgresqlconn->typemangling) {
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
		} else {
			type=(int)pgfieldtype+NUMBER_OF_DATATYPES;
		}

		// send column definition
		name=PQfname(pgresult,i);
		size=PQfsize(pgresult,i);
		if (size<0) {
			size=0;
		}
		conn->sendColumnDefinition(name,strlen(name),type,size);
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
