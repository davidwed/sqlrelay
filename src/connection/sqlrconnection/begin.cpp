// Copyright (c) 2011  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void sqlrconnection_svr::beginCommand() {
	dbgfile.debugPrint("connection",1,"begin");
	clientsock->write(beginInternal());
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

	// for db's that don't support begin queries,
	// don't do anything, just return true
	if (!supportsTransactionBlocks()) {
		return true;
	}

	// for db's that support begin queries, run one
	dbgfile.debugPrint("connection",1,"begin...");

	sqlrcursor_svr	*begincur=initCursorUpdateStats();
	const char	*beginquery=beginTransactionQuery();
	int		beginquerylen=charstring::length(beginquery);
	bool		retval=false;
	// since we're creating a new cursor for this, make sure it can't
	// have an ID that might already exist
	if (begincur->openCursorInternal(cursorcount+1) &&
		begincur->prepareQuery(beginquery,beginquerylen)) {
		retval=executeQueryUpdateStats(begincur,beginquery,
						beginquerylen,true);
	}
	begincur->cleanUpData(true,true);
	begincur->closeCursor();
	deleteCursorUpdateStats(begincur);

	char	string[38];
	snprintf(string,38,"begin result: %d",retval);
	dbgfile.debugPrint("connection",2,string);

	if (retval) {
		commitorrollback=true;
	}

	return retval;
}
