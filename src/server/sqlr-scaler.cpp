// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <sqlrelay/sqlrutil.h>
#include <rudiments/semaphoreset.h>
#include <rudiments/sharedmemory.h>
#include <rudiments/commandline.h>
#include <rudiments/snooze.h>
#include <rudiments/permissions.h>
#include <rudiments/file.h>
#include <rudiments/userentry.h>
#include <rudiments/groupentry.h>
#include <rudiments/process.h>
#include <rudiments/datetime.h>
#include <rudiments/error.h>
#include <rudiments/randomnumber.h>
#include <rudiments/charstring.h>
#include <rudiments/sys.h>
#include <rudiments/stdio.h>

#include <config.h>
#include <defaults.h>
#include <version.h>

// for pid_t
#include <sys/types.h>

class SQLRSERVER_DLLSPEC scaler {

	public:
			scaler();
			~scaler();
		bool	initScaler(int argc, const char **argv);
		void	loop();

		static	void	shutDown(int32_t signum);

	private:
		void	cleanUp();

		pid_t	openOneConnection();
		bool	connectionStarted();
		void	killConnection(pid_t connpid);
		bool	openMoreConnections();
		bool	reapChildren(pid_t connpid);
		void	getRandomConnectionId();
		bool	availableDatabase();

		uint32_t	getConnectedClientCount();
		uint32_t	getConnectionCount();
		void		incrementConnectionCount();
		void		decrementConnectionCount();

		char		*pidfile;

		const char	*id;
		const char	*configurl;
		const char	*config;
		const char	*dbase;

		sqlrconfigs	*sqlrcfgs;
		sqlrconfig	*cfg;

		uint32_t	maxconnections;
		uint32_t	maxqueuelength;
		uint32_t	growby;
		int32_t		ttl;

		semaphoreset	*semset;

		sharedmemory	*shmem;
		sqlrshm		*shm;

		linkedlist< connectstringcontainer * > *connectstringlist;
		const char	*connectionid;
		int32_t		metrictotal;

		uint32_t	currentseed;

		bool		init;

		sqlrpaths	*sqlrpth;
		sqlrcmdline	*cmdl;

		bool		iswindows;

		const char	*backtrace;
		bool		disablecrashhandler;

		static	bool	shutdown;
};

bool	scaler::shutdown=false;

scaler::scaler() {

	init=false;

	cmdl=NULL;

	pidfile=NULL;
	semset=NULL;
	shmem=NULL;
	shm=0;

	sqlrcfgs=NULL;
	cfg=NULL;
	sqlrpth=NULL;

	id=NULL;
	configurl=NULL;
	config=NULL;
	dbase=NULL;

	iswindows=!charstring::compareIgnoringCase(
				sys::getOperatingSystemName(),"Windows");
}

scaler::~scaler() {
	if (init) {
		reapChildren(-1);
		cleanUp();
	}
}

