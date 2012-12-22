// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrclient.h>
#include <defines.h>

bool sqlrconnection::autoCommitOn() {
	return autoCommit(true);
}

bool sqlrconnection::autoCommitOff() {
	return autoCommit(false);
}

bool sqlrconnection::autoCommit(bool on) {

	if (!openSession()) {
		return false;
	}

	clearError();

	if (debug) {
		debugPreStart();
		debugPrint((on)?"Setting autocommit on\n":
				"Setting autocommit off\n");
		debugPreEnd();
	}

	cs->write((uint16_t)AUTOCOMMIT);
	cs->write(on);
	flushWriteBuffer();

	return !gotError();
}
