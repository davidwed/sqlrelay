// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrclient.h>

int sqlrconnection::resumeSession(int port, const char *socket) {

	// if already connected, end the session
	if (connected) {
		endSession();
	}

	// set the connectionunixport and connectioninetport values
	if (copyrefs) {
		if (strlen(socket)<=MAXPATHLEN) {
			strcpy(connectionunixportbuffer,socket);
			connectionunixport=connectionunixportbuffer;
		} else {
			connectionunixport="";
		}
	} else {
		connectionunixport=(char *)socket;
	}
	connectioninetport=(unsigned short)port;

	// first, try for the unix port
	if (socket && socket[0]) {
		connected=unixclientsocket::connectToServer(socket,
							-1,-1,retrytime,tries);
	}

	// then try for the inet port
	if (!connected) {
		connected=inetclientsocket::connectToServer(server,port,
							-1,-1,retrytime,tries);
	}

	if (debug) {
		debugPreStart();
		debugPrint("Resuming Session: ");
		debugPreEnd();
	}

	if (connected) {
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
