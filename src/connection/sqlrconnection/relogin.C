// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <config.h>
#include <sqlrconnection.h>
#include <rudiments/snooze.h>

#include <unistd.h>

void sqlrconnection_svr::reLogIn() {

	markDatabaseUnavailable();

	// run the session end queries
	sessionEndQueries();

	// get the current db so we can restore it
	char	*currentdb=getCurrentDatabase();

	// FIXME: get the isolation level so we can restore it

	dbgfile.debugPrint("connection",4,"relogging in...");

	// attempt to log in over and over, once every 5 seconds
	closeCursors(false);
	logOutUpdateStats();
	for (;;) {
			
		dbgfile.debugPrint("connection",5,"trying...");

		if (logInUpdateStats(false)) {
			if (!initCursors()) {
				closeCursors(false);
				logOutUpdateStats();
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
