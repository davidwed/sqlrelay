// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <config.h>

#include <sqlrlistener.h>

#include <rudiments/permissions.h>
#include <rudiments/unixclientsocket.h>
#include <rudiments/inetclientsocket.h>

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

#include <defines.h>
#include <defaults.h>

sqlrlistener::sqlrlistener() : daemonprocess(), listener(), debugfile() {

	cmdl=NULL;

	init=false;

	authc=NULL;

	semset=NULL;
	idmemory=NULL;

	unixport=NULL;
	pidfile=NULL;

	clientsockin=NULL;
	clientsockun=NULL;
	handoffsockun=NULL;
	removehandoffsockun=NULL;
	fixupsockun=NULL;
	fixupsockname=NULL;

	handoffsocklist=NULL;

	denied=NULL;
	allowed=NULL;
}

sqlrlistener::~sqlrlistener() {
	delete semset;
	delete idmemory;
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

void sqlrlistener::cleanUp() {

	delete authc;
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
	delete[] fixupsockname;
	delete fixupsockun;
	delete denied;
	delete allowed;

	#ifdef SERVER_DEBUG
	closeDebugFile();
	#endif
}

bool sqlrlistener::initListener(int argc, const char **argv) {

	init=true;

	cmdl=new cmdline(argc,argv);

	tempdir		tmpdir(cmdl);
	sqlrconfigfile	cfgfl;

	if (!cfgfl.parse(cmdl->getConfig(),cmdl->getId())) {
		return false;
	}

	setUserAndGroup(&cfgfl);

	#ifdef SERVER_DEBUG
	openDebugFile("listener",cmdl->getLocalStateDir());
	#endif

	if (!handlePidFile(&tmpdir,cmdl->getId())) {
		return false;
	}

	handleDynamicScaling(&cfgfl);

	setHandoffMethod(&cfgfl);

	if (cfgfl.getAuthOnListener()) {
		authc=new authenticator(&cfgfl);
	}

	setIpPermissions(&cfgfl);

	if (!createSharedMemoryAndSemaphores(&tmpdir,cmdl->getId())) {
		return false;
	}

	if (!listenOnClientSockets(&cfgfl)) {
		return false;
	}

	if ((passdescriptor=cfgfl.getPassDescriptor())) {
		if (!listenOnHandoffSocket(&tmpdir,cmdl->getId())) {
			return false;
		}
		if (!listenOnDeregistrationSocket(&tmpdir,cmdl->getId())) {
			return false;
		}
		if (!listenOnFixupSocket(&tmpdir,cmdl->getId())) {
			return false;
		}
	}

	#ifndef SERVER_DEBUG
	detach();
	#endif

	return true;
}

void sqlrlistener::setUserAndGroup(sqlrconfigfile *cfgfl) {

	if (!runAsGroup(cfgfl->getRunAsGroup())) {
		fprintf(stderr,"Warning: could not change group to %s\n",
						cfgfl->getRunAsGroup());
	}

	if (!runAsUser(cfgfl->getRunAsUser())) {
		fprintf(stderr,"Warning: could not change user to %s\n",
						cfgfl->getRunAsUser());
	} 
}

bool sqlrlistener::handlePidFile(tempdir *tmpdir, const char *id) {

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
		fprintf(stderr,"	%s",id);
		fprintf(stderr," instance.\n");
		fprintf(stderr,"	If it is not running, please remove ");
		fprintf(stderr,"the file and restart.\n");
		delete[] pidfile;
		pidfile=NULL;
		return false;
	}

	createPidFile(pidfile,permissions::ownerReadWrite());
	return true;
}

void sqlrlistener::handleDynamicScaling(sqlrconfigfile *cfgfl) {

	// get the dynamic connection scaling parameters
	maxconnections=cfgfl->getMaxConnections();

	// if dynamic scaling isn't going to be used, disable it
	dynamicscaling=cfgfl->getDynamicScaling();
}

