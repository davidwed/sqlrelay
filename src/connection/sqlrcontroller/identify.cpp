// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrcontroller.h>

void sqlrcontroller_svr::identifyCommand() {

	dbgfile.debugPrint("connection",1,"identify");

	// get the identification
	const char	*ident=conn->identify();

	// send it to the client
	uint16_t	idlen=charstring::length(ident);
	clientsock->write(idlen);
	clientsock->write(ident,idlen);
	flushWriteBuffer();
}
