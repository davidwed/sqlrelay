// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void sqlrconnection_svr::getLastInsertIdCommand() {

	dbgfile.debugPrint("connection",1,"getting last insert id");

	// get the last insert id
	uint64_t	id;
	char		*error=NULL;
	bool	success=getLastInsertId(&id,&error);

	// send success/failure
	clientsock->write(success);
	if (success) {

		// return the id
		clientsock->write(id);

	} else {

		// return the error
		uint16_t	errorlen=charstring::length(error);
		clientsock->write(errorlen);
		clientsock->write(error,errorlen);
	}

	// clean up
	delete[] error;

	flushWriteBuffer();
}

bool sqlrconnection_svr::getLastInsertId(uint64_t *id, char **error) {

	// get the get current database query base
	const char	*liiquery=getLastInsertIdQuery();

	// If there is no query for this then the db we're using doesn't
	// support switching.
	if (!liiquery) {
		*error=charstring::duplicate(
				"get last insert id not supported");
		return false;
	}

	size_t	liiquerylen=charstring::length(liiquery);

	sqlrcursor_svr	*liicur=initCursorUpdateStats();
	// since we're creating a new cursor for this, make sure it can't
	// have an ID that might already exist
	bool	retval=false;
	if (liicur->openCursorInternal(cursorcount+1) &&
		liicur->prepareQuery(liiquery,liiquerylen) &&
		executeQueryUpdateStats(liicur,liiquery,liiquerylen,true)) {

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

			*error=charstring::duplicate("no values returned");
			retval=false;
		}

	} else {
		// get error message
		bool	liveconnection;
		*error=charstring::duplicate(
				liicur->errorMessage(&liveconnection));
	}

	liicur->cleanUpData(true,true);
	liicur->closeCursor();
	deleteCursorUpdateStats(liicur);
	return retval;
}

const char *sqlrconnection_svr::getLastInsertIdQuery() {
	return NULL;
}
