// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void sqlrconnection_svr::getCurrentDatabaseCommand() {

	dbgfile.debugPrint("connection",1,"get current database");

	// get the current database
	char	*currentdb=getCurrentDatabase();

	// send it to the client
	uint16_t	currentdbsize=charstring::length(currentdb);
	clientsock->write(currentdbsize);
	clientsock->write(currentdb,currentdbsize);
	flushWriteBuffer();

	// clean up
	delete[] currentdb;
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
	if (gcdcur->openCursorInternal(cursorcount+1) &&
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
	gcdcur->closeCursor();
	deleteCursorInternal(gcdcur);
	return retval;
}

const char *sqlrconnection_svr::getCurrentDatabaseQuery() {
	return NULL;
}
