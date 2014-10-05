// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrscaler.h>

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
#include <rudiments/stdio.h>

#include <defaults.h>

bool	scaler::shutdown=false;

scaler::scaler() {

	init=false;

	cmdl=NULL;

	pidfile=NULL;
	semset=NULL;
	idmemory=NULL;
	shm=0;

	cfgfile=NULL;
	tmpdir=NULL;

	id=NULL;
	config=NULL;
	dbase=NULL;

	debug=false;
}

scaler::~scaler() {
	if (init) {
		reapChildren(-1);
		cleanUp();
	}
}

bool scaler::initScaler(int argc, const char **argv) {

	process::handleShutDown(shutDown);
	process::handleCrash(shutDown);

	init=true;

	// read the commandline
	cmdl=new cmdline(argc,argv);

	// get the id
	const char	*tmpid=cmdl->getValue("-id");
	if (!(tmpid && tmpid[0])) {
		tmpid=DEFAULT_ID;
		stderror.printf("Warning: using default id.\n");
	}
	id=charstring::duplicate(tmpid);

	tmpdir=new tempdir(cmdl);

	// check for listener's pid file
	// (Look a few times.  It might not be there right away.  The listener
	// writes it out after forking and it's possible that the scaler might
	// start up after the sqlr-listener has forked, but before it writes
	// out the pid file)
	size_t	listenerpidfilelen=tmpdir->getLength()+20+
					charstring::length(id)+1;
	char	*listenerpidfile=new char[listenerpidfilelen];
	charstring::printf(listenerpidfile,listenerpidfilelen,
				"%s/pids/sqlr-listener-%s",
				tmpdir->getString(),id);
	bool	found=false;
	for (uint8_t i=0; !found && i<20; i++) {
		if (i) {
			snooze::microsnooze(0,100000);
		}
		found=(process::checkForPidFile(listenerpidfile)!=-1);
	}
	if (!found) {
		stderror.printf("\nsqlr-scaler error: \n");
		stderror.printf("	The file %s",listenerpidfile);
		stderror.printf(" was not found.\n");
		stderror.printf("	This usually means that the ");
		stderror.printf("sqlr-listener is not running.\n");
		stderror.printf("	The sqlr-listener must be running ");
		stderror.printf("for the sqlr-scaler to start.\n\n");
		delete[] listenerpidfile;
		return false;
	}
	delete[] listenerpidfile;

	// check/set pid file
	size_t	pidfilelen=tmpdir->getLength()+18+charstring::length(id)+1;
	pidfile=new char[pidfilelen];
	charstring::printf(pidfile,pidfilelen,
				"%s/pids/sqlr-scaler-%s",
				tmpdir->getString(),id);
	if (process::checkForPidFile(pidfile)!=-1) {
		stderror.printf("\nsqlr-scaler error:\n");
		stderror.printf("	The pid file %s",pidfile);
		stderror.printf(" exists.\n");
		stderror.printf("	This usually means that the ");
		stderror.printf("sqlr-scaler is already running for the \n");
		stderror.printf("	%s instance.\n",id);
		stderror.printf("	If it is not running, please remove ");
		stderror.printf("the file and restart.\n");
		delete[] pidfile;
		pidfile=NULL;
		return false;
	}

	// check for debug
	debug=cmdl->found("-debug");

	// get the config file
	const char	*tmpconfig=cmdl->getValue("-config");
	if (!(tmpconfig && tmpconfig[0])) {
		tmpconfig=DEFAULT_CONFIG_FILE;
	}
	config=new char[charstring::length(tmpconfig)+1];
	charstring::copy(config,tmpconfig);

	// parse the config file
	cfgfile=new sqlrconfigfile;
	if (cfgfile->parse(config,id)) {

		// don't even start if we're not using dynamic scaling
		if (!cfgfile->getDynamicScaling()) {
			return false;
		}

		// run as user/group specified in the config file
		const char	*runasuser=cfgfile->getRunAsUser();
		const char	*runasgroup=cfgfile->getRunAsGroup();
		if (runasuser[0] && runasgroup[0]) {

			// get the user that we're currently running as
			char	*currentuser=
			userentry::getName(process::getEffectiveUserId());

			// get the group that we're currently running as
			char	*currentgroup=
			groupentry::getName(process::getEffectiveGroupId());

			// switch groups, but only if we're not currently
			// running as the group that we should switch to
			if (charstring::compare(currentgroup,
						cfgfile->getRunAsGroup()) &&
					!process::setGroup(
						cfgfile->getRunAsGroup())) {
				stderror.printf("Warning: could not change ");
				stderror.printf("group to %s\n",
						cfgfile->getRunAsGroup());
			}

			// switch users, but only if we're not currently
			// running as the user that we should switch to
			if (charstring::compare(currentuser,
						cfgfile->getRunAsUser()) &&
					!process::setUser(
						cfgfile->getRunAsUser())) {
				stderror.printf("Warning: could not change ");
				stderror.printf("user to %s\n",
						cfgfile->getRunAsUser());
			}

			// clean up
			delete[] currentuser;
			delete[] currentgroup;
		}

		// make sure user/group can read the config file
		// (This shouldn't be necessary because if the user/group
		// can't read the file, the sqlr-listener won't start and if
		// it won't start, the scaler won't start.  However someone
		// could get crafty and force the sqlr-scaler to start so
		// we'll do this check just to make sure)
		file	test;
		if (!test.open(config,O_RDONLY)) {
			stderror.printf("\nsqlr-scaler error:\n");
			stderror.printf("	This instance of ");
			stderror.printf("SQL Relay is ");
			stderror.printf("configured to run as:\n");
			stderror.printf("		user: %s\n",
						cfgfile->getRunAsUser());
			stderror.printf("		group: %s\n\n",
						cfgfile->getRunAsGroup());
			stderror.printf("	However, the config file %s\n",
								config);
			stderror.printf("	cannot be read by that user ");
			stderror.printf("or group.\n\n");
			stderror.printf("	Since you're using ");
			stderror.printf("dynamic scaling ");
			stderror.printf("(ie. maxconnections>connections),\n");
			stderror.printf("	new connections would be ");
			stderror.printf("started as\n");
			stderror.printf("		user: %s\n",
						cfgfile->getRunAsUser());
			stderror.printf("		group: %s\n\n",
						cfgfile->getRunAsGroup());
			stderror.printf("	They would not be able to ");
			stderror.printf("read the");
			stderror.printf("config file and would shut down.\n\n");
			stderror.printf("	To remedy this problem, ");
			stderror.printf("make %s\n",config);
			stderror.printf("	readable by\n");
			stderror.printf("		user: %s\n",
						cfgfile->getRunAsUser());
			stderror.printf("		group: %s\n",
						cfgfile->getRunAsGroup());
			return false;
		}
		test.close();

		// get the dynamic connection scaling parameters
		maxconnections=cfgfile->getMaxConnections();
		maxqueuelength=cfgfile->getMaxQueueLength();
		growby=cfgfile->getGrowBy();
		ttl=cfgfile->getTtl();

		// get the database type
		dbase=charstring::duplicate(cfgfile->getDbase());

		// get the list of connect strings
		connectstringlist=cfgfile->getConnectStringList();

		// add up the connection metrics
		metrictotal=cfgfile->getMetricTotal();
	}

	// initialize the shared memory segment filename
	size_t	idfilenamelen=tmpdir->getLength()+5+charstring::length(id)+1;
	char	*idfilename=new char[idfilenamelen];
	charstring::printf(idfilename,idfilenamelen,
				"%s/ipc/%s",tmpdir->getString(),id);
	key_t	key=file::generateKey(idfilename,1);
	delete[] idfilename;

	// connect to the shared memory segment
	idmemory=new sharedmemory;
	if (!idmemory->attach(key,sizeof(shmdata))) {
		char	*err=error::getErrorString();
		stderror.printf("Couldn't attach to shared memory segment: ");
		stderror.printf("%s\n",err);
		delete[] err;
		delete idmemory;
		idmemory=NULL;
		return false;
	}
	shm=(shmdata *)idmemory->getPointer();
	if (!shm) {
		stderror.printf("failed to get pointer to shmdata\n");
		delete idmemory;
		idmemory=NULL;
		return false;
	}

	// connect to the semaphore set
	semset=new semaphoreset;
	if (!semset->attach(key,11)) {
		char	*err=error::getErrorString();
		stderror.printf("Couldn't attach to semaphore set: ");
		stderror.printf("%s\n",err);
		delete[] err;
		delete semset;
		delete idmemory;
		semset=NULL;
		idmemory=NULL;
		return false;
	}

	// set up random number generator
	datetime	dt;
	dt.getSystemDateAndTime();
	currentseed=dt.getEpoch();

	// detach from the controlling tty
	process::detach();

	// create the pid file
	process::createPidFile(pidfile,permissions::ownerReadWrite());

	return true;
}

