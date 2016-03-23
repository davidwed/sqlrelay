// Copyright (c) 1999-2014  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/hostentry.h>
#include <rudiments/process.h>

#include <defines.h>

sqlrserverconnection::sqlrserverconnection(sqlrservercontroller *cont) {

	this->cont=cont;

	maxquerysize=cont->cfg->getMaxQuerySize();
	maxerrorlength=cont->cfg->getMaxErrorLength();

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

sqlrserverconnection::~sqlrserverconnection() {
	delete[] error;
	delete[] dbhostname;
	delete[] dbipaddress;
}

bool sqlrserverconnection::mustDetachBeforeLogIn() {
	return false;
}

bool sqlrserverconnection::supportsAuthOnDatabase() {
	return true;
}

bool sqlrserverconnection::changeUser(const char *newuser,
						const char *newpassword) {
	return cont->changeUser(newuser,newpassword);
}

bool sqlrserverconnection::changeProxiedUser(const char *newuser,
						const char *newpassword) {
	return false;
}

bool sqlrserverconnection::autoCommitOn() {
	fakeautocommit=true;
	autocommit=true;
	return true;
}

bool sqlrserverconnection::autoCommitOff() {
	fakeautocommit=true;
	autocommit=false;
	return true;
}

bool sqlrserverconnection::isTransactional() {
	return true;
}

bool sqlrserverconnection::supportsTransactionBlocks() {
	return true;
}

bool sqlrserverconnection::begin() {

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
	sqlrservercursor	*begincur=cont->newCursor();
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

const char *sqlrserverconnection::beginTransactionQuery() {
	return "BEGIN";
}

bool sqlrserverconnection::commit() {

	// re-init error data
	clearError();

	// init some variables
	const char	*commitquery="commit";
	int		commitquerylen=6;
	bool		retval=false;

	// run the query...
	sqlrservercursor	*commitcur=cont->newCursor();
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

bool sqlrserverconnection::rollback() {

	// re-init error data
	clearError();

	// init some variables
	const char	*rollbackquery="rollback";
	int		rollbackquerylen=8;
	bool		retval=false;

	// run the query...
	sqlrservercursor	*rbcur=cont->newCursor();
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

bool sqlrserverconnection::selectDatabase(const char *database) {

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
	sqlrservercursor	*sdcur=cont->newCursor();
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

const char *sqlrserverconnection::selectDatabaseQuery() {
	return NULL;
}

char *sqlrserverconnection::getCurrentDatabase() {

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
	sqlrservercursor	*gcdcur=cont->newCursor();
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

const char *sqlrserverconnection::getCurrentDatabaseQuery() {
	return NULL;
}

bool sqlrserverconnection::getLastInsertId(uint64_t *id) {

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
	sqlrservercursor	*liicur=cont->newCursor();
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

const char *sqlrserverconnection::getLastInsertIdQuery() {
	return NULL;
}

bool sqlrserverconnection::setIsolationLevel(const char *isolevel) {

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
	sqlrservercursor	*silcur=cont->newCursor();
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

const char *sqlrserverconnection::setIsolationLevelQuery() {
	return "set transaction isolation level %s";
}

bool sqlrserverconnection::ping() {
	const char	*pingquery=pingQuery();
	int		pingquerylen=charstring::length(pingquery);
	sqlrservercursor	*pingcur=cont->newCursor();
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

const char *sqlrserverconnection::pingQuery() {
	return "select 1";
}

const char *sqlrserverconnection::dbHostNameQuery() {
	return NULL;
}

const char *sqlrserverconnection::dbIpAddressQuery() {
	return NULL;
}

const char *sqlrserverconnection::dbHostName() {

	if (dbhostname) {
		return dbhostname;
	}

	// don't get looped up...
	if (dbhostiploop==2) {
		dbhostiploop=0;
		return NULL;
	}
	dbhostiploop++;

	// if we have a host name query then use it, otherwise get the
	// ip address and convert it to a host name...

	const char	*dbhnquery=dbHostNameQuery();
	if (dbhnquery) {

		size_t		dbhnquerylen=charstring::length(dbhnquery);
		sqlrservercursor	*dbhncur=cont->newCursor();
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

const char *sqlrserverconnection::dbIpAddress() {

	if (dbipaddress) {
		return dbipaddress;
	}

	// don't get looped up...
	if (dbhostiploop==2) {
		dbhostiploop=0;
		return NULL;
	}
	dbhostiploop++;

	// if we have an ip address query then use it, otherwise get the
	// host name and convert it to an ip address...

	const char	*dbiaquery=dbIpAddressQuery();
	if (dbiaquery) {

		size_t		dbiaquerylen=charstring::length(dbiaquery);
		sqlrservercursor	*dbiacur=cont->newCursor();
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

bool sqlrserverconnection::getListsByApiCalls() {
	return false;
}

bool sqlrserverconnection::getDatabaseList(sqlrservercursor *cursor,
						const char *wild) {
	return false;
}

bool sqlrserverconnection::getTableList(sqlrservercursor *cursor,
						const char *wild) {
	return false;
}

bool sqlrserverconnection::getColumnList(sqlrservercursor *cursor,
						const char *table,
						const char *wild) {
	return false;
}

const char *sqlrserverconnection::getDatabaseListQuery(bool wild) {
	return "select 1";
}

const char *sqlrserverconnection::getTableListQuery(bool wild) {
	return "select 1";
}

const char *sqlrserverconnection::getGlobalTempTableListQuery() {
	return NULL;
}

const char *sqlrserverconnection::getColumnListQuery(const char *table,
								bool wild) {
	return "select 1";
}

bool sqlrserverconnection::isSynonym(const char *table) {

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

	sqlrservercursor	*syncur=cont->newCursor();
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

const char *sqlrserverconnection::isSynonymQuery() {
	return NULL;
}

const char *sqlrserverconnection::bindFormat() {
	return ":*";
}

int16_t sqlrserverconnection::nonNullBindValue() {
	return 0;
}

int16_t sqlrserverconnection::nullBindValue() {
	return -1;
}

char sqlrserverconnection::bindVariablePrefix() {
	return ':';
}

bool sqlrserverconnection::bindValueIsNull(int16_t isnull) {
	return (isnull==nullBindValue());
}

const char *sqlrserverconnection::tempTableDropPrefix() {
	return "";
}

bool sqlrserverconnection::tempTableTruncateBeforeDrop() {
	return false;
}

void sqlrserverconnection::endSession() {
	// by default, do nothing
}

bool sqlrserverconnection::getAutoCommit() {
	return autocommit;
}

void sqlrserverconnection::setAutoCommit(bool autocommit) {
	this->autocommit=autocommit;
}

bool sqlrserverconnection::getFakeAutoCommit() {
	return fakeautocommit;
}

void sqlrserverconnection::clearError() {
	setError(NULL,0,true);
}

void sqlrserverconnection::setError(const char *err,
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

char *sqlrserverconnection::getErrorBuffer() {
	return error;
}

uint32_t sqlrserverconnection::getErrorLength() {
	return errorlength;
}

void sqlrserverconnection::setErrorLength(uint32_t errorlength) {
	this->errorlength=errorlength;
}

uint32_t sqlrserverconnection::getErrorNumber() {
	return errnum;
}

void sqlrserverconnection::setErrorNumber(uint32_t errnum) {
	this->errnum=errnum;
}

bool sqlrserverconnection::getLiveConnection() {
	return liveconnection;
}

void sqlrserverconnection::setLiveConnection(bool liveconnection) {
	this->liveconnection=liveconnection;
}