bool scaler::initScaler(int argc, const char **argv) {

	// read the commandline
	cmdl=new sqlrcmdline(argc,argv);

	process::handleShutDown(shutDown);
	disablecrashhandler=cmdl->found("-disable-crash-handler");
	if (!disablecrashhandler) {
		process::handleCrash(shutDown);
	}

	backtrace=cmdl->getValue("-backtrace");

	init=true;

	// get the id
	id=cmdl->getId();
	if (!charstring::compare(id,DEFAULT_ID)) {
		stderror.printf("Warning: using default id.\n");
	}

	sqlrpth=new sqlrpaths(cmdl);

	// check for listener's pid file
	// (Look a few times.  It might not be there right away.  The listener
	// writes it out after forking and it's possible that the scaler might
	// start up after the sqlr-listener has forked, but before it writes
	// out the pid file)
	char	*listenerpidfile=NULL;
	charstring::printf(&listenerpidfile,"%ssqlr-listener-%s.pid",
						sqlrpth->getPidDir(),id);

	// On most platforms, 3 seconds is plenty of time to wait for the
	// listener to come up, but on 64-bit windows, when running 32-bit
	// apps, listening on an inet socket can take many seconds.
	uint8_t	listenertimeout=30;
	if (iswindows &&
		(!charstring::compareIgnoringCase(
			sys::getOperatingSystemArchitecture(),"x86_64") ||
		!charstring::compareIgnoringCase(
			sys::getOperatingSystemArchitecture(),"amd64")) &&
		sizeof(void *)==4) {
		listenertimeout=100;
	}

	bool	found=false;
	for (uint8_t i=0; !found && i<listenertimeout; i++) {
		if (i) {
			snooze::microsnooze(0,100000);
		}
		found=(process::checkForPidFile(listenerpidfile)!=-1);
	}
	if (!found) {
		stderror.printf("\n%s-scaler error: \n"
				"	The file %s"
				" was not found.\n"
				"	This usually means that the "
				"%s-listener is not running.\n"
				"	The %s-listener must be running "
				"for the %s-scaler to start.\n\n",
				SQLR,listenerpidfile,SQLR,SQLR,SQLR);
		delete[] listenerpidfile;
		return false;
	}
	delete[] listenerpidfile;

	// check/set pid file
	charstring::printf(&pidfile,"%ssqlr-scaler-%s.pid",
					sqlrpth->getPidDir(),id);
	if (process::checkForPidFile(pidfile)!=-1) {
		stderror.printf("\n%s-scaler error:\n"
				"	The pid file %s"
				" exists.\n"
				"	This usually means that the "
				"%s-scaler is already running for the \n"
				"	%s instance.\n"
				"	If it is not running, please remove "
				"the file and restart.\n",
				SQLR,pidfile,SQLR,id);
		delete[] pidfile;
		pidfile=NULL;
		return false;
	}

	// get the config url
	configurl=sqlrpth->getConfigUrl();
	config=cmdl->getValue("-config");

	// load the configuration
	sqlrcfgs=new sqlrconfigs(sqlrpth);
	cfg=sqlrcfgs->load(configurl,id);
	if (cfg) {

		// don't even start if we're not using dynamic scaling
		if (!cfg->getDynamicScaling()) {
			return false;
		}

		// run as user/group specified in the configuration
		const char	*runasuser=cfg->getRunAsUser();
		const char	*runasgroup=cfg->getRunAsGroup();
		if (!charstring::isNullOrEmpty(runasuser) &&
			!charstring::isNullOrEmpty(runasgroup)) {

			// get the user that we're currently running as
			char	*currentuser=
			userentry::getName(process::getEffectiveUserId());

			// get the group that we're currently running as
			char	*currentgroup=
			groupentry::getName(process::getEffectiveGroupId());

			stringbuffer	errorstr;

			// switch groups, but only if we're not currently
			// running as the group that we should switch to
			if (charstring::compare(currentgroup,
						cfg->getRunAsGroup()) &&
					!process::setGroup(
						cfg->getRunAsGroup())) {
				errorstr.append(
					"Warning: could not change group to ")->
					append(cfg->getRunAsGroup())->
					append('\n');
			}

			// switch users, but only if we're not currently
			// running as the user that we should switch to
			if (charstring::compare(currentuser,
						cfg->getRunAsUser()) &&
					!process::setUser(
						cfg->getRunAsUser())) {
				errorstr.append(
					"Warning: could not change user to ")->
					append(cfg->getRunAsUser())->
					append('\n');
			}

			// write the error, if there was one
			stderror.write(errorstr.getString(),
					errorstr.getStringLength());

			// clean up
			delete[] currentuser;
			delete[] currentgroup;
		}

		// make sure user/group can access the configuration
		// (This shouldn't be necessary because if the user/group
		// can't access the file, the sqlr-listener won't start and if
		// it won't start, the scaler won't start.  However someone
		// could get crafty and force the sqlr-scaler to start so
		// we'll do this check just to make sure)
		if (!cfg->accessible()) {
			stderror.printf("\n%s-scaler error:\n"
					"	This instance of "
					"%s is "
					"configured to run as:\n"
					"		user: %s\n"
					"		group: %s\n\n"
					"	However, the config url %s\n"
					"	cannot be accessed by that "
					"user or group.\n\n"
					"	Since you're using "
					"dynamic scaling "
					"(ie. maxconnections>connections),\n"
					"	new connections would be "
					"started as\n"
					"		user: %s\n"
					"		group: %s\n\n"
					"	They would not be able to "
					"access the"
					"config url and would shut down.\n\n"
					"	To remedy this problem, "
					"make %s\n"
					"	accessible by\n"
					"		user: %s\n"
					"		group: %s\n",
					SQLR,
					SQL_RELAY,
					cfg->getRunAsUser(),
					cfg->getRunAsGroup(),
					configurl,
					cfg->getRunAsUser(),
					cfg->getRunAsGroup(),
					configurl,
					cfg->getRunAsUser(),
					cfg->getRunAsGroup());
			return false;
		}

		// get the dynamic connection scaling parameters
		maxconnections=cfg->getMaxConnections();
		maxqueuelength=cfg->getMaxQueueLength();
		growby=cfg->getGrowBy();
		ttl=cfg->getTtl();

		// get the database type
		dbase=cfg->getDbase();

		// get the list of connect strings
		connectstringlist=cfg->getConnectStringList();

		// add up the connection metrics
		metrictotal=cfg->getMetricTotal();
	}

	// initialize the shared memory segment filename
	char	*idfilename=NULL;
	charstring::printf(&idfilename,"%s%s.ipc",sqlrpth->getIpcDir(),id);
	key_t	key=file::generateKey(idfilename,1);
	delete[] idfilename;

	// connect to the shared memory segment
	shmem=new sharedmemory;
	if (!shmem->attach(key,sizeof(sqlrshm))) {
		char	*err=error::getErrorString();
		stderror.printf("Couldn't attach to shared memory segment: "
				"%s\n",err);
		delete[] err;
		delete shmem;
		shmem=NULL;
		return false;
	}
	shm=(sqlrshm *)shmem->getPointer();
	if (!shm) {
		stderror.printf("failed to get pointer to shm\n");
		delete shmem;
		shmem=NULL;
		return false;
	}

	// connect to the semaphore set
	semset=new semaphoreset;
	if (!semset->attach(key,13)) {
		char	*err=error::getErrorString();
		stderror.printf("Couldn't attach to semaphore set: "
				"%s\n",err);
		delete[] err;
		delete semset;
		delete shmem;
		semset=NULL;
		shmem=NULL;
		return false;
	}

	// set up random number generator
	datetime	dt;
	dt.getSystemDateAndTime();
	currentseed=dt.getEpoch();

	if (!cmdl->found("-nodetach")) {
		// detach from the controlling tty
		process::detach();
	}

	// create the pid file
	process::createPidFile(pidfile,permissions::ownerReadWrite());

	return true;
}

