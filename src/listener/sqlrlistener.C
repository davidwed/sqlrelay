// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <config.h>
#include <defaults.h>
#include <defines.h>

#include <sqlrlistener.h>

#include <rudiments/permissions.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_UNISTD_H
	#include <unistd.h>
#endif
#include <errno.h>

sqlrlistener::sqlrlistener() : daemonprocess(), listener(), debugfile() {

	cmdl=NULL;

	init=0;

	semset=NULL;
	idmemory=NULL;

	pidfile=NULL;

	authc=NULL;

	inetport=0;
	unixport=NULL;

	clientsockin=NULL;
	clientsockun=NULL;
	handoffsockun=NULL;
	removehandoffsockun=NULL;

	handoffsocklist=NULL;

	denied=NULL;
	allowed=NULL;
}

sqlrlistener::~sqlrlistener() {
	if (unixport) {
		unlink(unixport);
	}
	if (pidfile) {
		unlink(pidfile);
	}
	if (init) {
		cleanUp();
	}
	delete cmdl;
}

void	sqlrlistener::cleanUp() {

	delete authc;
	delete[] pidfile;
	delete semset;
	delete idmemory;
	delete[] unixport;
	delete clientsockun;
	delete clientsockin;
	delete handoffsockun;

	if (handoffsocklist) {
		for (int i=0; i<maxconnections; i++) {
			delete handoffsocklist[i]->sock;
			delete handoffsocklist[i];
		}
		delete[] handoffsocklist;
	}

	delete removehandoffsockun;
	delete denied;
	delete allowed;

	#ifdef SERVER_DEBUG
	closeDebugFile();
	#endif
}

int	sqlrlistener::initListener(int argc, const char **argv) {

	init=1;

	cmdl=new cmdline(argc,argv);

	tempdir		*tmpdir=new tempdir(cmdl);
	sqlrconfigfile	*cfgfl=new sqlrconfigfile();

	if (!cfgfl->parse(cmdl->getConfig(),cmdl->getId())) {
		delete tmpdir;
		delete cfgfl;
		return 0;
	}

	setUserAndGroup(cfgfl);

	#ifdef SERVER_DEBUG
	openDebugFile("listener",cmdl->getLocalStateDir());
	#endif

	if (!handlePidFile(tmpdir,cmdl->getId())) {
		delete tmpdir;
		delete cfgfl;
		return 0;
	}

	handleDynamicScaling(cfgfl);

	setHandoffMethod(cfgfl);

	if (cfgfl->getAuthOnListener()) {
		authc=new authenticator(cfgfl);
	}

	setIpPermissions(cfgfl);

	if (!createSharedMemoryAndSemaphores(tmpdir,cmdl->getId())) {
		delete tmpdir;
		delete cfgfl;
		return 0;
	}

	if (!listenOnClientSockets(cfgfl)) {
		delete tmpdir;
		delete cfgfl;
		return 0;
	}

	if (passdescriptor) {
		if (!listenOnHandoffSocket(tmpdir,cmdl->getId())) {
			delete tmpdir;
			delete cfgfl;
			return 0;
		}
		if (!listenOnDeregistrationSocket(tmpdir,cmdl->getId())) {
			delete tmpdir;
			delete cfgfl;
			return 0;
		}
	}

	#ifndef SERVER_DEBUG
	detach();
	#endif

	delete tmpdir;
	delete cfgfl;
	return 1;
}

void	sqlrlistener::setUserAndGroup(sqlrconfigfile *cfgfl) {
	runAsUser(cfgfl->getRunAsUser());
	runAsGroup(cfgfl->getRunAsGroup());
}

int	sqlrlistener::handlePidFile(tempdir *tmpdir, const char *id) {

	// check/set pid file
	pidfile=new char[tmpdir->getLength()+15+strlen(id)+1];
	sprintf(pidfile,"%s/sqlr-listener-%s",tmpdir->getString(),id);

	if (checkForPidFile(pidfile)!=-1) {
		fprintf(stderr,"\nsqlr-listener error:\n");
		fprintf(stderr,"	The pid file %s",tmpdir->getString());
		fprintf(stderr,"/sqlr-listener-%s",id);
		fprintf(stderr," exists.\n");
		fprintf(stderr,"	This usually means that the ");
		fprintf(stderr,"sqlr-listener is already running for ");
		fprintf(stderr,"the \n");
		fprintf(stderr,"	%s");
		fprintf(stderr," instance.\n");
		fprintf(stderr,"	If it is not running, please remove ");
		fprintf(stderr,"the file and restart.\n");
		delete[] pidfile;
		pidfile=NULL;
		return 0;
	}

	createPidFile(pidfile,permissions::ownerReadWrite());
	return 1;
}

