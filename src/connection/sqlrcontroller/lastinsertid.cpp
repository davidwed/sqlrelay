// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void sqlrconnection_svr::getLastInsertIdCommand() {

	dbgfile.debugPrint("connection",1,"getting last insert id");

	// get the last insert id
	uint64_t	id;
	bool	success=getLastInsertId(&id);

	// send success/failure
	clientsock->write(success);
	if (success) {

		// return the id
		clientsock->write(id);

	} else {

		// return the error
		uint16_t	errorlen=charstring::length(error);
		clientsock->write(errorlen);
		clientsock->write(error,errorlen);
	}

	flushWriteBuffer();
}
