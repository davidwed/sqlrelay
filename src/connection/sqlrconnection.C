// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <config.h>
#include <sqlrconfigfile.h>
#include <sqlrconnection.h>

#include <rudiments/permissions.h>
#ifdef SERVER_DEBUG
	#include <rudiments/logger.h>
#endif

#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#ifdef HAVE_UNISTD_H
	#include <unistd.h>
#endif
#include <string.h>
#ifdef HAVE_STRINGS_H
	#include <strings.h>
#endif

#include <defines.h>
#include <datatypes.h>
#include <defaults.h>


sqlrconnection::sqlrconnection() : daemonprocess(), listener(), debugfile() {

	cmdl=NULL;
	cfgfl=NULL;
	ipcptr=NULL;
	lsnrcom=NULL;
	sclrcom=NULL;
	ussf=NULL;

	updown=NULL;

	tmpdir=NULL;

	init=0;

	unixsocket=NULL;
	unixsocketptr=NULL;
	serversockun=NULL;
	serversockin=NULL;

	inetport=0;
	authc=NULL;
	lastuserbuffer[0]=(char)NULL;
	lastpasswordbuffer[0]=(char)NULL;
	lastauthsuccess=0;

	autocommit=0;
	checkautocommit=0;
	performautocommit=0;

	// maybe someday these parameters will be configurable
	bindpool=new memorypool(512,128,100);
}

sqlrconnection::~sqlrconnection() {

	delete cmdl;
	delete cfgfl;
	delete sclrcom;
	delete ussf;
	delete lsnrcom;

	delete[] updown;

	delete tmpdir;

	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"deleting authc...");
	#endif
	delete authc;
	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"done deleting authc");
	#endif

	delete ipcptr;

	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"deleting unixsocket...");
	#endif
	if (unixsocket) {
		unlink(unixsocket);
		delete[] unixsocket;
	}
	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"done deleting unixsocket");
	#endif

	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"deleting bindpool...");
	#endif
	delete bindpool;
	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"done deleting bindpool");
	#endif
}

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

	if (!setUserAndGroup()) {
		return 0;
	}

	#ifdef SERVER_DEBUG
		// set loggers here...
		debugfile::openDebugFile("connection",cmdl->getLocalStateDir());
		ipcptr->setDebugLogger(getDebugLogger());
		sclrcom->setDebugLogger(getDebugLogger());
		ussf->setDebugLogger(getDebugLogger());
		lsnrcom->setDebugLogger(getDebugLogger());
	#endif

	// handle the unix socket directory
	setUnixSocketDirectory();

	// handle the pid file
	if (!handlePidFile()) {
		return 0;
	}

	createCursorArray();

	constr=cfgfl->getConnectString(cmdl->getConnectionId());
	handleConnectString();

	initDatabaseAvailableFileName();

	if (!ussf->getUnixSocket(tmpdir->getString(),unixsocketptr)) {
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

void	sqlrconnection::initDatabaseAvailableFileName() {

	// initialize the database up/down filename
	updown=new char[strlen(tmpdir->getString())+1+strlen(cmdl->getId())+1+
					strlen(cmdl->getConnectionId())+1];
	sprintf(updown,"%s/%s-%s",tmpdir->getString(),cmdl->getId(),
						cmdl->getConnectionId());
}

void	sqlrconnection::markDatabaseAvailable() {

	#ifdef SERVER_DEBUG
	char	*string=new char[9+strlen(updown)+1];
	sprintf(string,"creating %s",updown);
	getDebugLogger()->write("connection",4,string);
	delete[] string;
	#endif

	// the database is up if the file is there, 
	// opening and closing it will create it
	int	fd=creat(updown,permissions::ownerReadWrite());
	close(fd);
}

void	sqlrconnection::setUnixSocketDirectory() {
	unixsocket=new char[tmpdir->getLength()+23];
	sprintf(unixsocket,"%s/",tmpdir->getString());
	unixsocketptr=unixsocket+tmpdir->getLength()+1;
}

char	*sqlrconnection::connectStringValue(const char *variable) {
	return constr->getConnectStringValue(variable);
}

void	sqlrconnection::createCursorArray() {

	// how many cursors should we use, create an array for them
	int	cursorcount=cfgfl->getCursors();
	cur=new sqlrcursor *[cursorcount];
	for (int i=0; i<cursorcount; i++) {
		cur[i]=NULL;
	}
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

int	sqlrconnection::getCursor() {

	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"getting a cursor...");
	#endif

	// which cursor is the client requesting?
	unsigned short	index;
	if (clientsock->read(&index)!=sizeof(unsigned short)) {
		#ifdef SERVER_DEBUG
		debugPrint("connection",2,
				"error: client cursor request failed");
		#endif
		return 0;
	}

	// don't allow the client to request a cursor 
	// beyond the end of the array.
	if (index>cfgfl->getCursors()) {
		#ifdef SERVER_DEBUG
		debugPrint("connection",2,
				"error: client requested an invalid cursor");
		#endif
		return 0;
	}

	// set the current cursor to the one they requested.
	currentcur=index;
	cur[index]->busy=1;

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"returning requested cursor");
	debugPrint("connection",1,"done getting a cursor");
	#endif
	return 1;
}

int	sqlrconnection::findAvailableCursor() {

	for (int i=0; i<cfgfl->getCursors(); i++) {
		if (!cur[i]->busy) {
			cur[i]->busy=1;
			#ifdef SERVER_DEBUG
			debugPrint("connection",3,(long)currentcur);
			debugPrint("connection",2,"found a free cursor...");
			debugPrint("connection",2,"done getting a cursor");
			#endif
			return i;
		}
	}
	#ifdef SERVER_DEBUG
	debugPrint("connection",1,
			"find available cursor failed: all cursors are busy");
	#endif
	return -1;
}

void	sqlrconnection::closeCursors(int destroy) {

	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"closing cursors...");
	#endif

	if (cur) {
		for (int i=0; i<cfgfl->getCursors(); i++) {

			#ifdef SERVER_DEBUG
			debugPrint("connection",1,(long)i);
			#endif

			if (cur[i]) {
				cur[i]->closeCursor();
				if (destroy) {
					deleteCursor(cur[i]);
				}
			}
		}
		if (destroy) {
			delete[] cur;
		}
	}

	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"done closing cursors...");
	#endif
}

int	sqlrconnection::setUserAndGroup() {

	// don't even try to change users/groups unless we're root
	if (geteuid()) {
		return 1;
	}

	if (!runAsGroup(cfgfl->getRunAsGroup())) {
		fprintf(stderr,"Could not change group to %s\n",
						cfgfl->getRunAsGroup());
		return 0;
	}

	if (!runAsUser(cfgfl->getRunAsUser())) {
		fprintf(stderr,"Could not change user to %s\n",
						cfgfl->getRunAsUser());
		return 0;
	}

	return 1;
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

void	sqlrconnection::closeConnection() {

	// decrement the connection counter
	if (cfgfl->getDynamicScaling() && ipcptr->initialized()) {
		sclrcom->decrementConnectionCount();
	}

	// deregister and close the handoff socket if necessary
	if (cfgfl->getPassDescriptor()) {
		lsnrcom->deRegisterForHandoff(tmpdir->getString());
	}

	// close the cursors
	closeCursors(1);


	// try to log out
	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"logging out...");
	#endif
	logOut();
	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"done logging out");
	#endif


	// clear the pool
	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"removing all sockets...");
	#endif
	removeAllFileDescriptors();
	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"done removing all sockets");
	#endif


	// close, clean up all sockets
	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"deleting unix socket...");
	#endif
	delete serversockun;
	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"done deleting unix socket");
	#endif


	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"deleting inet socket...");
	#endif
	delete serversockin;
	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"done deleting inet socket");
	#endif
}

