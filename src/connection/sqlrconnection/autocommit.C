// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void sqlrconnection::setAutoCommitBehavior(bool ac) {
	autocommit=ac;
}

bool sqlrconnection::getAutoCommitBehavior() {
	return autocommit;
}

void sqlrconnection::autoCommitCommand() {
	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"autocommit...");
	#endif
	bool	on;
	clientsock->read(&on);
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
	flushWriteBuffer();
}

bool sqlrconnection::autoCommitOn() {
	checkautocommit=true;
	performautocommit=true;
	return true;
}

bool sqlrconnection::autoCommitOff() {
	checkautocommit=true;
	performautocommit=false;
	return true;
}
