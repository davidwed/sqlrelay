// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <msqlconnection.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <datatypes.h>

msqlconnection::msqlconnection() {
	devnull=-2;
}

int	msqlconnection::getNumberOfConnectStringVars() {
	return NUM_CONNECT_STRING_VARS;
}

void	msqlconnection::handleConnectString() {
	host=connectStringValue("host");
	db=connectStringValue("db");
}

int	msqlconnection::logIn() {

	// handle db
	char	*dbval;
	if (db && db[0]) {
		dbval=db;
	} else {
		dbval="";
	}

	if ((msql=msqlConnect(host))==-1) {
		return 0;
	}

	if (msqlSelectDB(msql,dbval)==-1) {
		logOut();
		return 0;
	} else {
		return 1;
	}
}

sqlrcursor	*msqlconnection::initCursor() {
	return (sqlrcursor *)new msqlcursor((sqlrconnection *)this);
}

void	msqlconnection::deleteCursor(sqlrcursor *curs) {
	delete (msqlcursor *)curs;
}

void	msqlconnection::logOut() {
	msqlClose(msql);
}

int	msqlconnection::ping() {
	// if we don't redirect stdout/stderr, 
	// msqlGetServerStats will spew out data
	/*if (devnull==-2 && (devnull=open("/dev/null",O_RDONLY))>0) {
		dup2(devnull,STDOUT_FILENO);
		dup2(devnull,STDERR_FILENO);
	}
	if (msqlGetServerStats(msql)==-1) {
		return 0;
	}*/
	return 1;
}

char	*msqlconnection::identify() {
	return "msql";
}

int	msqlconnection::isTransactional() {
	return 0;
}

unsigned short	msqlconnection::autoCommitOn() {
	// do nothing
	return 1;
}

unsigned short	msqlconnection::autoCommitOff() {
	// do nothing
	return 1;
}

int	msqlconnection::commit() {
	// do nothing
	return 1;
}

int	msqlconnection::rollback() {
	// do nothing
	return 1;
}

msqlcursor::msqlcursor(sqlrconnection *conn) : sqlrcursor(conn) {
	msqlconn=(msqlconnection *)conn;
	msqlresult=NULL;
}

int	msqlcursor::executeQuery(const char *query, long length,
					unsigned short execute) {

	// initialize return values
	ncols=0;
	nrows=0;
	msqlresult=(m_result *)NULL;

	// fake binds
	stringbuffer	*newquery=fakeInputBinds(query);

	// execute the query
	if (newquery) {
		if (msqlQuery(msqlconn->msql,newquery->getString())==-1) {
			delete newquery;
			return 0;
		}
		delete newquery;
	} else {
		if (msqlQuery(msqlconn->msql,(char *)query)==-1) {
			return 0;
		}
	}

	// store the result set
	if ((msqlresult=msqlStoreResult())==(m_result *)NULL) {

		// if no error message was generated then the query
		// must have been some DDL or DML, have to check
		// the 0'th array element here, it's kinda weird
		if (msqlErrMsg[0]) {
			return 0;
		} else {
			return 1;
		}
	}

	// get the column count
	ncols=msqlNumFields(msqlresult);

	// get the row count
	nrows=msqlNumRows(msqlresult);

	return 1;
}

char	*msqlcursor::getErrorMessage(int *liveconnection) {

	*liveconnection=1;

	return msqlErrMsg;
}

void	msqlcursor::returnRowCounts() {

	// send row counts (affected row count unknown in msql)
	conn->sendRowCounts((long)nrows,(long)-1);
}

void	msqlcursor::returnColumnCount() {
	conn->sendColumnCount(ncols);
}

void	msqlcursor::returnColumnInfo() {

	conn->sendColumnTypeFormat(COLUMN_TYPE_IDS);

	// for dml/ddl queries, return no header
	if (!msqlresult) {
		return;
	}

	// some useful variables
	int	type;
	int	precision;
	int	scale;

	// position ourselves on the first field
	msqlFieldSeek(msqlresult,0);

	// for each column...
	for (int i=0; i<ncols; i++) {

		// fetch the field
		msqlfield=msqlFetchField(msqlresult);

		// initialize precision
		precision=0;
		scale=0;

		// append column type to the header
		if (msqlfield->type==CHAR_TYPE) {
			type=CHAR_DATATYPE;
			precision=msqlfield->length;
		} else if (msqlfield->type==TEXT_TYPE) {
			type=TEXT_DATATYPE;
			precision=msqlfield->length;
		} else if (msqlfield->type==INT_TYPE) {
			type=INT_DATATYPE;
			precision=10;
		} else if (msqlfield->type==UINT_TYPE) {
			type=UINT_DATATYPE;
			precision=10;
		} else if (msqlfield->type==MONEY_TYPE) {
			type=MONEY_DATATYPE;
			precision=12;
			scale=2;
		// For some reason, msql reports time datatypes as "lastreal"
		// yet lastreal is not a valid msql datatype.  Strange.
		} else if (msqlfield->type==LAST_REAL_TYPE) {
			type=TIME_DATATYPE;
			precision=8;
		} else if (msqlfield->type==REAL_TYPE) {
			type=REAL_DATATYPE;
		} else if (msqlfield->type==DATE_TYPE) {
			type=DATE_DATATYPE;
			precision=11;
		} else if (msqlfield->type==TIME_TYPE) {
			type=TIME_DATATYPE;
			precision=8;
		} else {
			type=UNKNOWN_DATATYPE;
		}

		// send the column definition
		conn->sendColumnDefinition(msqlfield->name,
					strlen(msqlfield->name),
					type,msqlfield->length,
					precision,scale,
					!(IS_NOT_NULL(msqlfield->flags)),
					0,IS_UNIQUE(msqlfield->flags));
	}
}

int	msqlcursor::noRowsToReturn() {

	// for dml/ddl queries, return no data
	if (!msqlresult) {
		return 1;
	}
	return 0;
}

int	msqlcursor::skipRow() {
	return fetchRow();
}

int	msqlcursor::fetchRow() {
	return (msqlrow=msqlFetchRow(msqlresult))!=NULL;
}

void	msqlcursor::returnRow() {

	for (int col=0; col<ncols; col++) {

		if (msqlrow[col]) {
			conn->sendField(msqlrow[col],strlen(msqlrow[col]));
		} else {
			conn->sendNullField();
		}
	}
}


void	msqlcursor::cleanUpData(bool freerows, bool freecols,
							bool freebinds) {

	if (freerows && msqlresult) {
		msqlFreeResult(msqlresult);
		msqlresult=NULL;
	}
}
