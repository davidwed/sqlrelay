// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <config.h>
#include <sqlrconnection.h>
#include <rudiments/snooze.h>

bool sqlrconnection_svr::logIn(bool printerrors) {

	// don't do anything if we're already logged in
	if (loggedin) {
		return true;
	}

	// attempt to log in
	if (!logIn(printerrors)) {
		return false;
	}

	// success... update stats
	semset->waitWithUndo(9);
	statistics->open_svr_connections++;
	statistics->opened_svr_connections++;
	semset->signalWithUndo(9);
	loggedin=true;
	return true;
}

void sqlrconnection_svr::logOut() {

	// don't do anything if we're already logged out
	if (!loggedin) {
		return;
	}

	// log out
	logOut();

	// update stats
	semset->waitWithUndo(9);
	statistics->open_svr_connections--;
	if (statistics->open_svr_connections<0) {
		statistics->open_svr_connections=0;
	}
	semset->signalWithUndo(9);
	loggedin=false;
}

void sqlrconnection_svr::reLogIn() {

	markDatabaseUnavailable();

	// run the session end queries
	sessionEndQueries();

	// get the current db so we can restore it
	char	*currentdb=getCurrentDatabase();

	// FIXME: get the isolation level so we can restore it

	dbgfile.debugPrint("connection",4,"relogging in...");

	// attempt to log in over and over, once every 5 seconds
	int32_t	oldcursorcount=cursorcount;
	closeCursors(false);
	logOut();
	for (;;) {
			
		dbgfile.debugPrint("connection",5,"trying...");

		if (logIn(false)) {
			if (!initCursors(oldcursorcount)) {
				closeCursors(false);
				logOut();
			} else {
				break;
			}
		}
		snooze::macrosnooze(5);
	}

	dbgfile.debugPrint("connection",4,"done relogging in");

	// run the session-start queries
	sessionStartQueries();

	// restore the db
	char	*error=NULL;
	selectDatabase(currentdb,&error);
	delete[] error;

	// restore autocommit
	if (autocommit) {
		autoCommitOn();
	} else {
		autoCommitOff();
	}

	// FIXME: restore the isolation level

	markDatabaseAvailable();
}
