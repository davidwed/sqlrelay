// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrlistener.h>

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
#include <rudiments/datetime.h>
#include <rudiments/logger.h>

// for printf
#include <stdio.h>

#ifdef RUDIMENTS_NAMESPACE
using namespace rudiments;
#endif

signalhandler		sqlrlistener::alarmhandler;
volatile sig_atomic_t	sqlrlistener::alarmrang=0;

signalhandler		sqlrlistener::sigusr1handler;
volatile sig_atomic_t	sqlrlistener::gotsigusr1=0;

sqlrlistener::sqlrlistener() : daemonprocess(), listener() {

	cmdl=NULL;

	init=false;

	authc=NULL;

	semset=NULL;
	idmemory=NULL;
	shm=NULL;

	pidfile=NULL;
	tmpdir=NULL;

	clientsockin=NULL;
	clientsockincount=0;
	clientsockun=NULL;
	unixport=NULL;

	mysqlclientsockin=NULL;
	mysqlclientsockincount=0;
	mysqlclientsockun=NULL;
	mysqlunixport=NULL;

	handoffsockun=NULL;
	removehandoffsockun=NULL;
	fixupsockun=NULL;
	fixupsockname=NULL;

	handoffsocklist=NULL;

	denied=NULL;
	allowed=NULL;

	maxquerysize=0;
	maxbindcount=0;
	maxbindnamelength=0;
	idleclienttimeout=-1;

	isforkedchild=false;
	handoffmode=HANDOFF_PASS;
}

sqlrlistener::~sqlrlistener() {
	delete semset;
	delete idmemory;
	if (!isforkedchild) {
		if (unixport) {
			file::remove(unixport);
		}
		if (pidfile) {
			file::remove(pidfile);
		}
	}
	if (init) {
		cleanUp();
	}
	delete cmdl;
}

void sqlrlistener::cleanUp() {

	delete authc;
	delete[] unixport;
	delete[] pidfile;
	delete clientsockun;
	for (uint64_t index=0; index<clientsockincount; index++) {
		delete clientsockin[index];
	}
	delete[] clientsockin;
	delete mysqlclientsockun;
	for (uint64_t index=0; index<mysqlclientsockincount; index++) {
		delete mysqlclientsockin[index];
	}
	delete[] mysqlclientsockin;
	delete handoffsockun;

	if (handoffsocklist) {
		for (uint32_t i=0; i<maxconnections; i++) {
			delete handoffsocklist[i].sock;
		}
		delete[] handoffsocklist;
	}

	delete removehandoffsockun;
	delete[] fixupsockname;
	delete fixupsockun;
	delete denied;
	delete allowed;

	delete tmpdir;
}

bool sqlrlistener::initListener(int argc, const char **argv) {

	init=true;

	cmdl=new cmdline(argc,argv);

	tmpdir=new tempdir(cmdl);

	if (!charstring::compare(cmdl->getId(),DEFAULT_ID)) {
		fprintf(stderr,"Warning: using default id.\n");
	}

	if (!cfgfl.parse(cmdl->getConfig(),cmdl->getId())) {
		return false;
	}

	setUserAndGroup();

	if (!verifyAccessToConfigFile(cmdl->getConfig())) {
		return false;
	}

	if (!handlePidFile(cmdl->getId())) {
		return false;
	}

	handleDynamicScaling();

	const char	*loggers=cfgfl.getLoggers();
	if (charstring::length(loggers)) {
		sqlrlg=new sqlrloggers;
		sqlrlg->loadLoggers(loggers);
		sqlrlg->initLoggers(this,NULL);
	}

	setHandoffMethod(cmdl->getId());

	setIpPermissions();

	if (!createSharedMemoryAndSemaphores(cmdl->getId())) {
		return false;
	}

	if (!listenOnHandoffSocket(cmdl->getId())) {
		return false;
	}
	if (!listenOnDeregistrationSocket(cmdl->getId())) {
		return false;
	}
	if (!listenOnFixupSocket(cmdl->getId())) {
		return false;
	}

	idleclienttimeout=cfgfl.getIdleClientTimeout();
	maxquerysize=cfgfl.getMaxQuerySize();
	maxbindcount=cfgfl.getMaxBindCount();
	maxbindnamelength=cfgfl.getMaxBindNameLength();
	maxlisteners=cfgfl.getMaxListeners();
	listenertimeout=cfgfl.getListenerTimeout();

	detach();

	createPidFile(pidfile,permissions::ownerReadWrite());

	setMaxListeners(maxlisteners);

	return true;
}

void sqlrlistener::setUserAndGroup() {

	// get the user that we're currently running as
	char	*currentuser=NULL;
	passwdentry::getName(process::getEffectiveUserId(),&currentuser);

	// get the group that we're currently running as
	char	*currentgroup=NULL;
	groupentry::getName(process::getEffectiveGroupId(),&currentgroup);

	// switch groups, but only if we're not currently running as the
	// group that we should switch to
	if (charstring::compare(currentgroup,cfgfl.getRunAsGroup()) &&
					!runAsGroup(cfgfl.getRunAsGroup())) {
		fprintf(stderr,"Warning: could not change group to %s\n",
						cfgfl.getRunAsGroup());
	}

	// switch users, but only if we're not currently running as the
	// user that we should switch to
	if (charstring::compare(currentuser,cfgfl.getRunAsUser()) &&
					!runAsUser(cfgfl.getRunAsUser())) {
		fprintf(stderr,"Warning: could not change user to %s\n",
						cfgfl.getRunAsUser());
	}

	// clean up
	delete[] currentuser;
	delete[] currentgroup;
}

