// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

#include <rudiments/passwdentry.h>
#include <rudiments/groupentry.h>
#include <rudiments/process.h>
#include <rudiments/permissions.h>
#include <rudiments/snooze.h>
#include <rudiments/filesystem.h>

bool sqlrconnection_svr::initConnection(int argc, const char **argv) {

	shmdata	*shm;

	// process command line
	cmdl=new cmdline(argc,argv);

	// get the connection id from the command line
	connectionid=cmdl->getValue("-connectionid");
	if (!connectionid[0]) {
		connectionid=DEFAULT_CONNECTIONID;
		fprintf(stderr,"Warning: using default connectionid.\n");
	}

	// get the time to live from the command line
	ttl=charstring::toInteger(cmdl->getValue("-ttl"));

	silent=cmdl->found("-silent");

	cfgfl=new sqlrconfigfile();
	authc=new authenticator(cfgfl);
	tmpdir=new tempdir(cmdl);

	if (!cfgfl->parse(cmdl->getConfig(),cmdl->getId(),
					getNumberOfConnectStringVars())) {
		return false;
	}

	setUserAndGroup();

	dbgfile.init("connection",cmdl->getLocalStateDir());
	if (cmdl->found("-debug")) {
		dbgfile.enable();
	}

	// handle the unix socket directory
	if (cfgfl->getListenOnUnix()) {
		setUnixSocketDirectory();
	}

	// handle the pid file
	if (!handlePidFile()) {
		return false;
	}

	constr=cfgfl->getConnectString(connectionid);
	if (!constr) {
		fprintf(stderr,"Error: invalid connectionid \"%s\".\n",
							connectionid);
		return false;
	}
	handleConnectString();

	initDatabaseAvailableFileName();

	if (cfgfl->getListenOnUnix() &&
		!getUnixSocket(tmpdir->getString(),unixsocketptr)) {
		return false;
	}

	// get from config file
	bool	reloginatstart=cfgfl->getReLoginAtStart();

	bool	nodetach=cmdl->found("-nodetach");

	/*if (!nodetach && reloginatstart) {
		// detach from the controlling tty
		detach();
	}*/

	if (!createSharedMemoryAndSemaphores(tmpdir->getString(),
							cmdl->getId())) {
		return false;
	}

	shm=(shmdata *)idmemory->getPointer();
	if (!shm) {
		fprintf(stderr,"failed to get pointer to shmdata\n");
		return false;
	}

	statistics=&shm->statistics;
	if (!statistics) {
		fprintf(stderr,"failed to point statistics at idmemory\n");
	}

	if (!reloginatstart) {
		if (!attemptLogIn(!silent)) {
			return false;
		}
	}

	//if (!nodetach && !reloginatstart) {
	if (!nodetach) {
		// detach from the controlling tty
		detach();
	}

	if (reloginatstart) {
		while (!attemptLogIn(false)) {
			snooze::macrosnooze(5);
		}
	}

	if (!initCursors()) {
		return false;
	}

	// create connection pid file
	pid_t	pid=process::getProcessId();
	size_t	pidfilelen=tmpdir->getLength()+22+
				charstring::length(cmdl->getId())+1+
				charstring::integerLength((uint64_t)pid)+1;
	pidfile=new char[pidfilelen];
	snprintf(pidfile,pidfilelen,"%s/pids/sqlr-connection-%s.%d",
				tmpdir->getString(),cmdl->getId(),pid);

	createPidFile(pidfile,permissions::ownerReadWrite());

	setInitialAutoCommitBehavior();

	// create sqlrconnection for sid database
	if (cfgfl->getSidEnabled()) {
		sid_sqlrcon=new sqlrconnection(cfgfl->getSidHost(),
						cfgfl->getSidPort(),
						cfgfl->getSidUnixPort(),
						cfgfl->getSidUser(),
						cfgfl->getSidPassword(),
						0,1);
	}

	// increment connection counter
	if (cfgfl->getDynamicScaling()) {
		incrementConnectionCount();
	}

	markDatabaseAvailable();

	// update maximum query size, bind value lengths and idle client timeout
	maxquerysize=cfgfl->getMaxQuerySize();
	maxstringbindvaluelength=cfgfl->getMaxStringBindValueLength();
	maxlobbindvaluelength=cfgfl->getMaxLobBindValueLength();
	idleclienttimeout=cfgfl->getIdleClientTimeout();

	// if we're not passing descriptors around, listen on 
	// inet and unix sockets for client connections
	if (!cfgfl->getPassDescriptor()) {
		return openSockets();
	}

	// bail here unless we're timing queries
	if (cfgfl->getTimeQueriesSeconds()==-1 ||
		cfgfl->getTimeQueriesMicroSeconds()==-1) {
		return true;
	}

	// initialize query log
	size_t	querylognamelen;
	char	*querylogname;
	if (charstring::length(cmdl->getLocalStateDir())) {
		querylognamelen=charstring::length(cmdl->getLocalStateDir())+33+
				charstring::length(cmdl->getId())+10+20+1;
		querylogname=new char[querylognamelen];
		snprintf(querylogname,querylognamelen,
				"%s/sqlrelay/debug/sqlr-connection-%s"
				"-querylog.%d",
				cmdl->getLocalStateDir(),cmdl->getId(),pid);
	} else {
		querylognamelen=charstring::length(DEBUG_DIR)+17+
				charstring::length(cmdl->getId())+10+20+1;
		querylogname=new char[querylognamelen];
		snprintf(querylogname,querylognamelen,
				"%s/sqlr-connection-%s-querylog.%d",
				DEBUG_DIR,cmdl->getId(),pid);
	}
	file::remove(querylogname);
	if (querylog.create(querylogname,
				permissions::evalPermString("rw-------"))) {
		filesystem	fs;
		fs.initialize(querylogname);
		querylog.setWriteBufferSize(fs.getOptimumTransferBlockSize());
	}
	delete[] querylogname;
	return true;
}

