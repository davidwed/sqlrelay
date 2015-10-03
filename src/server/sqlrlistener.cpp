// Copyright (c) 1999-2014  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

#include <rudiments/permissions.h>
#include <rudiments/unixsocketclient.h>
#include <rudiments/inetsocketclient.h>
#include <rudiments/bytestring.h>
#include <rudiments/snooze.h>
#include <rudiments/userentry.h>
#include <rudiments/groupentry.h>
#include <rudiments/process.h>
#include <rudiments/file.h>
#include <rudiments/error.h>
#include <rudiments/datetime.h>
#include <rudiments/sys.h>
#include <rudiments/stdio.h>
#include <rudiments/thread.h>

#include <defaults.h>
#include <defines.h>

#ifndef MAXPATHLEN
	#define MAXPATHLEN	256
#endif

signalhandler		sqlrlistener::alarmhandler;
volatile sig_atomic_t	sqlrlistener::alarmrang=0;

sqlrlistener::sqlrlistener() : listener() {

	cmdl=NULL;
	sqlrcfgs=NULL;
	cfg=NULL;

	initialized=false;

	sqlrlg=NULL;

	semset=NULL;
	shmem=NULL;
	shm=NULL;
	idfilename=NULL;

	pidfile=NULL;
	sqlrpth=NULL;

	clientsockin=NULL;
	clientsockinproto=NULL;
	clientsockincount=0;
	clientsockinindex=0;
	clientsockun=NULL;
	clientsockunproto=NULL;
	clientsockuncount=0;
	clientsockunindex=0;

	handoffsockun=NULL;
	handoffsockname=NULL;
	removehandoffsockun=NULL;
	removehandoffsockname=NULL;
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
	isforkedthread=false;
	handoffmode=HANDOFF_PASS;

	usethreads=false;
}

sqlrlistener::~sqlrlistener() {
	if (!isforkedchild && idfilename) {
		file::remove(idfilename);
	}
	delete[] idfilename;

	if (!isforkedchild) {
		for (linkedlistnode< listenercontainer * > *node=
					cfg->getListenerList()->getFirst();
					node; node=node->getNext()) {
			const char	*unixport=node->getValue()->getSocket();
			if (unixport) {
				file::remove(unixport);
			}
		}
		if (pidfile) {
			file::remove(pidfile);
		}
	}
	if (initialized) {
		cleanUp();
	}

	// remove files that indicate whether the db is up or down
	linkedlist< connectstringcontainer * >	*csl=
					cfg->getConnectStringList();
	for (linkedlistnode< connectstringcontainer * > *node=csl->getFirst();
						node; node=node->getNext()) {
		connectstringcontainer	*cs=node->getValue();
		const char	*connectionid=cs->getConnectionId();
		size_t	updownlen=charstring::length(sqlrpth->getIpcDir())+
					charstring::length(cmdl->getId())+1+
					charstring::length(connectionid)+1;
		char	*updown=new char[updownlen];
		charstring::printf(updown,updownlen,
					"%s%s-%s",
					sqlrpth->getIpcDir(),
					cmdl->getId(),connectionid);
		file::remove(updown);
		delete[] updown;
	}
	delete sqlrpth;
	delete sqlrcfgs;
	delete cmdl;

	delete shmem;

	// Delete the semset last...
	// If the listener is killed while waiting on a semaphore, sometimes
	// the signal doesn't interrupt the wait, and sometimes removing
	// the semaphore during the wait causes a segfault.  The shutdown
	// process catches this and exits, but lets make sure that everything
	// else is cleaned up before this can even happen.
	delete semset;
}

void sqlrlistener::cleanUp() {

	delete[] pidfile;

	uint64_t	csind;
	for (csind=0; csind<clientsockincount; csind++) {
		delete clientsockin[csind];
	}
	delete[] clientsockin;
	delete[] clientsockinproto;
	for (csind=0; csind<clientsockuncount; csind++) {
		delete clientsockun[csind];
	}
	delete[] clientsockun;
	delete[] clientsockunproto;

	if (!isforkedchild && handoffsockname) {
		file::remove(handoffsockname);
	}
	delete[] handoffsockname;
	delete handoffsockun;

	if (handoffsocklist) {
		for (uint32_t i=0; i<maxconnections; i++) {
			delete handoffsocklist[i].sock;
		}
		delete[] handoffsocklist;
	}

	if (!isforkedchild && removehandoffsockname) {
		file::remove(removehandoffsockname);
	}
	delete[] removehandoffsockname;
	delete removehandoffsockun;

	if (!isforkedchild && fixupsockname) {
		file::remove(fixupsockname);
	}
	delete[] fixupsockname;
	delete fixupsockun;

	delete denied;
	delete allowed;
	delete sqlrlg;
}

bool sqlrlistener::init(int argc, const char **argv) {

	initialized=true;

	cmdl=new sqlrcmdline(argc,argv);
	sqlrpth=new sqlrpaths(cmdl);
	sqlrcfgs=new sqlrconfigs(sqlrpth);

	if (!charstring::compare(cmdl->getId(),DEFAULT_ID)) {
		stderror.printf("Warning: using default id.\n");
	}

	cfg=sqlrcfgs->load(sqlrpth->getConfigUrl(),cmdl->getId());
	if (!cfg) {
		return false;
	}

	setUserAndGroup();

	if (!verifyAccessToConfigUrl(sqlrpth->getConfigUrl())) {
		return false;
	}

	if (!handlePidFile(cmdl->getId())) {
		return false;
	}

	handleDynamicScaling();

	const char	*loggers=cfg->getLoggers();
	if (charstring::length(loggers)) {
		sqlrlg=new sqlrloggers(sqlrpth);
		sqlrlg->loadLoggers(loggers);
		sqlrlg->initLoggers(this,NULL);
	}

	idleclienttimeout=cfg->getIdleClientTimeout();
	maxquerysize=cfg->getMaxQuerySize();
	maxbindcount=cfg->getMaxBindCount();
	maxbindnamelength=cfg->getMaxBindNameLength();
	maxlisteners=cfg->getMaxListeners();
	listenertimeout=cfg->getListenerTimeout();

	setHandoffMethod();

	setSessionHandlerMethod();

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

	process::detach();

	process::createPidFile(pidfile,permissions::ownerReadWrite());

	setMaxListeners(maxlisteners);

	// set a handler for SIGALRMs
	#ifdef SIGALRM
	alarmhandler.setHandler(alarmHandler);
	alarmhandler.handleSignal(SIGALRM);
	#endif

	return true;
}

