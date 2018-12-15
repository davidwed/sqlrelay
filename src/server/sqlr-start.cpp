// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrutil.h>
#include <sqlrelay/private/sqlrshm.h>
#include <rudiments/process.h>
#ifndef _WIN32
	#include <rudiments/inetsocketclient.h>
	#include <rudiments/unixsocketclient.h>
	#include <rudiments/snooze.h>
#endif
#include <rudiments/stdio.h>
#include <config.h>
#include <defaults.h>
#include <version.h>

#ifdef _WIN32
	#include <windows.h>
	#include <stdio.h>
	#include <io.h>
#endif

// On some older platforms (at least redhat 1.1) endian.h and asm/byteorder.h
// battle it out.  This makes endian.h (which is included by math.h) win.
#undef LITTLE_ENDIAN
#undef BIG_ENDIAN

// for ceil()
#include <math.h>

bool	iswindows;

static bool startListener(sqlrpaths *sqlrpth,
				const char *id,
				const char *config,
				const char *localstatedir,
				const char *backtrace,
				bool disablecrashhandler) {

	// start the listener
	stdoutput.printf("\nStarting listener:\n");

	// build command name
	stringbuffer	cmdname;
	cmdname.append(SQLR)->append("-listener");

	// build command to spawn
	stringbuffer	cmd;
	cmd.append(sqlrpth->getBinDir())->append(cmdname.getString());
	if (iswindows) {
		cmd.append(".exe");
 	}

	// build args
	uint16_t	i=0;
	const char	*args[11];
	args[i++]=cmdname.getString();
	args[i++]="-id";
	args[i++]=id;
	if (!charstring::isNullOrEmpty(config)) {
		args[i++]="-config";
		args[i++]=config;
	}
	if (!charstring::isNullOrEmpty(localstatedir)) {
		args[i++]="-localstatedir";
		args[i++]=localstatedir;
	}
	if (!charstring::isNullOrEmpty(backtrace)) {
		args[i++]="-backtrace";
		args[i++]=backtrace;
	}
	if (disablecrashhandler) {
		args[i++]="-disable-crash-handler";
	}
	args[i]=NULL;
	
	// display command
	stdoutput.printf("  ");
	for (uint16_t index=0; index<i; index++) {
		stdoutput.printf("%s ",args[index]);
	}
	stdoutput.printf("\n");

	// spawn the command
	if (process::spawn(cmd.getString(),args,(iswindows)?true:false)==-1) {
		stdoutput.printf("\n%s failed to start.\n",cmdname.getString());
		return false;
	}
	return true;
}


static bool startConnection(sqlrpaths *sqlrpth,
				const char *id,
				const char *connectionid,
				const char *config,
				const char *localstatedir,
				bool strace,
				const char *backtrace,
				bool disablecrashhandler) {

	// build command name
	stringbuffer	cmdname;
	cmdname.append(SQLR)->append("-connection");

	// build command to spawn
	stringbuffer	cmd;
	if (strace) {
		cmd.append("strace");
	} else {
		cmd.append(sqlrpth->getBinDir())->append(cmdname.getString());
		if (iswindows) {
			cmd.append(".exe");
		}
	}

	// build args
	uint16_t	i=0;
	const char	*args[17];
	if (strace) {
		args[i++]="strace";
		args[i++]="-ff";
		args[i++]="-o";
	}
	args[i++]=cmdname.getString();
	args[i++]="-id";
	args[i++]=id;
	if (connectionid) {
		args[i++]="-connectionid";
		args[i++]=connectionid;
	}
	if (!charstring::isNullOrEmpty(config)) {
		args[i++]="-config";
		args[i++]=config;
	}
	if (!charstring::isNullOrEmpty(localstatedir)) {
		args[i++]="-localstatedir";
		args[i++]=localstatedir;
	}
	if (!charstring::isNullOrEmpty(backtrace)) {
		args[i++]="-backtrace";
		args[i++]=backtrace;
	}
	if (disablecrashhandler) {
		args[i++]="-disable-crash-handler";
	}
	if (strace) {
		args[i++]="&";
	}
	args[i]=NULL;

	// display command
	stdoutput.printf("  ");
	for (uint16_t index=0; index<i; index++) {
		stdoutput.printf("%s ",args[index]);
	}
	stdoutput.printf("\n");

	// spawn the command
	if (process::spawn(cmd.getString(),args,(iswindows)?true:false)==-1) {
		stdoutput.printf("\n%s failed to start.\n",cmdname.getString());
		return false;
	}
	return true;
}

