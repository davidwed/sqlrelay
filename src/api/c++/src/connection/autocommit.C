// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrclient.h>
#include <defines.h>

int sqlrconnection::autoCommitOn() {
	return autoCommit(true);
}

int sqlrconnection::autoCommitOff() {
	return autoCommit(false);
}

int sqlrconnection::autoCommit(bool on) {

	if (!openSession()) {
		return 0;
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

	write((unsigned short)AUTOCOMMIT);
	write(on);

	bool	response;
	if (read(&response)!=sizeof(bool)) {
		setError("Failed to get autocommit status.\n A network error may have ocurred.");
		return -1;
	}
	return (response)?1:0;
}
