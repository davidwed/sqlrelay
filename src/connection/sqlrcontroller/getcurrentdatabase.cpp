// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrcontroller.h>

void sqlrcontroller_svr::getCurrentDatabaseCommand() {

	dbgfile.debugPrint("connection",1,"get current database");

	// get the current database
	char	*currentdb=conn->getCurrentDatabase();

	// send it to the client
	uint16_t	currentdbsize=charstring::length(currentdb);
	clientsock->write(currentdbsize);
	clientsock->write(currentdb,currentdbsize);
	flushWriteBuffer();

	// clean up
	delete[] currentdb;
}