void	sqlrconnection::listen() {

	for (;;) {

		waitForAvailableDatabase();
		initSession();
		lsnrcom->announceAvailability(tmpdir->getString(),
						cfgfl->getPassDescriptor(),
						unixsocket,
						inetport,
						cmdl->getConnectionId());

		// loop to handle suspended sessions
		for (;;) {

			int	success=waitForClient();
			if (success==1) {

				suspendedsession=0;

				// have a session with the client
				clientSession();

				// break out of the loop unless the client
				// suspended the session
				if (!suspendedsession) {
					break;
				}

			} else if (success==-1) {

				// if waitForClient() errors out, break out of
				// the suspendedsession loop and loop back
				// for another session
				break;

			} else {

				// if waitForClient() times out waiting for
				// someone to pick up the suspended
				// session, roll it back and kill it
				if (suspendedsession) {
					if (isTransactional()) {
						rollback();
					}
					suspendedsession=0;
				}
			}
		}

		if (cfgfl->getDynamicScaling()) {
			sclrcom->decrementSessionCount();
		}
	}
}

void	sqlrconnection::waitForAvailableDatabase() {

	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"waiting for available database...");
	#endif

	if (!availableDatabase()) {
		reLogIn();
		markDatabaseAvailable();
	}

	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"database is available");
	#endif
}

int	sqlrconnection::availableDatabase() {

	// return whether the file "updown" is there or not
	#ifdef SERVER_DEBUG
		if (!file::exists(updown)) {
			getDebugLogger()->write("connection",0,"database is not available");
			return 0;
		} else {
			getDebugLogger()->write("connection",0,"database is available");
			return 1;
		}
	#else
		return file::exists(updown);
	#endif
}

int	sqlrconnection::openSockets() {

	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"listening on sockets...");
	#endif

	// get the next available unix socket and open it
	if (cfgfl->getListenOnUnix() && unixsocketptr[0]) {

		#ifdef SERVER_DEBUG
		char	*string=new char[26+strlen(unixsocket)+1];
		sprintf(string,"listening on unix socket: %s",unixsocket);
		debugPrint("connection",1,string);
		delete[] string;
		#endif

		if (!serversockun) {
			serversockun=new unixserversocket();
			if (serversockun->listenOnSocket(unixsocket,0000,5)) {
				addFileDescriptor(
					serversockun->getFileDescriptor());
			} else {
				fprintf(stderr,"Could not listen on ");
				fprintf(stderr,"unix socket: ");
				fprintf(stderr,"%s\n",unixsocket);
				fprintf(stderr,"Make sure that the file and ");
				fprintf(stderr,"directory are readable ");
				fprintf(stderr,"and writable.\n\n");
				delete serversockun;
				return 0;
			}
		}
	}

	// open the next available inet socket
	if (cfgfl->getListenOnInet()) {

		#ifdef SERVER_DEBUG
		char	string[33];
		sprintf(string,"listening on inet socket: %d",inetport);
		debugPrint("connection",1,string);
		#endif

		if (!serversockin) {
			serversockin=new inetserversocket();
			if (serversockin->listenOnSocket(NULL,inetport,5)) {
				if (inetport==0) {
					inetport=serversockin->getPort();
				}
				addFileDescriptor(
					serversockin->getFileDescriptor());
			} else {
				fprintf(stderr,"Could not listen on ");
				fprintf(stderr,"inet socket: ");
				fprintf(stderr,"%d\n\n",inetport);
				delete serversockin;
				return 0;
			}
		}
	}

	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"done listening on sockets");
	#endif

	return 1;
}

int	sqlrconnection::waitForClient() {

	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"waiting for client...");
	#endif

	// Unless we're in the middle of a suspended session, if we're passing 
	// file descriptors around, wait for one to be passed to us, otherwise,
	// accept on the unix/inet sockets. 
	if (!suspendedsession && cfgfl->getPassDescriptor()) {

		// receive the descriptor and use it, if we failed to get the
		// descriptor, delete the socket and return failure
		int	descriptor;
		if (!lsnrcom->receiveFileDescriptor(&descriptor)) {

			#ifdef SERVER_DEBUG
			debugPrint("connection",1,"pass failed");
			debugPrint("connection",0,"done waiting for client");
			#endif

			return -1;
		}
		clientsock=new datatransport(descriptor);

		#ifdef SERVER_DEBUG
		debugPrint("connection",1,"pass succeeded");
		debugPrint("connection",0,"done waiting for client");
		#endif

	} else {

		int	fd=waitForNonBlockingRead(accepttimeout,0);
		if (fd==serversockin->getFileDescriptor()) {
			clientsock=serversockin->acceptClientConnection();
		} else if (fd==serversockun->getFileDescriptor()) {
			clientsock=serversockun->acceptClientConnection();
		}

		#ifdef SERVER_DEBUG
		if (fd>-1) {
			debugPrint("connection",1,"reconnect succeeded");
		} else {
			debugPrint("connection",1,"reconnect failed");
		}
		debugPrint("connection",0,"done waiting for client");
		#endif

		if (fd==-1) {
			return -1;
		}
	}
	return 1;
}

void	sqlrconnection::clientSession() {

	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"client session...");
	#endif

	// a session consists of getting a query and returning a result set
	// over and over
	for (;;) {

		// is this a query, fetch, abort, suspend or resume...
		unsigned short	command;
		if (!getCommand(&command)) {
			endSession();
			break;
		}

		// handle some things up front
		if (command==AUTHENTICATE) {
			if (authenticateCommand()) {
				continue;
			}
			break;
		} else if (command==SUSPEND_SESSION) {
			suspendSessionCommand();
			break;
		} else if (command==END_SESSION) {
			endSessionCommand();
			break;
		} else if (command==PING) {
			pingCommand();
			continue;
		} else if (command==IDENTIFY) {
			identifyCommand();
			continue;
		} else if (command==AUTOCOMMIT) {
			autoCommitCommand();
			continue;
		} else if (command==COMMIT) {
			commitCommand();
			continue;
		} else if (command==ROLLBACK) {
			rollbackCommand();
			continue;
		} else if (command==NEW_QUERY) {
			if (newQueryCommand()) {
				continue;
			}
			break;
		}

		// For the rest of the commands, the client will be requesting
		// a cursor.  Get the cursor to work with.  Save the result of
		// this, the client may be sending more information and we need
		// to collect it.
		if (!getCursor()) {
			getQueryFromClient(0);
			noAvailableCursors();
			continue;
		}

		if (command==REEXECUTE_QUERY) {
			if (!reExecuteQueryCommand()) {
				break;
			}
		} else if (command==FETCH_FROM_BIND_CURSOR) {
			if (!fetchFromBindCursorCommand()) {
				break;
			}
		} else if (command==FETCH_RESULT_SET) {
			if (!fetchResultSetCommand()) {
				break;
			}
		} else if (command==ABORT_RESULT_SET) {
			abortResultSetCommand();
		} else if (command==SUSPEND_RESULT_SET) {
			suspendResultSetCommand();
		} else if (command==RESUME_RESULT_SET) {
			if (!resumeResultSetCommand()) {
				break;
			}
		} else {
			endSession();
			break;
		}
	}

	waitForClientClose();

	closeSuspendedSessionSockets();

	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"done with client session");
	#endif
}

int	sqlrconnection::authenticateCommand() {

	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"authenticate");
	#endif

	if (!authenticate()) {
		// indicate that an error has occurred
		clientsock->write((unsigned short)ERROR);
		endSession();
		return 0;
	}
	// indicate that no error has occurred
	clientsock->write((unsigned short)NO_ERROR);
	return 1;
}

int	sqlrconnection::authenticate() {

	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"authenticate...");
	#endif

	// get the user/password from the client
	if (!getUserFromClient() || !getPasswordFromClient()) {
		return 0;
	}

	// authenticate on the approprite tier
	if (cfgfl->getAuthOnConnection()) {
		return connectionBasedAuth(userbuffer,passwordbuffer);
	} else if (cfgfl->getAuthOnDatabase()) {
		return databaseBasedAuth(userbuffer,passwordbuffer);
	}

	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"authentication was done on listener");
	#endif
	return 1;
}

int	sqlrconnection::getUserFromClient() {
	unsigned long size=0;
	clientsock->read(&size);
	if (size>(unsigned long)USERSIZE ||
		(unsigned long)(clientsock->read(userbuffer,size))!=size) {
		#ifdef SERVER_DEBUG
		debugPrint("connection",1,
			"authentication failed: user size is wrong");
		#endif
		return 0;
	}
	userbuffer[size]=(char)NULL;
	return 1;
}

