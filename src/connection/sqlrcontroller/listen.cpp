// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrcontroller.h>
#include <rudiments/snooze.h>
#include <unistd.h>

bool sqlrcontroller_svr::listen() {

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
		bool	loopback=false;
		for (;;) {

			int	success=waitForClient();

			if (success==1) {

				// update the stats with the client address
				setClientAddr();

				suspendedsession=false;

				// have a session with the client
				clientSession();

				// break out of the loop unless the client
				// suspended the session
				if (!suspendedsession) {
					break;
				}

			} else if (success==2) {

				// this is a special case, basically it means
				// that the listener wants the connection to
				// reconnect to the database, just loop back
				// so that can be handled naturally
				loopback=true;
				break;

			} else if (success==-1) {

				// if waitForClient() errors out, break out of
				// the suspendedsession loop and loop back
				// for another session and close connection if
				// it is possible otherwise wait for session,
				// but it seems that on hard load it's
				// impossible to change handoff socket for pid
				clientconnectfailed=true;
				break;

			} else {

				// if waitForClient() times out waiting for
				// someone to pick up the suspended
				// session, roll it back and kill it
				if (suspendedsession) {
					if (conn->isTransactional()) {
						rollback();
					}
					suspendedsession=false;
				}
			}
		}

		if (!loopback && cfgfl->getDynamicScaling()) {

			decrementSessionCount();

			if (scalerspawned) {

				if (clientconnectfailed) {
					return false;
				}

				if (ttl==0) {
					return true;
				}

				if (ttl>0 && cfgfl->getMaxSessionCount()) {
					sessioncount++;
					if (sessioncount==
						cfgfl->getMaxSessionCount()) {
						return true;
					}
				}
			}
		}
	}
}

void sqlrcontroller_svr::waitForAvailableDatabase() {

	dbgfile.debugPrint("connection",0,"waiting for available database...");

	setState(WAIT_FOR_AVAIL_DB);

	if (!availableDatabase()) {
		reLogIn();
		markDatabaseAvailable();
	}

	dbgfile.debugPrint("connection",0,"database is available");
}

bool sqlrcontroller_svr::availableDatabase() {

	// return whether the file "updown" is there or not
	if (file::exists(updown)) {
		dbgfile.debugPrint("connection",0,"database is available");
		return true;
	} else {
		dbgfile.debugPrint("connection",0,"database is not available");
		return false;
	}
}

void sqlrcontroller_svr::initSession() {

	dbgfile.debugPrint("connection",0,"initializing session...");

	commitorrollback=false;
	suspendedsession=false;
	for (int32_t i=0; i<cursorcount; i++) {
		cur[i]->state=SQLRCURSOR_STATE_AVAILABLE;
	}
	accepttimeout=5;

	dbgfile.debugPrint("connection",0,"done initializing session...");
}

int32_t sqlrcontroller_svr::waitForClient() {

	dbgfile.debugPrint("connection",0,"waiting for client...");

	setState(WAIT_CLIENT);

	// FIXME: listen() checks for 2,1,0 or -1 from this method, but this
	// method only returns 2, 1 or -1.  0 should indicate that a suspended
	// session timed out.

	// Unless we're in the middle of a suspended session, if we're passing 
	// file descriptors around, wait for one to be passed to us, otherwise,
	// accept on the unix/inet sockets. 
	if (!suspendedsession && cfgfl->getPassDescriptor()) {

		// get what we're supposed to do...
		uint16_t	command;
		if (handoffsockun.read(&command)!=sizeof(uint16_t)) {
			dbgfile.debugPrint("connection",1,"read failed");
			dbgfile.debugPrint("connection",0,
						"done waiting for client");
			// If this fails, then the listener most likely died
			// because sqlr-stop was run.  Arguably this condition
			// should initiate a shut down of this process as well,
			// but for now we'll just wait to be shut down manually.
			// Unfortunatley, that means looping over and over,
			// with that read failing every time.  We'll sleep so
			// as not to slam the machine while we loop.
			snooze::microsnooze(0,100000);
			return -1;
		}

		// if we're supposed to reconnect, then just do that...
		if (command==HANDOFF_RECONNECT) {
			return 2;
		}

		// receive the descriptor and use it, if we failed to get the
		// descriptor, delete the socket and return failure
		int32_t	descriptor;
		if (!receiveFileDescriptor(&descriptor)) {
			dbgfile.debugPrint("connection",1,"pass failed");
			dbgfile.debugPrint("connection",0,
						"done waiting for client");
			// If this fails, then the listener most likely died
			// because sqlr-stop was run.  Arguably this condition
			// should initiate a shut down of this process as well,
			// but for now we'll just wait to be shut down manually.
			// Unfortunatley, that means looping over and over,
			// with that read above failing every time, thus the
			//  sleep so as not to slam the machine while we loop.
			return -1;
		}

		clientsock=new filedescriptor;
		clientsock->setFileDescriptor(descriptor);

		// For some reason, at least on OpenBSD 4.9, this
		// filedescriptor is getting created in non-blocking mode.
		// Force it into to blocking mode.
		clientsock->useBlockingMode();

		dbgfile.debugPrint("connection",1,"pass succeeded");
		dbgfile.debugPrint("connection",0,"done waiting for client");

	} else {

		if (waitForNonBlockingRead(accepttimeout,0)<1) {
			dbgfile.debugPrint("connection",0,
					"wait for non blocking read failed");
			// FIXME: I think this should return 0
			return -1;
		}

		// get the first socket that had data available...
		filedescriptor	*fd=NULL;
		if (!getReadyList()->getDataByIndex(0,&fd)) {
			dbgfile.debugPrint("connection",0,
						"ready list was empty");
			// FIXME: I think this should return 0
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
			dbgfile.debugPrint("connection",1,
						"reconnect succeeded");
		} else {
			dbgfile.debugPrint("connection",1,
						"reconnect failed");
		}
		dbgfile.debugPrint("connection",0,"done waiting for client");

		if (!fd) {
			// FIXME: I think this should return 0
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
