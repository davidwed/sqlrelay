// Copyright (c) 2007  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrclient.h>
#include <defines.h>

const char *sqlrconnection::bindFormat() {

	if (!openSession()) {
		return NULL;
	}

	clearError();

	if (debug) {
		debugPreStart();
		debugPrint("bind format...");
		debugPrint("\n");
		debugPreEnd();
	}

	// tell the server we want the bind format
	cs->write((uint16_t)BINDFORMAT);
	flushWriteBuffer();

	if (gotError()) {
		return NULL;
	}


	// get the bindformat size
	uint16_t	size;
	if (cs->read(&size,responsetimeoutsec,
				responsetimeoutusec)!=sizeof(uint16_t)) {
		setError("Failed to get bind format.\n"
			" A network error may have ocurred.");
		return NULL;
	}

	// get the bindformat
	delete[] bindformat;
	bindformat=new char[size+1];
	if (cs->read(bindformat,size)!=size) {
		setError("Failed to get bind format.\n "
			"A network error may have ocurred.");
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
	return bindformat;
}
