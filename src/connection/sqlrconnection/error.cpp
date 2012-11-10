// Copyright (c) 2012  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void sqlrconnection_svr::clearError() {
	setError(NULL,0,true);
}

void sqlrconnection_svr::setError(const char *err,
					int64_t errn,
					bool liveconn) {
	delete[] error;
	error=charstring::duplicate(err);
	errnum=errn;
	liveconnection=liveconn;
}

void sqlrconnection_svr::returnError() {

	// Get the error data if none is set already
	if (!error) {
		const char	*err=NULL;
		errorMessage(&err,&errnum,&liveconnection);
		error=charstring::duplicate(err);
	}

	// send the appropriate error status
	if (liveconnection) {
		clientsock->write((uint16_t)ERROR_OCCURRED);
	} else {
		clientsock->write((uint16_t)ERROR_OCCURRED_DISCONNECT);
	}

	// send the error code and error string
	clientsock->write((uint64_t)errnum);

	// send the error string
	size_t	errorlen=charstring::length(error);
	clientsock->write((uint16_t)errorlen);
	clientsock->write(error,errorlen);
}

void sqlrconnection_svr::returnError(sqlrcursor_svr *cursor) {

	dbgfile.debugPrint("connection",2,"returning error...");

	// send the appropriate error status
	if (cursor->liveconnection) {
		clientsock->write((uint16_t)ERROR_OCCURRED);
	} else {
		clientsock->write((uint16_t)ERROR_OCCURRED_DISCONNECT);
	}

	// send the error code
	clientsock->write((uint64_t)cursor->errnum);

	// send the error string
	size_t	errorlen=charstring::length(cursor->error);
	clientsock->write((uint16_t)errorlen);
	clientsock->write(cursor->error,errorlen);

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
