// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/hostentry.h>
#include <rudiments/process.h>

#include <defines.h>

#define FETCH_AT_ONCE		10
#define MAX_COLUMN_COUNT	256
#define MAX_FIELD_LENGTH	32768	
#define MAX_OUT_BIND_LOB_SIZE	2097152

class sqlrserverconnectionprivate {
	friend class sqlrserverconnection;

		uint32_t	_maxquerysize;
		uint32_t	_maxerrorsize;

		char		*_errorbuffer;
		uint32_t	_errorbuffersize;
		uint32_t	_errorsize;
		int64_t		_errnum;
		bool		_liveconnection;

		char		*_dbhostname;
		char		*_dbipaddress;
		uint32_t	_dbhostiploop;

		bool		_detachbeforelogin;

		stringbuffer	_tablelistquery;
};

sqlrserverconnection::sqlrserverconnection(sqlrservercontroller *cont) :
							sqlrserverbase() {

	pvt=new sqlrserverconnectionprivate;

	this->cont=cont;

	pvt->_maxquerysize=cont->getConfig()->getMaxQuerySize();
	pvt->_maxerrorsize=cont->getConfig()->getMaxErrorSize();

	pvt->_errorbuffersize=pvt->_maxerrorsize+1;
	pvt->_errorbuffer=new char[pvt->_errorbuffersize];
	pvt->_errorsize=0;
	pvt->_errnum=0;
	pvt->_liveconnection=false;

	pvt->_dbhostname=NULL;
	pvt->_dbipaddress=NULL;
	pvt->_dbhostiploop=0;

	pvt->_detachbeforelogin=false;
}

sqlrserverconnection::~sqlrserverconnection() {
	delete[] pvt->_errorbuffer;
	delete[] pvt->_dbhostname;
	delete[] pvt->_dbipaddress;
	delete pvt;
}

bool sqlrserverconnection::mustDetachBeforeLogIn() {
	return pvt->_detachbeforelogin;
}

void sqlrserverconnection::handleConnectString() {

	// get some parameters that are common to most db's

	// user and password
	cont->setUser(cont->getConnectStringValue("user"));
	cont->setPassword(cont->getConnectStringValue("password"));

	// autocommit
	const char	*autocommit=cont->getConnectStringValue("autocommit");
	if (charstring::isYes(autocommit)) {
		cont->setInitialAutoCommit(true);
	} else if (charstring::isNo(autocommit)) {
		cont->setInitialAutoCommit(false);
	} else {
		cont->setInitialAutoCommit(supportsTransactionBlocks());
	}

	// fake transaction blocks
	cont->setFakeTransactionBlocks(charstring::isYes(
			cont->getConnectStringValue("faketransactionblocks")));

	// fake binds
	if (charstring::isYes(cont->getConnectStringValue("fakebinds"))) {
		cont->setFakeInputBinds(true);
	}

	// rows to fetch-at-once
	uint32_t	fetchatonce=FETCH_AT_ONCE;
	const char	*fao=cont->getConnectStringValue("fetchatonce");
	if (fao) {
		fetchatonce=charstring::convertToUnsignedInteger(fao);
		if (fetchatonce<1) {
			fetchatonce=1;
		}
	}
	cont->setFetchAtOnce(fetchatonce);

	// max column count
	int32_t		maxcolumncount=MAX_COLUMN_COUNT;
	const char	*mcc=cont->getConnectStringValue("maxcolumncount");
	if (!mcc) {
		mcc=cont->getConnectStringValue("maxselectlistsize");
	}
	if (mcc) {
		maxcolumncount=charstring::convertToInteger(mcc);
		if (maxcolumncount<0) {
			maxcolumncount=0;
		}
	}
	cont->setMaxColumnCount(maxcolumncount);

	// max field size
	int32_t		maxfieldsize=MAX_FIELD_LENGTH;
	const char	*mfs=cont->getConnectStringValue("maxfieldsize");
	if (!mfs) {
		mfs=cont->getConnectStringValue("maxfieldlength");
	}
	if (!mfs) {
		mfs=cont->getConnectStringValue("maxitembuffersize");
	}
	if (mfs) {
		maxfieldsize=charstring::convertToInteger(mfs);
		if (maxfieldsize<0) {
			maxfieldsize=0;
		}
	}
	cont->setMaxFieldSize(maxfieldsize);

	// connect timeout
	int64_t		connecttimeout=0;
	const char	*cto=cont->getConnectStringValue("connecttimeout");
	if (!cto) {
		cto=cont->getConnectStringValue("timeout");
	}
	if (cto) {
		connecttimeout=charstring::convertToInteger(cto);
		if (connecttimeout<0) {
			connecttimeout=0;
		}
	}
	cont->setConnectTimeout(connecttimeout);

	// query timeout
	int64_t		querytimeout=0;
	const char	*qto=cont->getConnectStringValue("querytimeout");
	if (qto) {
		querytimeout=charstring::convertToInteger(qto);
		if (querytimeout<0) {
			querytimeout=0;
		}
	}
	cont->setQueryTimeout(querytimeout);

	// execute-direct
	cont->setExecuteDirect(charstring::isYes(
			cont->getConnectStringValue("executedirect")));

	// detach before login
	pvt->_detachbeforelogin=charstring::isYes(
			cont->getConnectStringValue("detachbeforelogin"));

	// database type
	const char	*dbtype=cont->getConnectStringValue("dbtype");
	if (!dbtype) {
		dbtype=cont->getConnectStringValue("identity");
	}
	cont->setDbType(dbtype);
}

