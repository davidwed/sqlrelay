// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sybaseconnection.h>
#include <rudiments/signalclasses.h>

#include <stdlib.h>
#include <stdio.h>

#include <sys/types.h>
#include <unistd.h>

#ifdef NEED_GLIBC_2_3_HACK
extern "C" {
	#include <ctype.h>
	const unsigned short int *__ctype_b;
	int	__ctype_toupper(int c) {
		return toupper(c);
	}
	int	__ctype_tolower(int c) {
		return tolower(c);
	}
}
#endif


sybaseconnection	*conn;
signalhandler		*alarmhandler;

void	cleanUp() {
	conn->closeConnection();
	delete conn;
	delete alarmhandler;
}

void	shutDown() {
	cleanUp();
	exit(0);
}

int	main(int argc, const char **argv) {

	#include <version.h>

	conn=new sybaseconnection();

	// handle kill signals
	conn->handleShutDown((void *)shutDown);

	// handle alarm
	alarmhandler=new signalhandler(SIGALRM,(void *)shutDown);

	// open the connection
	if (conn->initConnection(argc,argv,0)) {
		// wait for connections
		conn->listen();
	}

	// unsuccessful completion
	cleanUp();
	exit(1);
}
