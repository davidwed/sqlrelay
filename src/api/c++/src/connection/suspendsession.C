// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrclient.h>
#include <defines.h>

int	sqlrconnection::suspendSession() {

	if (!openSession()) {
		return 0;
	}

	if (debug) {
		debugPreStart();
		debugPrint("Suspending Session\n");
		debugPreEnd();
	}

	// suspend the session
	int	retval=0;
	write((unsigned short)SUSPEND_SESSION);
	suspendsessionsent=1;
	retval=1;

	// If the server is passing around file descriptors to handoff clients
	// from listener to connection, then it will have to open a socket and
	// port to enable suspend/resume.   It will pass that socket/port to
	// us here.
	if (!reconnect) {
		retval=getNewPort();
	}

	closeConnection();

	return retval;
}
