// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrcontroller.h>

void sqlrcontroller_svr::endSession() {

	dbgfile.debugPrint("connection",2,"ending session...");

	updateState(SESSION_END);

	dbgfile.debugPrint("connection",2,"aborting all cursors...");
	for (int32_t i=0; i<cursorcount; i++) {
		if (cur[i]) {
			cur[i]->abort();
		}
	}
	dbgfile.debugPrint("connection",2,"done aborting all cursors");

	// truncate/drop temp tables
	// (Do this before running the end-session queries becuase
	// with oracle, it may be necessary to log out and log back in to
	// drop a temp table.  With each log-out the session end queries
	// are run and with each log-in the session start queries are run.)
	truncateTempTables(cur[0],&sessiontemptablesfortrunc);
	dropTempTables(cur[0],&sessiontemptablesfordrop);

	sessionEndQueries();

	// must set suspendedsession to false here so resumed sessions won't 
	// automatically re-suspend
	suspendedsession=false;

	// if we're faking transaction blocks and the session was ended but we
	// haven't ended the transaction block, then we need to rollback and
	// end the block
	if (intransactionblock) {

		rollback();
		intransactionblock=false;

	} else if (conn->isTransactional() && commitorrollback) {

		// otherwise, commit or rollback as necessary
		if (cfgfl->getEndOfSessionCommit()) {
			dbgfile.debugPrint("connection",2,
						"committing...");
			commit();
			dbgfile.debugPrint("connection",2,
						"done committing...");
		} else {
			dbgfile.debugPrint("connection",2,
						"rolling back...");
			rollback();
			dbgfile.debugPrint("connection",2,
						"done rolling back...");
		}
	}

	// reset database/schema
	if (dbselected) {
		// FIXME: we're ignoring the result and error,
		// should we do something if there's an error?
		conn->selectDatabase(originaldb);
		dbselected=false;
	}

	// reset autocommit behavior
	setAutoCommit(conn->autocommit);

	// set isolation level
	conn->setIsolationLevel(isolationlevel);

	// reset sql translation
	if (sqlt) {
		sqlt->endSession();
	}

	// shrink the cursor array, if necessary
	while (cursorcount>mincursorcount) {
		cursorcount--;
		deleteCursor(cur[cursorcount]);
		cur[cursorcount]=NULL;
	}

	// end the session
	conn->endSession();

	dbgfile.debugPrint("connection",1,"done ending session");
}

void sqlrcontroller_svr::cleanUpAllCursorData() {
	dbgfile.debugPrint("connection",2,"cleaning up all cursors...");
	for (int32_t i=0; i<cursorcount; i++) {
		if (cur[i]) {
			cur[i]->cleanUpData();
		}
	}
	dbgfile.debugPrint("connection",2,"done cleaning up all cursors");
}
