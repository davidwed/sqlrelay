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
		uint32_t	_maxerrorlength;

		char		*_error;
		uint32_t	_errorlength;
		int64_t		_errnum;
		bool		_liveconnection;

		char		*_dbhostname;
		char		*_dbipaddress;
		uint32_t	_dbhostiploop;

		bool		_detachbeforelogin;

		stringbuffer	_tablelistquery;
};

sqlrserverconnection::sqlrserverconnection(sqlrservercontroller *cont) {

	pvt=new sqlrserverconnectionprivate;

	this->cont=cont;

	pvt->_maxquerysize=cont->getConfig()->getMaxQuerySize();
	pvt->_maxerrorlength=cont->getConfig()->getMaxErrorLength();

	pvt->_error=new char[pvt->_maxerrorlength+1];
	pvt->_errorlength=0;
	pvt->_errnum=0;
	pvt->_liveconnection=false;

	pvt->_dbhostname=NULL;
	pvt->_dbipaddress=NULL;
	pvt->_dbhostiploop=0;

	pvt->_detachbeforelogin=false;
}

sqlrserverconnection::~sqlrserverconnection() {
	delete[] pvt->_error;
	delete[] pvt->_dbhostname;
	delete[] pvt->_dbipaddress;
	delete pvt;
}

bool sqlrserverconnection::mustDetachBeforeLogIn() {
	return pvt->_detachbeforelogin;
}

bool sqlrserverconnection::supportsAuthOnDatabase() {
	return true;
}

void sqlrserverconnection::handleConnectString() {

	// get some parameters that are common to most db's

	// user and password
	cont->setUser(cont->getConnectStringValue("user"));
	cont->setPassword(cont->getConnectStringValue("password"));

	// autocommit
	cont->setInitialAutoCommit(charstring::isYes(
			cont->getConnectStringValue("autocommit")));

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
		fetchatonce=charstring::toUnsignedInteger(fao);
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
		maxcolumncount=charstring::toInteger(mcc);
		if (maxcolumncount<0) {
			maxcolumncount=0;
		}
	}
	cont->setMaxColumnCount(maxcolumncount);

	// max field length
	int32_t		maxfieldlength=MAX_FIELD_LENGTH;
	const char	*mfl=cont->getConnectStringValue("maxfieldlength");
	if (!mfl) {
		mfl=cont->getConnectStringValue("maxitembuffersize");
	}
	if (mfl) {
		maxfieldlength=charstring::toInteger(mfl);
		if (maxfieldlength<0) {
			maxfieldlength=0;
		}
	}
	cont->setMaxFieldLength(maxfieldlength);

	// connect timeout
	int64_t		connecttimeout=0;
	const char	*cto=cont->getConnectStringValue("connecttimeout");
	if (!cto) {
		cto=cont->getConnectStringValue("timeout");
	}
	if (cto) {
		connecttimeout=charstring::toInteger(cto);
		if (connecttimeout<0) {
			connecttimeout=0;
		}
	}
	cont->setConnectTimeout(connecttimeout);

	// query timeout
	int64_t		querytimeout=0;
	const char	*qto=cont->getConnectStringValue("querytimeout");
	if (qto) {
		querytimeout=charstring::toInteger(qto);
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
	cont->setFakeAutoCommit(true);
	return true;
}

bool sqlrserverconnection::autoCommitOff() {
	cont->setFakeAutoCommit(false);
	return true;
}

bool sqlrserverconnection::isTransactional() {
	return true;
}

bool sqlrserverconnection::supportsTransactionBlocks() {
	return true;
}

