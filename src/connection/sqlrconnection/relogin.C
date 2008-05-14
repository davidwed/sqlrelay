// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <config.h>
#include <sqlrconnection.h>
#include <rudiments/snooze.h>

#include <unistd.h>

void sqlrconnection_svr::reLogIn() {

	markDatabaseUnavailable();

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

	markDatabaseAvailable();
}
