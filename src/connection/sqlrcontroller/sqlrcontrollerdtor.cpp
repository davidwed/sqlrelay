// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <config.h>
#include <sqlrcontroller.h>
#include <rudiments/file.h>
#include <rudiments/rawbuffer.h>

sqlrcontroller_svr::~sqlrcontroller_svr() {

	if (connstats) {
		rawbuffer::zero(connstats,sizeof(sqlrconnstatistics));
	}

	delete cmdl;
	delete cfgfl;

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
	delete bindmappingspool;
	delete inbindmappings;
	delete outbindmappings;
	dbgfile.debugPrint("connection",0,"done deleting bindmappings");

	delete sqlp;
	delete sqlt;
	delete sqlw;
	delete sqltr;
	delete sqlrlg;
	delete sqlrq;

	delete[] clientinfo;

	if (pidfile) {
		file::remove(pidfile);
		delete[] pidfile;
	}

	delete conn;
}