bool sqlrserverconnection::changeUser(const char *newuser,
						const char *newpassword) {
	return cont->changeUser(newuser,newpassword);
}

bool sqlrserverconnection::changeProxiedUser(const char *newuser,
						const char *newpassword) {
	return false;
}

bool sqlrserverconnection::setAutoCommitOn() {

	// if the database doesn't support transactions, then autocommit
	// is always on
	if (!isTransactional()) {
		return true;
	}
	// the APIs of databases that don't support transaction blocks must
	// provide an auto-commit function, and setAutoCommitOn() must be
	// implemented for them, using that function
	if (!supportsTransactionBlocks()) {
		return false;
	}
	// if we're in a transaction block, then just commit it
	// and don't start a new one
	if (cont->getInTransaction()) {
		return cont->commit();
	}
	// otherwise, we're already outside of a transaction block,
	// so autocommit must already be on
	return true;
}

bool sqlrserverconnection::setAutoCommitOff() {

	// if the database doesn't support transactions, then it's impossible
	// to turn auto commit off
	if (!isTransactional()) {
		return false;
	}
	// the APIs of databases that don't support transaction blocks must
	// provide an auto-commit function, and setAutoCommitOff() must be
	// implemented for them, using that function
	if (!supportsTransactionBlocks()) {
		return false;
	}
	// if we're not in a transaction block, then start a new one
	if (!cont->getInTransaction()) {
		return cont->begin();
	}
	// otherwise, we're already in a transaction block,
	// so autocommit must already be off
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
	cont->clearError();

	// for db's that don't support begin queries,
	// don't do anything, just return true
	if (!supportsTransactionBlocks()) {
		return true;
	}

	// for db's that support begin queries, run one...

	// init some variables
	const char	*beginquery=beginTransactionQuery();
	int		beginquerysize=charstring::getLength(beginquery);
	bool		retval=false;

	// run the query...
	sqlrservercursor	*begincur=cont->newCursor();
	if (begincur->open() &&
		begincur->prepareQuery(beginquery,beginquerysize)) {
		retval=begincur->executeQuery(beginquery,beginquerysize);
	}

	// If there was an error, copy it out.  We'll be destroying the
	// cursor in a moment and the error will be lost otherwise.
	if (!retval) {
		cont->saveErrorFromCursor(begincur);
	}

	// clean up
	begincur->closeResultSet();
	begincur->close();
	cont->deleteCursor(begincur);

	// we will need to commit or rollback at the end of the session now
	if (retval) {
		cont->setNeedsCommitOrRollback(true);
	}

	return retval;
}

const char *sqlrserverconnection::beginTransactionQuery() {
	return "BEGIN";
}

