// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <oracle8connection.h>
#include <rudiments/signalclasses.h>

#include <stdlib.h>
#include <stdio.h>

oracle8connection	*conn;
signalhandler		*alarmhandler;

void cleanUp() {
	conn->closeConnection();
	delete conn;
	delete alarmhandler;
}

void shutDown() {
	cleanUp();
	exit(0);
}

int main(int argc, const char **argv) {

	#include <version.h>

	conn=new oracle8connection();

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