void scaler::shutDown(int32_t signum) {
	shutdown=true;
}

void scaler::cleanUp() {

	delete semset;
	delete shmem;
	delete sqlrcfgs;

	delete sqlrpth;

	if (pidfile) {
		file::remove(pidfile);
		delete[] pidfile;
	}

	delete cmdl;
}

bool scaler::reapChildren(pid_t connpid) {

	bool	reaped=false;

	for (;;) {

		// We need to decrement the connection count after waiting for
		// the child to exit.  On unix-like platforms, we can handle
		// that with SIGCHLD/waitpid().  On other platforms we can
		// do it with a semaphore.

		if (process::supportsGetChildStateChange()) {

			childstatechange	childstate;
			int32_t			exitstatus=0;
			int32_t			signum=0;
			bool			coredump=false;
			int	pid=process::getChildStateChange(connpid,
						false,true,true,
						&childstate,
						&exitstatus,&signum,&coredump);
			if (pid<1) {
				break;
			}

			if (childstate==EXIT_CHILDSTATECHANGE) {
				if (exitstatus) {
					stderror.printf(
						"Connection (pid=%d) "
						"exited with code %d\n",
						pid,exitstatus);
				}
			} else if (childstate==TERMINATED_CHILDSTATECHANGE) {
				if (coredump) {
					stderror.printf(
						"Connection (pid=%d) "
						"terminated by signal %d, "
						"with coredump\n",
						pid,signum);
				} else {
					stderror.printf(
						"Connection (pid=%d) "
						"terminated by signal %d\n",
						pid,signum);
				}
			} else if (childstate==STOPPED_CHILDSTATECHANGE) {
				// this shouldn't happen
				stderror.printf("Connection (pid=%d) "
							"stopped",pid);
			} else if (childstate==CONTINUED_CHILDSTATECHANGE) {
				// this shouldn't happen
				stderror.printf("Connection (pid=%d) "
							"continued",pid);
			}

		} else {
			if (!semset->wait(11,0,0)) {
				break;
			}
		}

		decrementConnectionCount();
		reaped=true;
	}

	return reaped;
}

