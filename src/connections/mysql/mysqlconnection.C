// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <mysqlconnection.h>
#if defined(MYSQL_VERSION_ID) && MYSQL_VERSION_ID>=32200
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

	// Handle host.
	// For really old versions of mysql, a NULL host indicates that the
	// unix socket should be used.  There's no way to specify what unix
	// socket or inet port to connect to, those values are hardcoded
	// into the client library.
	// For some newer versions, a NULL host causes problems, but an empty
	// string is safe.
#ifdef HAVE_MYSQL_REAL_CONNECT_FOR_SURE
	char	*hostval=(char *)((host && host[0])?host:"");
#else
	char	*hostval=(char *)((host && host[0])?host:NULL);
#endif

	// Handle db.
	char	*dbval=(char *)((db && db[0])?db:"");
	
	// log in
	char	*user=getUser();
	char	*password=getPassword();
#ifdef HAVE_MYSQL_REAL_CONNECT_FOR_SURE
	// Handle port and socket.
	int	portval=(port && port[0])?atoi(port):0;
	char	*socketval=(char *)((socket && socket[0])?socket:NULL);
	#if MYSQL_VERSION_ID>=32200
	// initialize database connection structure
	if (!mysql_init(&mysql)) {
		return 0;
	}
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
#ifdef MYSQL_SELECT_DB
		if (mysql_select_db(&mysql,dbval)) {
			logOut();
			return 0;
		}
#endif
		connected=1;
		return 1;
	}
}

#ifdef HAVE_MYSQL_CHANGE_USER
int	mysqlconnection::changeUser(const char *newuser,
					const char *newpassword) {
	return !mysql_change_user(&mysql,newuser,newpassword,
					(char *)((db && db[0])?db:""));
}
#endif

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

