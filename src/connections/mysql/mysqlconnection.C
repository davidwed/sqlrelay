// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <mysqlconnection.h>
#if MYSQL_VERSION_ID>=32200
	#include <errmsg.h>
#endif

#include <datatypes.h>

#include <config.h>

#include <stdlib.h>
#include <string.h>

mysqlconnection::mysqlconnection() {
	connected=0;
}

int	mysqlconnection::getNumberOfConnectStringVars() {
	return NUM_CONNECT_STRING_VARS;
}

void	mysqlconnection::handleConnectString() {
	setUser(connectStringValue("user"));
	setPassword(connectStringValue("password"));
	db=connectStringValue("db");
	host=connectStringValue("host");
	port=connectStringValue("port");
	socket=connectStringValue("socket");
}

int	mysqlconnection::logIn() {


	// handle host
	char	*hostval;
	if (host && host[0]) {
		hostval=host;
	} else {
		hostval="";
	}

	// handle port
	int	portval;
	if (port && port[0]) {
		portval=atoi(port);
	} else {
		portval=0;
	}

	// handle socket
	char	*socketval;
	if (socket && socket[0]) {
		socketval=socket;
	} else {
		socketval=NULL;
	}

	// handle db
	char	*dbval;
	if (db && db[0]) {
		dbval=db;
	} else {
		dbval="";
	}
	
	// initialize database connection structure
#if MYSQL_VERSION_ID>=32200
	if (!mysql_init(&mysql)) {
		return 0;
	}
#endif

	// log in
	char	*user=getUser();
	char	*password=getPassword();
#ifdef MYSQL_VERSION_ID
	#if MYSQL_VERSION_ID>=32200
		if (!mysql_real_connect(&mysql,hostval,user,password,dbval,
						portval,socketval,0)) {
	#else
		if (!mysql_real_connect(&mysql,hostval,user,password,
						portval,socketval,0)) {
	#endif
#else
	if (!mysql_connect(&mysql,hostval,user,password)) {
#endif
		logOut();
		return 0;
	} else {
#if MYSQL_VERSION_ID<32200
		if (!mysql_select_db(&mysql,dbval)) {
			logOut();
			return 0;
		}
#endif
		connected=1;
		return 1;
	}
}

sqlrcursor	*mysqlconnection::initCursor() {
	return (sqlrcursor *)new mysqlcursor((sqlrconnection *)this);
}

void	mysqlconnection::deleteCursor(sqlrcursor *curs) {
	delete (mysqlcursor *)curs;
}

void	mysqlconnection::logOut() {
	connected=0;
	mysql_close(&mysql);
}

int	mysqlconnection::ping() {
#if MYSQL_VERSION_ID>=32200
	if (!mysql_ping(&mysql)) {
		return 1;
	}
	return 0;
#else
	int	retval=0;
	mysqlcursor	cur(this);
	if (cur.openCursor(-1) && 
		cur.prepareQuery("select 1",8) && 
		cur.executeQuery("select 1",8,1)) 
		cur.cleanUpData();
		cur.closeCursor();
		retval=1;
	}
	return retval;
#endif
}

char	*mysqlconnection::identify() {
	return "mysql";
}

int	mysqlconnection::isTransactional() {
	return 0;
}

unsigned short	mysqlconnection::autoCommitOn() {
	// do nothing
	return 1;
}

unsigned short	mysqlconnection::autoCommitOff() {
	// do nothing
	return 1;
}

int	mysqlconnection::commit() {
	// do nothing
	return 1;
}

int	mysqlconnection::rollback() {
	// do nothing
	return 1;
}

mysqlcursor::mysqlcursor(sqlrconnection *conn) : sqlrcursor(conn) {
	mysqlconn=(mysqlconnection *)conn;
	mysqlresult=NULL;
}

int	mysqlcursor::executeQuery(const char *query, long length,
						unsigned short execute) {

	// initialize counts
	ncols=0;
	nrows=0;

	// initialize result set
	mysqlresult=NULL;

	// fake binds
	stringbuffer	*newquery=fakeInputBinds(query);

	// execute the query
	if (newquery) {
		if (queryresult=mysql_real_query(&mysqlconn->mysql,
					newquery->getString(),
					strlen(newquery->getString()))) {
			delete newquery;
			return 0;
		}
		delete newquery;
	} else {
		if (queryresult=mysql_real_query(&mysqlconn->mysql,
							query,length)) {
			return 0;
		}
	}

	// get the affected row count
	affectedrows=mysql_affected_rows(&mysqlconn->mysql);

	// store the result set
	if ((mysqlresult=mysql_store_result(&mysqlconn->mysql))==
						(MYSQL_RES *)NULL) {

		// if there was an error then return failure, otherwise
		// the query must have been some DML or DDL
		char	*err=mysql_error(&mysqlconn->mysql);
		if (err && err[0]) {
			return 0;
		} else {
			return 1;
		}
	}

	// get the column count
	ncols=mysql_num_fields(mysqlresult);

	// get the row count
	nrows=mysql_num_rows(mysqlresult);

	return 1;
}

char	*mysqlcursor::getErrorMessage(int *liveconnection) {

	*liveconnection=1;
	char	*err=mysql_error(&mysqlconn->mysql);
#if MYSQL_VERSION_ID>=32200
	if (queryresult==CR_SERVER_GONE_ERROR || queryresult==CR_SERVER_LOST) {
		*liveconnection=0;
	}
#else
	if (strstr(err,"mysql server has gone away")) {
		*liveconnection=0;
	}
#endif
	return err;
}

void	mysqlcursor::returnColumnCount() {
	conn->sendColumnCount(ncols);
}

void	mysqlcursor::returnRowCounts() {

	// send row counts
	conn->sendRowCounts((long)nrows,(long)affectedrows);
}

void	mysqlcursor::returnColumnInfo() {

	conn->sendColumnTypeFormat(COLUMN_TYPE_IDS);

	// for DML or DDL queries, return no column info
	if (!mysqlresult) {
		return;
	}

	// a useful variable
	int	type;

	// position ourselves at the first field
	mysql_field_seek(mysqlresult,0);

	// for each column...
	for (int i=0; i<ncols; i++) {

		// fetch the field
		mysqlfield=mysql_fetch_field(mysqlresult);

		// append column type to the header
		if (mysqlfield->type==FIELD_TYPE_STRING) {
			type=STRING_DATATYPE;
		} else if (mysqlfield->type==FIELD_TYPE_VAR_STRING) {
			type=CHAR_DATATYPE;
		} else if (mysqlfield->type==FIELD_TYPE_DECIMAL) {
			type=DECIMAL_DATATYPE;
		} else if (mysqlfield->type==FIELD_TYPE_TINY) {
			type=TINYINT_DATATYPE;
		} else if (mysqlfield->type==FIELD_TYPE_SHORT) {
			type=SMALLINT_DATATYPE;
		} else if (mysqlfield->type==FIELD_TYPE_LONG) {
			type=INT_DATATYPE;
		} else if (mysqlfield->type==FIELD_TYPE_FLOAT) {
			type=FLOAT_DATATYPE;
		} else if (mysqlfield->type==FIELD_TYPE_DOUBLE) {
			type=REAL_DATATYPE;
		} else if (mysqlfield->type==FIELD_TYPE_LONGLONG) {
			type=BIGINT_DATATYPE;
		} else if (mysqlfield->type==FIELD_TYPE_INT24) {
			type=MEDIUMINT_DATATYPE;
		} else if (mysqlfield->type==FIELD_TYPE_TIMESTAMP) {
			type=TIMESTAMP_DATATYPE;
		} else if (mysqlfield->type==FIELD_TYPE_DATE) {
			type=DATE_DATATYPE;
		} else if (mysqlfield->type==FIELD_TYPE_TIME) {
			type=TIME_DATATYPE;
		} else if (mysqlfield->type==FIELD_TYPE_DATETIME) {
			type=DATETIME_DATATYPE;
#if MYSQL_VERSION_ID>=32200
		} else if (mysqlfield->type==FIELD_TYPE_YEAR) {
			type=YEAR_DATATYPE;
		} else if (mysqlfield->type==FIELD_TYPE_NEWDATE) {
			type=NEWDATE_DATATYPE;
#endif
		} else if (mysqlfield->type==FIELD_TYPE_NULL) {
			type=NULL_DATATYPE;
#ifdef MYSQL_VERSION_ID
		} else if (mysqlfield->type==FIELD_TYPE_ENUM) {
			type=ENUM_DATATYPE;
		} else if (mysqlfield->type==FIELD_TYPE_SET) {
			type=SET_DATATYPE;
#endif
		// For some reason, tinyblobs, mediumblobs and longblobs
		// all show up as FIELD_TYPE_BLOB despite field types being
		// defined for those types.  tinyblobs have a length
		// of 255 though, so that can be used for something.  medium
		// and long blobs both have the same length though.  Go
		// figure.  Also, the word TEXT and BLOB appear to be
		// interchangable.  We'll use BLOB because it appears to be
		// more standard than TEXT.  I wonder if this will be changed
		// in a future incarnation of mysql.  I also wonder what
		// happens on a 64 bit machine.
		} else if (mysqlfield->type==FIELD_TYPE_TINY_BLOB) {
			type=TINY_BLOB_DATATYPE;
		} else if (mysqlfield->type==FIELD_TYPE_MEDIUM_BLOB) {
			type=MEDIUM_BLOB_DATATYPE;
		} else if (mysqlfield->type==FIELD_TYPE_LONG_BLOB) {
			type=LONG_BLOB_DATATYPE;
		} else if (mysqlfield->type==FIELD_TYPE_BLOB) {
			if ((int)mysqlfield->length==255) {
				type=TINY_BLOB_DATATYPE;
			} else {
				type=BLOB_DATATYPE;
			}
		} else {
			type=UNKNOWN_DATATYPE;
		}

		// send column definition
		conn->sendColumnDefinition(mysqlfield->name,
					strlen(mysqlfield->name),
					type,(int)mysqlfield->length);
	}
}

int	mysqlcursor::noRowsToReturn() {

	// for DML or DDL queries, return no data
	if (!mysqlresult) {
		return 1;
	}
	return 0;
}

int	mysqlcursor::skipRow() {
	return fetchRow();
}

int	mysqlcursor::fetchRow() {

	return ((mysqlrow=mysql_fetch_row(mysqlresult))!=NULL);
}

void	mysqlcursor::returnRow() {

	for (int col=0; col<ncols; col++) {

		if (mysqlrow[col]) {
			conn->sendField(mysqlrow[col],strlen(mysqlrow[col]));
		} else {
			conn->sendNullField();
		}
	}
}

void	mysqlcursor::cleanUpData() {
	if (mysqlresult!=(MYSQL_RES *)NULL) {
		mysql_free_result(mysqlresult);
		mysqlresult=NULL;
	}
}