void	sqlrlistener::handleDynamicScaling(sqlrconfigfile *cfgfl) {

	// get the dynamic connection scaling parameters
	maxconnections=cfgfl->getMaxConnections();

	// if dynamic scaling isn't going to be used, disable it
	dynamicscaling=0;
	if (cfgfl->getMaxQueueLength()>=0 &&
			maxconnections>cfgfl->getConnections()>0 &&
			cfgfl->getGrowBy()>0 &&
			cfgfl->getTtl()>0) {
		dynamicscaling=1;
	}
}

void	sqlrlistener::setHandoffMethod(sqlrconfigfile *cfgfl) {

	// get handoff method
	char	*handoffmethod=cfgfl->getHandOff();
	passdescriptor=0;
	if (!strcmp(handoffmethod,"pass")) {
		passdescriptor=1;

		// create the list of handoff nodes
		handoffsocklist=new handoffsocketnode *[maxconnections];
		for (int i=0; i<maxconnections; i++) {
			handoffsocklist[i]=new handoffsocketnode;
			handoffsocklist[i]->pid=0;
		}
	}
}

void	sqlrlistener::setIpPermissions(sqlrconfigfile *cfgfl) {

	// get denied and allowed ip's and compile the expressions
	char	*deniedips=cfgfl->getDeniedIps();
	char	*allowedips=cfgfl->getAllowedIps();
	if (deniedips[0]) {
		denied=new regularexpression(deniedips);
	}
	if (allowedips[0]) {
		allowed=new regularexpression(allowedips);
	}
}

int	sqlrlistener::createSharedMemoryAndSemaphores(tempdir *tmpdir,
							const char *id) {

	// initialize the ipc filename
	char	*idfilename=new char[tmpdir->getLength()+1+strlen(id)+1];
	sprintf(idfilename,"%s/%s",tmpdir->getString(),id);

	#ifdef SERVER_DEBUG
	debugPrint(logger::logHeader("connection"),0,
				"creating shared memory and semaphores");
	debugPrint(logger::logHeader("connection"),0,"id filename: ");
	debugPrint(logger::logHeader("connection"),0,idfilename);
	#endif

	// make sure that the file exists and is read/writeable
	int	idfd=open(idfilename,O_CREAT|O_RDWR,
					permissions::ownerReadWrite());
	if (idfd==-1) {
		ipcFileError(idfilename);
		delete[] idfilename;
		return 0;
	} else {
		close(idfd);
	}


	// get the ipc key
	key_t	key=ftok(idfilename,0);
	if (key==-1) {
		ftokError(idfilename);
		delete[] idfilename;
		return 0;
	}

	// create the shared memory segment
	// FIXME: if it already exists, attempt to remove and re-create it
	#ifdef SERVER_DEBUG
	debugPrint(logger::logHeader("connection"),1,
					"creating shared memory...");
	#endif
	idmemory=new sharedmemory();
	if (!idmemory->create(key,sizeof(unsigned int)+1024+
				(2*sizeof(unsigned short)),
				permissions::ownerReadWrite())) {

		idmemory->attach(key);
		shmError(id,idmemory->getId());
		delete[] idfilename;
		delete idmemory;
		return 0;
	}

	// create (or connect) to the semaphore set
	// FIXME: if it already exists, attempt to remove and re-create it
	#ifdef SERVER_DEBUG
	debugPrint(logger::logHeader("connection"),1,
					"creating semaphores...");
	#endif
	int	vals[10]={1,1,0,0,1,1,0,0,0,1};
	semset=new semaphoreset();
	if (!semset->create(key,permissions::ownerReadWrite(),10,vals)) {

		semset->attach(key,10);
		semError(id,semset->getId());
		delete[] idfilename;
		delete idmemory;
		delete semset;
		return 0;
	}

	return 1;
}

