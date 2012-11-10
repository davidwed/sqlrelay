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
	// FIXME: use bandwidth delay product to tune these
	// SO_SNDBUF=0 causes no data to ever be sent on openbsd
	//cs->setTcpReadBufferSize(8192);
	//cs->setTcpWriteBufferSize(0);
	cs->setReadBufferSize(8192);
	cs->setWriteBufferSize(8192);

	// authenticate with the listner
	if (!authenticateWithListener()) {
		closeConnection();
		return false;
	}

	// do we need to reconnect to the connection daemon
	if (!getReconnect()) {
		closeConnection();
		return false;
	}


	if (!reconnect) {

		// if we don't need to reconnect, just authenticate with the
		// connection daemon
		if (!authenticateWithConnection()) {
			closeConnection();
			return false;
		}
		connected=true;

	} else {

		// if we do need to reconnect, get which port(s) to reconnect
		// to and reconnect
		bool	success=getNewPort();

		// close the connection to the listener
		closeConnection();

		// handle an error
		if (!success) {
			return false;
		}

		// first, try for the unix port
		if (listenerunixport && listenerunixport[0] &&
			connectionunixport && connectionunixport[0]) {

			if (debug) {
				debugPreStart();
				debugPrint("Reconnecting to \n");
				debugPrint("	unix port: ");
				debugPrint(connectionunixport);
				debugPrint("\n");
				debugPreEnd();
			}

			connected=(ucs.connect(connectionunixport,
						connecttimeoutsec,
						connecttimeoutusec,
						retrytime,tries)==
							RESULT_SUCCESS);
			if (connected) {
				cs=&ucs;
			}

			if (debug && !connected) {
				char	*err=error::getErrorString();
				debugPreStart();
				debugPrint("ERROR:\n");
				debugPrint("connection to unix port failed: ");
				debugPrint(err);
				debugPrint("\n");
				debugPreEnd();
				delete[] err;
			}
		}

		// then try for the inet port
		if (!connected && connectioninetport) {

			if (debug) {
				debugPreStart();
				debugPrint("Reconnecting to \n");
				debugPrint("	server: ");
				debugPrint(server);
				debugPrint("\n");
				debugPrint("	inet port: ");
				debugPrint((int64_t)connectioninetport);
				debugPrint("\n");
				debugPreEnd();
			}

			connected=(ics.connect(server,
						connectioninetport,
						connecttimeoutsec,
						connecttimeoutusec,
						retrytime,tries)==
							RESULT_SUCCESS);
			if (connected) {
				cs=&ics;
			}

			if (debug && !connected) {
				char	*err=error::getErrorString();
				debugPreStart();
				debugPrint("ERROR:\n");
				debugPrint("connection to inet port failed: ");
				debugPrint(err);
				debugPrint("\n");
				debugPreEnd();
				delete[] err;
			}
		}

		// did we successfully reconnect?
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
				debugPrint("Connected.");
				debugPrint("\n");
				debugPreEnd();
			}

			clearSessionFlags();

			if (!authenticateWithConnection()) {
				return false;
			}

		} else {

			// handle failure to connect to database 
			// connection daemon
			stringbuffer	errstr;
			errstr.append("Couldn't connect to the database");
			errstr.append("connection daemon.\n");
			if (connectionunixport) {
				errstr.append("Tried unix port ");
				errstr.append(connectionunixport);
			}
			if (connectioninetport) {
				errstr.append("Tried inet port ");
				errstr.append(connectioninetport);
			}
			errstr.append("\n");
			setError(errstr.getString());
			return false;
		}
	}

	// if we made it here then everything went well;  listener 
	// authentication succeeded and we either didn't have to reconnect or
	// we sucessfully reconnected and sucessfully authenticated with the 
	// connection daemon
	return true;
}