void sqlrlistener::setUserAndGroup() {

	// get the user that we're currently running as
	char	*currentuser=
		userentry::getName(process::getEffectiveUserId());

	// get the group that we're currently running as
	char	*currentgroup=
		groupentry::getName(process::getEffectiveGroupId());

	// switch groups, but only if we're not currently running as the
	// group that we should switch to
	if (charstring::compare(currentgroup,cfg->getRunAsGroup()) &&
				!process::setGroup(cfg->getRunAsGroup())) {
		stderror.printf("Warning: could not change group to %s\n",
						cfg->getRunAsGroup());
	}

	// switch users, but only if we're not currently running as the
	// user that we should switch to
	if (charstring::compare(currentuser,cfg->getRunAsUser()) &&
				!process::setUser(cfg->getRunAsUser())) {
		stderror.printf("Warning: could not change user to %s\n",
						cfg->getRunAsUser());
	}

	// clean up
	delete[] currentuser;
	delete[] currentgroup;
}

bool sqlrlistener::verifyAccessToConfigUrl(const char *url) {

	if (!cfg->getDynamicScaling()) {
		return true;
	}

	if (!cfg->accessible()) {
		stderror.printf("\nsqlr-listener error:\n");
		stderror.printf("	This instance of SQL Relay is ");
		stderror.printf("configured to run as:\n");
		stderror.printf("		user: %s\n",
						cfg->getRunAsUser());
		stderror.printf("		group: %s\n\n",
						cfg->getRunAsGroup());
		stderror.printf("	However, the config url %s\n",url);
		stderror.printf("	cannot be accessed by that user ");
		stderror.printf("or group.\n\n");
		stderror.printf("	Since you're using dynamic scaling ");
		stderror.printf("(ie. maxconnections>connections),\n");
		stderror.printf("	new connections would be started as\n");
		stderror.printf("		user: %s\n",
						cfg->getRunAsUser());
		stderror.printf("		group: %s\n\n",
						cfg->getRunAsGroup());
		stderror.printf("	They would not be able to access the");
		stderror.printf("config url and would shut down.\n\n");
		stderror.printf("	To remedy this problem, make %s\n",url);
		stderror.printf("	accessible by\n");
		stderror.printf("		user: %s\n",
						cfg->getRunAsUser());
		stderror.printf("		group: %s\n",
						cfg->getRunAsGroup());
		return false;
	}
	return true;
}

bool sqlrlistener::handlePidFile(const char *id) {

	// check/set pid file
	size_t	pidfilelen=charstring::length(sqlrpth->getPidDir())+14+
						charstring::length(id)+1;
	pidfile=new char[pidfilelen];
	charstring::printf(pidfile,pidfilelen,
				"%ssqlr-listener-%s",
				sqlrpth->getPidDir(),id);

	if (process::checkForPidFile(pidfile)!=-1) {
		stderror.printf("\nsqlr-listener error:\n");
		stderror.printf("	The pid file %s",pidfile);
		stderror.printf(" exists.\n");
		stderror.printf("	This usually means that the ");
		stderror.printf("sqlr-listener is already running for ");
		stderror.printf("the \n");
		stderror.printf("	%s",id);
		stderror.printf(" instance.\n");
		stderror.printf("	If it is not running, please remove ");
		stderror.printf("the file and restart.\n");
		delete[] pidfile;
		pidfile=NULL;
		return false;
	}
	return true;
}

void sqlrlistener::handleDynamicScaling() {

	// get the dynamic connection scaling parameters
	maxconnections=cfg->getMaxConnections();

	// if dynamic scaling isn't going to be used, disable it
	dynamicscaling=cfg->getDynamicScaling();
}

void sqlrlistener::setSessionHandlerMethod() {
	
	usethreads=false;
	if (!charstring::compare(cfg->getSessionHandler(),"thread")) {

		if (!thread::supportsThreads()) {
			stderror.printf("Warning: sessionhandler=\"thread\" "
					"not supported, falling back to "
					"sessionhandler=\"process\".  "
					"Either threads are not supported on "
					"this platform or Rudiments was "
					"compiled without support for threads."
					"\n");
			return;
		}

		usethreads=true;

	} else {

		if (!process::supportsFork()) {
			stderror.printf("Warning: sessionhandler=\"process\" "
					"not supported on this platform, "
					"falling back to "
					"sessionhandler=\"thread\".\n");
			usethreads=true;
		}
	}
}