int	sqlrconnection::getPasswordFromClient() {
	unsigned long size=0;
	clientsock->read(&size);
	if (size>(unsigned long)USERSIZE ||
		(unsigned long)(clientsock->read(passwordbuffer,size))!=size) {
		#ifdef SERVER_DEBUG
		debugPrint("connection",1,
			"authentication failed: password size is wrong");
		#endif
		return 0;
	}
	passwordbuffer[size]=(char)NULL;
	return 1;
}

int	sqlrconnection::connectionBasedAuth(const char *userbuffer,
						const char *passwordbuffer) {

	// handle connection-based authentication
	int	retval=authc->authenticate(userbuffer,passwordbuffer);
	#ifdef SERVER_DEBUG
	if (retval) {
		debugPrint("connection",1,
			"connection-based authentication succeeded");
	} else {
		debugPrint("connection",1,
			"connection-based authentication failed: invalid user/password");
	}
	#endif
	return retval;
}

int	sqlrconnection::databaseBasedAuth(const char *userbuffer,
						const char *passwordbuffer) {

	// if the user we want to change to is different from the
	// user that's currently proxied, try to change to that user
	int	authsuccess;
	if ((!lastuserbuffer[0] && !lastpasswordbuffer[0]) || 
		strcmp(lastuserbuffer,userbuffer) ||
		strcmp(lastpasswordbuffer,passwordbuffer)) {

		// change authentication 
		authsuccess=changeUser(userbuffer,passwordbuffer);

		// keep a record of which user we're changing to
		// and whether that user was successful in 
		// authenticating
		strcpy(lastuserbuffer,userbuffer);
		strcpy(lastpasswordbuffer,passwordbuffer);
		lastauthsuccess=authsuccess;
	}

	#ifdef SERVER_DEBUG
	if (lastauthsuccess) {
		debugPrint("connection",1,
			"database-based authentication succeeded");
	} else {
		debugPrint("connection",1,
			"database-based authentication failed: invalid user/password");
	}
	#endif
	return lastauthsuccess;
}

int	sqlrconnection::changeUser(const char *newuser,
					const char *newpassword) {

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"change user");
	#endif

	closeCursors(1);
	logOut();
	setUser(newuser);
	setPassword(newpassword);
	return (initCursors(1) && logIn());
}

void	sqlrconnection::suspendSessionCommand() {
	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"suspend session");
	#endif
	suspendSession();
}

void	sqlrconnection::endSessionCommand() {
	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"end session");
	#endif
	endSession();
}

void	sqlrconnection::pingCommand() {
	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"ping");
	#endif
	clientsock->write((unsigned short)ping());
}

void	sqlrconnection::identifyCommand() {
	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"identify");
	#endif
	char		*ident=identify();
	unsigned short	idlen=(unsigned short)strlen(ident);
	clientsock->write(idlen);
	clientsock->write(ident,idlen);
}

void	sqlrconnection::autoCommitCommand() {
	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"autocommit...");
	#endif
	unsigned short	on;
	clientsock->read(&on);
	if (on) {
		#ifdef SERVER_DEBUG
		debugPrint("connection",2,"autocommit on");
		#endif
		clientsock->write(autoCommitOn());
	} else {
		#ifdef SERVER_DEBUG
		debugPrint("connection",2,"autocommit off");
		#endif
		clientsock->write(autoCommitOff());
	}
}

void	sqlrconnection::commitCommand() {
	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"commit");
	#endif
	clientsock->write((unsigned short)commit());
	commitorrollback=0;
}

void	sqlrconnection::rollbackCommand() {
	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"rollback");
	#endif
	clientsock->write((unsigned short)rollback());
	commitorrollback=0;
}

int	sqlrconnection::newQueryCommand() {

	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"new query");
	#endif

	// find an available cursor
	if ((currentcur=findAvailableCursor())==-1) {
		getQueryFromClient(0);
		noAvailableCursors();
		return 1;
	}

	// handle query will return 1 for success,
	// 0 for network error and -1 for a bad query
	int	querystatus=handleQuery(0,1);
	if (querystatus==1) {

		// reinit lastrow
		lastrow=-1;
		if (!returnResultSetData()) {
			endSession();
			return 0;
		}
		return 1;

	} else if (querystatus==-1) {
		return 1;
	} else {
		endSession();
		return 0;
	}
}

int	sqlrconnection::reExecuteQueryCommand() {

	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"re-execute query");
	#endif

	// handle query will return 1 for success,
	// 0 for network error and -1 for a bad query
	int	querystatus=handleQuery(1,1);
	if (querystatus==1) {

		// reinit lastrow
		lastrow=-1;
		if (!returnResultSetData()) {
			endSession();
			return 0;
		}
		return 1;

	} else if (querystatus==-1) {
		return 1;
	} else {
		endSession();
		return 0;
	}
}

int	sqlrconnection::fetchFromBindCursorCommand() {

	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"fetch from bind cursor");
	#endif

	// handle query will return 1 for success,
	// 0 for network error and -1 for a bad query
	int	querystatus=handleQuery(1,0);
	if (querystatus==1) {

		// reinit lastrow
		lastrow=-1;
		if (!returnResultSetData()) {
			endSession();
			return 0;
		}
		return 1;

	} else if (querystatus==-1) {
		return 1;
	} else {
		endSession();
		return 0;
	}
}

int	sqlrconnection::fetchResultSetCommand() {

	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"fetch result set");
	#endif
	if (!returnResultSetData()) {
		endSession();
		return 0;
	}
	return 1;
}

void	sqlrconnection::abortResultSetCommand() {
	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"abort result set");
	#endif
	cur[currentcur]->abort();
}

void	sqlrconnection::suspendResultSetCommand() {
	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"suspend result set");
	#endif
	cur[currentcur]->suspendresultset=1;
}

int	sqlrconnection::resumeResultSetCommand() {
	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"resume result set");
	#endif
	resumeResultSet();
	if (!returnResultSetData()) {
		endSession();
		return 0;
	}
	return 1;
}

void	sqlrconnection::waitForClientClose() {

	// Sometimes the server sends the result set and closes the socket
	// while part of it is buffered but not yet transmitted.  This caused
	// the client to receive a partial result set or error.  Telling the
	// socket to linger doesn't always fix it.  Doing a read here should 
	// guarantee that the client will close it's end of the connection 
	// before the server closes it's end; the server will wait for data 
	// from the client (which it will never receive) and when the client 
	// closes it's end (which it will only do after receiving the entire
	// result set) the read will fall through.  This should guarantee 
	// that the client will get the the entire result set without
	// requiring the client to send data back indicating so.
	#ifdef SERVER_DEBUG
	debugPrint("connection",1,
			"waiting for client to close the connection...");
	#endif
	unsigned short	dummy;
	clientsock->read(&dummy);
	clientsock->close();
	delete clientsock;
	#ifdef SERVER_DEBUG
	debugPrint("connection",1,
			"done waiting for client to close the connection...");
	#endif
}

void	sqlrconnection::closeSuspendedSessionSockets() {

	// If we're no longer in a suspended session and we we're passing 
	// around file descriptors but had to open a set of sockets to handle 
	// a suspended session, close those sockets here.
	if (!suspendedsession && cfgfl->getPassDescriptor()) {
		#ifdef SERVER_DEBUG
		debugPrint("connection",1,
			"closing sockets from a previously suspended session...");
		#endif
		if (serversockun) {
			removeFileDescriptor(serversockun->getFileDescriptor());
			delete serversockun;
			serversockun=NULL;
		}
		if (serversockin) {
			removeFileDescriptor(serversockin->getFileDescriptor());
			delete serversockin;
			serversockin=NULL;
		}
		#ifdef SERVER_DEBUG
		debugPrint("connection",1,
			"done closing sockets from a previously suspended session...");
		#endif
	}
}