void	sqlrlistener::ipcFileError(const char *idfilename) {
	fprintf(stderr,"Could not open: %s\n",idfilename);
	fprintf(stderr,"Make sure that the file and directory are ");
	fprintf(stderr,"readable and writable.\n\n");
}

void	sqlrlistener::ftokError(const char *idfilename) {
	fprintf(stderr,"\nsqlr-listener error:\n");
	fprintf(stderr,"	Unable to generate a key from ");
	fprintf(stderr,"%s\n",idfilename);
	fprintf(stderr,"	Error was: %s\n\n",strerror(errno));
}

void	sqlrlistener::shmError(const char *id, int shmid) {
	fprintf(stderr,"\nsqlr-listener error:\n");
	fprintf(stderr,"	Unable to create a shared memory ");
	fprintf(stderr,"segment.  This is usally because an \n");
	fprintf(stderr,"	sqlr-listener is already running for ");
	fprintf(stderr,"the %s instance.\n\n",id);
	fprintf(stderr,"	If it is not running, something may ");
	fprintf(stderr,"have crashed and left an old segment\n");
	fprintf(stderr,"	lying around.  Use the ipcs command ");
	fprintf(stderr,"to inspect existing shared memory \n");
	fprintf(stderr,"	segments and the ipcrm command to ");
	fprintf(stderr,"remove the shared memory segment with ");
	fprintf(stderr,"\n	id %d\n\n",shmid);
}

void	sqlrlistener::semError(const char *id, int semid) {
	fprintf(stderr,"\nsqlr-listener error:\n");
	fprintf(stderr,"	Unable to create a semaphore ");
	fprintf(stderr,"set segment.  This is usally because ");
	fprintf(stderr,"an \n");
	fprintf(stderr,"	sqlr-listener is already ");
	fprintf(stderr,"running for the %s",id);
	fprintf(stderr," instance.\n\n");
	fprintf(stderr,"	If it is not running, ");
	fprintf(stderr,"something may have crashed and left ");
	fprintf(stderr,"an old segment\n");
	fprintf(stderr,"	lying around.  Use the ipcs ");
	fprintf(stderr,"command to inspect existing ");
	fprintf(stderr,"semaphore set \n");
	fprintf(stderr,"	segments and the ipcrm ");
	fprintf(stderr,"command to remove the semaphore set ");
	fprintf(stderr,"segment with \n");
	fprintf(stderr,"	id %d",semid);
	fprintf(stderr,".\n\n");
}

int	sqlrlistener::listenOnClientSockets(sqlrconfigfile *cfgfl) {

	// get inet and unix ports to listen on
	unsigned short	port=cfgfl->getPort();
	char		*uport=cfgfl->getUnixPort();
	if (uport && uport[0]) {
		unixport=strdup(uport);
	}

	// if neither port nor unixport are specified, set up to 
	// listen on the default inet port
	if (!port && !unixport) {
		port=atoi(DEFAULT_PORT);
		fprintf(stderr,"Warning! using default port.\n");
	}

	// attempt to listen on the inet port, if necessary
	int	listening=0;
	if (port) {
		clientsockin=new inetserversocket();
		clientsockin->reuseAddresses();
		listening=clientsockin->listenOnSocket(NULL,port,15);
		if (listening) {
			addFileDescriptor(clientsockin->getFileDescriptor());
		} else {
			fprintf(stderr,"Could not listen on inet port: ");
			fprintf(stderr,"%d\n",port);
			fprintf(stderr,"Make sure that no other ");
			fprintf(stderr,"processes are ");
			fprintf(stderr,"listening on that port.");
			fprintf(stderr,"\n\n");
			delete clientsockin;
			clientsockin=NULL;
		}
	}

	if (listening && unixport) {
		clientsockun=new unixserversocket();
		clientsockun->reuseAddresses();
		listening=clientsockun->listenOnSocket(unixport,0000,15);
		if (listening) {
			addFileDescriptor(clientsockun->getFileDescriptor());
		} else {
			fprintf(stderr,"Could not listen on unix socket: ");
			fprintf(stderr,"%s\n",unixport);
			fprintf(stderr,"Make sure that the file and ");
			fprintf(stderr,"directory are readable and writable.");
			fprintf(stderr,"\n\n");
			delete clientsockun;
			clientsockun=NULL;
			delete[] unixport;
			unixport=NULL;
		}
	}

	return listening;
}

