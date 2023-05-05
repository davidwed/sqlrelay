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
const char		*backtrace=NULL;

#define SHUTDOWNFLAG 1

#ifndef SHUTDOWNFLAG
volatile sig_atomic_t	shutdowninprogress=0;
static void shutDown(int32_t signum) {

	// Since this handler is established for more than one kind of signal,
	// it might get invoked recursively by delivery of multiple signals.
	// It might also get called if a signal is received during the final
	// exit after lots of stuff has been freed.  So, keep track of whether
	// the shutdown is in progress or not, and bail if we get called when
	// it is.
	if (shutdowninprogress) {
		/*stderror.printf("%s-connection (pid=%d) "
				"Shutdown loop detected, exiting.\n",
				SQLR,(uint32_t)process::getProcessId());*/
		process::exit(0);
	}
	shutdowninprogress=1;

	if (!charstring::isNullOrEmpty(backtrace) && signum!=SIGTERM) {
		stringbuffer    filename;
		filename.append(backtrace);
		filename.append(sys::getDirectorySeparator());
		filename.append("sqlr-connection.");
		filename.append((uint32_t)process::getProcessId());
		filename.append(".bt");
		file	f;
		if (f.create(filename.getString(),
				permissions::parsePermString("rw-------"))) {
			f.printf("signal: %d\n\n",signum);
			process::writeBacktrace(&f);
		}
	}

	if (!signalhandler::supportsSignalHandlerParameter()) {
		delete cont;
		process::exit(0);
	}

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
#endif

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

	if (!cmdl.getWasFound("-id") || !cmdl.getWasFound("-connectionid")) {
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
	if (!cmdl.getWasFound("-disable-crash-handler")) {
		process::exitOnCrash();
	}

	// create the controller
	cont=new sqlrservercontroller;

#ifdef SHUTDOWNFLAG
	// handle kill and crash signals
	process::setShutDownFlagOnShutDown();
	if (!cmdl.getWasFound("-disable-crash-handler")) {
		process::setShutDownFlagOnCrash();
	}
#else
	// handle kill and crash signals
	process::setShutDownHandler(shutDown);
	if (!cmdl.getWasFound("-disable-crash-handler")) {
		process::setCrashHandler(shutDown);
	}
#endif

	// ignore various signals
	signalset	set;
	set.addAllSignals();
	set.removeShutDownSignals();
	set.removeCrashSignals();
	// don't ignore alarms (we use these to implement semaphore timeouts
	// on platforms that don't support timed semaphore operations)
	#ifdef SIGALRM
	set.removeSignal(SIGALRM);
	#endif
	signalmanager::ignoreSignals(&set);

	// initialize and wait for client connections
	int32_t exitstatus=(cont->init(argc,argv) && cont->listen())?0:1;

#ifdef SHUTDOWNFLAG
	if (process::getShutDownFlag()) {

		int32_t	signum=process::getShutDownSignal();

		// generate a backtrace if necessary
		if (!charstring::isNullOrEmpty(backtrace) && signum!=SIGTERM) {

			stringbuffer    filename;
			filename.append(backtrace);
			filename.append(sys::getDirectorySeparator());
			filename.append("sqlr-connection.");
			filename.append((uint32_t)process::getProcessId());
			filename.append(".bt");
			file	f;
			if (f.create(filename.getString(),
				permissions::parsePermString("rw-------"))) {
				f.printf("signal: %d\n\n",signum);
				process::writeBacktrace(&f);
			}
		}

		// print exit message
		stderror.printf("%s-connection (pid=%d) ",
				SQLR,(uint32_t)process::getProcessId());
		stderror.printf(
				(signum==SIGINT ||
					signum==SIGTERM
					#ifdef SIGQUIT
					|| signum==SIGQUIT
					#endif
					)?
				"Process terminated with signal %d\n":
				"Abnormal termination: signal %d received\n",
			signum);

		// set successful exit on SIGTERM, otherwise set
		// the exit status to 128 + the signal number
		exitstatus=(signum==SIGTERM)?0:128+signum;
	}
#else
	// If sqlr-stop has been run, we may be here because the sqlr-listener
	// has been killed.  In that case, we'll get a SIGTERM soon, but we
	// want to ignore it and just let the shutdown proceed normally,
	// otherwise we could be halfway through cleanUp() below when we
	// get it, which will ultimately run cleanUp() again and result in
	// double-free's and a crash.  If we happen to receive the SIGTERM
	// before this point, then the shutdown will proceed that way.
	shutdowninprogress=1;
#endif

	// clean up and exit
	delete cont;
	process::exit(exitstatus);
}
