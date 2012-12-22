// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrclient.h>
#include <defines.h>

bool sqlrconnection::begin() {

	if (!openSession()) {
		return false;
	}

	clearError();

	if (debug) {
		debugPreStart();
		debugPrint("Beginning...\n");
		debugPreEnd();
	}

	cs->write((uint16_t)BEGIN);
	flushWriteBuffer();

	return !gotError();
}