bool sqlrlistener::verifyAccessToConfigFile(const char *configfile) {

	if (!cfgfl.getDynamicScaling()) {
		return true;
	}

	file	test;
	if (!test.open(configfile,O_RDONLY)) {
		fprintf(stderr,"\nsqlr-listener error:\n");
		fprintf(stderr,"	This instance of SQL Relay is ");
		fprintf(stderr,"configured to run as:\n");
		fprintf(stderr,"		user: %s\n",
						cfgfl.getRunAsUser());
		fprintf(stderr,"		group: %s\n\n",
						cfgfl.getRunAsGroup());
		fprintf(stderr,"	However, the config file %s\n",
								configfile);
		fprintf(stderr,"	cannot be read by that user ");
		fprintf(stderr,"or group.\n\n");
		fprintf(stderr,"	Since you're using dynamic scaling ");
		fprintf(stderr,"(ie. maxconnections>connections),\n");
		fprintf(stderr,"	new connections would be started as\n");
		fprintf(stderr,"		user: %s\n",
						cfgfl.getRunAsUser());
		fprintf(stderr,"		group: %s\n\n",
						cfgfl.getRunAsGroup());
		fprintf(stderr,"	They would not be able to read the");
		fprintf(stderr,"config file and would shut down.\n\n");
		fprintf(stderr,"	To remedy this problem, make %s\n",
								configfile);
		fprintf(stderr,"	readable by\n");
		fprintf(stderr,"		user: %s\n",
						cfgfl.getRunAsUser());
		fprintf(stderr,"		group: %s\n",
						cfgfl.getRunAsGroup());
		return false;
	}
	test.close();
	return true;
}

bool sqlrlistener::handlePidFile(const char *id) {

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
	return true;
}

void sqlrlistener::handleDynamicScaling() {

	// get the dynamic connection scaling parameters
	maxconnections=cfgfl.getMaxConnections();

	// if dynamic scaling isn't going to be used, disable it
	dynamicscaling=cfgfl.getDynamicScaling();
}

void sqlrlistener::setHandoffMethod(const char *id) {

	// Fork a child which will listen on a unix socket while the main
	// process attempts to send it a file descriptor.  If this succeeds
	// then we support file descriptor passing, if it fails, we do not.
	stringbuffer	testsockname;
	testsockname.append(tmpdir->getString());
	testsockname.append("/sockets/")->append(id)->append("-test");
	handoffmode=HANDOFF_PROXY;
	pid_t	childpid;
	if (!(childpid=process::fork())) {
		// the child process...
		unixserversocket	us;
		us.listen(testsockname.getString(),0000,15);
		filedescriptor	*fd=us.accept();
		int32_t	desc;
		fd->receiveFileDescriptor(&desc);
		delete fd;
		process::exit(0);
	} else if (childpid>0) {
		unixclientsocket	uc;
		// the child process might take a bit to get fired up,
		// retry 5 times, waiting 1 second in between each
		if (uc.connect(testsockname.getString(),-1,-1,1,5) &&
					uc.passFileDescriptor(0)) {
			handoffmode=HANDOFF_PASS;
		} else {
			signalmanager::sendSignal(childpid,SIGTERM);
		}
	}

	// warn if the test failed
	if (handoffmode==HANDOFF_PROXY) {
		fprintf(stderr,"Warning: file descriptor passing "
				"not supported on this platform\n");
	}

	// create the list of handoff nodes
	handoffsocklist=new handoffsocketnode[maxconnections];
	for (uint32_t i=0; i<maxconnections; i++) {
		handoffsocklist[i].pid=0;
		handoffsocklist[i].sock=NULL;
	}
}

void sqlrlistener::setIpPermissions() {

	// get denied and allowed ip's and compile the expressions
	const char	*deniedips=cfgfl.getDeniedIps();
	const char	*allowedips=cfgfl.getAllowedIps();
	if (deniedips[0]) {
		denied=new regularexpression(deniedips);
	}
	if (allowedips[0]) {
		allowed=new regularexpression(allowedips);
	}
}

bool sqlrlistener::createSharedMemoryAndSemaphores(const char *id) {

	// initialize the ipc filename
	size_t	idfilenamelen=tmpdir->getLength()+5+charstring::length(id)+1;
	char	*idfilename=new char[idfilenamelen];
	snprintf(idfilename,idfilenamelen,"%s/ipc/%s",tmpdir->getString(),id);

	stringbuffer	info;
	info.append("creating shared memory and semaphores: id filename: ");
	info.append(idfilename);
	logDebugMessage(info.getString());

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
	logDebugMessage("creating shared memory...");

	idmemory=new sharedmemory;
	if (!idmemory->create(key,sizeof(shmdata),
				permissions::evalPermString("rw-r-----"))) {
		idmemory->attach(key);
		shmError(id,idmemory->getId());
		return false;
	}
	shm=(shmdata *)idmemory->getPointer();
	rawbuffer::zero(shm,sizeof(shmdata));

	setStartTime();

	// create (or connect) to the semaphore set
	// FIXME: if it already exists, attempt to remove and re-create it
	logDebugMessage("creating semaphores...");

	// semaphores are:
	//
	// "connection count" - number of open database connections
	// "connected client count" - number of clients currently connected
	//
	// 0 - connection: connection registration mutex
	// 1 - listener:   connection registration mutex
	//
	// connection/listener registration interlocks:
	// 2 - connection/listener: ensures that it's safe for a listener to
	//			  read a registration
	//       listener waits for a connection to register itself
	//       connection signals when it's done registering
	// 3 - connection/listener: ensures that it's safe for a connection to
	//			  register itself
	//       connection waits for the listener to read its registration
	//       listener signals when it's done reading a registration
	//
	// connection/lisetner/scaler interlocks:
	// 6 - scaler/listener: used to decide whether to scale or not
	//       listener signals after incrementing connected client count
	//       scaler waits before counting sessions/connections
	// 4 - connection/scaler: connection count mutex
	//       connection increases/decreases connection count
	//       scalar reads connection count
	// 5 - connection/listener: connected client count mutex
	//       listener increases connected client count when
	//       	a client connects
	//       connection decreases connected client count when
	//       	a client disconnects
	// 7 - scaler/listener: used to decide whether to scale or not
	//       scaler signals after counting sessions/connections
	//       listener waits for scaler to count sessions/connections
	// 8 - scaler/connection:
	//       scaler waits for the connection count to increase
	//		 (in effect, waiting for a new connection to fire up)
	//       connection signals after increasing connection count
	//
	// statistics:
	// 9 - coordinates access to statistics shared memory segment
	//
	// main listenter process/listener children:
	// 10 - listener: number of busy listeners
	//
	int32_t	vals[11]={1,1,0,0,1,1,0,0,0,1,0};
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
	char	*err=error::getErrorString();
	fprintf(stderr,"\nsqlr-listener error:\n");
	fprintf(stderr,"	Unable to generate a key from ");
	fprintf(stderr,"%s\n",idfilename);
	fprintf(stderr,"	Error was: %s\n\n",err);
	delete[] err;
}

