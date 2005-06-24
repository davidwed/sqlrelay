// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <msqlconnection.h>

#include <datatypes.h>

msqlconnection::msqlconnection() {
	devnull=-2;
}

uint16_t msqlconnection::getNumberOfConnectStringVars() {
	return NUM_CONNECT_STRING_VARS;
}

void msqlconnection::handleConnectString() {
	host=connectStringValue("host");
	db=connectStringValue("db");
}

bool msqlconnection::logIn() {

	// handle db
	const char	*dbval;
	if (db && db[0]) {
		dbval=db;
	} else {
		dbval="";
	}

	if ((msql=msqlConnect(const_cast<char *>(host)))==-1) {
		return false;
	}

	if (msqlSelectDB(msql,const_cast<char *>(dbval))==-1) {
		logOut();
		return false;
	}
	return true;
}

sqlrcursor *msqlconnection::initCursor() {
	return (sqlrcursor *)new msqlcursor((sqlrconnection *)this);
}

void msqlconnection::deleteCursor(sqlrcursor *curs) {
	delete (msqlcursor *)curs;
}

void msqlconnection::logOut() {
	msqlClose(msql);
}

bool msqlconnection::ping() {
	// if we don't redirect stdout/stderr, 
	// msqlGetServerStats will spew out data
	/*if (devnull==-2 && (devnull=open("/dev/null",O_RDONLY))>0) {
		dup2(devnull,STDOUT_FILENO);
		dup2(devnull,STDERR_FILENO);
	}
	if (msqlGetServerStats(msql)==-1) {
		return false;
	}*/
	return true;
}

const char *msqlconnection::identify() {
	return "msql";
}

bool msqlconnection::isTransactional() {
	return false;
}

bool msqlconnection::autoCommitOn() {
	// do nothing
	return true;
}

bool msqlconnection::autoCommitOff() {
	// do nothing
	return true;
}

bool msqlconnection::commit() {
	// do nothing
	return true;
}

bool msqlconnection::rollback() {
	// do nothing
	return true;
}

msqlcursor::msqlcursor(sqlrconnection *conn) : sqlrcursor(conn) {
	msqlconn=(msqlconnection *)conn;
	msqlresult=NULL;
}

bool msqlcursor::executeQuery(const char *query, uint32_t length,
							bool execute) {

	// initialize return values
	ncols=0;
	nrows=0;
	msqlresult=(m_result *)NULL;

	// fake binds
	stringbuffer	*newquery=fakeInputBinds(query);

	// execute the query
	if (newquery) {
		if (msqlQuery(msqlconn->msql,
			const_cast<char *>(newquery->getString()))==-1) {
			delete newquery;
			return false;
		}
		delete newquery;
	} else {
		if (msqlQuery(msqlconn->msql,const_cast<char *>(query))==-1) {
			return false;
		}
	}

	// store the result set
	if ((msqlresult=msqlStoreResult())==(m_result *)NULL) {

		// if no error message was generated then the query
		// must have been some DDL or DML, have to check
		// the 0'th array element here, it's kinda weird
		if (msqlErrMsg[0]) {
			return false;
		} else {
			return true;
		}
	}

	// get the column count
	ncols=msqlNumFields(msqlresult);

	// get the row count
	nrows=msqlNumRows(msqlresult);

	return true;
}

const char *msqlcursor::getErrorMessage(bool *liveconnection) {

	*liveconnection=true;
	if (!charstring::compareIgnoringCase(msqlErrMsg,
					"msql server has gone away")) {
		*liveconnection=false;
	}
	return msqlErrMsg;
}

void msqlcursor::returnRowCounts() {

	// send row counts (affected row count unknown in msql)
	conn->sendRowCounts(true,nrows,false,0);
}

void msqlcursor::returnColumnCount() {
	conn->sendColumnCount(ncols);
}

void msqlcursor::returnColumnInfo() {

	conn->sendColumnTypeFormat(COLUMN_TYPE_IDS);

	// for dml/ddl queries, return no header
	if (!msqlresult) {
		return;
	}

	// some useful variables
	uint16_t	type;
	uint32_t	precision;
	uint32_t	scale;

	// position ourselves on the first field
	msqlFieldSeek(msqlresult,0);

	// for each column...
	for (int32_t i=0; i<ncols; i++) {

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
		// For some reason, msql reports time datatypes as
		// "lastreal" yet lastreal is not a valid msql datatype.
		// Strange.
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
					charstring::length(msqlfield->name),
					type,msqlfield->length,
					precision,scale,
					!(IS_NOT_NULL(msqlfield->flags)),
					0,IS_UNIQUE(msqlfield->flags),
					0,(type==UINT_DATATYPE),0,0,0);
	}
}

bool msqlcursor::noRowsToReturn() {
	// for dml/ddl queries, return no data
	return (!msqlresult);
}

bool msqlcursor::skipRow() {
	return fetchRow();
}

bool msqlcursor::fetchRow() {
	return ((msqlrow=msqlFetchRow(msqlresult))!=NULL);
}

void msqlcursor::returnRow() {

	for (int32_t col=0; col<ncols; col++) {

		if (msqlrow[col]) {
			conn->sendField(msqlrow[col],
					charstring::length(msqlrow[col]));
		} else {
			conn->sendNullField();
		}
	}
}

void msqlcursor::cleanUpData(bool freeresult, bool freebinds) {

	if (freeresult && msqlresult) {
		msqlFreeResult(msqlresult);
		msqlresult=NULL;
	}
}
