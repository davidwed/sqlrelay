// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void	sqlrconnection::closeConnection() {

	// decrement the connection counter
	if (cfgfl->getDynamicScaling() && ipcptr->initialized()) {
		sclrcom->decrementConnectionCount();
	}

	// deregister and close the handoff socket if necessary
	if (cfgfl->getPassDescriptor()) {
		lsnrcom->deRegisterForHandoff(tmpdir->getString());
	}

	// close the cursors
	closeCursors(1);


	// try to log out
	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"logging out...");
	#endif
	logOut();
	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"done logging out");
	#endif


	// clear the pool
	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"removing all sockets...");
	#endif
	removeAllFileDescriptors();
	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"done removing all sockets");
	#endif


	// close, clean up all sockets
	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"deleting unix socket...");
	#endif
	delete serversockun;
	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"done deleting unix socket");
	#endif


	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"deleting inet socket...");
	#endif
	delete serversockin;
	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"done deleting inet socket");
	#endif
}
