// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrcontroller.h>

void sqlrcontroller_svr::setAutoCommitBehavior(bool ac) {
	conn->autocommit=ac;
}

void sqlrcontroller_svr::autoCommitCommand() {
	dbgfile.debugPrint("connection",1,"autocommit...");
	bool	on;
	if (clientsock->read(&on,idleclienttimeout,0)==sizeof(bool)) {
		if (on) {
			dbgfile.debugPrint("connection",2,"autocommit on");
			clientsock->write(autoCommitOn());
		} else {
			dbgfile.debugPrint("connection",2,"autocommit off");
			clientsock->write(autoCommitOff());
		}
	}
	flushWriteBuffer();
}

void sqlrcontroller_svr::setAutoCommit(bool ac) {
	dbgfile.debugPrint("connection",0,"setting autocommit...");
	if (ac) {
		if (!autoCommitOn()) {
			dbgfile.debugPrint("connection",0,
					"setting autocommit on failed");
			fprintf(stderr,"Couldn't set autocommit on.\n");
			return;
		}
	} else {
		if (!autoCommitOff()) {
			dbgfile.debugPrint("connection",0,
					"setting autocommit off failed");
			fprintf(stderr,"Couldn't set autocommit off.\n");
			return;
		}
	}
	dbgfile.debugPrint("connection",0,"done setting autocommit");
}

bool sqlrcontroller_svr::autoCommitOn() {
	autocommitforthissession=true;
	return conn->autoCommitOn();
}

bool sqlrcontroller_svr::autoCommitOff() {
	autocommitforthissession=false;
	return conn->autoCommitOff();
}
