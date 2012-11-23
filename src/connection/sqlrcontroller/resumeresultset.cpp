// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrcontroller.h>

bool sqlrcontroller_svr::resumeResultSetCommand(sqlrcursor_svr *cursor) {
	dbgfile.debugPrint("connection",1,"resume result set...");

	bool	retval=true;

	if (cursor->state==SQLRCURSOR_STATE_SUSPENDED) {

		dbgfile.debugPrint("connection",2,
				"previous result set was suspended");

		// indicate that no error has occurred
		clientsock->write((uint16_t)NO_ERROR_OCCURRED);

		// send the client the id of the 
		// cursor that it's going to use
		clientsock->write(cursor->id);
		clientsock->write((uint16_t)SUSPENDED_RESULT_SET);

		// if the requested cursor really had a suspended
		// result set, send the lastrow of it to the client
		// then resume the result set
		clientsock->write(cursor->lastrow);

		returnResultSetHeader(cursor);
		retval=returnResultSetData(cursor);

	} else {

		dbgfile.debugPrint("connection",2,
				"previous result set was not suspended");

		// indicate that an error has occurred
		clientsock->write((uint16_t)ERROR_OCCURRED);

		// send the error code (zero for now)
		clientsock->write((uint64_t)SQLR_ERROR_RESULTSETNOTSUSPENDED);

		// send the error itself
		uint16_t	len=charstring::length(
				SQLR_ERROR_RESULTSETNOTSUSPENDED_STRING);
		clientsock->write(len);
		clientsock->write(SQLR_ERROR_RESULTSETNOTSUSPENDED_STRING,len);

		retval=false;
	}

	dbgfile.debugPrint("connection",1,"done resuming result set");
	return retval;
}
