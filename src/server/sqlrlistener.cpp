// Copyright (c) 1999-2018 David Muse
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
#include <rudiments/directory.h>
#include <rudiments/error.h>
#include <rudiments/datetime.h>
#include <rudiments/sys.h>
#include <rudiments/stdio.h>
#include <rudiments/thread.h>
#include <rudiments/semaphoreset.h>
#include <rudiments/sharedmemory.h>
#include <rudiments/unixsocketserver.h>
#include <rudiments/inetsocketserver.h>
#include <rudiments/listener.h>

#include <config.h>
#include <defaults.h>
#include <defines.h>

#ifndef MAXPATHLEN
	#define MAXPATHLEN	256
#endif

class SQLRSERVER_DLLSPEC handoffsocketnode {
	friend class sqlrlistener;
	private:
		uint32_t	pid;
		filedescriptor	*sock;
};

class sqlrlistenerprivate {
	friend class sqlrlistener;
	private:
		listener	_lsnr;

		uint32_t	_maxconnections;
		bool		_dynamicscaling;

		int64_t		_maxlisteners;
		uint64_t	_listenertimeout;

		char		*_pidfile;

		sqlrcmdline	*_cmdl;
		sqlrpaths	*_sqlrpth;
		sqlrconfigs	*_sqlrcfgs;
		sqlrconfig	*_cfg;

		sqlrloggers		*_sqlrlg;
		sqlrnotifications	*_sqlrn;

		semaphoreset	*_semset;
		sharedmemory	*_shmem;
		sqlrshm		*_shm;
		char		*_idfilename;

		bool	_initialized;

		inetsocketserver	**_clientsockin;
		uint16_t		*_clientsockinprotoindex;
		uint64_t		_clientsockincount;
		uint64_t		_clientsockinindex;

		unixsocketserver	**_clientsockun;
		uint16_t		*_clientsockunprotoindex;
		uint64_t		_clientsockuncount;
		uint64_t		_clientsockunindex;

		unixsocketserver	*_handoffsockun;
		char			*_handoffsockname;
		unixsocketserver	*_removehandoffsockun;
		char			*_removehandoffsockname;
		unixsocketserver	*_fixupsockun;
		char			*_fixupsockname;

		uint16_t		_handoffmode;
		handoffsocketnode	*_handoffsocklist;

		regularexpression	*_allowed;
		regularexpression	*_denied;

		uint32_t	_maxquerysize;
		uint16_t	_maxbindcount;
		uint16_t	_maxbindnamelength;
		int32_t		_idleclienttimeout;

		bool	_isforkedchild;
		bool	_isforkedthread;

		bool	_usethreads;
};

static signalhandler		alarmhandler;
static volatile sig_atomic_t	alarmrang=0;

sqlrlistener::sqlrlistener() {

	pvt=new sqlrlistenerprivate;

	pvt->_cmdl=NULL;
	pvt->_sqlrcfgs=NULL;
	pvt->_cfg=NULL;

	pvt->_initialized=false;

	pvt->_sqlrlg=NULL;
	pvt->_sqlrn=NULL;

	pvt->_semset=NULL;
	pvt->_shmem=NULL;
	pvt->_shm=NULL;
	pvt->_idfilename=NULL;

	pvt->_pidfile=NULL;
	pvt->_sqlrpth=NULL;

	pvt->_clientsockin=NULL;
	pvt->_clientsockinprotoindex=NULL;
	pvt->_clientsockincount=0;
	pvt->_clientsockinindex=0;
	pvt->_clientsockun=NULL;
	pvt->_clientsockunprotoindex=NULL;
	pvt->_clientsockuncount=0;
	pvt->_clientsockunindex=0;

	pvt->_handoffsockun=NULL;
	pvt->_handoffsockname=NULL;
	pvt->_removehandoffsockun=NULL;
	pvt->_removehandoffsockname=NULL;
	pvt->_fixupsockun=NULL;
	pvt->_fixupsockname=NULL;

	pvt->_handoffsocklist=NULL;

	pvt->_denied=NULL;
	pvt->_allowed=NULL;

	pvt->_maxquerysize=0;
	pvt->_maxbindcount=0;
	pvt->_maxbindnamelength=0;
	pvt->_idleclienttimeout=-1;

	pvt->_isforkedchild=false;
	pvt->_isforkedthread=false;
	pvt->_handoffmode=HANDOFF_PASS;

	pvt->_usethreads=false;
}

sqlrlistener::~sqlrlistener() {
	if (!pvt->_isforkedchild && pvt->_idfilename) {
		file::remove(pvt->_idfilename);
	}
	delete[] pvt->_idfilename;

	if (!pvt->_isforkedchild) {
		if (pvt->_cfg && !pvt->_cfg->getListeners()->isNullNode()) {
			for (domnode *node=
				pvt->_cfg->getListeners()->
					getFirstTagChild("listener");
				!node->isNullNode();
				node=node->getNextTagSibling("listener")) {
				const char	*unixport=
					node->getAttributeValue("socket");
				if (!charstring::isNullOrEmpty(unixport)) {
					file::remove(unixport);
				}
			}
		}
		if (pvt->_pidfile) {
			file::remove(pvt->_pidfile);
		}
	}
	if (pvt->_initialized) {
		cleanUp();
	}

	// remove files that indicate whether the db is up or down
	if (pvt->_cfg && pvt->_cfg->getConnectStringList()) {
		for (linkedlistnode< connectstringcontainer * > *node=
				pvt->_cfg->getConnectStringList()->getFirst();
				node; node=node->getNext()) {
			connectstringcontainer	*cs=node->getValue();
			const char	*connectionid=cs->getConnectionId();
			char	*updown=NULL;
			charstring::printf(&updown,"%s%s-%s.up",
						pvt->_sqlrpth->getIpcDir(),
						pvt->_cmdl->getId(),
						connectionid);
			file::remove(updown);
			delete[] updown;
		}
	}
	delete pvt->_sqlrpth;
	delete pvt->_sqlrcfgs;
	delete pvt->_cmdl;

	delete pvt->_shmem;

	// Delete the semset last...
	// If the listener is killed while waiting on a semaphore, sometimes
	// the signal doesn't interrupt the wait, and sometimes removing
	// the semaphore during the wait causes a segfault.  The shutdown
	// process catches this and exits, but lets make sure that everything
	// else is cleaned up before this can even happen.
	delete pvt->_semset;

	delete pvt;
}

void sqlrlistener::cleanUp() {

	delete[] pvt->_pidfile;

	uint64_t	csind;
	for (csind=0; csind<pvt->_clientsockincount; csind++) {
		delete pvt->_clientsockin[csind];
	}
	delete[] pvt->_clientsockin;
	delete[] pvt->_clientsockinprotoindex;
	for (csind=0; csind<pvt->_clientsockuncount; csind++) {
		delete pvt->_clientsockun[csind];
	}
	delete[] pvt->_clientsockun;
	delete[] pvt->_clientsockunprotoindex;

	if (!pvt->_isforkedchild && pvt->_handoffsockname) {
		file::remove(pvt->_handoffsockname);
	}
	delete[] pvt->_handoffsockname;
	delete pvt->_handoffsockun;

	if (pvt->_handoffsocklist) {
		for (uint32_t i=0; i<pvt->_maxconnections; i++) {
			delete pvt->_handoffsocklist[i].sock;
		}
		delete[] pvt->_handoffsocklist;
	}

	if (!pvt->_isforkedchild && pvt->_removehandoffsockname) {
		file::remove(pvt->_removehandoffsockname);
	}
	delete[] pvt->_removehandoffsockname;
	delete pvt->_removehandoffsockun;

	if (!pvt->_isforkedchild && pvt->_fixupsockname) {
		file::remove(pvt->_fixupsockname);
	}
	delete[] pvt->_fixupsockname;
	delete pvt->_fixupsockun;

	delete pvt->_denied;
	delete pvt->_allowed;
	delete pvt->_sqlrlg;
	delete pvt->_sqlrn;
}

