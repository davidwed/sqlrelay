// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrclient.h>
#include <defines.h>

uint64_t sqlrconnection::getLastInsertId() {

	if (!openSession()) {
		return 0;
	}

	if (debug) {
		debugPreStart();
		debugPrint("Getting the last insert id...\n");
		debugPreEnd();
	}

	clearError();

	// tell the server we want the last insert id
	cs->write((uint16_t)GET_LAST_INSERT_ID);
	flushWriteBuffer();

	// get the last insert id
	uint64_t	id=0;
	bool		success;
	if (cs->read(&success,responsetimeoutsec,
				responsetimeoutusec)==sizeof(bool)) {

		if (success) {

			// get the id
			if (cs->read(&id)==sizeof(uint64_t)) {
				if (debug) {
					debugPreStart();
					debugPrint("Got the last insert id: ");
					debugPrint((int64_t)id);
					debugPrint("\n");
					debugPreEnd();
				}
				return id;
			}
		} else {

			// get the error
			if (getError()) {
				if (debug) {
					debugPreStart();
					debugPrint("Failed to get the "
							"last insert id...\n");
					debugPrint(error);
					debugPrint("\n");
					debugPreEnd();
				}
				return 0;
			}
		}
	}

	// handle network errors
	if (debug) {
		debugPreStart();
		setError("Failed to get the last insert id.\n"
				"A network error may have ocurred.");
		debugPrint("Failed to get the last insert id...\n");
		debugPreEnd();
	}
	return 0;
}
