// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrclient.h>
#include <defines.h>

bool sqlrconnection::getReconnect() {

	unsigned short	recon;
	if (cs->read(&recon)!=sizeof(unsigned short)) {
		setError("Failed to get whether we need to reconnect.\n A network error may have ocurred.");
		return false;
	}

	reconnect=(recon==RECONNECT);
	if (debug) {
		debugPreStart();
		if (reconnect) {
			debugPrint("Must Reconnect.\n");
		} else {
			debugPrint("Must Not Reconnect.\n");
		}
		debugPreEnd();
	}
	return true;
}
