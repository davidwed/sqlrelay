// Copyright (c) 2007  David Muse
// See the file COPYING for more information

#include <sqlrcontroller.h>

void sqlrcontroller_svr::serverVersionCommand() {

	dbgfile.debugPrint("connection",1,"server version");

	// get the server version
	const char	*svrversion=SQLR_VERSION;

	// send it to the client
	uint16_t	svrvlen=charstring::length(svrversion);
	clientsock->write(svrvlen);
	clientsock->write(svrversion,svrvlen);
	flushWriteBuffer();
}
