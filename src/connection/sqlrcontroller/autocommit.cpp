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
			clientsock->write(autoCommitOnInternal());
		} else {
			dbgfile.debugPrint("connection",2,"autocommit off");
			clientsock->write(autoCommitOffInternal());
		}
	}
	flushWriteBuffer();
}

void sqlrcontroller_svr::setAutoCommit(bool ac) {
	dbgfile.debugPrint("connection",0,"setting autocommit...");
	if (ac) {
		if (!autoCommitOnInternal()) {
			dbgfile.debugPrint("connection",0,
					"setting autocommit on failed");
			fprintf(stderr,"Couldn't set autocommit on.\n");
			return;
		}
	} else {
		if (!autoCommitOffInternal()) {
			dbgfile.debugPrint("connection",0,
					"setting autocommit off failed");
			fprintf(stderr,"Couldn't set autocommit off.\n");
			return;
		}
	}
	dbgfile.debugPrint("connection",0,"done setting autocommit");
}

bool sqlrcontroller_svr::autoCommitOnInternal() {
	autocommitforthissession=true;
	return conn->autoCommitOn();
}

bool sqlrcontroller_svr::autoCommitOffInternal() {
	autocommitforthissession=false;
	return conn->autoCommitOff();
}
