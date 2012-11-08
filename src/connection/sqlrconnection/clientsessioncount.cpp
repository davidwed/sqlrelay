// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void sqlrconnection_svr::incrementClientSessionCount() {

	dbgfile.debugPrint("connection",0,"incrementing client session count...");

	if (inclientsession) {
		dbgfile.debugPrint("connection",0,"error. already in client session");
		return;
	}

	semset->waitWithUndo(9);
	statistics->open_cli_connections++;
	inclientsession=true;
	statistics->opened_cli_connections++;
	semset->signalWithUndo(9);

	dbgfile.debugPrint("connection",0,"done incrementing client session count...");
}

void sqlrconnection_svr::decrementClientSessionCount() {

	dbgfile.debugPrint("connection",0,"decrementing client session count...");

	if (!inclientsession) {
		dbgfile.debugPrint("connection",0,"error. not in client session");
		return;
	}

	semset->waitWithUndo(9);
	statistics->open_cli_connections--;
	inclientsession=false;
	if (statistics->open_cli_connections<0) {
		statistics->open_cli_connections=0;
	}
	semset->signalWithUndo(9);

	dbgfile.debugPrint("connection",0,"done decrementing client session count...");
}
