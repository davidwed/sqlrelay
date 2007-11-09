// Copyright (c) 2007  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void sqlrconnection_svr::serverVersionCommand() {

	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"server version");
	#endif

	const char	*svrversion=SERVER_VERSION;
	uint16_t	svrvlen=(uint16_t)charstring::length(svrversion);
	clientsock->write(svrvlen);
	clientsock->write(svrversion,svrvlen);
	flushWriteBuffer();
}
