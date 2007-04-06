// Copyright (c) 2007  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrclient.h>
#include <defines.h>

const char *sqlrconnection::dbVersion() {

	if (!openSession()) {
		return NULL;
	}

	if (debug) {
		debugPreStart();
		debugPrint("DB Version...");
		debugPrint("\n");
		debugPreEnd();
	}

	cs->write((uint16_t)DBVERSION);
	flushWriteBuffer();

	// get the dbversion
	uint16_t	size;
	if (cs->read(&size)==sizeof(uint16_t)) {
		delete[] dbversion;
		dbversion=new char[size+1];
		if (cs->read(dbversion,size)!=size) {
			setError("Failed to get DB version.\n A network error may have ocurred.");
			delete[] dbversion;
			dbversion=NULL;
			return NULL;
		}
		dbversion[size]=(char)NULL;

		if (debug) {
			debugPreStart();
			debugPrint(dbversion);
			debugPrint("\n");
			debugPreEnd();
		}
	} else {
		setError("Failed to get DB version.\n A network error may have ocurred.");
		return NULL;
	}
	return dbversion;
}
