// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void sqlrconnection_svr::selectDatabaseCommand() {

	dbgfile.debugPrint("connection",1,"select database");

	// get length of db parameter
	uint32_t	dblen;
	if (clientsock->read(&dblen,idleclienttimeout,0)!=sizeof(uint32_t)) {
		dbgfile.debugPrint("connection",2,
			"get list failed: client sent bad db length");
		clientsock->write(false);
		return;
	}

	// bounds checking
	if (dblen>maxquerysize) {
		dbgfile.debugPrint("connection",2,
			"get list failed: client sent bad db length");
		clientsock->write(false);
		return;
	}

	// read the db parameter into the buffer
	char	*db=new char[dblen+1];
	if (dblen) {
		if ((uint32_t)(clientsock->read(db,dblen,
					idleclienttimeout,0))!=dblen) {
			dbgfile.debugPrint("connection",2,
				"get list failed: "
				"client sent short db parameter");
			clientsock->write(false);
			return;
		}
	}
	db[dblen]='\0';
	
	// select the db and send back the result
	bool	result=selectDatabase(db);
	clientsock->write(result);

	// FIXME: if there was an error, send it back
	if (!result) {
	}

	// send back the result
	flushWriteBuffer();

	return;
}

bool sqlrconnection_svr::selectDatabase(const char *database) {

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
	int		sdquerylen=charstring::length(sdquerybase)+
					charstring::length(database)+1;
	if (sdquerylen>maxquerysize) {
		dbgfile.debugPrint("connection",2,
			"get list failed: client sent bad db length");
		return false;
	}

	// create the select database query
	char	*sdquery=new char[sdquerylen];
	snprintf(sdquery,sdquerylen,sdquerybase,database);
	sdquerylen=charstring::length(sdquery);

	sqlrcursor_svr	*sdcur=initCursorUpdateStats();
	// since we're creating a new cursor for this, make sure it can't
	// have an ID that might already exist
	bool	retval=false;
	if (sdcur->openCursorInternal(cursorcount+1) &&
		sdcur->prepareQuery(sdquery,sdquerylen) &&
		executeQueryUpdateStats(sdcur,sdquery,sdquerylen,true)) {
		sdcur->cleanUpData(true,true);
		retval=true;

		// set a flag indicating that the db has been changed
		// so it can be reset at the end of the session
		dbselected=true;
	}
	delete[] sdquery;
	sdcur->closeCursor();
	deleteCursorUpdateStats(sdcur);
	return retval;
}

const char *sqlrconnection_svr::selectDatabaseQuery() {
	return NULL;
}

void sqlrconnection_svr::getCurrentDatabaseCommand() {

	dbgfile.debugPrint("connection",1,"get current database");

	// get the current database
	char		*currentdb=getCurrentDatabase();
	uint16_t	currentdbsize=charstring::length(currentdb);

	// send it to the client
	clientsock->write(currentdbsize);
	clientsock->write(currentdb,currentdbsize);
	flushWriteBuffer();

	// clean up
	delete[] currentdb;

	return;
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

	sqlrcursor_svr	*gcdcur=initCursorUpdateStats();
	// since we're creating a new cursor for this, make sure it can't
	// have an ID that might already exist
	char	*retval=NULL;
	if (gcdcur->openCursorInternal(cursorcount+1) &&
		gcdcur->prepareQuery(gcdquery,gcdquerylen) &&
		executeQueryUpdateStats(gcdcur,gcdquery,gcdquerylen,true)) {

		if (!gcdcur->noRowsToReturn() && gcdcur->fetchRow()) {

			// get the first field of the row and return it
			const char	*field=NULL;
			uint64_t	fieldlength=0;
			bool		blob=false;
			bool		null=false;
			gcdcur->getField(0,&field,&fieldlength,&blob,&null);
			retval=charstring::duplicate(field);
		} 

		gcdcur->cleanUpData(true,true);
	}
	gcdcur->closeCursor();
	deleteCursorUpdateStats(gcdcur);
	return retval;
}

const char *sqlrconnection_svr::getCurrentDatabaseQuery() {
	return NULL;
}
