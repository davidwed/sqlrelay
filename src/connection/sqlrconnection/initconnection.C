// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

#include <sys/types.h>
#ifdef HAVE_UNISTD_H
	#include <unistd.h>
#endif

int	sqlrconnection::initConnection(int argc, const char **argv,
						int detachbeforeloggingin) {

	init=1;

	cmdl=new connectioncmdline(argc,argv);
	cfgfl=new sqlrconfigfile();
	authc=new authenticator(cfgfl);
	ipcptr=new ipc();
	lsnrcom=new listenercomm(ipcptr,cmdl);
	sclrcom=new scalercomm(ipcptr);
	ussf=new unixsocketseqfile();
	tmpdir=new tempdir(cmdl);

	if (!cfgfl->parse(cmdl->getConfig(),cmdl->getId(),
					getNumberOfConnectStringVars())) {
		return 0;
	}

	setUserAndGroup();

	#ifdef SERVER_DEBUG
		// set loggers here...
		debugfile::openDebugFile("connection",cmdl->getLocalStateDir());
		ipcptr->setDebugLogger(getDebugLogger());
		sclrcom->setDebugLogger(getDebugLogger());
		ussf->setDebugLogger(getDebugLogger());
		lsnrcom->setDebugLogger(getDebugLogger());
	#endif

	// handle the unix socket directory
	if (cfgfl->getListenOnUnix()) {
		setUnixSocketDirectory();
	}

	// handle the pid file
	if (!handlePidFile()) {
		return 0;
	}

	createCursorArray();

	constr=cfgfl->getConnectString(cmdl->getConnectionId());
	handleConnectString();

	initDatabaseAvailableFileName();

	if (cfgfl->getListenOnUnix() &&
		!ussf->getUnixSocket(tmpdir->getString(),unixsocketptr)) {
		return 0;
	}

	#ifndef SERVER_DEBUG
		if (detachbeforeloggingin) {
			// detach from the controlling tty
			detach();
		}
	#endif

	blockSignals();

	if (!attemptLogIn()) {
		return 0;
	}

	#ifndef SERVER_DEBUG
		if (!detachbeforeloggingin) {
			// detach from the controlling tty
			detach();
		}
	#endif

	setInitialAutoCommitBehavior();

	if (!initCursors(1)) {
		return 0;
	}

	if (!ipcptr->createSharedMemoryAndSemaphores(tmpdir->getString(),
							cmdl->getId())) {
		return 0;
	}

	// increment connection counter
	if (cfgfl->getDynamicScaling()) {
		sclrcom->incrementConnectionCount();
	}

	markDatabaseAvailable();

	// if we're not passing descriptors around, listen on 
	// inet and unix sockets for client connections
	if (!cfgfl->getPassDescriptor()) {
		return openSockets();
	}

	return 1;
}

void	sqlrconnection::setUserAndGroup() {

	if (!runAsGroup(cfgfl->getRunAsGroup())) {
		fprintf(stderr,"Warning: could not change group to %s\n",
						cfgfl->getRunAsGroup());
	}

	if (!runAsUser(cfgfl->getRunAsUser())) {
		fprintf(stderr,"Warning: could not change user to %s\n",
						cfgfl->getRunAsUser());
	}
}

void	sqlrconnection::setUnixSocketDirectory() {
	unixsocket=new char[tmpdir->getLength()+23];
	sprintf(unixsocket,"%s/",tmpdir->getString());
	unixsocketptr=unixsocket+tmpdir->getLength()+1;
}

int	sqlrconnection::handlePidFile() {

	// check for pid file
	char	*pidfile=new char[tmpdir->getLength()+
				15+strlen(cmdl->getId())+1];
	sprintf(pidfile,"%s/sqlr-listener-%s",
				tmpdir->getString(),cmdl->getId());

	int	retval=1;
	if (checkForPidFile(pidfile)==-1) {
		printf("\nsqlr-connection error:\n");
		printf("	The file %s",tmpdir->getString());
		printf("/sqlr-listener-%s",cmdl->getId());
		printf(" was not found.\n");
		printf("	This usually means that the sqlr-listener \n");
		printf("is not running.\n");
		printf("	The sqlr-listener must be running ");
		printf("for the sqlr-connection to start.\n\n");
		retval=0;
	}

	delete[] pidfile;
	return retval;
}

void	sqlrconnection::createCursorArray() {

	// how many cursors should we use, create an array for them
	int	cursorcount=cfgfl->getCursors();
	cur=new sqlrcursor *[cursorcount];
	for (int i=0; i<cursorcount; i++) {
		cur[i]=NULL;
	}
}

void	sqlrconnection::initDatabaseAvailableFileName() {

	// initialize the database up/down filename
	updown=new char[strlen(tmpdir->getString())+1+strlen(cmdl->getId())+1+
					strlen(cmdl->getConnectionId())+1];
	sprintf(updown,"%s/%s-%s",tmpdir->getString(),cmdl->getId(),
						cmdl->getConnectionId());
}

void	sqlrconnection::blockSignals() {

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

int	sqlrconnection::attemptLogIn() {

	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"logging in...");
	#endif
	if (!logIn()) {
		#ifdef SERVER_DEBUG
		debugPrint("connection",0,"log in failed");
		#endif
		fprintf(stderr,"Couldn't log into database.\n");
		return 0;
	}
	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"done logging in");
	#endif

	return 1;
}

void	sqlrconnection::setInitialAutoCommitBehavior() {

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

int	sqlrconnection::initCursors(int create) {

	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"initializing cursors...");
	#endif

	for (int i=0; i<cfgfl->getCursors(); i++) {

		#ifdef SERVER_DEBUG
		debugPrint("connection",1,(long)i);
		#endif

		if (create) {
			cur[i]=initCursor();
		}
		if (!cur[i]->openCursor(i)) {

			#ifdef SERVER_DEBUG
			debugPrint("connection",1,"cursor init failure...");
			#endif

			logOut();
			fprintf(stderr,"Couldn't create cursors.\n");
			return 0;
		}
	}

	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"done initializing cursors");
	#endif

	return 1;
}