int	sqlrconnection::handleQuery(int reexecute, int reallyexecute) {

	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"handling query...");
	#endif

	if (!getQueryFromClient(reexecute)) {
		#ifdef SERVER_DEBUG
		debugPrint("connection",1,"failed to handle query");
		#endif
		return 0;
	}

	// loop here to handle down databases
	for (;;) {

		// process the query
		if (processQuery(reexecute,reallyexecute)) {

			// indicate that no error has occurred
			clientsock->write((unsigned short)NO_ERROR);

			// send the client the id of the 
			// cursor that it's going to use
			clientsock->write((unsigned short)currentcur);

			// tell the client that this is not a
			// suspended result set
			clientsock->write(
				(unsigned short)NO_SUSPENDED_RESULT_SET);

			// if the query processed 
			// ok then return a result set
			// header and loop back to send the
			// result set itself...
			returnResultSetHeader();

			// free memory used by binds
			bindpool->free();

			#ifdef SERVER_DEBUG
			debugPrint("connection",1,"handle query succeeded");
			#endif
			return 1;

		} else {

			// If the query didn't process ok,
			// handle the error.
			// If handleError returns a 0 then the error 
			// was a down database that has presumably
			// come back up by now.  Loop back...
			if (handleError()) {

				// client will be sending skip/fetch,
				// better get it even though we're not gonna
				// use it
				unsigned long	skipfetch;
				clientsock->read(&skipfetch);
				clientsock->read(&skipfetch);

				cur[currentcur]->abort();
				#ifdef SERVER_DEBUG
				debugPrint("connection",1,
					"failed to handle query: error");
				#endif
				return -1;
			}
		}
	}

	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"done handling query");
	#endif
	return 1;
}

int	sqlrconnection::getQueryFromClient(int reexecute) {

	// get the query unless we're re-executing,
	// get the binds, column flag from the client
	return ((reexecute)?1:getQuery()) &&
			getInputBinds() && 
			getOutputBinds() &&
			getSendColumnInfo();
}

void	sqlrconnection::resumeResultSet() {

	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"resume result set...");
	#endif

	if (cur[currentcur]->suspendresultset) {

		#ifdef SERVER_DEBUG
		debugPrint("connection",2,"previous result set was suspended");
		#endif

		// indicate that no error has occurred
		clientsock->write((unsigned short)NO_ERROR);

		// send the client the id of the 
		// cursor that it's going to use
		clientsock->write((unsigned short)(currentcur));
		clientsock->write((unsigned short)SUSPENDED_RESULT_SET);

		// if the requested cursor really had a suspended
		// result set, send the lastrow of it to the client
		// then send the result set header
		clientsock->write((unsigned long)lastrow);
		returnResultSetHeader();
	} else {

		#ifdef 	SERVER_DEBUG
		debugPrint("connection",2,
				"previous result set was not suspended");
		#endif

		// indicate that an error has occurred
		clientsock->write((unsigned short)ERROR);

		// send the error itself
		clientsock->write((unsigned short)43);
		clientsock->write("The requested result set was not suspended.",
					43);
	}

	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"done resuming result set");
	#endif
}

void	sqlrconnection::suspendSession() {

	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"suspending session...");
	#endif

	// abort all cursors that aren't already suspended
	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"aborting busy, unsuspended cursors...");
	#endif
	suspendedsession=1;
	accepttimeout=cfgfl->getSessionTimeout();
	for (int i=0; i<cfgfl->getCursors(); i++) {
		if (!cur[i]->suspendresultset && cur[i]->busy) {
			#ifdef SERVER_DEBUG
			debugPrint("connection",3,(long)i);
			#endif
			cur[i]->abort();
		}
	}

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"done aborting busy, unsuspended cursors");
	#endif

	// If we're passing file descriptors around, we'll have to listen on a 
	// set of ports like we would if we were not passing descriptors around 
	// so the suspended client has something to resume to.  It's possible 
	// that the current session is just a resumed session though.  In that
	// case, no new sockets will be opened, the old ones will just be 
	// reused.  We'll also have to pass the socket/port to the client here.
	if (cfgfl->getPassDescriptor()) {

		#ifdef SERVER_DEBUG
		debugPrint("connection",2,"opening a socket to resume on...");
		#endif
		if (!openSockets()) {
			// send the client a 0 sized unix port and a 0 for the
			// inet port if an error occurred opening the sockets
			clientsock->write((unsigned short)0);
			clientsock->write((unsigned short)0);
		}
		#ifdef SERVER_DEBUG
		debugPrint("connection",2,"done opening a socket to resume on");
		#endif

		#ifdef SERVER_DEBUG
		debugPrint("connection",2,"passing socket info to client...");
		#endif
		if (serversockun) {
			unsigned short	unixsocketsize=strlen(unixsocket);
			clientsock->write(unixsocketsize);
			clientsock->write(unixsocket,unixsocketsize);
		} else {
			clientsock->write((unsigned short)0);
		}
		clientsock->write(inetport);
		#ifdef SERVER_DEBUG
		debugPrint("connection",2,
				"done passing socket info to client...");
		#endif
	}

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"done suspending session");
	#endif
}

void	sqlrconnection::endSession() {

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"ending session...");
	#endif

	dropTempTables(&sessiontemptables);

	// must set suspendedsession to 0 here so resumed sessions won't 
	// automatically re-suspend
	suspendedsession=0;

	// abort all cursors
	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"aborting all busy cursors...");
	#endif
	for (int i=0; i<cfgfl->getCursors(); i++) {
		if (cur[i]->busy) {
			#ifdef SERVER_DEBUG
			debugPrint("connection",3,(long)i);
			#endif
			cur[i]->abort();
		}
	}
	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"done aborting all busy cursors");
	#endif

	// commit or rollback if necessary
	if (isTransactional() && commitorrollback) {
		if (cfgfl->getEndOfSessionCommit()) {
			#ifdef SERVER_DEBUG
			debugPrint("connection",2,"committing...");
			#endif
			commit();
			#ifdef SERVER_DEBUG
			debugPrint("connection",2,"done committing...");
			#endif
		} else {
			#ifdef SERVER_DEBUG
			debugPrint("connection",2,"rolling back...");
			#endif
			rollback();
			#ifdef SERVER_DEBUG
			debugPrint("connection",2,"done rolling back...");
			#endif
		}
	}

	// reset autocommit behavior
	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"resetting autocommit behavior...");
	#endif
	if (autocommit) {
		#ifdef SERVER_DEBUG
		debugPrint("connection",3,"setting autocommit on...");
		#endif
		autoCommitOn();
		#ifdef SERVER_DEBUG
		debugPrint("connection",3,"done setting autocommit on...");
		#endif
	} else {
		#ifdef SERVER_DEBUG
		debugPrint("connection",3,"setting autocommit off...");
		#endif
		autoCommitOff();
		#ifdef SERVER_DEBUG
		debugPrint("connection",3,"done setting autocommit off...");
		#endif
	}
	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"done resetting autocommit behavior...");
	debugPrint("connection",1,"done ending session");
	#endif
}

void	sqlrconnection::dropTempTables(stringlist *tablelist) {

	// run through the temp table list, dropping tables
	stringlistnode	*sln=tablelist->getNodeByIndex(0);
	while (sln) {
		dropTempTable(sln->getData());
		delete[] sln->getData();
		sln=(stringlistnode *)sln->getNext();
	}
	tablelist->clear();
}

void	sqlrconnection::dropTempTable(const char *tablename) {
	stringbuffer	dropquery;
	dropquery.append("drop table ")->append(tablename);
	sqlrcursor	*dropcur=initCursor();
	if (dropcur->openCursor(-1) &&
		dropcur->prepareQuery(dropquery.getString(),
					dropquery.getStringLength()) &&
		dropcur->executeQuery(dropquery.getString(),
					dropquery.getStringLength(),1)) {
		dropcur->cleanUpData();
	}
	dropcur->closeCursor();
	delete dropcur;
}

void	sqlrconnection::truncateTempTables(stringlist *tablelist) {

	// run through the temp table list, truncateing tables
	stringlistnode	*sln=tablelist->getNodeByIndex(0);
	while (sln) {
		truncateTempTable(sln->getData());
		delete[] sln->getData();
		sln=(stringlistnode *)sln->getNext();
	}
	tablelist->clear();
}

void	sqlrconnection::truncateTempTable(const char *tablename) {
	stringbuffer	truncatequery;
	truncatequery.append("delete from table ")->append(tablename);
	sqlrcursor	*truncatecur=initCursor();
	if (truncatecur->openCursor(-1) &&
		truncatecur->prepareQuery(truncatequery.getString(),
					truncatequery.getStringLength()) &&
		truncatecur->executeQuery(truncatequery.getString(),
					truncatequery.getStringLength(),1)) {
		truncatecur->cleanUpData();
	}
	truncatecur->closeCursor();
	delete truncatecur;
}

