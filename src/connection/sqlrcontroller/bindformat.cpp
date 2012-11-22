// Copyright (c) 2007  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void sqlrconnection_svr::bindFormatCommand() {

	dbgfile.debugPrint("connection",1,"bind format");

	// get the bind format
	const char	*bf=bindFormat();

	// send it to the client
	uint16_t	bflen=charstring::length(bf);
	clientsock->write(bflen);
	clientsock->write(bf,bflen);
	flushWriteBuffer();
}
