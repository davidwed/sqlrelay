// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

bool sqlrconnection_svr::supportsAuthOnDatabase() {
	return true;
}

bool sqlrconnection_svr::changeUser(const char *newuser,
					const char *newpassword) {
	int32_t	oldcursorcount=cursorcount;
	closeCursors(false);
	logOutInternal();
	setUser(newuser);
	setPassword(newpassword);
	return (logInInternal(false) && initCursors(cursorcount));
}

bool sqlrconnection_svr::autoCommitOn() {
	fakeautocommit=true;
	autocommit=true;
	return true;
}

bool sqlrconnection_svr::autoCommitOff() {
	fakeautocommit=true;
	autocommit=false;
	return true;
}

bool sqlrconnection_svr::isTransactional() {
	return true;
}

bool sqlrconnection_svr::supportsTransactionBlocks() {
	return true;
}

bool sqlrconnection_svr::begin() {

	// re-init error data
	clearError();

	// for db's that don't support begin queries,
	// don't do anything, just return true
	if (!supportsTransactionBlocks()) {
		return true;
	}

	// for db's that support begin queries, run one...

	// init some variables
	sqlrcursor_svr	*begincur=initCursorInternal();
	const char	*beginquery=beginTransactionQuery();
	int		beginquerylen=charstring::length(beginquery);
	bool		retval=false;

	// run the query...
	// since we're creating a new cursor for this, make sure it can't
	// have an ID that might already exist
	if (begincur->openInternal(cursorcount+1) &&
		begincur->prepareQuery(beginquery,beginquerylen)) {
		retval=executeQueryInternal(begincur,beginquery,beginquerylen);
	}

	// If there was an error, copy it out.  We'll be destroying the
	// cursor in a moment and the error will be lost otherwise.
	if (!retval) {
		begincur->errorMessage(error,maxerrorlength,
					&errorlength,&errnum,&liveconnection);
	}

	// clean up
	begincur->cleanUpData(true,true);
	begincur->close();
	deleteCursorInternal(begincur);

	// we will need to commit or rollback at the end of the session now
	if (retval) {
		commitorrollback=true;
	}

	return retval;
}

const char *sqlrconnection_svr::beginTransactionQuery() {
	return "BEGIN";
}

bool sqlrconnection_svr::commit() {

	// re-init error data
	clearError();

	// init some variables
	sqlrcursor_svr	*commitcur=initCursorInternal();
	const char	*commitquery="commit";
	int		commitquerylen=6;
	bool		retval=false;

	// run the query...
	// since we're creating a new cursor for this, make sure it can't
	// have an ID that might already exist
	if (commitcur->openInternal(cursorcount+1) &&
		commitcur->prepareQuery(commitquery,commitquerylen)) {
		retval=executeQueryInternal(commitcur,commitquery,
							commitquerylen);
	}

	// If there was an error, copy it out.  We'll be destroying the
	// cursor in a moment and the error will be lost otherwise.
	if (!retval) {
		commitcur->errorMessage(error,maxerrorlength,
					&errorlength,&errnum,&liveconnection);
	}

	// clean up
	commitcur->cleanUpData(true,true);
	commitcur->close();
	deleteCursorInternal(commitcur);

	// we don't need to commit or rollback at the end of the session now
	if (retval) {
		commitorrollback=false;
	}

	return retval;
}

bool sqlrconnection_svr::rollback() {

	// re-init error data
	clearError();

	// init some variables
	sqlrcursor_svr	*rollbackcur=initCursorInternal();
	const char	*rollbackquery="rollback";
	int		rollbackquerylen=8;
	bool		retval=false;

	// run the query...
	// since we're creating a new cursor for this, make sure it can't
	// have an ID that might already exist
	if (rollbackcur->openInternal(cursorcount+1) &&
		rollbackcur->prepareQuery(rollbackquery,rollbackquerylen)) {
		retval=executeQueryInternal(rollbackcur,rollbackquery,
							rollbackquerylen);
	}

	// If there was an error, copy it out.  We'll be destroying the
	// cursor in a moment and the error will be lost otherwise.
	if (!retval) {
		rollbackcur->errorMessage(error,maxerrorlength,
					&errorlength,&errnum,&liveconnection);
	}

	// clean up
	rollbackcur->cleanUpData(true,true);
	rollbackcur->close();
	deleteCursorInternal(rollbackcur);

	// we don't need to commit or rollback at the end of the session now
	if (retval) {
		commitorrollback=false;
	}

	return retval;
}