void	sqlrconnection::noAvailableCursors() {

	// indicate that an error has occurred
	clientsock->write((unsigned short)ERROR);

	// send the error itself
	clientsock->write((unsigned short)62);
	clientsock->write("No server-side cursors were available to process the query.",62);
}

int	sqlrconnection::handleError() {

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"handling error...");
	#endif

	// return the error unless the error was a dead connection, 
	// in which case, re-establish the connection
	if (!returnError()) {
		#ifdef SERVER_DEBUG
		debugPrint("connection",3,"database is down...");
		#endif
		reLogIn();
		return 0;
	}

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"done handling error...");
	#endif
	return 1;
}

void	sqlrconnection::reLogIn() {

	markDatabaseUnavailable();

	#ifdef SERVER_DEBUG
	debugPrint("connection",4,"relogging in...");
	#endif

	// attempt to log in over and over, once every 5 seconds
	closeCursors(0);
	logOut();
	for (;;) {
			
		#ifdef SERVER_DEBUG
		debugPrint("connection",5,"trying...");
		#endif

		if (logIn()) {
			if (!initCursors(0)) {
				closeCursors(0);
				logOut();
			} else {
				break;
			}
		}
		sleep(5);
	}

	#ifdef SERVER_DEBUG
	debugPrint("connection",4,"done relogging in");
	#endif

	markDatabaseAvailable();
}

void	sqlrconnection::markDatabaseUnavailable() {

	#ifdef SERVER_DEBUG
	char	*string=new char[10+strlen(updown)+1];
	sprintf(string,"unlinking %s",updown);
	getDebugLogger()->write("connection",4,string);
	delete[] string;
	#endif

	// the database is down if the file isn't there
	unlink(updown);
}

int	sqlrconnection::getCommand(unsigned short *command) {

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"getting command...");
	#endif

	// get the command
	if (clientsock->read(command)!=sizeof(unsigned short)) {
		#ifdef SERVER_DEBUG
		debugPrint("connection",2,
			"getting command failed: client sent bad command");
		#endif
		return 0;
	}

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"done getting command");
	#endif
	return 1;
}

int	sqlrconnection::getQuery() {

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"getting query...");
	#endif

	// get the length of the query
	if (clientsock->read(&cur[currentcur]->querylength)!=
					sizeof(unsigned long)) {
		#ifdef SERVER_DEBUG
		debugPrint("connection",2,
			"getting query failed: client sent bad query length size");
		#endif
		return 0;
	}

	// bounds checking
	if (cur[currentcur]->querylength>MAXQUERYSIZE) {
		#ifdef SERVER_DEBUG
		debugPrint("connection",2,
			"getting query failed: client sent bad query size");
		#endif
		return 0;
	}

	// read the query into the buffer
	if ((unsigned long)(clientsock->read(cur[currentcur]->querybuffer,
				cur[currentcur]->querylength))!=
					(unsigned long)(cur[currentcur]->
								querylength)) {
		#ifdef SERVER_DEBUG
		debugPrint("connection",2,
			"getting query failed: client sent short query");
		#endif
		return 0;
	}
	cur[currentcur]->querybuffer[cur[currentcur]->querylength]=(char)NULL;

	#ifdef SERVER_DEBUG
	debugPrint("connection",3,"querylength:");
	debugPrint("connection",4,(long)cur[currentcur]->querylength);
	debugPrint("connection",3,"query:");
	debugPrint("connection",0,cur[currentcur]->querybuffer);
	debugPrint("connection",2,"getting query succeeded");
	#endif

	return 1;
}

int	sqlrconnection::getInputBinds() {

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"getting input binds...");
	#endif

	// get the number of input bind variable/values
	if (!getBindVarCount(&(cur[currentcur]->inbindcount))) {
		return 0;
	}
	
	// fill the buffers
	for (int i=0; i<cur[currentcur]->inbindcount && i<MAXVAR; i++) {

		bindvar	*bv=&(cur[currentcur]->inbindvars[i]);

		// get the variable name and type
		if (!(getBindVarName(bv) && getBindVarType(bv))) {
			return 0;
		}

		// get the value
		if (bv->type==NULL_BIND) {
			getNullBind(bv);
		} else if (bv->type==STRING_BIND) {
			if (!getStringBind(bv)) {
				return 0;
			}
		} else if (bv->type==LONG_BIND) {
			if (!getLongBind(bv)) {
				return 0;
			}
		} else if (bv->type==DOUBLE_BIND) {
			if (!getDoubleBind(bv)) {
				return 0;
			}
		} else if (bv->type==BLOB_BIND || bv->type==CLOB_BIND) {
			if (!getLobBind(bv)) {
				return 0;
			}
		}		  
	}

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"done getting input binds");
	#endif
	return 1;
}

int	sqlrconnection::getOutputBinds() {

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"getting output binds...");
	#endif

	// get the number of output bind variable/values
	if (!getBindVarCount(&(cur[currentcur]->outbindcount))) {
		return 0;
	}

	// fill the buffers
	for (int i=0; i<cur[currentcur]->outbindcount && i<MAXVAR; i++) {

		bindvar	*bv=&(cur[currentcur]->outbindvars[i]);

		// get the variable name and type
		if (!(getBindVarName(bv) && getBindVarType(bv))) {
			return 0;
		}

		// get the size of the value
		if (bv->type==STRING_BIND) {
			if (!getBindSize(bv,STRINGBINDVALUELENGTH)) {
				return 0;
			}
			// This must be a calloc because oracle8 get's angry if
			// these aren't initialized to NULL's.  It's possible
			// that just the first character needs to be NULL, but
			// for now I'm just going to go ahead and use calloc
			bv->value.stringval=
				(char *)bindpool->calloc(bv->valuesize+1);
			#ifdef SERVER_DEBUG
			debugPrint("connection",4,"STRING");
			#endif
		} else if (bv->type==BLOB_BIND || bv->type==CLOB_BIND) {
			if (!getBindSize(bv,LOBBINDVALUELENGTH)) {
				return 0;
			}
			#ifdef SERVER_DEBUG
			if (bv->type==BLOB_BIND) {
				debugPrint("connection",4,"BLOB");
			} else if (bv->type==CLOB_BIND) {
				debugPrint("connection",4,"CLOB");
			}
			#endif
		} else if (bv->type==CURSOR_BIND) {
			#ifdef SERVER_DEBUG
			debugPrint("connection",4,"CURSOR");
			#endif
			short	curs=findAvailableCursor();
			if (curs==-1) {
				// FIXME: set error here
				return 0;
			}
			bv->value.cursorid=curs;
		}
	}

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"done getting output binds");
	#endif
	return 1;
}

void	sqlrconnection::returnOutputBindValues() {

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"returning output bind values");
	debugPrint("connection",3,(long)cur[currentcur]->outbindcount);
	#endif

	// run through the output bind values, sending them back
	for (int i=0; i<cur[currentcur]->outbindcount; i++) {

		bindvar	*bv=&(cur[currentcur]->outbindvars[i]);

		#ifdef SERVER_DEBUG
		debugstr=new stringbuffer();
		debugstr->append((long)i);
		debugstr->append(":");
		#endif

		if (bindValueIsNull(bv->isnull)) {

			#ifdef SERVER_DEBUG
			debugstr->append("NULL");
			#endif

			clientsock->write((unsigned short)NULL_DATA);

		} else if (bv->type==BLOB_BIND) {

			#ifdef SERVER_DEBUG
			debugstr->append("BLOB:\n");
			#endif

			cur[currentcur]->returnOutputBindBlob(i);

		} else if (bv->type==CLOB_BIND) {

			#ifdef SERVER_DEBUG
			debugstr->append("CLOB:\n");
			#endif

			cur[currentcur]->returnOutputBindClob(i);

		} else if (bv->type==STRING_BIND) {

			#ifdef SERVER_DEBUG
			debugstr->append("STRING:\n");
			debugstr->append(bv->value.stringval);
			#endif

			clientsock->write((unsigned short)NORMAL_DATA);
			bv->valuesize=strlen((char *)bv->value.stringval);
			clientsock->write(bv->valuesize);
			clientsock->write(bv->value.stringval,bv->valuesize);

		} else if (bv->type==CURSOR_BIND) {

			#ifdef SERVER_DEBUG
			debugstr->append("CURSOR:\n");
			debugstr->append((long)bv->value.cursorid);
			#endif

			clientsock->write((unsigned short)CURSOR_DATA);
			clientsock->write(bv->value.cursorid);
		}

		#ifdef SERVER_DEBUG
		debugPrint("connection",3,debugstr->getString());
		delete debugstr;
		#endif
	}

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"done returning output bind values");
	#endif
}





