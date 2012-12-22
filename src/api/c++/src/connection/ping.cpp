// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrclient.h>
#include <defines.h>

bool sqlrconnection::ping() {

	if (!openSession()) {
		return 0;
	}

	clearError();

	if (debug) {
		debugPreStart();
		debugPrint("Pinging...\n");
		debugPreEnd();
	}

	cs->write((uint16_t)PING);
	flushWriteBuffer();

	return !gotError();
}
