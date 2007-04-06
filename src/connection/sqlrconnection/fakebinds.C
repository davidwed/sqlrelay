// Copyright (c) 2007  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void sqlrconnection_svr::fakeBindsCommand() {

	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"fake binds");
	#endif

	clientsock->write(fakeBinds());
	flushWriteBuffer();
}

bool sqlrconnection_svr::fakeBinds() {
	return false;
}
