// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <postgresqlconnection.h>
#include <rudiments/signalclasses.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

postgresqlconnection	*conn;
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

	conn=new postgresqlconnection();

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