static bool startConnections(sqlrpaths *sqlrpth,
				sqlrconfig *cfg,
				const char *id,
				const char *config,
				const char *localstatedir,
				bool strace,
				const char *backtrace,
				bool disablecrashhandler) {

	// get the connection count and total metric
	linkedlist< connectstringcontainer *>	*connectionlist=
						cfg->getConnectStringList();

	// if the metrictotal was 0, start no connections
	if (!cfg->getMetricTotal()) {
		return true;
	}

	// if no connections were defined in the configuration,
	// start 1 default one
	if (!cfg->getConnectionCount()) {
		return !startConnection(sqlrpth,id,config,localstatedir,NULL,
					strace,backtrace,disablecrashhandler);
	}

	// get number of connections
	int32_t	connections=cfg->getConnections();

	// start the connections
	connectstringnode	*csn=connectionlist->getFirst();
	connectstringcontainer	*csc;
	int32_t	metric=0;
	int32_t	startup=0;
	int32_t	totalstarted=0;
	bool	done=false;
	while (!done) {

		// get the appropriate connection
		csc=csn->getValue();
	
		// scale the number of each connection to start by 
		// each connection's metric vs the total number of 
		// connections to start up
		metric=csc->getMetric();
		if (metric>0) {
			startup=(int32_t)ceil(
				((double)(metric*connections))/
				((double)cfg->getMetricTotal()));
		} else {
			startup=0;
		}

		// keep from starting too many connections
		if (totalstarted+startup>connections) {
			startup=connections-totalstarted;
			done=true;
		}

		stdoutput.printf("\nStarting %d connections to ",startup);
		stdoutput.printf("%s :\n",csc->getConnectionId());

		// fire them up
		for (int32_t i=0; i<startup; i++) {
			if (!startConnection(sqlrpth,id,
					csc->getConnectionId(),
					config,localstatedir,strace,
					backtrace,disablecrashhandler)) {
				// it's ok if at least 1 connection started up
				return (totalstarted>0 || i>0);
			}
		}

		// have we started enough connections?
		totalstarted=totalstarted+startup;
		if (totalstarted==connections) {
			done=true;
		}

		// next...
		csn=csn->getNext();
	}
	return true;
}

static bool startScaler(sqlrpaths *sqlrpth,
			sqlrconfig *cfg,
			const char *id,
			const char *config,
			const char *localstatedir,
			const char *backtrace,
			bool disablecrashhandler) {

	// don't start the scalar if unless dynamic scaling is enabled
	if (!cfg->getDynamicScaling()) {
		return true;
	}

	stdoutput.printf("\nStarting scaler:\n");

	// build command name
	stringbuffer	cmdname;
	cmdname.append(SQLR)->append("-scaler");

	// build command to spawn
	stringbuffer	cmd;
	cmd.append(sqlrpth->getBinDir())->append(cmdname.getString());
	if (iswindows) {
		cmd.append(".exe");
 	}

	// build args
	uint16_t	i=0;
	const char	*args[11];
	args[i++]=cmdname.getString();
	args[i++]="-id";
	args[i++]=id;
	if (!charstring::isNullOrEmpty(config)) {
		args[i++]="-config";
		args[i++]=config;
	}
	if (!charstring::isNullOrEmpty(localstatedir)) {
		args[i++]="-localstatedir";
		args[i++]=localstatedir;
	}
	if (!charstring::isNullOrEmpty(backtrace)) {
		args[i++]="-backtrace";
		args[i++]=backtrace;
	}
	if (disablecrashhandler) {
		args[i++]="-disable-crash-handler";
	}
	args[i]=NULL;

	// display command
	stdoutput.printf("  ");
	for (uint16_t index=0; index<i; index++) {
		stdoutput.printf("%s ",args[index]);
	}
	stdoutput.printf("\n");

	// spawn the command
	if (process::spawn(cmd.getString(),args,(iswindows)?true:false)==-1) {
		stdoutput.printf("\n%s failed to start.\n",cmdname.getString());
		return false;
	}
	return true;
}