void sqlrlistener::setHandoffMethod(sqlrconfigfile *cfgfl) {

	// get handoff method
	if (cfgfl->getPassDescriptor()) {

		// create the list of handoff nodes
		handoffsocklist=new handoffsocketnode *[maxconnections];
		for (int i=0; i<maxconnections; i++) {
			handoffsocklist[i]=new handoffsocketnode;
			handoffsocklist[i]->pid=0;
		}
	}
}

void sqlrlistener::setIpPermissions(sqlrconfigfile *cfgfl) {

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

bool sqlrlistener::createSharedMemoryAndSemaphores(tempdir *tmpdir,
							const char *id) {

	// initialize the ipc filename
	char	idfilename[tmpdir->getLength()+1+strlen(id)+1];
	sprintf(idfilename,"%s/%s",tmpdir->getString(),id);

	#ifdef SERVER_DEBUG
	debugPrint("listener",0,"creating shared memory and semaphores");
	debugPrint("listener",0,"id filename: ");
	debugPrint("listener",0,idfilename);
	#endif

	// make sure that the file exists and is read/writeable
	int	idfd=open(idfilename,O_CREAT|O_RDWR,
					permissions::ownerReadWrite());
	if (idfd==-1) {
		ipcFileError(idfilename);
		return false;
	} else {
		close(idfd);
	}


	// get the ipc key
	key_t	key=ftok(idfilename,0);
	if (key==-1) {
		ftokError(idfilename);
		return false;
	}

	// create the shared memory segment
	// FIXME: if it already exists, attempt to remove and re-create it
	#ifdef SERVER_DEBUG
	debugPrint("listener",1,"creating shared memory...");
	#endif

	idmemory=new sharedmemory;
	if (!idmemory->create(key,sizeof(shmdata),
					permissions::ownerReadWrite())) {
		idmemory->attach(key);
		shmError(id,idmemory->getId());
		return false;
	}

	// create (or connect) to the semaphore set
	// FIXME: if it already exists, attempt to remove and re-create it
	#ifdef SERVER_DEBUG
	debugPrint("listener",1,"creating semaphores...");
	#endif
	int	vals[11]={1,1,0,0,1,1,0,0,0,1,0};
	semset=new semaphoreset();
	if (!semset->create(key,permissions::ownerReadWrite(),11,vals)) {

		semset->attach(key,11);
		semError(id,semset->getId());
		return false;
	}

	return true;
}

void sqlrlistener::ipcFileError(const char *idfilename) {
	fprintf(stderr,"Could not open: %s\n",idfilename);
	fprintf(stderr,"Make sure that the file and directory are ");
	fprintf(stderr,"readable and writable.\n\n");
}

void sqlrlistener::ftokError(const char *idfilename) {
	fprintf(stderr,"\nsqlr-listener error:\n");
	fprintf(stderr,"	Unable to generate a key from ");
	fprintf(stderr,"%s\n",idfilename);
	fprintf(stderr,"	Error was: %s\n\n",strerror(errno));
}

void sqlrlistener::shmError(const char *id, int shmid) {
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
	fprintf(stderr,"\n	id %d.\n\n",shmid);
	fprintf(stderr,"	Error was: %s\n\n",strerror(errno));
}

void sqlrlistener::semError(const char *id, int semid) {
	fprintf(stderr,"\nsqlr-listener error:\n");
	fprintf(stderr,"	Unable to create a semaphore ");
	fprintf(stderr,"set.  This is usally because an \n");
	fprintf(stderr,"	sqlr-listener is already ");
	fprintf(stderr,"running for the %s",id);
	fprintf(stderr," instance.\n\n");
	fprintf(stderr,"	If it is not running, ");
	fprintf(stderr,"something may have crashed and left ");
	fprintf(stderr,"an old semaphore set\n");
	fprintf(stderr,"	lying around.  Use the ipcs ");
	fprintf(stderr,"command to inspect existing ");
	fprintf(stderr,"semaphore sets \n");
	fprintf(stderr,"	and the ipcrm ");
	fprintf(stderr,"command to remove the semaphore set ");
	fprintf(stderr,"with \n");
	fprintf(stderr,"	id %d.\n\n",semid);
	fprintf(stderr,"	Error was: %s\n\n",strerror(errno));
}

bool sqlrlistener::listenOnClientSockets(sqlrconfigfile *cfgfl) {

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
	bool	listening=false;
	if (port) {
		clientsockin=new inetserversocket();
		clientsockin->reuseAddresses();
		listening=clientsockin->listen(NULL,port,15);
		if (listening) {
			addFileDescriptor(clientsockin);
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
		listening=clientsockun->listen(unixport,0000,15);
		if (listening) {
			addFileDescriptor(clientsockun);
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

bool sqlrlistener::listenOnHandoffSocket(tempdir *tmpdir, const char *id) {

	// the handoff socket
	char	handoffsockname[tmpdir->getLength()+1+strlen(id)+8+1];
	sprintf(handoffsockname,"%s/%s-handoff",tmpdir->getString(),id);

	handoffsockun=new unixserversocket();
	bool	success=handoffsockun->listen(handoffsockname,0066,15);

	if (success) {
		addFileDescriptor(handoffsockun);
	} else {
		fprintf(stderr,"Could not listen on unix socket: ");
		fprintf(stderr,"%s\n",handoffsockname);
		fprintf(stderr,"Make sure that the file and ");
		fprintf(stderr,"directory are readable and writable.");
		fprintf(stderr,"\n\n");
	}

	return success;
}

bool sqlrlistener::listenOnDeregistrationSocket(tempdir *tmpdir,
							const char *id) {

	// the deregistration socket
	char	removehandoffsockname[tmpdir->getLength()+1+strlen(id)+14+1];
	sprintf(removehandoffsockname,"%s/%s-removehandoff",
						tmpdir->getString(),id);

	removehandoffsockun=new unixserversocket();
	bool	success=removehandoffsockun->listen(
						removehandoffsockname,0066,15);

	if (success) {
		addFileDescriptor(removehandoffsockun);
	} else {
		fprintf(stderr,"Could not listen on unix socket: ");
		fprintf(stderr,"%s\n",removehandoffsockname);
		fprintf(stderr,"Make sure that the file and ");
		fprintf(stderr,"directory are readable and writable.");
		fprintf(stderr,"\n\n");
	}

	return success;
}

bool sqlrlistener::listenOnFixupSocket(tempdir *tmpdir, const char *id) {

	// the fixup socket
	fixupsockname=new char[tmpdir->getLength()+1+strlen(id)+6+1];
	sprintf(fixupsockname,"%s/%s-fixup",tmpdir->getString(),id);

	fixupsockun=new unixserversocket();
	bool	success=fixupsockun->listen(fixupsockname,0066,15);

	if (success) {
		addFileDescriptor(fixupsockun);
	} else {
		fprintf(stderr,"Could not listen on unix socket: ");
		fprintf(stderr,"%s\n",fixupsockname);
		fprintf(stderr,"Make sure that the file and ");
		fprintf(stderr,"directory are readable and writable.");
		fprintf(stderr,"\n\n");
	}

	return success;
}

void sqlrlistener::listen() {
	blockSignals();
	for(;;) {
		// FIXME: this can return true/false, should we do anything
		// with that?
		handleClientConnection(waitForData());
	}
}

void sqlrlistener::blockSignals() {

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

filedescriptor *sqlrlistener::waitForData() {

	#ifdef SERVER_DEBUG
	debugPrint("listener",0,"waiting for client connection...");
	#endif

	// wait for data on one of the sockets...
	// if something bad happened, return an invalid file descriptor
	if (listener::waitForNonBlockingRead(-1,-1)<1) {
		return NULL;
	}

	// return first file descriptor that had data available or an invalid
	// file descriptor on error
	filedescriptor	*fd=NULL;
	listener::getReadyList()->getDataByIndex(0,&fd);

	#ifdef SERVER_DEBUG
	debugPrint("listener",0,"done waiting for client connection");
	#endif

	return fd;
}

bool sqlrlistener::handleClientConnection(filedescriptor *fd) {

	if (!fd) {
		return false;
	}


	// If something connected to the handoff or derigistration 
	// socket, it must have been a connection.
	//
	// If something connected to the fixup socket, it must have been a
	// forked off listener looking for the file descriptor of a socket
	// associated with a newly spawned connection daemon.
	//
	// Either way, handle it and loop back.
	filedescriptor	*clientsock;
	if (passdescriptor) {
		if (fd==handoffsockun) {
			clientsock=handoffsockun->accept();
			return registerHandoff(clientsock);
		} else if (fd==removehandoffsockun) {
			clientsock=removehandoffsockun->accept();
			return deRegisterHandoff(clientsock);
		} else if (fd==fixupsockun) {
			clientsock=fixupsockun->accept();
			return fixup(clientsock);
		}
	}


	// handle connections to the client sockets
	if (fd==clientsockin) {

		clientsock=clientsockin->accept();
		clientsock->translateByteOrder();

		// For inet clients, make sure that the ip address is
		// not denied.  If the ip was denied, disconnect the
		// socket and loop back.
		if (denied && deniedIp()) {
			disconnectClient(clientsock);
			return true;
		}

	} else if (fd==clientsockun) {
		clientsock=clientsockun->accept();
		clientsock->translateByteOrder();
	} else {
		return true;
	}

	// The logic here is that if there are no other forked, busy listeners
	// and there are available connections, then we don't need to fork a
	// child, otherwise we do.
	//
	// It's entirely possible that a connection will become available
	// immediately after this call to getValue(2), but in that case, the
	// worst thing that happens is that we forked a child.  While less
	// efficient, it is safe to do.
	//
	// It is not possible that a connection will immediately become
	// UNavailable after this call to getValue(2).  For that to happen,
	// there would need to be another sqlr-listener out there.  In that
	// case getValue(10) would return something greater than 0 and we would
	// have forked anyway.
	if (dynamicscaling || semset->getValue(10) || !semset->getValue(2)) {

		// increment the number of "forked, busy listeners"
		semset->signal(10);

		forkChild(clientsock);

	} else {

		// increment the number of "forked, busy listeners"
		semset->signal(10);

		clientSession(clientsock);
	}

	return true;
}


bool sqlrlistener::registerHandoff(filedescriptor *sock) {

	#ifdef SERVER_DEBUG
	debugPrint("listener",0,"registering handoff...");
	#endif

	// get the connection daemon's pid
	unsigned long processid;
	if (sock->read(&processid)!=sizeof(unsigned long)) {
		#ifdef SERVER_DEBUG
		debugPrint("listener",1,"failed to read process id");
		#endif
		delete sock;
		return false;
	}

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
	debugPrint("listener",0,"done registering handoff...");
	#endif
	return true;
}

bool sqlrlistener::deRegisterHandoff(filedescriptor *sock) {

	#ifdef SERVER_DEBUG
	debugPrint("listener",0,"de-registering handoff...");
	#endif

	// get the connection daemon's pid
	unsigned long	processid;
	if (sock->read(&processid)!=sizeof(unsigned long)) {
		#ifdef SERVER_DEBUG
		debugPrint("listener",1,"failed to read process id");
		#endif
		delete sock;
		return false;
	}

	// remove the matching socket from the list
	for (int i=0; i<maxconnections; i++) {
		if (handoffsocklist[i]->pid==processid) {
			handoffsocklist[i]->pid=0;
			delete handoffsocklist[i]->sock;
			handoffsocklist[i]->sock=NULL;
			break;
		}
	}

	// clean up
	delete sock;

	#ifdef SERVER_DEBUG
	debugPrint("listener",0,"done de-registering handoff...");
	#endif
	return true;
}

bool sqlrlistener::fixup(filedescriptor *sock) {

	#ifdef SERVER_DEBUG
	debugPrint("listener",0,
			"passing socket of newly spawned connection...");
	#endif

	// get the pid of the connection daemon the child listener needs
	unsigned long	processid;
	if (sock->read(&processid)!=sizeof(unsigned long)) {
		#ifdef SERVER_DEBUG
		debugPrint("listener",1,"failed to read process id");
		#endif
		delete sock;
		return false;
	}

	// look through the handoffsocklist for the pid
	bool	retval=false;
	for (int i=0; i<maxconnections; i++) {
		if (handoffsocklist[i]->pid==processid) {
			retval=sock->passFileDescriptor(handoffsocklist[i]->
						sock->getFileDescriptor());
			#ifdef SERVER_DEBUG
			debugPrint("listener",1,
					"found socket for requested pid ");
			if (retval) {
				debugPrint("listener",1,
						"passed it successfully");
			} else {
				debugPrint("listener",1,"failed to pass it");
			}
			#endif
		}
	}

	// clean up
	delete sock;

	#ifdef SERVER_DEBUG
	debugPrint("listener",0,
			"done passing socket of newly spawned connection");
	#endif

	return retval;
}

bool sqlrlistener::deniedIp() {

	#ifdef SERVER_DEBUG
	debugPrint("listener",0,"checking for valid ip...");
	#endif

	char	*ip;
	if ((ip=clientsockin->getClientAddress()) && denied->match(ip) && 
			(!allowed || (allowed && !allowed->match(ip)))) {

		#ifdef SERVER_DEBUG
		debugPrint("listener",0,"invalid ip...");
		#endif

		delete[] ip;
		return true;
	}

	#ifdef SERVER_DEBUG
	debugPrint("listener",0,"valid ip...");
	#endif

	delete[] ip;
	return false;
}

void sqlrlistener::disconnectClient(filedescriptor *sock) {
	sock->close();
	delete sock;
}

void sqlrlistener::forkChild(filedescriptor *sock) {

	// if the client connected to one of the non-handoff
	// sockets, fork a child to handle it
	long	childpid;
	if (!(childpid=fork())) {

		#ifdef SERVER_DEBUG
		closeDebugFile();
		openDebugFile("listener",cmdl->getLocalStateDir());
		#endif

		clientSession(sock);

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
	sprintf(debugstring,"forked a child: %ld",childpid);
	debugPrint("listener",0,debugstring);
	#endif


	// the main process doesn't need to stay connected 
	// to the client, only the forked process
	delete sock;
}

void sqlrlistener::clientSession(filedescriptor *sock) {

	bool	passstatus=false;

	// handle authentication
	int	authstatus=getAuth(sock);

	// 3 possible outcomes: 1=pass 0=fail -1=bad data
	if (authstatus==1) {

		if (dynamicscaling) {
			incrementSessionCount();
		}
		passstatus=handOffClient(sock);

	} else if (authstatus==0) {

		// sleep before and after returning an
		// authentication error to discourage
		// brute-force password attacks
		sleep(2);
		sock->write((unsigned short)ERROR);
		sleep(2);
	}

	waitForClientClose(authstatus,passstatus,sock);
	disconnectClient(sock);
}

int sqlrlistener::getAuth(filedescriptor *sock) {

	#ifdef SERVER_DEBUG
	debugPrint("listener",0,"getting authentication...");
	#endif

	// Get the user/password. For either one, if they are too big or
	// if there's a read error, just exit with an error code
	unsigned long	size;
	sock->read(&size);
	char		userbuffer[(unsigned long)USERSIZE+1];
	if (size>(unsigned long)USERSIZE ||
	    (unsigned long)(sock->read(userbuffer,size))!=size) {
		#ifdef SERVER_DEBUG
		debugPrint("listener",0,
			"authentication failed: user size is wrong");
		#endif
		return -1;
	}
	userbuffer[size]=(char)NULL;

	char		passwordbuffer[(unsigned long)USERSIZE+1];
	sock->read(&size);
	if (size>(unsigned long)USERSIZE ||
		(unsigned long)(sock->read(passwordbuffer,size))!=size) {
		#ifdef SERVER_DEBUG
		debugPrint("listener",0,
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
		bool	retval=authc->authenticate(userbuffer,passwordbuffer);
		#ifdef SERVER_DEBUG
		if (retval) {
			debugPrint("listener",1,
				"listener-based authentication succeeded");
		} else {
			debugPrint("listener",1,
				"listener-based authentication failed: invalid user/password");
		}
		#endif
		return (retval)?1:0;
	}

	#ifdef SERVER_DEBUG
	debugPrint("listener",0,"done getting authentication");
	#endif

	return 1;
}

void sqlrlistener::incrementSessionCount() {

	#ifdef SERVER_DEBUG
	debugPrint("listener",0,"incrementing session count...");
	#endif

	// wait for access
	semset->wait(5);

	// increment the counter
	unsigned int	*sessioncount=
				(unsigned int *)((long)idmemory->getPointer()+
					sizeof(unsigned int));
	(*sessioncount)++;

	#ifdef SERVER_DEBUG
	debugPrint("listener",1,(long)(*sessioncount));
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
	debugPrint("listener",0,"done incrementing session count");
	#endif
}


bool sqlrlistener::handOffClient(filedescriptor *sock) {

	unsigned long	connectionpid;
	unsigned short	inetport;
	char 		unixportstr[MAXPATHLEN+1];
	unsigned short	unixportstrlen;

	getAConnection(&connectionpid,&inetport,unixportstr,&unixportstrlen);
	sock->write((unsigned short)NO_ERROR);

	// if we're passing file descriptors around,
	// tell the client not to reconnect and pass
	// the descriptor to the appropriate database
	// connection daemon, otherwise tell the client
	// to reconnect and which ports to do it on
	if (passdescriptor) {

		sock->write((unsigned short)DONT_RECONNECT);

		// Get the socket associated with the pid of the available
		// connection and pass the client to the connection.  If any
		// of this fails, we can still write to the client, send it an
		// error message.
		filedescriptor	connectionsock;
		if (!findMatchingSocket(connectionpid,&connectionsock) ||
			!passClientFileDescriptorToConnection(
						&connectionsock,
						sock->getFileDescriptor())) {

			sock->write((unsigned short)ERROR);
			sock->write((unsigned short)70);
			sock->write("The listener failed to hand the client off to the database connection.");
			return false;
		}

		// Set the file descriptor to -1 here, otherwise it will get
		// closed when connectionsock is freed.  If the file descriptor
		// gets closed, the next time we try to pass a file descriptor
		// to the same connection, it will fail.
		connectionsock.setFileDescriptor(-1);

	} else {
		sock->write((unsigned short)RECONNECT);
		sock->write(unixportstrlen);
		sock->write(unixportstr);
		sock->write((unsigned short)inetport);
	}

	return true;
}

void sqlrlistener::getAConnection(unsigned long *connectionpid,
					unsigned short *inetport,
					char *unixportstr,
					unsigned short *unixportstrlen) {

	for (;;) {

		#ifdef SERVER_DEBUG
		debugPrint("listener",0,"getting a connection...");
		#endif

		// wait for exclusive access to the
		// shared memory among listeners
		semset->wait(1);

		// wait for an available connection
		semset->wait(2);

		// get a pointer to the shared memory segment
		shmdata	*ptr=(shmdata *)idmemory->getPointer();

		// if we're passing descriptors around, the connection will
		// pass it's pid to us, otherwise it will pass it's inet and
		// unix ports
		if (passdescriptor) {

			#ifdef SERVER_DEBUG
			debugPrint("listener",1,"handoff=pass");
			#endif

			// get the pid
			*connectionpid=ptr->connectioninfo.connectionpid;

		} else {

			#ifdef SERVER_DEBUG
			debugPrint("listener",1,"handoff=reconnect");
			#endif

			// get the inet port
			*inetport=ptr->connectioninfo.sockets.inetport;

			// get the unix port
			strncpy(unixportstr,
				ptr->connectioninfo.sockets.unixsocket,
				MAXPATHLEN);
			*unixportstrlen=strlen(unixportstr);

			#ifdef SERVER_DEBUG
			char	debugstring[15+*unixportstrlen+21];
			sprintf(debugstring,"socket=%s  port=%d",
						unixportstr,*inetport);
			debugPrint("listener",1,debugstring);
			#endif

		}

		// tell the connection that we've gotten it's data
		semset->signal(3);

		// allow other listeners access to the shared memory
		semset->signal(1);

		// decerment the number of "forked, busy listeners"
		semset->wait(10);

		// make sure the connection is actually up, if not, fork a child
		// to jog it, spin back and get another connection
		if (connectionIsUp(ptr->connectionid)) {
			break;
		} else {
			pingDatabase(*connectionpid,unixportstr,*inetport);
			semset->signal(10);
		}

		#ifdef SERVER_DEBUG
		debugPrint("listener",1,"done getting a connection");
		#endif
	}
	
}

bool sqlrlistener::connectionIsUp(const char *connectionid) {

	// initialize the database up/down filename
	char	updown[strlen(TMP_DIR)+1+strlen(cmdl->getId())+1+
						strlen(connectionid)+1];
	sprintf(updown,"%s/%s-%s",TMP_DIR,cmdl->getId(),connectionid);
	bool	retval=file::exists(updown);
	return retval;
}

void sqlrlistener::pingDatabase(unsigned long connectionpid,
					const char *unixportstr,
					unsigned short inetport) {

	// fork off and cause the connection to ping the database, this should
	// cause it to reconnect
	long	childpid;
	if (!(childpid=fork())) {

		// connect to the database connection
		filedescriptor	*connsock=connectToConnection(connectionpid,
								unixportstr,
								inetport);
		if (connsock) {

			// send it a ping command
			connsock->write((unsigned short)PING);

			// get the ping result
			unsigned short	result=1;
			if (connsock->read(&result)!=sizeof(unsigned short)) {
				result=0;
			}

			// disconnect
			disconnectFromConnection(connsock);
		}

		// since this is the forked off listener, we don't
		// want to actually remove the semaphore set or shared
		// memory segment
		idmemory->dontRemove();
		semset->dontRemove();

		cleanUp();
		_exit(0);
	}
}

filedescriptor *sqlrlistener::connectToConnection(unsigned long connectionpid,
						const char *unixportstr,
						unsigned short inetport) {

	if (passdescriptor) {

		for (int i=0; i<maxconnections; i++) {
			if (handoffsocklist[i]->pid==connectionpid) {
				return handoffsocklist[i]->sock;
			}
		}

	} else {

		int connected=0;

		// first, try for the unix port
		if (unixportstr && unixportstr[0]) {
			unixclientsocket	*unixsock=
							new unixclientsocket();
			connected=unixsock->connect(unixportstr,-1,-1,0,1);
			if (connected) {
				return unixsock;
			}
		}

		// then try for the inet port
		if (!connected) {
			inetclientsocket	*inetsock=
							new inetclientsocket();
			connected=inetsock->connect("127.0.0.1",inetport,
								-1,-1,0,1);
			if (connected) {
				return inetsock;
			}
		}
	}
	return NULL;
}

void sqlrlistener::disconnectFromConnection(filedescriptor *sock) {
	sock->close();
	if (!passdescriptor) {
		delete sock;
	}
}

bool sqlrlistener::passClientFileDescriptorToConnection(
					filedescriptor *connectionsock,
					int fd) {

	#ifdef SERVER_DEBUG
	debugPrint("listener",1,"passing descriptor...");
	#endif

	// pass the file descriptor of the connected client over to the
	// available connection
	bool	retval=connectionsock->passFileDescriptor(fd);
	if (retval) {

		#ifdef SERVER_DEBUG
		debugPrint("listener",0,"done passing descriptor");
		#endif

	} else {

		#ifdef SERVER_DEBUG
		debugPrint("listener",0,"passing descriptor failed");
		#endif

		// If we get an error, delete the socket and remove the
		// connection from the list.  The connection must be out of
		// commission for some reason, perhaps it died.  We need to
		// remove it from the list because if we didn't fork off to
		// handle the client and another client connects, we'll be
		// using the same list and we don't want to run into this
		// trouble again.
		// FIXME: What if a client gets forked off and runs into the
		// bad connection?  The bad connection is still in the parent
		// process's list.
		/*delete availableconnectionsock;
		availableconnectionsock=NULL;
		*availableconnectionpid=0;*/
	}
	return retval;
}

bool sqlrlistener::findMatchingSocket(unsigned long connectionpid,
					filedescriptor *connectionsock) {

	// Look through the list of handoff sockets for the pid of the 
	// connection that we got during the call to getAConnection().
	// When we find it, send the descriptor of the clientsock to the 
	// connection over the handoff socket associated with that node.
	for (int i=0; i<maxconnections; i++) {
		if (handoffsocklist[i]->pid==connectionpid) {
			connectionsock->setFileDescriptor(handoffsocklist[i]->
						sock->getFileDescriptor());
			return true;
		}
	}

	// if the available connection wasn't in our list then it must have
	// fired up after we forked, so we'll need to connect back to the main
	// listener process and ask it for the pid
	return requestFixup(connectionpid,connectionsock);
}

bool sqlrlistener::requestFixup(unsigned long connectionpid,
					filedescriptor *connectionsock) {

	#ifdef SERVER_DEBUG
	debugPrint("listener",0,
			"requesting socket of newly spawned connection...");
	#endif

	// connect to the fixup socket of the parent listener
	unixclientsocket	fixupclientsockun;
	if (!fixupclientsockun.connect(fixupsockname,-1,-1,0,1)) {
		#ifdef SERVER_DEBUG
		debugPrint("listener",0,
			"failed to connect to parent listener process");
		#endif
		return false;
	}

	// send the pid of the connection that we need
	if (fixupclientsockun.write(connectionpid)!=sizeof(unsigned long)) {
		#ifdef SERVER_DEBUG
		debugPrint("listener",0,"failed to send the pid");
		#endif
		return false;
	}

	// get the file descriptor of the socket
	int	fd;
	if (!fixupclientsockun.receiveFileDescriptor(&fd)) {
		#ifdef SERVER_DEBUG
		debugPrint("listener",0,"failed to receive the socket");
		#endif
		return false;
	}
	connectionsock->setFileDescriptor(fd);

	#ifdef SERVER_DEBUG
	debugPrint("listener",0,
			"received socket of newly spawned connection");
	#endif
	return true;
}

void sqlrlistener::waitForClientClose(int authstatus, bool passstatus,
							filedescriptor *sock) {

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
		sock->read(&dummy);

	} else if (!passstatus) {

		// If the descriptor pass failed, the client
		// cound send an entire query and bind vars
		// before it reads the error and closes the
		// socket.  We have to absorb all of that
		// data.  We shouldn't just loop forever
		// though, that would provide a point of entry
		// for a DOS attack.  We'll read the maximum
		// number of bytes that could be sent.

		unsigned int	counter=0;
		sock->useNonBlockingMode();
		while (sock->read(&dummy)>0 && counter<
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
		sock->useBlockingMode();
	}
}
