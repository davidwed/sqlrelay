// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <mdbtoolsconnection.h>
#include <rudiments/signalclasses.h>

#include <stdlib.h>
#include <stdio.h>

// for _exit
#ifdef HAVE_UNISTD_H
	#include <unistd.h>
#endif

mdbtoolsconnection	*conn;
signalhandler		*alarmhandler;

void cleanUp() {
	conn->closeConnection();
	delete conn;
	delete alarmhandler;
}

void shutDown(int signum) {
	cleanUp();
	_exit(0);
}

int main(int argc, const char **argv) {

	#include <version.h>

	conn=new mdbtoolsconnection();

	// handle signals
	alarmhandler=conn->handleSignals(shutDown);

	// open the connection
	bool	listenresult=false;
	if (conn->initConnection(argc,argv)) {
		// wait for connections
		listenresult=conn->listen();
	}

	cleanUp();

	// return successful or unsuccessful completion based on listenresult
	exit((listenresult)?0:1);
}
