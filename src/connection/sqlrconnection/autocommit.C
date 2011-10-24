// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void sqlrconnection_svr::setAutoCommitBehavior(bool ac) {
	autocommit=ac;
}

bool sqlrconnection_svr::getAutoCommitBehavior() {
	return autocommit;
}

void sqlrconnection_svr::autoCommitCommand() {
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

bool sqlrconnection_svr::autoCommitOnInternal() {
	autocommit=true;
	return autoCommitOn();
}

bool sqlrconnection_svr::autoCommitOffInternal() {
	autocommit=false;
	return autoCommitOff();
}

bool sqlrconnection_svr::autoCommitOn() {
	fakeautocommit=true;
	return true;
}

bool sqlrconnection_svr::autoCommitOff() {
	fakeautocommit=true;
	return true;
}
