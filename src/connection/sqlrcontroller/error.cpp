// Copyright (c) 2012  David Muse
// See the file COPYING for more information

#include <sqlrcontroller.h>

void sqlrcontroller_svr::returnError(bool disconnect) {

	// Get the error data if none is set already
	if (!conn->error) {
		conn->errorMessage(conn->error,maxerrorlength,
				&conn->errorlength,&conn->errnum,
				&conn->liveconnection);
		if (!conn->liveconnection) {
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
	clientsock->write((uint64_t)conn->errnum);

	// send the error string
	clientsock->write((uint16_t)conn->errorlength);
	clientsock->write(conn->error,conn->errorlength);
}

void sqlrcontroller_svr::returnError(sqlrcursor_svr *cursor, bool disconnect) {

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
