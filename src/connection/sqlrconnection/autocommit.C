// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void	sqlrconnection::setAutoCommitBehavior(int ac) {
	autocommit=ac;
}

int	sqlrconnection::getAutoCommitBehavior() {
	return autocommit;
}

void	sqlrconnection::autoCommitCommand() {
	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"autocommit...");
	#endif
	unsigned short	on;
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
}

unsigned short	sqlrconnection::autoCommitOn() {
	checkautocommit=1;
	performautocommit=1;
	return 1;
}

unsigned short	sqlrconnection::autoCommitOff() {
	checkautocommit=1;
	performautocommit=0;
	return 1;
}