bool sqlrlistener::init(int argc, const char **argv) {

	pvt->_initialized=true;

	pvt->_cmdl=new sqlrcmdline(argc,argv);
	pvt->_sqlrpth=new sqlrpaths(pvt->_cmdl);
	pvt->_sqlrcfgs=new sqlrconfigs(pvt->_sqlrpth);

	// The tmpdir his is often in /run or /var/run, which is often a tmpfs,
	// at least on Linux.  So, it's blown away with each reboot.  Re-create
	// it if it doesn't exist.
#ifdef WIN32
	const char	*slash="\\";
#else
	const char	*slash="/";
#endif
	const char	*tmpdir=pvt->_sqlrpth->getTmpDir();
	if (!file::exists(tmpdir)) {
		char		**parts=NULL;
		uint64_t	partcount=0;
		charstring::split(tmpdir,slash,true,&parts,&partcount);
		stringbuffer	path;
		for (uint64_t i=0; i<partcount; i++) {
			path.append(slash)->append(parts[i]);
			if (!file::exists(path.getString())) {
				mode_t	mode=(i==partcount-1)?
					permissions::evalPermString(
							"rwxrwxrwx"):
					permissions::evalPermString(
							"rwxr-xr-x");
				mode_t	oldumask=
					process::setFileCreationMask(0000);
				directory::create(path.getString(),mode);
				process::setFileCreationMask(oldumask);
			}
		}
		for (uint64_t i=0; i<partcount; i++) {
			delete[] parts[i];
		}
		delete[] parts;
	}

	if (!charstring::compare(pvt->_cmdl->getId(),DEFAULT_ID)) {
		stderror.printf("Warning: using default id.\n");
	}

	pvt->_cfg=pvt->_sqlrcfgs->load(pvt->_sqlrpth->getConfigUrl(),
							pvt->_cmdl->getId());
	if (!pvt->_cfg) {
		return false;
	}

	setUserAndGroup();

	if (!verifyAccessToConfigUrl(pvt->_sqlrpth->getConfigUrl())) {
		return false;
	}

	if (!handlePidFile(pvt->_cmdl->getId())) {
		return false;
	}

	handleDynamicScaling();

	domnode	*loggers=pvt->_cfg->getLoggers();
	if (!loggers->isNullNode()) {
		pvt->_sqlrlg=new sqlrloggers(pvt->_sqlrpth);
		pvt->_sqlrlg->load(loggers);
		pvt->_sqlrlg->init(this,NULL);
	}

	domnode	*notifications=pvt->_cfg->getNotifications();
	if (!notifications->isNullNode()) {
		pvt->_sqlrn=new sqlrnotifications(pvt->_sqlrpth);
		pvt->_sqlrn->load(notifications);
	}

	pvt->_idleclienttimeout=pvt->_cfg->getIdleClientTimeout();
	pvt->_maxquerysize=pvt->_cfg->getMaxQuerySize();
	pvt->_maxbindcount=pvt->_cfg->getMaxBindCount();
	pvt->_maxbindnamelength=pvt->_cfg->getMaxBindNameLength();
	pvt->_maxlisteners=pvt->_cfg->getMaxListeners();
	pvt->_listenertimeout=pvt->_cfg->getListenerTimeout();

	// if dynamic scaling is enabled then we need to adjust the
	// listenertimeout to accomodate attempts to start connections, plus
	// a little grace
	uint64_t	mintimeout=DEFAULT_CONNECTION_START_ATTEMPTS*
					(DEFAULT_CONNECTION_START_TIMEOUT+2);
	if (pvt->_dynamicscaling &&
			pvt->_listenertimeout &&
			pvt->_listenertimeout<mintimeout) {
		pvt->_listenertimeout=mintimeout;
	}

	setHandoffMethod();

	setSessionHandlerMethod();

	setIpPermissions();

	if (!createSharedMemoryAndSemaphores(pvt->_cmdl->getId())) {
		return false;
	}

	if (!listenOnHandoffSocket(pvt->_cmdl->getId())) {
		return false;
	}
	if (!listenOnDeregistrationSocket(pvt->_cmdl->getId())) {
		return false;
	}
	if (!listenOnFixupSocket(pvt->_cmdl->getId())) {
		return false;
	}

	if (!pvt->_cmdl->found("-nodetach")) {
		process::detach();
	}

	process::createPidFile(pvt->_pidfile,permissions::ownerReadWrite());

	setMaxListeners(pvt->_maxlisteners);

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

	stringbuffer	errorstr;

	// switch groups, but only if we're not currently running as the
	// group that we should switch to
	if (charstring::compare(currentgroup,pvt->_cfg->getRunAsGroup()) &&
			!process::setGroup(pvt->_cfg->getRunAsGroup())) {
		errorstr.append("Warning: could not change group to ")->
			append(pvt->_cfg->getRunAsGroup())->append('\n');
	}

	// switch users, but only if we're not currently running as the
	// user that we should switch to
	if (charstring::compare(currentuser,pvt->_cfg->getRunAsUser()) &&
			!process::setUser(pvt->_cfg->getRunAsUser())) {
		errorstr.append("Warning: could not change user to ")->
			append(pvt->_cfg->getRunAsUser())->append('\n');
	}

	// write the error, if there was one
	stderror.write(errorstr.getString(),errorstr.getStringLength());

	// clean up
	delete[] currentuser;
	delete[] currentgroup;
}

bool sqlrlistener::verifyAccessToConfigUrl(const char *url) {

	if (!pvt->_cfg->getDynamicScaling()) {
		return true;
	}

	if (!pvt->_cfg->accessible()) {
		stderror.printf("\n%s-listener error:\n"
				"	This instance of %s is "
				"configured to run as:\n"
				"		user: %s\n"
				"		group: %s\n\n"
				"	However, the config url %s\n"
				"	cannot be accessed by that user "
				"or group.\n\n"
				"	Since you're using dynamic scaling "
				"(ie. maxconnections>connections),\n"
				"	new connections would be started as\n"
				"		user: %s\n"
				"		group: %s\n\n"
				"	They would not be able to access the"
				"config url and would shut down.\n\n"
				"	To remedy this problem, make %s\n"
				"	accessible by\n"
				"		user: %s\n"
				"		group: %s\n"
				SQLR,
				SQL_RELAY,
				pvt->_cfg->getRunAsUser(),
				pvt->_cfg->getRunAsGroup(),
				url,
				pvt->_cfg->getRunAsUser(),
				pvt->_cfg->getRunAsGroup(),
				url,
				pvt->_cfg->getRunAsUser(),
				pvt->_cfg->getRunAsGroup());
		return false;
	}
	return true;
}

bool sqlrlistener::handlePidFile(const char *id) {

	// check/set pid file
	charstring::printf(&pvt->_pidfile,
				"%ssqlr-listener-%s.pid",
				pvt->_sqlrpth->getPidDir(),id);

	if (process::checkForPidFile(pvt->_pidfile)!=-1) {
		stderror.printf("\n%s-listener error:\n"
				"	The pid file %s"
				" exists.\n"
				"	This usually means that the "
				"%s-listener is already running for "
				"the \n"
				"	%s"
				" instance.\n"
				"	If it is not running, please remove "
				"the file and restart.\n",
				SQLR,pvt->_pidfile,SQLR,id);
		delete[] pvt->_pidfile;
		pvt->_pidfile=NULL;
		return false;
	}
	return true;
}

void sqlrlistener::handleDynamicScaling() {

	// get the dynamic connection scaling parameters
	pvt->_maxconnections=pvt->_cfg->getMaxConnections();

	// if dynamic scaling isn't going to be used, disable it
	pvt->_dynamicscaling=pvt->_cfg->getDynamicScaling();
}

