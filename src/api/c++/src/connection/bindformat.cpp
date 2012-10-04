// Copyright (c) 2007  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrclient.h>
#include <defines.h>

const char *sqlrconnection::bindFormat() {

	if (!openSession()) {
		return NULL;
	}

	if (debug) {
		debugPreStart();
		debugPrint("bind format...");
		debugPrint("\n");
		debugPreEnd();
	}

	cs->write((uint16_t)BINDFORMAT);
	flushWriteBuffer();

	// get the bindformat
	uint16_t	size;
	if (cs->read(&size)==sizeof(uint16_t)) {
		delete[] bindformat;
		bindformat=new char[size+1];
		if (cs->read(bindformat,size)!=size) {
			setError("Failed to get bind format.\n A network error may have ocurred.");
			delete[] bindformat;
			bindformat=NULL;
			return NULL;
		}
		bindformat[size]='\0';

		if (debug) {
			debugPreStart();
			debugPrint(bindformat);
			debugPrint("\n");
			debugPreEnd();
		}
	} else {
		setError("Failed to get bind format.\n A network error may have ocurred.");
		return NULL;
	}
	return bindformat;
}
