// Copyright (c) 1999-2014  David Muse
// See the file COPYING for more information

#include <config.h>

#include <sqlrelay/sqlrserver.h>

#include <rudiments/commandline.h>
#include <rudiments/process.h>

sqlrlistener	*lsnr;
bool		shutdownalready=false;

void shutDown(int32_t signum) {

	// A shutdown loop can occur sometimes, on some platforms, if:
	// * the listener is waiting on a semaphore
	// * the signal runs this method before interrupting the wait
	// * the semaphore is removed
	// * a segfault occurs
	// * this method is run again
	// * this method deletes lsnr again
	// * a segfault occurs
	// * ...
	// So, we'll catch shutdown loops and exit cleanly.  I'm not sure
	// what else can be done.
	if (shutdownalready) {
		stderror.printf("%s-listener: (pid=%d) "
				"Shutdown loop detected, exiting.\n",
				SQLR,(uint32_t)process::getProcessId());
		process::exit(0);
	}

	shutdownalready=true;

	delete lsnr;
	process::exit(0);
}

int main(int argc, const char **argv) {

	commandline	cmdl(argc,argv);

	// set up default signal handling
	process::exitOnShutDown();
	if (!cmdl.found("-disable-crash-handler")) {
		process::exitOnCrash();
	}

	#include <version.h>

	// create the listener
	lsnr=new sqlrlistener();

	// handle kill and crash signals
	process::handleShutDown(shutDown);
	if (!cmdl.found("-disable-crash-handler")) {
		process::handleCrash(shutDown);
	}

	// ignore all other signals except SIGALRM and SIGCHLD
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
