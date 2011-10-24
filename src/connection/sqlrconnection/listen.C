// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>
#include <unistd.h>

bool sqlrconnection_svr::listen() {

	uint16_t	sessioncount=0;
	bool		clientconnectfailed=false;

	for (;;) {

		waitForAvailableDatabase();
		initSession();
		announceAvailability(tmpdir->getString(),
						cfgfl->getPassDescriptor(),
						unixsocket,
						inetport,
						connectionid);

		// loop to handle suspended sessions
		for (;;) {

			int	success=waitForClient();

			if (success==1) {

				suspendedsession=false;

				// have a session with the client
				clientSession();

				// break out of the loop unless the client
				// suspended the session
				if (!suspendedsession) {
					break;
				}

			} else if (success==-1) {

				// if waitForClient() errors out, break out of
				// the suspendedsession loop and loop back
				// for another session
				// the suspendedsession loop and
				// close connection if it is possible
				// otherwise wait for session, but
				// it seems that on hard load it's impossible
				// to change handoff socket for pid
				clientconnectfailed=true;
				break;

			} else {

				// if waitForClient() times out waiting for
				// someone to pick up the suspended
				// session, roll it back and kill it
				if (suspendedsession) {
					if (isTransactional()) {
						rollbackInternal();
					}
					suspendedsession=false;
				}
			}
		}

		if (cfgfl->getDynamicScaling()) {
			decrementSessionCount();
			if (clientconnectfailed) {
				return false;
			}
		}

		if (cfgfl->getDynamicScaling() && ttl &&
				cfgfl->getMaxSessionCount()) {
			sessioncount++;
			if (sessioncount==cfgfl->getMaxSessionCount()) {
				return true;
			}
		}
	}
}

void sqlrconnection_svr::waitForAvailableDatabase() {

	dbgfile.debugPrint("connection",0,"waiting for available database...");

	if (!availableDatabase()) {
		reLogIn();
		markDatabaseAvailable();
	}

	dbgfile.debugPrint("connection",0,"database is available");
}

bool sqlrconnection_svr::availableDatabase() {

	// return whether the file "updown" is there or not
	if (file::exists(updown)) {
		dbgfile.debugPrint("connection",0,"database is available");
		return true;
	} else {
		dbgfile.debugPrint("connection",0,"database is not available");
		return false;
	}
}

void sqlrconnection_svr::initSession() {

	dbgfile.debugPrint("connection",0,"initializing session...");

	commitorrollback=false;
	suspendedsession=false;
	for (int32_t i=0; i<cursorcount; i++) {
		cur[i]->suspendresultset=false;
	}
	accepttimeout=5;

	dbgfile.debugPrint("connection",0,"done initializing session...");
}

int32_t sqlrconnection_svr::waitForClient() {

	dbgfile.debugPrint("connection",0,"waiting for client...");

	// FIXME: listen() checks for 1,-1 or 0 from this method, but this
	// method only returns 1 or -1????  0 should indicate that a suspended
	// session timed out...

	// Unless we're in the middle of a suspended session, if we're passing 
	// file descriptors around, wait for one to be passed to us, otherwise,
	// accept on the unix/inet sockets. 
	if (!suspendedsession && cfgfl->getPassDescriptor()) {

		// receive the descriptor and use it, if we failed to get the
		// descriptor, delete the socket and return failure
		int	descriptor;
		if (!receiveFileDescriptor(&descriptor)) {

			dbgfile.debugPrint("connection",1,"pass failed");
			dbgfile.debugPrint("connection",0,"done waiting for client");

			return -1;
		}

		clientsock=new filedescriptor;
		clientsock->setFileDescriptor(descriptor);

		dbgfile.debugPrint("connection",1,"pass succeeded");
		dbgfile.debugPrint("connection",0,"done waiting for client");

	} else {

		if (waitForNonBlockingRead(accepttimeout,0)<1) {
			dbgfile.debugPrint("connection",0,"wait for non blocking read failed");
			return -1;
		}

		// get the first socket that had data available...
		filedescriptor	*fd=NULL;
		if (!getReadyList()->getDataByIndex(0,&fd)) {
			dbgfile.debugPrint("connection",0,"ready list was empty");
			return -1;
		}

		inetserversocket	*iss=NULL;
		for (uint64_t index=0; index<serversockincount; index++) {
			if (fd==serversockin[index]) {
				iss=serversockin[index];
			}
		}
		if (iss) {
			clientsock=iss->accept();
		} else if (fd==serversockun) {
			clientsock=serversockun->accept();
		}

		if (fd) {
			dbgfile.debugPrint("connection",1,"reconnect succeeded");
		} else {
			dbgfile.debugPrint("connection",1,"reconnect failed");
		}
		dbgfile.debugPrint("connection",0,"done waiting for client");

		if (!fd) {
			return -1;
		}
	}

	clientsock->translateByteOrder();
	clientsock->dontUseNaglesAlgorithm();
	// FIXME: use bandwidth delay product to tune these
	// SO_SNDBUF=0 causes no data to ever be sent on openbsd
	//clientsock->setTcpReadBufferSize(8192);
	//clientsock->setTcpWriteBufferSize(0);
	clientsock->setReadBufferSize(8192);
	clientsock->setWriteBufferSize(8192);
	return 1;
}
