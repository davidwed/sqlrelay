// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrclient.h>
#include <defines.h>

bool sqlrconnection::begin() {

	if (!openSession()) {
		return false;
	}

	if (debug) {
		debugPreStart();
		debugPrint("Beginning...");
		debugPrint("\n");
		debugPreEnd();
	}

	cs->write((uint16_t)BEGIN);

	flushWriteBuffer();

	uint16_t	status;
	if (cs->read(&status,responsetimeoutsec,
				responsetimeoutusec)!=sizeof(uint16_t)) {
		setError("Failed to get commit status.\n "
				"A network error may have ocurred.");
		return false;
	}

	if (status==NO_ERROR_OCCURRED) {
		return true;
	}

	if (!getError()) {
		setError("There was an error, but the connection"
				" died trying to retrieve it.  Sorry.");
	}

	if (status==ERROR_OCCURRED_DISCONNECT) {
		endSession();
	}
	return false;
}
