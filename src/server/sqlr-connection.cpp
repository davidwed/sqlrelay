// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/commandline.h>
#include <rudiments/process.h>
#include <rudiments/stringbuffer.h>
#include <rudiments/file.h>
#include <rudiments/permissions.h>
#include <rudiments/sys.h>
#include <rudiments/signalclasses.h>
#include <rudiments/stdio.h>
#include <config.h>
#include <version.h>

sqlrservercontroller	*cont=NULL;
volatile sig_atomic_t	shutdowninprogress=0;
signalhandler		shutdownhandler;
bool			shutdownalready=false;
const char		*backtrace=NULL;

static void shutDown(int32_t signum) {

	if (shutdownalready) {
		stderror.printf("%s-connection (pid=%d) "
				"Shutdown loop detected, exiting.\n",
				SQLR,(uint32_t)process::getProcessId());
		process::exit(0);
	}

	shutdownalready=true;

	if (!charstring::isNullOrEmpty(backtrace) && signum!=SIGINT) {
		stringbuffer    filename;
		filename.append(backtrace);
		filename.append(sys::getDirectorySeparator());
		filename.append("sqlr-connection.");
		filename.append((uint32_t)process::getProcessId());
		filename.append(".bt");
		file	f;
		if (f.create(filename.getString(),
				permissions::evalPermString("rw-------"))) {
			f.printf("signal: %d\n\n",signum);
			process::backtrace(&f);
		}
	}

	if (!signalhandler::isSignalHandlerIntUsed()) {
		delete cont;
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
		#ifdef SIGQUIT
		case SIGQUIT:
		#endif
			// These signals indicate normal termination
			stderror.printf("%s-connection (pid=%d) "
					"Process terminated with signal %d\n",
					SQLR,
					(uint32_t)process::getProcessId(),
					signum);
			break;

		case SIGTERM:
			// Shutdown
		#ifdef SIGALRM
		case SIGALRM:
			// Timeout
			stderror.printf("%s-connection (pid=%d) "
					"Process terminated with signal %d\n",
					SQLR,
					(uint32_t)process::getProcessId(),
					signum);
			exitcode=0;
			break;
		#endif

		default:
			// Other signals are bugs
			stderror.printf("%s-connection (pid=%d) "
					"Abnormal termination: "
					"signal %d received\n",
					SQLR,
					(uint32_t)process::getProcessId(),
					signum);
			delete cont;
			// Now reraise the signal.  We reactivate the signal's
		   	// default handling, which is to terminate the process.
		   	// We could just call exit or abort,
		   	// but reraising the signal sets the return status
		   	// from the process correctly.
			signal(signum,SIG_DFL);
			process::raiseSignal(signum);
	}

	delete cont;
	process::exit(exitcode);
}

static void helpmessage(const char *progname) {
	stdoutput.printf(
		"%s is the %s database connection daemon.\n"
		"\n"
		"Each %s maintains a persistent connection to a database.  Together, a set of %s daemons provide a database connection pool to %s client applications.\n"
		"\n"
		"%s is not intended to be run manually.  Rather the %s-start and %s-scaler processes spawn instances of %s as-necessary.\n"
		"\n"
		"Usage: %s [OPTIONS]\n"
		"\n"
		"Options:\n"
		SERVEROPTIONS
		"	-scaler		Indicates to the %s that it was spawned\n"
		"			by the %s-scaler.\n"
		"\n"
		"	-ttl sec	Time-to-live, in seconds.  If the %s is\n"
		"			idle for this number of seconds, then it will exit.\n"
		"\n"
		"	-silent		Suppresses log-in errors.\n"
		"\n"
		"	-nodetach	Suppresses detachment from the controlling terminal.\n"
		"			Useful for debugging.\n"
		"\n"
		DISABLECRASHHANDLER
		BACKTRACE,
		progname,SQL_RELAY,progname,progname,SQL_RELAY,
		progname,SQLR,SQLR,progname,progname,progname,SQLR,progname);
}

int main(int argc, const char **argv) {

	version(argc,argv);
	help(argc,argv);

	commandline	cmdl(argc,argv);

	if (!cmdl.found("-id") || !cmdl.found("-connectionid")) {
		stdoutput.printf("usage: \n"
			" %s-connection [-config config] "
			"-id id -connectionid connectionid\n"
			"                 [-localstatedir dir] "
			"[-scaler] [-ttl sec] [-silent] [-nodetach]\n",
			SQLR);
		process::exit(0);
	}

	// enable/disable backtrace
	backtrace=cmdl.getValue("-backtrace");

	// set up default signal handling
	process::exitOnShutDown();
	if (!cmdl.found("-disable-crash-handler")) {
		process::exitOnCrash();
	}

	// create the controller
	cont=new sqlrservercontroller;


	// handle kill and crash signals
	process::handleShutDown(shutDown);
	if (!cmdl.found("-disable-crash-handler")) {
		process::handleCrash(shutDown);
	}

	// handle various other shutdown conditions
	shutdownhandler.setHandler(shutDown);
	// timeouts
	#ifdef SIGALRM
	shutdownhandler.handleSignal(SIGALRM);
	#endif
	// CPU consumption soft limit
	#ifdef SIGXCPU
	shutdownhandler.handleSignal(SIGXCPU);
	#endif
	// File size limit exceeded
	#ifdef SIGXFSZ
	shutdownhandler.handleSignal(SIGXFSZ);
	#endif
	// power failure
	#ifdef SIGPWR
	shutdownhandler.handleSignal(SIGPWR);
	#endif

	// ignore others
	signalset	set;
	set.addAllSignals();
	set.removeShutDownSignals();
	set.removeCrashSignals();
	#ifdef SIGALRM
	set.removeSignal(SIGALRM);
	#endif
	// CPU consumption soft limit
	#ifdef SIGXCPU
	set.removeSignal(SIGXCPU);
	#endif
	// File size limit exceeded
	#ifdef SIGXFSZ
	set.removeSignal(SIGXFSZ);
	#endif
	// power failure
	#ifdef SIGPWR
	set.removeSignal(SIGPWR);
	#endif
	signalmanager::ignoreSignals(&set);


	// connect to the db
	bool	result=cont->init(argc,argv);
	if (result) {
		// wait for client connections
		result=cont->listen();
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
	delete cont;

	// return successful or unsuccessful completion based on listenresult
	process::exit((result)?0:1);
}
