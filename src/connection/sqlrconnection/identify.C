// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>
#include <string.h>

void sqlrconnection::identifyCommand() {

	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"identify");
	#endif

	char		*ident=identify();
	unsigned short	idlen=(unsigned short)strlen(ident);
	clientsock->write(idlen);
	clientsock->write(ident,idlen);
}
