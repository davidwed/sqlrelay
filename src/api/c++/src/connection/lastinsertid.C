// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrclient.h>
#include <defines.h>

bool sqlrconnection::getLastInsertId(uint64_t *id) {

	if (!openSession()) {
		return false;
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
	bool	success;
	if (cs->read(&success)==sizeof(bool)) {

		if (success) {

			// get the id
			if (cs->read(id)==sizeof(uint64_t)) {
				if (debug) {
					debugPreStart();
					debugPrint("Got the last insert id: ");
					debugPrint((int64_t)*id);
					debugPrint("\n");
					debugPreEnd();
				}
				return true;
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
				return false;
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
	return false;
}