void sqlrlistener::setSessionHandlerMethod() {
	
	pvt->_usethreads=false;
	if (!charstring::compare(pvt->_cfg->getSessionHandler(),"thread")) {

		if (!thread::supported()) {
			stderror.printf("Warning: sessionhandler=\"thread\" "
					"not supported, falling back to "
					"sessionhandler=\"process\".  "
					"Either threads are not supported on "
					"this platform or Rudiments was "
					"compiled without support for threads."
					"\n");
			return;
		}

		pvt->_usethreads=true;

	} else {

		if (!process::supportsFork()) {
			stderror.printf("Warning: sessionhandler=\"process\" "
					"not supported on this platform, "
					"falling back to "
					"sessionhandler=\"thread\".\n");
			pvt->_usethreads=true;
		}
	}
}

void sqlrlistener::setHandoffMethod() {

	if (!charstring::compare(pvt->_cfg->getHandoff(),"pass")) {

        	// on some OS'es, force proxy, even if pass was specified...

        	// get the os and version
        	char    *os=sys::getOperatingSystemName();
        	char    *rel=sys::getOperatingSystemRelease();
        	double  ver=charstring::toFloatC(rel);
	
        	// force proxy for Cygwin and Linux < 2.2
        	if (!charstring::compare(os,"CYGWIN",6) ||
                	(!charstring::compare(os,"Linux",5) && ver<2.2)) {
			pvt->_handoffmode=HANDOFF_PROXY;
			stderror.printf("Warning: handoff=\"pass\" not "
					"supported, falling back to "
					"handoff=\"proxy\".\n");
        	} else {
			pvt->_handoffmode=HANDOFF_PASS;
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
			pvt->_handoffmode=HANDOFF_PASS;
			stderror.printf("Warning: handoff=\"proxy\" not "
					"supported, falling back to "
					"handoff=\"pass\".\n");
		} else {
			pvt->_handoffmode=HANDOFF_PROXY;
		}

        	// clean up
        	delete[] os;
	}

	// create the list of handoff nodes
	pvt->_handoffsocklist=new handoffsocketnode[pvt->_maxconnections];
	for (uint32_t i=0; i<pvt->_maxconnections; i++) {
		pvt->_handoffsocklist[i].pid=0;
		pvt->_handoffsocklist[i].sock=NULL;
	}
}

void sqlrlistener::setIpPermissions() {

	// get denied and allowed ip's and compile the expressions
	const char	*deniedips=pvt->_cfg->getDeniedIps();
	const char	*allowedips=pvt->_cfg->getAllowedIps();
	if (!charstring::isNullOrEmpty(deniedips)) {
		pvt->_denied=new regularexpression(deniedips);
	}
	if (!charstring::isNullOrEmpty(allowedips)) {
		pvt->_allowed=new regularexpression(allowedips);
	}
}

bool sqlrlistener::createSharedMemoryAndSemaphores(const char *id) {

	// initialize the ipc filename
	charstring::printf(&pvt->_idfilename,
				"%s%s.ipc",pvt->_sqlrpth->getIpcDir(),id);

	if (pvt->_sqlrlg || pvt->_sqlrn) {
		stringbuffer	debugstr;
		debugstr.append("creating shared memory "
					"and semaphores: id filename: ");
		debugstr.append(pvt->_idfilename);
		raiseDebugMessageEvent(debugstr.getString());
	}

	// make sure that the file exists and is read/writeable
	if (!file::createFile(pvt->_idfilename,permissions::ownerReadWrite())) {
		ipcFileError(pvt->_idfilename);
		return false;
	}

	// get the ipc key
	key_t	key=file::generateKey(pvt->_idfilename,1);
	if (key==-1) {
		keyError(pvt->_idfilename);
		return false;
	}

	// create the shared memory segment
	// FIXME: if it already exists, attempt to remove and re-create it
	raiseDebugMessageEvent("creating shared memory...");

	pvt->_shmem=new sharedmemory;
	if (!pvt->_shmem->create(key,sizeof(sqlrshm),
				permissions::evalPermString("rw-r-----"))) {
		shmError(id,pvt->_shmem->getId());
		pvt->_shmem->attach(key,sizeof(sqlrshm));
		return false;
	}
	pvt->_shm=(sqlrshm *)pvt->_shmem->getPointer();
	bytestring::zero(pvt->_shm,sizeof(sqlrshm));

	setStartTime();

	// create (or connect) to the semaphore set
	// FIXME: if it already exists, attempt to remove and re-create it
	raiseDebugMessageEvent("creating semaphores...");

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
	pvt->_semset=new semaphoreset();
	if (!pvt->_semset->create(key,permissions::ownerReadWrite(),13,vals)) {
		semError(id,pvt->_semset->getId());
		pvt->_semset->attach(key,13);
		return false;
	}

	// issue warning about ttl if necessary
	if (pvt->_cfg->getTtl()>0 &&
		!pvt->_semset->supportsTimedSemaphoreOperations() &&
		!sys::signalsInterruptSystemCalls()) {
		stderror.printf("Warning: ttl forced to 0...\n"
				"         semaphore waits cannot be "
				"interrupted:\n"
				"         system doesn't support timed "
				"semaphore operations and\n"
				"         signals don't interrupt system "
				"calls\n");
	}

	// issue warning about listener timeout if necessary
	if (pvt->_cfg->getListenerTimeout()>0 &&
		!charstring::compare(pvt->_cfg->getSessionHandler(),"thread") &&
		thread::supported() &&
		!pvt->_semset->supportsTimedSemaphoreOperations()) {
		stderror.printf("Warning: listenertimeout disabled...\n"
				"         sessionhandler=\"thread\" requested "
				"(or defaulted) but system doesn't\n"
				"         support timed semaphore "
				"operations\n");
	}

	return true;
}

void sqlrlistener::ipcFileError(const char *idfilename) {

	char	*currentuser=
		userentry::getName(process::getEffectiveUserId());
	char	*currentgroup=
		groupentry::getName(process::getEffectiveGroupId());

	stderror.printf("Could not open: %s\n"
			"Make sure that the directory "
			"is writable by %s:%s.\n\n",
			idfilename,currentuser,currentgroup);
	delete[] currentuser;
	delete[] currentgroup;
}

void sqlrlistener::keyError(const char *idfilename) {
	char	*err=error::getErrorString();
	stderror.printf("\n%s-listener error:\n"
			"	Unable to generate a key from "
			"%s\n"
			"	Error was: %s\n\n",
			SQLR,idfilename,err);
	delete[] err;
}

void sqlrlistener::shmError(const char *id, int shmid) {
	char	*err=error::getErrorString();
	stderror.printf("\n%s-listener error:\n"
			"	Unable to create a shared memory "
			"segment.  This is usally because an \n"
			"	%s-listener is already running for "
			"the %s instance.\n\n"
			"	If it is not running, something may "
			"have crashed and left an old segment\n"
			"	lying around.  Use the ipcs command "
			"to inspect existing shared memory \n"
			"	segments and the ipcrm command to "
			"remove the shared memory segment with "
			"\n	id %d.\n\n"
			"	Error was: %s\n\n",
			SQLR,SQLR,id,shmid,err);
	delete[] err;
}

void sqlrlistener::semError(const char *id, int semid) {
	char	*err=error::getErrorString();
	stderror.printf("\n%s-listener error:\n"
			"	Unable to create a semaphore "
			"set.  This is usally because an \n"
			"	%s-listener is already "
			"running for the %s"
			" instance.\n\n"
			"	If it is not running, "
			"something may have crashed and left "
			"an old semaphore set\n"
			"	lying around.  Use the ipcs "
			"command to inspect existing "
			"semaphore sets \n"
			"	and the ipcrm "
			"command to remove the semaphore set "
			"with \n"
			"	id %d.\n\n"
			"	Error was: %s\n\n"
			SQLR,SQLR,id,semid,err);
	delete[] err;
}