bool sqlrserverconnection::supportsAutoCommit() {
	return false;
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
	size_t		sdquerylen=charstring::length(sdquerybase)+
					charstring::length(database)+1;
	if (sdquerylen>pvt->_maxquerysize) {
		return false;
	}

	// create the select database query
	char	*sdquery=new char[sdquerylen];
	charstring::printf(sdquery,sdquerylen,sdquerybase,database);
	sdquerylen=charstring::length(sdquery);

	// run the query...
	// (enable translations, triggers, etc. for this one)
	bool	retval=false;
	sqlrservercursor	*sdcur=cont->newCursor();
	if (cont->open(sdcur) &&
		cont->prepareQuery(sdcur,sdquery,sdquerylen,true,true,true) &&
		cont->executeQuery(sdcur,true,true,true,true)) {
		cont->closeResultSet(sdcur);
		retval=true;

		// set a flag indicating that the db has been changed
		// so it can be reset at the end of the session
		cont->dbHasChanged();
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

	size_t		gcdquerylen=charstring::length(gcdquery);

	// run the query...
	char	*retval=NULL;
	sqlrservercursor	*gcdcur=cont->newCursor();
	if (gcdcur->open() &&
		gcdcur->prepareQuery(gcdquery,gcdquerylen) &&
		gcdcur->executeQuery(gcdquery,gcdquerylen)) {

		bool	error=false;
		if (!gcdcur->noRowsToReturn() && gcdcur->fetchRow(&error)) {

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

char *sqlrserverconnection::getCurrentSchema() {

	// get the get current database query base
	const char	*gcsquery=getCurrentSchemaQuery();

	// bail if there is no query for this
	if (!gcsquery) {
		return NULL;
	}

	size_t		gcsquerylen=charstring::length(gcsquery);

	// run the query...
	char	*retval=NULL;
	sqlrservercursor	*gcscur=cont->newCursor();
	if (gcscur->open() &&
		gcscur->prepareQuery(gcsquery,gcsquerylen) &&
		gcscur->executeQuery(gcsquery,gcsquerylen)) {

		bool	error=false;
		if (!gcscur->noRowsToReturn() && gcscur->fetchRow(&error)) {

			// get the first field of the row and return it
			const char	*field=NULL;
			uint64_t	fieldlength=0;
			bool		blob=false;
			bool		null=false;
			gcscur->getField(0,&field,&fieldlength,&blob,&null);
			retval=charstring::duplicate(field);
		} 
	}
	gcscur->closeResultSet();
	gcscur->close();
	cont->deleteCursor(gcscur);
	return retval;
}

const char *sqlrserverconnection::getCurrentSchemaQuery() {
	return NULL;
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

	size_t	liiquerylen=charstring::length(liiquery);

	// run the query...
	bool	retval=false;
	sqlrservercursor	*liicur=cont->newCursor();
	if (liicur->open() &&
		liicur->prepareQuery(liiquery,liiquerylen) &&
		liicur->executeQuery(liiquery,liiquerylen)) {

		bool	error=false;
		if (!liicur->noRowsToReturn() && liicur->fetchRow(&error)) {

			// get the first field of the row and return it
			const char	*field=NULL;
			uint64_t	fieldlength=0;
			bool		blob=false;
			bool		null=false;
			liicur->getField(0,&field,&fieldlength,&blob,&null);
			*id=charstring::toInteger(field);
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
	return NULL;
}

const char *sqlrserverconnection::noopQuery() {
	return "";
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
	if (silquerylen>pvt->_maxquerysize) {
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

	const char	*dbhnquery=dbHostNameQuery();
	if (dbhnquery) {

		size_t		dbhnquerylen=charstring::length(dbhnquery);
		sqlrservercursor	*dbhncur=cont->newCursor();
		if (dbhncur->open() &&
			dbhncur->prepareQuery(dbhnquery,dbhnquerylen) &&
			dbhncur->executeQuery(dbhnquery,dbhnquerylen)) {

			bool	error=false;
			if (!dbhncur->noRowsToReturn() &&
					dbhncur->fetchRow(&error)) {
				const char	*field=NULL;
				uint64_t	fieldlength=0;
				bool		blob=false;
				bool		null=false;
				dbhncur->getField(0,&field,&fieldlength,
								&blob,&null);
				pvt->_dbhostname=charstring::duplicate(field);
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
		pvt->_dbhostname=hostentry::getName(ip,4,AF_INET);
	}
	pvt->_dbhostiploop=0;
	return pvt->_dbhostname;
}

const char *sqlrserverconnection::dbIpAddress() {

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

	const char	*dbiaquery=dbIpAddressQuery();
	if (dbiaquery) {

		size_t		dbiaquerylen=charstring::length(dbiaquery);
		sqlrservercursor	*dbiacur=cont->newCursor();
		if (dbiacur->open() &&
			dbiacur->prepareQuery(dbiaquery,dbiaquerylen) &&
			dbiacur->executeQuery(dbiaquery,dbiaquerylen)) {

			bool	error=false;
			if (!dbiacur->noRowsToReturn() &&
					dbiacur->fetchRow(&error)) {
				const char	*field=NULL;
				uint64_t	fieldlength=0;
				bool		blob=false;
				bool		null=false;
				dbiacur->getField(0,&field,&fieldlength,
								&blob,&null);
				pvt->_dbipaddress=charstring::duplicate(field);
			} 
		
			dbiacur->closeResultSet();
		}
		dbiacur->close();
		cont->deleteCursor(dbiacur);

	} else {
		pvt->_dbipaddress=hostentry::getAddressString(dbHostName());
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

bool sqlrserverconnection::getProcedureBindAndColumnList(
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

const char *sqlrserverconnection::getDatabaseListQuery(bool wild) {
	return "select 1";
}

const char *sqlrserverconnection::getSchemaListQuery(bool wild) {
	return "select 1";
}

const char *sqlrserverconnection::getTableListQuery(bool wild,
						uint16_t objecttypes) {
	return getTableListQuery(wild,objecttypes,NULL);

}

const char *sqlrserverconnection::getTableListQuery(bool wild,
						uint16_t objecttypes,
						const char *extrawhere) {

	stringbuffer	otypes;
	otypes.append("	(");
	if (objecttypes&DB_OBJECT_TABLE) {
		otypes.append("	table_type = 'BASE TABLE' ");
	}
	if (objecttypes&DB_OBJECT_VIEW) {
		if (otypes.getSize()) {
			otypes.append("	or ");
		}
		otypes.append("	table_type = 'VIEW' ");
	}
	if (objecttypes&DB_OBJECT_ALIAS) {
		if (otypes.getSize()) {
			otypes.append("	or ");
		}
		otypes.append("	table_type = 'ALIAS' ");
	}
	if (objecttypes&DB_OBJECT_SYNONYM) {
		if (otypes.getSize()) {
			otypes.append("	or ");
		}
		otypes.append("	table_type = 'SYNONYM' ");
	}
	otypes.append(") ");

	pvt->_tablelistquery.clear();
	pvt->_tablelistquery.append("select ");
	pvt->_tablelistquery.append("	table_catalog as table_cat, ");
	pvt->_tablelistquery.append("	table_schema as table_schem, ");
	pvt->_tablelistquery.append("	table_name, ");
	pvt->_tablelistquery.append("	case ");
	pvt->_tablelistquery.append("		when table_type = ");
	pvt->_tablelistquery.append("'BASE TABLE' then 'TABLE' ");
	pvt->_tablelistquery.append("		else table_type ");
	pvt->_tablelistquery.append("	end as table_type, ");
	pvt->_tablelistquery.append("	NULL as remarks, ");
	pvt->_tablelistquery.append("	NULL as extra ");
	pvt->_tablelistquery.append("from ");
	pvt->_tablelistquery.append("	information_schema.tables ");
	pvt->_tablelistquery.append("where ");
	if (wild) {
		pvt->_tablelistquery.append("	table_name like '%s' ");
		pvt->_tablelistquery.append("	and ");
	}
	pvt->_tablelistquery.append(otypes.getString());
	pvt->_tablelistquery.append(extrawhere);
	pvt->_tablelistquery.append("order by ");
	pvt->_tablelistquery.append("	table_cat, ");
	pvt->_tablelistquery.append("	table_schem, ");
	pvt->_tablelistquery.append("	table_name");

	return pvt->_tablelistquery.getString();
}

const char *sqlrserverconnection::getTableTypeListQuery(bool wild) {
	return "select 1";
}

const char *sqlrserverconnection::getGlobalTempTableListQuery() {
	return NULL;
}

const char *sqlrserverconnection::getColumnListQuery(const char *table,
							bool wild) {
	return "select 1";
}

const char *sqlrserverconnection::getPrimaryKeyListQuery(const char *table,
							bool wild) {
	return "select 1";
}

const char *sqlrserverconnection::getKeyAndIndexListQuery(const char *table,
							bool wild) {
	return "select 1";
}

const char *sqlrserverconnection::getProcedureBindAndColumnListQuery(
							const char *procedure,
							bool wild) {
	return "select 1";
}

const char *sqlrserverconnection::getTypeInfoListQuery(const char *type,
							bool wild) {
	return "select 1";
}

const char *sqlrserverconnection::getProcedureListQuery(bool wild) {
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
	bool	error=false;
	bool	result=(syncur->open() &&
			syncur->prepareQuery(synquery,synquerylen) &&
			syncur->executeQuery(synquery,synquerylen) &&
			!syncur->noRowsToReturn() &&
			syncur->fetchRow(&error));
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

char *sqlrserverconnection::getErrorBuffer() {
	return pvt->_error;
}

uint32_t sqlrserverconnection::getErrorLength() {
	return pvt->_errorlength;
}

void sqlrserverconnection::setErrorLength(uint32_t errorlength) {
	pvt->_errorlength=errorlength;
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

bool sqlrserverconnection::send(unsigned char *data, size_t size) {
	// by default, do nothing
	return false;
}

bool sqlrserverconnection::recv(unsigned char **data, size_t *size) {
	// by default, do nothing
	return false;
}
