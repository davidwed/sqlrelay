// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrclient.h>
#include <defines.h>

bool sqlrconnection::suspendSession() {

	if (!openSession()) {
		return 0;
	}

	clearError();

	if (debug) {
		debugPreStart();
		debugPrint("Suspending Session\n");
		debugPreEnd();
	}

	// suspend the session
	cs->write((uint16_t)SUSPEND_SESSION);
	flushWriteBuffer();
	suspendsessionsent=true;

	// check for error
	if (gotError()) {
		return false;
	}

	// get port to resume on
	bool	retval=getNewPort();

	closeConnection();

	return retval;
}