bool sqlrlistener::listenOnClientSockets() {

	domnode	*listenerlist=pvt->_cfg->getListeners();

	// count sockets and build socket arrays
	pvt->_clientsockincount=0;
	pvt->_clientsockuncount=0;
	for (domnode	*node=listenerlist->getFirstTagChild("listener");
			!node->isNullNode();
			node=node->getNextTagSibling("listener")) {
		uint64_t	addrcount=0;
		charstring::split(node->getAttributeValue("addresses"),
						",",true,NULL,&addrcount);
		if (!charstring::isNullOrEmpty(
				node->getAttributeValue("port"))) {
			pvt->_clientsockincount=
				pvt->_clientsockincount+addrcount;
		}
		if (!charstring::isNullOrEmpty(
				node->getAttributeValue("socket"))) {
			pvt->_clientsockuncount=pvt->_clientsockuncount+1;
		}
	}
	pvt->_clientsockin=new inetsocketserver *[pvt->_clientsockincount];
	pvt->_clientsockinprotoindex=new uint16_t[pvt->_clientsockincount];
	pvt->_clientsockinindex=0;
	pvt->_clientsockun=new unixsocketserver *[pvt->_clientsockuncount];
	pvt->_clientsockunprotoindex=new uint16_t[pvt->_clientsockuncount];
	pvt->_clientsockunindex=0;

	// listen on sockets
	bool		listening=false;
	uint16_t	protocolindex=0;
	for (domnode	*node=listenerlist->getFirstTagChild("listener");
			!node->isNullNode();
			node=node->getNextTagSibling("listener")) {
		if (listenOnClientSocket(protocolindex,node)) {
			listening=true;
		}
		protocolindex++;
	}
	return listening;
}

bool sqlrlistener::listenOnClientSocket(uint16_t protocolindex,
						domnode *ln) {

	// init return value
	bool	listening=false;

	// get addresses/inet port and unix port to listen on
	const char	*addresses=ln->getAttributeValue("addresses");
	uint16_t	port=charstring::toUnsignedInteger(
					ln->getAttributeValue("port"));

	// split addresses
	char		**addr=NULL;
	uint64_t	addrcount=0;
	charstring::split(addresses,",",true,&addr,&addrcount);

	// trim them too
	uint64_t	index;
	for (index=0; index<addrcount; index++) {
		charstring::bothTrim(addr[index]);
	}

	// attempt to listen on the inet ports (on each specified address)
	if (port && addrcount) {

		for ( index=0; index<addrcount; index++) {

			uint64_t	ind=pvt->_clientsockinindex+index;
			pvt->_clientsockin[ind]=new inetsocketserver();
			pvt->_clientsockinprotoindex[ind]=protocolindex;

			if (pvt->_clientsockin[ind]->
					listen(addr[index],port,128)) {
				pvt->_lsnr.addReadFileDescriptor(
						pvt->_clientsockin[ind]);
				listening=true;
			} else {
				stringbuffer	info;
				info.append("failed to listen "
						"on client port: ");
				info.append(port);
				raiseInternalErrorEvent(info.getString());

				char	*err=error::getErrorString();
				stderror.printf(
					"Could not listen "
					"on: %s/%d\n"
					"Error was: %s\n"
					"Make sure that no other "
					"processes are listening "
					"on that port.\n\n",
					addr[index],port,err);
				delete[] err;

				delete pvt->_clientsockin[ind];
				pvt->_clientsockin[ind]=NULL;
			}

			pvt->_clientsockinindex++;
		}
	}

	// attempt to listen on the unix socket
	const char	*sock=ln->getAttributeValue("socket");
	if (!charstring::isNullOrEmpty(sock)) {

		pvt->_clientsockun[pvt->_clientsockunindex]=
						new unixsocketserver();
		pvt->_clientsockunprotoindex[pvt->_clientsockunindex]=
							protocolindex;

		if (pvt->_clientsockun[pvt->_clientsockunindex]->
						listen(sock,0000,128)) {
			pvt->_lsnr.addReadFileDescriptor(
				pvt->_clientsockun[pvt->_clientsockunindex]);
			listening=true;
		} else {
			stringbuffer	info;
			info.append("failed to listen on client socket: ");
			info.append(sock);
			raiseInternalErrorEvent(info.getString());

			char	*currentuser=
					userentry::getName(
						process::getEffectiveUserId());
			char	*currentgroup=
					groupentry::getName(
						process::getEffectiveGroupId());
			stderror.printf("Could not listen on unix socket: "
					"%s\n"
					"Make sure that the directory is "
					"writable by %s:%s.\n\n",
					sock,currentuser,currentgroup);
			delete[] currentuser;
			delete[] currentgroup;

			delete pvt->_clientsockun[pvt->_clientsockunindex];
			pvt->_clientsockun[pvt->_clientsockunindex]=NULL;
		}

		pvt->_clientsockunindex++;
	}

	// clean up addresses
	for (index=0; index<addrcount; index++) {
		delete[] addr[index];
	}

	return listening;
}

bool sqlrlistener::listenOnHandoffSocket(const char *id) {

	// the handoff socket
	charstring::printf(&pvt->_handoffsockname,
				"%s%s-handoff.sock",
				pvt->_sqlrpth->getSocketsDir(),id);

	pvt->_handoffsockun=new unixsocketserver();
	bool	success=pvt->_handoffsockun->listen(
				pvt->_handoffsockname,0077,128);

	if (success) {
		pvt->_lsnr.addReadFileDescriptor(pvt->_handoffsockun);
	} else {
		stringbuffer	info;
		info.append("failed to listen on handoff socket: ");
		info.append(pvt->_handoffsockname);
		raiseInternalErrorEvent(info.getString());

		char	*currentuser=userentry::getName(
						process::getEffectiveUserId());
		char	*currentgroup=groupentry::getName(
						process::getEffectiveGroupId());
		stderror.printf("Could not listen on unix socket: %s\n"
				"Make sure that the directory is "
				"writable by %s:%s.\n\n",
				pvt->_handoffsockname,currentuser,currentgroup);
		delete[] currentuser;
		delete[] currentgroup;
	}

	return success;
}

bool sqlrlistener::listenOnDeregistrationSocket(const char *id) {

	// the deregistration socket
	charstring::printf(&pvt->_removehandoffsockname,
				"%s%s-removehandoff.sock",
				pvt->_sqlrpth->getSocketsDir(),id);

	pvt->_removehandoffsockun=new unixsocketserver();
	bool	success=pvt->_removehandoffsockun->listen(
				pvt->_removehandoffsockname,0077,128);

	if (success) {
		pvt->_lsnr.addReadFileDescriptor(pvt->_removehandoffsockun);
	} else {
		stringbuffer	info;
		info.append("failed to listen on deregistration socket: ");
		info.append(pvt->_removehandoffsockname);
		raiseInternalErrorEvent(info.getString());

		char	*currentuser=userentry::getName(
						process::getEffectiveUserId());
		char	*currentgroup=groupentry::getName(
						process::getEffectiveGroupId());
		stderror.printf("Could not listen on unix socket: %s\n"
				"Make sure that the directory is "
				"writable by %s:%s.\n\n",
				pvt->_removehandoffsockname,
				currentuser,currentgroup);
		delete[] currentuser;
		delete[] currentgroup;
	}

	return success;
}

bool sqlrlistener::listenOnFixupSocket(const char *id) {

	// the fixup socket
	charstring::printf(&pvt->_fixupsockname,
				"%s%s-fixup.sock",
				pvt->_sqlrpth->getSocketsDir(),id);

	pvt->_fixupsockun=new unixsocketserver();
	bool	success=pvt->_fixupsockun->listen(pvt->_fixupsockname,0077,128);

	if (success) {
		pvt->_lsnr.addReadFileDescriptor(pvt->_fixupsockun);
	} else {
		stringbuffer	info;
		info.append("failed to listen on fixup socket: ");
		info.append(pvt->_fixupsockname);
		raiseInternalErrorEvent(info.getString());

		char	*currentuser=userentry::getName(
						process::getEffectiveUserId());
		char	*currentgroup=groupentry::getName(
						process::getEffectiveGroupId());
		stderror.printf("Could not listen on unix socket: %s\n"
				"Make sure that the directory is "
				"writable by %s:%s.\n\n",
				pvt->_fixupsockname,
				currentuser,currentgroup);
		delete[] currentuser;
		delete[] currentgroup;
	}

	return success;
}

