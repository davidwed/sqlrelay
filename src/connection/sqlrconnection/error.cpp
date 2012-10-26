// Copyright (c) 2012  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void sqlrconnection_svr::returnQueryError(sqlrcursor_svr *cursor,
						const char *error,
						int64_t errnum,
						bool disconnect) {

	dbgfile.debugPrint("connection",2,"returning error...");

	// indicate that an error has occurred
	if (disconnect) {
		clientsock->write((uint16_t)ERROR_OCCURRED_DISCONNECT);
	} else {
		clientsock->write((uint16_t)ERROR_OCCURRED);
	}

	// send the error code
	clientsock->write((uint64_t)errnum);

	// send the error string
	size_t	errorlen=charstring::length(error);
		
	#ifdef RETURN_QUERY_WITH_ERROR
		clientsock->write((uint16_t)(errorlen+
			charstring::length(cursor->querybuffer)+18));
		clientsock->write(error,errorlen);
		// send the attempted query back too
		clientsock->write("\nAttempted Query:\n");
		clientsock->write(cursor->querybuffer);
	#else
		clientsock->write((uint16_t)(errorlen));
		clientsock->write(error,errorlen);
	#endif

	// client will be sending skip/fetch, better get
	// it even though we're not going to use it
	uint64_t	skipfetch;
	clientsock->read(&skipfetch,idleclienttimeout,0);
	clientsock->read(&skipfetch,idleclienttimeout,0);

	// Even though there was an error, we still 
	// need to send the client the id of the 
	// cursor that it's going to use.
	clientsock->write(cursor->id);
	flushWriteBuffer();

	dbgfile.debugPrint("connection",2,"done returning error");
}

void sqlrconnection_svr::returnTransactionError() {

	// Get the error data.  If the db api didn't have a commit function
	// then txerror will be set, grab it from there.  If it did, then grab
	// it from the connection-level error message.
	const char	*err;
	int64_t		errnum;
	bool		liveconnection;
	if (txerror) {
		err=txerror;
		errnum=txerrnum;
		liveconnection=txliveconnection;
	} else {
		errorMessage(&err,&errnum,&liveconnection);
	}

	// send the appropriate error status
	if (liveconnection) {
		clientsock->write((uint16_t)ERROR_OCCURRED);
	} else {
		clientsock->write((uint16_t)ERROR_OCCURRED_DISCONNECT);
	}

	// send the error code and error string
	clientsock->write((uint64_t)errnum);
	size_t	errorlen=charstring::length(err);
	clientsock->write((uint16_t)errorlen);
	clientsock->write(err,errorlen);

	// clean up if necessary
	if (txerror) {
		delete[] txerror;
		txerror=NULL;
	}
}
