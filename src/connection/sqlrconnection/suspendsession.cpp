// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void sqlrconnection_svr::suspendSessionCommand() {

	dbgfile.debugPrint("connection",1,"suspend session");

	// suspend the session
	bool	success=suspendSession();

	// If we're passing file descriptors around, the suspendSession call
	// will have opened a set of ports so the suspended client has
	// something to resume to.  Pass that data to the client here.
	if (cfgfl->getPassDescriptor()) {

		dbgfile.debugPrint("connection",1,
				"passing socket info to client...");

		// get the unix and inet socket data
		uint16_t	unixsocketsize=0;
		uint16_t	inetportnumber=0;
		if (success) {
			if (serversockun) {
				unixsocketsize=charstring::length(unixsocket);
			}
			inetportnumber=inetport;
		}

		// send the 
		clientsock->write(unixsocketsize);
		if (unixsocketsize) {
			clientsock->write(unixsocket,unixsocketsize);
		}
		clientsock->write(inetportnumber);

		dbgfile.debugPrint("connection",1,
				"done passing socket info to client...");
	}

	flushWriteBuffer();
}

bool sqlrconnection_svr::suspendSession() {

	dbgfile.debugPrint("connection",1,"suspending session...");

	// abort all cursors that aren't already suspended
	dbgfile.debugPrint("connection",2,
				"aborting busy, unsuspended cursors...");
	suspendedsession=true;
	accepttimeout=cfgfl->getSessionTimeout();
	for (int32_t i=0; i<cfgfl->getCursors(); i++) {
		if (!cur[i]->suspendresultset && cur[i]->busy) {

			dbgfile.debugPrint("connection",3,i);

			// Very important...
			// Do not cleanUpData() here, otherwise result sets
			// that were suspended after the entire result set was
			// fetched won't be able to return column data when
			// resumed.
			cur[i]->abort();
		}
	}

	// end sid session
	if (cfgfl->getSidEnabled()) {
		sid_sqlrcon->endSession();
	}

	dbgfile.debugPrint("connection",2,
				"done aborting busy, unsuspended cursors");

	// If we're passing file descriptors around, we'll have to listen on a 
	// set of ports so the suspended client has something to resume to.
	// It's possible that the current session is just a resumed session
	// though.  In that case, no new sockets will be opened by the call to
	// openSockets(), the old ones will just be reused.
	bool	success=true;
	if (cfgfl->getPassDescriptor()) {
		dbgfile.debugPrint("connection",1,
					"opening sockets to resume on...");
		success=openSockets();
		dbgfile.debugPrint("connection",1,
					"done opening sockets to resume on");
	}

	dbgfile.debugPrint("connection",1,"done suspending session");
	return success;
}
