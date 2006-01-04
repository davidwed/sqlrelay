// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

#include <rudiments/passwdentry.h>
#include <rudiments/groupentry.h>
#include <rudiments/process.h>

bool sqlrconnection_svr::initConnection(int argc, const char **argv,
						bool detachbeforeloggingin) {

	// process command line
	cmdl=new cmdline(argc,argv);

	// get the connection id from the command line
	connectionid=cmdl->value("-connectionid");
	if (!connectionid[0]) {
		connectionid=DEFAULT_CONNECTIONID;
		fprintf(stderr,"Warning: using default connectionid.\n");
	}
	// get the time to live from the command line
	ttl=charstring::toInteger(cmdl->value("-ttl"));

	cfgfl=new sqlrconfigfile();
	authc=new authenticator(cfgfl);
	tmpdir=new tempdir(cmdl);

	if (!cfgfl->parse(cmdl->getConfig(),cmdl->getId(),
					getNumberOfConnectStringVars())) {
		return false;
	}

	setUserAndGroup();

	#ifdef SERVER_DEBUG
	debugfile::openDebugFile("connection",cmdl->getLocalStateDir());
	#endif

	// handle the unix socket directory
	if (cfgfl->getListenOnUnix()) {
		setUnixSocketDirectory();
	}

	// handle the pid file
	if (!handlePidFile()) {
		return false;
	}

	constr=cfgfl->getConnectString(connectionid);
	if (!constr) {
		fprintf(stderr,"Error: invalid connectionid \"%s\".\n",
							connectionid);
		return false;
	}
	handleConnectString();

	initDatabaseAvailableFileName();

	if (cfgfl->getListenOnUnix() &&
		!getUnixSocket(tmpdir->getString(),unixsocketptr)) {
		return false;
	}

	#ifndef SERVER_DEBUG
	bool	nodetach=cmdl->found("-nodetach");

	if (!nodetach && detachbeforeloggingin) {
		// detach from the controlling tty
		detach();
	}
	#endif

	blockSignals();

	if (!attemptLogIn()) {
		return false;
	}

	#ifndef SERVER_DEBUG
	if (!nodetach && !detachbeforeloggingin) {
		// detach from the controlling tty
		detach();
	}
	#endif

	setInitialAutoCommitBehavior();

	// create sqlrconnection for sid database
	if (cfgfl->getSidEnabled()) {
		sid_sqlrcon=new sqlrconnection(cfgfl->getSidHost(),
						cfgfl->getSidPort(),
						cfgfl->getSidUnixPort(),
						cfgfl->getSidUser(),
						cfgfl->getSidPassword(),
						0,1);
sid_sqlrcon->debugOn();
	}

	if (!initCursors(true)) {
		return false;
	}

	if (!createSharedMemoryAndSemaphores(tmpdir->getString(),
							cmdl->getId())) {
		return false;
	}

	// increment connection counter
	if (cfgfl->getDynamicScaling()) {
		incrementConnectionCount();
	}

	markDatabaseAvailable();

	// update maximum query size, bind value lengths and idle client timeout
	maxquerysize=cfgfl->getMaxQuerySize();
	maxstringbindvaluelength=cfgfl->getMaxStringBindValueLength();
	maxlobbindvaluelength=cfgfl->getMaxLobBindValueLength();
	idleclienttimeout=cfgfl->getIdleClientTimeout();

	// if we're not passing descriptors around, listen on 
	// inet and unix sockets for client connections
	if (!cfgfl->getPassDescriptor()) {
		return openSockets();
	}
	return true;
}

void sqlrconnection_svr::setUserAndGroup() {

	// get the user that we're currently running as
	char	*currentuser=NULL;
	passwdentry::getName(process::getEffectiveUserId(),&currentuser);

	// get the group that we're currently running as
	char	*currentgroup=NULL;
	groupentry::getName(process::getEffectiveGroupId(),&currentgroup);

	// switch groups, but only if we're not currently running as the
	// group that we should switch to
	if (charstring::compare(currentgroup,cfgfl->getRunAsGroup()) &&
					!runAsGroup(cfgfl->getRunAsGroup())) {
		fprintf(stderr,"Warning: could not change group to %s\n",
						cfgfl->getRunAsGroup());
	}

	// switch users, but only if we're not currently running as the
	// user that we should switch to
	if (charstring::compare(currentuser,cfgfl->getRunAsUser()) &&
					!runAsUser(cfgfl->getRunAsUser())) {
		fprintf(stderr,"Warning: could not change user to %s\n",
						cfgfl->getRunAsUser());
	}

	// clean up
	delete[] currentuser;
	delete[] currentgroup;
}

