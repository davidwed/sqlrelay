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
	clientsock->write(selectDatabase(db));

	// send back the result
	flushWriteBuffer();

	return;
}

bool sqlrconnection_svr::selectDatabase(const char *database) {

	// get the select database query base
	const char	*sdquerybase=selectDatabaseQuery();

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
		// FIXME: set a flag indicating that the db has been changed
		// so it can be reset when the session is done
		// dbselected=true;
	}
	delete[] sdquery;
	sdcur->closeCursor();
	deleteCursorUpdateStats(sdcur);
	return retval;
}

const char *sqlrconnection_svr::selectDatabaseQuery() {
	return "use %s";
}
