// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void sqlrconnection_svr::endSessionCommand() {
	dbgfile.debugPrint("connection",1,"end session");
	endSessionInternal();
}

void sqlrconnection_svr::endSession() {
	// by default, do nothing
}

void sqlrconnection_svr::endSessionInternal() {

	dbgfile.debugPrint("connection",2,"ending session...");

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

	abortAllCursors();

	// commit or rollback if necessary
	if (isTransactional() && commitorrollback) {
		if (cfgfl->getEndOfSessionCommit()) {
			dbgfile.debugPrint("connection",2,"committing...");
			commitInternal();
			dbgfile.debugPrint("connection",2,"done committing...");
		} else {
			dbgfile.debugPrint("connection",2,"rolling back...");
			rollbackInternal();
			dbgfile.debugPrint("connection",2,"done rolling back...");
		}
	}

	// reset database/schema
	if (dbselected) {
		// FIXME: we're ignoring the result and error,
		// should we do something if there's an error?
		char	*error=NULL;
		selectDatabase(originaldb,&error);
		delete[] error;
		dbselected=false;
	}

	// reset autocommit behavior
	dbgfile.debugPrint("connection",2,"resetting autocommit behavior...");
	if (autocommit) {
		dbgfile.debugPrint("connection",3,"setting autocommit on...");
		autoCommitOn();
		dbgfile.debugPrint("connection",3,"done setting autocommit on...");
	} else {
		dbgfile.debugPrint("connection",3,"setting autocommit off...");
		autoCommitOff();
		dbgfile.debugPrint("connection",3,"done setting autocommit off...");
	}
	dbgfile.debugPrint("connection",2,"done resetting autocommit behavior...");
	dbgfile.debugPrint("connection",1,"done ending session");

	// set isolation level
	setIsolationLevel(isolationlevel);

	// reset sql translation
	if (sqlt) {
		sqlt->endSession();
	}

	// end the session
	endSession();
}

void sqlrconnection_svr::abortAllCursors() {
	// abort all cursors
	dbgfile.debugPrint("connection",2,"aborting all busy cursors...");
	for (int32_t i=0; i<cursorcount; i++) {
		if (cur[i] && cur[i]->busy) {

			dbgfile.debugPrint("connection",3,i);

			// It's ok to call cleanUpData() here, ordinarily we
			// wouldn't so that result sets that were suspended
			// after the entire result set was fetched would be
			// able to return column data when resumed, but since
			// we're ending the session, we don't care...
			cur[i]->cleanUpData(true,true);
			cur[i]->abort();
		}
	}
	dbgfile.debugPrint("connection",2,"done aborting all busy cursors");

	// end sid session
	if (cfgfl->getSidEnabled()) {
		sid_sqlrcon->endSession();
	}
}

void sqlrconnection_svr::cleanUpAllCursorData(bool freeresult, bool freebinds) {

	// clean up all busy cursors
	dbgfile.debugPrint("connection",2,"cleaning up all busy cursors...");
	for (int32_t i=0; i<cursorcount; i++) {
		if (cur[i] && cur[i]->busy) {
			cur[i]->cleanUpData(freeresult,freebinds);
		}
	}
	dbgfile.debugPrint("connection",2,"done aborting all busy cursors");
}
