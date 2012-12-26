// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrclient.h>
#include <rudiments/error.h>

bool sqlrconnection::openSession() {

	if (connected) {
		return true;
	}

	if (debug) {
		debugPreStart();
		debugPrint("Connecting to listener...");
		debugPrint("\n");
		debugPreEnd();
	}

	// open a connection to the listener
	int	openresult=RESULT_ERROR;

	// first, try for a unix connection
	if (listenerunixport && listenerunixport[0]) {

		if (debug) {
			debugPreStart();
			debugPrint("Unix socket: ");
			debugPrint(listenerunixport);
			debugPrint("\n");
			debugPreEnd();
		}

		ucs.useBlockingMode();
		openresult=ucs.connect(listenerunixport,
						connecttimeoutsec,
						connecttimeoutusec,
						retrytime,tries);
		if (openresult==RESULT_SUCCESS) {
			cs=&ucs;
		}
	}

	// then try for an inet connection
	if (openresult!=RESULT_SUCCESS && listenerinetport) {

		if (debug) {
			debugPreStart();
			debugPrint("Inet socket: ");
			debugPrint(server);
			debugPrint(":");
			debugPrint((int64_t)listenerinetport);
			debugPrint("\n");
			debugPreEnd();
		}

		ics.useBlockingMode();
		openresult=ics.connect(server,listenerinetport,
						connecttimeoutsec,
						connecttimeoutusec,
						retrytime,tries);
		if (openresult==RESULT_SUCCESS) {
			cs=&ics;
		}
	}

	// handle failure to connect to listener
	if (openresult!=RESULT_SUCCESS) {
		setError("Couldn't connect to the listener.");
		return false;
	}

	// use 8k read and write buffers
	cs->dontUseNaglesAlgorithm();
	// FIXME: make these buffer sizes user-configurable
	// SO_SNDBUF=0 causes no data to ever be sent on openbsd
	//cs->setTcpReadBufferSize(8192);
	//cs->setTcpWriteBufferSize(0);
	cs->setReadBufferSize(8192);
	cs->setWriteBufferSize(8192);
	cs->useBlockingMode();

	// authenticate
	authenticate();

	// if we made it here then everything went well and we are successfully
	// connected and authenticated with the connection daemon
	connected=true;
	return true;
}