void sqlrconnection_svr::setUnixSocketDirectory() {
	size_t	unixsocketlen=tmpdir->getLength()+31;
	unixsocket=new char[unixsocketlen];
	snprintf(unixsocket,unixsocketlen,"%s/sockets/",tmpdir->getString());
	unixsocketptr=unixsocket+tmpdir->getLength()+8+1;
}

bool sqlrconnection_svr::handlePidFile() {

	// check for pid file
	size_t	pidfilelen=tmpdir->getLength()+20+
				charstring::length(cmdl->getId())+1;
	char	*pidfile=new char[pidfilelen];
	snprintf(pidfile,pidfilelen,"%s/pids/sqlr-listener-%s",
				tmpdir->getString(),cmdl->getId());

	bool	retval=true;
	if (checkForPidFile(pidfile)==-1) {
		printf("\nsqlr-connection error:\n");
		printf("	The pid file %s",pidfile);
		printf(" was not found.\n");
		printf("	This usually means that the sqlr-listener \n");
		printf("is not running.\n");
		printf("	The sqlr-listener must be running ");
		printf("for the sqlr-connection to start.\n\n");
		retval=false;
	}

	delete[] pidfile;
	return retval;
}

void sqlrconnection_svr::initDatabaseAvailableFileName() {

	// initialize the database up/down filename
	size_t	updownlen=charstring::length(tmpdir->getString())+5+
					charstring::length(cmdl->getId())+1+
					charstring::length(connectionid)+1;
	updown=new char[updownlen];
	snprintf(updown,updownlen,"%s/ipc/%s-%s",
			tmpdir->getString(),cmdl->getId(),connectionid);
}

void sqlrconnection_svr::blockSignals() {

	// the daemon class handles SIGTERM's and SIGINT's
	// and we need to handle SIGALRMS so dynamically spawned daemons
	// can shut down but we're going to block all other signals
	signalset	set;
	set.removeAllSignals();

	#ifdef HAVE_SIGHUP
		set.addSignal(SIGHUP);
	#endif
	#ifdef HAVE_SIGQUIT
		set.addSignal(SIGQUIT);
	#endif
	#ifdef HAVE_SIGILL
		set.addSignal(SIGILL);
	#endif
	#ifdef HAVE_SIGTRAP
		set.addSignal(SIGTRAP);
	#endif
	#ifdef HAVE_SIGABRT
		set.addSignal(SIGABRT);
	#endif
	#ifdef HAVE_SIGIOT
		set.addSignal(SIGIOT);
	#endif
	#ifdef HAVE_SIGBUS
		set.addSignal(SIGBUS);
	#endif
	#ifdef HAVE_SIGFPE
		set.addSignal(SIGFPE);
	#endif
	#ifdef HAVE_SIGUSR1
		set.addSignal(SIGUSR1);
	#endif
	#ifdef HAVE_SIGSEGV
		set.addSignal(SIGSEGV);
	#endif
	#ifdef HAVE_SIGUSR2
		set.addSignal(SIGUSR2);
	#endif
	#ifdef HAVE_SIGPIPE
		set.addSignal(SIGPIPE);
	#endif
	#ifdef HAVE_SIGSTKFLT
		set.addSignal(SIGSTKFLT);
	#endif
	#ifdef HAVE_SIGCHLD
		set.addSignal(SIGCHLD);
	#endif
	#ifdef HAVE_SIGCONT
		set.addSignal(SIGCONT);
	#endif
	#ifdef HAVE_SIGSTOP
		set.addSignal(SIGSTOP);
	#endif
	#ifdef HAVE_SIGTSTP
		set.addSignal(SIGTSTP);
	#endif
	#ifdef HAVE_SIGTTIN
		set.addSignal(SIGTTIN);
	#endif
	#ifdef HAVE_SIGTTOU
		set.addSignal(SIGTTOU);
	#endif
	#ifdef HAVE_SIGURG
		set.addSignal(SIGURG);
	#endif
	#ifdef HAVE_SIGXCPU
		set.addSignal(SIGXCPU);
	#endif
	#ifdef HAVE_SIGXFSZ
		set.addSignal(SIGXFSZ);
	#endif
	#ifdef HAVE_SIGVTALRM
		set.addSignal(SIGVTALRM);
	#endif
	#ifdef HAVE_SIGPROF
		set.addSignal(SIGPROF);
	#endif
	#ifdef HAVE_SIGWINCH
		set.addSignal(SIGWINCH);
	#endif
	#ifdef HAVE_SIGIO
		set.addSignal(SIGIO);
	#endif
	#ifdef HAVE_SIGPOLL
		set.addSignal(SIGPOLL);
	#endif
	#ifdef HAVE_SIGPWR
		set.addSignal(SIGPWR);
	#endif
	#ifdef HAVE_SIGUNUSED
		set.addSignal(SIGUNUSED);
	#endif
	#ifdef HAVE_SIGEMT
		set.addSignal(SIGEMT);
	#endif
	#ifdef HAVE_SIGSYS
		set.addSignal(SIGSYS);
	#endif
	#ifdef HAVE_SIGCLD
		set.addSignal(SIGCLD);
	#endif
	#ifdef HAVE_SIGWAITING
		set.addSignal(SIGWAITING);
	#endif
	#ifdef HAVE_SIGLWP
		set.addSignal(SIGLWP);
	#endif
	#ifdef HAVE_SIGFREEZE
		set.addSignal(SIGFREEZE);
	#endif
	#ifdef HAVE_SIGTHAW
		set.addSignal(SIGTHAW);
	#endif
	#ifdef HAVE_SIGCANCEL
		set.addSignal(SIGCANCEL);
	#endif
	#ifdef HAVE_SIGLOST
		set.addSignal(SIGLOST);
	#endif
	#ifdef HAVE__SIGRTMIN
		set.addSignal(_SIGRTMIN);
	#endif
	#ifdef HAVE__SIGRTMAX
		set.addSignal(_SIGRTMAX);
	#endif
	#ifdef HAVE_SIGRTMIN
		set.addSignal(SIGRTMIN);
	#endif
	#ifdef HAVE_SIGRTMAX
		set.addSignal(SIGRTMAX);
	#endif

	signalmanager::ignoreSignals(set.getSignalSet());
}