pid_t scaler::openOneConnection() {

	// build command name
	stringbuffer	cmdname;
	cmdname.append(SQLR)->append("-connection");

	// build command to spawn
	stringbuffer	cmd;
	cmd.append(sqlrpth->getBinDir())->append(cmdname.getString());
	if (iswindows) {
		cmd.append(".exe");
	}

	// build ttl string
	char	ttlstr[20];
	charstring::printf(ttlstr,sizeof(ttlstr),"%d",ttl);
	ttlstr[19]='\0';

	// build args
	uint16_t	p=0;
	const char	*args[18];
	args[p++]=cmdname.getString();
	args[p++]="-silent";
	args[p++]="-nodetach";
	args[p++]="-ttl";
	args[p++]=ttlstr;
	args[p++]="-id";
	args[p++]=id;
	args[p++]="-connectionid";
	args[p++]=connectionid;
	if (!charstring::isNullOrEmpty(config)) {
		args[p++]="-config";
		args[p++]=config;
	}
	args[p++]="-localstatedir";
	args[p++]=sqlrpth->getLocalStateDir();
	if (!charstring::isNullOrEmpty(backtrace)) {
		args[p++]="-backtrace";
		args[p++]=backtrace;
	}
	args[p++]="-scaler";
	if (disablecrashhandler) {
		args[p++]="-disable-crash-handler";
	}
	args[p++]=NULL; // the last

	pid_t	pid=process::spawn(cmd.getString(),args,(iswindows)?true:false);
	if (pid==-1) {
		// error
		stderror.printf("spawn() failed: %s\n",error::getErrorString());
	}
	return (pid>0)?pid:0;
}

bool scaler::openMoreConnections() {

	// Wait 1/10th of a second.  If the os supports timed semaphore
	// operations then wait on a semaphore too so the listener can
	// cause this to run on-demand.
	bool	waitresult=false;
	if (semset->supportsTimedSemaphoreOperations()) {
		waitresult=semset->wait(6,0,100000000);

		// If the wait returned false for some other reason than a
		// timeout, then an error has occurred and the semaphore can't
		// be accessed.  Most likely the sqlr-listener has been killed.
		// Shut down.
		if (!waitresult && error::getErrorNumber()!=EAGAIN) {
			shutdown=true;
		}
	} else {
		snooze::microsnooze(0,100000);
	}

	// exit if a shutdown request has been made
	if (shutdown) {
		return false;
	}

	// reap children here, no matter what
	reapChildren(-1);
		
	// get connected client and connection counts
	uint32_t	connectedclients=getConnectedClientCount();
	uint32_t	currentconnections=getConnectionCount();

	// do we need to open more connections?
	if (connectedclients<currentconnections ||
		(connectedclients-currentconnections)<=maxqueuelength) {
		return true;
	}

	// can more be opened, or will we exceed the max?
	if ((currentconnections+growby)>maxconnections) {
		return true;
	}

	// open "growby" connections
	for (uint32_t i=0; i<growby; i++) {

		// initialize attempts to start a connection...
		// We'll try to start it some number of times.  If it fails,
		// that many times then usually something bad happened, like
		// the password expired or the DB died, or something, and it's
		// a bad idea to keep trying to start more, so we'll stop that
		// too.
		uint16_t	attempts=0;

		// loop until a connection is successfully started
		pid_t	connpid=0;
		while (!connpid) {

			// exit if a shutdown request has been made
			if (shutdown) {
				return false;
			}

			getRandomConnectionId();

			// if the database associated with the
			// connection id that was randomly chosen is
			// currently unavailable, loop back and get
			// another one
			// if no connections are currently open then
			// we won't know if the database is up or down
			// because no connections have tried to log
			// in to it yet, so in that case, don't even
			// test to see if the database is up or down
			if (currentconnections && !availableDatabase()) {
				snooze::macrosnooze(1);
				continue;
			}

			// The semaphore should be at 0, though the
			// race condition described below could
			// potentially leave it set to 1, so we'll make
			// sure to set it back to 0 here.
			semset->setValue(8,0);

			connpid=openOneConnection();

			if (connpid) {
				incrementConnectionCount();
				if (!connectionStarted()) {
					// There is a race condition
					// here.  connectionStarted()
					// waits for some number seconds.
					// Presumably the connection
					// will start up and signal
					// during that time, or will be
					// killed before it signals,
					// but if it takes just barely
					// longer than that for the
					// connection to start, the wait
					// could time out, then the
					// connection could signal,
					// then it could be killed,
					// leaving the semaphore set to
					// 1 rather than 0.
					killConnection(connpid);
					connpid=0;
					attempts++;
				}
			}

			// Stop trying to start this connection and bail on
			// trying to starting others too.  The listener should
			// stop waiting in a few seconds and send errors to the
			// clients.
			if (attempts==DEFAULT_CONNECTION_START_ATTEMPTS) {
				i=growby;
				break;
			}
		}
	}

	return true;
}

bool scaler::connectionStarted() {

	// wait for the connection count to increase
	// with a timeout, if supported
	// (the timeout should be at least 20 seconds because, if logging is
	// enabled, but someone forgot to put the database host name in DNS,
	// it might take up to 15 seconds for the hostname/ipaddress lookup to
	// time out)
	return semset->supportsTimedSemaphoreOperations()?
			semset->wait(8,DEFAULT_CONNECTION_START_TIMEOUT,0):
			semset->wait(8);
}

