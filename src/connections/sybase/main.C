// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sybaseconnection.h>
#include <rudiments/signalclasses.h>

#include <stdlib.h>
#include <stdio.h>

#include <sys/types.h>
#ifdef HAVE_UNISTD_H
	#include <unistd.h>
#endif

#ifdef NEED_REDHAT_9_GLIBC_2_3_2_HACK
extern "C" {
	#include <ctype.h>
	const unsigned short int **__ctype_b() {
        	return __ctype_b_loc();
	}

	const __int32_t **__ctype_toupper() {
        	return __ctype_toupper_loc();
	}

	const __int32_t **__ctype_tolower() {
        	return __ctype_tolower_loc();
	}
}
#endif


sybaseconnection	*conn;
signalhandler		*alarmhandler;

void cleanUp() {
	conn->closeConnection();
	delete conn;
	delete alarmhandler;
}

void shutDown() {
	cleanUp();
	_exit(0);
}

int main(int argc, const char **argv) {

	#include <version.h>

	conn=new sybaseconnection();

	// handle kill signals
	conn->handleShutDown((void *)shutDown);

	// handle alarm
	alarmhandler=new signalhandler(SIGALRM,(void *)shutDown);

	// open the connection
	if (conn->initConnection(argc,argv)) {
		// wait for connections
		conn->listen();
	}

	// unsuccessful completion
	cleanUp();
	exit(1);
}
