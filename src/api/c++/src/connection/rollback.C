// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrclient.h>
#include <defines.h>

int sqlrconnection::rollback() {

	if (!openSession()) {
		return 0;
	}

	if (debug) {
		debugPreStart();
		debugPrint("Rolling Back...");
		debugPrint("\n");
		debugPreEnd();
	}

	write((unsigned short)ROLLBACK);

	bool	response;
	if (read(&response)!=sizeof(bool)) {
		setError("Failed to get rollback status.\n A network error may have ocurred.");
		return -1;
	}
	return (response)?1:0;
}
