// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

int	sqlrconnection::resumeResultSetCommand() {
	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"resume result set");
	#endif
	resumeResultSet();
	if (!returnResultSetData()) {
		endSession();
		return 0;
	}
	return 1;
}

void	sqlrconnection::resumeResultSet() {

	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"resume result set...");
	#endif

	if (cur[currentcur]->suspendresultset) {

		#ifdef SERVER_DEBUG
		debugPrint("connection",2,"previous result set was suspended");
		#endif

		// indicate that no error has occurred
		clientsock->write((unsigned short)NO_ERROR);

		// send the client the id of the 
		// cursor that it's going to use
		clientsock->write((unsigned short)(currentcur));
		clientsock->write((unsigned short)SUSPENDED_RESULT_SET);

		// if the requested cursor really had a suspended
		// result set, send the lastrow of it to the client
		// then send the result set header
		clientsock->write((unsigned long)lastrow);
		returnResultSetHeader();

	} else {

		#ifdef 	SERVER_DEBUG
		debugPrint("connection",2,
				"previous result set was not suspended");
		#endif

		// indicate that an error has occurred
		clientsock->write((unsigned short)ERROR);

		// send the error itself
		clientsock->write((unsigned short)43);
		clientsock->write("The requested result set was not suspended.",
					43);
	}

	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"done resuming result set");
	#endif
}
