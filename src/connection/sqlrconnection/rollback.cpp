// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void sqlrconnection_svr::rollbackCommand() {
	dbgfile.debugPrint("connection",1,"rollback");
	if (rollbackInternal()) {
		clientsock->write((uint16_t)NO_ERROR_OCCURRED);
	} else {
		returnError();
	}
	flushWriteBuffer();
}

bool sqlrconnection_svr::rollbackInternal() {

	if (rollback()) {
		endFakeTransactionBlock();
		return true;
	}
	return false;
}

bool sqlrconnection_svr::rollback() {

	dbgfile.debugPrint("connection",1,"rollback...");

	// re-init error data
	clearError();

	// init some variables
	sqlrcursor_svr	*rollbackcur=initCursorInternal();
	const char	*rollbackquery="rollback";
	int		rollbackquerylen=8;
	bool		retval=false;

	// run the query...
	// since we're creating a new cursor for this, make sure it can't
	// have an ID that might already exist
	if (rollbackcur->openCursorInternal(cursorcount+1) &&
		rollbackcur->prepareQuery(rollbackquery,rollbackquerylen)) {
		retval=executeQueryInternal(rollbackcur,rollbackquery,
							rollbackquerylen);
	}

	// If there was an error, copy it out.  We'll be destroying the
	// cursor in a moment and the error will be lost otherwise.
	if (!retval) {
		const char	*err;
		rollbackcur->errorMessage(&err,&errnum,&liveconnection);
		error=charstring::duplicate(err);
	}

	// clean up
	rollbackcur->cleanUpData(true,true);
	rollbackcur->closeCursor();
	deleteCursorInternal(rollbackcur);

	// debug
	char	string[38];
	snprintf(string,38,"rollback result: %d",retval);
	dbgfile.debugPrint("connection",2,string);

	// we don't need to commit or rollback at the end of the session now
	if (retval) {
		commitorrollback=false;
	}

	return retval;
}
