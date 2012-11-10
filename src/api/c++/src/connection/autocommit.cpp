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

	if (debug) {
		debugPreStart();
		debugPrint("Setting AutoCommit");
		if (on) {
			debugPrint("on");
		} else {
			debugPrint("off");
		}
		debugPrint("...\n");
		debugPreEnd();
	}

	cs->write((uint16_t)AUTOCOMMIT);
	cs->write(on);

	flushWriteBuffer();

	bool	response;
	if (cs->read(&response,responsetimeoutsec,
				responsetimeoutusec)!=sizeof(bool)) {
		if (!on) {
			setError("Failed to set autocommit off.\n A network error may have ocurred.");
		} else {
			setError("Failed to set autocommit on.\n A network error may have ocurred.");
		}
		return false;
	}

	if (!response) {
		if (!on) {
			setError("Failed to set autocommit off.");
		} else {
			setError("Failed to set autocommit on.");
		}
	}

	return response;
}
