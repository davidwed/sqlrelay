// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void sqlrconnection::suspendSessionCommand() {
	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"suspend session");
	#endif
	suspendSession();
}

void sqlrconnection::suspendSession() {

	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"suspending session...");
	#endif

	// abort all cursors that aren't already suspended
	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"aborting busy, unsuspended cursors...");
	#endif
	suspendedsession=true;
	accepttimeout=cfgfl->getSessionTimeout();
	for (int i=0; i<cfgfl->getCursors(); i++) {
		if (!cur[i]->suspendresultset && cur[i]->busy) {

			#ifdef SERVER_DEBUG
			debugPrint("connection",3,(long)i);
			#endif

			// Very important...
			// Do not cleanUpData() here, otherwise result sets
			// that were suspended after the entire result set was
			// fetched won't be able to return column data when
			// resumed.
			cur[i]->abort();
		}
	}

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"done aborting busy, unsuspended cursors");
	#endif

	// If we're passing file descriptors around, we'll have to listen on a 
	// set of ports like we would if we were not passing descriptors around 
	// so the suspended client has something to resume to.  It's possible 
	// that the current session is just a resumed session though.  In that
	// case, no new sockets will be opened, the old ones will just be 
	// reused.  We'll also have to pass the socket/port to the client here.
	if (cfgfl->getPassDescriptor()) {

		#ifdef SERVER_DEBUG
		debugPrint("connection",2,"opening a socket to resume on...");
		#endif
		if (!openSockets()) {
			// send the client a 0 sized unix port and a 0 for the
			// inet port if an error occurred opening the sockets
			clientsock->write((unsigned short)0);
			clientsock->write((unsigned short)0);
		}
		#ifdef SERVER_DEBUG
		debugPrint("connection",2,"done opening a socket to resume on");
		#endif

		#ifdef SERVER_DEBUG
		debugPrint("connection",2,"passing socket info to client...");
		#endif
		if (serversockun) {
			unsigned short	unixsocketsize=strlen(unixsocket);
			clientsock->write(unixsocketsize);
			clientsock->write(unixsocket,unixsocketsize);
		} else {
			clientsock->write((unsigned short)0);
		}
		clientsock->write(inetport);
		#ifdef SERVER_DEBUG
		debugPrint("connection",2,
				"done passing socket info to client...");
		#endif
	}

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"done suspending session");
	#endif
}
