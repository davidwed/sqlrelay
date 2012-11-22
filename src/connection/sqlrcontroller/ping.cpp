// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void sqlrconnection_svr::pingCommand() {
	dbgfile.debugPrint("connection",1,"ping");
	bool	pingresult=ping();
	clientsock->write(pingresult);
	flushWriteBuffer();
	if (!pingresult) {
		reLogIn();
	}
}