void sqlrlistener::shmError(const char *id, int shmid) {
	char	*err=error::getErrorString();
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
	fprintf(stderr,"	Error was: %s\n\n",err);
	delete[] err;
}

void sqlrlistener::semError(const char *id, int semid) {
	char	*err=error::getErrorString();
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
	fprintf(stderr,"	Error was: %s\n\n",err);
	delete[] err;
}

bool sqlrlistener::listenOnClientSockets() {

	// get addresses/inet port and unix port to
	// listen on for the SQL Relay protocol
	const char * const *addresses=cfgfl.getAddresses();
	clientsockincount=cfgfl.getAddressCount();
	uint16_t	port=cfgfl.getPort();
	if (!port) {
		clientsockincount=0;
	}
	unixport=charstring::duplicate(cfgfl.getUnixPort());

	// attempt to listen on the SQL Relay inet ports
	// (on each specified address), if necessary
	bool	listening=false;
	if (port && clientsockincount) {
		clientsockin=new inetserversocket *[clientsockincount];
		bool	failed=false;
		for (uint64_t index=0; index<clientsockincount; index++) {
			clientsockin[index]=NULL;
			if (failed) {
				continue;
			}
			clientsockin[index]=new inetserversocket();
			listening=clientsockin[index]->
					listen(addresses[index],port,15);
			if (listening) {
				addFileDescriptor(clientsockin[index]);
			} else {
				stringbuffer	info;
				info.append("failed to listen "
						"on client port: ");
				info.append(port);
				logInternalError(info.getString());

				char	*err=error::getErrorString();
				fprintf(stderr,
					"Could not listen "
					"on: %s/%d\n"
					"Error was: %s\n"
					"Make sure that no other "
					"processes are listening "
					"on that port.\n\n",
					addresses[index],port,err);
				delete[] err;
				failed=true;
			}
		}
	}

	if (charstring::length(unixport)) {
		clientsockun=new unixserversocket();
		listening=clientsockun->listen(unixport,0000,15);
		if (listening) {
			addFileDescriptor(clientsockun);
		} else {
			stringbuffer	info;
			info.append("failed to listen on client socket: ");
			info.append(unixport);
			logInternalError(info.getString());

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

	// get addresses/inet port and unix port to
	// listen on for the MySQL protocol
	const char * const *mysqladdresses=cfgfl.getMySQLAddresses();
	mysqlclientsockincount=cfgfl.getMySQLAddressCount();
	uint16_t	mysqlport=cfgfl.getMySQLPort();
	if (!mysqlport) {
		mysqlclientsockincount=0;
	}
	mysqlunixport=charstring::duplicate(cfgfl.getMySQLUnixPort());

	// attempt to listen on the MySQL inet ports
	// (on each specified address), if necessary
	if (mysqlport && mysqlclientsockincount) {
		mysqlclientsockin=
			new inetserversocket *[mysqlclientsockincount];
		bool	failed=false;
		for (uint64_t index=0; index<mysqlclientsockincount; index++) {
			mysqlclientsockin[index]=NULL;
			if (failed) {
				continue;
			}
			mysqlclientsockin[index]=new inetserversocket();
			listening=mysqlclientsockin[index]->listen(
							mysqladdresses[index],
							mysqlport,15);
			if (listening) {
				addFileDescriptor(mysqlclientsockin[index]);
			} else {
				stringbuffer	info;
				info.append("failed to listen "
						"on client port: ");
				info.append(mysqlport);
				logInternalError(info.getString());

				char	*err=error::getErrorString();
				fprintf(stderr,
					"Could not listen "
					"on: %s/%d\n"
					"Error was: %s\n"
					"Make sure that no other "
					"processes are listening "
					"on that port.\n\n",
					mysqladdresses[index],mysqlport,err);
				delete[] err;
				failed=true;
			}
		}
	}

	if (charstring::length(mysqlunixport)) {
		mysqlclientsockun=new unixserversocket();
		listening=mysqlclientsockun->listen(mysqlunixport,0000,15);
		if (listening) {
			addFileDescriptor(mysqlclientsockun);
		} else {
			stringbuffer	info;
			info.append("failed to listen on client socket: ");
			info.append(mysqlunixport);
			logInternalError(info.getString());

			fprintf(stderr,"Could not listen on unix socket: ");
			fprintf(stderr,"%s\n",mysqlunixport);
			fprintf(stderr,"Make sure that the file and ");
			fprintf(stderr,"directory are readable and writable.");
			fprintf(stderr,"\n\n");
			delete mysqlclientsockun;
			mysqlclientsockun=NULL;
			delete[] mysqlunixport;
			mysqlunixport=NULL;
		}
	}

	return listening;
}

bool sqlrlistener::listenOnHandoffSocket(const char *id) {

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
		stringbuffer	info;
		info.append("failed to listen on handoff socket: ");
		info.append(handoffsockname);
		logInternalError(info.getString());

		fprintf(stderr,"Could not listen on unix socket: ");
		fprintf(stderr,"%s\n",handoffsockname);
		fprintf(stderr,"Make sure that the file and ");
		fprintf(stderr,"directory are readable and writable.");
		fprintf(stderr,"\n\n");
	}

	delete[] handoffsockname;
	return success;
}

bool sqlrlistener::listenOnDeregistrationSocket(const char *id) {

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
		stringbuffer	info;
		info.append("failed to listen on deregistration socket: ");
		info.append(removehandoffsockname);
		logInternalError(info.getString());

		fprintf(stderr,"Could not listen on unix socket: ");
		fprintf(stderr,"%s\n",removehandoffsockname);
		fprintf(stderr,"Make sure that the file and ");
		fprintf(stderr,"directory are readable and writable.");
		fprintf(stderr,"\n\n");
	}

	delete[] removehandoffsockname;

	return success;
}

bool sqlrlistener::listenOnFixupSocket(const char *id) {

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
		stringbuffer	info;
		info.append("failed to listen on fixup socket: ");
		info.append(fixupsockname);
		logInternalError(info.getString());

		fprintf(stderr,"Could not listen on unix socket: ");
		fprintf(stderr,"%s\n",fixupsockname);
		fprintf(stderr,"Make sure that the file and ");
		fprintf(stderr,"directory are readable and writable.");
		fprintf(stderr,"\n\n");
	}

	return success;
}

