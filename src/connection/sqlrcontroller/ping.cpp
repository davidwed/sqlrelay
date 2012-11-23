// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrcontroller.h>

void sqlrcontroller_svr::pingCommand() {
	dbgfile.debugPrint("connection",1,"ping");
	bool	pingresult=conn->ping();
	clientsock->write(pingresult);
	flushWriteBuffer();
	if (!pingresult) {
		reLogIn();
	}
}
