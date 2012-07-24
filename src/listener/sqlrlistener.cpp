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

#include <defines.h>
#include <defaults.h>

#ifdef RUDIMENTS_NAMESPACE
using namespace rudiments;
#endif

signalhandler		sqlrlistener::alarmhandler;
volatile sig_atomic_t	sqlrlistener::alarmrang=0;

// The signal handler just turn the flag on and re-enables itself.
void sqlrlistener::alarmHandler(int32_t signum) {
	alarmrang=1;
	alarmhandler.handleSignal(SIGALRM);
}

sqlrlistener::sqlrlistener() : daemonprocess(), listener() {

	cmdl=NULL;

	init=false;

	authc=NULL;

	semset=NULL;
	idmemory=NULL;

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

	sessiontype=SQLRELAY_CLIENT_SESSION_TYPE;

	handoffsockun=NULL;
	removehandoffsockun=NULL;
	fixupsockun=NULL;
	fixupsockname=NULL;

	handoffsocklist=NULL;

	denied=NULL;
	allowed=NULL;

	maxquerysize=0;
	idleclienttimeout=-1;

	isforkedchild=false;
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

	dbgfile.closeDebugFile();

	delete tmpdir;
}

bool sqlrlistener::initListener(int argc, const char **argv) {

	init=true;

	cmdl=new cmdline(argc,argv);

	tmpdir=new tempdir(cmdl);

	if (!cfgfl.parse(cmdl->getConfig(),cmdl->getId())) {
		return false;
	}

	setUserAndGroup();

	if (!verifyAccessToConfigFile(cmdl->getConfig())) {
		return false;
	}

	dbgfile.init("listener",cmdl->getLocalStateDir());
	if (cmdl->found("-debug")) {
		dbgfile.enable();
	}

	if (!handlePidFile(cmdl->getId())) {
		return false;
	}

	handleDynamicScaling();

	setHandoffMethod();

	if (cfgfl.getAuthOnListener()) {
		authc=new authenticator(&cfgfl);
	}

	setIpPermissions();

	if (!createSharedMemoryAndSemaphores(cmdl->getId())) {
		return false;
	}

	if ((passdescriptor=cfgfl.getPassDescriptor())) {
		if (!listenOnHandoffSocket(cmdl->getId())) {
			return false;
		}
		if (!listenOnDeregistrationSocket(cmdl->getId())) {
			return false;
		}
		if (!listenOnFixupSocket(cmdl->getId())) {
			return false;
		}
	}

	idleclienttimeout=cfgfl.getIdleClientTimeout();
	maxquerysize=cfgfl.getMaxQuerySize();
	maxlisteners=cfgfl.getMaxListeners();
	listenertimeout=cfgfl.getListenerTimeout();

	detach();

	createPidFile(pidfile,permissions::ownerReadWrite());

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

void sqlrlistener::setHandoffMethod() {

	// get handoff method
	if (cfgfl.getPassDescriptor()) {

		// create the list of handoff nodes
		handoffsocklist=new handoffsocketnode[maxconnections];
		for (int32_t i=0; i<maxconnections; i++) {
			handoffsocklist[i].pid=0;
			handoffsocklist[i].sock=NULL;
		}
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

	dbgfile.debugPrint("listener",0,
			"creating shared memory and semaphores");
	dbgfile.debugPrint("listener",0,"id filename: ");
	dbgfile.debugPrint("listener",0,idfilename);

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
	dbgfile.debugPrint("listener",1,"creating shared memory...");

	idmemory=new sharedmemory;
	if (!idmemory->create(key,sizeof(shmdata),
				permissions::evalPermString("rw-r-----"))) {
		idmemory->attach(key);
		shmError(id,idmemory->getId());
		return false;
	}
	rawbuffer::zero(idmemory->getPointer(),sizeof(shmdata));

	// create (or connect) to the semaphore set
	// FIXME: if it already exists, attempt to remove and re-create it
	dbgfile.debugPrint("listener",1,"creating semaphores...");

	// semaphores are:
	//
	// "connection count" - number of open database connections
	// "session count" - number of clients currently connected
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

bool sqlrlistener::listenOnClientSockets() {

	// get addresses/inet port and unix port to
	// listen on for the SQL Relay protocol
	const char * const *addresses=cfgfl.getAddresses();
	clientsockincount=cfgfl.getAddressCount();
	uint16_t	port=cfgfl.getPort();
	if (!port) {
		clientsockincount=0;
	}
	const char	*uport=cfgfl.getUnixPort();
	unixport=charstring::duplicate(uport);

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
				fprintf(stderr,
					"Could not listen "
					"on: %s/%d\n"
					"Error was: %s\n"
					"Make sure that no other "
					"processes are listening "
					"on that port.\n\n",
					addresses[index],port,
					error::getErrorString());
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
	const char	*mysqluport=cfgfl.getMySQLUnixPort();
	mysqlunixport=charstring::duplicate(mysqluport);

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
				fprintf(stderr,
					"Could not listen "
					"on: %s/%d\n"
					"Error was: %s\n"
					"Make sure that no other "
					"processes are listening "
					"on that port.\n\n",
					mysqladdresses[index],mysqlport,
					error::getErrorString());
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
	shmdata	*ptr=(shmdata *)idmemory->getPointer();
	for (;;) {
		semset->waitWithUndo(9);
		int32_t	opensvrconnections=ptr->statistics.open_svr_connections;
		semset->signalWithUndo(9);

		if (opensvrconnections<
			static_cast<int32_t>(cfgfl.getConnections())) {
			dbgfile.debugPrint("listener",0,
				"waiting for server connections (sleeping 1s)");
			snooze::macrosnooze(1);
		} else {
			dbgfile.debugPrint("listener",0,
				"finished waiting for server connections");
			break;
		}
	}

	// listen for client connections
	if (!listenOnClientSockets()) {
		return;
	}

	blockSignals();
	for(;;) {
		// FIXME: this can return true/false, should we do anything
		// with that?
		handleClientConnection(waitForData());
	}
}

void sqlrlistener::blockSignals() {

	// the daemon class handles SIGTERM's and SIGINT's and SIGCHLDS
	// and we need to catch SIGALRM's for timeouts
	// but we're going to block all other signals
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

	// set a handler for SIGALRM's
	alarmhandler.setHandler(alarmHandler);
	alarmhandler.handleSignal(SIGALRM);
}

filedescriptor *sqlrlistener::waitForData() {

	dbgfile.debugPrint("listener",0,"waiting for client connection...");

	// wait for data on one of the sockets...
	// if something bad happened, return an invalid file descriptor
	if (listener::waitForNonBlockingRead(-1,-1)<1) {
		return NULL;
	}

	// return first file descriptor that had data available or an invalid
	// file descriptor on error
	filedescriptor	*fd=NULL;
	listener::getReadyList()->getDataByIndex(0,&fd);

	dbgfile.debugPrint("listener",0,
			"finished waiting for client connection");

	return fd;
}

// increment the number of "busy listeners"
void sqlrlistener::incBusyListeners() {
	dbgfile.debugPrint("listener",0,"incrementing busy listeners");
	if (!semset->signal(10)) {
		// FIXME: bail somehow
	}
	dbgfile.debugPrint("listener",0,"finished incrementing busy listeners");
}

// decrement the number of "busy listeners"
void sqlrlistener::decBusyListeners() {
	dbgfile.debugPrint("listener",0,"decrementing busy listeners");
	if (!semset->wait(10)) {
		// FIXME: bail somehow
	}
	dbgfile.debugPrint("listener",0,"finished decrementing busy listeners");
}

// get number of "busy listeners"
int sqlrlistener::getBusyListeners() {
	return semset->getValue(10);
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

	// init session type
	sessiontype=SQLRELAY_CLIENT_SESSION_TYPE;

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
			sessiontype=MYSQL_CLIENT_SESSION_TYPE;
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
		sessiontype=MYSQL_CLIENT_SESSION_TYPE;
	} else {
		return true;
	}

	clientsock->dontUseNaglesAlgorithm();
	// FIXME: use bandwidth delay product to tune these
	// SO_SNDBUF=0 causes no data to ever be sent on openbsd
	//clientsock->setTcpReadBufferSize(8192);
	//clientsock->setTcpWriteBufferSize(0);
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
		incBusyListeners();
		clientSession(clientsock);
		decBusyListeners();
	}

	return true;
}


bool sqlrlistener::registerHandoff(filedescriptor *sock) {

	dbgfile.debugPrint("listener",0,"registering handoff...");

	// get the connection daemon's pid
	uint32_t processid;
	if (sock->read(&processid)!=sizeof(uint32_t)) {
		dbgfile.debugPrint("listener",1,"failed to read process id");
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

	dbgfile.debugPrint("listener",0,"finished registering handoff...");
	return true;
}

bool sqlrlistener::deRegisterHandoff(filedescriptor *sock) {

	dbgfile.debugPrint("listener",0,"de-registering handoff...");

	// get the connection daemon's pid
	uint32_t	processid;
	if (sock->read(&processid)!=sizeof(uint32_t)) {
		dbgfile.debugPrint("listener",1,"failed to read process id");
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

	dbgfile.debugPrint("listener",0,"finished de-registering handoff...");
	return true;
}

bool sqlrlistener::fixup(filedescriptor *sock) {

	dbgfile.debugPrint("listener",0,
			"passing socket of newly spawned connection...");

	// get the pid of the connection daemon the child listener needs
	uint32_t	processid;
	if (sock->read(&processid)!=sizeof(uint32_t)) {
		dbgfile.debugPrint("listener",1,"failed to read process id");
		delete sock;
		return false;
	}

	// look through the handoffsocklist for the pid
	bool	retval=false;
	for (int32_t i=0; i<maxconnections; i++) {
		if (handoffsocklist[i].pid==processid) {
			retval=sock->passFileDescriptor(handoffsocklist[i].
						sock->getFileDescriptor());
			dbgfile.debugPrint("listener",1,
					"found socket for requested pid ");
			if (retval) {
				dbgfile.debugPrint("listener",1,
						"passed it successfully");
			} else {
				dbgfile.debugPrint("listener",1,"failed to pass it");
			}
			break;
		}
	}

	// clean up
	delete sock;

	dbgfile.debugPrint("listener",0,
			"finished passing socket of newly spawned connection");

	return retval;
}

bool sqlrlistener::deniedIp(filedescriptor *clientsock) {

	dbgfile.debugPrint("listener",0,"checking for valid ip...");

	char	*ip=clientsock->getPeerAddress();
	if (ip && denied->match(ip) &&
			(!allowed || (allowed && !allowed->match(ip)))) {

		dbgfile.debugPrint("listener",0,"invalid ip...");

		delete[] ip;
		return true;
	}

	dbgfile.debugPrint("listener",0,"valid ip...");

	delete[] ip;
	return false;
}

// increment the number of forked listeners
int sqlrlistener::incForkedListeners() {

	incBusyListeners();

	shmdata	*ptr=(shmdata *)idmemory->getPointer();
	semset->waitWithUndo(9);
	int	forkedlisteners=++(ptr->statistics.forked_listeners);
	semset->signalWithUndo(9);
	return forkedlisteners;
}

// decrement the number of forked listeners
int sqlrlistener::decForkedListeners() {

	decBusyListeners();

	shmdata	*ptr=(shmdata *)idmemory->getPointer();
	semset->waitWithUndo(9);
	if (--(ptr->statistics.forked_listeners)<0) {
		ptr->statistics.forked_listeners=0;
	}
	int forkedlisteners=ptr->statistics.forked_listeners;
	semset->signalWithUndo(9);
	return forkedlisteners;
}

void sqlrlistener::errorClientSession(filedescriptor *clientsock,
							const char *err) {
	// get auth and ignore the result
	getAuth(clientsock);
	clientsock->write((uint16_t)ERROR_OCCURRED);
	clientsock->write((uint64_t)0);
	clientsock->write((uint16_t)strlen(err));
	clientsock->write(err);
	flushWriteBuffer(clientsock);
	// FIXME: if we got -1 from getAuth, then the client may be
	// spewing garbage and we should close the connection...
	waitForClientClose(0,false,clientsock);
	delete clientsock;
}

void sqlrlistener::forkChild(filedescriptor *clientsock) {

	// increment the number of "forked listeners"
	// do this before we actually fork to prevent a race condition where
	// a bunch of children get forked off before any of them get a chance
	// to increment this and prevent more from getting forked off
	int	forkedlisteners=incForkedListeners();

	// if we already have too many listeners running,
	// bail and return an error to the client
	if (maxlisteners>-1 &&
		forkedlisteners>maxlisteners) {

		// since we've decided not to fork, decrement the counters
		decForkedListeners();
		errorClientSession(clientsock,"Too many listeners.");
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

		dbgfile.init("listener",cmdl->getLocalStateDir());

		clientSession(clientsock);

		decForkedListeners();

		cleanUp();
		process::exit(0);
	} else if (childpid>0) {
		// parent
		char	debugstring[22];
		snprintf(debugstring,22,"forked a child: %d",childpid);
		dbgfile.debugPrint("listener",0,debugstring);
		// the main process doesn't need to stay connected
		// to the client, only the forked process
		delete clientsock;
	} else {
		// error
		decForkedListeners();
		errorClientSession(clientsock, "Error forking listener");
	}
}

void sqlrlistener::clientSession(filedescriptor *clientsock) {

	switch (sessiontype) {
		case SQLRELAY_CLIENT_SESSION_TYPE:
			sqlrelayClientSession(clientsock);
			break;
		case MYSQL_CLIENT_SESSION_TYPE:
			mysqlClientSession(clientsock);
			break;
	}

	delete clientsock;
}

void sqlrlistener::sqlrelayClientSession(filedescriptor *clientsock) {

	bool	passstatus=false;

	// handle authentication
	int32_t	authstatus=getAuth(clientsock);

	// 3 possible outcomes: 1=pass 0=fail -1=bad data
	if (authstatus==1) {

		if (dynamicscaling) {
			incrementSessionCount();
		}
		passstatus=handOffClient(clientsock);

		// If the handoff failed, decrement the session count.
		// If it had succeeded then the connection daemon would
		// decrement it later.
		if (dynamicscaling && !passstatus) {
			decrementSessionCount();
		}

	} else if (authstatus==0) {

		dbgfile.debugPrint("listener",1,"sending client auth error");

		// snooze before and after returning an
		// authentication error to discourage
		// brute-force password attacks
		snooze::macrosnooze(2);
		const char	err[]="Authentication Error.";
		clientsock->write((uint16_t)ERROR_OCCURRED);
		clientsock->write((uint64_t)0);
		clientsock->write((uint16_t)strlen(err));
		clientsock->write(err);
		flushWriteBuffer(clientsock);
		snooze::macrosnooze(2);
	}

	// FIXME: if we got -1 from getAuth, then the client may be spewing
	// garbage and we should close the connection...
	waitForClientClose(authstatus,passstatus,clientsock);
}

void sqlrlistener::mysqlClientSession(filedescriptor *clientsock) {

	bool	passstatus=false;

	// handle authentication
	int32_t	authstatus=getMySQLAuth(clientsock);

	// 3 possible outcomes: 1=pass 0=fail -1=bad data
	if (authstatus==1) {

		if (dynamicscaling) {
			incrementSessionCount();
		}
		passstatus=handOffClient(clientsock);

	} else if (authstatus==0) {

		dbgfile.debugPrint("listener",1,"sending client auth error");

		// snooze before and after returning an
		// authentication error to discourage
		// brute-force password attacks
		snooze::macrosnooze(2);
		// FIXME: send error using mysql protocol
		//clientsock->write(...);
		flushWriteBuffer(clientsock);
		snooze::macrosnooze(2);
	}

	// FIXME: if we got -1 from getAuth, then the client may be spewing
	// garbage and we should close the connection...

	// FIXME: waitForClientClose probably isn't right for mysql
	waitForClientClose(authstatus,passstatus,clientsock);
}

int32_t sqlrlistener::getAuth(filedescriptor *clientsock) {

	dbgfile.debugPrint("listener",0,"getting authentication...");

	// Get the user/password. For either one, if they are too big or
	// if there's a read error, just exit with an error code
	uint32_t	size;
	clientsock->read(&size,idleclienttimeout,0);
	char		userbuffer[(uint32_t)USERSIZE+1];
	if (size>(uint32_t)USERSIZE ||
		(uint32_t)(clientsock->read(userbuffer,size,
						idleclienttimeout,0))!=size) {
		dbgfile.debugPrint("listener",0,
			"authentication failed: user size is wrong");
		return -1;
	}
	userbuffer[size]='\0';

	char		passwordbuffer[(uint32_t)USERSIZE+1];
	clientsock->read(&size,idleclienttimeout,0);
	if (size>(uint32_t)USERSIZE ||
		(uint32_t)(clientsock->read(passwordbuffer,size,
						idleclienttimeout,0))!=size) {
		dbgfile.debugPrint("listener",0,
			"authentication failed: password size is wrong");
		return -1;
	}
	passwordbuffer[size]='\0';

	// If the listener is supposed to authenticate, then do so, otherwise
	// just return 1 as if authentication succeeded.
	if (authc) {

		// Return 1 if what the client sent matches one of the
		// user/password sets and 0 if no match is found.
		bool	retval=authc->authenticate(userbuffer,passwordbuffer);
		if (retval) {
			dbgfile.debugPrint("listener",1,
				"listener-based authentication succeeded");
		} else {
			dbgfile.debugPrint("listener",1,
				"listener-based authentication failed: "
				"invalid user/password");
		}
		return (retval)?1:0;
	}

	dbgfile.debugPrint("listener",0,"finished getting authentication");

	return 1;
}

// MySQL server/client capabilities
#define CLIENT_LONG_PASSWORD	1	/* new more secure passwords */
#define CLIENT_FOUND_ROWS	2	/* Found instead of affected rows */
#define CLIENT_LONG_FLAG	4	/* Get all column flags */
#define CLIENT_CONNECT_WITH_DB	8	/* One can specify db on connect */
#define CLIENT_NO_SCHEMA	16	/* Don't allow database.table.column */
#define CLIENT_COMPRESS		32	/* Can use compression protocol */
#define CLIENT_ODBC		64	/* Odbc client */
#define CLIENT_LOCAL_FILES	128	/* Can use LOAD DATA LOCAL */
#define CLIENT_IGNORE_SPACE	256	/* Ignore spaces before '(' */
#define CLIENT_PROTOCOL_41	512	/* New 4.1 protocol */
#define CLIENT_INTERACTIVE	1024	/* This is an interactive client */
#define CLIENT_SSL		2048	/* Switch to SSL after handshake */
#define CLIENT_IGNORE_SIGPIPE	4096	/* IGNORE sigpipes */
#define CLIENT_TRANSACTIONS	8192	/* Client knows about transactions */
#define CLIENT_RESERVED		16384	/* Old flag for 4.1 protocol  */
#define CLIENT_SECURE_CONNECTION	32768	/* New 4.1 authentication */
#define CLIENT_MULTI_STATEMENTS	65536	/* Enable/disable multi-stmt support */
#define CLIENT_MULTI_RESULTS	131072	/* Enable/disable multi-results */

int32_t sqlrlistener::getMySQLAuth(filedescriptor *clientsock) {

	// send handshake initialization packet...

	// protocol version
	clientsock->write((char)10);

	// server version
	clientsock->write("5.1.0",6);

	// thread id
	clientsock->write((uint32_t)0);

	// scramble buf (part 1)
	clientsock->write("00000000",8);
	
	// filler
	clientsock->write((char)0);
	
	// define the server capabilities
	union servercap_t {
		uint32_t	together;
		struct {
			uint16_t	lower;
			uint16_t	upper;
		} split;
	};
	union servercap_t	servercap;
	servercap.together=0;

	// server capabilities (two lower bytes)
	clientsock->write(servercap.split.lower);
	
	// server language
	clientsock->write((char)1);
	
	// server status
	clientsock->write((uint16_t)0);
	
	// server capabilities (two upper bytes)
	clientsock->write(servercap.split.upper);
	
	// length of scramble buf part 2
	clientsock->write((char)8);
	
	// filler
	clientsock->write("\0\0\0\0\0\0\0\0\0\0",10);
	
	// scramble buf (part 2)
	clientsock->write("00000000",8);
	
	// null terminator
	clientsock->write((char)0);

	// get client authentication packet...
	uint16_t	clientflags1;
	clientsock->read(&clientflags1);
	

	// send ok/error packet...

	return 0;
}

void sqlrlistener::acquireSessionCountMutex() {
	if (!semset->waitWithUndo(5)) {
		// FIXME: bail somehow
	}
}

void sqlrlistener::releaseSessionCountMutex() {
	if (!semset->signalWithUndo(5)) {
		// FIXME: bail somehow
	}
}

void sqlrlistener::incrementSessionCount() {

	dbgfile.debugPrint("listener",0,"incrementing session count...");

	acquireSessionCountMutex();

	// increment the counter
	shmdata	*ptr=(shmdata *)idmemory->getPointer();
	ptr->connectionsinuse++;

	dbgfile.debugPrint("listener",1,ptr->connectionsinuse);

	// If the system supports timed semaphore ops then the scaler can be
	// jogged into running on-demand, and we can do that here.  If the 
	// sytem does not support timed semaphore ops then the scaler will
	// just loop periodically on its own and we shouldn't attempt to
	// jog it.
	if (semset->supportsTimedSemaphoreOperations()) {

		// signal the scaler to evaluate the connection count
		// and start more connections if necessary
		dbgfile.debugPrint("listener",1,"signalling the scaler...");
		if (!semset->signal(6)) {
			// FIXME: bail somehow
		}
		dbgfile.debugPrint("listener",1,
					"finished signalling the scaler...");

		// wait for the scaler
		dbgfile.debugPrint("listener",1,"waiting for the scaler...");
		if (!semset->wait(7)) {
			// FIXME: bail somehow
		}
		dbgfile.debugPrint("listener",1,
					"finished waiting for the scaler...");
	}

	releaseSessionCountMutex();

	dbgfile.debugPrint("listener",0,"finished incrementing session count");
}

void sqlrlistener::decrementSessionCount() {

	dbgfile.debugPrint("listener",0,"decrementing session count...");
 
	acquireSessionCountMutex();

	// decrement the counter
	shmdata	*ptr=(shmdata *)idmemory->getPointer();
	ptr->connectionsinuse--;
	if (ptr->connectionsinuse<0) {
		ptr->connectionsinuse=0;
	}

	dbgfile.debugPrint("listener",1,ptr->connectionsinuse);

	releaseSessionCountMutex();

	dbgfile.debugPrint("listener",0,"finished decrementing session count");
}

bool sqlrlistener::handOffClient(filedescriptor *sock) {

	uint32_t	connectionpid;
	uint16_t	inetport;
	char 		unixportstr[MAXPATHLEN+1];
	uint16_t	unixportstrlen;
	bool		retval=false;

	// loop in case client doesn't get handed off successfully
	for (;;) {

		if (!getAConnection(&connectionpid,&inetport,
					unixportstr,&unixportstrlen)) {
			// fatal error occurred while getting a connection
			sock->write((uint16_t)ERROR_OCCURRED);
			sock->write((uint64_t)0);
			sock->write((uint16_t)70);
			sock->write("The listener failed to hand the client off to the database connection.");
			flushWriteBuffer(sock);
			retval=false;
			break;
		}

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
				/*sock->write((uint16_t)ERROR_OCCURRED);
				sock->write((uint64_t)0);
				sock->write((uint16_t)70);
				sock->write("The listener failed to hand the client off to the database connection.");
				flushWriteBuffer(sock);
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
			sock->write((uint16_t)NO_ERROR_OCCURRED);
			sock->write((uint16_t)DONT_RECONNECT);
			flushWriteBuffer(sock);
			retval=true;
			break;

		} else {

			// FIXME: if we're not passing around file descriptors,
			// how can we deterimine if a connection has crashed
			// or been killed?
			sock->write((uint16_t)NO_ERROR_OCCURRED);
			sock->write((uint16_t)RECONNECT);
			sock->write(unixportstrlen);
			sock->write(unixportstr);
			sock->write((uint16_t)inetport);
			flushWriteBuffer(sock);
			retval=true;
			break;
		}
	}

	return retval;
}

bool sqlrlistener::acquireShmAccess() {

	dbgfile.debugPrint("listener",0,"acquiring exclusive shm access");

	// don't even begin to wait if the alarm already rang
	if (alarmrang) {
		dbgfile.debugPrint("listener",0,"timeout occured");
		return false;
	}

	// Loop, waiting.  Retry the wait if it was interrupted by a signal,
	// other than an alarm, but if an alarm interrupted it then bail.
	bool	result=true;
	do {
		result=semset->waitWithUndo(1);
	} while (!result && error::getErrorNumber()==EINTR && alarmrang!=1);

	// handle alarm...
	if (alarmrang) {
		dbgfile.debugPrint("listener",0,"timeout occured");
		return false;
	}

	// handle general failure...
	if (!result) {
		dbgfile.debugPrint("listener",0,
			"failed to acquire exclusive shm access");
		return false;
	}

	// success...
	dbgfile.debugPrint("listener",0,"acquired exclusive shm access");
	return true;
}

bool sqlrlistener::releaseShmAccess() {

	dbgfile.debugPrint("listener",-1,"releasing exclusive shm access");

	if (!semset->signalWithUndo(1)) {
		dbgfile.debugPrint("listener",0,
			"failed to release exclusive shm access");
		return false;
	}

	dbgfile.debugPrint("listener",0,
			"finished releasing exclusive shm access");
	return true;
}

bool sqlrlistener::acceptAvailableConnection() {

	dbgfile.debugPrint("listener",0,"waiting for an available connection");

	// don't even begin to wait if the alarm already rang
	if (alarmrang) {
		dbgfile.debugPrint("listener",0,"timeout occured");
		return false;
	}

	// Loop, waiting.  Retry the wait if it was interrupted by a signal,
	// other than an alarm, but if an alarm interrupted it then bail.
	bool	result=true;
	do {
		result=semset->wait(2);
	} while (!result && error::getErrorNumber()==EINTR && alarmrang!=1);

	// handle alarm...
	if (alarmrang) {
		dbgfile.debugPrint("listener",0,"timeout occured");
		return false;
	}

	// handle general failure...
	if (!result) {
		dbgfile.debugPrint("listener",0,
			"failed to wait for an available connection");
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

	dbgfile.debugPrint("listener",0,
			"succeeded in waiting for an available connection");
	return true;
}

bool sqlrlistener::doneAcceptingAvailableConnection() {

	dbgfile.debugPrint("listener",0,
		"signalling accepted connection");

	if (!semset->signal(3)) {
		dbgfile.debugPrint("listener",0,
		"failed to signal accapted connection");
		return false;
	}

	dbgfile.debugPrint("listener",0,
		"succeeded signalling accepted connection");

	return true;
}

bool sqlrlistener::getAConnection(uint32_t *connectionpid,
					uint16_t *inetport,
					char *unixportstr,
					uint16_t *unixportstrlen) {

	// get a pointer to the shared memory segment
	shmdata *ptr=(shmdata *)idmemory->getPointer();

	// use an alarm, if we're a forked child and a timeout is set
	bool	usealarm=(isforkedchild && listenertimeout);

	for (;;) {

		dbgfile.debugPrint("listener",0,"getting a connection...");

		// set an alarm
		if (usealarm) {
			semset->dontRetryInterruptedOperations();
			alarmrang=0;
			signalmanager::alarm(listenertimeout);
printf("setting alarm: %d\n",listenertimeout);
		}

		// acquire access to the shared memory
		bool	ok=acquireShmAccess();

		if (ok) {	

			// This section should be executed without returns or
			// breaks so that releaseShmAccess will get called
			// at the end, no matter what...

			// wait for an available connection
			ok=acceptAvailableConnection();

			// turn off the alarm
			if (usealarm) {
				signalmanager::alarm(0);
printf("disabling alarm\n");
				semset->retryInterruptedOperations();
			}

			if (ok) {

				// This section should be executed without
				// returns or breaks so that
				// signalAcceptedConnection will get called at
				// the end, no matter what...

				// if we're passing descriptors around, the
				// connection will pass it's pid to us,
				// otherwise it will pass it's inet and unix
				// ports
				if (passdescriptor) {

					dbgfile.debugPrint("listener",1,
							"handoff=pass");

					// get the pid
					*connectionpid=ptr->connectioninfo.
								connectionpid;

				} else {

					dbgfile.debugPrint("listener",1,
							"handoff=reconnect");

					// get the inet port
					*inetport=ptr->connectioninfo.
							sockets.inetport;

					// get the unix port
					charstring::copy(unixportstr,
							ptr->connectioninfo.
							sockets.unixsocket,
							MAXPATHLEN);
					*unixportstrlen=charstring::length(
								unixportstr);

					// debug...
					size_t  debugstringlen=
						15+*unixportstrlen+21;
					char	*debugstring=
						new char[debugstringlen];
					snprintf(debugstring,debugstringlen,
							"socket=%s  port=%d",
							unixportstr,*inetport);
					dbgfile.debugPrint("listener",1,
								debugstring);
					delete[] debugstring;
				}

				// signal the connection that we waited for
				ok=doneAcceptingAvailableConnection();
			}

			// release access to the shared memory
			ok=(releaseShmAccess() && ok);
		}

		// turn off the alarm
		if (usealarm) {
			signalmanager::alarm(0);
printf("disabling alarm\n");
			semset->retryInterruptedOperations();
		}

		// execute this only if code above executed without errors...
		if (ok) {

			// if we're not waiting for down databases then return
			if (!cfgfl.getWaitForDownDatabase()) {
				dbgfile.debugPrint("listener",1,
					"finished getting a connection");
				return true;
			}

			// make sure the connection is actually up...
			if (connectionIsUp(ptr->connectionid)) {
				dbgfile.debugPrint("listener",1,
					"finished getting a connection");
				return true;
			}

			// if the connection wasn't up, fork a child to jog it,
			// spin back and get another connection
			dbgfile.debugPrint("listener",1,"connection was down");
			pingDatabase(*connectionpid,unixportstr,*inetport);
		}

		// bail if the timeout was reached
		if (alarmrang) {
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

	// fork off and cause the connection to ping the database,
	// this should cause it to reconnect
	if (process::fork()) {
		return;
	}

	isforkedchild=true;

	if (passdescriptor) {

		// send file descriptor 0 to the connection,
		// it will interpret this as a ping
		filedescriptor	connectionsock;
		if (findMatchingSocket(connectionpid,&connectionsock)) {
			connectionsock.write((uint16_t)HANDOFF_RECONNECT);
			snooze::macrosnooze(1);
		}

	} else {

		// connect to the database connection daemon
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
			connsock->close();

			// clean up
			delete connsock;
		}
	}

	cleanUp();
	process::exit(0);
}

filedescriptor *sqlrlistener::connectToConnection(uint32_t connectionpid,
						const char *unixportstr,
						uint16_t inetport) {

	// first, try for the unix port
	if (unixportstr && unixportstr[0]) {
		unixclientsocket	*unixsock=new unixclientsocket();
		if (unixsock->connect(unixportstr,-1,-1,0,1)==RESULT_SUCCESS) {
			return unixsock;
		}
		delete unixsock;
	}

	// then try for the inet port
	inetclientsocket	*inetsock=new inetclientsocket();
	if (inetsock->connect("127.0.0.1",inetport,-1,-1,0,1)==RESULT_SUCCESS) {
		return inetsock;
	}
	delete inetsock;

	return NULL;
}

bool sqlrlistener::passClientFileDescriptorToConnection(
					filedescriptor *connectionsock,
					int fd) {

	dbgfile.debugPrint("listener",1,"passing descriptor...");

	// tell the connection we're passing a file descriptor
	if (connectionsock->write((uint16_t)HANDOFF_FD)!=sizeof(uint16_t)) {
		dbgfile.debugPrint("listener",0,"passing descriptor failed");
		return false;
	}

	// pass the file descriptor
	if (!connectionsock->passFileDescriptor(fd)) {
		dbgfile.debugPrint("listener",0,"passing descriptor failed");
		return false;
	}

	dbgfile.debugPrint("listener",0,"finished passing descriptor");
	return true;
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
	//return requestFixup(connectionpid,connectionsock);
	return requestFixup(connectionpid,connectionsock);
}

bool sqlrlistener::requestFixup(uint32_t connectionpid,
					filedescriptor *connectionsock) {

	dbgfile.debugPrint("listener",0,
			"requesting socket of newly spawned connection...");

	// connect to the fixup socket of the parent listener
	unixclientsocket	fixupclientsockun;
	if (fixupclientsockun.connect(fixupsockname,-1,-1,0,1)
						!=RESULT_SUCCESS) {
		dbgfile.debugPrint("listener",0,
			"failed to connect to parent listener process");
		return false;
	}

	// send the pid of the connection that we need
	if (fixupclientsockun.write(connectionpid)!=sizeof(uint32_t)) {
		dbgfile.debugPrint("listener",0,"failed to send the pid");
		return false;
	}

	// get the file descriptor of the socket
	int32_t	fd;
	if (!fixupclientsockun.receiveFileDescriptor(&fd)) {
		dbgfile.debugPrint("listener",0,"failed to receive the socket");
		return false;
	}
	connectionsock->setFileDescriptor(fd);

	dbgfile.debugPrint("listener",0,
			"received socket of newly spawned connection");
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
