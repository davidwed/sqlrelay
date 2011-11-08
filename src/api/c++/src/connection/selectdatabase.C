// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrclient.h>
#include <defines.h>

bool sqlrconnection::selectDatabase(const char *database) {

	if (!charstring::length(database)) {
		return true;
	}

	if (!openSession()) {
		return 0;
	}

	if (debug) {
		debugPreStart();
		debugPrint("Pinging...");
		debugPrint("\n");
		debugPreEnd();
	}

	// tell the server we want to select a db
	cs->write((uint16_t)SELECT_DATABASE);

	// send the database name
	uint32_t	len=charstring::length(database);
	cs->write(len);
	if (len) {
		cs->write(database,len);
	}

	flushWriteBuffer();

	// get the result
	bool	result;
	if (cs->read(&result)!=sizeof(bool)) {
		setError("Failed to select database.\n "
				"A network error may have ocurred.");
		return false;
	}
	return result;
}