void scaler::shutDown(int32_t signum) {
	shutdown=true;
}

void scaler::cleanUp() {

	delete semset;
	delete idmemory;
	delete cfgfile;
	delete[] id;

	delete tmpdir;

	if (pidfile) {
		file::remove(pidfile);
		delete[] pidfile;
	}

	delete[] config;
	delete[] dbase;

	delete cmdl;
}

bool scaler::reapChildren(pid_t connpid) {

	bool	reaped=false;

	for (;;) {
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

		reaped=true;
		decrementConnectionCount();

		if (childstate==EXIT_CHILDSTATECHANGE) {
			if (exitstatus) {
				stderror.printf(
					"Connection (pid=%d) exited "
					"with code %d\n",
					pid,exitstatus);
			}
		} else if (childstate==TERMINATED_CHILDSTATECHANGE) {
			if (coredump) {
				stderror.printf(
					"Connection (pid=%d) terminated "
					"by signal %d, with coredump\n",
					pid,signum);
			} else {
				stderror.printf(
					"Connection (pid=%d) terminated "
					"by signal %d\n",
					pid,signum);
			}
		} else if (childstate==STOPPED_CHILDSTATECHANGE) {
			// this shouldn't happen
			stderror.printf("Connection (pid=%d) stopped",pid);
		} else if (childstate==CONTINUED_CHILDSTATECHANGE) {
			// this shouldn't happen
			stderror.printf("Connection (pid=%d) continued",pid);
		}
	}

	return reaped;
}

