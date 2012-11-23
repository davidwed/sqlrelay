// Copyright (c) 1999-2010  David Muse
// See the file COPYING for more information

#include <config.h>
#include <sqlrcontroller.h>
#include <rudiments/process.h>

sqlrcontroller_svr	*sqlrcontroller_svr::staticcont=NULL;
signalhandler		*sqlrcontroller_svr::sigh=NULL;
volatile sig_atomic_t	sqlrcontroller_svr::shutdowninprogress=0;

void sqlrcontroller_svr::cleanUp() {
	staticcont->closeConnection();
	delete staticcont;
	delete sigh;
}

void sqlrcontroller_svr::shutDown(int32_t signum) {

	if (!signalhandler::isSignalHandlerIntUsed()) {
		cleanUp();
		process::exit(0);
	}

	// Since this handler is established for more than one kind of signal,
	// it might still get invoked recursively by delivery of some other kind
	// of signal.  Use a static variable to keep track of that.
	if (shutdowninprogress) {
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

void sqlrcontroller_svr::main(int argc, const char **argv,
					sqlrconnection_svr *conn) {

	#include <version.h>

	staticcont=conn->cont;

	// handle signals
	sigh=staticcont->handleSignals(sqlrcontroller_svr::shutDown);

	// open the connection to the db
	bool	result=false;
	if ((result=staticcont->init(argc,argv,conn))) {
		// wait for client connections
		result=staticcont->listen();
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
