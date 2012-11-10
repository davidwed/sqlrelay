// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrclient.h>
#include <defines.h>

bool sqlrconnection::ping() {

	if (!openSession()) {
		return 0;
	}

	if (debug) {
		debugPreStart();
		debugPrint("Pinging...");
		debugPrint("\n");
		debugPreEnd();
	}

	cs->write((uint16_t)PING);
	flushWriteBuffer();

	// get the ping result
	bool	result;
	if (cs->read(&result,responsetimeoutsec,
				responsetimeoutusec)!=sizeof(bool)) {
		setError("Failed to ping.\n A network error may have ocurred.");
		return false;
	}
	return result;
}
