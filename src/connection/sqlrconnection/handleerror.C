// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

bool sqlrconnection::handleError(sqlrcursor *cursor) {

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"handling error...");
	#endif

	// return the error unless the error was a dead connection, 
	// in which case, re-establish the connection
	if (!returnError(cursor)) {
		#ifdef SERVER_DEBUG
		debugPrint("connection",3,"database is down...");
		#endif
		reLogIn();
		return false;
	}

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"done handling error...");
	#endif
	return true;
}

bool sqlrconnection::returnError(sqlrcursor *cursor) {

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"returning error...");
	#endif

	// get the error message from the database
	// return value: 1 if database connection is still alive, 0 if not
	bool	liveconnection;
	char	*error=cursor->getErrorMessage(&liveconnection);

	// only return an error message if the error wasn't a dead database
	if (liveconnection) {

		// indicate that an error has occurred
		clientsock->write((unsigned short)ERROR);

		// send the error itself
		int	errorlen=strlen(error);
		clientsock->write((unsigned short)(errorlen+
				strlen(cursor->querybuffer)+18));
		clientsock->write(error,errorlen);

		// send the attempted query back too
		clientsock->write("\nAttempted Query:\n");
		clientsock->write(cursor->querybuffer);
	}
	
	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"done returning error");
	#endif

	return liveconnection;
}
