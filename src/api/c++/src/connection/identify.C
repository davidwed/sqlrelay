// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrclient.h>
#include <defines.h>

const char *sqlrconnection::identify() {

	if (!openSession()) {
		return NULL;
	}

	if (debug) {
		debugPreStart();
		debugPrint("Identifying...");
		debugPrint("\n");
		debugPreEnd();
	}

	cs->write((uint16_t)IDENTIFY);
	flushWriteBuffer();

	// get the id
	uint16_t	size;
	if (cs->read(&size)==sizeof(uint16_t)) {
		delete[] id;
		id=new char[size+1];
		if (cs->read(id,size)!=size) {
			setError("Failed to identify.\n A network error may have ocurred.");
			delete[] id;
			id=NULL;
			return NULL;
		}
		id[size]=(char)NULL;

		if (debug) {
			debugPreStart();
			debugPrint(id);
			debugPrint("\n");
			debugPreEnd();
		}
	} else {
		setError("Failed to identify.\n A network error may have ocurred.");
		return NULL;
	}
	return id;
}