#ifdef HAVE_MYSQL_PING
int	mysqlconnection::ping() {
	if (!mysql_ping(&mysql)) {
		return 1;
	}
	return 0;
}
#endif

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
		if ((queryresult=mysql_real_query(&mysqlconn->mysql,
					newquery->getString(),
					strlen(newquery->getString())))) {
			delete newquery;
			return 0;
		}
		delete newquery;
	} else {
		if ((queryresult=mysql_real_query(&mysqlconn->mysql,
							query,length))) {
			return 0;
		}
	}

	checkForTempTable(query,length);

	// get the affected row count
	affectedrows=mysql_affected_rows(&mysqlconn->mysql);

	// store the result set
	if ((mysqlresult=mysql_store_result(&mysqlconn->mysql))==
						(MYSQL_RES *)NULL) {

		// if there was an error then return failure, otherwise
		// the query must have been some DML or DDL
		char	*err=(char *)mysql_error(&mysqlconn->mysql);
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
	char	*err=(char *)mysql_error(&mysqlconn->mysql);
#if defined(HAVE_MYSQL_CR_SERVER_GONE_ERROR) || \
		defined(HAVE_MYSQL_CR_SERVER_LOST) 
	#ifdef HAVE_MYSQL_CR_SERVER_GONE_ERROR
		if (queryresult==CR_SERVER_GONE_ERROR) {
			*liveconnection=0;
		}
	#endif
	#ifdef HAVE_MYSQL_CR_SERVER_LOST
		if (queryresult==CR_SERVER_LOST) {
			*liveconnection=0;
		}
	#endif
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

	// some useful variables
	int	type;
	int	length;

	// position ourselves at the first field
	mysql_field_seek(mysqlresult,0);

	// for each column...
	for (int i=0; i<ncols; i++) {

		// fetch the field
		mysqlfield=mysql_fetch_field(mysqlresult);

		// append column type to the header
		if (mysqlfield->type==FIELD_TYPE_STRING) {
			type=STRING_DATATYPE;
			length=(int)mysqlfield->length;
		} else if (mysqlfield->type==FIELD_TYPE_VAR_STRING) {
			type=CHAR_DATATYPE;
			length=(int)mysqlfield->length+1;
		} else if (mysqlfield->type==FIELD_TYPE_DECIMAL) {
			type=DECIMAL_DATATYPE;
			if (mysqlfield->decimals>0) {
				length=(int)mysqlfield->length+2;
			} else if (mysqlfield->decimals==0) {
				length=(int)mysqlfield->length+1;
			}
			if (mysqlfield->length<mysqlfield->decimals) {
				length=(int)mysqlfield->decimals+2;
			}
		} else if (mysqlfield->type==FIELD_TYPE_TINY) {
			type=TINYINT_DATATYPE;
			length=1;
		} else if (mysqlfield->type==FIELD_TYPE_SHORT) {
			type=SMALLINT_DATATYPE;
			length=2;
		} else if (mysqlfield->type==FIELD_TYPE_LONG) {
			type=INT_DATATYPE;
			length=4;
		} else if (mysqlfield->type==FIELD_TYPE_FLOAT) {
			type=FLOAT_DATATYPE;
			if (mysqlfield->length<=24) {
				length=4;
			} else {
				length=8;
			}
		} else if (mysqlfield->type==FIELD_TYPE_DOUBLE) {
			type=REAL_DATATYPE;
			length=8;
		} else if (mysqlfield->type==FIELD_TYPE_LONGLONG) {
			type=BIGINT_DATATYPE;
			length=8;
		} else if (mysqlfield->type==FIELD_TYPE_INT24) {
			type=MEDIUMINT_DATATYPE;
			length=3;
		} else if (mysqlfield->type==FIELD_TYPE_TIMESTAMP) {
			type=TIMESTAMP_DATATYPE;
			length=4;
		} else if (mysqlfield->type==FIELD_TYPE_DATE) {
			type=DATE_DATATYPE;
			length=3;
		} else if (mysqlfield->type==FIELD_TYPE_TIME) {
			type=TIME_DATATYPE;
			length=3;
		} else if (mysqlfield->type==FIELD_TYPE_DATETIME) {
			type=DATETIME_DATATYPE;
			length=8;
#ifdef HAVE_MYSQL_FIELD_TYPE_YEAR
		} else if (mysqlfield->type==FIELD_TYPE_YEAR) {
			type=YEAR_DATATYPE;
			length=1;
#endif
#ifdef HAVE_MYSQL_FIELD_TYPE_NEWDATE
		} else if (mysqlfield->type==FIELD_TYPE_NEWDATE) {
			type=NEWDATE_DATATYPE;
			length=1;
#endif
		} else if (mysqlfield->type==FIELD_TYPE_NULL) {
			type=NULL_DATATYPE;
#ifdef HAVE_MYSQL_FIELD_TYPE_ENUM
		} else if (mysqlfield->type==FIELD_TYPE_ENUM) {
			type=ENUM_DATATYPE;
			// 1 or 2 bytes delepending on the # of enum values
			// (65535 max)
			length=2;
#endif
#ifdef HAVE_MYSQL_FIELD_TYPE_SET
		} else if (mysqlfield->type==FIELD_TYPE_SET) {
			type=SET_DATATYPE;
			// 1,2,3,4 or 8 bytes delepending on the # of members
			// (64 max)
			length=8;
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
			length=(int)mysqlfield->length+2;
		} else if (mysqlfield->type==FIELD_TYPE_MEDIUM_BLOB) {
			type=MEDIUM_BLOB_DATATYPE;
			length=(int)mysqlfield->length+3;
		} else if (mysqlfield->type==FIELD_TYPE_LONG_BLOB) {
			type=LONG_BLOB_DATATYPE;
			length=(int)mysqlfield->length+4;
		} else if (mysqlfield->type==FIELD_TYPE_BLOB) {
			if ((int)mysqlfield->length==255) {
				type=TINY_BLOB_DATATYPE;
				length=(int)mysqlfield->length+2;
			} else {
				type=BLOB_DATATYPE;
				length=(int)mysqlfield->length+3;
			}
		} else {
			type=UNKNOWN_DATATYPE;
			length=(int)mysqlfield->length;
		}

		// send column definition
		// for mysql, length is actually precision
		conn->sendColumnDefinition(mysqlfield->name,
					strlen(mysqlfield->name),
					type,length,
					mysqlfield->length,
					mysqlfield->decimals,
					!(IS_NOT_NULL(mysqlfield->flags)),
					IS_PRI_KEY(mysqlfield->flags),
					mysqlfield->flags&UNIQUE_KEY_FLAG,
					mysqlfield->flags&MULTIPLE_KEY_FLAG,
					mysqlfield->flags&UNSIGNED_FLAG,
					mysqlfield->flags&ZEROFILL_FLAG,
					mysqlfield->flags&BINARY_FLAG,
					mysqlfield->flags&AUTO_INCREMENT_FLAG);
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

void	mysqlcursor::cleanUpData(bool freerows, bool freecols,
							bool freebinds) {
	if (freerows && mysqlresult!=(MYSQL_RES *)NULL) {
		mysql_free_result(mysqlresult);
		mysqlresult=NULL;
	}
}
