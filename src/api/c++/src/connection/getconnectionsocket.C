// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrclient.h>

const char *sqlrconnection::getConnectionSocket() {

	if (!suspendsessionsent && !openSession()) {
		return NULL;
	}

	if (debug) {
		debugPreStart();
		debugPrint("Getting connection socket: ");
		if (connectionunixport) {
			debugPrint(connectionunixport);
		}
		debugPrint("\n");
		debugPreEnd();
	}

	if (connectionunixport) {
		return connectionunixport;
	}
	return NULL;
}
