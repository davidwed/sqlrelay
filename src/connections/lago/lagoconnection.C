// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <config.h>
#include <datatypes.h>

#include <lagoconnection.h>

#include <stdlib.h>
#include <stdio.h>

int lagoconnection::getNumberOfConnectStringVars() {
	return NUM_CONNECT_STRING_VARS;
}

void lagoconnection::handleConnectString() {
	host=connectStringValue("host");
	port=connectStringValue("port");
	db=connectStringValue("db");
	setUser(connectStringValue("user"));
	setPassword(connectStringValue("password"));
}

bool lagoconnection::logIn() {

	lagocontext=Lnewctx();

	// handle host
	char	*hostval="localhost";
	if (host && host[0]) {
		hostval=host;
	}

	// handle port
	char	*portval="7412";
	if (port && port[0]) {
		portval=port;
	}

	// handle db
	if (!(db && db[0])) {
		printf("No db was specified in the connect string.\n");
		Ldelctx(lagocontext);
		return false;
	}

	// handle user
	char	*user=getUser();
	if (!(user && user[0])) {
		printf("No user was specified in the connect string.\n");
		Ldelctx(lagocontext);
		return false;
	}

	// handle password
	char	*password=getPassword();
	if (!(password && password[0])) {
		printf("No password was specified in the connect string.\n");
		Ldelctx(lagocontext);
		return false;
	}

	if (Lconnect(lagocontext,hostval,portval,db,user,password)==-1) {
		Ldelctx(lagocontext);
		return false;
	}
	return true;
}

sqlrcursor *lagoconnection::initCursor() {
	return (sqlrcursor *)new lagocursor((sqlrconnection *)this);
}

void lagoconnection::deleteCursor(sqlrcursor *curs) {
	delete (lagocursor *)curs;
}

void lagoconnection::logOut() {
	Ldisconnect(lagocontext);
	Ldelctx(lagocontext);
}

bool lagoconnection::ping() {
	return (Lisconnected(lagocontext))?true:false;
}

char *lagoconnection::identify() {
	return "lago";
}

bool lagoconnection::isTransactional() {
	return false;
}

bool lagoconnection::autoCommitOn() {
	// do nothing
	return true;
}

bool lagoconnection::autoCommitOff() {
	// do nothing
	return true;
}

bool lagoconnection::commit() {
	// do nothing
	return true;
}

bool lagoconnection::rollback() {
	// do nothing
	return true;
}

lagocursor::lagocursor(sqlrconnection *conn) : sqlrcursor(conn) {
	lagoconn=(lagoconnection *)conn;
}

bool lagocursor::executeQuery(const char *query, long length, bool execute) {

	// initialize return values
	ncols=0;
	nrows=0;

	// fake binds
	stringbuffer	*newquery=fakeInputBinds(query);

	// execute the query
	if (newquery) {
		lagoresult=Lquery(lagoconn->lagocontext,
					newquery->getString());
		delete newquery;
	} else {
		lagoresult=Lquery(lagoconn->lagocontext,query);
	}
	if (!lagoresult) {
		return false;
	}

	// workaround, lagoresult doesn't necessarily get set to 0 for
	// every error such as a select on a non-existant table
	char	*err=(char *)Lgeterrmsg(lagoconn->lagocontext);
	if (err && err[0]) {
		return false;
	}

	// get the column count
	ncols=Lgetncols(lagoresult);

	// get the row count
	nrows=Lgetnrows(lagoresult);

	return true;
}

char *lagocursor::getErrorMessage(bool *liveconnection) {

	// only return an error message of the error wasn't a dead database
	*liveconnection=(Lisconnected(lagoconn->lagocontext))?true:false;
	if (*liveconnection) {
		return (char *)Lgeterrmsg(lagoconn->lagocontext);
	} else {
		return "";
	}
}

void lagocursor::returnRowCounts() {

	// send row counts (affected row count unknown in Lago)
	conn->sendRowCounts((long)nrows,(long)-1);
}

void lagocursor::returnColumnCount() {
	conn->sendColumnCount(ncols);
}

void lagocursor::returnColumnInfo() {

	conn->sendColumnTypeFormat(COLUMN_TYPE_IDS);

	// gonna need this later
	char	*name;
	int	precision;
	int	scale;
	int	nullable;
	int	length;
	int	type;

	// for each column...
	for (int i=1; i<ncols+1; i++) {

		// get name, precision, scale and nullability
		name=(char *)Lgetcolname(lagoresult,i);
		precision=Lgetcolprec(lagoresult,i);
		scale=Lgetcolscale(lagoresult,i);

		// though Lgetcolnull is defined in lago.h,
		// it's not in liblago.a
		//nullable=(Lgetcolnull(lagoresult,i)!=0);
		nullable=0;

		// set column type
		LType	coltype=Lgetcoltype(lagoresult,i);
		if (coltype==LSQL_UNDEFINED) {
			type=UNDEFINED_DATATYPE;
			length=precision;
		} else if (coltype==LSQL_SMALLINT) {
			type=SMALLINT_DATATYPE;
			length=2;
		} else if (coltype==LSQL_INT) {
			type=INT_DATATYPE;
			length=4;
		} else if (coltype==LSQL_FLOAT) {
			type=FLOAT_DATATYPE;
			length=4;
		} else if (coltype==LSQL_DOUBLE) {
			type=DOUBLE_DATATYPE;
			length=8;
		} else if (coltype==LSQL_CHAR) {
			type=CHAR_DATATYPE;
			length=precision;
		} else if (coltype==LSQL_VARCHAR) {
			type=VARCHAR_DATATYPE;
			length=precision;
		} else if (coltype==LSQL_DATE) {
			type=DATE_DATATYPE;
			length=4;
		} else if (coltype==LSQL_TIME) {
			type=TIME_DATATYPE;
			length=4;
		} else if (coltype==LSQL_TIMESTAMP) {
			type=TIMESTAMP_DATATYPE;
			length=4;
		} else {
			type=UNKNOWN_DATATYPE;
			length=precision;
		}

		// send the column definition
		conn->sendColumnDefinition(name,strlen(name),type,
						length,precision,scale,
						nullable,0,0,
						0,0,0,0,0);
	}
}

bool lagocursor::noRowsToReturn() {

	// for queries which returned nothing, such as DML or blank selects,
	// abort early, unfortunately, Lgetnrows doesn't always work
	// correctly hope that gets fixed some day
	return (!Lgetnrows(lagoresult));
}

bool lagocursor::skipRow() {
	return fetchRow();
}

bool lagocursor::fetchRow() {
	return (Lfetch(lagoresult)!=LFETCH_END);
}

void lagocursor::returnRow() {

	// a useful variable
	char	*field;

	// here's that 1 based column thing again
	for (int col=1; col<ncols+1; col++) {

		field=(char *)Lgetasstr(lagoresult,col);

		// This is weird, lago.h defines Lisnull and the documentation
		// talks about it, but it is not implemented.  For non-character
		// types, Lgetasstr will be null if the field is null, but for
		// character types it's the string "null".  Weird.  I guess you
		// can't store "null" in a character field in lago.
		if (!field || (field && !strcmp(field,"null"))) {
			conn->sendNullField();
		} else {
			conn->sendField(field,strlen(field));
		}
	}
}

void lagocursor::cleanUpData(bool freeresult, bool freebinds) {
	// should I call Lclear here?
}
