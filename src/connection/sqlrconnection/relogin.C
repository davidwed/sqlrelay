// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <config.h>
#include <sqlrconnection.h>
#include <rudiments/snooze.h>

void sqlrconnection::reLogIn() {

	markDatabaseUnavailable();

	#ifdef SERVER_DEBUG
	debugPrint("connection",4,"relogging in...");
	#endif

	// attempt to log in over and over, once every 5 seconds
	closeCursors(false);
	logOut();
	for (;;) {
			
		#ifdef SERVER_DEBUG
		debugPrint("connection",5,"trying...");
		#endif

		if (logIn()) {
			if (!initCursors(false)) {
				closeCursors(false);
				logOut();
			} else {
				break;
			}
		}
		snooze::macrosnooze(5);
	}

	#ifdef SERVER_DEBUG
	debugPrint("connection",4,"done relogging in");
	#endif

	markDatabaseAvailable();
}
