// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrclient.h>
#include <defines.h>

bool sqlrconnection::rollback() {

	if (!openSession()) {
		return false;
	}

	if (debug) {
		debugPreStart();
		debugPrint("Rolling Back...");
		debugPrint("\n");
		debugPreEnd();
	}

	cs->write((unsigned short)ROLLBACK);
	flushWriteBuffer();

	bool	response;
	if (cs->read(&response)!=sizeof(bool)) {
		setError("Failed to get rollback status.\n A network error may have ocurred.");
		return false;
	}
	return response;
}
