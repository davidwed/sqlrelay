// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <config.h>
#include <sqlrconnection.h>
#include <rudiments/file.h>

sqlrconnection::~sqlrconnection() {

	delete cmdl;
	delete cfgfl;

	delete[] updown;

	delete tmpdir;

	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"deleting authc...");
	#endif
	delete authc;
	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"done deleting authc");
	#endif

	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"deleting idmemory...");
	#endif
	delete idmemory;
	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"done deleting idmemory");
	#endif

	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"deleting semset...");
	#endif
	delete semset;
	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"done deleting semset");
	#endif

	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"deleting unixsocket...");
	#endif
	if (unixsocket) {
		file::remove(unixsocket);
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