void sqlrlistener::setHandoffMethod() {

	if (!charstring::compare(cfg->getHandoff(),"pass")) {

        	// on some OS'es, force proxy, even if pass was specified...

        	// get the os and version
        	char    *os=sys::getOperatingSystemName();
        	char    *rel=sys::getOperatingSystemRelease();
        	double  ver=charstring::toFloat(rel);
	
        	// force proxy for Cygwin and Linux < 2.2
        	if (!charstring::compare(os,"CYGWIN",6) ||
                	(!charstring::compare(os,"Linux",5) && ver<2.2)) {
			handoffmode=HANDOFF_PROXY;
			stderror.printf("Warning: handoff=\"pass\" not "
					"supported, falling back to "
					"handoff=\"proxy\".\n");
        	} else {
			handoffmode=HANDOFF_PASS;
		}

        	// clean up
        	delete[] os;
        	delete[] rel;

	} else {

        	// on some OS'es, force pass, even if proxy was specified...

        	// get the os and version
        	char    *os=sys::getOperatingSystemName();
	
        	// force pass for Windows
        	if (!charstring::compare(os,"Windows",7)) {
			handoffmode=HANDOFF_PASS;
			stderror.printf("Warning: handoff=\"proxy\" not "
					"supported, falling back to "
					"handoff=\"pass\".\n");
		} else {
			handoffmode=HANDOFF_PROXY;
		}
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
	const char	*deniedips=cfg->getDeniedIps();
	const char	*allowedips=cfg->getAllowedIps();
	if (deniedips[0]) {
		denied=new regularexpression(deniedips);
	}
	if (allowedips[0]) {
		allowed=new regularexpression(allowedips);
	}
}

bool sqlrlistener::createSharedMemoryAndSemaphores(const char *id) {

	// initialize the ipc filename
	size_t	idfilenamelen=charstring::length(sqlrpth->getIpcDir())+
						charstring::length(id)+1;
	idfilename=new char[idfilenamelen];
	charstring::printf(idfilename,idfilenamelen,
				"%s%s",sqlrpth->getIpcDir(),id);

	if (sqlrlg) {
		debugstr.clear();
		debugstr.append("creating shared memory "
				"and semaphores: id filename: ");
		debugstr.append(idfilename);
		logDebugMessage(debugstr.getString());
	}

	// make sure that the file exists and is read/writeable
	if (!file::createFile(idfilename,permissions::ownerReadWrite())) {
		ipcFileError(idfilename);
		return false;
	}

	// get the ipc key
	key_t	key=file::generateKey(idfilename,1);
	if (key==-1) {
		keyError(idfilename);
		return false;
	}

	// create the shared memory segment
	// FIXME: if it already exists, attempt to remove and re-create it
	logDebugMessage("creating shared memory...");

	shmem=new sharedmemory;
	if (!shmem->create(key,sizeof(shmdata),
				permissions::evalPermString("rw-r-----"))) {
		shmError(id,shmem->getId());
		shmem->attach(key,sizeof(shmdata));
		return false;
	}
	shm=(shmdata *)shmem->getPointer();
	bytestring::zero(shm,sizeof(shmdata));

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
	// 2 - connection/listener: 
	//       * listener waits
	//       * connection signals when it's done registering itself
	// 3 - connection/listener:
	//       * connection waits
	//       * listener signals when it's done reading the registration
	// 12 - connection/listener:
	//       * listener waits
	//       * connection signals when it's ready to be handed a client
	//
	// connection/listener/scaler interlocks:
	// 6 - scaler/listener: used to decide whether to scale or not
	//       * listener signals after incrementing connected client count
	//       * scaler waits before counting sessions/connections
	// 4 - connection/scaler: connection count mutex
	//       * connection increases/decreases connection count
	//       * scalar reads connection count
	// 5 - connection/listener: connected client count mutex
	//       * listener increases connected client count when
	//         a client connects
	//       * connection decreases connected client count when
	//         a client disconnects
	// 7 - scaler/listener: used to decide whether to scale or not
	//       * scaler signals after counting sessions/connections
	//       * listener waits for scaler to count sessions/connections
	// 8 - scaler/connection:
	//       * scaler waits for the connection count to increase
	//		 (in effect, waiting for a new connection to fire up)
	//       * connection signals after increasing connection count
	// 11 - scaler/connection:
	//       * scaler waits for the connection to signal to indicate that
	//	 * its exiting on platforms that don't support SIGCHLD/waitpid()
	//
	// statistics:
	// 9 - coordinates access to statistics shared memory segment
	//
	// main listenter process/listener children:
	// 10 - listener: number of busy listeners
	//
	int32_t	vals[13]={1,1,0,0,1,1,0,0,0,1,0,0,0};
	semset=new semaphoreset();
	if (!semset->create(key,permissions::ownerReadWrite(),13,vals)) {
		semError(id,semset->getId());
		semset->attach(key,13);
		return false;
	}

	// issue warning about ttl if necessary
	if (cfg->getTtl()>0 &&
			!semset->supportsTimedSemaphoreOperations() &&
			!sys::signalsInterruptSystemCalls()) {
		stderror.printf("Warning: ttl forced to 0...\n"
				"         semaphore waits cannot be "
				"interrupted:\n"
				"         system doesn't support timed "
				"semaphore operations and\n"
				"         signals don't interrupt system "
				"calls\n");
	}


	return true;
}

void sqlrlistener::ipcFileError(const char *idfilename) {
	stderror.printf("Could not open: %s\n",idfilename);
	stderror.printf("Make sure that the file and directory are ");
	stderror.printf("readable and writable.\n\n");
}

void sqlrlistener::keyError(const char *idfilename) {
	char	*err=error::getErrorString();
	stderror.printf("\nsqlr-listener error:\n");
	stderror.printf("	Unable to generate a key from ");
	stderror.printf("%s\n",idfilename);
	stderror.printf("	Error was: %s\n\n",err);
	delete[] err;
}

void sqlrlistener::shmError(const char *id, int shmid) {
	char	*err=error::getErrorString();
	stderror.printf("\nsqlr-listener error:\n");
	stderror.printf("	Unable to create a shared memory ");
	stderror.printf("segment.  This is usally because an \n");
	stderror.printf("	sqlr-listener is already running for ");
	stderror.printf("the %s instance.\n\n",id);
	stderror.printf("	If it is not running, something may ");
	stderror.printf("have crashed and left an old segment\n");
	stderror.printf("	lying around.  Use the ipcs command ");
	stderror.printf("to inspect existing shared memory \n");
	stderror.printf("	segments and the ipcrm command to ");
	stderror.printf("remove the shared memory segment with ");
	stderror.printf("\n	id %d.\n\n",shmid);
	stderror.printf("	Error was: %s\n\n",err);
	delete[] err;
}

void sqlrlistener::semError(const char *id, int semid) {
	char	*err=error::getErrorString();
	stderror.printf("\nsqlr-listener error:\n");
	stderror.printf("	Unable to create a semaphore ");
	stderror.printf("set.  This is usally because an \n");
	stderror.printf("	sqlr-listener is already ");
	stderror.printf("running for the %s",id);
	stderror.printf(" instance.\n\n");
	stderror.printf("	If it is not running, ");
	stderror.printf("something may have crashed and left ");
	stderror.printf("an old semaphore set\n");
	stderror.printf("	lying around.  Use the ipcs ");
	stderror.printf("command to inspect existing ");
	stderror.printf("semaphore sets \n");
	stderror.printf("	and the ipcrm ");
	stderror.printf("command to remove the semaphore set ");
	stderror.printf("with \n");
	stderror.printf("	id %d.\n\n",semid);
	stderror.printf("	Error was: %s\n\n",err);
	delete[] err;
}

bool sqlrlistener::listenOnClientSockets() {

	linkedlist< listenercontainer * >	*listenerlist=
						cfg->getListenerList();

	// count sockets and build socket arrays
	clientsockincount=0;
	clientsockuncount=0;
	linkedlistnode< listenercontainer * >	*node;
	for (node=listenerlist->getFirst(); node; node=node->getNext()) {
		if (node->getValue()->getPort()) {
			clientsockincount=clientsockincount+
					node->getValue()->getAddressCount();
		}
		if (charstring::length(node->getValue()->getSocket())) {
			clientsockuncount=clientsockuncount+1;
		}
	}
	clientsockin=new inetsocketserver *[clientsockincount];
	clientsockinproto=new const char *[clientsockincount];
	clientsockinindex=0;
	clientsockun=new unixsocketserver *[clientsockuncount];
	clientsockunproto=new const char *[clientsockuncount];
	clientsockunindex=0;

	// listen on sockets
	bool	listening=false;
	for (node=listenerlist->getFirst(); node; node=node->getNext()) {
		if (listenOnClientSocket(node->getValue())) {
			listening=true;
		}
	}
	return listening;
}

bool sqlrlistener::listenOnClientSocket(listenercontainer *lc) {

	// init return value
	bool	listening=false;

	// get addresses/inet port and unix port to listen on
	const char * const *addresses=lc->getAddresses();
	uint16_t	port=lc->getPort();

	// attempt to listen on the inet ports (on each specified address)
	if (port && lc->getAddressCount()) {

		for (uint64_t index=0; index<lc->getAddressCount(); index++) {

			uint64_t	ind=clientsockinindex+index;
			clientsockin[ind]=new inetsocketserver();
			clientsockinproto[ind]=lc->getProtocol();

			// for the default protocol, use an empty string
			// the listener has to pass the protocol to the
			// connection and this makes that even faster for
			// the default protocol
			if (!charstring::compare(clientsockinproto[ind],
							DEFAULT_PROTOCOL)) {
				clientsockinproto[ind]="";
			}

			if (clientsockin[ind]->
					listen(addresses[index],port,15)) {
				addReadFileDescriptor(clientsockin[ind]);
				listening=true;
			} else {
				stringbuffer	info;
				info.append("failed to listen "
						"on client port: ");
				info.append(port);
				logInternalError(info.getString());

				char	*err=error::getErrorString();
				stderror.printf(
					"Could not listen "
					"on: %s/%d\n"
					"Error was: %s\n"
					"Make sure that no other "
					"processes are listening "
					"on that port.\n\n",
					addresses[index],port,err);
				delete[] err;

				delete clientsockin[ind];
				clientsockin[ind]=NULL;
			}

			clientsockinindex++;
		}
	}

	// attempt to listen on the unix socket
	if (charstring::length(lc->getSocket())) {

		clientsockun[clientsockunindex]=new unixsocketserver();
		clientsockunproto[clientsockunindex]=lc->getProtocol();

		// for the default protocol, use an empty string
		// the listener has to pass the protocol to the connection
		// and this makes that even faster for the default protocol
		if (!charstring::compare(clientsockunproto[clientsockunindex],
							DEFAULT_PROTOCOL)) {
			clientsockunproto[clientsockunindex]="";
		}

		if (clientsockun[clientsockunindex]->
				listen(lc->getSocket(),0000,15)) {
			addReadFileDescriptor(clientsockun[clientsockunindex]);
			listening=true;
		} else {
			stringbuffer	info;
			info.append("failed to listen on client socket: ");
			info.append(lc->getSocket());
			logInternalError(info.getString());

			stderror.printf("Could not listen on unix socket: ");
			stderror.printf("%s\n",lc->getSocket());
			stderror.printf("Make sure that the file and ");
			stderror.printf("directory are readable and writable.");
			stderror.printf("\n\n");

			delete clientsockun[clientsockunindex];
			clientsockun[clientsockunindex]=NULL;
		}

		clientsockunindex++;
	}

	return listening;
}

bool sqlrlistener::listenOnHandoffSocket(const char *id) {

	// the handoff socket
	size_t	handoffsocknamelen=
			charstring::length(sqlrpth->getSocketsDir())+
						charstring::length(id)+8+1;
	handoffsockname=new char[handoffsocknamelen];
	charstring::printf(handoffsockname,handoffsocknamelen,
				"%s%s-handoff",
				sqlrpth->getSocketsDir(),id);

	handoffsockun=new unixsocketserver();
	bool	success=handoffsockun->listen(handoffsockname,0066,15);

	if (success) {
		addReadFileDescriptor(handoffsockun);
	} else {
		stringbuffer	info;
		info.append("failed to listen on handoff socket: ");
		info.append(handoffsockname);
		logInternalError(info.getString());

		stderror.printf("Could not listen on unix socket: ");
		stderror.printf("%s\n",handoffsockname);
		stderror.printf("Make sure that the file and ");
		stderror.printf("directory are readable and writable.");
		stderror.printf("\n\n");
	}

	return success;
}

bool sqlrlistener::listenOnDeregistrationSocket(const char *id) {

	// the deregistration socket
	size_t	removehandoffsocknamelen=
			charstring::length(sqlrpth->getSocketsDir())+
						charstring::length(id)+14+1;
	removehandoffsockname=new char[removehandoffsocknamelen];
	charstring::printf(removehandoffsockname,removehandoffsocknamelen,
				"%s%s-removehandoff",
				sqlrpth->getSocketsDir(),id);

	removehandoffsockun=new unixsocketserver();
	bool	success=removehandoffsockun->listen(
						removehandoffsockname,0066,15);

	if (success) {
		addReadFileDescriptor(removehandoffsockun);
	} else {
		stringbuffer	info;
		info.append("failed to listen on deregistration socket: ");
		info.append(removehandoffsockname);
		logInternalError(info.getString());

		stderror.printf("Could not listen on unix socket: ");
		stderror.printf("%s\n",removehandoffsockname);
		stderror.printf("Make sure that the file and ");
		stderror.printf("directory are readable and writable.");
		stderror.printf("\n\n");
	}

	return success;
}

bool sqlrlistener::listenOnFixupSocket(const char *id) {

	// the fixup socket
	size_t	fixupsocknamelen=
			charstring::length(sqlrpth->getSocketsDir())+
						charstring::length(id)+6+1;
	fixupsockname=new char[fixupsocknamelen];
	charstring::printf(fixupsockname,fixupsocknamelen,
				"%s%s-fixup",
				sqlrpth->getSocketsDir(),id);

	fixupsockun=new unixsocketserver();
	bool	success=fixupsockun->listen(fixupsockname,0066,15);

	if (success) {
		addReadFileDescriptor(fixupsockun);
	} else {
		stringbuffer	info;
		info.append("failed to listen on fixup socket: ");
		info.append(fixupsockname);
		logInternalError(info.getString());

		stderror.printf("Could not listen on unix socket: ");
		stderror.printf("%s\n",fixupsockname);
		stderror.printf("Make sure that the file and ");
		stderror.printf("directory are readable and writable.");
		stderror.printf("\n\n");
	}

	return success;
}

void sqlrlistener::listen() {

	// wait until all of the connections have started
	for (;;) {
		int32_t	opendbconnections=shm->open_db_connections;

		if (opendbconnections<
			static_cast<int32_t>(cfg->getConnections())) {
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
		handleTraffic(waitForTraffic());
	}
}

filedescriptor *sqlrlistener::waitForTraffic() {

	logDebugMessage("waiting for traffic...");

	// wait for data on one of the sockets...
	// if something bad happened, return an invalid file descriptor
	if (listener::listen(-1,-1)<1) {
		return NULL;
	}

	// return first file descriptor that had data available or an invalid
	// file descriptor on error
	filedescriptor	*fd=
		listener::getReadReadyList()->getFirst()->getValue();

	logDebugMessage("finished waiting for traffic");

	return fd;
}


bool sqlrlistener::handleTraffic(filedescriptor *fd) {

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
		if (!clientsock) {
			return false;
		}
		clientsock->dontUseNaglesAlgorithm();
		return registerHandoff(clientsock);
	} else if (fd==removehandoffsockun) {
		clientsock=removehandoffsockun->accept();
		if (!clientsock) {
			return false;
		}
		clientsock->dontUseNaglesAlgorithm();
		return deRegisterHandoff(clientsock);
	} else if (fd==fixupsockun) {
		clientsock=fixupsockun->accept();
		if (!clientsock) {
			return false;
		}
		clientsock->dontUseNaglesAlgorithm();
		return fixup(clientsock);
	}

	// handle connections to the client sockets
	uint64_t 		csind=0;
	inetsocketserver	*iss=NULL;
	unixsocketserver	*uss=NULL;
	const char		*protocol=NULL;
	for (csind=0; csind<clientsockincount; csind++) {
		if (fd==clientsockin[csind]) {
			iss=clientsockin[csind];
			protocol=clientsockinproto[csind];
			break;
		}
	}
	if (!iss) {
		for (csind=0; csind<clientsockuncount; csind++) {
			if (fd==clientsockun[csind]) {
				uss=clientsockun[csind];
				protocol=clientsockunproto[csind];
				break;
			}
		}
	}

	if (iss) {

		clientsock=iss->accept();
		if (!clientsock) {
			return false;
		}
		clientsock->translateByteOrder();

		// For inet clients, make sure that the ip address is
		// not denied.  If the ip was denied, disconnect the
		// socket and loop back.
		if (denied && deniedIp(clientsock)) {
			delete clientsock;
			return true;
		}

	} else if (uss) {

		clientsock=uss->accept();
		if (!clientsock) {
			return false;
		}
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
		forkChild(clientsock,protocol);
	} else {
		incrementBusyListeners();
		clientSession(clientsock,protocol,NULL);
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
			retval=sock->passSocket(handoffsocklist[i].
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

	logDebugMessage("valid ip");

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
	clientsock->flushWriteBuffer(-1,-1);
	// FIXME: hmm, if the client is just spewing
	// garbage then we should close the connection...
	waitForClientClose(false,clientsock);
	delete clientsock;
}

struct clientsessionattr {
	thread		*thr;
	sqlrlistener	*lsnr;
	filedescriptor	*clientsock;
	const char	*protocol;
};

void sqlrlistener::forkChild(filedescriptor *clientsock, const char *protocol) {

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

	// if threads are supported, fork a thread
	// to handle the client connection
	if (usethreads) {

		// set up the thread
		thread			*thr=new thread;
		clientsessionattr	*csa=new clientsessionattr;
		csa->thr=thr;
		csa->lsnr=this;
		csa->clientsock=clientsock;
		csa->protocol=protocol;
		thr->setFunction((void*(*)(void*))clientSessionThread);
		thr->setArgument(csa);

		// run the thread
		if (thr->run()) {
			isforkedthread=true;
			return;
		}

		// error
		decrementBusyListeners();
		decrementForkedListeners();
		errorClientSession(clientsock,
			SQLR_ERROR_ERRORFORKINGLISTENER,
			SQLR_ERROR_ERRORFORKINGLISTENER_STRING);
		logInternalError(
			SQLR_ERROR_ERRORFORKINGLISTENER_STRING);
		delete csa;
		delete thr;
		return;
	}

	// if threads are not supported, fork a child
	// process to handle the client connection
	pid_t	childpid=process::fork();
	if (!childpid) {

		// child...
		isforkedchild=true;

		// since this is the forked off listener, we don't
		// want to actually remove the semaphore set or shared
		// memory segment when it exits
		shmem->dontRemove();
		semset->dontRemove();

		// re-init loggers
		if (sqlrlg) {
			sqlrlg->initLoggers(this,NULL);
		}

		clientSession(clientsock,protocol,NULL);

		decrementBusyListeners();
		decrementForkedListeners();

		cleanUp();
		process::exit(0);

	} else if (childpid>0) {

		// parent...
		if (sqlrlg) {
			debugstr.clear();
			debugstr.append("forked a child: ");
			debugstr.append((int32_t)childpid);
			logDebugMessage(debugstr.getString());
		}

		// the main process doesn't need to stay connected
		// to the client, only the forked process
		delete clientsock;

	} else {

		// error...
		decrementBusyListeners();
		decrementForkedListeners();
		errorClientSession(clientsock,
				SQLR_ERROR_ERRORFORKINGLISTENER,
				SQLR_ERROR_ERRORFORKINGLISTENER_STRING);
		logInternalError(SQLR_ERROR_ERRORFORKINGLISTENER_STRING);
	}
}

void sqlrlistener::clientSessionThread(void *attr) {
	clientsessionattr	*csa=(clientsessionattr *)attr;
	csa->thr->detach();
	csa->lsnr->clientSession(csa->clientsock,csa->protocol,csa->thr);
	csa->lsnr->decrementBusyListeners();
	csa->lsnr->decrementForkedListeners();
	delete csa->thr;
	delete csa;
}

void sqlrlistener::clientSession(filedescriptor *clientsock,
					const char *protocol, thread *thr) {

	if (dynamicscaling) {
		incrementConnectedClientCount();
	}

	bool	passstatus=handOffOrProxyClient(clientsock,protocol,thr);

	// If the handoff failed, decrement the connected client count.
	// If it had succeeded then the connection daemon would
	// decrement it later.
	if (dynamicscaling && !passstatus) {
		decrementConnectedClientCount();
	}

	// FIXME: hmm, if the client is just spewing
	// garbage then we should close the connection...
	waitForClientClose(passstatus,clientsock);

	delete clientsock;
}

bool sqlrlistener::handOffOrProxyClient(filedescriptor *sock,
						const char *protocol,
						thread *thr) {

	unixsocketclient	connectionsock;
	uint32_t		connectionpid;
	uint16_t		inetport;
	char 			unixportstr[MAXPATHLEN+1];
	uint16_t		unixportstrlen;
	bool			retval=false;

	// loop in case client doesn't get handed off successfully
	for (;;) {

		if (!getAConnection(&connectionpid,&inetport,
					unixportstr,&unixportstrlen,
					sock,thr)) {
			// fatal error occurred while getting a connection
			retval=false;
			break;
		}

		// Get the socket associated with the pid of the
		// available connection.
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

		// tell the connection what protocol to use
		connectionsock.write((uint16_t)charstring::length(protocol));
		connectionsock.write(protocol);

		if (handoffmode==HANDOFF_PASS) {

			// pass the file descriptor
			if (!connectionsock.passSocket(
					sock->getFileDescriptor())) {

				// this could fail if a connection
				// died because its ttl expired...

				logInternalError("failed to pass "
						"file descriptor");
				continue;
			}

		} else {

			// proxy the client
			if (!proxyClient(connectionpid,&connectionsock,sock)) {

				// this could fail if a connection
				// died because its ttl expired...
				continue;
			}
		}

		// If we got this far, everything worked.
		retval=true;
		break;
	}
		
	// Set the file descriptor to -1, otherwise it will get closed when
	// connectionsock is freed.  If the file descriptor gets closed, the
	// next time we try to pass a file descriptor to the same connection,
	// it will fail.
	connectionsock.setFileDescriptor(-1);

	return retval;
}

bool sqlrlistener::acquireShmAccess() {

	logDebugMessage("acquiring exclusive shm access");

	// don't even begin to wait if the alarm already rang
	if (alarmrang) {
		logDebugMessage("timeout occured");
		return false;
	}

	// Loop, waiting.  Bail if interrupted by an alarm.
	// FIXME: refactor this, similarly to controller
	bool	result=true;
	if (sys::signalsInterruptSystemCalls()) {
		semset->dontRetryInterruptedOperations();
		do {
			result=semset->waitWithUndo(1);
		} while (!result && error::getErrorNumber()==EINTR &&
							alarmrang!=1);
		semset->retryInterruptedOperations();
	} else {
		do {
			result=semset->waitWithUndo(1,0,500000000);
		} while (!result && alarmrang!=1);
	}

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
	if (!cfg->getWaitForDownDatabase()) {
		*alldbsdown=true;
		linkedlist< connectstringcontainer * >	*csl=
					cfg->getConnectStringList();
		for (linkedlistnode< connectstringcontainer * > *node=
						csl->getFirst(); node;
						node=node->getNext()) {
			connectstringcontainer	*cs=node->getValue();
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

	// Loop, waiting.  Bail if interrupted by an alarm.
	// FIXME: refactor this, similarly to controller
	bool	result=true;
	if (sys::signalsInterruptSystemCalls()) {
		semset->dontRetryInterruptedOperations();
		do {
			result=semset->wait(2);
		} while (!result && error::getErrorNumber()==EINTR &&
							alarmrang!=1);
		semset->retryInterruptedOperations();
	} else {
		do {
			result=semset->wait(2,0,500000000);
		} while (!result && alarmrang!=1);
	}

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
		logDebugMessage("failed to signal accepted connection");
		return false;
	}

	logDebugMessage("succeeded signalling accepted connection");
	return true;
}

void sqlrlistener::waitForConnectionToBeReadyForHandoff() {
	logDebugMessage("waiting for connection to be ready for handoff");
	semset->wait(12);
	logDebugMessage("done waiting for connection to be ready for handoff");
}

struct alarmthreadattr {
	thread		*alarmthr;
	thread		*mainthr;
	uint64_t	listenertimeout;
};

bool sqlrlistener::getAConnection(uint32_t *connectionpid,
					uint16_t *inetport,
					char *unixportstr,
					uint16_t *unixportstrlen,
					filedescriptor *sock,
					thread *thr) {

	// use an alarm, if we're a forked child and a timeout is set
	bool	usealarm=((isforkedthread || isforkedchild) && listenertimeout);

	for (;;) {

		logDebugMessage("getting a connection...");

		// set up an alarm thread, should we need it
		thread		*alarmthread=NULL;
		alarmthreadattr	*ata=NULL;

		// set an alarm
		bool	alarmon=false;
		if (usealarm) {
			alarmrang=0;
			if (isforkedthread) {
				alarmthread=new thread;
				ata=new alarmthreadattr;
				ata->alarmthr=alarmthread;
				ata->mainthr=thr;
				ata->listenertimeout=listenertimeout;
				alarmthread->setFunction(
					(void*(*)(void*))alarmThread);
				alarmthread->setArgument(ata);
				alarmthread->run();
			} else {
				signalmanager::alarm(listenertimeout);
			}
			alarmon=true;
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
				if (isforkedthread) {
					alarmthread->cancel();
				} else {
					signalmanager::alarm(0);
				}
				alarmon=false;
			}

			if (ok) {

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
		if (usealarm && alarmon) {
			if (isforkedthread) {
				alarmthread->cancel();
			} else {
				signalmanager::alarm(0);
			}
		}

		// clean up alarm thread stuff
		delete alarmthread;
		delete ata;

		// wait for the connection to let us know that it's ready
		// to have a client handed off to it
		waitForConnectionToBeReadyForHandoff();

		// execute this only if code above executed without errors...
		if (ok) {

			// make sure the connection is actually up...
			if (connectionIsUp(shm->connectionid)) {
				if (sqlrlg) {
					debugstr.clear();
					debugstr.append("finished getting "
							"a connection: ");
					debugstr.append(
						(int32_t)*connectionpid);
					logDebugMessage(debugstr.getString());
				}
				return true;
			}

			// if the connection wasn't up, fork a child to jog it,
			// spin back and get another connection
			logDebugMessage("connection was down");
			pingDatabase(*connectionpid,unixportstr,*inetport);
		}

		// return an error if the timeout was reached
		if (alarmrang) {
			logDebugMessage("failed to get "
					"a connection: timeout");
			sock->write((uint16_t)ERROR_OCCURRED_DISCONNECT);
			sock->write((uint64_t)SQLR_ERROR_HANDOFFFAILED);
			sock->write((uint16_t)charstring::length(
					SQLR_ERROR_HANDOFFFAILED_STRING));
			sock->write(SQLR_ERROR_HANDOFFFAILED_STRING);
			sock->flushWriteBuffer(-1,-1);
			return false;
		}

		// return an error if all db's were down
		if (alldbsdown) {
			logDebugMessage("failed to get "
					"a connection: all dbs were down");
			sock->write((uint16_t)ERROR_OCCURRED);
			sock->write((uint64_t)SQLR_ERROR_DBSDOWN);
			sock->write((uint16_t)charstring::length(
						SQLR_ERROR_DBSDOWN_STRING));
			sock->write(SQLR_ERROR_DBSDOWN_STRING);
			sock->flushWriteBuffer(-1,-1);
			return false;
		}
	}
}

void sqlrlistener::alarmThread(void *attr) {
	alarmthreadattr	*ata=(alarmthreadattr *)attr;
	ata->alarmthr->detach();
	snooze::macrosnooze(ata->listenertimeout);
	#ifdef SIGALRM
	ata->mainthr->raiseSignal(SIGALRM);
	#endif
}

bool sqlrlistener::connectionIsUp(const char *connectionid) {

	// initialize the database up/down filename
	size_t	updownlen=charstring::length(sqlrpth->getIpcDir())+
					charstring::length(cmdl->getId())+1+
					charstring::length(connectionid)+1;
	char	*updown=new char[updownlen];
	charstring::printf(updown,updownlen,
				"%s%s-%s",
				sqlrpth->getIpcDir(),
				cmdl->getId(),connectionid);
	bool	retval=file::exists(updown);
	delete[] updown;
	return retval;
}

struct pingdatabaseattr {
	thread		*thr;
	sqlrlistener	*lsnr;
	uint32_t	connectionpid;
	const char	*unixportstr;
	uint16_t	inetport;
};

void sqlrlistener::pingDatabase(uint32_t connectionpid,
					const char *unixportstr,
					uint16_t inetport) {

	// if threads are supported, fork a thread
	// to ping the database
	if (usethreads) {

		// set up the thread
		thread			*thr=new thread;
		pingdatabaseattr	*pda=new pingdatabaseattr;
		pda->thr=thr;
		pda->lsnr=this;
		pda->connectionpid=connectionpid;
		pda->unixportstr=unixportstr;
		pda->inetport=inetport;
		thr->setFunction((void*(*)(void*))pingDatabaseThread);
		thr->setArgument(pda);

		// run the thread
		thr->run();
		return;
	}

	// if threads are not supported, fork a child
	// process to ping the database
	pid_t	childpid=process::fork();
	if (!childpid) {
		isforkedchild=true;
		pingDatabaseInternal(connectionpid,unixportstr,inetport);
		cleanUp();
		process::exit(0);
	}
}

void sqlrlistener::pingDatabaseThread(void *attr) {
	pingdatabaseattr	*pda=(pingdatabaseattr *)attr;
	pda->thr->detach();
	pda->lsnr->pingDatabaseInternal(pda->connectionpid,
					pda->unixportstr,pda->inetport);
	delete pda->thr;
	delete pda;
}

void sqlrlistener::pingDatabaseInternal(uint32_t connectionpid,
					const char *unixportstr,
					uint16_t inetport) {
	unixsocketclient	connectionsock;
	if (findMatchingSocket(connectionpid,&connectionsock)) {
		connectionsock.write((uint16_t)HANDOFF_RECONNECT);
		connectionsock.flushWriteBuffer(-1,-1);
		snooze::macrosnooze(1);
	}
}

bool sqlrlistener::findMatchingSocket(uint32_t connectionpid,
					filedescriptor *connectionsock) {

	// Look through the list of handoff sockets for the pid of the 
	// connection that we got during the call to getAConnection().
	// When we find it, send the descriptor of the clientsock to the 
	// connection over the handoff socket associated with that node.
	for (uint32_t i=0; i<maxconnections; i++) {
		if (handoffsocklist[i].pid==connectionpid) {
			connectionsock->setFileDescriptor(
						handoffsocklist[i].
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
	unixsocketclient	fixupclientsockun;
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
	fixupclientsockun.flushWriteBuffer(-1,-1);

	// get the file descriptor of the socket
	int32_t	fd;
	if (!fixupclientsockun.receiveSocket(&fd)) {
		logInternalError("fixup failed to receive socket");
		return false;
	}
	connectionsock->setFileDescriptor(fd);

	// On most systems, the file descriptor is in whatever mode
	// it was in the other process, but on FreeBSD < 5.0 and
	// possibly other systems, it ends up in non-blocking mode
	// in this process, independent of its mode in the other
	// process.  So, we force it to blocking mode here.
	connectionsock->useBlockingMode();

	logDebugMessage("received socket of newly spawned connection");
	return true;
}

bool sqlrlistener::proxyClient(pid_t connectionpid,
				filedescriptor *serversock,
				filedescriptor *clientsock) {

	logDebugMessage("proxying client...");

	// send the connection our PID
	serversock->write((uint32_t)process::getProcessId());
	serversock->flushWriteBuffer(-1,-1);

	// wait up to 5 seconds for a response
	unsigned char	ack=0;
	if (serversock->read(&ack,5,0)!=sizeof(unsigned char)) {
		logDebugMessage("proxying client failed: "
				"failed to receive ack");
		return false;
	}
	#define ACK	6
	if (ack!=ACK) {
		logDebugMessage("proxying client failed: "
				"received bad ack");
		return false;
	}

	// allow short reads and use non blocking mode
	serversock->allowShortReads();
	serversock->useNonBlockingMode();
	clientsock->allowShortReads();
	clientsock->useNonBlockingMode();

	// Set up a listener to listen on both client and server sockets.
	listener	proxy;
	proxy.addReadFileDescriptor(serversock);
	proxy.addReadFileDescriptor(clientsock);

	// set up a read buffer
	unsigned char	readbuffer[8192];

	// should we send an end session command to the connection?
	bool	endsession=false;

	for (;;) {

		// wait for data to be available from the client or server
		error::clearError();
		int32_t	waitcount=proxy.listen(-1,-1);

		// The wait fell through but nobody had data.  This is just here
		// for good measure now.  I'm not sure what could cause this.
		// I originally thought it could happen if one side or the other
		// closed the socket, but it appears in that case, the wait does		// fall through with the side that closed the socket indicated
		// as ready and then the read fails.
		if (waitcount<1) {
			logDebugMessage("wait exited with no data");
			endsession=true;
			break;
		}

		// get the file descriptor that data was available from
		filedescriptor	*fd=
			proxy.getReadReadyList()->getFirst()->getValue();

		// read whatever data was available
		ssize_t	readcount=fd->read(readbuffer,sizeof(readbuffer));
		if (readcount<1) {
			if (sqlrlg) {
				debugstr.clear();
				debugstr.append("read failed: ");
				debugstr.append((uint32_t)readcount);
				debugstr.append(" : ");
				char	*err=error::getErrorString();
				debugstr.append(err);
				delete[] err;
				logDebugMessage(debugstr.getString());
			}
			endsession=(fd==clientsock);
			break;
		}

		// write the data to the other side
		if (fd==serversock) {
			if (sqlrlg) {
				debugstr.clear();
				debugstr.append("read ");
				debugstr.append((uint32_t)readcount);
				debugstr.append(" bytes from server");
				logDebugMessage(debugstr.getString());
			}
			clientsock->write(readbuffer,readcount);
			clientsock->flushWriteBuffer(-1,-1);
		} else if (fd==clientsock) {
			if (sqlrlg) {
				debugstr.clear();
				debugstr.append("read ");
				debugstr.append((uint32_t)readcount);
				debugstr.append(" bytes from client");
				logDebugMessage(debugstr.getString());
			}
			serversock->write(readbuffer,readcount);
			serversock->flushWriteBuffer(-1,-1);
		}
	}

	// If the client closed the socket then we can't be sure whether it
	// succeeded in transmitting an END_SESSION command or whether it even
	// tried.  The connection daemon would usually detect the socket close
	// and end the session but since we're not closing any socket, we'll
	// send an END_SESSION ourselves.  Worst case, the server will receive
	// a second END_SESSION, but we'll kludge it to tolerate that.
	if (endsession) {
		logDebugMessage("ending the session");
		// translate byte order for this, as the client would
		serversock->translateByteOrder();
		serversock->write((uint16_t)END_SESSION);
		serversock->flushWriteBuffer(-1,-1);
		serversock->dontTranslateByteOrder();
	}

	// set everything back to normal
	serversock->dontAllowShortReads();
	serversock->useBlockingMode();
	clientsock->dontAllowShortReads();
	clientsock->useBlockingMode();

	logDebugMessage("finished proxying client");

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
	#ifdef SIGALRM
	alarmhandler.handleSignal(SIGALRM);
	#endif
}

const char *sqlrlistener::getId() {
	return cmdl->getId();
}

const char *sqlrlistener::getLogDir() {
	return sqlrpth->getLogDir();
}

const char *sqlrlistener::getDebugDir() {
	return sqlrpth->getDebugDir();
}