bool sqlrconnection_svr::selectDatabase(const char *database) {

	// re-init error data
	clearError();

	// handle the degenerate case
	if (!database) {
		return true;
	}

	// get the select database query base
	const char	*sdquerybase=selectDatabaseQuery();

	// If there is no query for this then the db we're using doesn't
	// support switching.  Return true as if it succeeded though.
	if (!sdquerybase) {
		return true;
	}

	// bounds checking
	size_t		sdquerylen=charstring::length(sdquerybase)+
					charstring::length(database)+1;
	if (sdquerylen>maxquerysize) {
		return false;
	}

	// create the select database query
	char	*sdquery=new char[sdquerylen];
	snprintf(sdquery,sdquerylen,sdquerybase,database);
	sdquerylen=charstring::length(sdquery);

	sqlrcursor_svr	*sdcur=initCursorInternal();
	// since we're creating a new cursor for this, make sure it can't
	// have an ID that might already exist
	bool	retval=false;
	if (sdcur->openInternal(cursorcount+1) &&
		sdcur->prepareQuery(sdquery,sdquerylen) &&
		executeQueryInternal(sdcur,sdquery,sdquerylen)) {
		sdcur->cleanUpData(true,true);
		retval=true;

		// set a flag indicating that the db has been changed
		// so it can be reset at the end of the session
		dbselected=true;
	} else {
		// If there was an error, copy it out.  We'l be destroying the
		// cursor in a moment and the error will be lost otherwise.
		sdcur->errorMessage(error,maxerrorlength,
					&errorlength,&errnum,&liveconnection);
	}
	delete[] sdquery;
	sdcur->close();
	deleteCursorInternal(sdcur);
	return retval;
}

const char *sqlrconnection_svr::selectDatabaseQuery() {
	return NULL;
}

char *sqlrconnection_svr::getCurrentDatabase() {

	// get the get current database query base
	const char	*gcdquery=getCurrentDatabaseQuery();

	// If there is no query for this then the db we're using doesn't
	// support switching.
	if (!gcdquery) {
		return NULL;
	}

	size_t		gcdquerylen=charstring::length(gcdquery);

	sqlrcursor_svr	*gcdcur=initCursorInternal();
	// since we're creating a new cursor for this, make sure it can't
	// have an ID that might already exist
	char	*retval=NULL;
	if (gcdcur->openInternal(cursorcount+1) &&
		gcdcur->prepareQuery(gcdquery,gcdquerylen) &&
		executeQueryInternal(gcdcur,gcdquery,gcdquerylen)) {

		if (!gcdcur->noRowsToReturn() && gcdcur->fetchRow()) {

			// get the first field of the row and return it
			const char	*field=NULL;
			uint64_t	fieldlength=0;
			bool		blob=false;
			bool		null=false;
			gcdcur->getField(0,&field,&fieldlength,&blob,&null);
			retval=charstring::duplicate(field);
		} 
	}
	gcdcur->cleanUpData(true,true);
	gcdcur->close();
	deleteCursorInternal(gcdcur);
	return retval;
}

const char *sqlrconnection_svr::getCurrentDatabaseQuery() {
	return NULL;
}

bool sqlrconnection_svr::getLastInsertId(uint64_t *id) {

	// re-init error data
	clearError();

	// get the get current database query base
	const char	*liiquery=getLastInsertIdQuery();

	// If there is no query for this then the db we're using doesn't
	// support switching.
	if (!liiquery) {
		error=charstring::duplicate(
				"get last insert id not supported");
		return false;
	}

	size_t	liiquerylen=charstring::length(liiquery);

	sqlrcursor_svr	*liicur=initCursorInternal();
	// since we're creating a new cursor for this, make sure it can't
	// have an ID that might already exist
	bool	retval=false;
	if (liicur->openInternal(cursorcount+1) &&
		liicur->prepareQuery(liiquery,liiquerylen) &&
		executeQueryInternal(liicur,liiquery,liiquerylen)) {

		if (!liicur->noRowsToReturn() && liicur->fetchRow()) {

			// get the first field of the row and return it
			const char	*field=NULL;
			uint64_t	fieldlength=0;
			bool		blob=false;
			bool		null=false;
			liicur->getField(0,&field,&fieldlength,&blob,&null);
			*id=charstring::toInteger(field);
			retval=true;

		}  else {

			error=charstring::duplicate("no values returned");
			retval=false;
		}

	} else {
		// If there was an error, copy it out.  We'l be destroying the
		// cursor in a moment and the error will be lost otherwise.
		liicur->errorMessage(error,maxerrorlength,
					&errorlength,&errnum,&liveconnection);
	}

	liicur->cleanUpData(true,true);
	liicur->close();
	deleteCursorInternal(liicur);
	return retval;
}

