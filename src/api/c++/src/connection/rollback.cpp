// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrclient.h>
#include <defines.h>

bool sqlrconnection::rollback() {

	if (!openSession()) {
		return false;
	}

	clearError();

	if (debug) {
		debugPreStart();
		debugPrint("Rolling Back...\n");
		debugPreEnd();
	}

	cs->write((uint16_t)ROLLBACK);
	flushWriteBuffer();

	return !gotError();
}