bool sqlrserverconnection::commit() {

	// re-init error data
	cont->clearError();

	// init some variables
	const char	*commitquery="commit";
	int		commitquerysize=6;
	bool		retval=false;

	// run the query...
	sqlrservercursor	*commitcur=cont->newCursor();
	if (commitcur->open() &&
		commitcur->prepareQuery(commitquery,commitquerysize)) {
		retval=commitcur->executeQuery(commitquery,commitquerysize);
	}

	// If there was an error, copy it out.  We'll be destroying the
	// cursor in a moment and the error will be lost otherwise.
	if (!retval) {
		cont->saveErrorFromCursor(commitcur);
	}

	// clean up
	commitcur->closeResultSet();
	commitcur->close();
	cont->deleteCursor(commitcur);

	// we don't need to commit or rollback at the end of the session now
	if (retval) {
		cont->setNeedsCommitOrRollback(false);
	}

	return retval;
}

bool sqlrserverconnection::rollback() {

	// re-init error data
	cont->clearError();

	// init some variables
	const char	*rollbackquery="rollback";
	int		rollbackquerysize=8;
	bool		retval=false;

	// run the query...
	sqlrservercursor	*rbcur=cont->newCursor();
	if (rbcur->open() &&
		rbcur->prepareQuery(rollbackquery,rollbackquerysize)) {
		retval=rbcur->executeQuery(rollbackquery,rollbackquerysize);
	}

	// If there was an error, copy it out.  We'll be destroying the
	// cursor in a moment and the error will be lost otherwise.
	if (!retval) {
		cont->saveErrorFromCursor(rbcur);
	}

	// clean up
	rbcur->closeResultSet();
	rbcur->close();
	cont->deleteCursor(rbcur);

	// we don't need to commit or rollback at the end of the session now
	if (retval) {
		cont->setNeedsCommitOrRollback(false);
	}

	return retval;
}

bool sqlrserverconnection::selectDatabase(const char *database) {

	// re-init error data
	cont->clearError();

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
	size_t	sdquerysize=charstring::getLength(sdquerybase)+
				charstring::getLength(database)+1;
	if (sdquerysize>pvt->_maxquerysize) {
		return false;
	}

	// create the select database query
	char	*sdquery=new char[sdquerysize];
	charstring::printf(sdquery,sdquerysize,sdquerybase,database);
	sdquerysize=charstring::getLength(sdquery);

	// run the query...
	// (enable translations, triggers, etc. for this one)
	bool	retval=false;
	sqlrservercursor	*sdcur=cont->newCursor();
	if (cont->open(sdcur) &&
		cont->prepareQuery(sdcur,sdquery,sdquerysize,true,true,true) &&
		cont->executeQuery(sdcur,true,true,true,true)) {
		cont->closeResultSet(sdcur);
		retval=true;
	} else {
		// If there was an error, copy it out.  We'll be destroying the
		// cursor in a moment and the error will be lost otherwise.
		cont->saveErrorFromCursor(sdcur);
	}
	delete[] sdquery;
	cont->close(sdcur);
	cont->deleteCursor(sdcur);
	return retval;
}

const char *sqlrserverconnection::selectDatabaseQuery() {
	return NULL;
}

char *sqlrserverconnection::getCurrentDatabase() {

	// get the get current database query base
	const char	*gcdquery=getCurrentDatabaseQuery();

	// bail if there is no query for this
	if (!gcdquery) {
		return NULL;
	}

	size_t	gcdquerysize=charstring::getLength(gcdquery);

	// run the query...
	char	*retval=NULL;
	sqlrservercursor	*gcdcur=cont->newCursor();
	if (gcdcur->open() &&
		gcdcur->prepareQuery(gcdquery,gcdquerysize) &&
		gcdcur->executeQuery(gcdquery,gcdquerysize)) {

		bool	error=false;
		if (!gcdcur->noRowsToReturn() && gcdcur->fetchRow(&error)) {

			// get the first field of the row and return it
			const char	*field=NULL;
			uint64_t	fieldsize=0;
			bool		lob=false;
			bool		null=false;
			gcdcur->getField(0,&field,&fieldsize,&lob,&null);
			retval=charstring::duplicate(field);
		} 
	}
	gcdcur->closeResultSet();
	gcdcur->close();
	cont->deleteCursor(gcdcur);
	return retval;
}

const char *sqlrserverconnection::getCurrentDatabaseQuery() {
	return getNoopQuery();
}

