// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrclient.h>
#include <defines.h>

int	sqlrconnection::ping() {

	if (!openSession()) {
		return 0;
	}

	if (debug) {
		debugPreStart();
		debugPrint("Pinging...");
		debugPrint("\n");
		debugPreEnd();
	}

	write((unsigned short)PING);

	// get the ping result
	unsigned short	result;
	if (read(&result)!=sizeof(unsigned short)) {
		setError("Failed to ping.\n A network error may have ocurred.");
		result=0;
	}
	return (int)result;
}