#ifndef _WIN32
static bool waitForInstance(sqlrpaths *sqlrpth,
				sqlrconfig *cfg,
				const char *thisid) {

	stdoutput.printf("\nWaiting for instance...\n");

	bool	retval=false;
	for (uint16_t i=0; i<30; i++) {

		// try to connect
		if (cfg->getListenOnUnix()) {
			unixsocketclient	s;
			if (s.connect(cfg->getDefaultSocket(),1,0,0,1)==
							RESULT_SUCCESS) {
				retval=true;
				break;
			}

		} else if (cfg->getListenOnInet()) {
			inetsocketclient	s;
			if (s.connect("localhost",
					cfg->getDefaultPort(),1,0,0,1)==
							RESULT_SUCCESS) {
				retval=true;
				break;
			}
		}

		// FIXME: Ideally, rather than waiting 30 seconds, we'd check
		// to see if the sqlr-connections are still running and bail if
		// they aren't.

		// wait a second and try again
		snooze::macrosnooze(1);
	}

	if (retval) {
		stdoutput.printf("Instance is ready.\n");
	} else {
		stdoutput.printf("Instance failed to start.\n");
	}
	return true;
}
#endif

static void helpmessage(const char *progname) {
	stdoutput.printf(
		"%s is the startup program for the %s server processes.\n"
		"\n"
		"The %s program spawns %s-listener, %s-connection, and %s-scaler processes.\n"
		"\n"
		"When run with the -id argument, %s starts processes for the specified instance.  When run with no -id argument, %s starts processes for all enabled instances.\n"
		"\n"
		"Usage: %s [OPTIONS]\n"
		"\n"
		"Options:\n"
		SERVEROPTIONS
		"	-abs-max-connections	Displays the absolute maximum number of\n"
		"				database connections that may be opened by\n"
		"				instance of SQL Relay and exits.n\n"
		"\n"
		DISABLECRASHHANDLER
		BACKTRACECHILDREN
#ifdef _WIN32
		"	-disable-new-window	Spawns child processes in the current window,\n"
		"				rather than opening a new window.  Helpful\n"
		"				when debugging startup errors.\n"
		"\n"
#endif
#ifndef _WIN32
		"	-wait			Wait up to 30 seconds for each instance to\n"
		"				become ready before exiting and exit with\n"
		"				status 1 if an instance failed to become ready\n"
		"				in the allotted time.\n"
		"\n"
#endif
		"Examples:\n"
		"\n"
		"Start instance \"myinst\" as defined in the default configuration.\n"
		"\n"
		"	%s -id myinst\n"
		"\n"
		"Start instance \"myinst\" as defined in the config file ./myconfig.conf\n"
		"\n"
		"	%s -config ./myconfig.conf -id myinst\n"
		"\n"
		"Start all instances enabled in the default configuration.\n"
		"\n"
		"	%s\n"
		"\n"
		"Start all instances enabled in the config file ./myconfig.conf\n"
		"\n"
		"	%s -config ./myconfig.conf\n"
		"\n",
		progname,SQL_RELAY,progname,
		SQLR,SQLR,SQLR,progname,progname,
		progname,progname,progname,progname,progname);
}

