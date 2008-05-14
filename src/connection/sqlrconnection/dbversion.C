// Copyright (c) 2007  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void sqlrconnection_svr::dbVersionCommand() {

	dbgfile.debugPrint("connection",1,"db version");

	const char	*dbversion=dbVersion();
	uint16_t	dbvlen=(uint16_t)charstring::length(dbversion);
	clientsock->write(dbvlen);
	clientsock->write(dbversion,dbvlen);
	flushWriteBuffer();
}
