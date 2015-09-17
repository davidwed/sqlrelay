// Copyright (c) 1999-2014  David Muse
// See the file COPYING for more information

#include <config.h>

#include <sqlrelay/sqlrserver.h>

#include <rudiments/process.h>

sqlrlistener	*lsnr;
bool		shutdownalready=false;

void shutDown(int32_t signum) {

	if (shutdownalready) {
		process::exit(0);
	}
	shutdownalready=true;

	delete lsnr;
	process::exit(0);
}

int main(int argc, const char **argv) {

	// set up default signal handling
	process::exitOnCrashOrShutDown();

	#include <version.h>

	// create the listener
	lsnr=new sqlrlistener();

	// handle kill and crash signals
	process::handleShutDown(shutDown);
	process::handleCrash(shutDown);

	// block all signals except SIGALRM and SIGCHLD
	signalset	set;
	set.addAllSignals();
	set.removeShutDownSignals();
	set.removeCrashSignals();
	// alarm and chld
	#ifdef SIGALRM
	set.removeSignal(SIGALRM);
	#endif
	#ifdef SIGCHLD
	set.removeSignal(SIGCHLD);
	#endif
	signalmanager::ignoreSignals(&set);

	// initialize
	if (lsnr->init(argc,argv)) {

		// wait for client connections
		lsnr->listen();
	}

	// clean up and exit
	delete lsnr;
	process::exit(1);
}