char *sqlrserverconnection::getCurrentSchema() {

	// get the get current database query base
	const char	*gcsquery=getCurrentSchemaQuery();

	// bail if there is no query for this
	if (!gcsquery) {
		return NULL;
	}

	size_t	gcsquerysize=charstring::getLength(gcsquery);

	// run the query...
	char	*retval=NULL;
	sqlrservercursor	*gcscur=cont->newCursor();
	if (gcscur->open() &&
		gcscur->prepareQuery(gcsquery,gcsquerysize) &&
		gcscur->executeQuery(gcsquery,gcsquerysize)) {

		bool	error=false;
		if (!gcscur->noRowsToReturn() && gcscur->fetchRow(&error)) {

			// get the first field of the row and return it
			const char	*field=NULL;
			uint64_t	fieldsize=0;
			bool		lob=false;
			bool		null=false;
			gcscur->getField(0,&field,&fieldsize,&lob,&null);
			retval=charstring::duplicate(field);
		} 
	}
	gcscur->closeResultSet();
	gcscur->close();
	cont->deleteCursor(gcscur);
	return retval;
}

const char *sqlrserverconnection::getCurrentSchemaQuery() {
	return getNoopQuery();
}

bool sqlrserverconnection::getLastInsertId(uint64_t *id) {

	// re-init error data
	cont->clearError();

	// get the get current database query base
	const char	*liiquery=getLastInsertIdQuery();

	// If there is no query for this then the db we're using doesn't
	// support switching.
	if (!liiquery) {
		cont->setError(
			SQLR_ERROR_LASTINSERTIDNOTSUPPORTED_STRING,
			SQLR_ERROR_LASTINSERTIDNOTSUPPORTED,true);
		return false;
	}

	size_t	liiquerysize=charstring::getLength(liiquery);

	// run the query...
	bool	retval=false;
	sqlrservercursor	*liicur=cont->newCursor();
	if (liicur->open() &&
		liicur->prepareQuery(liiquery,liiquerysize) &&
		liicur->executeQuery(liiquery,liiquerysize)) {

		bool	error=false;
		if (!liicur->noRowsToReturn() && liicur->fetchRow(&error)) {

			// get the first field of the row and return it
			const char	*field=NULL;
			uint64_t	fieldsize=0;
			bool		lob=false;
			bool		null=false;
			liicur->getField(0,&field,&fieldsize,&lob,&null);
			*id=charstring::convertToInteger(field);
			retval=true;

		}  else {

			cont->setError(
				SQLR_ERROR_LASTINSERTIDNOTSUPPORTED_STRING,
				SQLR_ERROR_LASTINSERTIDNOTSUPPORTED,true);
			retval=false;
		}

	} else {
		// If there was an error, copy it out.  We'll be destroying the
		// cursor in a moment and the error will be lost otherwise.
		cont->saveErrorFromCursor(liicur);
	}

	liicur->closeResultSet();
	liicur->close();
	cont->deleteCursor(liicur);
	return retval;
}

const char *sqlrserverconnection::getLastInsertIdQuery() {
	return getNoopQuery();
}

const char *sqlrserverconnection::getNoopQuery() {
	return "";
}

