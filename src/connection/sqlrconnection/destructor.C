// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <config.h>
#include <sqlrconnection.h>

#ifdef HAVE_UNISTD_H
	#include <unistd.h>
#endif

sqlrconnection::~sqlrconnection() {

	delete cmdl;
	delete cfgfl;
	delete sclrcom;
	delete ussf;
	delete lsnrcom;

	delete[] updown;

	delete tmpdir;

	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"deleting authc...");
	#endif
	delete authc;
	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"done deleting authc");
	#endif

	delete ipcptr;

	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"deleting unixsocket...");
	#endif
	if (unixsocket) {
		unlink(unixsocket);
		delete[] unixsocket;
	}
	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"done deleting unixsocket");
	#endif

	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"deleting bindpool...");
	#endif
	delete bindpool;
	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"done deleting bindpool");
	#endif
}