int	sqlrconnection::getBindVarCount(unsigned short *count) {

	// get the number of input bind variable/values
	if (clientsock->read(count)!=sizeof(unsigned short)) {
		#ifdef SERVER_DEBUG
		debugPrint("connection",2,
			"getting binds failed: client sent bad bind count size");
		#endif
		return 0;
	}

	// bounds checking
	if (*count>MAXVAR) {
		#ifdef SERVER_DEBUG
		debugPrint("connection",2,
			"getting binds failed: client tried to send too many binds");
		#endif
		return 0;
	}

	return 1;
}

int	sqlrconnection::getBindVarName(bindvar *bv) {

	unsigned short	bindnamesize;

	// get the variable name size
	if (clientsock->read(&bindnamesize)!=sizeof(unsigned short)) {
		#ifdef SERVER_DEBUG
		debugPrint("connection",2,
			"getting binds failed: bad variable name length size");
		#endif
		return 0;
	}

	// bounds checking
	if (bindnamesize>BINDVARLENGTH) {
		#ifdef SERVER_DEBUG
		debugPrint("connection",2,
			"getting binds failed: bad variable name length");
		#endif
		return 0;
	}

	// get the variable name
	bv->variablesize=bindnamesize+1;
	bv->variable=(char *)bindpool->malloc(bv->variablesize+2);
	bv->variable[0]=bindVariablePrefix();
	if (clientsock->read(bv->variable+1,bindnamesize)!=bindnamesize) {
		#ifdef SERVER_DEBUG
		debugPrint("connection",2,
			"getting binds failed: bad variable name");
		#endif
		return 0;
	}
	bv->variable[bindnamesize+1]=(char)NULL;

	#ifdef SERVER_DEBUG
	debugPrint("connection",4,bv->variable);
	#endif

	return 1;
}

int	sqlrconnection::getBindVarType(bindvar *bv) {

	// get the type
        unsigned short type;
	if (clientsock->read((unsigned short *)&type)!=sizeof(unsigned short)) {
		#ifdef SERVER_DEBUG
		debugPrint("connection",2,
				"getting binds failed: bad type size");
		#endif
		return 0;
	}
	bv->type=(bindtype)type;
	
	return 1;
}

void	sqlrconnection::getNullBind(bindvar *bv) {

	#ifdef SERVER_DEBUG
		debugPrint("connection",4,"NULL");
	#endif

	bv->value.stringval=(char *)bindpool->malloc(1);
	bv->value.stringval[0]=(char)NULL;
	bv->valuesize=0;
	bv->isnull=nullBindValue();
}

int	sqlrconnection::getBindSize(bindvar *bv, unsigned long maxsize) {

	// get the size of the value
	if (clientsock->read(&(bv->valuesize))!=sizeof(unsigned long)) {
		#ifdef SERVER_DEBUG
		debugPrint("connection",
			2,"getting binds failed: bad value length size");
		#endif
		return 0;
	}

	// bounds checking
	if (bv->valuesize>maxsize) {
		#ifdef SERVER_DEBUG
		debugPrint("connection",2,
				"getting binds failed: bad value length");
		debugPrint("connection",3,(long)bv->valuesize);
		#endif
		return 0;
	}

	return 1;
}

int	sqlrconnection::getStringBind(bindvar *bv) {

	// get the size of the value
	if (!getBindSize(bv,STRINGBINDVALUELENGTH)) {
		return 0;
	}

	// allocate space to store the value
	bv->value.stringval=(char *)bindpool->malloc(bv->valuesize+1);

	#ifdef SERVER_DEBUG
		debugPrint("connection",4,"STRING");
	#endif

	// get the bind value
	if ((unsigned long)(clientsock->read(bv->value.stringval,
			bv->valuesize))!=(unsigned long)(bv->valuesize)) {
		#ifdef SERVER_DEBUG
		debugPrint("connection",2,"getting binds failed: bad value");
		#endif
		return 0;
	}
	bv->value.stringval[bv->valuesize]=(char)NULL;
	bv->isnull=nonNullBindValue();

	#ifdef SERVER_DEBUG
		debugPrint("connection",4,bv->value.stringval);
	#endif

	return 1;
}

int	sqlrconnection::getLongBind(bindvar *bv) {

	#ifdef SERVER_DEBUG
		debugPrint("connection",4,"LONG");
	#endif

	// get positive/negative
	char		negative;
	if (clientsock->read(&negative)!=sizeof(char)) {
		#ifdef SERVER_DEBUG
			debugPrint("connection",2,
				"getting binds failed: bad positive/negative");
		#endif
		return 0;
	}

	// get the value itself
	unsigned long	value;
	if (clientsock->read(&value)!=sizeof(unsigned long)) {
		#ifdef SERVER_DEBUG
			debugPrint("connection",2,
					"getting binds failed: bad value");
		#endif
		return 0;
	}

	// set the value
	bv->value.longval=((long)value)*(negative?-1:1);

	#ifdef SERVER_DEBUG
		debugPrint("connection",4,bv->value.longval);
	#endif

	return 1;
}

int	sqlrconnection::getDoubleBind(bindvar *bv) {

	#ifdef SERVER_DEBUG
		debugPrint("connection",4,"DOUBLE");
	#endif

	// get the value
	if (clientsock->read(&(bv->value.doubleval.value))!=sizeof(double)) {
		#ifdef SERVER_DEBUG
			debugPrint("connection",2,
					"getting binds failed: bad value");
		#endif
		return 0;
	}

	// get the precision
	if (clientsock->read(&(bv->value.doubleval.precision))!=
						sizeof(unsigned short)) {
		#ifdef SERVER_DEBUG
			debugPrint("connection",2,
					"getting binds failed: bad precision");
		#endif
		return 0;
	}

	// get the scale
	if (clientsock->read(&(bv->value.doubleval.scale))!=
						sizeof(unsigned short)) {
		#ifdef SERVER_DEBUG
			debugPrint("connection",2,
					"getting binds failed: bad scale");
		#endif
		return 0;
	}

	#ifdef SERVER_DEBUG
		debugPrint("connection",4,bv->value.doubleval.value);
	#endif

	return 1;
}

int	sqlrconnection::getLobBind(bindvar *bv) {

	#ifdef SERVER_DEBUG
		if (bv->type==BLOB_BIND) {
			debugPrint("connection",4,"BLOB");
		}
		if (bv->type==CLOB_BIND) {
			debugPrint("connection",4,"CLOB");
		}
	#endif

	// get the size of the value
	if (!getBindSize(bv,LOBBINDVALUELENGTH)) {
		return 0;
	}

	// allocate space to store the value
	bv->value.stringval=(char *)bindpool->malloc(bv->valuesize+1);

	// get the bind value
	if ((unsigned long)(clientsock->read(bv->value.stringval,
			bv->valuesize))!=(unsigned long)(bv->valuesize)) {
		#ifdef SERVER_DEBUG
		debugPrint("connection",2,
				"getting binds failed: bad value");
		#endif
		return 0;
	}

	// It shouldn't hurt to NULL-terminate the lob because the actual size
	// (which doesn't include the NULL terminator) should be used when
	// binding.
	bv->value.stringval[bv->valuesize]=(char)NULL;
	bv->isnull=nonNullBindValue();

	#ifdef SERVER_DEBUG
		if (bv->type==BLOB_BIND) {
			debugPrintBlob(bv->value.stringval,bv->valuesize);
		}
		if (bv->type==CLOB_BIND) {
			debugPrintClob(bv->value.stringval,bv->valuesize);
		}
	#endif

	return 1;
}