void sqlrlistener::listen() {

	// wait until all of the connections have started
	for (;;) {
		int32_t	opendbconnections=shm->open_db_connections;

		if (opendbconnections<
			static_cast<int32_t>(cfgfl.getConnections())) {
			logDebugMessage("waiting for server "
					"connections (sleeping 1s)");
			snooze::macrosnooze(1);
		} else {
			logDebugMessage("finished waiting for "
					"server connections");
			break;
		}
	}

	// listen for client connections
	if (!listenOnClientSockets()) {
		return;
	}

	for(;;) {
		// FIXME: this can return true/false, should we do anything
		// with that?
		handleClientConnection(waitForData());
	}
}

void sqlrlistener::handleSignals(void (*shutdownfunction)(int32_t)) {

	// handle SIGUSR1
	sigusr1handler.setHandler(sigUsr1Handler);
	sigusr1handler.handleSignal(SIGUSR1);

	// handle kill (SIGINT, SIGTERM) and crash (SIGSEGV) signals
	handleShutDown(shutdownfunction);
	handleCrash(shutdownfunction);

	// set a handler for SIGALRM's
	alarmhandler.setHandler(alarmHandler);
	alarmhandler.handleSignal(SIGALRM);

	// the daemon class handles SIGTERM's and SIGINT's and SIGCHLDS
	// and we catch SIGALRM's for timeouts but we're going to block
	// all other signals
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

	signalmanager::ignoreSignals(&set);
}

filedescriptor *sqlrlistener::waitForData() {

	logDebugMessage("waiting for client connection...");

	// wait for data on one of the sockets...
	// if something bad happened, return an invalid file descriptor
	if (listener::waitForNonBlockingRead(-1,-1)<1) {
		return NULL;
	}

	// return first file descriptor that had data available or an invalid
	// file descriptor on error
	filedescriptor	*fd=NULL;
	listener::getReadyList()->getDataByIndex(0,&fd);

	logDebugMessage("finished waiting for client connection");

	return fd;
}


bool sqlrlistener::handleClientConnection(filedescriptor *fd) {

	if (!fd) {
		return false;
	}

	// If something connected to the handoff or deregistration
	// socket, it must have been a connection.
	//
	// If something connected to the fixup socket, it must have been a
	// forked off listener looking for the file descriptor of a socket
	// associated with a newly spawned connection daemon.
	//
	// Either way, handle it and loop back.
	filedescriptor	*clientsock;
	if (fd==handoffsockun) {
		clientsock=handoffsockun->accept();
		clientsock->dontUseNaglesAlgorithm();
		return registerHandoff(clientsock);
	} else if (fd==removehandoffsockun) {
		clientsock=removehandoffsockun->accept();
		clientsock->dontUseNaglesAlgorithm();
		return deRegisterHandoff(clientsock);
	} else if (fd==fixupsockun) {
		clientsock=fixupsockun->accept();
		clientsock->dontUseNaglesAlgorithm();
		return fixup(clientsock);
	}

	// handle connections to the client sockets
	inetserversocket	*iss=NULL;
	for (uint64_t index=0; index<clientsockincount; index++) {
		if (fd==clientsockin[index]) {
			iss=clientsockin[index];
		}
	}
	for (uint64_t index=0; index<mysqlclientsockincount; index++) {
		if (fd==mysqlclientsockin[index]) {
			iss=mysqlclientsockin[index];
		}
	}
	if (iss) {

		clientsock=iss->accept();
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
	} else if (fd==mysqlclientsockun) {
		clientsock=mysqlclientsockun->accept();
		clientsock->translateByteOrder();
	} else {
		return true;
	}

	clientsock->dontUseNaglesAlgorithm();
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
	if (dynamicscaling || getBusyListeners() || !semset->getValue(2)) {
		forkChild(clientsock);
	} else {
		incrementBusyListeners();
		clientSession(clientsock);
		decrementBusyListeners();
	}

	return true;
}


