// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrlistener.h>

#include <rudiments/process.h>

sqlrlistener	*lsnr;

void shutDown(int32_t signum) {
	delete lsnr;
	process::exit(0);
}

void handleSignals(void (*shutdownfunction)(int32_t)) {

	// handle kill and crash signals
	process::handleShutDown(shutdownfunction);
	process::handleCrash(shutdownfunction);

	// block all other signals except SIGALRM and SIGCHLD
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
}

int main(int argc, const char **argv) {

	// set up default signal handling
	process::exitOnCrashOrShutDown();

	#include <version.h>

	// create the listener
	lsnr=new sqlrlistener();

	// hande signals
	handleSignals(shutDown);

	// initialize
	if (lsnr->initListener(argc,argv)) {

		// wait for client connections
		lsnr->listen();
	}

	// clean up and exit
	delete lsnr;
	process::exit(1);
}