int	sqlrconnection::getSendColumnInfo() {

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"getting send column info...");
	#endif

	if (clientsock->read(&sendcolumninfo)!=sizeof(unsigned short)) {
		#ifdef SERVER_DEBUG
		debugPrint("connection",2,"getting send column info failed");
		#endif
		return 0;
	}

	#ifdef SERVER_DEBUG
	if (sendcolumninfo==SEND_COLUMN_INFO) {
		debugPrint("connection",3,"send column info");
	} else {
		debugPrint("connection",3,"don't send column info");
	}
	debugPrint("connection",2,"done getting send column info...");
	#endif

	return 1;
}

int	sqlrconnection::sendColumnInfo() {
	if (sendcolumninfo==SEND_COLUMN_INFO) {
		return 1;
	}
	return 0;
}

int	sqlrconnection::processQuery(int reexecute, int reallyexecute) {

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"processing query...");
	#endif

	// if the reexecute flag is set, the query doesn't need to be prepared 
	// again.
	int	success=0;
	if (reexecute) {
		#ifdef SERVER_DEBUG
		debugPrint("connection",3,"re-executing...");
		#endif
		success=cur[currentcur]->handleBinds() && 
			cur[currentcur]->executeQuery(
					cur[currentcur]->querybuffer,
					cur[currentcur]->querylength,
					reallyexecute);
	} else {
		#ifdef SERVER_DEBUG
		debugPrint("connection",3,"preparing/executing...");
		#endif
		success=cur[currentcur]->prepareQuery(
					cur[currentcur]->querybuffer,
					cur[currentcur]->querylength) && 
			cur[currentcur]->handleBinds() && 
			cur[currentcur]->executeQuery(
					cur[currentcur]->querybuffer,
					cur[currentcur]->querylength,1);
	}

	// was the query a commit or rollback?
	commitOrRollback();

	// On success, autocommit if necessary.
	// Connection classes could override autoCommitOn() and autoCommitOff()
	// to do database API-specific things, but will not set 
	// checkautocommit, so this code won't get called at all for those 
	// connections.
	if (success && checkautocommit && isTransactional() && 
			performautocommit && commitorrollback) {
		#ifdef SERVER_DEBUG
		debugPrint("connection",3,"commit necessary...");
		#endif
		success=commit();
		commitorrollback=0;
	}

	#ifdef SERVER_DEBUG
	if (success) {
		debugPrint("connection",2,"processing query succeeded");
	} else {
		debugPrint("connection",2,"processing query failed");
	}
	debugPrint("connection",2,"done processing query");
	#endif

	return success;
}

short	sqlrconnection::nonNullBindValue() {
	return 0;
}

short	sqlrconnection::nullBindValue() {
	return -1;
}

char	sqlrconnection::bindVariablePrefix() {
	return ':';
}

int	sqlrconnection::bindValueIsNull(short isnull) {
	return abs(isnull);
}

int	sqlrconnection::returnError() {

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"returning error...");
	#endif

	// get the error message from the database
	// return value: 1 if database connection is still alive, 0 if not
	int	liveconnection;
	char	*error=cur[currentcur]->getErrorMessage(&liveconnection);

	// only return an error message if the error wasn't a dead database
	if (liveconnection) {

		// indicate that an error has occurred
		clientsock->write((unsigned short)ERROR);

		// send the error itself
		int	errorlen=strlen(error);
		clientsock->write((unsigned short)(errorlen+
				strlen(cur[currentcur]->querybuffer)+18));
		clientsock->write(error,errorlen);

		// send the attempted query back too
		clientsock->write("\nAttempted Query:\n");
		clientsock->write(cur[currentcur]->querybuffer);
	}
	
	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"done returning error");
	#endif

	return liveconnection;
}

void	sqlrconnection::returnResultSetHeader() {

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"returning result set header...");
	#endif


	// return the row counts
	#ifdef SERVER_DEBUG
	debugPrint("connection",3,"returning row counts...");
	#endif
	cur[currentcur]->returnRowCounts();
	#ifdef SERVER_DEBUG
	debugPrint("connection",3,"done returning row counts");
	#endif


	// write a flag to the client indicating whether 
	// or not the column information will be sent
	clientsock->write((unsigned short)sendcolumninfo);

	#ifdef SERVER_DEBUG
	if (sendcolumninfo==SEND_COLUMN_INFO) {
		debugPrint("connection",3,"column info will be sent");
	} else {
		debugPrint("connection",3,"column info will not be sent");
	}
	#endif


	// return the column count
	#ifdef SERVER_DEBUG
	debugPrint("connection",3,"returning column counts...");
	#endif
	cur[currentcur]->returnColumnCount();
	#ifdef SERVER_DEBUG
	debugPrint("connection",3,"done returning column counts");
	#endif


	if (sendcolumninfo==SEND_COLUMN_INFO) {
		// return the column info
		#ifdef SERVER_DEBUG
		debugPrint("connection",3,"returning column info...");
		#endif
		cur[currentcur]->returnColumnInfo();
		#ifdef SERVER_DEBUG
		debugPrint("connection",3,"done returning column info");
		#endif
	}


	// return the output bind vars
	returnOutputBindValues();


	// terminate the bind vars
	clientsock->write((unsigned short)END_BIND_VARS);


	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"done returning result set header");
	#endif
}

int	sqlrconnection::returnResultSetData() {

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"returning result set data...");
	#endif

	// see if this result set even has any rows to return
	int	norows=cur[currentcur]->noRowsToReturn();

	// get the number of rows to skip
	unsigned long	skip;
	if (clientsock->read(&skip)!=sizeof(unsigned long)) {
		#ifdef SERVER_DEBUG
		debugPrint("connection",2,"returning result set data failed");
		#endif
		return 0;
	}

	// get the number of rows to fetch
	unsigned long	fetch;
	if (clientsock->read(&fetch)!=sizeof(unsigned long)) {
		#ifdef SERVER_DEBUG
		debugPrint("connection",2,"returning result set data failed");
		#endif
		return 0;
	}


	// for some queries, there are no rows to return, 
	if (norows) {
		clientsock->write((unsigned short)END_RESULT_SET);
		cur[currentcur]->abort();
		#ifdef SERVER_DEBUG
		debugPrint("connection",2,"done returning result set data");
		#endif
		return 1;
	}


	// reinit suspendresultset
	cur[currentcur]->suspendresultset=0;


	// skip the specified number of rows
	if (!skipRows(skip)) {
		clientsock->write((unsigned short)END_RESULT_SET);
		cur[currentcur]->abort();
		#ifdef SERVER_DEBUG
		debugPrint("connection",2,"done returning result set data");
		#endif
		return 1;
	}


	// send the specified number of rows back
	for (unsigned long i=0; (!fetch || i<fetch); i++) {

		if (!cur[currentcur]->fetchRow()) {
			clientsock->write((unsigned short)END_RESULT_SET);
			cur[currentcur]->abort();
			#ifdef SERVER_DEBUG
			debugPrint("connection",2,
					"done returning result set data");
			#endif
			return 1;
		}

		#ifdef SERVER_DEBUG
			debugstr=new stringbuffer();
		#endif
		cur[currentcur]->returnRow();
		#ifdef SERVER_DEBUG
			debugPrint("connection",3,debugstr->getString());
			delete debugstr;
		#endif

		lastrow++;
	}

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"done returning result set data");
	#endif
	return 1;
}

int	sqlrconnection::skipRows(int rows) {

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"skipping rows...");
	#endif

	for (int i=0; i<rows; i++) {

		#ifdef SERVER_DEBUG
		debugPrint("connection",3,"skip...");
		#endif

		lastrow++;
		if (!cur[currentcur]->skipRow()) {
			#ifdef SERVER_DEBUG
			debugPrint("connection",2,
				"skipping rows hit the end of the result set");
			#endif
			return 0;
		}
	}

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"done skipping rows");
	#endif
	return 1;
}

