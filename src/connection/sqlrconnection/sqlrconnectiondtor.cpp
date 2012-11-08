// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <config.h>
#include <sqlrconnection.h>
#include <rudiments/file.h>
#include <rudiments/process.h>

sqlrconnection_svr::~sqlrconnection_svr() {

	querylog.flushWriteBuffer(-1,-1);
	delete[] querylogname;

	delete sid_sqlrcon;

	delete cmdl;
	delete cfgfl;

	delete[] txerror;

	delete[] updown;

	delete[] originaldb;

	delete tmpdir;

	dbgfile.debugPrint("connection",0,"deleting authc...");
	delete authc;
	dbgfile.debugPrint("connection",0,"done deleting authc");

	dbgfile.debugPrint("connection",0,"deleting idmemory...");
	delete idmemory;
	dbgfile.debugPrint("connection",0,"done deleting idmemory");

	dbgfile.debugPrint("connection",0,"deleting semset...");
	delete semset;
	dbgfile.debugPrint("connection",0,"done deleting semset");

	dbgfile.debugPrint("connection",0,"deleting unixsocket...");
	if (unixsocket) {
		file::remove(unixsocket);
		delete[] unixsocket;
	}
	dbgfile.debugPrint("connection",0,"done deleting unixsocket");

	dbgfile.debugPrint("connection",0,"deleting bindpool...");
	delete bindpool;
	dbgfile.debugPrint("connection",0,"done deleting bindpool");

	dbgfile.debugPrint("connection",0,"deleting bindmappings...");
	clearBindMappings();
	delete inbindmappings;
	delete outbindmappings;
	dbgfile.debugPrint("connection",0,"done deleting bindmappings");

	delete sqlp;
	delete sqlt;
	delete sqlw;

	if (pidfile) {
		file::remove(pidfile);
		delete[] pidfile;
	}
}