void sqlrlistener::listen() {

	// wait until all of the connections have started
	for (;;) {
		int32_t	opendbconnections=pvt->_shm->open_db_connections;

		if (opendbconnections<
			static_cast<int32_t>(pvt->_cfg->getConnections())) {
			raiseDebugMessageEvent("waiting for server "
					"connections (sleeping 1s)");
			snooze::macrosnooze(1);
		} else {
			raiseDebugMessageEvent("finished waiting for "
					"server connections");
			break;
		}
	}

	// listen for client connections
	if (!listenOnClientSockets()) {
		return;
	}

	for(;;) {
		error::clearError();
		if (!handleTraffic(waitForTraffic()) &&
			error::getErrorNumber()==EMFILE) {
			snooze::macrosnooze(1);
		}
	}
}

filedescriptor *sqlrlistener::waitForTraffic() {

	raiseDebugMessageEvent("waiting for traffic...");

	// wait for data on one of the sockets...
	// if something bad happened, return an invalid file descriptor
	if (pvt->_lsnr.listen(-1,-1)<1) {
		return NULL;
	}

	// return first file descriptor that had data available or an invalid
	// file descriptor on error
	filedescriptor	*fd=
		pvt->_lsnr.getReadReadyList()->getFirst()->getValue();

	raiseDebugMessageEvent("finished waiting for traffic");

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
	if (fd==pvt->_handoffsockun) {
		clientsock=pvt->_handoffsockun->accept();
		if (!clientsock) {
			return false;
		}
		return registerHandoff(clientsock);
	} else if (fd==pvt->_removehandoffsockun) {
		clientsock=pvt->_removehandoffsockun->accept();
		if (!clientsock) {
			return false;
		}
		return deRegisterHandoff(clientsock);
	} else if (fd==pvt->_fixupsockun) {
		clientsock=pvt->_fixupsockun->accept();
		if (!clientsock) {
			return false;
		}
		return fixup(clientsock);
	}

	// handle connections to the client sockets
	uint64_t 		csind=0;
	inetsocketserver	*iss=NULL;
	unixsocketserver	*uss=NULL;
	uint16_t		protocolindex=0;
	for (csind=0; csind<pvt->_clientsockincount; csind++) {
		if (fd==pvt->_clientsockin[csind]) {
			iss=pvt->_clientsockin[csind];
			protocolindex=pvt->_clientsockinprotoindex[csind];
			break;
		}
	}
	if (!iss) {
		for (csind=0; csind<pvt->_clientsockuncount; csind++) {
			if (fd==pvt->_clientsockun[csind]) {
				uss=pvt->_clientsockun[csind];
				protocolindex=
					pvt->_clientsockunprotoindex[csind];
				break;
			}
		}
	}

	if (iss) {

		clientsock=iss->accept();
		if (!clientsock) {
			return false;
		}

		// For inet clients, make sure that the ip address is
		// not denied.  If the ip was denied, disconnect the
		// socket and loop back.
		if (pvt->_denied && deniedIp(clientsock)) {
			delete clientsock;
			return true;
		}

		clientsock->dontUseNaglesAlgorithm();
		clientsock->translateByteOrder();

	} else if (uss) {

		clientsock=uss->accept();
		if (!clientsock) {
			return false;
		}
		clientsock->translateByteOrder();

	} else {
		return true;
	}

	// Don't fork unless we have to.
	//
	// If there are no busy listeners and there are available connections,
	// then we don't need to fork a child.  Otherwise we do.
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
	if (pvt->_dynamicscaling ||
			getBusyListeners() ||
			!pvt->_semset->getValue(2)) {
		forkChild(clientsock,protocolindex);
	} else {
		incrementBusyListeners();
		clientSession(clientsock,protocolindex,NULL);
		decrementBusyListeners();
	}

	return true;
}