void sqlrconnection_svr::setUserAndGroup() {

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

void sqlrconnection_svr::setUnixSocketDirectory() {
	size_t	unixsocketlen=tmpdir->getLength()+31;
	unixsocket=new char[unixsocketlen];
	snprintf(unixsocket,unixsocketlen,"%s/sockets/",tmpdir->getString());
	unixsocketptr=unixsocket+tmpdir->getLength()+8+1;
}

bool sqlrconnection_svr::handlePidFile() {

	// check for listener pid file
	size_t	listenerpidfilelen=tmpdir->getLength()+20+
				charstring::length(cmdl->getId())+1;
	char	*listenerpidfile=new char[listenerpidfilelen];
	snprintf(listenerpidfile,listenerpidfilelen,
				"%s/pids/sqlr-listener-%s",
				tmpdir->getString(),cmdl->getId());

	bool	retval=true;
	if (checkForPidFile(listenerpidfile)==-1) {
		printf("\nsqlr-connection error:\n");
		printf("	The pid file %s",listenerpidfile);
		printf(" was not found.\n");
		printf("	This usually means that the sqlr-listener \n");
		printf("is not running.\n");
		printf("	The sqlr-listener must be running ");
		printf("for the sqlr-connection to start.\n\n");
		retval=false;
	}

	delete[] listenerpidfile;

	return retval;
}

void sqlrconnection_svr::initDatabaseAvailableFileName() {

	// initialize the database up/down filename
	size_t	updownlen=charstring::length(tmpdir->getString())+5+
					charstring::length(cmdl->getId())+1+
					charstring::length(connectionid)+1;
	updown=new char[updownlen];
	snprintf(updown,updownlen,"%s/ipc/%s-%s",
			tmpdir->getString(),cmdl->getId(),connectionid);
}

bool sqlrconnection_svr::attemptLogIn(bool printerrors) {

	dbgfile.debugPrint("connection",0,"logging in...");
	if (!logInUpdateStats(printerrors)) {
		dbgfile.debugPrint("connection",0,"log in failed");
		if (printerrors) {
			fprintf(stderr,"Couldn't log into database.\n");
		}
		return false;
	}
	dbgfile.debugPrint("connection",0,"done logging in");

	return true;
}

void sqlrconnection_svr::setInitialAutoCommitBehavior() {

	dbgfile.debugPrint("connection",0,"setting autocommit...");
	if (autocommit) {
		if (!autoCommitOn()) {
			dbgfile.debugPrint("connection",0,
					"setting autocommit on failed");
			fprintf(stderr,"Couldn't set autocommit on.\n");
			return;
		}
	} else {
		if (!autoCommitOff()) {
			dbgfile.debugPrint("connection",0,
					"setting autocommit off failed");
			fprintf(stderr,"Couldn't set autocommit off.\n");
			return;
		}
	}
	dbgfile.debugPrint("connection",0,"done setting autocommit");
}

bool sqlrconnection_svr::initCursors() {

	dbgfile.debugPrint("connection",0,"initializing cursors...");

	cursorcount=cfgfl->getCursors();
	if (!cur) {
		cur=(sqlrcursor_svr **)malloc(sizeof(sqlrcursor_svr **)*cursorcount);
		for (int32_t i=0; i<cursorcount; i++) {
			cur[i]=NULL;
		}
	}

	for (int32_t i=0; i<cursorcount; i++) {

		dbgfile.debugPrint("connection",1,i);

		if (!cur[i]) {
			cur[i]=initCursorUpdateStats();
			// FIXME: LAME!!!  oh god this is lame....
			cur[i]->querybuffer=new char[
						cfgfl->getMaxQuerySize()+1];
		}
		if (!cur[i]->openCursorInternal(i)) {

			dbgfile.debugPrint("connection",1,"cursor init failure...");

			logOutUpdateStats();
			//fprintf(stderr,"Couldn't create cursors.\n");
			return false;
		}
//printf("opened cursor %d id=%d\n",i,cur[i]->id);
	}

	// end sid database session
	// FIXME: is there a better place for this?
	if (cfgfl->getSidEnabled()) {
		sid_sqlrcon->endSession();
	}

	dbgfile.debugPrint("connection",0,"done initializing cursors");

	return true;
}
