// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <config.h>
#include <oracle8connection.h>
#include <rudiments/signalclasses.h>
#include <rudiments/process.h>

//#include <stdlib.h>
//#include <stdio.h>

// for _exit
#ifdef HAVE_UNISTD_H
	#include <unistd.h>
#endif

oracle8connection	*conn;
signalhandler		*alarmhandler;

void cleanUp() {
	conn->closeConnection();
	delete conn;
	delete alarmhandler;
}

volatile sig_atomic_t shutdowninprogress=0;

void shutDown(int signum) {

	if (!signalhandler::isSignalHandlerIntUsed()) {
		cleanUp();
		_exit(0);
	}

	/* Since this handler is established for more than one kind of signal,
	   it might still get invoked recursively by delivery of some other kind
	   of signal.  Use a static variable to keep track of that. */
	if (shutdowninprogress) {
		raise(signum);
	}
	shutdowninprogress=1;

	int	ret=1;

	switch (signum) {
		case SIGINT:
#ifdef HAVE_SIGQUIT
		case SIGQUIT:
#endif
			// These signals indicate normal termination
			fprintf(stderr,"(pid=%d) Process terminated with signal %d\n",process::getProcessId(),signum);
			break;

		case SIGTERM:
			// Shutdown
		case SIGALRM:
			// Timeout
			ret=0;
			break;

		default:
			// Other signals are bugs
			fprintf(stderr,"(pid=%d) Abnormal termination: signal %d received\n",process::getProcessId(),signum);
			cleanUp();
			/* Now reraise the signal.  We reactivate the signal's
		   	default handling, which is to terminate the process.
		   	We could just call exit or abort,
		   	but reraising the signal sets the return status
		   	from the process correctly. */
			signal(signum, SIG_DFL);
			raise(signum);
	}

	cleanUp();
	_exit(ret);
}

int main(int argc, const char **argv) {

	#include <version.h>

	conn=new oracle8connection();

	// handle signals
	alarmhandler=conn->handleSignals(shutDown);

	// open the connection
	bool	listenresult=false;
	if (conn->initConnection(argc,argv)) {
		// wait for connections
		listenresult=conn->listen();
	}

	// unsuccessful completion
	cleanUp();

	// return successful or unsuccessful completion based on listenresult
	exit((listenresult)?0:1);
}
