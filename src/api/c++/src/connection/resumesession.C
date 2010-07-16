// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrclient.h>

bool sqlrconnection::resumeSession(uint16_t port, const char *socket) {

	// if already connected, end the session
	if (connected) {
		endSession();
	}

	// set the connectionunixport and connectioninetport values
	if (copyrefs) {
		if (charstring::length(socket)<=MAXPATHLEN) {
			charstring::copy(connectionunixportbuffer,socket);
			connectionunixport=connectionunixportbuffer;
		} else {
			connectionunixport="";
		}
	} else {
		connectionunixport=(char *)socket;
	}
	connectioninetport=port;

	// first, try for the unix port
	if (socket && socket[0]) {
		if ((connected=ucs.connect(socket,-1,-1,retrytime,tries))) {
			cs=&ucs;
		}
	}

	// then try for the inet port
	if (!connected) {
		if ((connected=ics.connect(server,port,-1,-1,retrytime,tries))) {
			cs=&ics;
		}
	}

	if (debug) {
		debugPreStart();
		debugPrint("Resuming Session: ");
		debugPreEnd();
	}

	if (connected) {

		// use 8k read and write buffers
		cs->dontUseNaglesAlgorithm();
		// FIXME: use bandwidth delay product to tune these
		// SO_SNDBUF=0 causes no data to ever be sent on openbsd
		//cs->setTcpReadBufferSize(8192);
		//cs->setTcpWriteBufferSize(0);
		cs->setReadBufferSize(8192);
		cs->setWriteBufferSize(8192);

		if (debug) {
			debugPreStart();
			debugPrint("success");
			debugPrint("\n");
			debugPreEnd();
		}
		clearSessionFlags();
	} else {
		if (debug) {
			debugPreStart();
			debugPrint("failure");
			debugPrint("\n");
			debugPreEnd();
		}
	}

	return connected;
}