const char *sqlrconnection_svr::getLastInsertIdQuery() {
	return NULL;
}

bool sqlrconnection_svr::setIsolationLevel(const char *isolevel) {

	// if no isolation level was passed in then bail
	if (!charstring::length(isolevel)) {
		return false;
	}

	// get the set isolation level query base
	const char	*silquerybase=setIsolationLevelQuery();

	// If there is no query for this then the db we're using doesn't
	// support switching.  Return true as if it succeeded though.
	if (!charstring::length(silquerybase)) {
		return true;
	}

	// bounds checking
	size_t		silquerylen=charstring::length(silquerybase)+
					charstring::length(isolevel)+1;
	if (silquerylen>maxquerysize) {
		return false;
	}

	// create the set isolation level query
	char	*silquery=new char[silquerylen];
	snprintf(silquery,silquerylen,silquerybase,isolevel);
	silquerylen=charstring::length(silquery);

	sqlrcursor_svr	*silcur=initCursorInternal();
	// since we're creating a new cursor for this, make sure it can't
	// have an ID that might already exist
	bool	retval=false;
	if (silcur->openInternal(cursorcount+1) &&
		silcur->prepareQuery(silquery,silquerylen) &&
		executeQueryInternal(silcur,silquery,silquerylen)) {
		retval=true;
	}

	// FIXME: we don't really need to do this now but we will
	// later if we ever add an API call to set the isolation level
	/* else {
		silcur->errorMessage(error,maxerrorlength,
					&errorlength,&errnum,&liveconnection));
	} */

	delete[] silquery;
	silcur->cleanUpData(true,true);
	silcur->close();
	deleteCursorInternal(silcur);
	return retval;
}

const char *sqlrconnection_svr::setIsolationLevelQuery() {
	return "set transaction isolation level %s";
}

bool sqlrconnection_svr::ping() {
	sqlrcursor_svr	*pingcur=initCursorInternal();
	const char	*pingquery=pingQuery();
	int		pingquerylen=charstring::length(pingquery);
	// since we're creating a new cursor for this, make sure it can't
	// have an ID that might already exist
	if (pingcur->openInternal(cursorcount+1) &&
		pingcur->prepareQuery(pingquery,pingquerylen) &&
		executeQueryInternal(pingcur,pingquery,pingquerylen)) {
		pingcur->cleanUpData(true,true);
		pingcur->close();
		deleteCursorInternal(pingcur);
		return true;
	}
	pingcur->close();
	deleteCursorInternal(pingcur);
	return false;
}

const char *sqlrconnection_svr::pingQuery() {
	return "select 1";
}

bool sqlrconnection_svr::getListsByApiCalls() {
	return false;
}

bool sqlrconnection_svr::getDatabaseList(sqlrcursor_svr *cursor,
						const char *wild) {
	return false;
}

bool sqlrconnection_svr::getTableList(sqlrcursor_svr *cursor,
						const char *wild) {
	return false;
}

bool sqlrconnection_svr::getColumnList(sqlrcursor_svr *cursor,
						const char *table,
						const char *wild) {
	return false;
}

const char *sqlrconnection_svr::getDatabaseListQuery(bool wild) {
	return "select 1";
}

const char *sqlrconnection_svr::getTableListQuery(bool wild) {
	return "select 1";
}

const char *sqlrconnection_svr::getColumnListQuery(bool wild) {
	return "select 1";
}

const char *sqlrconnection_svr::bindFormat() {
	return ":*";
}

int16_t sqlrconnection_svr::nonNullBindValue() {
	return 0;
}

int16_t sqlrconnection_svr::nullBindValue() {
	return -1;
}

char sqlrconnection_svr::bindVariablePrefix() {
	return ':';
}

bool sqlrconnection_svr::bindValueIsNull(int16_t isnull) {
	return (isnull==nullBindValue());
}

const char *sqlrconnection_svr::tempTableDropPrefix() {
	return "";
}

bool sqlrconnection_svr::tempTableDropReLogIn() {
	return false;
}

void sqlrconnection_svr::endSession() {
	// by default, do nothing
}

sqlwriter *sqlrconnection_svr::getSqlWriter() {
	return new sqlwriter;
}

sqltranslations *sqlrconnection_svr::getSqlTranslations() {
	return new sqltranslations;
}
