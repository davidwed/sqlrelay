// Copyright (c) 2012  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void sqlrconnection_svr::clearError() {
	setError(NULL,0,true);
}

void sqlrconnection_svr::setError(const char *err,
					int64_t errn,
					bool liveconn) {
	errorlength=charstring::length(err);
	if (errorlength>conn->maxerrorlength) {
		errorlength=maxerrorlength;
	}
	charstring::copy(error,err,errorlength);
	errnum=errn;
	liveconnection=liveconn;
}

void sqlrconnection_svr::returnError(bool disconnect) {

	// Get the error data if none is set already
	if (!error) {
		errorMessage(error,maxerrorlength,
				&errorlength,&errnum,&liveconnection);
		if (!liveconnection) {
			disconnect=true;
		}
	}

	// send the appropriate error status
	if (disconnect) {
		clientsock->write((uint16_t)ERROR_OCCURRED_DISCONNECT);
	} else {
		clientsock->write((uint16_t)ERROR_OCCURRED);
	}

	// send the error code and error string
	clientsock->write((uint64_t)errnum);

	// send the error string
	clientsock->write((uint16_t)errorlength);
	clientsock->write(error,errorlength);
}

void sqlrconnection_svr::returnError(sqlrcursor_svr *cursor, bool disconnect) {

	dbgfile.debugPrint("connection",2,"returning error...");

	// send the appropriate error status
	if (disconnect) {
		clientsock->write((uint16_t)ERROR_OCCURRED_DISCONNECT);
	} else {
		clientsock->write((uint16_t)ERROR_OCCURRED);
	}

	// send the error code
	clientsock->write((uint64_t)cursor->errnum);

	// send the error string
	clientsock->write((uint16_t)cursor->errorlength);
	clientsock->write(cursor->error,cursor->errorlength);

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