bool sqlrconnection_svr::attemptLogIn() {

	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"logging in...");
	#endif
	if (!logIn()) {
		#ifdef SERVER_DEBUG
		debugPrint("connection",0,"log in failed");
		#endif
		fprintf(stderr,"Couldn't log into database.\n");
		return false;
	}
	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"done logging in");
	#endif

	return true;
}

void sqlrconnection_svr::setInitialAutoCommitBehavior() {

	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"setting autocommit...");
	#endif
	if (autocommit) {
		if (!autoCommitOn()) {
			#ifdef SERVER_DEBUG
			debugPrint("connection",0,
					"setting autocommit on failed");
			#endif
			fprintf(stderr,"Couldn't set autocommit on.\n");
			return;
		}
	} else {
		if (!autoCommitOff()) {
			#ifdef SERVER_DEBUG
			debugPrint("connection",0,
					"setting autocommit off failed");
			#endif
			fprintf(stderr,"Couldn't set autocommit off.\n");
			return;
		}
	}
	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"done setting autocommit");
	#endif
}

bool sqlrconnection_svr::initCursors(bool create) {

	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"initializing cursors...");
	#endif

	int32_t	cursorcount=cfgfl->getCursors();
	if (create) {
		cur=new sqlrcursor_svr *[cursorcount];
		for (int32_t i=0; i<cursorcount; i++) {
			cur[i]=NULL;
		}
	}

	for (int32_t i=0; i<cursorcount; i++) {

		#ifdef SERVER_DEBUG
		debugPrint("connection",1,i);
		#endif

		if (create) {
			cur[i]=initCursor();
			// FIXME: LAME!!!  oh god this is lame....
			cur[i]->querybuffer=new char[
						cfgfl->getMaxQuerySize()+1];
		}
		if (!cur[i]->openCursor(i)) {

			#ifdef SERVER_DEBUG
			debugPrint("connection",1,"cursor init failure...");
			#endif

			logOut();
			fprintf(stderr,"Couldn't create cursors.\n");
			return false;
		}
	}

	// end sid database session
	// FIXME: is there a better place for this?
	if (cfgfl->getSidEnabled()) {
		sid_sqlrcon->endSession();
	}

	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"done initializing cursors");
	#endif

	return true;
}
