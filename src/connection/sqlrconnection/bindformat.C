// Copyright (c) 2007  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void sqlrconnection_svr::bindFormatCommand() {

	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"bind format");
	#endif

	const char	*bf=bindFormat();
	uint16_t	bflen=(uint16_t)charstring::length(bf);
	clientsock->write(bflen);
	clientsock->write(bf,bflen);
	flushWriteBuffer();
}

const char *sqlrconnection_svr::bindFormat() {
	return ":*";
}
