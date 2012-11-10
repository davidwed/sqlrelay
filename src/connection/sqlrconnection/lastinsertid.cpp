// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void sqlrconnection_svr::getLastInsertIdCommand() {

	dbgfile.debugPrint("connection",1,"getting last insert id");

	// get the last insert id
	uint64_t	id;
	bool	success=getLastInsertId(&id);

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

	flushWriteBuffer();
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

	sqlrcursor_svr	*liicur=initCursorInternal();
	// since we're creating a new cursor for this, make sure it can't
	// have an ID that might already exist
	bool	retval=false;
	if (liicur->openCursorInternal(cursorcount+1) &&
		liicur->prepareQuery(liiquery,liiquerylen) &&
		executeQueryInternal(liicur,liiquery,liiquerylen)) {

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
		const char	*err;
		liicur->errorMessage(&err,&errnum,&liveconnection);
		error=charstring::duplicate(err);
	}

	liicur->cleanUpData(true,true);
	liicur->closeCursor();
	deleteCursorInternal(liicur);
	return retval;
}

const char *sqlrconnection_svr::getLastInsertIdQuery() {
	return NULL;
}
