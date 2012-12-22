// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrclient.h>
#include <defines.h>

const char *sqlrconnection::identify() {

	if (!openSession()) {
		return NULL;
	}

	clearError();

	if (debug) {
		debugPreStart();
		debugPrint("Identifying...\n");
		debugPreEnd();
	}

	// tell the server we want the identity of the db
	cs->write((uint16_t)IDENTIFY);
	flushWriteBuffer();

	if (gotError()) {
		return NULL;
	}

	// get the identity size
	uint16_t	size;
	if (cs->read(&size,responsetimeoutsec,
				responsetimeoutusec)!=sizeof(uint16_t)) {
		setError("Failed to identify.\n "
			"A network error may have ocurred.");
		return NULL;
	}

	// get the identity
	delete[] id;
	id=new char[size+1];
	if (cs->read(id,size)!=size) {
		setError("Failed to identify.\n "
			"A network error may have ocurred.");
		delete[] id;
		id=NULL;
		return NULL;
	}
	id[size]='\0';

	if (debug) {
		debugPreStart();
		debugPrint(id);
		debugPrint("\n");
		debugPreEnd();
	}
	return id;
}
