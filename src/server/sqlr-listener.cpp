// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/commandline.h>
#include <rudiments/process.h>
#include <rudiments/stringbuffer.h>
#include <rudiments/file.h>
#include <rudiments/permissions.h>
#include <rudiments/sys.h>
#include <config.h>
#include <version.h>

static sqlrlistener	*lsnr;
static const char	*backtrace=NULL;

#define SHUTDOWNFLAG 1

#ifndef SHUTDOWNFLAG
volatile sig_atomic_t	shutdowninprogress=0;
static void shutDown(int32_t signum) {

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
	if (shutdowninprogress) {
		/*stderror.printf("%s-listener: (pid=%d) "
				"Shutdown loop detected, exiting.\n",
				SQLR,(uint32_t)process::getProcessId());*/
		process::exit(0);
	}

	shutdowninprogress=1;

	if (!charstring::isNullOrEmpty(backtrace) && signum!=SIGTERM) {
		stringbuffer	filename;
		filename.append(backtrace);
		filename.append(sys::getDirectorySeparator());
		filename.append("sqlr-listener.");
		filename.append((uint32_t)process::getProcessId());
		filename.append(".bt");
		file	f;
		if (f.create(filename.getString(),
				permissions::evalPermString("rw-------"))) {
			f.printf("signal: %d\n\n",signum);
			process::backtrace(&f);
		}
	}

	delete lsnr;
	process::exit(0);
}
#endif

static void helpmessage(const char *progname) {
	stdoutput.printf(
		"%s is the %s listener daemon.\n"
		"\n"
		"The %s listens for connections from %s client applications.  When a client connects, the %s hands it off to an available connection daemon or queues it until a connection daemon is available.\n"
		"\n"
		"The %s is not intended to be run manually.  Rather the %s-start process spawns it as-necessary.\n"
		"\n"
		"Usage: %s [OPTIONS]\n"
		"\n"
		"Options:\n"
		SERVEROPTIONS
		"	-nodetach	Suppresses detachment from the controlling terminal.\n"
		"			Useful for debugging.\n"
		"\n"
		DISABLECRASHHANDLER
		BACKTRACE,
		progname,SQL_RELAY,progname,SQL_RELAY,
		progname,progname,SQLR,progname);
}

int main(int argc, const char **argv) {

	version(argc,argv);
	help(argc,argv);

	commandline	cmdl(argc,argv);

	if (!cmdl.found("-id")) {
		stdoutput.printf("usage:\n"
			" %s-listener [-config config] -id id "
			"[-localstatedir dir] [-nodetach]\n",
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

	// create the listener
	lsnr=new sqlrlistener();

#ifdef SHUTDOWNFLAG
	// handle kill and crash signals
	process::setShutDownFlagOnShutDown();
	if (!cmdl.found("-disable-crash-handler")) {
		process::setShutDownFlagOnCrash();
	}
#else
	// handle kill and crash signals
	process::handleShutDown(shutDown);
	if (!cmdl.found("-disable-crash-handler")) {
		process::handleCrash(shutDown);
	}
#endif

	// handle child processes
	process::waitForChildren();

	// ignore various signals
	signalset	set;
	set.addAllSignals();
	set.removeShutDownSignals();
	set.removeCrashSignals();
	// timeouts
	#ifdef SIGALRM
	set.removeSignal(SIGALRM);
	#endif
	// child
	#ifdef SIGCHLD
	set.removeSignal(SIGCHLD);
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

	// initialize
	if (lsnr->init(argc,argv)) {

		// wait for client connections
		lsnr->listen();
	}

#ifdef SHUTDOWNFLAG
	// generate a backtrace if necessary
	if (process::getShutDownFlag() &&
			!charstring::isNullOrEmpty(backtrace) &&
			process::getShutDownSignal()!=SIGTERM) {

		stringbuffer	filename;
		filename.append(backtrace);
		filename.append(sys::getDirectorySeparator());
		filename.append("sqlr-listener.");
		filename.append((uint32_t)process::getProcessId());
		filename.append(".bt");
		file	f;
		if (f.create(filename.getString(),
				permissions::evalPermString("rw-------"))) {
			f.printf("signal: %d\n\n",process::getShutDownSignal());
			process::backtrace(&f);
		}
	}
#endif

	// clean up and exit
	delete lsnr;
	process::exit(1);
}
