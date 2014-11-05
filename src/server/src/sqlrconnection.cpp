// Copyright (c) 1999-2014  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrcontroller.h>
#include <sqlrelay/sqlrconnection.h>
#include <rudiments/hostentry.h>

#include <defines.h>

sqlrconnection_svr::sqlrconnection_svr(sqlrcontroller_svr *cont) {

	this->cont=cont;

	maxquerysize=cont->cfgfl->getMaxQuerySize();
	maxerrorlength=cont->cfgfl->getMaxErrorLength();

	error=new char[maxerrorlength+1];
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

bool sqlrconnection_svr::mustDetachBeforeLogIn() {
	return false;
}

bool sqlrconnection_svr::supportsAuthOnDatabase() {
	return true;
}

bool sqlrconnection_svr::changeUser(const char *newuser,
					const char *newpassword) {
	return cont->changeUser(newuser,newpassword);
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
	const char	*beginquery=beginTransactionQuery();
	int		beginquerylen=charstring::length(beginquery);
	bool		retval=false;

	// run the query...
	sqlrcursor_svr	*begincur=cont->newCursor();
	if (begincur->open() &&
		begincur->prepareQuery(beginquery,beginquerylen)) {
		retval=begincur->executeQuery(beginquery,beginquerylen);
	}

	// If there was an error, copy it out.  We'll be destroying the
	// cursor in a moment and the error will be lost otherwise.
	if (!retval) {
		begincur->errorMessage(error,maxerrorlength,
					&errorlength,&errnum,&liveconnection);
	}

	// clean up
	begincur->closeResultSet();
	begincur->close();
	cont->deleteCursor(begincur);

	// we will need to commit or rollback at the end of the session now
	if (retval) {
		cont->commitOrRollbackIsNeeded();
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
	const char	*commitquery="commit";
	int		commitquerylen=6;
	bool		retval=false;

	// run the query...
	sqlrcursor_svr	*commitcur=cont->newCursor();
	if (commitcur->open() &&
		commitcur->prepareQuery(commitquery,commitquerylen)) {
		retval=commitcur->executeQuery(commitquery,commitquerylen);
	}

	// If there was an error, copy it out.  We'll be destroying the
	// cursor in a moment and the error will be lost otherwise.
	if (!retval) {
		commitcur->errorMessage(error,maxerrorlength,
					&errorlength,&errnum,&liveconnection);
	}

	// clean up
	commitcur->closeResultSet();
	commitcur->close();
	cont->deleteCursor(commitcur);

	// we don't need to commit or rollback at the end of the session now
	if (retval) {
		cont->commitOrRollbackIsNotNeeded();
	}

	return retval;
}

bool sqlrconnection_svr::rollback() {

	// re-init error data
	clearError();

	// init some variables
	const char	*rollbackquery="rollback";
	int		rollbackquerylen=8;
	bool		retval=false;

	// run the query...
	sqlrcursor_svr	*rbcur=cont->newCursor();
	if (rbcur->open() &&
		rbcur->prepareQuery(rollbackquery,rollbackquerylen)) {
		retval=rbcur->executeQuery(rollbackquery,rollbackquerylen);
	}

	// If there was an error, copy it out.  We'll be destroying the
	// cursor in a moment and the error will be lost otherwise.
	if (!retval) {
		rbcur->errorMessage(error,maxerrorlength,
					&errorlength,&errnum,&liveconnection);
	}

	// clean up
	rbcur->closeResultSet();
	rbcur->close();
	cont->deleteCursor(rbcur);

	// we don't need to commit or rollback at the end of the session now
	if (retval) {
		cont->commitOrRollbackIsNotNeeded();
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
	charstring::printf(sdquery,sdquerylen,sdquerybase,database);
	sdquerylen=charstring::length(sdquery);

	// run the query...
	bool	retval=false;
	sqlrcursor_svr	*sdcur=cont->newCursor();
	if (sdcur->open() &&
		sdcur->prepareQuery(sdquery,sdquerylen) &&
		sdcur->executeQuery(sdquery,sdquerylen)) {
		sdcur->closeResultSet();
		retval=true;

		// set a flag indicating that the db has been changed
		// so it can be reset at the end of the session
		cont->dbHasChanged();
	} else {
		// If there was an error, copy it out.  We'l be destroying the
		// cursor in a moment and the error will be lost otherwise.
		sdcur->errorMessage(error,maxerrorlength,
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

	// run the query...
	char	*retval=NULL;
	sqlrcursor_svr	*gcdcur=cont->newCursor();
	if (gcdcur->open() &&
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
	gcdcur->closeResultSet();
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
		setError(SQLR_ERROR_LASTINSERTIDNOTSUPPORTED_STRING,
				SQLR_ERROR_LASTINSERTIDNOTSUPPORTED,true);
		return false;
	}

	size_t	liiquerylen=charstring::length(liiquery);

	// run the query...
	bool	retval=false;
	sqlrcursor_svr	*liicur=cont->newCursor();
	if (liicur->open() &&
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

			setError(SQLR_ERROR_LASTINSERTIDNOTSUPPORTED_STRING,
				SQLR_ERROR_LASTINSERTIDNOTSUPPORTED,true);
			retval=false;
		}

	} else {
		// If there was an error, copy it out.  We'l be destroying the
		// cursor in a moment and the error will be lost otherwise.
		liicur->errorMessage(error,maxerrorlength,
					&errorlength,&errnum,&liveconnection);
	}

	liicur->closeResultSet();
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
	if (silquerylen>maxquerysize) {
		return false;
	}

	// create the set isolation level query
	char	*silquery=new char[silquerylen];
	charstring::printf(silquery,silquerylen,silquerybase,isolevel);
	silquerylen=charstring::length(silquery);

	// run the query...
	bool	retval=false;
	sqlrcursor_svr	*silcur=cont->newCursor();
	if (silcur->open() &&
		silcur->prepareQuery(silquery,silquerylen) &&
		silcur->executeQuery(silquery,silquerylen)) {
		retval=true;
	}

	// FIXME: we don't really need to do this now but we will
	// later if we ever add an API call to set the isolation level
	/* else {
		silcur->errorMessage(error,maxerrorlength,
					&errorlength,&errnum,&liveconnection));
	} */

	delete[] silquery;
	silcur->closeResultSet();
	silcur->close();
	cont->deleteCursor(silcur);
	return retval;
}

const char *sqlrconnection_svr::setIsolationLevelQuery() {
	return "set transaction isolation level %s";
}

bool sqlrconnection_svr::ping() {
	const char	*pingquery=pingQuery();
	int		pingquerylen=charstring::length(pingquery);
	sqlrcursor_svr	*pingcur=cont->newCursor();
	if (pingcur->open() &&
		pingcur->prepareQuery(pingquery,pingquerylen) &&
		pingcur->executeQuery(pingquery,pingquerylen)) {
		pingcur->closeResultSet();
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

		int		dbhnquerylen=charstring::length(dbhnquery);
		sqlrcursor_svr	*dbhncur=cont->newCursor();
		if (dbhncur->open() &&
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
		
			dbhncur->closeResultSet();
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
		dbhostname=hostentry::getName(ip,4,AF_INET);
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

		int		dbiaquerylen=charstring::length(dbiaquery);
		sqlrcursor_svr	*dbiacur=cont->newCursor();
		if (dbiacur->open() &&
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
		
			dbiacur->closeResultSet();
		}
		dbiacur->close();
		cont->deleteCursor(dbiacur);

	} else {
		dbipaddress=hostentry::getAddressString(dbHostName());
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

const char *sqlrconnection_svr::getColumnListQuery(const char *table,
								bool wild) {
	return "select 1";
}

bool sqlrconnection_svr::isSynonym(const char *table) {

	// get the base query
	const char	*synquerybase=isSynonymQuery();
	if (!synquerybase) {
		return false;
	}

	// rebuild it to include the table
	size_t	synquerylen=charstring::length(synquerybase)+
					charstring::length(table);
	char	*synquery=new char[synquerylen+1];
	charstring::printf(synquery,synquerylen+1,synquerybase,table);
	synquerylen=charstring::length(synquery);

	sqlrcursor_svr	*syncur=cont->newCursor();
	bool	result=(syncur->open() &&
			syncur->prepareQuery(synquery,synquerylen) &&
			syncur->executeQuery(synquery,synquerylen) &&
			!syncur->noRowsToReturn() &&
			syncur->fetchRow());
	syncur->closeResultSet();
	syncur->close();
	cont->deleteCursor(syncur);
	delete[] synquery;
	return result;
}

const char *sqlrconnection_svr::isSynonymQuery() {
	return NULL;
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

bool sqlrconnection_svr::getAutoCommit() {
	return autocommit;
}

void sqlrconnection_svr::setAutoCommit(bool autocommit) {
	this->autocommit=autocommit;
}

bool sqlrconnection_svr::getFakeAutoCommit() {
	return fakeautocommit;
}

void sqlrconnection_svr::clearError() {
	setError(NULL,0,true);
}

void sqlrconnection_svr::setError(const char *err,
					int64_t errn,
					bool liveconn) {
	errorlength=charstring::length(err);
	if (errorlength>maxerrorlength-1) {
		errorlength=maxerrorlength-1;
	}
	charstring::copy(error,err,errorlength);
	error[errorlength]='\0';
	errnum=errn;
	liveconnection=liveconn;
}

char *sqlrconnection_svr::getErrorBuffer() {
	return error;
}

uint32_t sqlrconnection_svr::getErrorLength() {
	return errorlength;
}

void sqlrconnection_svr::setErrorLength(uint32_t errorlength) {
	this->errorlength=errorlength;
}

uint32_t sqlrconnection_svr::getErrorNumber() {
	return errnum;
}

void sqlrconnection_svr::setErrorNumber(uint32_t errnum) {
	this->errnum=errnum;
}

bool sqlrconnection_svr::getLiveConnection() {
	return liveconnection;
}

void sqlrconnection_svr::setLiveConnection(bool liveconnection) {
	this->liveconnection=liveconnection;
}
