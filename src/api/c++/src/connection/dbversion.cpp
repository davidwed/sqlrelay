// Copyright (c) 2007  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrclient.h>
#include <defines.h>

const char *sqlrconnection::dbVersion() {

	if (!openSession()) {
		return NULL;
	}

	clearError();

	if (debug) {
		debugPreStart();
		debugPrint("DB Version...");
		debugPrint("\n");
		debugPreEnd();
	}

	// tell the server we want the db version
	cs->write((uint16_t)DBVERSION);
	flushWriteBuffer();

	if (gotError()) {
		return NULL;
	}

	// get the db version size
	uint16_t	size;
	if (cs->read(&size,responsetimeoutsec,
				responsetimeoutusec)!=sizeof(uint16_t)) {
		setError("Failed to get DB version.\n "
			"A network error may have ocurred.");
		return NULL;
	} 

	// get the db version
	delete[] dbversion;
	dbversion=new char[size+1];
	if (cs->read(dbversion,size)!=size) {
		setError("Failed to get DB version.\n "
			"A network error may have ocurred.");
		delete[] dbversion;
		dbversion=NULL;
		return NULL;
	}
	dbversion[size]='\0';

	if (debug) {
		debugPreStart();
		debugPrint(dbversion);
		debugPrint("\n");
		debugPreEnd();
	}
	return dbversion;
}