pid_t scaler::openOneConnection() {

	char	command[]="sqlr-connection";

	char	ttlstr[20];
	charstring::printf(ttlstr,20,"%d",ttl);
	ttlstr[19]='\0';

	uint16_t	p=0;
	const char	*args[20];
	args[p++]="sqlr-connection";
	args[p++]="-silent";
	args[p++]="-nodetach";
	args[p++]="-ttl";
	args[p++]=ttlstr;
	args[p++]="-id";
	args[p++]=id;
	args[p++]="-connectionid";
	args[p++]=connectionid;
	args[p++]="-config";
	args[p++]=config;
	if (charstring::length(cmdl->getLocalStateDir())) {
		args[p++]="-localstatedir";
		args[p++]=cmdl->getLocalStateDir();
	}
	args[p++]="-scaler";
	if (debug) {
		args[p++]="-debug";
	}
	args[p++]=NULL; // the last

	pid_t	pid=process::spawn(command,args);
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

	// if we were signalled by the listener, signal
	// the listener back so that it can keep going
	if (waitresult) {
		semset->signal(7);
	}

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
					// waits for 20 seconds.
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
				}
			}
		}
	}

	return true;
}

bool scaler::connectionStarted() {

	// wait for the connection count to increase
	// with 20 second timeout, if supported
	// (the 20 second timeout is because, if logging is enabled, but
	// someone forgot to put the database host name in DNS, it might take
	// up to 15 seconds for the hostname/ipaddress lookup to time out)
	return semset->supportsTimedSemaphoreOperations()?
					semset->wait(8,20,0):
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
			signalmanager::sendSignal(connpid,
					(tries==1)?SIGTERM:SIGKILL);
			#else
			signalmanager::sendSignal(connpid,SIGTERM);
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
	size_t	updownlen=tmpdir->getLength()+5+charstring::length(id)+1+
					charstring::length(connectionid)+1;
	char	*updown=new char[updownlen];
	charstring::printf(updown,updownlen,"%s/ipc/%s-%s",
				tmpdir->getString(),id,connectionid);
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
