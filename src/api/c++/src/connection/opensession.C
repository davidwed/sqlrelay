// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrclient.h>
#include <errno.h>

int	sqlrconnection::openSession() {

	if (connected) {
		return 1;
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

		openresult=unixclientsocket::
				connectToServer(listenerunixport,
						-1,-1,retrytime,tries);
	}

	// then try for an inet connection
	if (openresult!=RESULT_SUCCESS && listenerinetport) {

		if (debug) {
			debugPreStart();
			debugPrint("Inet socket: ");
			debugPrint(server);
			debugPrint(":");
			debugPrint((long)listenerinetport);
			debugPrint("\n");
			debugPreEnd();
		}

		openresult=inetclientsocket::
				connectToServer(server,listenerinetport,
						-1,-1,retrytime,tries);
	}

	// handle failure to connect to listener
	if (openresult!=RESULT_SUCCESS) {
		setError("Couldn't connect to the listener.");
		return 0;
	}

	// authenticate with the listner
	if (authenticateWithListener()<1) {
		closeConnection();
		return 0;
	}

	// do we need to reconnect to the connection daemon
	getReconnect();


	if (reconnect==-1) {

		// if an error ocurred, set the error and close the connection
		setError("Failed to get whether we need to reconnect.\n A network error may have ocurred.");
		closeConnection();
		return 0;

	} else if (!reconnect) {

		// if we don't need to reconnect, just authenticate with the
		// connection daemon
		if (!authenticateWithConnection()) {
			closeConnection();
			return 0;
		}
		connected=1;

	} else if (reconnect==1) {

		// if we do need to reconnect, get which port(s) to reconnect
		// to and reconnect

		// try to get the connection daemon ports
		int	success=getNewPort();

		// close the connection to the listener
		closeConnection();

		// If getNewPort() returns -1 or 0 then an error ocurred.
		// If it returns 0, the error is already set.
		if (success<1) {
			if (success==-1) {
				setError("Failed to get connection ports.\n A network error may have ocurred.");
			}
			return 0;
		}

		if (debug) {
			debugPreStart();
			debugPrint("Reconnecting to ");
			debugPrint("\n");
			debugPrint("	unix port: ");
			debugPrint(connectionunixport);
			debugPrint("\n");
			debugPrint("	inet port: ");
			debugPrint((long)connectioninetport);
			debugPrint("\n");
			debugPreEnd();
		}

		// first, try for the unix port
		if (listenerunixport && listenerunixport[0] &&
					connectionunixport) {
			connected=unixclientsocket::
					connectToServer(connectionunixport,
							-1,-1,retrytime,tries);
			if (debug && !connected) {
				debugPreStart();
				debugPrint("ERROR:\n");
				debugPrint("connection to unix port failed: ");
				debugPrint(strerror(errno));
				debugPrint("\n");
				debugPreEnd();
			}
		}

		// then try for the inet port
		if (!connected && connectioninetport) {
			connected=inetclientsocket::
					connectToServer(server,
							connectioninetport,
							-1,-1,retrytime,tries);
			if (debug && !connected) {
				debugPreStart();
				debugPrint("ERROR:\n");
				debugPrint("connection to inet port failed: ");
				debugPrint(strerror(errno));
				debugPrint("\n");
				debugPreEnd();
			}
		}

		// did we successfully reconnect?
		if (connected) {

			if (debug) {
				debugPreStart();
				debugPrint("Connected.");
				debugPrint("\n");
				debugPreEnd();
			}

			clearSessionFlags();

			if (!authenticateWithConnection()) {
				return 0;
			}

		} else {

			// handle failure to connect to database 
			// connection daemon
			stringbuffer	errstr;
			errstr.append("Couldn't connect to the database");
			errstr.append("connection daemon.\n");
			if (connectionunixport) {
				errstr.append("Tried unix port ");
				errstr.append((long)connectionunixport);
			}
			if (connectioninetport) {
				errstr.append("Tried inet port ");
				errstr.append((long)connectioninetport);
			}
			errstr.append("\n");
			setError(errstr.getString());
			return 0;
		}
	}

	// if we made it here then everything went well;  listener 
	// authentication succeeded and we either didn't have to reconnect or
	// we sucessfully reconnected and sucessfully authenticated with the 
	// connection daemon
	return 1;
}
