// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void sqlrconnection_svr::identifyCommand() {

	dbgfile.debugPrint("connection",1,"identify");

	const char	*ident=identify();
	uint16_t	idlen=(uint16_t)charstring::length(ident);
	clientsock->write(idlen);
	clientsock->write(ident,idlen);
	flushWriteBuffer();
}
