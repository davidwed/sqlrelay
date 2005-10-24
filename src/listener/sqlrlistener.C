// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <config.h>

#include <sqlrlistener.h>

#include <rudiments/signalclasses.h>
#include <rudiments/permissions.h>
#include <rudiments/unixclientsocket.h>
#include <rudiments/inetclientsocket.h>
#include <rudiments/rawbuffer.h>
#include <rudiments/snooze.h>
#include <rudiments/passwdentry.h>
#include <rudiments/groupentry.h>
#include <rudiments/process.h>
#include <rudiments/file.h>
#include <rudiments/error.h>

// for printf
#include <stdio.h>

// for _exit
#ifdef HAVE_UNISTD_H
	#include <unistd.h>
#endif

#include <defines.h>
#include <defaults.h>

#ifdef RUDIMENTS_NAMESPACE
using namespace rudiments;
#endif

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

	maxquerysize=0;
	idleclienttimeout=-1;
}

sqlrlistener::~sqlrlistener() {
	delete semset;
	delete idmemory;
	if (unixport) {
		file::remove(unixport);
	}
	if (pidfile) {
		file::remove(pidfile);
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
		for (int32_t i=0; i<maxconnections; i++) {
			delete handoffsocklist[i].sock;
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

	if (!verifyAccessToConfigFile(cmdl->getConfig(),&cfgfl)) {
		return false;
	}

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

	idleclienttimeout=cfgfl.getIdleClientTimeout();
	maxquerysize=cfgfl.getMaxQuerySize();

	#ifndef SERVER_DEBUG
	detach();
	#endif

	return true;
}

void sqlrlistener::setUserAndGroup(sqlrconfigfile *cfgfl) {

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

bool sqlrlistener::verifyAccessToConfigFile(const char *configfile,
						sqlrconfigfile *cfgfl) {

	if (!cfgfl->getDynamicScaling()) {
		return true;
	}

	file	test;
	if (!test.open(configfile,O_RDONLY)) {
		fprintf(stderr,"\nsqlr-listener error:\n");
		fprintf(stderr,"	This instance of SQL Relay is ");
		fprintf(stderr,"configured to run as:\n");
		fprintf(stderr,"		user: %s\n",
						cfgfl->getRunAsUser());
		fprintf(stderr,"		group: %s\n\n",
						cfgfl->getRunAsGroup());
		fprintf(stderr,"	However, the config file %s\n",
								configfile);
		fprintf(stderr,"	cannot be read by that user ");
		fprintf(stderr,"or group.\n\n");
		fprintf(stderr,"	Since you're using dynamic scaling ");
		fprintf(stderr,"(ie. maxconnections>connections),\n");
		fprintf(stderr,"	new connections would be started as\n");
		fprintf(stderr,"		user: %s\n",
						cfgfl->getRunAsUser());
		fprintf(stderr,"		group: %s\n\n",
						cfgfl->getRunAsGroup());
		fprintf(stderr,"	They would not be able to read the");
		fprintf(stderr,"config file and would shut down.\n\n");
		fprintf(stderr,"	To remedy this problem, make %s\n",
								configfile);
		fprintf(stderr,"	readable by\n");
		fprintf(stderr,"		user: %s\n",
						cfgfl->getRunAsUser());
		fprintf(stderr,"		group: %s\n",
						cfgfl->getRunAsGroup());
		return false;
	}
	test.close();
	return true;
}

bool sqlrlistener::handlePidFile(tempdir *tmpdir, const char *id) {

	// check/set pid file
	size_t	pidfilelen=tmpdir->getLength()+20+charstring::length(id)+1;
	pidfile=new char[pidfilelen];
	snprintf(pidfile,pidfilelen,
			"%s/pids/sqlr-listener-%s",tmpdir->getString(),id);

	if (checkForPidFile(pidfile)!=-1) {
		fprintf(stderr,"\nsqlr-listener error:\n");
		fprintf(stderr,"	The pid file %s",pidfile);
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
		handoffsocklist=new handoffsocketnode[maxconnections];
		for (int32_t i=0; i<maxconnections; i++) {
			handoffsocklist[i].pid=0;
		}
	}
}

void sqlrlistener::setIpPermissions(sqlrconfigfile *cfgfl) {

	// get denied and allowed ip's and compile the expressions
	const char	*deniedips=cfgfl->getDeniedIps();
	const char	*allowedips=cfgfl->getAllowedIps();
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
	size_t	idfilenamelen=tmpdir->getLength()+5+charstring::length(id)+1;
	char	*idfilename=new char[idfilenamelen];
	snprintf(idfilename,idfilenamelen,"%s/ipc/%s",tmpdir->getString(),id);

	#ifdef SERVER_DEBUG
	debugPrint("listener",0,"creating shared memory and semaphores");
	debugPrint("listener",0,"id filename: ");
	debugPrint("listener",0,idfilename);
	#endif

	// make sure that the file exists and is read/writeable
	if (!file::createFile(idfilename,permissions::ownerReadWrite())) {
		ipcFileError(idfilename);
		delete[] idfilename;
		return false;
	}

	// get the ipc key
	key_t	key=file::generateKey(idfilename,1);
	if (key==-1) {
		keyError(idfilename);
		delete[] idfilename;
		return false;
	}
	delete[] idfilename;

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
	rawbuffer::zero(idmemory->getPointer(),sizeof(shmdata));

	// create (or connect) to the semaphore set
	// FIXME: if it already exists, attempt to remove and re-create it
	#ifdef SERVER_DEBUG
	debugPrint("listener",1,"creating semaphores...");
	#endif

	// semaphores are:
	//
	// "connection count" - number of open database connections
	// "session count" - number of clients currently connected
	//
	// 9 - UNUSED???
	//
	// 0 - connection: connection registration mutex
	// 1 - listener:   connection registration mutex
	//
	// connection/listener registration interlocks:
	// 2 - connection/listener: ensures that it's safe for a listener to
	//                          read a registration
	//       listener waits for a connection to register itself
	//       connection signals when it's done registering
	// 3 - connection/listener: ensures that it's safe for a connection to
	//                          register itself
	//       connection waits for the listener to read its registration
	//       listener signals when it's done reading a registration
	//
	// connection/lisetner/scaler interlocks:
	// 6 - scaler/listener: used to decide whether to scale or not
	//       listener signals after incrementing session count
	//       scaler waits before counting sessions/connections
	// 4 - connection/scaler: connection count mutex
	//       connection increases/decreases connection count
	//       scalar reads connection count
	// 5 - connection/listener: session count mutex
        //       listener increases session count when a client connects
	//       connection decreases session count when a client disconnects
	// 7 - scaler/listener: used to decide whether to scale or not
	//       scaler signals after counting sessions/connections
	//       listener waits for scaler to count sessions/connections
	// 8 - scaler/connection:
	//       scaler waits for the connection count to increase
	//                 (in effect, waiting for a new connection to fire up)
	//       connection signals after increasing connection count
	//
	// main listenter process/listener children:
	// 10 - listener: number of busy listeners
	//
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

void sqlrlistener::keyError(const char *idfilename) {
	fprintf(stderr,"\nsqlr-listener error:\n");
	fprintf(stderr,"	Unable to generate a key from ");
	fprintf(stderr,"%s\n",idfilename);
	fprintf(stderr,"	Error was: %s\n\n",error::getErrorString());
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
	fprintf(stderr,"	Error was: %s\n\n",error::getErrorString());
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
	fprintf(stderr,"	Error was: %s\n\n",error::getErrorString());
}

bool sqlrlistener::listenOnClientSockets(sqlrconfigfile *cfgfl) {

	// get inet and unix ports to listen on
	uint16_t	port=cfgfl->getPort();
	const char	*uport=cfgfl->getUnixPort();
	if (uport && uport[0]) {
		unixport=charstring::duplicate(uport);
	}

	// if neither port nor unixport are specified, set up to 
	// listen on the default inet port
	if (!port && !unixport) {
		port=charstring::toInteger(DEFAULT_PORT);
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
	size_t	handoffsocknamelen=tmpdir->getLength()+9+
					charstring::length(id)+8+1;
	char	*handoffsockname=new char[handoffsocknamelen];
	snprintf(handoffsockname,handoffsocknamelen,
			"%s/sockets/%s-handoff",tmpdir->getString(),id);

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

	delete[] handoffsockname;
	return success;
}

bool sqlrlistener::listenOnDeregistrationSocket(tempdir *tmpdir,
							const char *id) {

	// the deregistration socket
	size_t	removehandoffsocknamelen=tmpdir->getLength()+9+
						charstring::length(id)+14+1;
	char	*removehandoffsockname=new char[removehandoffsocknamelen];
	snprintf(removehandoffsockname,removehandoffsocknamelen,
			"%s/sockets/%s-removehandoff",tmpdir->getString(),id);

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

	delete[] removehandoffsockname;

	return success;
}

bool sqlrlistener::listenOnFixupSocket(tempdir *tmpdir, const char *id) {

	// the fixup socket
	size_t	fixupsocknamelen=tmpdir->getLength()+9+
					charstring::length(id)+6+1;
	fixupsockname=new char[fixupsocknamelen];
	snprintf(fixupsockname,fixupsocknamelen,
			"%s/sockets/%s-fixup",tmpdir->getString(),id);

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
		if (denied && deniedIp(clientsock)) {
			delete clientsock;
			return true;
		}

	} else if (fd==clientsockun) {
		clientsock=clientsockun->accept();
		clientsock->translateByteOrder();
	} else {
		return true;
	}

	clientsock->dontUseNaglesAlgorithm();
	clientsock->setTcpReadBufferSize(8192);
	clientsock->setTcpWriteBufferSize(0);
	clientsock->setReadBufferSize(8192);
	clientsock->setWriteBufferSize(8192);

	// Don't fork unless we have to.
	//
	// If there are no busy listeners and there are available connections,
	// then we don't need to fork a child, otherwise we do.
	//
	// It's possible that getValue(2) will be 0, indicating no connections
	// are available, but one will become available immediately after this
	// call to getValue(2).  In that case, the worst thing that happens is
	// that we forked.  While less efficient, it is safe to do.
	//
	// It is not possible that a connection will immediately become
	// UNavailable after this call to getValue(2).  For that to happen,
	// there would need to be another main sqlr-listener process out there.
	// This should never happen, the listener would have to have the same
	// id as this one and that is checked at startup.  However, if it did
	// happen, getValue(10) would return something greater than 0 and we
	// would have forked anyway.
	if (dynamicscaling || semset->getValue(10) || !semset->getValue(2)) {

		// increment the number of "busy listeners"
		semset->signal(10);

		forkChild(clientsock);

	} else {

		// increment the number of "busy listeners"
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
	uint32_t processid;
	if (sock->read(&processid)!=sizeof(uint32_t)) {
		#ifdef SERVER_DEBUG
		debugPrint("listener",1,"failed to read process id");
		#endif
		delete sock;
		return false;
	}

	// find a free node in the list, if we find another node with the
	// same pid, then the old connection must have died off mysteriously,
	// replace it
	bool	inserted=false;
	for (int32_t i=0; i<maxconnections; i++) {
		if (!handoffsocklist[i].pid) {
			handoffsocklist[i].pid=processid;
			handoffsocklist[i].sock=sock;
			inserted=true;
			break;
		} else if (handoffsocklist[i].pid==processid) {
			handoffsocklist[i].sock=sock;
			inserted=true;
			break;
		}
	}

	// if for some reason the scaler started more connections than
	// "maxconnections" or if someone manually started one and the number
	// of connections exceeded maxconnections, then the new connection won't
	// fit in our list, grow the list to accommodate it...
	if (inserted==false) {
		handoffsocketnode	*newhandoffsocklist=
				new handoffsocketnode[maxconnections+1];
		for (int32_t i=0; i<maxconnections; i++) {
			newhandoffsocklist[i].pid=handoffsocklist[i].pid;
			newhandoffsocklist[i].sock=handoffsocklist[i].sock;
		}
		delete[] handoffsocklist;
		newhandoffsocklist[maxconnections].pid=processid;
		newhandoffsocklist[maxconnections].sock=sock;
		maxconnections++;
		handoffsocklist=newhandoffsocklist;
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
	uint32_t	processid;
	if (sock->read(&processid)!=sizeof(uint32_t)) {
		#ifdef SERVER_DEBUG
		debugPrint("listener",1,"failed to read process id");
		#endif
		delete sock;
		return false;
	}

	// remove the matching socket from the list
	for (int32_t i=0; i<maxconnections; i++) {
		if (handoffsocklist[i].pid==processid) {
			handoffsocklist[i].pid=0;
			delete handoffsocklist[i].sock;
			handoffsocklist[i].sock=NULL;
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
	uint32_t	processid;
	if (sock->read(&processid)!=sizeof(uint32_t)) {
		#ifdef SERVER_DEBUG
		debugPrint("listener",1,"failed to read process id");
		#endif
		delete sock;
		return false;
	}

	// look through the handoffsocklist for the pid
	bool	retval=false;
	for (int32_t i=0; i<maxconnections; i++) {
		if (handoffsocklist[i].pid==processid) {
			retval=sock->passFileDescriptor(handoffsocklist[i].
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
			break;
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

bool sqlrlistener::deniedIp(filedescriptor *clientsock) {

	#ifdef SERVER_DEBUG
	debugPrint("listener",0,"checking for valid ip...");
	#endif

	char	*ip=clientsock->getPeerAddress();
	if (ip && denied->match(ip) && 
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

void sqlrlistener::forkChild(filedescriptor *clientsock) {

	// if the client connected to one of the non-handoff
	// sockets, fork a child to handle it
	pid_t	childpid;
	if (!(childpid=fork())) {

		#ifdef SERVER_DEBUG
		closeDebugFile();
		openDebugFile("listener",cmdl->getLocalStateDir());
		#endif

		clientSession(clientsock);

		// since this is the forked off listener, we don't
		// want to actually remove the semaphore set or shared
		// memory segment
		idmemory->dontRemove();
		semset->dontRemove();

		cleanUp();
		exit(0);
	}

	#ifdef SERVER_DEBUG
	char	debugstring[22];
	snprintf(debugstring,22,"forked a child: %d",childpid);
	debugPrint("listener",0,debugstring);
	#endif


	// the main process doesn't need to stay connected 
	// to the client, only the forked process
	delete clientsock;
}

void sqlrlistener::clientSession(filedescriptor *clientsock) {

	bool	passstatus=false;

	// handle authentication
	int32_t	authstatus=getAuth(clientsock);

	// 3 possible outcomes: 1=pass 0=fail -1=bad data
	if (authstatus==1) {

		if (dynamicscaling) {
			incrementSessionCount();
		}
		passstatus=handOffClient(clientsock);

	} else if (authstatus==0) {

		#ifdef SERVER_DEBUG
		debugPrint("listener",1,"sending client auth error");
		#endif

		// snooze before and after returning an
		// authentication error to discourage
		// brute-force password attacks
		snooze::macrosnooze(2);
		clientsock->write((uint16_t)ERROR);
		flushWriteBuffer(clientsock);
		snooze::macrosnooze(2);
	}

	// FIXME: if we got -1 from getAuth, then the client may be spewing
	// garbage and we should close the connection...
	waitForClientClose(authstatus,passstatus,clientsock);
	delete clientsock;
}

int32_t sqlrlistener::getAuth(filedescriptor *clientsock) {

	#ifdef SERVER_DEBUG
	debugPrint("listener",0,"getting authentication...");
	#endif

	// Get the user/password. For either one, if they are too big or
	// if there's a read error, just exit with an error code
	uint32_t	size;
	clientsock->read(&size,idleclienttimeout,0);
	char		userbuffer[(uint32_t)USERSIZE+1];
	if (size>(uint32_t)USERSIZE ||
		(uint32_t)(clientsock->read(userbuffer,size,
						idleclienttimeout,0))!=size) {
		#ifdef SERVER_DEBUG
		debugPrint("listener",0,
			"authentication failed: user size is wrong");
		#endif
		return -1;
	}
	userbuffer[size]=(char)NULL;

	char		passwordbuffer[(uint32_t)USERSIZE+1];
	clientsock->read(&size,idleclienttimeout,0);
	if (size>(uint32_t)USERSIZE ||
		(uint32_t)(clientsock->read(passwordbuffer,size,
						idleclienttimeout,0))!=size) {
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
	#ifdef SERVER_DEBUG
	debugPrint("listener",1,"waiting for exclusive access...");
	#endif
	semset->waitWithUndo(5);
	#ifdef SERVER_DEBUG
	debugPrint("listener",1,"done waiting for exclusive access...");
	#endif

	// increment the counter
	uint32_t	*sessioncount=
				(uint32_t *)((long)idmemory->getPointer()+
					sizeof(uint32_t));
	(*sessioncount)++;

	#ifdef SERVER_DEBUG
	debugPrint("listener",1,(int32_t)(*sessioncount));
	#endif

	if (dynamicscaling) {
		// signal the scaler
		semset->signal(6);

		// wait for the scaler
		#ifdef SERVER_DEBUG
		debugPrint("listener",1,"waiting for the scaler...");
		#endif
		semset->wait(7);
		#ifdef SERVER_DEBUG
		debugPrint("listener",1,"done waiting for the scaler...");
		#endif
	}

	// signal that others may have access
	semset->signalWithUndo(5);

	#ifdef SERVER_DEBUG
	debugPrint("listener",0,"done incrementing session count");
	#endif
}


bool sqlrlistener::handOffClient(filedescriptor *sock) {

	uint32_t	connectionpid;
	uint16_t	inetport;
	char 		unixportstr[MAXPATHLEN+1];
	uint16_t	unixportstrlen;
	bool		retval=false;

	// loop in case client doesn't get handed off successfully
	for (;;) {

		getAConnection(&connectionpid,&inetport,
				unixportstr,&unixportstrlen);

		// if we're passing file descriptors around,
		// tell the client not to reconnect and pass
		// the descriptor to the appropriate database
		// connection daemon, otherwise tell the client
		// to reconnect and which ports to do it on
		if (passdescriptor) {

			// Get the socket associated with the pid of the
			// available connection and pass the client to the
			// connection.  If any of this fails, the connection
			// may have crashed or been killed.  Loop back and get
			// another connection.
			filedescriptor	connectionsock;
			if (!findMatchingSocket(connectionpid,
						&connectionsock) ||
				!passClientFileDescriptorToConnection(
						&connectionsock,
						sock->getFileDescriptor())) {

				// FIXME: should there be a limit to the number
				// of times we retry?  If so, should we return
				// this error
				/*sock->write((uint16_t)ERROR);
				sock->write((uint16_t)70);
				sock->write("The listener failed to hand the client off to the database connection.");
				retval=false;
				break;*/
				continue;
			}

			// Set the file descriptor to -1 here, otherwise it
			// will get closed when connectionsock is freed.  If
			// the file descriptor gets closed, the next time we
			// try to pass a file descriptor to the same connection,
			// it will fail.
			connectionsock.setFileDescriptor(-1);

			// if we got this far, everything has worked,
			// inform the client...
			sock->write((uint16_t)NO_ERROR);
			sock->write((uint16_t)DONT_RECONNECT);
			flushWriteBuffer(sock);
			retval=true;
			break;

		} else {

			// FIXME: if we're not passing around file descriptors,
			// how can we deterimine if a connection has crashed
			// or been killed?
			sock->write((uint16_t)NO_ERROR);
			sock->write((uint16_t)RECONNECT);
			sock->write(unixportstrlen);
			sock->write(unixportstr);
			sock->write((uint16_t)inetport);
			flushWriteBuffer(sock);
			retval=true;
			break;
		}
	}

	// decrement the number of "busy listeners"
	#ifdef SERVER_DEBUG
	debugPrint("listener",0,"decrementing busy listeners");
	#endif
	semset->wait(10);
	#ifdef SERVER_DEBUG
	debugPrint("listener",0,"done decrementing busy listeners");
	#endif

	return retval;
}

void sqlrlistener::getAConnection(uint32_t *connectionpid,
					uint16_t *inetport,
					char *unixportstr,
					uint16_t *unixportstrlen) {

	// get a pointer to the shared memory segment
	shmdata	*ptr=(shmdata *)idmemory->getPointer();

	for (;;) {

		#ifdef SERVER_DEBUG
		debugPrint("listener",0,"getting a connection...");
		#endif

		// wait for exclusive access to the
		// shared memory among listeners
		#ifdef SERVER_DEBUG
		debugPrint("listener",0,"acquiring exclusive shm access");
		#endif
		semset->waitWithUndo(1);
		#ifdef SERVER_DEBUG
		debugPrint("listener",0,"done acquiring exclusive shm access");
		#endif

		// wait for an available connection
		#ifdef SERVER_DEBUG
		debugPrint("listener",0,
				"waiting for an available connection");
		#endif
		semset->wait(2);
		#ifdef SERVER_DEBUG
		debugPrint("listener",0,
				"done waiting for an available connection");
		#endif

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
			charstring::copy(unixportstr,
				ptr->connectioninfo.sockets.unixsocket,
				MAXPATHLEN);
			*unixportstrlen=charstring::length(unixportstr);

			#ifdef SERVER_DEBUG
			size_t	debugstringlen=15+*unixportstrlen+21;
			char	*debugstring=new char[debugstringlen];
			snprintf(debugstring,debugstringlen,
					"socket=%s  port=%d",
						unixportstr,*inetport);
			debugPrint("listener",1,debugstring);
			delete[] debugstring;
			#endif

		}

		// tell the connection that we've gotten it's data
		#ifdef SERVER_DEBUG
		debugPrint("listener",0,
				"signalling connection that we've read");
		#endif
		semset->signal(3);
		#ifdef SERVER_DEBUG
		debugPrint("listener",0,
				"done signalling connection that we've read");
		#endif

		// allow other listeners access to the shared memory
		#ifdef SERVER_DEBUG
		debugPrint("listener",0,"releasing exclusive shm access");
		#endif
		semset->signalWithUndo(1);
		#ifdef SERVER_DEBUG
		debugPrint("listener",0,"done releasing exclusive shm access");
		#endif

		// make sure the connection is actually up, if not,
		// fork a child to jog it, spin back and get another connection
		if (connectionIsUp(ptr->connectionid)) {

			#ifdef SERVER_DEBUG
			debugPrint("listener",1,"done getting a connection");
			#endif
			break;

		} else {

			#ifdef SERVER_DEBUG
			debugPrint("listener",1,"connection was down");
			#endif

			pingDatabase(*connectionpid,unixportstr,*inetport);
		}
	}
	
}

bool sqlrlistener::connectionIsUp(const char *connectionid) {

	// initialize the database up/down filename
	size_t	updownlen=charstring::length(TMP_DIR)+5+
			charstring::length(cmdl->getId())+1+
			charstring::length(connectionid)+1;
	char	*updown=new char[updownlen];
	snprintf(updown,updownlen,"%s/ipc/%s-%s",
			TMP_DIR,cmdl->getId(),connectionid);
	bool	retval=file::exists(updown);
	delete[] updown;
	return retval;
}

void sqlrlistener::pingDatabase(uint32_t connectionpid,
					const char *unixportstr,
					uint16_t inetport) {

	// fork off and cause the connection to ping the database, this should
	// cause it to reconnect
	pid_t	childpid;
	if (!(childpid=fork())) {

		// connect to the database connection
		filedescriptor	*connsock=connectToConnection(connectionpid,
								unixportstr,
								inetport);
		if (connsock) {

			// send it a ping command
			connsock->write((uint16_t)PING);
			flushWriteBuffer(connsock);

			// get the ping result
			uint16_t	result=1;
			if (connsock->read(&result)!=sizeof(uint16_t)) {
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

filedescriptor *sqlrlistener::connectToConnection(uint32_t connectionpid,
						const char *unixportstr,
						uint16_t inetport) {

	if (passdescriptor) {

		for (int32_t i=0; i<maxconnections; i++) {
			if (handoffsocklist[i].pid==connectionpid) {
				return handoffsocklist[i].sock;
			}
		}

	} else {

		int32_t connected=RESULT_ERROR;

		// first, try for the unix port
		if (unixportstr && unixportstr[0]) {
			unixclientsocket	*unixsock=
							new unixclientsocket();
			connected=unixsock->connect(unixportstr,-1,-1,0,1);
			if (connected==RESULT_SUCCESS) {
				return unixsock;
			}
		}

		// then try for the inet port
		if (connected!=RESULT_SUCCESS) {
			inetclientsocket	*inetsock=
							new inetclientsocket();
			connected=inetsock->connect("127.0.0.1",inetport,
								-1,-1,0,1);
			if (connected==RESULT_SUCCESS) {
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

bool sqlrlistener::findMatchingSocket(uint32_t connectionpid,
					filedescriptor *connectionsock) {

	// Look through the list of handoff sockets for the pid of the 
	// connection that we got during the call to getAConnection().
	// When we find it, send the descriptor of the clientsock to the 
	// connection over the handoff socket associated with that node.
	for (int32_t i=0; i<maxconnections; i++) {
		if (handoffsocklist[i].pid==connectionpid) {
			connectionsock->setFileDescriptor(handoffsocklist[i].
						sock->getFileDescriptor());
			return true;
		}
	}

	// if the available connection wasn't in our list then it must have
	// fired up after we forked, so we'll need to connect back to the main
	// listener process and ask it for the pid
	return requestFixup(connectionpid,connectionsock);
}

bool sqlrlistener::requestFixup(uint32_t connectionpid,
					filedescriptor *connectionsock) {

	#ifdef SERVER_DEBUG
	debugPrint("listener",0,
			"requesting socket of newly spawned connection...");
	#endif

	// connect to the fixup socket of the parent listener
	unixclientsocket	fixupclientsockun;
	if (fixupclientsockun.connect(fixupsockname,-1,-1,0,1)
						!=RESULT_SUCCESS) {
		#ifdef SERVER_DEBUG
		debugPrint("listener",0,
			"failed to connect to parent listener process");
		#endif
		return false;
	}

	// send the pid of the connection that we need
	if (fixupclientsockun.write(connectionpid)!=sizeof(uint32_t)) {
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

void sqlrlistener::waitForClientClose(int32_t authstatus, bool passstatus,
						filedescriptor *clientsock) {

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

	uint16_t	dummy;
	if (!passdescriptor || (passdescriptor && authstatus<1)) {

		// If we're not passing descriptors or if
		// we are but authentication failed, the client
		// shouldn't be sending any data, so a single
		// read should suffice.
		clientsock->read(&dummy,idleclienttimeout,0);

	} else if (!passstatus) {

		// If the descriptor pass failed, the client
		// cound send an entire query and bind vars
		// before it reads the error and closes the
		// socket.  We have to absorb all of that
		// data.  We shouldn't just loop forever
		// though, that would provide a point of entry
		// for a DOS attack.  We'll read the maximum
		// number of bytes that could be sent.

		uint32_t	counter=0;
		clientsock->useNonBlockingMode();
		while (clientsock->read(&dummy,idleclienttimeout,0)>0 &&
				counter<
				// sending auth
				(sizeof(uint16_t)+
				// user/password
				2*(sizeof(uint32_t)+USERSIZE)+
				// sending query
				sizeof(uint16_t)+
				// need a cursor
				sizeof(uint16_t)+
				// executing new query
				sizeof(uint16_t)+
				// query size and query
				sizeof(uint32_t)+maxquerysize+
				// input bind var count
				sizeof(uint16_t)+
				// input bind vars
				MAXVAR*(2*sizeof(uint16_t)+BINDVARLENGTH)+
				// output bind var count
				sizeof(uint16_t)+
				// output bind vars
				MAXVAR*(2*sizeof(uint16_t)+BINDVARLENGTH)+
				// get column info
				sizeof(uint16_t)+
				// skip/fetch
				2*sizeof(uint32_t)
				// divide by two because we're
				// reading 2 bytes at a time
				)/2) {
			counter++;
		}
		clientsock->useBlockingMode();
	}
}

void sqlrlistener::flushWriteBuffer(filedescriptor *fd) {
	fd->flushWriteBuffer(-1,-1);
}
