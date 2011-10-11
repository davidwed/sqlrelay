// Copyright (c) 2007  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrclient.h>
#include <defines.h>

const char *sqlrconnection::serverVersion() {

	if (!openSession()) {
		return NULL;
	}

	if (debug) {
		debugPreStart();
		debugPrint("Server Version...");
		debugPrint("\n");
		debugPreEnd();
	}

	cs->write((uint16_t)SERVERVERSION);
	flushWriteBuffer();

	// get the server version
	uint16_t	size;
	if (cs->read(&size)==sizeof(uint16_t)) {
		delete[] serverversion;
		serverversion=new char[size+1];
		if (cs->read(serverversion,size)!=size) {
			setError("Failed to get Server version.\n A network error may have ocurred.");
			delete[] serverversion;
			serverversion=NULL;
			return NULL;
		}
		serverversion[size]='\0';

		if (debug) {
			debugPreStart();
			debugPrint(serverversion);
			debugPrint("\n");
			debugPreEnd();
		}
	} else {
		setError("Failed to get Server version.\n A network error may have ocurred.");
		return NULL;
	}
	return serverversion;
}
