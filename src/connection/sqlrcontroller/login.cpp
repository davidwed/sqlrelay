// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <config.h>
#include <sqlrcontroller.h>
#include <rudiments/snooze.h>

bool sqlrcontroller_svr::logIn(bool printerrors) {

	// don't do anything if we're already logged in
	if (loggedin) {
		return true;
	}

	// attempt to log in
	if (!conn->logIn(printerrors)) {
		return false;
	}

	// success... update stats
	incrementOpenServerConnections();

	loggedin=true;
	return true;
}

void sqlrcontroller_svr::logOut() {

	// don't do anything if we're already logged out
	if (!loggedin) {
		return;
	}

	// log out
	conn->logOut();

	// update stats
	decrementOpenServerConnections();

	loggedin=false;
}

void sqlrcontroller_svr::reLogIn() {

	markDatabaseUnavailable();

	// run the session end queries
	sessionEndQueries();

	// get the current db so we can restore it
	char	*currentdb=conn->getCurrentDatabase();

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
	conn->selectDatabase(currentdb);

	// restore autocommit
	if (conn->autocommit) {
		conn->autoCommitOn();
	} else {
		conn->autoCommitOff();
	}

	// FIXME: restore the isolation level

	markDatabaseAvailable();
}
