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
	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"autocommit...");
	#endif
	bool	on;
	if (clientsock->read(&on,idleclienttimeout,0)==sizeof(bool)) {
		if (on) {
			#ifdef SERVER_DEBUG
			debugPrint("connection",2,"autocommit on");
			#endif
			clientsock->write(autoCommitOn());
		} else {
			#ifdef SERVER_DEBUG
			debugPrint("connection",2,"autocommit off");
			#endif
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
