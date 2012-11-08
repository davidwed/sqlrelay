// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

#include <rudiments/passwdentry.h>
#include <rudiments/groupentry.h>
#include <rudiments/process.h>
#include <rudiments/permissions.h>
#include <rudiments/snooze.h>
#include <rudiments/rawbuffer.h>

bool sqlrconnection_svr::initConnection(int argc, const char **argv) {

	shmdata	*shm;

	// process command line
	cmdl=new cmdline(argc,argv);

	// default id warning
	if (!charstring::compare(cmdl->getId(),DEFAULT_ID)) {
		fprintf(stderr,"Warning: using default id.\n");
	}

	// get whether this connection was spawned by the scaler
	scalerspawned=cmdl->found("-scaler");

	// get the connection id from the command line
	connectionid=cmdl->getValue("-connectionid");
	if (!connectionid[0]) {
		connectionid=DEFAULT_CONNECTIONID;
		fprintf(stderr,"Warning: using default connectionid.\n");
	}

	// get the time to live from the command line
	const char	*ttlstr=cmdl->getValue("-ttl");
	ttl=(ttlstr)?charstring::toInteger(cmdl->getValue("-ttl")):-1;

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

	if (!nodetach) {
		// detach from the controlling tty
		detach();
	}

	if (reloginatstart) {
		while (!attemptLogIn(false)) {
			snooze::macrosnooze(5);
		}
	}

	// Get the query translators.  Do it after logging in, as
	// getSqlTranslator might return a different class depending on what
	// version of the db it gets logged into
	const char	*translations=cfgfl->getTranslations();
	if (charstring::length(translations)) {
		sqlp=new sqlparser;
		sqlt=getSqlTranslations();
		sqlt->loadTranslations(translations);
		sqlw=getSqlWriter();
	}
	debugsqltranslation=cfgfl->getDebugTranslations();

	// Get the triggers
	const char	*triggers=cfgfl->getTriggers();
	if (charstring::length(triggers)) {
		// for triggers, we'll need an sqlparser as well
		if (!sqlp) {
			sqlp=new sqlparser;
		}
		sqltr=new sqltriggers;
		sqltr->loadTriggers(triggers);
	}
	debugtriggers=cfgfl->getDebugTriggers();

	// update maximum query size, bind value lengths and idle client timeout
	maxquerysize=cfgfl->getMaxQuerySize();
	maxstringbindvaluelength=cfgfl->getMaxStringBindValueLength();
	maxlobbindvaluelength=cfgfl->getMaxLobBindValueLength();
	idleclienttimeout=cfgfl->getIdleClientTimeout();

	// set autocommit behavior
	setAutoCommit(autocommit);

	// get fake input bind variable behavior
	// (this may have already been set true by the connect string)
	fakeinputbinds=(fakeinputbinds || cfgfl->getFakeInputBindVariables());

	// get translate bind variable behavior
	translatebinds=cfgfl->getTranslateBindVariables();

	// initialize cursors
	mincursorcount=cfgfl->getCursors();
	maxcursorcount=cfgfl->getMaxCursors();
	if (!initCursors(mincursorcount)) {
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

	// set the transaction isolation level
	isolationlevel=cfgfl->getIsolationLevel();
	setIsolationLevel(isolationlevel);

	// ignore selectDatabase() calls?
	ignoreselectdb=cfgfl->getIgnoreSelectDatabase();

	// get the database/schema we're using so
	// we can switch back to it at end of session
	originaldb=getCurrentDatabase();

	markDatabaseAvailable();

	// if we're not passing descriptors around, listen on 
	// inet and unix sockets for client connections
	if (!cfgfl->getPassDescriptor() && !openSockets()) {
		return false;
	}

	// initialize the query log, if we're timing queries
	if (cfgfl->getTimeQueriesSeconds()!=-1 &&
		cfgfl->getTimeQueriesMicroSeconds()!=-1 &&
		!initQueryLog()) {
		return false;
	}
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

	// check for listener's pid file
	// (Look a few times.  It might not be there right away.  The listener
	// writes it out after forking and it's possible that the connection
	// might start up after the sqlr-listener has forked, but before it
	// writes out the pid file)
	size_t	listenerpidfilelen=tmpdir->getLength()+20+
				charstring::length(cmdl->getId())+1;
	char	*listenerpidfile=new char[listenerpidfilelen];
	snprintf(listenerpidfile,listenerpidfilelen,
				"%s/pids/sqlr-listener-%s",
				tmpdir->getString(),cmdl->getId());

	bool	retval=true;
	bool	found=false;
	for (uint8_t i=0; !found && i<10; i++) {
		if (i) {
			snooze::microsnooze(0,100000);
		}
		found=(checkForPidFile(listenerpidfile)!=-1);
	}
	if (!found) {
		fprintf(stderr,"\nsqlr-connection error:\n");
		fprintf(stderr,"	The pid file %s",listenerpidfile);
		fprintf(stderr," was not found.\n");
		fprintf(stderr,"	This usually means "
					"that the sqlr-listener \n");
		fprintf(stderr,"is not running.\n");
		fprintf(stderr,"	The sqlr-listener must be running ");
		fprintf(stderr,"for the sqlr-connection to start.\n\n");
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

bool sqlrconnection_svr::initCursors(int32_t count) {

	dbgfile.debugPrint("connection",0,"initializing cursors...");

	cursorcount=count;
	if (!cur) {
		cur=new sqlrcursor_svr *[maxcursorcount];
		rawbuffer::zero(cur,maxcursorcount*sizeof(sqlrcursor *));
	}

	for (int32_t i=0; i<cursorcount; i++) {

		dbgfile.debugPrint("connection",1,i);

		if (!cur[i]) {
			cur[i]=initCursorUpdateStats();
		}
		if (!cur[i]->openCursorInternal(i)) {

			dbgfile.debugPrint("connection",1,
					"cursor init failure...");

			logOutUpdateStats();
			return false;
		}
	}

	// end sid database session
	// FIXME: is there a better place for this?
	if (cfgfl->getSidEnabled()) {
		sid_sqlrcon->endSession();
	}

	dbgfile.debugPrint("connection",0,"done initializing cursors");

	return true;
}
