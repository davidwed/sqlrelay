// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrcontroller.h>
#include <sqlrconnection.h>
#include <rudiments/hostentry.h>

using namespace rudiments;

sqlrconnection_svr::sqlrconnection_svr(sqlrcontroller_svr *cont) {
	this->cont=cont;

	error=NULL;
	errorlength=0;
	errnum=0;
	liveconnection=false;

	autocommit=false;
	fakeautocommit=false;

	dbhostname=NULL;
	dbipaddress=NULL;
	dbhostiploop=0;
}

sqlrconnection_svr::~sqlrconnection_svr() {
	delete[] error;
	delete[] dbhostname;
	delete[] dbipaddress;
}

bool sqlrconnection_svr::supportsAuthOnDatabase() {
	return true;
}

bool sqlrconnection_svr::changeUser(const char *newuser,
					const char *newpassword) {
	cont->closeCursors(false);
	cont->logOut();
	cont->setUser(newuser);
	cont->setPassword(newpassword);
	return (cont->logIn(false) && cont->initCursors(cont->cursorcount));
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
	sqlrcursor_svr	*begincur=cont->initCursor();
	const char	*beginquery=beginTransactionQuery();
	int		beginquerylen=charstring::length(beginquery);
	bool		retval=false;

	// run the query...
	// since we're creating a new cursor for this, make sure it can't
	// have an ID that might already exist
	if (begincur->openInternal(cont->cursorcount+1) &&
		begincur->prepareQuery(beginquery,beginquerylen)) {
		retval=begincur->executeQuery(beginquery,beginquerylen);
	}

	// If there was an error, copy it out.  We'll be destroying the
	// cursor in a moment and the error will be lost otherwise.
	if (!retval) {
		begincur->errorMessage(error,cont->maxerrorlength,
					&errorlength,&errnum,&liveconnection);
	}

	// clean up
	begincur->cleanUpData();
	begincur->close();
	cont->deleteCursor(begincur);

	// we will need to commit or rollback at the end of the session now
	if (retval) {
		cont->commitorrollback=true;
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
	sqlrcursor_svr	*commitcur=cont->initCursor();
	const char	*commitquery="commit";
	int		commitquerylen=6;
	bool		retval=false;

	// run the query...
	// since we're creating a new cursor for this, make sure it can't
	// have an ID that might already exist
	if (commitcur->openInternal(cont->cursorcount+1) &&
		commitcur->prepareQuery(commitquery,commitquerylen)) {
		retval=commitcur->executeQuery(commitquery,commitquerylen);
	}

	// If there was an error, copy it out.  We'll be destroying the
	// cursor in a moment and the error will be lost otherwise.
	if (!retval) {
		commitcur->errorMessage(error,cont->maxerrorlength,
					&errorlength,&errnum,&liveconnection);
	}

	// clean up
	commitcur->cleanUpData();
	commitcur->close();
	cont->deleteCursor(commitcur);

	// we don't need to commit or rollback at the end of the session now
	if (retval) {
		cont->commitorrollback=false;
	}

	return retval;
}

bool sqlrconnection_svr::rollback() {

	// re-init error data
	clearError();

	// init some variables
	sqlrcursor_svr	*rbcur=cont->initCursor();
	const char	*rollbackquery="rollback";
	int		rollbackquerylen=8;
	bool		retval=false;

	// run the query...
	// since we're creating a new cursor for this, make sure it can't
	// have an ID that might already exist
	if (rbcur->openInternal(cont->cursorcount+1) &&
		rbcur->prepareQuery(rollbackquery,rollbackquerylen)) {
		retval=rbcur->executeQuery(rollbackquery,rollbackquerylen);
	}

	// If there was an error, copy it out.  We'll be destroying the
	// cursor in a moment and the error will be lost otherwise.
	if (!retval) {
		rbcur->errorMessage(error,cont->maxerrorlength,
					&errorlength,&errnum,&liveconnection);
	}

	// clean up
	rbcur->cleanUpData();
	rbcur->close();
	cont->deleteCursor(rbcur);

	// we don't need to commit or rollback at the end of the session now
	if (retval) {
		cont->commitorrollback=false;
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
	if (sdquerylen>cont->maxquerysize) {
		return false;
	}

	// create the select database query
	char	*sdquery=new char[sdquerylen];
	snprintf(sdquery,sdquerylen,sdquerybase,database);
	sdquerylen=charstring::length(sdquery);

	sqlrcursor_svr	*sdcur=cont->initCursor();
	// since we're creating a new cursor for this, make sure it can't
	// have an ID that might already exist
	bool	retval=false;
	if (sdcur->openInternal(cont->cursorcount+1) &&
		sdcur->prepareQuery(sdquery,sdquerylen) &&
		sdcur->executeQuery(sdquery,sdquerylen)) {
		sdcur->cleanUpData();
		retval=true;

		// set a flag indicating that the db has been changed
		// so it can be reset at the end of the session
		cont->dbselected=true;
	} else {
		// If there was an error, copy it out.  We'l be destroying the
		// cursor in a moment and the error will be lost otherwise.
		sdcur->errorMessage(error,cont->maxerrorlength,
					&errorlength,&errnum,&liveconnection);
	}
	delete[] sdquery;
	sdcur->close();
	cont->deleteCursor(sdcur);
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

	sqlrcursor_svr	*gcdcur=cont->initCursor();
	// since we're creating a new cursor for this, make sure it can't
	// have an ID that might already exist
	char	*retval=NULL;
	if (gcdcur->openInternal(cont->cursorcount+1) &&
		gcdcur->prepareQuery(gcdquery,gcdquerylen) &&
		gcdcur->executeQuery(gcdquery,gcdquerylen)) {

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
	gcdcur->cleanUpData();
	gcdcur->close();
	cont->deleteCursor(gcdcur);
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

	sqlrcursor_svr	*liicur=cont->initCursor();
	// since we're creating a new cursor for this, make sure it can't
	// have an ID that might already exist
	bool	retval=false;
	if (liicur->openInternal(cont->cursorcount+1) &&
		liicur->prepareQuery(liiquery,liiquerylen) &&
		liicur->executeQuery(liiquery,liiquerylen)) {

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
		liicur->errorMessage(error,cont->maxerrorlength,
					&errorlength,&errnum,&liveconnection);
	}

	liicur->cleanUpData();
	liicur->close();
	cont->deleteCursor(liicur);
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
	if (silquerylen>cont->maxquerysize) {
		return false;
	}

	// create the set isolation level query
	char	*silquery=new char[silquerylen];
	snprintf(silquery,silquerylen,silquerybase,isolevel);
	silquerylen=charstring::length(silquery);

	sqlrcursor_svr	*silcur=cont->initCursor();
	// since we're creating a new cursor for this, make sure it can't
	// have an ID that might already exist
	bool	retval=false;
	if (silcur->openInternal(cont->cursorcount+1) &&
		silcur->prepareQuery(silquery,silquerylen) &&
		silcur->executeQuery(silquery,silquerylen)) {
		retval=true;
	}

	// FIXME: we don't really need to do this now but we will
	// later if we ever add an API call to set the isolation level
	/* else {
		silcur->errorMessage(error,cont->maxerrorlength,
					&errorlength,&errnum,&liveconnection));
	} */

	delete[] silquery;
	silcur->cleanUpData();
	silcur->close();
	cont->deleteCursor(silcur);
	return retval;
}

const char *sqlrconnection_svr::setIsolationLevelQuery() {
	return "set transaction isolation level %s";
}

bool sqlrconnection_svr::ping() {
	sqlrcursor_svr	*pingcur=cont->initCursor();
	const char	*pingquery=pingQuery();
	int		pingquerylen=charstring::length(pingquery);
	// since we're creating a new cursor for this, make sure it can't
	// have an ID that might already exist
	if (pingcur->openInternal(cont->cursorcount+1) &&
		pingcur->prepareQuery(pingquery,pingquerylen) &&
		pingcur->executeQuery(pingquery,pingquerylen)) {
		pingcur->cleanUpData();
		pingcur->close();
		cont->deleteCursor(pingcur);
		return true;
	}
	pingcur->close();
	cont->deleteCursor(pingcur);
	return false;
}

const char *sqlrconnection_svr::pingQuery() {
	return "select 1";
}

const char *sqlrconnection_svr::dbHostNameQuery() {
	return NULL;
}

const char *sqlrconnection_svr::dbIpAddressQuery() {
	return NULL;
}

const char *sqlrconnection_svr::dbHostName() {

	// don't get looped up...
	if (dbhostiploop==2) {
		return NULL;
	}
	dbhostiploop++;

	// re-init buffer
	delete[] dbhostname;
	dbhostname=NULL;

	// if we have a host name query then use it, otherwise get the
	// ip address and convert it to a host name...

	const char	*dbhnquery=dbHostNameQuery();
	if (dbhnquery) {

		sqlrcursor_svr	*dbhncur=cont->initCursor();
		int		dbhnquerylen=charstring::length(dbhnquery);
		// since we're creating a new cursor for this, make
		// sure it can't have an ID that might already exist
		if (dbhncur->openInternal(cont->cursorcount+1) &&
			dbhncur->prepareQuery(dbhnquery,dbhnquerylen) &&
			dbhncur->executeQuery(dbhnquery,dbhnquerylen)) {

			if (!dbhncur->noRowsToReturn() && dbhncur->fetchRow()) {
				const char	*field=NULL;
				uint64_t	fieldlength=0;
				bool		blob=false;
				bool		null=false;
				dbhncur->getField(0,&field,&fieldlength,
								&blob,&null);
				dbhostname=charstring::duplicate(field);
			} 
		
			dbhncur->cleanUpData();
		}
		dbhncur->close();
		cont->deleteCursor(dbhncur);

	} else {

		const char	*ipaddr=dbIpAddress();
		char		ip[4];
		for (uint8_t i=0; i<4; i++) {
			ip[i]=charstring::toInteger(ipaddr);
			ipaddr=charstring::findFirst(ipaddr,'.');
			if (ipaddr) {
				ipaddr++;
			}
		}
		hostentry::getName(ip,4,AF_INET,&dbhostname);
	}
	dbhostiploop=0;
	return dbhostname;
}

const char *sqlrconnection_svr::dbIpAddress() {

	// don't get looped up...
	if (dbhostiploop==2) {
		return NULL;
	}
	dbhostiploop++;

	// re-init buffer
	delete[] dbipaddress;
	dbipaddress=NULL;

	// if we have an ip address query then use it, otherwise get the
	// host name and convert it to an ip address...

	const char	*dbiaquery=dbIpAddressQuery();
	if (dbiaquery) {

		sqlrcursor_svr	*dbiacur=cont->initCursor();
		int		dbiaquerylen=charstring::length(dbiaquery);
		// since we're creating a new cursor for this, make
		// sure it can't have an ID that might already exist
		if (dbiacur->openInternal(cont->cursorcount+1) &&
			dbiacur->prepareQuery(dbiaquery,dbiaquerylen) &&
			dbiacur->executeQuery(dbiaquery,dbiaquerylen)) {

			if (!dbiacur->noRowsToReturn() && dbiacur->fetchRow()) {
				const char	*field=NULL;
				uint64_t	fieldlength=0;
				bool		blob=false;
				bool		null=false;
				dbiacur->getField(0,&field,&fieldlength,
								&blob,&null);
				dbipaddress=charstring::duplicate(field);
			} 
		
			dbiacur->cleanUpData();
		}
		dbiacur->close();
		cont->deleteCursor(dbiacur);

	} else {
		hostentry::getAddressString(dbHostName(),0,&dbipaddress);
	}
	dbhostiploop=0;
	return dbipaddress;
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

sqltranslations *sqlrconnection_svr::getSqlTranslations() {
	return new sqltranslations;
}

sqlwriter *sqlrconnection_svr::getSqlWriter() {
	return new sqlwriter;
}

void sqlrconnection_svr::clearError() {
	setError(NULL,0,true);
}

void sqlrconnection_svr::setError(const char *err,
					int64_t errn,
					bool liveconn) {
	errorlength=charstring::length(err);
	if (errorlength>cont->maxerrorlength-1) {
		errorlength=cont->maxerrorlength-1;
	}
	charstring::copy(error,err,errorlength);
	error[errorlength]='\0';
	errnum=errn;
	liveconnection=liveconn;
}
