// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrclient.h>
#include <defines.h>

void sqlrconnection::getReconnect() {

	unsigned short	recon;
	if (read(&recon)!=sizeof(unsigned short)) {
		reconnect=-1;
		return;
	}

	if (recon==DONT_RECONNECT) {
		if (debug) {
			debugPreStart();
			debugPrint("Must Not Reconnect.\n");
			debugPreEnd();
		}
		reconnect=0;
		return;
	}
	if (debug) {
		debugPreStart();
		debugPrint("Must Reconnect.\n");
		debugPreEnd();
	}
	reconnect=1;
}