int	sqlrlistener::listenOnHandoffSocket(tempdir *tmpdir, const char *id) {

	// the handoff socket
	char	*handoffsockname=new char[tmpdir->getLength()+1+strlen(id)+8+1];
	sprintf(handoffsockname,"%s/%s-handoff",tmpdir->getString(),id);

	handoffsockun=new unixserversocket();
	int	success=handoffsockun->listenOnSocket(handoffsockname,0066,15);

	if (success) {
		addFileDescriptor(handoffsockun->getFileDescriptor());
	} else {
		fprintf(stderr,"Could not listen on unix socket: ");
		fprintf(stderr,"%s\n",handoffsockname);
		fprintf(stderr,"Make sure that the file and ");
		fprintf(stderr,"directory are readable and writable.");
		fprintf(stderr,"\n\n");
	}
	delete[] handoffsockname;

	return success;
}

int	sqlrlistener::listenOnDeregistrationSocket(tempdir *tmpdir,
							const char *id) {

	// the deregistration socket
	char	*removehandoffsockname=new char[tmpdir->getLength()+1+
							strlen(id)+14+1];
	sprintf(removehandoffsockname,"%s/%s-removehandoff",
						tmpdir->getString(),id);

	removehandoffsockun=new unixserversocket();
	int	success=removehandoffsockun->listenOnSocket(
						removehandoffsockname,0066,15);

	if (success) {
		addFileDescriptor(removehandoffsockun->getFileDescriptor());
	} else {
		fprintf(stderr,"Could not listen on unix socket: ");
		fprintf(stderr,"%s\n",removehandoffsockname);
		fprintf(stderr,"Make sure that the file and ");
		fprintf(stderr,"directory are readable and writable.");
		fprintf(stderr,"\n\n");
	}
	delete[] removehandoffsockname;

	return success;
}

void	sqlrlistener::listen() {
	blockSignals();
	for(;;) {
		handleClientConnection(waitForData());
	}
}