void	sqlrconnection::sendRowCounts(long actual, long affected) {

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"sending row counts...");
	#endif


	// send actual rows, if that is known
	if (actual>-1) {

		#ifdef SERVER_DEBUG
		char	string[30];
		sprintf(string,"actual rows: %ld",actual);
		debugPrint("connection",3,string);
		#endif

		clientsock->write((unsigned short)ACTUAL_ROWS);
		clientsock->write((unsigned long)actual);
	} else {

		#ifdef SERVER_DEBUG
		debugPrint("connection",3,"actual rows unknown");
		#endif

		clientsock->write((unsigned short)NO_ACTUAL_ROWS);
	}

	
	// send affected rows, if that is known
	if (affected>-1) {

		#ifdef SERVER_DEBUG
		char	string[46];
		sprintf(string,"affected rows: %ld",affected);
		debugPrint("connection",3,string);
		#endif

		clientsock->write((unsigned short)AFFECTED_ROWS);
		clientsock->write((unsigned long)affected);

	} else {

		#ifdef SERVER_DEBUG
		debugPrint("connection",3,"affected rows unknown");
		#endif

		clientsock->write((unsigned short)NO_AFFECTED_ROWS);
	}

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"done sending row counts");
	#endif
}

void	sqlrconnection::sendColumnCount(unsigned long ncols) {

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"sending column count...");
	#endif

	#ifdef SERVER_DEBUG
	debugPrint("connection",3,(long)ncols);
	#endif
	clientsock->write(ncols);

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"done sending column count");
	#endif
}

void	sqlrconnection::sendColumnTypeFormat(unsigned short format) {

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"sending column type format...");
	#endif

	#ifdef SERVER_DEBUG
	if (format==COLUMN_TYPE_IDS) {
		debugPrint("connection",3,"id's");
	} else {
		debugPrint("connection",3,"names");
	}
	#endif

	clientsock->write(format);

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"done sending column type format");
	#endif
}

void	sqlrconnection::sendColumnDefinition(const char *name,
						unsigned short namelen,
						unsigned short type, 
						unsigned long size,
						unsigned long precision,
						unsigned long scale) {

	#ifdef SERVER_DEBUG
	debugstr=new stringbuffer();
	for (int i=0; i<namelen; i++) {
		debugstr->append(name[i]);
	}
	debugstr->append(":");
	debugstr->append((long)type);
	debugstr->append(":");
	debugstr->append((long)size);
	debugstr->append(" (");
	debugstr->append((long)precision);
	debugstr->append(",");
	debugstr->append((long)scale);
	debugstr->append(")");
	debugPrint("connection",3,debugstr->getString());
	delete debugstr;
	#endif

	clientsock->write(namelen);
	clientsock->write(name,namelen);
	clientsock->write(type);
	clientsock->write(size);
	clientsock->write(precision);
	clientsock->write(scale);
}

void	sqlrconnection::sendColumnDefinitionString(const char *name,
						unsigned short namelen,
						const char *type, 
						unsigned short typelen,
						unsigned long size,
						unsigned long precision,
						unsigned long scale) {

	#ifdef SERVER_DEBUG
	debugstr=new stringbuffer();
	for (int i=0; i<namelen; i++) {
		debugstr->append(name[i]);
	}
	debugstr->append(":");
	for (int i=0; i<typelen; i++) {
		debugstr->append(type[i]);
	}
	debugstr->append(":");
	debugstr->append((long)size);
	debugstr->append(" (");
	debugstr->append((long)precision);
	debugstr->append(",");
	debugstr->append((long)scale);
	debugstr->append(")");
	debugPrint("connection",3,debugstr->getString());
	delete debugstr;
	#endif

	clientsock->write(namelen);
	clientsock->write(name,namelen);
	clientsock->write(typelen);
	clientsock->write(type,typelen);
	clientsock->write(size);
	clientsock->write(precision);
	clientsock->write(scale);
}

void	sqlrconnection::sendField(const char *data, unsigned long size) {

	#ifdef SERVER_DEBUG
	debugstr->append("\"");
	for (unsigned long i=0; i<size; i++) {
		debugstr->append(data[i]);
	}
	debugstr->append("\",");
	#endif

	clientsock->write((unsigned short)NORMAL_DATA);
	clientsock->write(size);
	clientsock->write(data,size);
}

void	sqlrconnection::sendNullField() {

	#ifdef SERVER_DEBUG
	debugstr->append("NULL");
	#endif

	clientsock->write((unsigned short)NULL_DATA);
}

void	sqlrconnection::startSendingLong() {
	clientsock->write((unsigned short)START_LONG_DATA);
}

void	sqlrconnection::sendLongSegment(const char *data, unsigned long size) {

	#ifdef SERVER_DEBUG
	for (unsigned long i=0; i<size; i++) {
		debugstr->append(data[i]);
	}
	#endif

	clientsock->write((unsigned short)NORMAL_DATA);
	clientsock->write(size);
	clientsock->write(data,size);
}

void	sqlrconnection::endSendingLong() {

	#ifdef SERVER_DEBUG
	debugstr->append(",");
	#endif

	clientsock->write((unsigned short)END_LONG_DATA);
}

int	sqlrconnection::isTransactional() {
	return 1;
}

void	sqlrconnection::initSession() {

	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"initializing session...");
	#endif

	commitorrollback=0;
	suspendedsession=0;
	for (int i=0; i<cfgfl->getCursors(); i++) {
		cur[i]->suspendresultset=0;
	}
	accepttimeout=5;

	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"done initializing session...");
	#endif
}

void	sqlrconnection::commitOrRollback() {

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"commit or rollback check...");
	#endif

	// if the query was a commit or rollback, set a flag indicating so
	if (isTransactional()) {
		if (cur[currentcur]->queryIsCommitOrRollback()) {
			#ifdef SERVER_DEBUG
			debugPrint("connection",3,
					"commit or rollback not needed");
			#endif
			commitorrollback=0;
		} else if (cur[currentcur]->queryIsNotSelect()) {
			#ifdef SERVER_DEBUG
			debugPrint("connection",3,
					"commit or rollback needed");
			#endif
			commitorrollback=1;
		}
	}

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"done with commit or rollback check");
	#endif
}

void	sqlrconnection::setAutoCommitBehavior(int ac) {
	autocommit=ac;
}

int	sqlrconnection::getAutoCommitBehavior() {
	return autocommit;
}

unsigned short	sqlrconnection::autoCommitOn() {
	checkautocommit=1;
	performautocommit=1;
	return 1;
}

unsigned short	sqlrconnection::autoCommitOff() {
	checkautocommit=1;
	performautocommit=0;
	return 1;
}

int	sqlrconnection::commit() {

	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"commit...");
	#endif

	cur[currentcur]->prepareQuery("commit",6);
	int	retval=cur[currentcur]->executeQuery("commit",6,1);
	cur[currentcur]->abort();

	#ifdef SERVER_DEBUG
	char	string[36];
	sprintf(string,"commit result: %d",retval);
	debugPrint("connection",2,string);
	#endif

	return retval;
}

int	sqlrconnection::rollback() {

	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"rollback...");
	#endif

	cur[currentcur]->prepareQuery("rollback",8);
	int	retval=cur[currentcur]->executeQuery("rollback",8,1);
	cur[currentcur]->abort();

	#ifdef SERVER_DEBUG
	char	string[38];
	sprintf(string,"rollback result: %d",retval);
	debugPrint("connection",2,string);
	#endif

	return retval;
}

char	*sqlrconnection::pingQuery() {
	return "select 1";
}

int	sqlrconnection::ping() {
	sqlrcursor	*pingcur=initCursor();
	char	*pingquery=pingQuery();
	int	pingquerylen=strlen(pingQuery());
	if (pingcur->openCursor(-1) &&
		pingcur->prepareQuery(pingquery,pingquerylen) &&
		pingcur->executeQuery(pingquery,pingquerylen,1)) {
		pingcur->cleanUpData();
		pingcur->closeCursor();
		delete pingcur;
		return 1;
	}
	pingcur->closeCursor();
	delete pingcur;
	return 0;
}

void	sqlrconnection::setUser(const char *user) {
	this->user=(char *)user;
}

void	sqlrconnection::setPassword(const char *password) {
	this->password=(char *)password;
}

char	*sqlrconnection::getUser() {
	return user;
}

char	*sqlrconnection::getPassword() {
	return password;
}

void	sqlrconnection::addSessionTempTable(const char *table) {
	sessiontemptables.append(strdup(table));
}

void	sqlrconnection::addTransactionTempTable(const char *table) {
	transtemptables.append(strdup(table));
}
