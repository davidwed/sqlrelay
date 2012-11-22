// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void sqlrconnection_svr::selectDatabaseCommand() {

	dbgfile.debugPrint("connection",1,"select database");

	// get length of db parameter
	uint32_t	dblen;
	if (clientsock->read(&dblen,idleclienttimeout,0)!=sizeof(uint32_t)) {
		dbgfile.debugPrint("connection",2,
			"get list failed: client sent bad db length");
		clientsock->write(false);
		return;
	}

	// bounds checking
	if (dblen>maxquerysize) {
		dbgfile.debugPrint("connection",2,
			"get list failed: client sent bad db length");
		clientsock->write(false);
		return;
	}

	// read the db parameter into the buffer
	char	*db=new char[dblen+1];
	if (dblen) {
		if ((uint32_t)(clientsock->read(db,dblen,
					idleclienttimeout,0))!=dblen) {
			dbgfile.debugPrint("connection",2,
				"get list failed: "
				"client sent short db parameter");
			clientsock->write(false);
			return;
		}
	}
	db[dblen]='\0';
	
	// Select the db and send back the result.  If we've been told to
	// ignore these calls, skip the actual call but act like it succeeded.
	bool	result=(ignoreselectdb)?true:selectDatabase(db);
	clientsock->write(result);

	// if there was an error, send it back
	if (!result) {
		clientsock->write(errorlength);
		clientsock->write(error,errorlength);
	}

	flushWriteBuffer();

	return;
}
