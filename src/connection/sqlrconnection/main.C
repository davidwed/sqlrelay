// Copyright (c) 1999-2010  David Muse
// See the file COPYING for more information

#include <config.h>
#include <sqlrconnection.h>
#include <rudiments/process.h>

sqlrconnection_svr	*sqlrconnection_svr::conn=NULL;
signalhandler		*sqlrconnection_svr::sigh=NULL;
volatile sig_atomic_t	sqlrconnection_svr::shutdowninprogress=0;

void sqlrconnection_svr::cleanUp() {
	conn->closeConnection();
	delete conn;
	delete sigh;
}

void sqlrconnection_svr::shutDown(int signum) {

	if (!signalhandler::isSignalHandlerIntUsed()) {
		cleanUp();
		process::exit(0);
	}

	/* Since this handler is established for more than one kind of signal,
	   it might still get invoked recursively by delivery of some other kind
	   of signal.  Use a static variable to keep track of that. */
	if (shutdowninprogress) {
		//raise(signum);
		return;
	}
	shutdowninprogress=1;

	int	exitcode=1;
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
			fprintf(stderr,"(pid=%d) Process terminated with signal %d\n",process::getProcessId(),signum);
			exitcode=0;
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
			signal(signum,SIG_DFL);
			raise(signum);
	}

	cleanUp();
	process::exit(exitcode);
}

int sqlrconnection_svr::main(int argc, const char **argv,
					sqlrconnection_svr *c) {

	#include <version.h>

	conn=c;

	// handle signals
	sigh=conn->handleSignals(sqlrconnection_svr::shutDown);

	// open the connection to the db
	bool	result=false;
	if ((result=conn->initConnection(argc,argv))) {
		// wait for client connections
		result=conn->listen();
	}

	// If sqlr-stop has been run, we may be here because the sqlr-listener
	// has been killed.  In that case, we'll get a SIGINT soon, but we
	// want to ignore it and just let the shutdown proceed normally,
	// otherwise we could be halfway through cleanUp() below when we
	// get it, which will ultimately run cleanUp() again and result in
	// double-free's and a crash.  If we happen to receive the SIGINT
	// before this point, then the shutdown will proceed that way.
	shutdowninprogress=1;

	// unsuccessful completion
	cleanUp();

	// return successful or unsuccessful completion based on listenresult
	process::exit((result)?0:1);
}