static void absmaxconnections(int argc, const char **argv) {

	if (argc!=2 || (charstring::compare(argv[1],"-abs-max-connections") &&
			charstring::compare(argv[1],"--abs-max-connections"))) {
		return;
	}

	stdoutput.printf("abs max connections: %d\n",MAXCONNECTIONS);
	stdoutput.printf("shmmax requirement:  %d\n",sizeof(sqlrshm));

	process::exit(0);
}

int main(int argc, const char **argv) {

	version(argc,argv);
	help(argc,argv);
	absmaxconnections(argc,argv);

	sqlrcmdline	cmdl(argc,argv);
	sqlrpaths	sqlrpth(&cmdl);
	sqlrconfigs	sqlrcfgs(&sqlrpth);

	// get the command line args
	const char	*localstatedir=sqlrpth.getLocalStateDir();
	bool		strace=cmdl.found("-strace");
	const char	*id=cmdl.getValue("-id");
	const char	*configurl=sqlrpth.getConfigUrl();
	const char	*config=cmdl.getValue("-config");
	const char	*backtrace=cmdl.getValue("-backtrace");
	bool		disablecrashhandler=
				cmdl.found("-disable-crash-handler");
	#ifndef _WIN32
	bool		wait=cmdl.found("-wait");
	#endif

	// on Windows, open a new console window and redirect everything to it
	// (unless that's specifically disabled)
	#ifdef _WIN32
	if (!cmdl.found("-disable-new-window")) {
		fclose(stdin);
		fclose(stdout);
		fclose(stderr);
		FreeConsole();
		AllocConsole();
		stringbuffer	title;
		title.append(SQL_RELAY);
		if (!charstring::isNullOrEmpty(id)) {
			title.append(" - ");
			title.append(id);
		}
		SetConsoleTitle(title.getString());
		*stdin=*(_fdopen(_open_osfhandle(
					(long)GetStdHandle(STD_INPUT_HANDLE),
					_O_TEXT),"r"));
		setvbuf(stdin,NULL,_IONBF,0);
		*stdout=*(_fdopen(_open_osfhandle(
					(long)GetStdHandle(STD_OUTPUT_HANDLE),
					_O_TEXT),"w"));
		setvbuf(stdout,NULL,_IONBF,0);
		*stderr=*(_fdopen(_open_osfhandle(
					(long)GetStdHandle(STD_ERROR_HANDLE),
					_O_TEXT),"w"));
		setvbuf(stderr,NULL,_IONBF,0);
	}
	iswindows=true;
	#else
	iswindows=false;
	#endif

	// get the id
	linkedlist< char * >	ids;
	if (!charstring::isNullOrEmpty(id)) {
		ids.append(charstring::duplicate(id));
	} else {
		sqlrcfgs.getEnabledIds(configurl,&ids);
	}

	// start each enabled instance
	int32_t	exitstatus=0;
	for (linkedlistnode< char * > *node=ids.getFirst();
					node; node=node->getNext()) {

		// get the id
		char	*thisid=node->getValue();

		// load the configuration and
		// start listener, connections and scaler
		sqlrconfig	*cfg=sqlrcfgs.load(configurl,thisid);
		if (!cfg ||
			!startListener(&sqlrpth,thisid,
					config,localstatedir,
					backtrace,disablecrashhandler) ||
			!startConnections(&sqlrpth,cfg,thisid,
					config,localstatedir,
					strace,backtrace,
					disablecrashhandler) ||
			!startScaler(&sqlrpth,cfg,thisid,
					config,localstatedir,
					backtrace,disablecrashhandler)
			#ifndef _WIN32
			|| (wait && !waitForInstance(&sqlrpth,cfg,thisid))
			#endif
			) {
			exitstatus=1;
		}

		// clean up
		delete[] thisid;
	}

	// successful exit
	process::exit(exitstatus);
}
