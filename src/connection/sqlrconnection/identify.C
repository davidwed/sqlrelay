// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void sqlrconnection::identifyCommand() {

	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"identify");
	#endif

	const char	*ident=identify();
	unsigned short	idlen=(unsigned short)charstring::length(ident);
printf("idlen=%d\n",idlen);
	clientsock->write(idlen);
printf("ident=%s\n",ident);
	clientsock->write(ident,idlen);
	flushWriteBuffer();
}
