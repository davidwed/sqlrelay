// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrclient.h>

int sqlrconnection::getConnectionPort() {

	if (!suspendsessionsent && !openSession()) {
		return 0;
	}

	if (debug) {
		debugPreStart();
		debugPrint("Getting connection port: ");
		debugPrint((long)connectioninetport);
		debugPrint("\n");
		debugPreEnd();
	}

	return (int)connectioninetport;
}
