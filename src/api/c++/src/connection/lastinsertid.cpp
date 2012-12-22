// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrclient.h>
#include <defines.h>

uint64_t sqlrconnection::getLastInsertId() {

	if (!openSession()) {
		return 0;
	}

	clearError();

	if (debug) {
		debugPreStart();
		debugPrint("Getting the last insert id...\n");
		debugPreEnd();
	}

	// tell the server we want the last insert id
	cs->write((uint16_t)GET_LAST_INSERT_ID);
	flushWriteBuffer();

	if (gotError()) {
		return 0;
	}

	// get the last insert id
	uint64_t	id=0;
	if (cs->read(&id)!=sizeof(uint64_t)) {
		setError("Failed to get the last insert id.\n"
				"A network error may have ocurred.");
		return 0;
	}

	if (debug) {
		debugPreStart();
		debugPrint("Got the last insert id: ");
		debugPrint((int64_t)id);
		debugPrint("\n");
		debugPreEnd();
	}
	return id;
}
