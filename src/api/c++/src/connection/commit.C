// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrclient.h>
#include <defines.h>

int sqlrconnection::commit() {

	if (!openSession()) {
		return 0;
	}

	if (debug) {
		debugPreStart();
		debugPrint("Committing...");
		debugPrint("\n");
		debugPreEnd();
	}

	write((unsigned short)COMMIT);

	bool	response;
	if (read(&response)!=sizeof(bool)) {
		setError("Failed to get commit status.\n A network error may have ocurred.");
		return -1;
	}
	return (response)?1:0;
}