void scaler::killConnection(pid_t connpid) {

	// The connection may have crashed or gotten hung up trying to start,
	// possibly because the database was down.  Either way, it won't
	// signal on sem(8) and must be killed.

	datetime	dt;
	dt.getSystemDateAndTime();
	stderror.printf("%s Connection (pid=%ld) failed to get ready\n",
						dt.getString(),(long)connpid);

	// try 3 times - in the first check whether it is already dead,
	// then use SIGTERM and at last use SIGKILL
	bool	dead=false;
	for (int tries=0; tries<3 && !dead; tries++) {
		if (tries) {
			dt.getSystemDateAndTime();
			stderror.printf("%s %s connection (pid=%ld)\n",
				dt.getString(),
				(tries==1)?"Terminating":"Killing",
				(long)connpid);

			#ifdef SIGKILL
			process::sendSignal(connpid,(tries==1)?SIGTERM:SIGKILL);
			#else
			process::sendSignal(connpid,SIGTERM);
			#endif

			// wait for process to terminate
			snooze::macrosnooze(5);
		}
		dead=reapChildren(connpid);
	}
}

void scaler::getRandomConnectionId() {

	// get a scaled random number
	currentseed=randomnumber::generateNumber(currentseed);
	int32_t	scalednum=randomnumber::scaleNumber(currentseed,0,metrictotal);

	// run through list, decrementing scalednum by the metric
	// for each, when scalednum is 0, pick that connection id
	for (connectstringnode *csn=connectstringlist->getFirst();
						csn; csn=csn->getNext()) {
		connectstringcontainer	*currentnode=csn->getValue();
		scalednum=scalednum-currentnode->getMetric();
		if (scalednum<=0) {
			connectionid=currentnode->getConnectionId();
			break;
		}
	}
}

bool scaler::availableDatabase() {
	
	// initialize the database up/down filename
	char	*updown=NULL;
	charstring::printf(&updown,"%s%s-%s.up",
				sqlrpth->getIpcDir(),id,connectionid);
	bool	retval=file::exists(updown);
	delete[] updown;
	return retval;
}

uint32_t scaler::getConnectedClientCount() {
	return shm->connectedclients;
}

uint32_t scaler::getConnectionCount() {

	// wait for access to the connection counter
	semset->waitWithUndo(4);

	// get the number of connections
	uint32_t	connections=shm->totalconnections;

	// signal that the connection counter may be accessed by someone else
	semset->signalWithUndo(4);

	return connections;
}

void scaler::incrementConnectionCount() {

	// wait for access to the connection counter
	semset->waitWithUndo(4);

	// increment connection counter
	shm->totalconnections++;

	// signal that the connection counter may be accessed by someone else
	semset->signalWithUndo(4);
}

void scaler::decrementConnectionCount() {

	// wait for access to the connection counter
	semset->waitWithUndo(4);

	// decrement connection counter
	if (shm->totalconnections) {
		shm->totalconnections--;
	}

	// signal that the connection counter may be accessed by someone else
	semset->signalWithUndo(4);
}

void scaler::loop() {
	while (openMoreConnections()) {}
}

static void helpmessage(const char *progname) {
	stdoutput.printf(
		"%s is the %s database connection scaling daemon.\n"
		"\n"
		"The %s monitors connections from %s client applications and opens new database connections, as-necessary.\n"
		"\n"
		"The %s is not intended to be run manually.  Rather the %s-start process spawns it as-necessary.\n"
		"\n"
		"Usage: %s [OPTIONS]\n"
		"\n"
		"Options:\n"
		SERVEROPTIONS
		"	-nodetach	Suppresses detachment from the controlling terminal.\n"
		"			Useful for debugging.\n"
		"\n"
		DISABLECRASHHANDLER
		BACKTRACECHILDREN,
		progname,SQL_RELAY,progname,SQL_RELAY,progname,SQLR,progname);
}

int main(int argc, const char **argv) {

	version(argc,argv);
	help(argc,argv);

	commandline	cmdl(argc,argv);

	if (!cmdl.found("-id")) {
		stdoutput.printf("usage: \n"
			" %s-scaler [-config config] -id id "
			"[-localstatedir dir] [-nodetach]\n",
			SQLR);
		process::exit(0);
	}

	{
		scaler	s;
		if (s.initScaler(argc,argv)) {
			s.loop();
		}
	}
	process::exit(1);
}
