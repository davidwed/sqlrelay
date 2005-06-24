// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrclient.h>

uint16_t sqlrconnection::getConnectionPort() {

	if (!suspendsessionsent && !openSession()) {
		return 0;
	}

	if (debug) {
		debugPreStart();
		debugPrint("Getting connection port: ");
		debugPrint((int64_t)connectioninetport);
		debugPrint("\n");
		debugPreEnd();
	}

	return connectioninetport;
}