void	sqlrlistener::blockSignals() {

	// the daemon class handles SIGTERM's and SIGINT's and SIGCHLDS
	// so we're going to block all other signals 
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
	#ifdef HAVE_SIGALRM
		set.addSignal(SIGALRM);
	#endif
	#ifdef HAVE_SIGSTKFLT
		set.addSignal(SIGSTKFLT);
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

int	sqlrlistener::waitForData() {

	#ifdef SERVER_DEBUG
	debugPrint(logger::logHeader("listener"),0,
				"waiting for client connection...");
	#endif
	int	fd=listener::waitForData(-1,-1);
	#ifdef SERVER_DEBUG
	debugPrint(logger::logHeader("listener"),0,
				"done waiting for client connection");
	#endif

	return fd;
}

void	sqlrlistener::handleClientConnection(int fd) {

	// this method returns 1 or 0.  0 is not an error condition though,
	// it just instructs the calling method that the connection has been
	// handled

	// if something connected to the handoff or derigistration 
	// socket, it must have been a connection, handle it and
	// loop back
	if (passdescriptor) {
		if (fd==handoffsockun->getFileDescriptor()) {
			clientsock=handoffsockun->
					acceptClientConnection();
			registerHandoff(clientsock);
			return;
		} else if (fd==removehandoffsockun->getFileDescriptor()) {
			clientsock=removehandoffsockun->
					acceptClientConnection();
			deRegisterHandoff(clientsock);
			return;
		}
	}


	// handle connections to the client sockets
	if (fd==clientsockin->getFileDescriptor()) {

		clientsock=clientsockin->acceptClientConnection();

		// For inet clients, make sure that the ip address is
		// not denied.  If the ip was denied, disconnect the
		// socket and loop back.
		if (denied && deniedIp()) {
			disconnectClient();
			return;
		}

		forkChild();
		return;

	} else if (fd==clientsockun->getFileDescriptor()) {
		clientsock=clientsockun->acceptClientConnection();
		forkChild();
		return;
	}
}


void	sqlrlistener::registerHandoff(datatransport *sock) {

	#ifdef SERVER_DEBUG
	debugPrint(logger::logHeader("listener"),0,"registering handoff...");
	#endif

	// get the connection daemon's pid
	unsigned long processid;
	sock->read(&processid);

	// find a free node in the list, if we find another node with the
	// same pid, then the old connection must have died off mysteriously,
	// replace it
	for (int i=0; i<maxconnections; i++) {
		if (!handoffsocklist[i]->pid) {
			handoffsocklist[i]->pid=processid;
			handoffsocklist[i]->sock=sock;
			break;
		} else if (handoffsocklist[i]->pid==processid) {
			handoffsocklist[i]->sock=sock;
			break;
		}
	}

	#ifdef SERVER_DEBUG
	debugPrint(logger::logHeader("listener"),0,
				"done registering handoff...");
	#endif
}

void	sqlrlistener::deRegisterHandoff(datatransport *sock) {

	#ifdef SERVER_DEBUG
	debugPrint(logger::logHeader("listener"),0,
				"de-registering handoff...");
	#endif

	// get the connection daemon's pid
	unsigned long	processid;
	sock->read(&processid);

	// remove the matching socket from the list
	for (int i=0; i<maxconnections; i++) {
		if (handoffsocklist[i]->pid==processid) {
			handoffsocklist[i]->pid=0;
			delete handoffsocklist[i]->sock;
			#ifdef SERVER_DEBUG
			debugPrint(logger::logHeader("listener"),0,
					"done de-registering handoff...");
			#endif
			return;
		}
	}

	// clean up
	delete clientsock;

	#ifdef SERVER_DEBUG
	debugPrint(logger::logHeader("listener"),0,
				"done de-registering handoff...");
	#endif
}

int	sqlrlistener::deniedIp() {

	#ifdef SERVER_DEBUG
	debugPrint(logger::logHeader("listener"),0,
				"checking for valid ip...");
	#endif

	char	*ip;
	if ((ip=clientsockin->getClientAddress()) && denied->match(ip) && 
			(!allowed || (allowed && !allowed->match(ip)))) {

		#ifdef SERVER_DEBUG
		debugPrint(logger::logHeader("listener"),0,"invalid ip...");
		#endif

		delete[] ip;
		return 1;
	}

	#ifdef SERVER_DEBUG
	debugPrint(logger::logHeader("listener"),0,"valid ip...");
	#endif

	delete[] ip;
	return 0;
}

void	sqlrlistener::disconnectClient() {
	clientsock->close();
	delete clientsock;
	clientsock=NULL;
}

void	sqlrlistener::forkChild() {

	// if the client connected to one of the non-handoff
	// sockets, fork a child to handle it
	long	childpid;
	if (!(childpid=fork())) {

		#ifdef SERVER_DEBUG
		closeDebugFile();
		openDebugFile("listener",cmdl->getLocalStateDir());
		#endif

		clientSession();

		// since this is the forked off listener, we don't
		// want to actually remove the semaphore set or shared
		// memory segment
		idmemory->dontRemove();
		semset->dontRemove();

		cleanUp();
		_exit(0);
	}

	#ifdef SERVER_DEBUG
	char	debugstring[16+6];
	sprintf(debugstring,"forked a child: %d",childpid);
	debugPrint(logger::logHeader("listener"),0,debugstring);
	#endif


	// the main process doesn't need to stay connected 
	// to the client, only the forked process
	delete clientsock;
}

void	sqlrlistener::clientSession() {

	int	passstatus=0;

	// handle authentication
	int	authstatus=getAuth();

	// 3 possible outcomes: 1=pass 0=fail -1=bad data
	if (authstatus==1) {

		if (dynamicscaling) {
			incrementSessionCount();
		}
		passstatus=handOffClient();

	} else if (authstatus==0) {

		// sleep before and after returning an
		// authentication error to discourage
		// brute-force password attacks
		sleep(2);
		clientsock->write((unsigned short)ERROR);
		sleep(2);
	}

	waitForClientClose(authstatus,passstatus);
	disconnectClient();
}

int	sqlrlistener::getAuth() {

	#ifdef SERVER_DEBUG
	debugPrint(logger::logHeader("listener"),0,
					"getting authentication...");
	#endif

	// Get the user/password. For either one, if they are too big or
	// if there's a read error, just exit with an error code
	unsigned long	size;
	clientsock->read(&size);
	char		userbuffer[USERSIZE+1];
	if (size>USERSIZE || clientsock->read(userbuffer,size)!=size) {
		#ifdef SERVER_DEBUG
		debugPrint(logger::logHeader("listener"),0,
			"authentication failed: user size is wrong");
		#endif
		return -1;
	}
	userbuffer[size]=(char)NULL;

	char		passwordbuffer[USERSIZE+1];
	clientsock->read(&size);
	if (size>USERSIZE || clientsock->read(passwordbuffer,size)!=size) {
		#ifdef SERVER_DEBUG
		debugPrint(logger::logHeader("listener"),0,
			"authentication failed: password size is wrong");
		#endif
		return -1;
	}
	passwordbuffer[size]=(char)NULL;

	// If the listener is supposed to authenticate, then do so, otherwise
	// just return 1 as if authentication succeeded.
	if (authc) {

		// Return 1 if what the client sent matches one of the 
		// user/password sets and 0 if no match is found.
		int	retval=authc->authenticate(userbuffer,passwordbuffer);
		#ifdef SERVER_DEBUG
		if (retval) {
			debugPrint(logger::logHeader("listener"),1,
				"listener-based authentication succeeded");
		} else {
			debugPrint(logger::logHeader("listener"),1,
				"listener-based authentication failed: invalid user/password");
		}
		#endif
		return retval;
	}

	#ifdef SERVER_DEBUG
	debugPrint(logger::logHeader("listener"),0,
				"done getting authentication");
	#endif

	return 1;
}

void	sqlrlistener::incrementSessionCount() {

	#ifdef SERVER_DEBUG
	debugPrint(logger::logHeader("listener"),0,
					"inrementing session count...");
	#endif

	// wait for access
	semset->wait(5);

	// increment the counter
	unsigned int	*sessioncount=
				(unsigned int *)((long)idmemory->getPointer()+
					sizeof(unsigned int));
	(*sessioncount)++;

	#ifdef SERVER_DEBUG
	debugPrint(logger::logHeader("listener"),1,(long)(*sessioncount));
	#endif

	if (dynamicscaling) {
		// signal the scaler
		semset->signal(6);

		// wait for the scaler
		semset->wait(7);
	}

	// signal that others may have access
	semset->signal(5);

	#ifdef SERVER_DEBUG
	debugPrint(logger::logHeader("listener"),0,
				"done inrementing session count");
	#endif
}


int	sqlrlistener::handOffClient() {

	getAConnection();
	clientsock->write((unsigned short)NO_ERROR);

	// if we're passing file descriptors around,
	// tell the client not to reconnect and pass
	// the descriptor to the appropriate database
	// connection daemon, otherwise tell the client
	// to reconnect and which ports to do it on
	if (passdescriptor) {
		clientsock->write((unsigned short)DONT_RECONNECT);

		// if pass descriptor fails, we can still write to the client,
		// send it an error message
		if (!passDescriptor()) {
			clientsock->write((unsigned short)ERROR);
			clientsock->write((unsigned short)70);
			clientsock->write("The listener failed to hand the client off to the database connection.");
			return 0;
		}
	} else {
		clientsock->write((unsigned short)RECONNECT);
		clientsock->write(unixportstrlen);
		clientsock->write(unixportstr);
		clientsock->write((unsigned short)inetport);
	}

	return 1;
}

void	sqlrlistener::getAConnection() {

	#ifdef SERVER_DEBUG
	debugPrint(logger::logHeader("listener"),0,"getting a connection...");
	#endif

	// wait on the read mutex
	semset->wait(1);

	// wait on a writer
	semset->wait(2);

	// get a pointer to the shared memory segment
	char	*ptr=(char *)((long)idmemory->getPointer()+
					(2*sizeof(unsigned int)));

	// if we're passing descriptors around, the connection will pass it's
	// pid to us, otherwise it will pass it's inet and unix ports
	if (passdescriptor) {

		#ifdef SERVER_DEBUG
		debugPrint(logger::logHeader("listener"),1,"handoff=pass");
		#endif

		// get the pid
		connectionpid=*((unsigned long *)ptr);

	} else {

		#ifdef SERVER_DEBUG
		debugPrint(logger::logHeader("listener"),1,"handoff=reconnect");
		#endif

		// buffer to hold the port
		unsigned short	connectionport;

		// get the unix port
		unixportstrlen=0;
		while (*ptr!=':' && unixportstrlen<MAXPATHLEN) {
			unixportstr[unixportstrlen]=*ptr;
			unixportstrlen++;
			ptr++;
		}
		unixportstr[unixportstrlen]=(char)NULL;
		ptr++;

		// get the inet port (it will be right-aligned)
		inetport=*((unsigned char *)ptr) | 
				(*((unsigned char *)(ptr+1)) << 8);

		#ifdef SERVER_DEBUG
		char	debugstring[15+unixportstrlen+21];
		sprintf(debugstring,"socket=%s  port=%d",unixportstr,inetport);
		debugPrint(logger::logHeader("listener"),1,debugstring);
		#endif

	}

	// done reading
	semset->signal(3);

	// signal on the read mutex
	semset->signal(1);

	#ifdef SERVER_DEBUG
	debugPrint(logger::logHeader("listener"),1,"done getting a connection");
	#endif
	
}

int	sqlrlistener::passDescriptor() {

	#ifdef SERVER_DEBUG
	debugPrint(logger::logHeader("listener"),1,"passing descriptor...");
	#endif

	// Look through the list of handoff sockets for the pid of the 
	// connection that we got during the call to getAConnection().
	// When we find it, send the descriptor of the clientsock to the 
	// connection over the handoff socket associated with that node.
	//
	// If for some reason, the socket is closed when we find it, remove it
	// and return an error.

	for (int i=0; i<maxconnections; i++) {
		if (handoffsocklist[i]->pid==connectionpid) {
			if (handoffsocklist[i]->sock->passFileDescriptor(
					clientsock->getFileDescriptor())) {

				#ifdef SERVER_DEBUG
				debugPrint(logger::logHeader("listener"),0,
					"done passing descriptor");
				#endif
				return 1;
			} 
			handoffsocklist[i]->pid=0;
			delete handoffsocklist[i]->sock;
			break;
		}
	}

	#ifdef SERVER_DEBUG
	debugPrint(logger::logHeader("listener"),0,
					"passing descriptor failed");
	#endif
	return 0;
}

void	sqlrlistener::waitForClientClose(int authstatus, int passstatus) {

	// Sometimes the listener sends the ports and closes
	// the socket while they are still buffered but not
	// yet transmitted.  This causes the client to receive
	// partial data or an error.  Telling the socket to
	// linger doesn't always fix it.  Doing a read here
	// should guarantee that the client will close it's end
	// of the connection before the server closes it's end;
	// the server will wait for data from the client
	// (which it will never receive) and when the client
	// closes it's end (which it will only do after getting
	// the ports), the read will fall through.  This should
	// guarantee that the client will get the ports without
	// requiring the client to send data back indicating so.

	unsigned short	dummy;
	if (!passdescriptor || (passdescriptor && authstatus<1)) {

		// If we're not passing descriptors or if
		// we are but authentication failed, the client
		// shouldn't be sending any data, so a single
		// read should suffice.
		clientsock->read(&dummy);

	} else if (!passstatus) {

		// If the descriptor pass failed, the client
		// cound send an entire query and bind vars
		// before it reads the error and closes the
		// socket.  We have to absorb all of that
		// data.  We shouldn't just loop forever
		// though, that would provide a point of entry
		// for a DOS attack.  We'll read the maximum
		// number of bytes that could be sent.

		int	counter=0;
		while (clientsock->read(&dummy)>0 && counter<
				// sending auth
				(sizeof(short)+
				// user/password
				2*(sizeof(long)+USERSIZE)+
				// sending query
				sizeof(short)+
				// need a cursor
				sizeof(short)+
				// executing new query
				sizeof(short)+
				// query size and query
				sizeof(long)+MAXQUERYSIZE+
				// input bind var count
				sizeof(short)+
				// input bind vars
				MAXVAR*(2*sizeof(short)+BINDVARLENGTH)+
				// output bind var count
				sizeof(short)+
				// output bind vars
				MAXVAR*(2*sizeof(short)+BINDVARLENGTH)+
				// get column info
				sizeof(short)+
				// skip/fetch
				2*sizeof(long)
				// divide by two because we're
				// reading 2 bytes at a time
				)/2) {
			counter++;
		}
	}
}
