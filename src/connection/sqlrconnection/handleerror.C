// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

bool sqlrconnection_svr::handleError(sqlrcursor_svr *cursor) {

	dbgfile.debugPrint("connection",2,"handling error...");

	// return the error unless the error was a dead connection, 
	// in which case, re-establish the connection
	if (!returnError(cursor)) {
		dbgfile.debugPrint("connection",3,"database is down...");
		reLogIn();
		return false;
	}

	dbgfile.debugPrint("connection",2,"done handling error...");
	return true;
}

bool sqlrconnection_svr::returnError(sqlrcursor_svr *cursor) {

	dbgfile.debugPrint("connection",2,"returning error...");

	// get the error message from the database
	// return value: 1 if database connection is still alive, 0 if not
	bool		liveconnection;
	const char	*error=cursor->errorMessage(&liveconnection);

	// only return an error message if the error wasn't a dead database
	if (liveconnection) {

		// indicate that an error has occurred
		clientsock->write((uint16_t)ERROR);

		// send the error itself
		int	errorlen=charstring::length(error);
		
		#ifdef RETURN_QUERY_WITH_ERROR
			clientsock->write((uint16_t)(errorlen+
				charstring::length(cursor->querybuffer)+18));
			clientsock->write(error,errorlen);
			// send the attempted query back too
			clientsock->write("\nAttempted Query:\n");
			clientsock->write(cursor->querybuffer);
		#else
			clientsock->write((uint16_t)(errorlen));
			clientsock->write(error);
		#endif

		// client will be sending skip/fetch,
		// better get it even though we're not gonna
		// use it
		uint64_t	skipfetch;
		clientsock->read(&skipfetch,idleclienttimeout,0);
		clientsock->read(&skipfetch,idleclienttimeout,0);

		// Even though there was an error, we still 
		// need to send the client the id of the 
		// cursor that it's going to use.
		clientsock->write(cursor->id);
		flushWriteBuffer();


		dbgfile.debugPrint("connection",1,"failed to handle query: error");
	}
	
	dbgfile.debugPrint("connection",2,"done returning error");

	return liveconnection;
}
