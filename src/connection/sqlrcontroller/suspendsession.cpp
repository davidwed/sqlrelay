// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrcontroller.h>

void sqlrcontroller_svr::suspendSessionCommand() {

	dbgfile.debugPrint("connection",1,"suspending session...");

	// mark the session suspended
	suspendedsession=true;

	// we can't wait forever for the client to resume, set a timeout
	accepttimeout=cfgfl->getSessionTimeout();

	// abort all cursors that aren't suspended...
	dbgfile.debugPrint("connection",2,"aborting busy cursors...");
	for (int32_t i=0; i<cursorcount; i++) {
		if (cur[i]->state==SQLRCURSORSTATE_BUSY) {
			cur[i]->abort();
		}
	}
	dbgfile.debugPrint("connection",2,"done aborting busy cursors");

	// If we're passing file descriptors around, we'll have to listen on a 
	// set of ports so the suspended client has something to resume to.
	// It's possible that the current session is just a resumed session
	// though.  In that case, no new sockets will be opened by the call to
	// openSockets(), the old ones will just be reused.
	if (cfgfl->getPassDescriptor()) {

		// open sockets to resume on
		dbgfile.debugPrint("connection",2,
					"opening sockets to resume on...");
		uint16_t	unixsocketsize=0;
		uint16_t	inetportnumber=0;
		if (openSockets()) {
			if (serversockun) {
				unixsocketsize=charstring::length(unixsocket);
			}
			inetportnumber=inetport;
		}
		dbgfile.debugPrint("connection",2,
					"done opening sockets to resume on");

		// pass the socket info to the client
		dbgfile.debugPrint("connection",2,
				"passing socket info to client...");
		clientsock->write(unixsocketsize);
		if (unixsocketsize) {
			clientsock->write(unixsocket,unixsocketsize);
		}
		clientsock->write(inetportnumber);
		flushWriteBuffer();
		dbgfile.debugPrint("connection",2,
				"done passing socket info to client");
	}

	dbgfile.debugPrint("connection",1,"done suspending session");
}
