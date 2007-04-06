// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrclient.h>

sqlrconnection::~sqlrconnection() {

	// unless it was already ended or suspended, end the session
	if (!endsessionsent && !suspendsessionsent) {
		endSession();
	}

	// deallocate id
	delete[] id;

	// deallocate dbversion
	delete[] dbversion;

	// deallocate copied references
	if (copyrefs) {
		delete[] server;
		delete[] listenerunixport;
		delete[] user;
		delete[] password;
	}

	// detach all cursors attached to this client
	sqlrcursor	*currentcursor=firstcursor;
	while (currentcursor) {
		firstcursor=currentcursor;
		currentcursor=currentcursor->next;
		firstcursor->sqlrc=NULL;
	}

	if (debug) {
		debugPreStart();
		debugPrint("Deallocated connection\n");
		debugPreEnd();
	}
}
