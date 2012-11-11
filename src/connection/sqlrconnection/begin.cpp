// Copyright (c) 2011  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void sqlrconnection_svr::beginCommand() {
	dbgfile.debugPrint("connection",1,"begin");
	if (beginInternal()) {
		clientsock->write((uint16_t)NO_ERROR_OCCURRED);
	} else {
		returnError();
	}
	flushWriteBuffer();
}

bool sqlrconnection_svr::beginInternal() {

	// if we're faking transaction blocks, do that,
	// otherwise run an actual begin query
	return (faketransactionblocks)?beginFakeTransactionBlock():begin();
}

const char *sqlrconnection_svr::beginTransactionQuery() {
	return "BEGIN";
}

bool sqlrconnection_svr::begin() {

	// re-init error data
	clearError();

	// for db's that don't support begin queries,
	// don't do anything, just return true
	if (!supportsTransactionBlocks()) {
		return true;
	}

	// for db's that support begin queries, run one
	dbgfile.debugPrint("connection",1,"begin...");

	// init some variables
	sqlrcursor_svr	*begincur=initCursorInternal();
	const char	*beginquery=beginTransactionQuery();
	int		beginquerylen=charstring::length(beginquery);
	bool		retval=false;

	// run the query...
	// since we're creating a new cursor for this, make sure it can't
	// have an ID that might already exist
	if (begincur->openCursorInternal(cursorcount+1) &&
		begincur->prepareQuery(beginquery,beginquerylen)) {
		retval=executeQueryInternal(begincur,beginquery,beginquerylen);
	}

	// If there was an error, copy it out.  We'll be destroying the
	// cursor in a moment and the error will be lost otherwise.
	if (!retval) {
		begincur->errorMessage(error,maxerrorlength,
					&errorlength,&errnum,&liveconnection);
	}

	// clean up
	begincur->cleanUpData(true,true);
	begincur->closeCursor();
	deleteCursorInternal(begincur);

	// debug
	char	string[38];
	snprintf(string,38,"begin result: %d",retval);
	dbgfile.debugPrint("connection",2,string);

	// we will need to commit or rollback at the end of the session now
	if (retval) {
		commitorrollback=true;
	}

	return retval;
}