bool sqlrlistener::registerHandoff(filedescriptor *sock) {

	logDebugMessage("registering handoff...");

	// get the connection daemon's pid
	uint32_t processid;
	if (sock->read(&processid)!=sizeof(uint32_t)) {
		logInternalError("failed to read process "
					"id during registration");
		delete sock;
		return false;
	}

	// find a free node in the list, if we find another node with the
	// same pid, then the old connection must have died off mysteriously,
	// replace it
	bool		inserted=false;
	uint32_t	index=0;
	for (; index<maxconnections; index++) {
		if (!handoffsocklist[index].pid ||
			handoffsocklist[index].pid==processid) {
			handoffsocklist[index].pid=processid;
			handoffsocklist[index].sock=sock;
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
		for (uint32_t i=0; i<maxconnections; i++) {
			newhandoffsocklist[i].pid=handoffsocklist[i].pid;
			newhandoffsocklist[i].sock=handoffsocklist[i].sock;
		}
		delete[] handoffsocklist;
		newhandoffsocklist[maxconnections].pid=processid;
		newhandoffsocklist[maxconnections].sock=sock;
		maxconnections++;
		handoffsocklist=newhandoffsocklist;
	}

	logDebugMessage("finished registering handoff...");
	return true;
}

bool sqlrlistener::deRegisterHandoff(filedescriptor *sock) {

	logDebugMessage("de-registering handoff...");

	// get the connection daemon's pid
	uint32_t	processid;
	if (sock->read(&processid)!=sizeof(uint32_t)) {
		logInternalError("failed to read process "
				"id during deregistration");
		delete sock;
		return false;
	}

	// remove the matching socket from the list
	for (uint32_t i=0; i<maxconnections; i++) {
		if (handoffsocklist[i].pid==processid) {
			handoffsocklist[i].pid=0;
			delete handoffsocklist[i].sock;
			handoffsocklist[i].sock=NULL;
			break;
		}
	}

	// clean up
	delete sock;

	logDebugMessage("finished de-registering handoff...");
	return true;
}

bool sqlrlistener::fixup(filedescriptor *sock) {

	logDebugMessage("passing socket of newly spawned connection...");

	// get the pid of the connection daemon the child listener needs
	uint32_t	processid;
	if (sock->read(&processid)!=sizeof(uint32_t)) {
		logInternalError("failed to read process id during fixup");
		delete sock;
		return false;
	}

	// look through the handoffsocklist for the pid
	bool	retval=false;
	for (uint32_t i=0; i<maxconnections; i++) {
		if (handoffsocklist[i].pid==processid) {
			retval=sock->passFileDescriptor(handoffsocklist[i].
						sock->getFileDescriptor());
			logDebugMessage("found socket for requested pid ");
			if (retval) {
				logDebugMessage("passed it successfully");
			} else {
				logDebugMessage("failed to pass it");
			}
			break;
		}
	}

	// clean up
	delete sock;

	logDebugMessage("finished passing socket of newly spawned connection");

	return retval;
}

bool sqlrlistener::deniedIp(filedescriptor *clientsock) {

	logDebugMessage("checking for valid ip...");

	char	*ip=clientsock->getPeerAddress();
	if (ip && denied->match(ip) &&
			(!allowed || (allowed && !allowed->match(ip)))) {

		stringbuffer	info;
		info.append("rejected IP address: ")->append(ip);
		logClientConnectionRefused(info.getString());

		delete[] ip;
		return true;
	}

	logDebugMessage("valid ip...");

	delete[] ip;
	return false;
}

void sqlrlistener::errorClientSession(filedescriptor *clientsock,
							int64_t errnum,
							const char *err) {
	// get auth and ignore the result
	clientsock->write((uint16_t)ERROR_OCCURRED);
	clientsock->write((uint64_t)errnum);
	clientsock->write((uint16_t)charstring::length(err));
	clientsock->write(err);
	flushWriteBuffer(clientsock);
	// FIXME: hmm, if the client is just spewing
	// garbage then we should close the connection...
	waitForClientClose(false,clientsock);
	delete clientsock;
}

void sqlrlistener::forkChild(filedescriptor *clientsock) {

	// increment the number of "forked listeners"
	// do this before we actually fork to prevent a race condition where
	// a bunch of children get forked off before any of them get a chance
	// to increment this and prevent more from getting forked off
	incrementBusyListeners();
	int32_t	forkedlisteners=incrementForkedListeners();

	// if we already have too many listeners running,
	// bail and return an error to the client
	if (maxlisteners>-1 && forkedlisteners>maxlisteners) {

		// since we've decided not to fork, decrement the counters
		decrementBusyListeners();
		decrementForkedListeners();
		incrementMaxListenersErrors();
		errorClientSession(clientsock,
				SQLR_ERROR_TOOMANYLISTENERS,
				SQLR_ERROR_TOOMANYLISTENERS_STRING);
		return;
	}

	// if the client connected to one of the non-handoff
	// sockets, fork a child to handle it
	pid_t	childpid;
	if (!(childpid=process::fork())) {

		isforkedchild=true;

		// since this is the forked off listener, we don't
		// want to actually remove the semaphore set or shared
		// memory segment when it exits
		idmemory->dontRemove();
		semset->dontRemove();

		// re-init loggers
		if (sqlrlg) {
			sqlrlg->initLoggers(this,NULL);
		}

		clientSession(clientsock);

		decrementBusyListeners();
		decrementForkedListeners();

		cleanUp();
		process::exit(0);
	} else if (childpid>0) {
		// parent
		char	debugstring[22];
		snprintf(debugstring,22,"forked a child: %ld",(long)childpid);
		logDebugMessage(debugstring);
		// the main process doesn't need to stay connected
		// to the client, only the forked process
		delete clientsock;
	} else {
		// error
		decrementBusyListeners();
		decrementForkedListeners();
		errorClientSession(clientsock,
				SQLR_ERROR_ERRORFORKINGLISTENER,
				SQLR_ERROR_ERRORFORKINGLISTENER_STRING);
		logInternalError(SQLR_ERROR_ERRORFORKINGLISTENER_STRING);
	}
}

void sqlrlistener::clientSession(filedescriptor *clientsock) {

	if (dynamicscaling) {
		incrementConnectedClientCount();
	}

	bool	passstatus=handOffOrProxyClient(clientsock);

	// If the handoff failed, decrement the connected client count.
	// If it had succeeded then the connection daemon would
	// decrement it later.
	if (dynamicscaling && !passstatus) {
		decrementConnectedClientCount();
	}

	// FIXME: hmm, if the client is just spewing
	// garbage then we should close the connection...
	waitForClientClose(passstatus,clientsock);
}

bool sqlrlistener::handOffOrProxyClient(filedescriptor *sock) {

	uint32_t	connectionpid;
	uint16_t	inetport;
	char 		unixportstr[MAXPATHLEN+1];
	uint16_t	unixportstrlen;
	bool		retval=false;

	// loop in case client doesn't get handed off successfully
	for (;;) {

		if (!getAConnection(&connectionpid,&inetport,
					unixportstr,&unixportstrlen,
					sock)) {
			// fatal error occurred while getting a connection
			retval=false;
			break;
		}

		// Get the socket associated with the pid of the
		// available connection.
		filedescriptor	connectionsock;
		if (!findMatchingSocket(connectionpid,&connectionsock)) {
			// FIXME: should there be a limit to the number
			// of times we retry?
			continue;
		}

		// Pass the client to the connection or proxy it.
		// If any of this fails, the connection may have crashed or
		// been killed.  Loop back and get another connection...

		// tell the connection what handoff mode to expect
		connectionsock.write(handoffmode);

		if (handoffmode==HANDOFF_PASS) {

			// pass the file descriptor
			if (!connectionsock.passFileDescriptor(
					sock->getFileDescriptor())) {
				logInternalError("failed to pass "
						"file descriptor");
				// FIXME: should there be a limit to the number
				// of times we retry?
				continue;
			}

		} else {

			// proxy the client
			if (!proxyClient(connectionpid,&connectionsock,sock)) {
				logInternalError("failed to proxy client");
				// FIXME: should there be a limit to the number
				// of times we retry?
				continue;
			}
		}

		// If we got this far, everything worked.
		retval=true;
		
		// Set the file descriptor to -1, otherwise it will get closed
		// when connectionsock is freed.  If the file descriptor gets
		// closed, the next time we try to pass a file descriptor to
		// the same connection, it will fail.
		connectionsock.setFileDescriptor(-1);
		break;
	}

	return retval;
}

bool sqlrlistener::acquireShmAccess() {

	logDebugMessage("acquiring exclusive shm access");

	// don't even begin to wait if the alarm already rang
	if (alarmrang) {
		logDebugMessage("timeout occured");
		return false;
	}

	// Loop, waiting.  Retry the wait if it was interrupted by a signal,
	// other than an alarm, but if an alarm interrupted it then bail.
	bool	result=true;
	semset->dontRetryInterruptedOperations();
	do {
		result=semset->waitWithUndo(1);
	} while (!result && error::getErrorNumber()==EINTR && alarmrang!=1);
	semset->retryInterruptedOperations();

	// handle alarm...
	if (alarmrang) {
		logDebugMessage("timeout occured");
		return false;
	}

	// handle general failure...
	if (!result) {
		logDebugMessage("failed to acquire exclusive shm access");
		return false;
	}

	// success...
	logDebugMessage("acquired exclusive shm access");
	return true;
}

bool sqlrlistener::releaseShmAccess() {

	logDebugMessage("releasing exclusive shm access");

	if (!semset->signalWithUndo(1)) {
		logDebugMessage("failed to release exclusive shm access");
		return false;
	}

	logDebugMessage("finished releasing exclusive shm access");
	return true;
}

bool sqlrlistener::acceptAvailableConnection(bool *alldbsdown) {

	// If we don't want to wait for down databases, then check to see if
	// any of the db's are up.  If none are, then don't even wait for an
	// available connection, just bail immediately.
	if (!cfgfl.getWaitForDownDatabase()) {
		*alldbsdown=true;
		linkedlist< connectstringcontainer * >	*csl=
					cfgfl.getConnectStringList();
		for (linkedlistnode< connectstringcontainer * > *node=
						csl->getFirstNode(); node;
						node=node->getNext()) {
			connectstringcontainer	*cs=node->getData();
			if (connectionIsUp(cs->getConnectionId())) {
				*alldbsdown=false;
				break;
			}
		}
		if (*alldbsdown) {
			return false;
		}
	}

	logDebugMessage("waiting for an available connection");

	// don't even begin to wait if the alarm already rang
	if (alarmrang) {
		logDebugMessage("timeout occured");
		return false;
	}

	// Loop, waiting.  Retry the wait if it was interrupted by a signal,
	// other than an alarm, but if an alarm interrupted it then bail.
	bool	result=true;
	semset->dontRetryInterruptedOperations();
	do {
		result=semset->wait(2);
	} while (!result && error::getErrorNumber()==EINTR && alarmrang!=1);
	semset->retryInterruptedOperations();

	// handle alarm...
	if (alarmrang) {
		logDebugMessage("timeout occured");
		return false;
	}

	// handle general failure...
	if (!result) {
		logInternalError("general failure waiting "
					"for available connection");
		return false;
	}

	// success...

	// Reset this semaphore to 0.
	// It can get left incremented if a sqlr-connection process is killed
	// between calls to signalListenerToRead() and
	// waitForListenerToFinishReading().  It's ok to reset it here because
	// no one except this process has access to this semaphore at this time
	// because of the lock on semaphore 1.
	semset->setValue(2,0);

	logDebugMessage("succeeded in waiting for an available connection");
	return true;
}

bool sqlrlistener::doneAcceptingAvailableConnection() {

	logDebugMessage("signalling accepted connection");

	if (!semset->signal(3)) {
		logDebugMessage("failed to signal accapted connection");
		return false;
	}

	logDebugMessage("succeeded signalling accepted connection");
	return true;
}

bool sqlrlistener::getAConnection(uint32_t *connectionpid,
					uint16_t *inetport,
					char *unixportstr,
					uint16_t *unixportstrlen,
					filedescriptor *sock) {

	// use an alarm, if we're a forked child and a timeout is set
	bool	usealarm=(isforkedchild && listenertimeout);

	for (;;) {

		logDebugMessage("getting a connection...");

		// set an alarm
		if (usealarm) {
			alarmrang=0;
			signalmanager::alarm(listenertimeout);
		}

		// set "all db's down" flag
		bool	alldbsdown=false;

		// acquire access to the shared memory
		bool	ok=acquireShmAccess();

		if (ok) {

			// This section should be executed without returns or
			// breaks so that releaseShmAccess will get called
			// at the end, no matter what...

			// wait for an available connection
			ok=acceptAvailableConnection(&alldbsdown);

			// turn off the alarm
			if (usealarm) {
				signalmanager::alarm(0);
			}

			if (ok) {

				// This section should be executed without
				// returns or breaks so that
				// signalAcceptedConnection will get called at
				// the end, no matter what...

				// get the pid
				*connectionpid=
					shm->connectioninfo.connectionpid;

				// signal the connection that we waited for
				ok=doneAcceptingAvailableConnection();
			}

			// release access to the shared memory
			ok=(releaseShmAccess() && ok);
		}

		// turn off the alarm
		if (usealarm) {
			signalmanager::alarm(0);
		}

		// execute this only if code above executed without errors...
		if (ok) {

			// make sure the connection is actually up...
			if (connectionIsUp(shm->connectionid)) {
				logDebugMessage("finished getting "
						"a connection");
				return true;
			}

			// if the connection wasn't up, fork a child to jog it,
			// spin back and get another connection
			logDebugMessage("connection was down");
			pingDatabase(*connectionpid,unixportstr,*inetport);
		}

		// return an error if the timeout was reached
		if (alarmrang) {
			sock->write((uint16_t)ERROR_OCCURRED);
			sock->write((uint64_t)SQLR_ERROR_HANDOFFFAILED);
			sock->write((uint16_t)charstring::length(
					SQLR_ERROR_HANDOFFFAILED_STRING));
			sock->write(SQLR_ERROR_HANDOFFFAILED_STRING);
			flushWriteBuffer(sock);
			return false;
		}

		// return an error if all db's were down
		if (alldbsdown) {
			sock->write((uint16_t)ERROR_OCCURRED);
			sock->write((uint64_t)SQLR_ERROR_DBSDOWN);
			sock->write((uint16_t)charstring::length(
						SQLR_ERROR_DBSDOWN_STRING));
			sock->write(SQLR_ERROR_DBSDOWN_STRING);
			flushWriteBuffer(sock);
			return false;
		}
	}
}

bool sqlrlistener::connectionIsUp(const char *connectionid) {

	// initialize the database up/down filename
	size_t	updownlen=tmpdir->getLength()+5+
			charstring::length(cmdl->getId())+1+
			charstring::length(connectionid)+1;
	char	*updown=new char[updownlen];
	snprintf(updown,updownlen,"%s/ipc/%s-%s",
			tmpdir->getString(),cmdl->getId(),connectionid);
	bool	retval=file::exists(updown);
	delete[] updown;
	return retval;
}

void sqlrlistener::pingDatabase(uint32_t connectionpid,
					const char *unixportstr,
					uint16_t inetport) {

	// fork off and tell the connection to reconnect to the database
	if (process::fork()) {
		return;
	}

	isforkedchild=true;

	filedescriptor	connectionsock;
	if (findMatchingSocket(connectionpid,&connectionsock)) {
		connectionsock.write((uint16_t)HANDOFF_RECONNECT);
		snooze::macrosnooze(1);
	}

	cleanUp();
	process::exit(0);
}

bool sqlrlistener::findMatchingSocket(uint32_t connectionpid,
					filedescriptor *connectionsock) {

	// Look through the list of handoff sockets for the pid of the 
	// connection that we got during the call to getAConnection().
	// When we find it, send the descriptor of the clientsock to the 
	// connection over the handoff socket associated with that node.
	for (uint32_t i=0; i<maxconnections; i++) {
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

	logDebugMessage("requesting socket of newly spawned connection...");

	// connect to the fixup socket of the parent listener
	unixclientsocket	fixupclientsockun;
	if (fixupclientsockun.connect(fixupsockname,-1,-1,0,1)
						!=RESULT_SUCCESS) {
		logInternalError("fixup failed to connect");
		return false;
	}

	fixupclientsockun.dontUseNaglesAlgorithm();

	// send the pid of the connection that we need
	if (fixupclientsockun.write(connectionpid)!=sizeof(uint32_t)) {
		logInternalError("fixup failed to write pid");
		return false;
	}

	// get the file descriptor of the socket
	int32_t	fd;
	if (!fixupclientsockun.receiveFileDescriptor(&fd)) {
		logInternalError("fixup failed to receive socket");
		return false;
	}
	connectionsock->setFileDescriptor(fd);

	logDebugMessage("received socket of newly spawned connection");
	return true;
}

bool sqlrlistener::proxyClient(pid_t connectionpid,
				filedescriptor *serversock,
				filedescriptor *clientsock) {

	// send the connection our PID
	serversock->write((uint32_t)process::getProcessId());
	serversock->flushWriteBuffer(-1,-1);

	// wait up to 5 seconds for a response
	#define ACK	6
	unsigned char	ack=0;
	if (serversock->read(&ack,5,0)!=sizeof(unsigned char) || ack!=ACK) {
		return false;
	}

	// allow short reads and use non blocking mode
	serversock->allowShortReads();
	serversock->useNonBlockingMode();
	clientsock->allowShortReads();
	clientsock->useNonBlockingMode();

	// Set up a listener to listen on both client and server sockets.
	// Allow waits to be interrupted because the connecton daemon will
	// send us a SIGUSR1 when it's done talking to the client.
	listener	proxy;
	proxy.addFileDescriptor(serversock);
	proxy.addFileDescriptor(clientsock);
	proxy.dontRetryInterruptedWaits();

	// set up a read buffer
	unsigned char	readbuffer[8192];

	// should we send an end session command to the connection?
	bool	endsession=false;

	for (;;) {

		// wait for data to be available from the client or server
		// or for the server to signal that we should bail
		error::clearError();
		int32_t	waitcount=proxy.waitForNonBlockingRead(-1,-1);

		// were we interrupted?
		if (error::getErrorNumber()==EINTR) {

			// if it was a SIGUSR1 then bail
			if (gotsigusr1) {
				break;
			}

			// must have been some other signal,
			// loop back and wait again
			continue;
		}

		// No interrupt, but nobody had data...
		// One of the sockets must have gotten closed.
		// Most likely the client disconnected.
		if (waitcount<1) {
			endsession=true;
			break;
		}

		// get the file descriptor that data was available from
		filedescriptor	*fd=NULL;
		proxy.getReadyList()->getDataByIndex(0,&fd);

		// read whatever data was available
		ssize_t	readcount=fd->read(readbuffer,sizeof(readbuffer));
		if (readcount<1) {
			endsession=(fd==clientsock);
			break;
		}

		// write the data to the other side
		if (fd==serversock) {
			clientsock->write(readbuffer,readcount);
			clientsock->flushWriteBuffer(-1,-1);
		} else if (fd==clientsock) {
			serversock->write(readbuffer,readcount);
			serversock->flushWriteBuffer(-1,-1);
		}
	}

	// send the server an end session command, if neccessary
	if (endsession) {
		// translate byte order for this, as the client would
		serversock->translateByteOrder();
		serversock->write((uint16_t)END_SESSION);
		serversock->dontTranslateByteOrder();
		serversock->flushWriteBuffer(-1,-1);
	}

	// set everything back to normal
	serversock->dontAllowShortReads();
	serversock->useBlockingMode();
	clientsock->dontAllowShortReads();
	clientsock->useBlockingMode();

	return true;
}

void sqlrlistener::waitForClientClose(bool passstatus,
					filedescriptor *clientsock) {

	// Sometimes the listener sends the ports and closes
	// the socket while they are still buffered but not
	// yet transmitted.  This causes the client to receive
	// partial data or an error.  Telling the socket to
	// linger doesn't always fix it.  Doing a read here
	// should guarantee that the client will close its end
	// of the connection before the server closes its end;
	// the server will wait for data from the client
	// (which it will never receive) and when the client
	// closes its end (which it will only do after getting
	// the ports), the read will fall through.  This should
	// guarantee that the client will get the ports without
	// requiring the client to send data back indicating so.

	uint16_t	dummy;
	if (!passstatus) {

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
					maxbindcount*(2*sizeof(uint16_t)+
							maxbindnamelength)+
					// output bind var count
					sizeof(uint16_t)+
					// output bind vars
					maxbindcount*(2*sizeof(uint16_t)+
							maxbindnamelength)+
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

void sqlrlistener::setStartTime() {
	datetime	dt;
	dt.getSystemDateAndTime();
	shm->starttime=dt.getEpoch();
}

void sqlrlistener::setMaxListeners(uint32_t maxlisteners) {
	shm->max_listeners=maxlisteners;
}

void sqlrlistener::incrementMaxListenersErrors() {
	shm->max_listeners_errors++;
}

void sqlrlistener::incrementConnectedClientCount() {

	logDebugMessage("incrementing connected client count...");

	if (!semset->waitWithUndo(5)) {
		// FIXME: bail somehow
	}

	// increment the connections-in-use counter
	shm->connectedclients++;

	// update the peak connections-in-use count
	if (shm->connectedclients>shm->peak_connectedclients) {
		shm->peak_connectedclients=shm->connectedclients;
	}

	// update the peak connections-in-use over the previous minute count
	datetime	dt;
	dt.getSystemDateAndTime();
	if (shm->connectedclients>shm->peak_connectedclients_1min ||
		dt.getEpoch()/60>shm->peak_connectedclients_1min_time/60) {
		shm->peak_connectedclients_1min=shm->connectedclients;
		shm->peak_connectedclients_1min_time=dt.getEpoch();
	}

	// If the system supports timed semaphore ops then the scaler can be
	// jogged into running on-demand, and we can do that here.  If the 
	// sytem does not support timed semaphore ops then the scaler will
	// just loop periodically on its own and we shouldn't attempt to
	// jog it.
	if (semset->supportsTimedSemaphoreOperations()) {

		// signal the scaler to evaluate the connection count
		// and start more connections if necessary
		logDebugMessage("signalling the scaler...");
		if (!semset->signal(6)) {
			// FIXME: bail somehow
		}
		logDebugMessage("finished signalling the scaler...");

		// wait for the scaler
		logDebugMessage("waiting for the scaler...");
		if (!semset->wait(7)) {
			// FIXME: bail somehow
		}
		logDebugMessage("finished waiting for the scaler...");
	}

	if (!semset->signalWithUndo(5)) {
		// FIXME: bail somehow
	}

	logDebugMessage("finished incrementing connected client count");
}

void sqlrlistener::decrementConnectedClientCount() {

	logDebugMessage("decrementing connected client count...");
 
	if (!semset->waitWithUndo(5)) {
		// FIXME: bail somehow
	}

	if (shm->connectedclients) {
		shm->connectedclients--;
	}

	if (!semset->signalWithUndo(5)) {
		// FIXME: bail somehow
	}

	logDebugMessage("finished decrementing connected client count");
}

uint32_t sqlrlistener::incrementForkedListeners() {

	semset->waitWithUndo(9);
	uint32_t	forkedlisteners=++(shm->forked_listeners);
	semset->signalWithUndo(9);
	return forkedlisteners;
}

uint32_t sqlrlistener::decrementForkedListeners() {

	semset->waitWithUndo(9);
	if (shm->forked_listeners) {
		shm->forked_listeners--;
	}
	uint32_t	forkedlisteners=shm->forked_listeners;
	semset->signalWithUndo(9);
	return forkedlisteners;
}

void sqlrlistener::incrementBusyListeners() {

	logDebugMessage("incrementing busy listeners");

	if (!semset->signal(10)) {
		// FIXME: bail somehow
	}

	// update the peak listeners count
	uint32_t	busylisteners=semset->getValue(10);
	if (shm->peak_listeners<busylisteners) {
		shm->peak_listeners=busylisteners;
	}

	// update the peak listeners over the previous minute count
	datetime	dt;
	dt.getSystemDateAndTime();
	if (busylisteners>shm->peak_listeners_1min ||
		dt.getEpoch()/60>shm->peak_listeners_1min_time/60) {
		shm->peak_listeners_1min=busylisteners;
		shm->peak_listeners_1min_time=dt.getEpoch();
	}

	logDebugMessage("finished incrementing busy listeners");
}

void sqlrlistener::decrementBusyListeners() {
	logDebugMessage("decrementing busy listeners");
	if (!semset->wait(10)) {
		// FIXME: bail somehow
	}
	logDebugMessage("finished decrementing busy listeners");
}

int32_t sqlrlistener::getBusyListeners() {
	return semset->getValue(10);
}

void sqlrlistener::logDebugMessage(const char *info) {
	if (!sqlrlg) {
		return;
	}
	sqlrlg->runLoggers(this,NULL,NULL,
			SQLRLOGGER_LOGLEVEL_DEBUG,
			SQLRLOGGER_EVENTTYPE_DEBUG_MESSAGE,
			info);
}

void sqlrlistener::logClientProtocolError(const char *info, ssize_t result) {
	if (!sqlrlg) {
		return;
	}
	stringbuffer	errorbuffer;
	errorbuffer.append(info);
	if (result==0) {
		errorbuffer.append(": client closed connection");
	} else if (result==RESULT_ERROR) {
		errorbuffer.append(": error");
	} else if (result==RESULT_TIMEOUT) {
		errorbuffer.append(": timeout");
	} else if (result==RESULT_ABORT) {
		errorbuffer.append(": abort");
	}
	if (error::getErrorNumber()) {
		char	*error=error::getErrorString();
		errorbuffer.append(": ")->append(error);
		delete[] error;
	}
	sqlrlg->runLoggers(this,NULL,NULL,
			SQLRLOGGER_LOGLEVEL_ERROR,
			SQLRLOGGER_EVENTTYPE_CLIENT_PROTOCOL_ERROR,
			errorbuffer.getString());
}

void sqlrlistener::logClientConnectionRefused(const char *info) {
	if (!sqlrlg) {
		return;
	}
	sqlrlg->runLoggers(this,NULL,NULL,
			SQLRLOGGER_LOGLEVEL_WARNING,
			SQLRLOGGER_EVENTTYPE_CLIENT_CONNECTION_REFUSED,
			info);
}

void sqlrlistener::logInternalError(const char *info) {
	if (!sqlrlg) {
		return;
	}
	stringbuffer	errorbuffer;
	errorbuffer.append(info);
	if (error::getErrorNumber()) {
		char	*error=error::getErrorString();
		errorbuffer.append(": ")->append(error);
		delete[] error;
	}
	sqlrlg->runLoggers(this,NULL,NULL,
			SQLRLOGGER_LOGLEVEL_ERROR,
			SQLRLOGGER_EVENTTYPE_INTERNAL_ERROR,
			errorbuffer.getString());
}

void sqlrlistener::alarmHandler(int32_t signum) {
	alarmrang=1;
	alarmhandler.handleSignal(SIGALRM);
}

void sqlrlistener::sigUsr1Handler(int32_t signum) {
	gotsigusr1=1;
	sigusr1handler.handleSignal(SIGUSR1);
}
