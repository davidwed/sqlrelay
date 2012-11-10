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
	
	// Select the db and send back the result.  If we've been told to
	// ignore these calls, skip the actual call but act like it succeeded.
	bool	result=(ignoreselectdb)?true:selectDatabase(db);
	clientsock->write(result);

	// if there was an error, send it back
	if (!result) {
		uint16_t	errorlen=charstring::length(error);
		clientsock->write(errorlen);
		clientsock->write(error,errorlen);
	}

	flushWriteBuffer();

	return;
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
		dbgfile.debugPrint("connection",2,
			"get list failed: client sent bad db length");
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
	if (sdcur->openCursorInternal(cursorcount+1) &&
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
		const char	*err;
		sdcur->errorMessage(&err,&errnum,&liveconnection);
		error=charstring::duplicate(err);
	}
	delete[] sdquery;
	sdcur->closeCursor();
	deleteCursorInternal(sdcur);
	return retval;
}

const char *sqlrconnection_svr::selectDatabaseQuery() {
	return NULL;
}
