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
			clientsock->write(autoCommitOn());
		} else {
			dbgfile.debugPrint("connection",2,"autocommit off");
			clientsock->write(autoCommitOff());
		}
	}
	flushWriteBuffer();
}

bool sqlrconnection_svr::autoCommitOn() {
	checkautocommit=true;
	performautocommit=true;
	return true;
}

bool sqlrconnection_svr::autoCommitOff() {
	checkautocommit=true;
	performautocommit=false;
	return true;
}
