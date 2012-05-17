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

	bool	response;
	if (cs->read(&response)!=sizeof(bool)) {
		setError("Failed to get begin status.\n A network error may have ocurred.");
		return false;
	}
	return response;
}
