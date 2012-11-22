// Copyright (c) 2007  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void sqlrconnection_svr::dbVersionCommand() {

	dbgfile.debugPrint("connection",1,"db version");

	// get the db version
	const char	*dbversion=dbVersion();

	// send it to the client
	uint16_t	dbvlen=charstring::length(dbversion);
	clientsock->write(dbvlen);
	clientsock->write(dbversion,dbvlen);
	flushWriteBuffer();
}
