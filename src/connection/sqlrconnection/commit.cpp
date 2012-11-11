// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void sqlrconnection_svr::commitCommand() {
	dbgfile.debugPrint("connection",1,"commit");
	if (commitInternal()) {
		clientsock->write((uint16_t)NO_ERROR_OCCURRED);
	} else {
		returnError();
	}
	flushWriteBuffer();
}

bool sqlrconnection_svr::commitInternal() {
	if (commit()) {
		endFakeTransactionBlock();
		return true;
	}
	return false;
}

bool sqlrconnection_svr::commit() {

	dbgfile.debugPrint("connection",1,"commit...");

	// re-init error data
	clearError();

	// init some variables
	sqlrcursor_svr	*commitcur=initCursorInternal();
	const char	*commitquery="commit";
	int		commitquerylen=6;
	bool		retval=false;

	// run the query...
	// since we're creating a new cursor for this, make sure it can't
	// have an ID that might already exist
	if (commitcur->openCursorInternal(cursorcount+1) &&
		commitcur->prepareQuery(commitquery,commitquerylen)) {
		retval=executeQueryInternal(commitcur,commitquery,
							commitquerylen);
	}

	// If there was an error, copy it out.  We'll be destroying the
	// cursor in a moment and the error will be lost otherwise.
	if (!retval) {
		commitcur->errorMessage(error,maxerrorlength,
					&errorlength,&errnum,&liveconnection);
	}

	// clean up
	commitcur->cleanUpData(true,true);
	commitcur->closeCursor();
	deleteCursorInternal(commitcur);

	// debug
	char	string[36];
	snprintf(string,36,"commit result: %d",retval);
	dbgfile.debugPrint("connection",2,string);

	// we don't need to commit or rollback at the end of the session now
	if (retval) {
		commitorrollback=false;
	}

	return retval;
}
