// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>
#include <rudiments/process.h>

void sqlrconnection_svr::closeConnection() {

	if (inclientsession) {
		decrementClientSessionCount();
	}

	// decrement the connection counter
	if (decrementonclose && cfgfl->getDynamicScaling() &&
						semset && idmemory) {
		decrementConnectionCount();
	}

	// deregister and close the handoff socket if necessary
	if (cfgfl->getPassDescriptor()) {
		deRegisterForHandoff(tmpdir->getString());
	}

	// close the cursors
	closeCursors(true);


	// try to log out
	dbgfile.debugPrint("connection",0,"logging out...");
	logOutUpdateStats();
	dbgfile.debugPrint("connection",0,"done logging out");


	// clear the pool
	dbgfile.debugPrint("connection",0,"removing all sockets...");
	removeAllFileDescriptors();
	dbgfile.debugPrint("connection",0,"done removing all sockets");


	// close, clean up all sockets
	dbgfile.debugPrint("connection",0,"deleting unix socket...");
	delete serversockun;
	dbgfile.debugPrint("connection",0,"done deleting unix socket");


printf("%d count=%d/%d\n",process::getProcessId(),serversockincount,cfgfl->getAddressCount());
	dbgfile.debugPrint("connection",0,"deleting inetsockets...");
	for (uint64_t index=0; index<serversockincount; index++) {
printf("	%d index=%d\n",process::getProcessId(),index);
		delete serversockin[index];
	}
	delete[] serversockin;
	dbgfile.debugPrint("connection",0,"done deleting inet socket");
}
