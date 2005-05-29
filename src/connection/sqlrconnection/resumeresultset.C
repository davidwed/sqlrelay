// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

bool sqlrconnection::resumeResultSetCommand(sqlrcursor *cursor) {
	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"resume result set");
	#endif
	resumeResultSet(cursor);
	if (!returnResultSetData(cursor)) {
		endSession();
		return false;
	}
	return true;
}

void sqlrconnection::resumeResultSet(sqlrcursor *cursor) {

	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"resume result set...");
	#endif

	if (cursor->suspendresultset) {

		#ifdef SERVER_DEBUG
		debugPrint("connection",2,"previous result set was suspended");
		#endif

		// indicate that no error has occurred
		clientsock->write((uint16_t)NO_ERROR);

		// send the client the id of the 
		// cursor that it's going to use
		clientsock->write(cursor->id);
		clientsock->write((uint16_t)SUSPENDED_RESULT_SET);

		// if the requested cursor really had a suspended
		// result set, send the lastrow of it to the client
		// then send the result set header
		clientsock->write((uint32_t)lastrow);
		returnResultSetHeader(cursor);

	} else {

		#ifdef 	SERVER_DEBUG
		debugPrint("connection",2,
				"previous result set was not suspended");
		#endif

		// indicate that an error has occurred
		clientsock->write((uint16_t)ERROR);

		// send the error itself
		clientsock->write((uint16_t)43);
		clientsock->write("The requested result set was not suspended.",
					43);
	}

	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"done resuming result set");
	#endif
}
