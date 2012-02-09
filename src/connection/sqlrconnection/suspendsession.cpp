// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void sqlrconnection_svr::suspendSessionCommand() {
	dbgfile.debugPrint("connection",1,"suspend session");
	suspendSession();
	flushWriteBuffer();
}

void sqlrconnection_svr::suspendSession() {

	dbgfile.debugPrint("connection",1,"suspending session...");

	// abort all cursors that aren't already suspended
	dbgfile.debugPrint("connection",2,"aborting busy, unsuspended cursors...");
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

	dbgfile.debugPrint("connection",2,"done aborting busy, unsuspended cursors");

	// If we're passing file descriptors around, we'll have to listen on a 
	// set of ports like we would if we were not passing descriptors around 
	// so the suspended client has something to resume to.  It's possible 
	// that the current session is just a resumed session though.  In that
	// case, no new sockets will be opened, the old ones will just be 
	// reused.  We'll also have to pass the socket/port to the client here.
	if (cfgfl->getPassDescriptor()) {

		dbgfile.debugPrint("connection",2,"opening a socket to resume on...");
		if (!openSockets()) {
			// send the client a 0 sized unix port and a 0 for the
			// inet port if an error occurred opening the sockets
			clientsock->write((uint16_t)0);
			clientsock->write((uint16_t)0);
		}
		dbgfile.debugPrint("connection",2,"done opening a socket to resume on");

		dbgfile.debugPrint("connection",2,"passing socket info to client...");
		if (serversockun) {
			uint16_t	unixsocketsize=
					charstring::length(unixsocket);
			clientsock->write(unixsocketsize);
			clientsock->write(unixsocket,unixsocketsize);
		} else {
			clientsock->write((uint16_t)0);
		}
		clientsock->write(inetport);
		dbgfile.debugPrint("connection",2,
				"done passing socket info to client...");
	}

	dbgfile.debugPrint("connection",2,"done suspending session");
}