bool sqlrlistener::registerHandoff(filedescriptor *sock) {

	raiseDebugMessageEvent("registering handoff...");

	// get the connection daemon's pid
	uint32_t processid;
	if (sock->read(&processid)!=sizeof(uint32_t)) {
		raiseInternalErrorEvent("failed to read process "
					"id during registration");
		delete sock;
		return false;
	}

	// find a free node in the list, if we find another node with the
	// same pid, then the old connection must have died off mysteriously,
	// replace it
	bool		inserted=false;
	uint32_t	index=0;
	for (; index<pvt->_maxconnections; index++) {
		if (!pvt->_handoffsocklist[index].pid ||
			pvt->_handoffsocklist[index].pid==processid) {
			pvt->_handoffsocklist[index].pid=processid;
			pvt->_handoffsocklist[index].sock=sock;
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
				new handoffsocketnode[pvt->_maxconnections+1];
		for (uint32_t i=0; i<pvt->_maxconnections; i++) {
			newhandoffsocklist[i].pid=
				pvt->_handoffsocklist[i].pid;
			newhandoffsocklist[i].sock=
				pvt->_handoffsocklist[i].sock;
		}
		delete[] pvt->_handoffsocklist;
		newhandoffsocklist[pvt->_maxconnections].pid=processid;
		newhandoffsocklist[pvt->_maxconnections].sock=sock;
		pvt->_maxconnections++;
		pvt->_handoffsocklist=newhandoffsocklist;
	}

	raiseDebugMessageEvent("finished registering handoff...");
	return true;
}

bool sqlrlistener::deRegisterHandoff(filedescriptor *sock) {

	raiseDebugMessageEvent("de-registering handoff...");

	// get the connection daemon's pid
	uint32_t	processid;
	if (sock->read(&processid)!=sizeof(uint32_t)) {
		raiseInternalErrorEvent("failed to read process "
				"id during deregistration");
		delete sock;
		return false;
	}

	// remove the matching socket from the list
	for (uint32_t i=0; i<pvt->_maxconnections; i++) {
		if (pvt->_handoffsocklist[i].pid==processid) {
			pvt->_handoffsocklist[i].pid=0;
			delete pvt->_handoffsocklist[i].sock;
			pvt->_handoffsocklist[i].sock=NULL;
			break;
		}
	}

	// clean up
	delete sock;

	raiseDebugMessageEvent("finished de-registering handoff...");
	return true;
}

bool sqlrlistener::fixup(filedescriptor *sock) {

	raiseDebugMessageEvent("passing socket of newly spawned connection...");

	// get the pid of the connection daemon the child listener needs
	uint32_t	processid;
	if (sock->read(&processid)!=sizeof(uint32_t)) {
		raiseInternalErrorEvent("failed to read process id during fixup");
		delete sock;
		return false;
	}

	// look through the handoffsocklist for the pid
	bool	retval=false;
	for (uint32_t i=0; i<pvt->_maxconnections; i++) {
		if (pvt->_handoffsocklist[i].pid==processid) {
			retval=sock->passSocket(pvt->_handoffsocklist[i].
						sock->getFileDescriptor());
			raiseDebugMessageEvent("found socket for requested pid ");
			if (retval) {
				raiseDebugMessageEvent("passed it successfully");
			} else {
				raiseDebugMessageEvent("failed to pass it");
			}
			break;
		}
	}

	// clean up
	delete sock;

	raiseDebugMessageEvent("finished passing socket of newly spawned connection");

	return retval;
}

bool sqlrlistener::deniedIp(filedescriptor *clientsock) {

	raiseDebugMessageEvent("checking for valid ip...");

	char	*ip=clientsock->getPeerAddress();
	if (ip && pvt->_denied->match(ip) &&
			(!pvt->_allowed ||
			(pvt->_allowed && !pvt->_allowed->match(ip)))) {

		stringbuffer	info;
		info.append("rejected IP address: ")->append(ip);
		raiseClientConnectionRefusedEvent(info.getString());

		delete[] ip;
		return true;
	}

	raiseDebugMessageEvent("valid ip");

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
	uint16_t	protocolindex;
};

void sqlrlistener::forkChild(filedescriptor *clientsock,
					uint16_t protocolindex) {

	// increment the number of "forked listeners"
	// do this before we actually fork to prevent a race condition where
	// a bunch of children get forked off before any of them get a chance
	// to increment this and prevent more from getting forked off
	incrementBusyListeners();
	int32_t	forkedlisteners=incrementForkedListeners();

	// if we already have too many listeners running,
	// bail and return an error to the client
	if (pvt->_maxlisteners>-1 && forkedlisteners>pvt->_maxlisteners) {

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
	if (pvt->_usethreads) {

		// set up the thread
		thread			*thr=new thread;
		clientsessionattr	*csa=new clientsessionattr;
		csa->thr=thr;
		csa->lsnr=this;
		csa->clientsock=clientsock;
		csa->protocolindex=protocolindex;

		// spawn the thread
		if (thr->spawn((void *(*)(void *))clientSessionThread,
							(void *)csa,true)) {
			pvt->_isforkedthread=true;
			return;
		}

		// error
		decrementBusyListeners();
		decrementForkedListeners();
		errorClientSession(clientsock,
			SQLR_ERROR_ERRORFORKINGLISTENER,
			SQLR_ERROR_ERRORFORKINGLISTENER_STRING);
		raiseInternalErrorEvent(
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
		pvt->_isforkedchild=true;

		// since this is the forked off listener, we don't
		// want to actually remove the semaphore set or shared
		// memory segment when it exits
		pvt->_shmem->dontRemove();
		pvt->_semset->dontRemove();

		// re-init loggers
		if (pvt->_sqlrlg) {
			pvt->_sqlrlg->init(this,NULL);
		}

		clientSession(clientsock,protocolindex,NULL);

		decrementBusyListeners();
		decrementForkedListeners();

		cleanUp();
		process::exit(0);

	} else if (childpid>0) {

		// parent...
		if (pvt->_sqlrlg || pvt->_sqlrn) {
			stringbuffer	debugstr;
			debugstr.append("forked a child: ");
			debugstr.append((int32_t)childpid);
			raiseDebugMessageEvent(debugstr.getString());
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
		raiseInternalErrorEvent(SQLR_ERROR_ERRORFORKINGLISTENER_STRING);
	}
}

void sqlrlistener::clientSessionThread(void *attr) {
	clientsessionattr	*csa=(clientsessionattr *)attr;
	csa->lsnr->clientSession(csa->clientsock,csa->protocolindex,csa->thr);
	csa->lsnr->decrementBusyListeners();
	csa->lsnr->decrementForkedListeners();
	delete csa->thr;
	delete csa;
}

void sqlrlistener::clientSession(filedescriptor *clientsock,
					uint16_t protocolindex,
					thread *thr) {

	if (pvt->_dynamicscaling) {
		incrementConnectedClientCount();
	}

	bool	passstatus=handOffOrProxyClient(clientsock,protocolindex,thr);

	// If the handoff failed, decrement the connected client count.
	// If it had succeeded then the connection daemon would
	// decrement it later.
	if (pvt->_dynamicscaling && !passstatus) {
		decrementConnectedClientCount();
	}

	// FIXME: hmm, if the client is just spewing
	// garbage then we should close the connection...
	waitForClientClose(passstatus,clientsock);

	delete clientsock;
}

bool sqlrlistener::handOffOrProxyClient(filedescriptor *sock,
						uint16_t protocolindex,
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
		connectionsock.write(pvt->_handoffmode);

		// tell the connection which protocol to use
		connectionsock.write(protocolindex);

		if (pvt->_handoffmode==HANDOFF_PASS) {

			// pass the file descriptor
			if (!connectionsock.passSocket(
					sock->getFileDescriptor())) {

				// this could fail if a connection
				// died because its ttl expired...

				raiseInternalErrorEvent("failed to pass "
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

bool sqlrlistener::semWait(int32_t index, thread *thr,
					bool withundo, bool *timeout) {

	// If dynamic scaling is enabled then we need to adjust the
	// listenertimeout.  In all cases where semWait() is called, the scaler
	// could be off trying to start connections, or connections could be
	// slow to start.  So, we must wait long enough to accommodate that,
	// plus a little grace.  If listenertimeout is too short, then it needs
	// to be made longer.
	// FIXME: arguably, this should just be done at startup...

	bool	result=true;
	*timeout=false;
	if (pvt->_listenertimeout>0 &&
			pvt->_semset->supportsTimedSemaphoreOperations()) {
		if (withundo) {
			result=pvt->_semset->waitWithUndo(
					index,pvt->_listenertimeout,0);
		} else {
			result=pvt->_semset->wait(
					index,pvt->_listenertimeout,0);
		}
		*timeout=(!result && error::getErrorNumber()==EAGAIN);
	} else if (pvt->_listenertimeout>0 &&
			!thr && sys::signalsInterruptSystemCalls()) {
		// We can't use this when using threads because alarmrang isn't
		// thread-local and there's no way to make it be.  Also, the
		// alarm doesn't reliably interrupt the wait() when it's called
		// from a thread, at least not on Linux.  Hopefully platforms
		// that supports threads also supports timed semaphore ops.
		pvt->_semset->dontRetryInterruptedOperations();
		alarmrang=0;
		signalmanager::alarm(pvt->_listenertimeout);
		do {
			if (withundo) {
				result=pvt->_semset->waitWithUndo(index);
			} else {
				result=pvt->_semset->wait(index);
			}
		} while (!result && error::getErrorNumber()==EINTR &&
							alarmrang!=1);
		*timeout=(alarmrang==1);
		signalmanager::alarm(0);
		pvt->_semset->retryInterruptedOperations();
	} else {
		if (withundo) {
			result=pvt->_semset->waitWithUndo(index);
		} else {
			result=pvt->_semset->wait(index);
		}
	}

	return result;
}

bool sqlrlistener::acquireShmAccess(thread *thr, bool *timeout) {

	raiseDebugMessageEvent("acquiring exclusive shm access");

	if (!semWait(1,thr,true,timeout)) {
		if (*timeout) {
			raiseDebugMessageEvent("timeout occured");
		} else {
			raiseDebugMessageEvent("failed to acquire "
						"exclusive shm access");
		}
		return false;
	}

	// success...
	raiseDebugMessageEvent("acquired exclusive shm access");
	return true;
}

bool sqlrlistener::releaseShmAccess() {

	raiseDebugMessageEvent("releasing exclusive shm access");

	if (!pvt->_semset->signalWithUndo(1)) {
		raiseDebugMessageEvent("failed to release exclusive shm access");
		return false;
	}

	raiseDebugMessageEvent("finished releasing exclusive shm access");
	return true;
}

bool sqlrlistener::acceptAvailableConnection(thread *thr,
						bool *alldbsdown,
						bool *timeout) {

	// If we don't want to wait for down databases, then check to see if
	// any of the db's are up.  If none are, then don't even wait for an
	// available connection, just bail immediately.
	if (!pvt->_cfg->getWaitForDownDatabase()) {
		*alldbsdown=true;
		linkedlist< connectstringcontainer * >	*csl=
					pvt->_cfg->getConnectStringList();
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

	raiseDebugMessageEvent("waiting for an available connection");

	if (!semWait(2,thr,false,timeout)) {
		if (*timeout) {
			raiseDebugMessageEvent("timeout occured");
		} else {
			raiseInternalErrorEvent("general failure waiting "
						"for available connection");
		}
		return false;
	}

	// success...

	// Reset this semaphore to 0.
	// It can get left incremented if a sqlr-connection process is killed
	// between calls to signalListenerToRead() and
	// waitForListenerToFinishReading().  It's safe to reset it here
	// because of the lock on semaphore 1.
	pvt->_semset->setValue(2,0);

	raiseDebugMessageEvent("succeeded in waiting for "
				"an available connection");
	return true;
}

bool sqlrlistener::doneAcceptingAvailableConnection() {

	raiseDebugMessageEvent("signalling accepted connection");

	if (!pvt->_semset->signal(3)) {
		raiseDebugMessageEvent("failed to signal accepted connection");
		return false;
	}

	raiseDebugMessageEvent("succeeded signalling accepted connection");
	return true;
}

void sqlrlistener::waitForConnectionToBeReadyForHandoff() {
	raiseDebugMessageEvent("waiting for connection to be ready for handoff");
	pvt->_semset->wait(12);
	raiseDebugMessageEvent("done waiting for connection to be ready for handoff");
}

bool sqlrlistener::getAConnection(uint32_t *connectionpid,
					uint16_t *inetport,
					char *unixportstr,
					uint16_t *unixportstrlen,
					filedescriptor *sock,
					thread *thr) {

	for (;;) {

		raiseDebugMessageEvent("getting a connection...");

		// set "all db's down" flag
		bool	alldbsdown=false;

		// acquire access to the shared memory	
		bool	timeout=false;
		bool	ok=acquireShmAccess(thr,&timeout);

		if (ok) {

			// This section should be executed without returns or
			// breaks so that releaseShmAccess will get called
			// at the end, no matter what...

			// wait for an available connection
			ok=acceptAvailableConnection(thr,&alldbsdown,&timeout);

			if (ok) {

				// get the pid
				*connectionpid=
					pvt->_shm->connectioninfo.connectionpid;

				// signal the connection that we waited for
				ok=doneAcceptingAvailableConnection();
			}

			// release access to the shared memory
			ok=(releaseShmAccess() && ok);
		}

		// execute this only if code above executed without errors...
		if (ok) {

			// wait for the connection to let us know that it's
			// ready to have a client handed off to it
			waitForConnectionToBeReadyForHandoff();

			// make sure the connection is actually up...
			if (connectionIsUp(pvt->_shm->connectionid)) {
				if (pvt->_sqlrlg || pvt->_sqlrn) {
					stringbuffer	debugstr;
					debugstr.append("finished getting "
							"a connection: ");
					debugstr.append(
						(int32_t)*connectionpid);
					raiseDebugMessageEvent(
						debugstr.getString());
				}
				return true;
			}

			// if the connection wasn't up, fork a child to jog it,
			// spin back and get another connection
			raiseDebugMessageEvent("connection was down");
			pingDatabase(*connectionpid,unixportstr,*inetport);
		}

		if (timeout) {
			raiseDebugMessageEvent("failed to get "
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
			raiseDebugMessageEvent("failed to get "
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

bool sqlrlistener::connectionIsUp(const char *connectionid) {

	// initialize the database up/down filename
	char	*updown=NULL;
	charstring::printf(&updown,"%s%s-%s.up",
				pvt->_sqlrpth->getIpcDir(),
				pvt->_cmdl->getId(),connectionid);
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
	if (pvt->_usethreads) {

		// set up the thread
		thread			*thr=new thread;
		pingdatabaseattr	*pda=new pingdatabaseattr;
		pda->thr=thr;
		pda->lsnr=this;
		pda->connectionpid=connectionpid;
		pda->unixportstr=unixportstr;
		pda->inetport=inetport;

		// spawn the thread
		thr->spawn((void *(*)(void *))pingDatabaseThread,
						(void *)pda,true);
		return;
	}

	// if threads are not supported, fork a child
	// process to ping the database
	pid_t	childpid=process::fork();
	if (!childpid) {
		pvt->_isforkedchild=true;
		pingDatabaseInternal(connectionpid,unixportstr,inetport);
		cleanUp();
		process::exit(0);
	}
}

void sqlrlistener::pingDatabaseThread(void *attr) {
	pingdatabaseattr	*pda=(pingdatabaseattr *)attr;
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
	for (uint32_t i=0; i<pvt->_maxconnections; i++) {
		if (pvt->_handoffsocklist[i].pid==connectionpid) {
			connectionsock->setFileDescriptor(
						pvt->_handoffsocklist[i].
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

	raiseDebugMessageEvent("requesting socket of newly "
					"spawned connection...");

	// connect to the fixup socket of the parent listener
	unixsocketclient	fixupclientsockun;
	if (fixupclientsockun.connect(pvt->_fixupsockname,-1,-1,0,1)
							!=RESULT_SUCCESS) {
		raiseInternalErrorEvent("fixup failed to connect");
		return false;
	}

	// send the pid of the connection that we need
	if (fixupclientsockun.write(connectionpid)!=sizeof(uint32_t)) {
		raiseInternalErrorEvent("fixup failed to write pid");
		return false;
	}
	fixupclientsockun.flushWriteBuffer(-1,-1);

	// get the file descriptor of the socket
	int32_t	fd;
	if (!fixupclientsockun.receiveSocket(&fd)) {
		raiseInternalErrorEvent("fixup failed to receive socket");
		return false;
	}
	connectionsock->setFileDescriptor(fd);

	// On most systems, the file descriptor is in whatever mode
	// it was in the other process, but on FreeBSD < 5.0 and
	// possibly other systems, it ends up in non-blocking mode
	// in this process, independent of its mode in the other
	// process.  So, we force it to blocking mode here.
	connectionsock->useBlockingMode();

	raiseDebugMessageEvent("received socket of newly spawned connection");
	return true;
}

bool sqlrlistener::proxyClient(pid_t connectionpid,
				filedescriptor *serversock,
				filedescriptor *clientsock) {

	raiseDebugMessageEvent("proxying client...");

	// send the connection our PID
	serversock->write((uint32_t)process::getProcessId());
	serversock->flushWriteBuffer(-1,-1);

	// wait up to 5 seconds for a response
	unsigned char	ack=0;
	if (serversock->read(&ack,5,0)!=sizeof(unsigned char)) {
		raiseDebugMessageEvent("proxying client failed: "
					"failed to receive ack");
		return false;
	}
	#define ACK	6
	if (ack!=ACK) {
		raiseDebugMessageEvent("proxying client failed: "
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
			raiseDebugMessageEvent("wait exited with no data");
			endsession=true;
			break;
		}

		// get the file descriptor that data was available from
		filedescriptor	*fd=
			proxy.getReadReadyList()->getFirst()->getValue();

		// read whatever data was available
		ssize_t	readcount=fd->read(readbuffer,sizeof(readbuffer));
		if (readcount<1) {
			if (pvt->_sqlrlg || pvt->_sqlrn) {
				stringbuffer	debugstr;
				debugstr.append("read failed: ");
				debugstr.append((uint32_t)readcount);
				debugstr.append(" : ");
				char	*err=error::getErrorString();
				debugstr.append(err);
				delete[] err;
				raiseDebugMessageEvent(debugstr.getString());
			}
			endsession=(fd==clientsock);
			break;
		}

		// write the data to the other side
		if (fd==serversock) {
			if (pvt->_sqlrlg || pvt->_sqlrn) {
				stringbuffer	debugstr;
				debugstr.append("read ");
				debugstr.append((uint32_t)readcount);
				debugstr.append(" bytes from server");
				raiseDebugMessageEvent(debugstr.getString());
			}
			clientsock->write(readbuffer,readcount);
			clientsock->flushWriteBuffer(-1,-1);
		} else if (fd==clientsock) {
			if (pvt->_sqlrlg || pvt->_sqlrn) {
				stringbuffer	debugstr;
				debugstr.append("read ");
				debugstr.append((uint32_t)readcount);
				debugstr.append(" bytes from client");
				raiseDebugMessageEvent(debugstr.getString());
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
		raiseDebugMessageEvent("ending the session");
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

	raiseDebugMessageEvent("finished proxying client");

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
		while (clientsock->read(&dummy,pvt->_idleclienttimeout,0)>0 &&
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
					sizeof(uint32_t)+pvt->_maxquerysize+
					// input bind var count
					sizeof(uint16_t)+
					// input bind vars
					pvt->_maxbindcount*
						(2*sizeof(uint16_t)+
						pvt->_maxbindnamelength)+
					// output bind var count
					sizeof(uint16_t)+
					// output bind vars
					pvt->_maxbindcount*
						(2*sizeof(uint16_t)+
						pvt->_maxbindnamelength)+
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
	pvt->_shm->starttime=dt.getEpoch();
}

void sqlrlistener::setMaxListeners(uint32_t maxlisteners) {
	pvt->_shm->max_listeners=maxlisteners;
}

void sqlrlistener::incrementMaxListenersErrors() {
	pvt->_shm->max_listeners_errors++;
}

void sqlrlistener::incrementConnectedClientCount() {

	raiseDebugMessageEvent("incrementing connected client count...");

	if (!pvt->_semset->waitWithUndo(5)) {
		// FIXME: bail somehow
	}

	// increment the connections-in-use counter
	pvt->_shm->connectedclients++;

	// update the peak connections-in-use count
	if (pvt->_shm->connectedclients>pvt->_shm->peak_connectedclients) {
		pvt->_shm->peak_connectedclients=pvt->_shm->connectedclients;
	}

	// update the peak connections-in-use over the previous minute count
	datetime	dt;
	dt.getSystemDateAndTime();
	if (pvt->_shm->connectedclients>
			pvt->_shm->peak_connectedclients_1min ||
		dt.getEpoch()/60>
			pvt->_shm->peak_connectedclients_1min_time/60) {
		pvt->_shm->peak_connectedclients_1min=
				pvt->_shm->connectedclients;
		pvt->_shm->peak_connectedclients_1min_time=
				dt.getEpoch();
	}

	if (!pvt->_semset->signalWithUndo(5)) {
		// FIXME: bail somehow
	}

	// The scaler loops, looking to see if it has to do anything, and then
	// sleeping for a short bit if it doesn't, before looping again.
	// If the system supports timed semaphore ops then the scaler can be
	// jogged into immediate action though, so we'll do that here.
	if (pvt->_semset->supportsTimedSemaphoreOperations()) {

		// signal the scaler to evaluate the connection count
		// and start more connections if necessary
		raiseDebugMessageEvent("signalling the scaler...");
		if (!pvt->_semset->signal(6)) {
			// FIXME: bail somehow
		}
		raiseDebugMessageEvent("finished signalling the scaler...");
	}

	raiseDebugMessageEvent("finished incrementing connected client count");
}

void sqlrlistener::decrementConnectedClientCount() {

	raiseDebugMessageEvent("decrementing connected client count...");
 
	if (!pvt->_semset->waitWithUndo(5)) {
		// FIXME: bail somehow
	}

	if (pvt->_shm->connectedclients) {
		pvt->_shm->connectedclients--;
	}

	if (!pvt->_semset->signalWithUndo(5)) {
		// FIXME: bail somehow
	}

	raiseDebugMessageEvent("finished decrementing connected client count");
}

uint32_t sqlrlistener::incrementForkedListeners() {

	pvt->_semset->waitWithUndo(9);
	uint32_t	forkedlisteners=++(pvt->_shm->forked_listeners);
	pvt->_semset->signalWithUndo(9);
	return forkedlisteners;
}

uint32_t sqlrlistener::decrementForkedListeners() {

	pvt->_semset->waitWithUndo(9);
	if (pvt->_shm->forked_listeners) {
		pvt->_shm->forked_listeners--;
	}
	uint32_t	forkedlisteners=pvt->_shm->forked_listeners;
	pvt->_semset->signalWithUndo(9);
	return forkedlisteners;
}

void sqlrlistener::incrementBusyListeners() {

	raiseDebugMessageEvent("incrementing busy listeners");

	if (!pvt->_semset->signal(10)) {
		// FIXME: bail somehow
	}

	// update the peak listeners count
	uint32_t	busylisteners=pvt->_semset->getValue(10);
	if (pvt->_shm->peak_listeners<busylisteners) {
		pvt->_shm->peak_listeners=busylisteners;
	}

	// update the peak listeners over the previous minute count
	datetime	dt;
	dt.getSystemDateAndTime();
	if (busylisteners>pvt->_shm->peak_listeners_1min ||
		dt.getEpoch()/60>pvt->_shm->peak_listeners_1min_time/60) {
		pvt->_shm->peak_listeners_1min=busylisteners;
		pvt->_shm->peak_listeners_1min_time=dt.getEpoch();
	}

	raiseDebugMessageEvent("finished incrementing busy listeners");
}

void sqlrlistener::decrementBusyListeners() {
	raiseDebugMessageEvent("decrementing busy listeners");
	if (!pvt->_semset->wait(10)) {
		// FIXME: bail somehow
	}
	raiseDebugMessageEvent("finished decrementing busy listeners");
}

int32_t sqlrlistener::getBusyListeners() {
	return pvt->_semset->getValue(10);
}

void sqlrlistener::raiseDebugMessageEvent(const char *info) {
	if (pvt->_sqlrlg) {
		pvt->_sqlrlg->run(this,NULL,NULL,
				SQLRLOGGER_LOGLEVEL_DEBUG,
				SQLREVENT_DEBUG_MESSAGE,
				info);
	}
	if (pvt->_sqlrn) {
		pvt->_sqlrn->run(this,NULL,NULL,
				SQLREVENT_DEBUG_MESSAGE,
				info);
	}
}

void sqlrlistener::raiseClientProtocolErrorEvent(
					const char *info, ssize_t result) {
	if (!pvt->_sqlrlg && !pvt->_sqlrn) {
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
	if (pvt->_sqlrlg) {
		pvt->_sqlrlg->run(this,NULL,NULL,
				SQLRLOGGER_LOGLEVEL_ERROR,
				SQLREVENT_CLIENT_PROTOCOL_ERROR,
				errorbuffer.getString());
	}
	if (pvt->_sqlrn) {
		pvt->_sqlrn->run(this,NULL,NULL,
				SQLREVENT_CLIENT_PROTOCOL_ERROR,
				errorbuffer.getString());
	}
}

void sqlrlistener::raiseClientConnectionRefusedEvent(const char *info) {
	if (pvt->_sqlrlg) {
		pvt->_sqlrlg->run(this,NULL,NULL,
				SQLRLOGGER_LOGLEVEL_WARNING,
				SQLREVENT_CLIENT_CONNECTION_REFUSED,
				info);
	}
	if (pvt->_sqlrn) {
		pvt->_sqlrn->run(this,NULL,NULL,
				SQLREVENT_CLIENT_CONNECTION_REFUSED,
				info);
	}
}

void sqlrlistener::raiseInternalErrorEvent(const char *info) {
	if (!pvt->_sqlrlg && !pvt->_sqlrn) {
		return;
	}
	stringbuffer	errorbuffer;
	errorbuffer.append(info);
	if (error::getErrorNumber()) {
		char	*error=error::getErrorString();
		errorbuffer.append(": ")->append(error);
		delete[] error;
	}
	if (pvt->_sqlrlg) {
		pvt->_sqlrlg->run(this,NULL,NULL,
				SQLRLOGGER_LOGLEVEL_ERROR,
				SQLREVENT_INTERNAL_ERROR,
				errorbuffer.getString());
	}
	if (pvt->_sqlrn) {
		pvt->_sqlrn->run(this,NULL,NULL,
				SQLREVENT_INTERNAL_ERROR,
				errorbuffer.getString());
	}
}

void sqlrlistener::alarmHandler(int32_t signum) {
	alarmrang=1;
	#ifdef SIGALRM
	alarmhandler.handleSignal(SIGALRM);
	#endif
}

const char *sqlrlistener::getId() {
	return pvt->_cmdl->getId();
}

const char *sqlrlistener::getLogDir() {
	return pvt->_sqlrpth->getLogDir();
}

const char *sqlrlistener::getDebugDir() {
	return pvt->_sqlrpth->getDebugDir();
}