bool sqlrserverconnection::setIsolationLevel(const char *isolevel) {

	// if no isolation level was passed in then bail
	if (!charstring::getLength(isolevel)) {
		return false;
	}

	// get the set isolation level query base
	const char	*silquerybase=setIsolationLevelQuery();

	// If there is no query for this then the db we're using doesn't
	// support switching.  Return true as if it succeeded though.
	if (!charstring::getLength(silquerybase)) {
		return true;
	}

	// bounds checking
	size_t		silquerysize=charstring::getLength(silquerybase)+
					charstring::getLength(isolevel)+1;
	if (silquerysize>pvt->_maxquerysize) {
		return false;
	}

	// create the set isolation level query
	char	*silquery=new char[silquerysize];
	charstring::printf(silquery,silquerysize,silquerybase,isolevel);
	silquerysize=charstring::getLength(silquery);

	// run the query...
	bool	retval=false;
	sqlrservercursor	*silcur=cont->newCursor();
	if (silcur->open() &&
		silcur->prepareQuery(silquery,silquerysize) &&
		silcur->executeQuery(silquery,silquerysize)) {
		retval=true;
	}

	// FIXME: we don't really need to do this now but we will
	// later if we ever add an API call to set the isolation level
	/* else {
		cont->saveErrorFromCursor(silcur);
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
	int		pingquerysize=charstring::getLength(pingquery);
	sqlrservercursor	*pingcur=cont->newCursor();
	if (pingcur->open() &&
		pingcur->prepareQuery(pingquery,pingquerysize) &&
		pingcur->executeQuery(pingquery,pingquerysize)) {
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

const char *sqlrserverconnection::getDbHostNameQuery() {
	return NULL;
}

const char *sqlrserverconnection::getDbIpAddressQuery() {
	return NULL;
}

const char *sqlrserverconnection::getDbHostName() {

	if (pvt->_dbhostname) {
		return pvt->_dbhostname;
	}

	// don't get looped up...
	if (pvt->_dbhostiploop==2) {
		pvt->_dbhostiploop=0;
		return NULL;
	}
	pvt->_dbhostiploop++;

	// if we have a host name query then use it, otherwise get the
	// ip address and convert it to a host name...

	const char	*dbhnquery=getDbHostNameQuery();
	if (dbhnquery) {

		size_t		dbhnquerysize=charstring::getLength(dbhnquery);
		sqlrservercursor	*dbhncur=cont->newCursor();
		if (dbhncur->open() &&
			dbhncur->prepareQuery(dbhnquery,dbhnquerysize) &&
			dbhncur->executeQuery(dbhnquery,dbhnquerysize)) {

			bool	error=false;
			if (!dbhncur->noRowsToReturn() &&
					dbhncur->fetchRow(&error)) {
				const char	*field=NULL;
				uint64_t	fieldsize=0;
				bool		lob=false;
				bool		null=false;
				dbhncur->getField(0,&field,&fieldsize,
								&lob,&null);
				pvt->_dbhostname=charstring::duplicate(field);
			} 
		
			dbhncur->closeResultSet();
		}
		dbhncur->close();
		cont->deleteCursor(dbhncur);

	} else {

		const char	*ipaddr=getDbIpAddress();
		char		ip[4];
		for (uint8_t i=0; i<4; i++) {
			ip[i]=charstring::convertToInteger(ipaddr);
			ipaddr=charstring::findFirst(ipaddr,'.');
			if (ipaddr) {
				ipaddr++;
			}
		}
		pvt->_dbhostname=hostentry::getName(ip,4,AF_INET);
	}
	pvt->_dbhostiploop=0;
	return pvt->_dbhostname;
}

const char *sqlrserverconnection::getDbIpAddress() {

	if (pvt->_dbipaddress) {
		return pvt->_dbipaddress;
	}

	// don't get looped up...
	if (pvt->_dbhostiploop==2) {
		pvt->_dbhostiploop=0;
		return NULL;
	}
	pvt->_dbhostiploop++;

	// if we have an ip address query then use it, otherwise get the
	// host name and convert it to an ip address...

	const char	*dbiaquery=getDbIpAddressQuery();
	if (dbiaquery) {

		size_t		dbiaquerysize=charstring::getLength(dbiaquery);
		sqlrservercursor	*dbiacur=cont->newCursor();
		if (dbiacur->open() &&
			dbiacur->prepareQuery(dbiaquery,dbiaquerysize) &&
			dbiacur->executeQuery(dbiaquery,dbiaquerysize)) {

			bool	error=false;
			if (!dbiacur->noRowsToReturn() &&
					dbiacur->fetchRow(&error)) {
				const char	*field=NULL;
				uint64_t	fieldsize=0;
				bool		lob=false;
				bool		null=false;
				dbiacur->getField(0,&field,&fieldsize,
								&lob,&null);
				pvt->_dbipaddress=charstring::duplicate(field);
			} 
		
			dbiacur->closeResultSet();
		}
		dbiacur->close();
		cont->deleteCursor(dbiacur);

	} else {
		pvt->_dbipaddress=hostentry::getAddressString(getDbHostName());
	}
	pvt->_dbhostiploop=0;
	return pvt->_dbipaddress;
}

bool sqlrserverconnection::cacheDbHostInfo() {
	return true;
}

bool sqlrserverconnection::getListsByApiCalls() {
	return false;
}

sqlrserverlistformat_t sqlrserverconnection::getNativeDatabaseListFormat() {
	return SQLRSERVERLISTFORMAT_MYSQL;
}

sqlrserverlistformat_t sqlrserverconnection::getNativeSchemaListFormat() {
	return SQLRSERVERLISTFORMAT_MYSQL;
}

sqlrserverlistformat_t sqlrserverconnection::getNativeTableListFormat() {
	return SQLRSERVERLISTFORMAT_MYSQL;
}

sqlrserverlistformat_t sqlrserverconnection::getNativeTableTypeListFormat() {
	return SQLRSERVERLISTFORMAT_MYSQL;
}

sqlrserverlistformat_t sqlrserverconnection::getNativeColumnListFormat() {
	return SQLRSERVERLISTFORMAT_MYSQL;
}

sqlrserverlistformat_t sqlrserverconnection::getNativePrimaryKeyListFormat() {
	return SQLRSERVERLISTFORMAT_MYSQL;
}

sqlrserverlistformat_t sqlrserverconnection::getNativeKeyAndIndexListFormat() {
	return SQLRSERVERLISTFORMAT_MYSQL;
}

sqlrserverlistformat_t sqlrserverconnection::
				getNativeProcedureParameterListFormat() {
	return SQLRSERVERLISTFORMAT_MYSQL;
}

sqlrserverlistformat_t sqlrserverconnection::getNativeTypeInfoListFormat() {
	return SQLRSERVERLISTFORMAT_MYSQL;
}

sqlrserverlistformat_t sqlrserverconnection::getNativeProcedureListFormat() {
	return SQLRSERVERLISTFORMAT_MYSQL;
}

bool sqlrserverconnection::getDatabaseList(sqlrservercursor *cursor,
						const char *wild) {
	cont->setError(cursor,SQLR_ERROR_NOTIMPLEMENTED_STRING,
				SQLR_ERROR_NOTIMPLEMENTED,true);
	return false;
}

bool sqlrserverconnection::getSchemaList(sqlrservercursor *cursor,
						const char *wild) {
	cont->setError(cursor,SQLR_ERROR_NOTIMPLEMENTED_STRING,
				SQLR_ERROR_NOTIMPLEMENTED,true);
	return false;
}

bool sqlrserverconnection::getTableList(sqlrservercursor *cursor,
						const char *wild,
						uint16_t objecttypes) {
	cont->setError(cursor,SQLR_ERROR_NOTIMPLEMENTED_STRING,
				SQLR_ERROR_NOTIMPLEMENTED,true);
	return false;
}

bool sqlrserverconnection::getTableTypeList(sqlrservercursor *cursor,
						const char *wild) {
	cont->setError(cursor,SQLR_ERROR_NOTIMPLEMENTED_STRING,
				SQLR_ERROR_NOTIMPLEMENTED,true);
	return false;
}

bool sqlrserverconnection::getColumnList(sqlrservercursor *cursor,
						const char *table,
						const char *wild) {
	cont->setError(cursor,SQLR_ERROR_NOTIMPLEMENTED_STRING,
				SQLR_ERROR_NOTIMPLEMENTED,true);
	return false;
}

bool sqlrserverconnection::getPrimaryKeyList(sqlrservercursor *cursor,
						const char *table,
						const char *wild) {
	cont->setError(cursor,SQLR_ERROR_NOTIMPLEMENTED_STRING,
				SQLR_ERROR_NOTIMPLEMENTED,true);
	return false;
}

bool sqlrserverconnection::getKeyAndIndexList(sqlrservercursor *cursor,
						const char *table,
						const char *wild) {
	cont->setError(cursor,SQLR_ERROR_NOTIMPLEMENTED_STRING,
				SQLR_ERROR_NOTIMPLEMENTED,true);
	return false;
}

bool sqlrserverconnection::getProcedureParameterList(
						sqlrservercursor *cursor,
						const char *procedure,
						const char *wild) {
	cont->setError(cursor,SQLR_ERROR_NOTIMPLEMENTED_STRING,
				SQLR_ERROR_NOTIMPLEMENTED,true);
	return false;
}

bool sqlrserverconnection::getTypeInfoList(sqlrservercursor *cursor,
						const char *type,
						const char *wild) {
	cont->setError(cursor,SQLR_ERROR_NOTIMPLEMENTED_STRING,
				SQLR_ERROR_NOTIMPLEMENTED,true);
	return false;
}

bool sqlrserverconnection::getProcedureList(sqlrservercursor *cursor,
						const char *wild) {
	cont->setError(cursor,SQLR_ERROR_NOTIMPLEMENTED_STRING,
				SQLR_ERROR_NOTIMPLEMENTED,true);
	return false;
}

const char *sqlrserverconnection::getDatabaseListQuery(
						bool wild) {
	return getNoopQuery();
}

const char *sqlrserverconnection::getSchemaListQuery(
						bool wild,
						bool currentdbonly) {
	return getNoopQuery();
}

const char *sqlrserverconnection::getTableListQuery(
						bool wild,
						uint16_t objecttypes,
						bool currentschemaonly) {
	return getNoopQuery();
}

const char *sqlrserverconnection::getTableTypeListQuery(bool wild,
						bool currentschemaonly) {
	return getNoopQuery();
}

const char *sqlrserverconnection::getGlobalTempTableListQuery(
						bool currentschemaonly) {
	return getNoopQuery();
}

const char *sqlrserverconnection::getColumnListQuery(
						const char *table,
						bool wild) {
	return getNoopQuery();
}

const char *sqlrserverconnection::getPrimaryKeyListQuery(
						const char *table,
						bool wild) {
	return getNoopQuery();
}

const char *sqlrserverconnection::getKeyAndIndexListQuery(
						const char *table,
						bool wild) {
	return getNoopQuery();
}

const char *sqlrserverconnection::getProcedureParameterListQuery(
						const char *procedure,
						bool wild) {
	return getNoopQuery();
}

const char *sqlrserverconnection::getTypeInfoListQuery(
						const char *type,
						bool wild,
						bool currentschemaonly) {
	return getNoopQuery();
}

const char *sqlrserverconnection::getProcedureListQuery(
						bool wild,
						bool currentschemaonly) {
	return getNoopQuery();
}

bool sqlrserverconnection::isSynonym(const char *table) {

	// get the base query
	const char	*synquerybase=isSynonymQuery();
	if (!synquerybase) {
		return false;
	}

	// rebuild it to include the table
	size_t	synquerysize=charstring::getLength(synquerybase)+
					charstring::getLength(table);
	char	*synquery=new char[synquerysize+1];
	charstring::printf(synquery,synquerysize+1,synquerybase,table);
	synquerysize=charstring::getLength(synquery);

	sqlrservercursor	*syncur=cont->newCursor();
	bool	error=false;
	bool	result=(syncur->open() &&
			syncur->prepareQuery(synquery,synquerysize) &&
			syncur->executeQuery(synquery,synquerysize) &&
			!syncur->noRowsToReturn() &&
			syncur->fetchRow(&error));
	syncur->closeResultSet();
	syncur->close();
	cont->deleteCursor(syncur);
	delete[] synquery;
	return result;
}

const char *sqlrserverconnection::isSynonymQuery() {
	return getNoopQuery();
}

const char *sqlrserverconnection::getBindFormat() {
	return ":*";
}

int16_t sqlrserverconnection::getNonNullBindValue() {
	return 0;
}

int16_t sqlrserverconnection::getNullBindValue() {
	return -1;
}

bool sqlrserverconnection::getBindValueIsNull(int16_t isnull) {
	return (isnull==getNullBindValue());
}

const char *sqlrserverconnection::getNextvalFormat() {
	return "%s.nextval";
}

const char *sqlrserverconnection::tempTablePrefix() {
	return "";
}

bool sqlrserverconnection::tempTableTruncateBeforeDrop() {
	return false;
}

void sqlrserverconnection::endSession() {
	// by default, do nothing
}

char *sqlrserverconnection::getErrorBuffer() {
	return pvt->_errorbuffer;
}

uint32_t sqlrserverconnection::getErrorBufferSize() {
	return pvt->_errorbuffersize;
}

uint32_t sqlrserverconnection::getErrorSize() {
	return pvt->_errorsize;
}

void sqlrserverconnection::setErrorSize(uint32_t errorsize) {
	pvt->_errorsize=errorsize;
}

uint32_t sqlrserverconnection::getErrorNumber() {
	return pvt->_errnum;
}

void sqlrserverconnection::setErrorNumber(uint32_t errnum) {
	pvt->_errnum=errnum;
}

bool sqlrserverconnection::getLiveConnection() {
	return pvt->_liveconnection;
}

void sqlrserverconnection::setLiveConnection(bool liveconnection) {
	pvt->_liveconnection=liveconnection;
}

bool sqlrserverconnection::send(byte_t *data, size_t size) {
	// by default, do nothing
	return false;
}

bool sqlrserverconnection::recv(byte_t **data, size_t *size) {
	// by default, do nothing
	return false;
}
