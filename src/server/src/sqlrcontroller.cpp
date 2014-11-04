// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <config.h>

#include <sqlrelay/sqlrcontroller.h>
#include <sqlrelay/sqlrclientprotocol.h>
#include <rudiments/file.h>
#include <rudiments/socketclient.h>
#include <rudiments/bytestring.h>
#include <rudiments/userentry.h>
#include <rudiments/groupentry.h>
#include <rudiments/process.h>
#include <rudiments/permissions.h>
#include <rudiments/snooze.h>
#include <rudiments/error.h>
#include <rudiments/character.h>
#include <rudiments/charstring.h>
#include <rudiments/randomnumber.h>
#include <rudiments/stdio.h>

#include <defines.h>
#include <defaults.h>
#include <datatypes.h>
#define NEED_CONVERT_DATE_TIME
#include <parsedatetime.h>

#ifndef SQLRELAY_ENABLE_SHARED
	extern "C" {
		#include "sqlrconnectiondeclarations.cpp"
	}
#endif

signalhandler		sqlrcontroller_svr::alarmhandler;
volatile sig_atomic_t	sqlrcontroller_svr::alarmrang=0;

sqlrcontroller_svr::sqlrcontroller_svr() : listener() {

	conn=NULL;

	cmdl=NULL;
	cfgfl=NULL;
	semset=NULL;
	shmem=NULL;
	connstats=NULL;

	updown=NULL;

	clientsock=NULL;

	user=NULL;
	password=NULL;

	dbchanged=false;
	originaldb=NULL;

	tmpdir=NULL;

	logdir=NULL;
	debugdir=NULL;

	unixsocket=NULL;
	unixsocketptr=NULL;
	unixsocketptrlen=0;
	serversockun=NULL;
	serversockin=NULL;
	serversockincount=0;

	inetport=0;

	users=NULL;
	passwords=NULL;
	passwordencryptions=NULL;
	usercount=0;

	lastuserbuffer[0]='\0';
	lastpasswordbuffer[0]='\0';
	lastauthsuccess=false;

	needcommitorrollback=false;

	autocommitforthissession=false;

	translatebegins=true;
	faketransactionblocks=false;
	faketransactionblocksautocommiton=false;
	intransactionblock=false;

	fakeinputbinds=false;
	translatebinds=false;

	bindswerefaked=false;
	querywasintercepted=false;
	executedsinceprepare=false;

	isolationlevel=NULL;

	maxquerysize=0;
	maxbindcount=0;
	maxerrorlength=0;
	idleclienttimeout=-1;

	connected=false;
	inclientsession=false;
	loggedin=false;
	reloginseed=0;
	relogintime=0;

	// maybe someday these parameters will be configurable
	bindmappingspool=new memorypool(512,128,100);
	inbindmappings=new namevaluepairs;
	outbindmappings=new namevaluepairs;

	sqlp=NULL;
	sqlrt=NULL;
	sqlrrst=NULL;
	sqlw=NULL;
	sqlrtr=NULL;
	sqlrlg=NULL;
	sqlrq=NULL;
	sqlrpe=NULL;
	sqlra=NULL;

	decrypteddbpassword=NULL;

	debugsqlrtranslation=false;
	debugtriggers=false;
	debugbindtranslation=false;

	cur=NULL;

	pidfile=NULL;

	decrementonclose=false;
	silent=false;

	loggedinsec=0;
	loggedinusec=0;

	dbhostname=NULL;
	dbipaddress=NULL;

	reformatdatetimes=false;
	reformattedfield=NULL;
	reformattedfieldlength=0;

	proxymode=false;
	proxypid=0;
}

sqlrcontroller_svr::~sqlrcontroller_svr() {

	shutDown();

	if (connstats) {
		bytestring::zero(connstats,sizeof(sqlrconnstatistics));
	}

	delete cmdl;
	delete cfgfl;

	delete[] updown;

	delete[] originaldb;

	delete[] logdir;
	delete[] debugdir;

	delete tmpdir;

	for (uint32_t i=0; i<usercount; i++) {
		delete[] users[i];
		delete[] passwords[i];
		delete[] passwordencryptions[i];
	}
	delete[] users;
	delete[] passwords;
	delete[] passwordencryptions;

	delete shmem;

	delete semset;

	if (unixsocket) {
		file::remove(unixsocket);
		delete[] unixsocket;
	}

	delete bindmappingspool;
	delete inbindmappings;
	delete outbindmappings;

	delete sqlp;
	delete sqlrt;
	delete sqlrrst;
	delete sqlw;
	delete sqlrtr;
	delete sqlrlg;
	delete sqlrq;
	delete sqlrpe;
	delete sqlra;

	delete[] decrypteddbpassword;

	if (pidfile) {
		file::remove(pidfile);
		delete[] pidfile;
	}

	delete[] reformattedfield;

	delete conn;
}

bool sqlrcontroller_svr::init(int argc, const char **argv) {

	// process command line
	cmdl=new cmdline(argc,argv);

	// set the tmpdir
	tmpdir=new tempdir(cmdl);

	// set the log and debug dirs
	const char	*localstatedir=cmdl->getLocalStateDir();
	if (localstatedir && localstatedir[0]) {

		size_t	dirlen=charstring::length(localstatedir)+14+1;
		logdir=new char[dirlen];
		charstring::printf(logdir,dirlen,
					"%s/sqlrelay/log/",localstatedir);

		dirlen=charstring::length(localstatedir)+16+1;
		debugdir=new char[dirlen];
		charstring::printf(debugdir,dirlen,
					"%s/sqlrelay/debug/",localstatedir);
	} else {
		logdir=charstring::duplicate(LOG_DIR);
		debugdir=charstring::duplicate(DEBUG_DIR);
	}

	// default id warning
	if (!charstring::compare(cmdl->getId(),DEFAULT_ID)) {
		stderror.printf("Warning: using default id.\n");
	}

	// get whether this connection was spawned by the scaler
	scalerspawned=cmdl->found("-scaler");

	// get the connection id from the command line
	connectionid=cmdl->getValue("-connectionid");
	if (!connectionid[0]) {
		connectionid=DEFAULT_CONNECTIONID;
		stderror.printf("Warning: using default connectionid.\n");
	}

	// get the time to live from the command line
	const char	*ttlstr=cmdl->getValue("-ttl");
	ttl=(ttlstr)?charstring::toInteger(cmdl->getValue("-ttl")):-1;

	silent=cmdl->found("-silent");

	// parse the config file
	cfgfl=new sqlrconfigfile();
	if (!cfgfl->parse(cmdl->getConfig(),cmdl->getId())) {
		return false;
	}

	// update various configurable parameters
	maxquerysize=cfgfl->getMaxQuerySize();
	maxbindcount=cfgfl->getMaxBindCount();
	maxerrorlength=cfgfl->getMaxErrorLength();
	idleclienttimeout=cfgfl->getIdleClientTimeout();
	reformatdatetimes=(cfgfl->getDateTimeFormat() ||
				cfgfl->getDateFormat() ||
				cfgfl->getTimeFormat());

	// get password encryptions
	const char	*pwdencs=cfgfl->getPasswordEncryptions();
	if (pwdencs && pwdencs[0]) {
		sqlrpe=new sqlrpwdencs;
		sqlrpe->loadPasswordEncryptions(pwdencs);
	}	

	// initialize authentication
	initLocalAuthentication();
	const char	*auths=cfgfl->getAuthentications();
	if (auths && auths[0]) {
		sqlra=new sqlrauths;
		sqlra->loadAuthenticators(auths,sqlrpe);
	}

	setUserAndGroup();

	// load database plugin
	conn=initConnection(cfgfl->getDbase());
	if (!conn) {
		return false;
	}

	// get loggers
	const char	*loggers=cfgfl->getLoggers();
	if (loggers && loggers[0]) {
		sqlrlg=new sqlrloggers;
		sqlrlg->loadLoggers(loggers);
		sqlrlg->initLoggers(NULL,conn);
	}

	// handle the unix socket directory
	if (cfgfl->getListenOnUnix()) {
		setUnixSocketDirectory();
	}

	// handle the pid file
	if (!handlePidFile()) {
		return false;
	}

	// handle the connect string
	constr=cfgfl->getConnectString(connectionid);
	if (!constr) {
		stderror.printf("Error: invalid connectionid \"%s\".\n",
								connectionid);
		return false;
	}
	conn->handleConnectString();

	initDatabaseAvailableFileName();

	if (cfgfl->getListenOnUnix() && !getUnixSocket()) {
		return false;
	}

	if (!createSharedMemoryAndSemaphores(cmdl->getId())) {
		return false;
	}

	// log in and detach
	if (conn->mustDetachBeforeLogIn() && !cmdl->found("-nodetach")) {
		process::detach();
	}
	bool	reloginatstart=cfgfl->getReLoginAtStart();
	if (!reloginatstart) {
		if (!attemptLogIn(!silent)) {
			return false;
		}
	}
	if (!conn->mustDetachBeforeLogIn() && !cmdl->found("-nodetach")) {
		process::detach();
	}
	if (reloginatstart) {
		while (!attemptLogIn(false)) {
			snooze::macrosnooze(5);
		}
	}
	initConnStats();

	// get the query translators
	debugsqlrtranslation=cfgfl->getDebugTranslations();
	const char	*translations=cfgfl->getTranslations();
	if (translations && translations[0]) {
		sqlp=new sqlparser;
		sqlrt=new sqlrtranslations(debugsqlrtranslation);
		sqlrt->loadTranslations(translations);
		sqlw=new sqlwriter;
	}

	// get the result set translators
	const char	*resultsettranslations=
				cfgfl->getResultSetTranslations();
	if (resultsettranslations && resultsettranslations[0]) {
		sqlrrst=new sqlrresultsettranslations;
		sqlrrst->loadResultSetTranslations(resultsettranslations);
	}

	// get the triggers
	debugtriggers=cfgfl->getDebugTriggers();
	const char	*triggers=cfgfl->getTriggers();
	if (triggers && triggers[0]) {
		// for triggers, we'll need an sqlparser as well
		if (!sqlp) {
			sqlp=new sqlparser;
		}
		sqlrtr=new sqlrtriggers(debugtriggers);
		sqlrtr->loadTriggers(triggers);
	}

	// set autocommit behavior
	setAutoCommit(conn->getAutoCommit());

	// get fake input bind variable behavior
	// (this may have already been set true by the connect string)
	fakeinputbinds=(fakeinputbinds || cfgfl->getFakeInputBindVariables());

	// get translate bind variable behavior
	translatebinds=cfgfl->getTranslateBindVariables();
	debugbindtranslation=cfgfl->getDebugBindTranslations();

	// initialize cursors
	mincursorcount=cfgfl->getCursors();
	maxcursorcount=cfgfl->getMaxCursors();
	if (!initCursors(mincursorcount)) {
		closeCursors(false);
		logOut();
		return false;
	}

	// create connection pid file
	pid_t	pid=process::getProcessId();
	size_t	pidfilelen=tmpdir->getLength()+22+
				charstring::length(cmdl->getId())+1+
				charstring::integerLength((uint64_t)pid)+1;
	pidfile=new char[pidfilelen];
	charstring::printf(pidfile,pidfilelen,
				"%s/pids/sqlr-connection-%s.%ld",
				tmpdir->getString(),cmdl->getId(),(long)pid);
	process::createPidFile(pidfile,permissions::ownerReadWrite());

	// increment connection counter
	if (cfgfl->getDynamicScaling()) {
		incrementConnectionCount();
	}

	// set the transaction isolation level
	isolationlevel=cfgfl->getIsolationLevel();
	conn->setIsolationLevel(isolationlevel);

	// get the database/schema we're using so
	// we can switch back to it at end of session
	originaldb=conn->getCurrentDatabase();

	markDatabaseAvailable();

	// get the custom query handlers
	const char	*queries=cfgfl->getQueries();
	if (queries && queries[0]) {
		sqlrq=new sqlrqueries;
		sqlrq->loadQueries(queries);
	}

	// init client protocols
	for (uint8_t i=0; i<SQLRPROTOCOLCOUNT; i++) {
		sqlrp[i]=NULL;
	}
	sqlrp[SQLRPROTOCOL_SQLRCLIENT]=new sqlrclientprotocol(this,conn);

	// set a handler for SIGALRMs
	#ifdef SIGALRM
	alarmhandler.setHandler(alarmHandler);
	alarmhandler.handleSignal(SIGALRM);
	#endif
	
	return true;
}

void sqlrcontroller_svr::setUserAndGroup() {

	// get the user that we're currently running as
	char	*currentuser=
		userentry::getName(process::getEffectiveUserId());

	// get the group that we're currently running as
	char	*currentgroup=
		groupentry::getName(process::getEffectiveGroupId());

	// switch groups, but only if we're not currently running as the
	// group that we should switch to
	if (charstring::compare(currentgroup,cfgfl->getRunAsGroup()) &&
				!process::setGroup(cfgfl->getRunAsGroup())) {
		stderror.printf("Warning: could not change group to %s\n",
						cfgfl->getRunAsGroup());
	}

	// switch users, but only if we're not currently running as the
	// user that we should switch to
	if (charstring::compare(currentuser,cfgfl->getRunAsUser()) &&
				!process::setUser(cfgfl->getRunAsUser())) {
		stderror.printf("Warning: could not change user to %s\n",
						cfgfl->getRunAsUser());
	}

	// clean up
	delete[] currentuser;
	delete[] currentgroup;
}

sqlrconnection_svr *sqlrcontroller_svr::initConnection(const char *dbase) {

#ifdef SQLRELAY_ENABLE_SHARED
	// load the connection module
	stringbuffer	modulename;
	modulename.append(LIBEXECDIR);
	modulename.append("/sqlrconnection_");
	modulename.append(dbase)->append(".")->append(SQLRELAY_MODULESUFFIX);
	if (!dl.open(modulename.getString(),true,true)) {
		stderror.printf("failed to load connection module: %s\n",
							modulename.getString());
		char	*error=dl.getError();
		stderror.printf("%s\n",error);
		delete[] error;
		return NULL;
	}

	// load the connection itself
	stringbuffer	functionname;
	functionname.append("new_")->append(dbase)->append("connection");
	sqlrconnection_svr	*(*newConn)(sqlrcontroller_svr *)=
				(sqlrconnection_svr *(*)(sqlrcontroller_svr *))
					dl.getSymbol(functionname.getString());
	if (!newConn) {
		stderror.printf("failed to load connection: %s\n",dbase);
		char	*error=dl.getError();
		stderror.printf("%s\n",error);
		delete[] error;
		return NULL;
	}

	sqlrconnection_svr	*conn=(*newConn)(this);

#else
	sqlrconnection_svr	*conn;
	stringbuffer		connectionname;
	connectionname.append(dbase)->append("connection");
	#include "sqlrconnectionassignments.cpp"
	{
		conn=NULL;
	}
#endif

	if (!conn) {
		stderror.printf("failed to create connection: %s\n",dbase);
#ifdef SQLRELAY_ENABLE_SHARED
		char	*error=dl.getError();
		stderror.printf("%s\n",error);
		delete[] error;
#endif
	}
	return conn;
}

void sqlrcontroller_svr::setUnixSocketDirectory() {
	size_t	unixsocketlen=tmpdir->getLength()+31;
	unixsocket=new char[unixsocketlen];
	charstring::printf(unixsocket,unixsocketlen,
				"%s/sockets/",tmpdir->getString());
	unixsocketptr=unixsocket+tmpdir->getLength()+8+1;
	unixsocketptrlen=unixsocketlen-(unixsocketptr-unixsocket);
}

bool sqlrcontroller_svr::handlePidFile() {

	// check for listener's pid file
	// (Look a few times.  It might not be there right away.  The listener
	// writes it out after forking and it's possible that the connection
	// might start up after the sqlr-listener has forked, but before it
	// writes out the pid file)
	size_t	listenerpidfilelen=tmpdir->getLength()+20+
				charstring::length(cmdl->getId())+1;
	char	*listenerpidfile=new char[listenerpidfilelen];
	charstring::printf(listenerpidfile,listenerpidfilelen,
				"%s/pids/sqlr-listener-%s",
				tmpdir->getString(),cmdl->getId());

	bool	retval=true;
	bool	found=false;
	for (uint8_t i=0; !found && i<10; i++) {
		if (i) {
			snooze::microsnooze(0,100000);
		}
		found=(process::checkForPidFile(listenerpidfile)!=-1);
	}
	if (!found) {
		stderror.printf("\nsqlr-connection error:\n");
		stderror.printf("	The pid file %s",listenerpidfile);
		stderror.printf(" was not found.\n");
		stderror.printf("	This usually means "
					"that the sqlr-listener \n");
		stderror.printf("is not running.\n");
		stderror.printf("	The sqlr-listener must be running ");
		stderror.printf("for the sqlr-connection to start.\n\n");
		retval=false;
	}

	delete[] listenerpidfile;

	return retval;
}

void sqlrcontroller_svr::initDatabaseAvailableFileName() {

	// initialize the database up/down filename
	size_t	updownlen=charstring::length(tmpdir->getString())+5+
					charstring::length(cmdl->getId())+1+
					charstring::length(connectionid)+1;
	updown=new char[updownlen];
	charstring::printf(updown,updownlen,"%s/ipc/%s-%s",
			tmpdir->getString(),cmdl->getId(),connectionid);
}

bool sqlrcontroller_svr::getUnixSocket() {

	logDebugMessage("getting unix socket...");

	file	sockseq;
	if (!openSequenceFile(&sockseq) || !lockSequenceFile(&sockseq)) {
		return false;
	}
	if (!getAndIncrementSequenceNumber(&sockseq)) {
		unLockSequenceFile(&sockseq);
		sockseq.close();
		return false;
	}
	if (!unLockSequenceFile(&sockseq)) {
		sockseq.close();
		return false;
	}
	if (!sockseq.close()) {
		return false;
	}

	logDebugMessage("done getting unix socket");

	return true;
}

bool sqlrcontroller_svr::openSequenceFile(file *sockseq) {

	// open the sequence file and get the current port number
	size_t	sockseqnamelen=tmpdir->getLength()+9;
	char	*sockseqname=new char[sockseqnamelen];
	charstring::printf(sockseqname,sockseqnamelen,
				"%s/sockseq",tmpdir->getString());

	size_t	stringlen=8+charstring::length(sockseqname)+1;
	char	*string=new char[stringlen];
	charstring::printf(string,stringlen,"opening %s",sockseqname);
	logDebugMessage(string);
	delete[] string;

	mode_t	oldumask=process::setFileCreationMask(011);
	bool	success=sockseq->open(sockseqname,O_RDWR|O_CREAT,
					permissions::everyoneReadWrite());
	process::setFileCreationMask(oldumask);

	// handle error
	if (!success) {

		stderror.printf("Could not open: %s\n",sockseqname);
		stderror.printf("Make sure that the file and directory are \n");
		stderror.printf("readable and writable.\n\n");
		unixsocketptr[0]='\0';

		debugstr.clear();
		debugstr.append("failed to open socket sequence file: ");
		debugstr.append(sockseqname);
		logInternalError(NULL,debugstr.getString());
		delete[] string;
	}

	delete[] sockseqname;

	return success;
}

bool sqlrcontroller_svr::lockSequenceFile(file *sockseq) {

	logDebugMessage("locking...");

	if (!sockseq->lockFile(F_WRLCK)) {
		logInternalError(NULL,"failed to lock socket sequence file");
		return false;
	}
	return true;
}

bool sqlrcontroller_svr::unLockSequenceFile(file *sockseq) {

	// unlock and close the file in a platform-independent manner
	logDebugMessage("unlocking...");

	if (!sockseq->unlockFile()) {
		logInternalError(NULL,"failed to unlock socket sequence file");
		return false;
	}
	return true;
}

bool sqlrcontroller_svr::getAndIncrementSequenceNumber(file *sockseq) {

	// get the sequence number from the file
	int32_t	buffer;
	if (sockseq->read(&buffer)!=sizeof(int32_t)) {
		buffer=0;
	}
	charstring::printf(unixsocketptr,unixsocketptrlen,"%d",buffer);

	size_t	stringlen=21+charstring::length(unixsocketptr)+1;
	char	*string=new char[stringlen];
	charstring::printf(string,stringlen,
			"got sequence number: %s",unixsocketptr);
	logDebugMessage(string);
	delete[] string;

	// increment the sequence number but don't let it roll over
	if (buffer==2147483647) {
		buffer=0;
	} else {
		buffer=buffer+1;
	}

	string=new char[50];
	charstring::printf(string,50,"writing new sequence number: %d",buffer);
	logDebugMessage(string);
	delete[] string;

	// write the sequence number back to the file
	if (sockseq->setPositionRelativeToBeginning(0)==-1 ||
			sockseq->write(buffer)!=sizeof(int32_t)) {
		logInternalError(NULL,"failed to update socket sequence file");
		return false;
	}
	return true;
}

bool sqlrcontroller_svr::attemptLogIn(bool printerrors) {

	logDebugMessage("logging in...");

	// log in
	if (!logIn(printerrors)) {
		return false;
	}

	// get stats
	datetime	dt;
	dt.getSystemDateAndTime();
	loggedinsec=dt.getSeconds();
	loggedinusec=dt.getMicroseconds();

	logDebugMessage("done logging in");
	return true;
}

bool sqlrcontroller_svr::logIn(bool printerrors) {

	// don't do anything if we're already logged in
	if (loggedin) {
		return true;
	}

	// attempt to log in
	const char	*err=NULL;
	if (!conn->logIn(&err)) {
		if (printerrors) {
			stderror.printf("Couldn't log into database.\n");
			if (err) {
				stderror.printf("%s\n",err);
			}
		}
		if (sqlrlg) {
			debugstr.clear();
			debugstr.append("database login failed");
			if (err) {
				debugstr.append(": ")->append(err);
			}
			logInternalError(NULL,debugstr.getString());
		}
		return false;
	}

	// success... update stats
	incrementOpenDatabaseConnections();

	// update db host name and ip address
	// (Only do this if logging is enabled.  For now only the loggers use
	// them, and if someone forgot to put the database host name in DNS
	// then it can cause the connection to delay until a DNS timeout occurs
	// to start.)
	if (sqlrlg) {
		dbhostname=conn->dbHostName();
		dbipaddress=conn->dbIpAddress();
	}

	loggedin=true;

	// If the db is behind a load balancer, we need to re-login
	// periodically to redistribute connections over newly added nodes.
	// Determine when to re-login next.
	if (constr->getBehindLoadBalancer()) {
		datetime	dt;
		if (dt.getSystemDateAndTime()) {
			if (!reloginseed) {
				// Ideally we'd use randomnumber:getSeed for
				// this, but on some platforms that's generated
				// from the epoch and could end up being the
				// same for all sqlr-connections.  The process
				// id is guaranteed unique.
				reloginseed=process::getProcessId();
			}
			reloginseed=randomnumber::generateNumber(reloginseed);
			int32_t	seconds=randomnumber::scaleNumber(
							reloginseed,600,900);
			relogintime=dt.getEpoch()+seconds;
		}
	}

	logDbLogIn();

	return true;
}

void sqlrcontroller_svr::logOut() {

	// don't do anything if we're already logged out
	if (!loggedin) {
		return;
	}

	logDebugMessage("logging out...");

	// log out
	conn->logOut();

	// update stats
	decrementOpenDatabaseConnections();

	logDbLogOut();

	loggedin=false;

	logDebugMessage("done logging out");
}

void sqlrcontroller_svr::setAutoCommit(bool ac) {
	logDebugMessage("setting autocommit...");
	if (ac) {
		if (!autoCommitOn()) {
			logDebugMessage("setting autocommit on failed");
			stderror.printf("Couldn't set autocommit on.\n");
			return;
		}
	} else {
		if (!autoCommitOff()) {
			logDebugMessage("setting autocommit off failed");
			stderror.printf("Couldn't set autocommit off.\n");
			return;
		}
	}
	logDebugMessage("done setting autocommit");
}

bool sqlrcontroller_svr::initCursors(uint16_t count) {

	logDebugMessage("initializing cursors...");

	cursorcount=count;
	if (!cur) {
		cur=new sqlrcursor_svr *[maxcursorcount];
		bytestring::zero(cur,maxcursorcount*sizeof(sqlrcursor_svr *));
	}

	for (uint16_t i=0; i<cursorcount; i++) {

		if (!cur[i]) {
			cur[i]=newCursor(i);
		}
		if (!cur[i]->open()) {
			debugstr.clear();
			debugstr.append("cursor init failed: ")->append(i);
			logInternalError(NULL,debugstr.getString());
			return false;
		}
	}

	logDebugMessage("done initializing cursors");

	return true;
}

sqlrcursor_svr *sqlrcontroller_svr::newCursor(uint16_t id) {
	sqlrcursor_svr	*cursor=conn->newCursor(id);
	if (cursor) {
		incrementOpenDatabaseCursors();
	}
	return cursor;
}

sqlrcursor_svr *sqlrcontroller_svr::newCursor() {
	// return a cursor with an ID that can't already exist
	return newCursor(cursorcount+1);
}

void sqlrcontroller_svr::incrementConnectionCount() {

	logDebugMessage("incrementing connection count...");

	if (scalerspawned) {

		logDebugMessage("scaler will do the job");
		signalScalerToRead();

	} else {

		acquireConnectionCountMutex();

		// increment the counter
		shm->totalconnections++;
		decrementonclose=true;

		releaseConnectionCountMutex();
	}

	logDebugMessage("done incrementing connection count");
}

void sqlrcontroller_svr::decrementConnectionCount() {

	logDebugMessage("decrementing connection count...");

	if (scalerspawned) {

		logDebugMessage("scaler will do the job");

	} else {

		acquireConnectionCountMutex();

		if (shm->totalconnections) {
			shm->totalconnections--;
		}
		decrementonclose=false;

		releaseConnectionCountMutex();
	}

	logDebugMessage("done decrementing connection count");
}

void sqlrcontroller_svr::markDatabaseAvailable() {

	size_t	stringlen=9+charstring::length(updown)+1;
	char	*string=new char[stringlen];
	charstring::printf(string,stringlen,"creating %s",updown);
	logDebugMessage(string);
	delete[] string;

	// the database is up if the file is there, 
	// opening and closing it will create it
	file	fd;
	fd.create(updown,permissions::ownerReadWrite());
}

void sqlrcontroller_svr::markDatabaseUnavailable() {

	// if the database is behind a load balancer, don't mark it unavailable
	if (constr->getBehindLoadBalancer()) {
		return;
	}

	size_t	stringlen=10+charstring::length(updown)+1;
	char	*string=new char[stringlen];
	charstring::printf(string,stringlen,"unlinking %s",updown);
	logDebugMessage(string);
	delete[] string;

	// the database is down if the file isn't there
	file::remove(updown);
}

bool sqlrcontroller_svr::openSockets() {

	logDebugMessage("listening on sockets...");

	// get the next available unix socket and open it
	if (cfgfl->getListenOnUnix() && unixsocketptr && unixsocketptr[0]) {

		if (!serversockun) {
			serversockun=new unixsocketserver();
			if (serversockun->listen(unixsocket,0000,5)) {

				size_t	stringlen=26+
					charstring::length(unixsocket)+1;
				char	*string=new char[stringlen];
				charstring::printf(string,stringlen,
						"listening on unix socket: %s",
						unixsocket);
				logDebugMessage(string);
				delete[] string;

				addReadFileDescriptor(serversockun);

			} else {
				debugstr.clear();
				debugstr.append("failed to listen on socket: ");
				debugstr.append(unixsocket);
				logInternalError(NULL,debugstr.getString());

				stderror.printf("Could not listen on ");
				stderror.printf("unix socket: ");
				stderror.printf("%s\n",unixsocket);
				stderror.printf("Make sure that the file and ");
				stderror.printf("directory are readable ");
				stderror.printf("and writable.\n\n");
				delete serversockun;
				return false;
			}
		}
	}

	// open the next available inet socket
	if (cfgfl->getListenOnInet()) {

		if (!serversockin) {
			const char * const *addresses=cfgfl->getAddresses();
			serversockincount=cfgfl->getAddressCount();
			serversockin=new inetsocketserver *[serversockincount];
			bool	failed=false;
			for (uint64_t index=0;
					index<serversockincount;
					index++) {
				serversockin[index]=NULL;
				if (failed) {
					continue;
				}
				serversockin[index]=new inetsocketserver();
				if (serversockin[index]->
					listen(addresses[index],inetport,5)) {

					if (!inetport) {
						inetport=serversockin[index]->
								getPort();
					}

					char	string[33];
					charstring::printf(string,33,
						"listening on inet socket: %d",
						inetport);
					logDebugMessage(string);
	
					addReadFileDescriptor(
							serversockin[index]);

				} else {
					debugstr.clear();
					debugstr.append("failed to listen "
							"on port: ");
					debugstr.append(inetport);
					logInternalError(NULL,
							debugstr.getString());

					stderror.printf("Could not listen on ");
					stderror.printf("inet socket: ");
					stderror.printf("%d\n\n",inetport);
					failed=true;
				}
			}
			if (failed) {
				for (uint64_t index=0;
						index<serversockincount;
						index++) {
					delete serversockin[index];
				}
				delete[] serversockin;
				serversockincount=0;
				return false;
			}
		}
	}

	logDebugMessage("done listening on sockets");

	return true;
}

bool sqlrcontroller_svr::listen() {

	uint16_t	sessioncount=0;
	bool		clientconnectfailed=false;

	for (;;) {

		waitForAvailableDatabase();
		initSession();
		if (!announceAvailability(unixsocket,inetport,connectionid)) {
			return true;
		}

		// loop to handle suspended sessions
		bool	loopback=false;
		for (;;) {

			int	success=waitForClient();

			if (success==1) {

				suspendedsession=false;

				// have a session with the client
				clientSession();

				// break out of the loop unless the client
				// suspended the session
				if (!suspendedsession) {
					break;
				}

			} else if (success==2) {

				// this is a special case, basically it means
				// that the listener wants the connection to
				// reconnect to the database, just loop back
				// so that can be handled naturally
				loopback=true;
				break;

			} else if (success==-1) {

				// if waitForClient() errors out, break out of
				// the suspendedsession loop and loop back
				// for another session and close connection if
				// it is possible otherwise wait for session,
				// but it seems that on hard load it's
				// impossible to change handoff socket for pid
				clientconnectfailed=true;
				break;

			} else if (success==0) {

				// if waitForClient() times out or otherwise
				// fails to wait for someone to pick up the
				// suspended session then roll back and break
				if (conn->isTransactional()) {
					rollback();
				}
				suspendedsession=false;
				break;
			}
		}

		if (!loopback && cfgfl->getDynamicScaling()) {

			decrementConnectedClientCount();

			if (scalerspawned) {

				if (clientconnectfailed) {
					return false;
				}

				if (ttl==0) {
					return true;
				}

				if (ttl>0 && cfgfl->getMaxSessionCount()) {
					sessioncount++;
					if (sessioncount==
						cfgfl->getMaxSessionCount()) {
						return true;
					}
				}
			}
		}
	}
}

void sqlrcontroller_svr::waitForAvailableDatabase() {

	logDebugMessage("waiting for available database...");

	updateState(WAIT_FOR_AVAIL_DB);

	if (!file::exists(updown)) {
		logDebugMessage("database is not available");
		reLogIn();
		markDatabaseAvailable();
	}

	logDebugMessage("database is available");
}

void sqlrcontroller_svr::reLogIn() {

	markDatabaseUnavailable();

	// run the session end queries
	// FIXME: only run these if a dead connection prompted
	// a relogin, not if we couldn't login at startup
	sessionEndQueries();

	// get the current db so we can restore it
	char	*currentdb=conn->getCurrentDatabase();

	// FIXME: get the isolation level so we can restore it

	logDebugMessage("relogging in...");

	// attempt to log in over and over, once every 5 seconds
	int32_t	oldcursorcount=cursorcount;
	closeCursors(false);
	logOut();
	for (;;) {
			
		logDebugMessage("trying...");

		incrementReLogInCount();

		if (logIn(false)) {
			if (!initCursors(oldcursorcount)) {
				closeCursors(false);
				logOut();
			} else {
				break;
			}
		}
		snooze::macrosnooze(5);
	}

	logDebugMessage("done relogging in");

	// run the session-start queries
	// FIXME: only run these if a dead connection prompted
	// a relogin, not if we couldn't login at startup
	sessionStartQueries();

	// restore the db
	conn->selectDatabase(currentdb);
	delete[] currentdb;

	// restore autocommit
	if (conn->getAutoCommit()) {
		conn->autoCommitOn();
	} else {
		conn->autoCommitOff();
	}

	// FIXME: restore the isolation level

	markDatabaseAvailable();
}

void sqlrcontroller_svr::initSession() {

	logDebugMessage("initializing session...");

	needcommitorrollback=false;
	suspendedsession=false;
	for (int32_t i=0; i<cursorcount; i++) {
		cur[i]->setState(SQLRCURSORSTATE_AVAILABLE);
	}
	accepttimeout=5;

	logDebugMessage("done initializing session...");
}

bool sqlrcontroller_svr::announceAvailability(const char *unixsocket,
						unsigned short inetport,
						const char *connectionid) {

	logDebugMessage("announcing availability...");

	// connect to listener if we haven't already
	// and pass it this process's pid
	if (!connected) {
		registerForHandoff();
	}

	// get a pointer to the shared memory segment
	shmdata	*shmemptr=getAnnounceBuffer();

	// set time-to-live alarm
	if (ttl>0) {
		alarmrang=0;
		signalmanager::alarm(ttl);
	}

	// This will fall through if the ttl was reached while waiting.
	// In that case, since we failed to acquire the announce mutex,
	// we don't need to release it.
	if (!acquireAnnounceMutex()) {
		logDebugMessage("ttl reached, aborting announcing availabilty");
		return false;
	}

	updateState(ANNOUNCE_AVAILABILITY);

	// write the connectionid and pid into the segment
	charstring::copy(shmemptr->connectionid,
				connectionid,MAXCONNECTIONIDLEN);
	shmemptr->connectioninfo.connectionpid=process::getProcessId();

	signalListenerToRead();

	// This will fall through if the ttl was reached while waiting.
	// Since we acquired the announce mutex earlier though, we need to
	// release it in either case.
	bool	retval=waitForListenerToFinishReading();

	// turn off the alarm
	signalmanager::alarm(0);

	releaseAnnounceMutex();

	if (retval) {
		logDebugMessage("done announcing availability...");
	} else {
		logDebugMessage("ttl reached, aborting announcing availabilty");
	}
	return retval;
}

void sqlrcontroller_svr::registerForHandoff() {

	logDebugMessage("registering for handoff...");

	// construct the name of the socket to connect to
	size_t	handoffsocknamelen=tmpdir->getLength()+9+
				charstring::length(cmdl->getId())+8+1;
	char	*handoffsockname=new char[handoffsocknamelen];
	charstring::printf(handoffsockname,handoffsocknamelen,
					"%s/sockets/%s-handoff",
					tmpdir->getString(),cmdl->getId());

	size_t	stringlen=17+charstring::length(handoffsockname)+1;
	char	*string=new char[stringlen];
	charstring::printf(string,stringlen,
				"handoffsockname: %s",handoffsockname);
	logDebugMessage(string);
	delete[] string;

	// Try to connect over and over forever on 1 second intervals.
	// If the connect succeeds but the write fails, loop back and
	// try again.
	connected=false;
	for (;;) {

		logDebugMessage("trying...");

		if (handoffsockun.connect(handoffsockname,-1,-1,1,0)==
							RESULT_SUCCESS) {
			handoffsockun.dontUseNaglesAlgorithm();
			if (handoffsockun.write(
				(uint32_t)process::getProcessId())==
							sizeof(uint32_t)) {
				connected=true;
				break;
			}
			handoffsockun.flushWriteBuffer(-1,-1);
			deRegisterForHandoff();
		}
		snooze::macrosnooze(1);
	}

	logDebugMessage("done registering for handoff");

	delete[] handoffsockname;
}

void sqlrcontroller_svr::deRegisterForHandoff() {
	
	logDebugMessage("de-registering for handoff...");

	// construct the name of the socket to connect to
	size_t	removehandoffsocknamelen=tmpdir->getLength()+9+
					charstring::length(cmdl->getId())+14+1;
	char	*removehandoffsockname=new char[removehandoffsocknamelen];
	charstring::printf(removehandoffsockname,
				removehandoffsocknamelen,
				"%s/sockets/%s-removehandoff",
				tmpdir->getString(),cmdl->getId());

	size_t	stringlen=23+charstring::length(removehandoffsockname)+1;
	char	*string=new char[stringlen];
	charstring::printf(string,stringlen,
				"removehandoffsockname: %s",
				removehandoffsockname);
	logDebugMessage(string);
	delete[] string;

	// attach to the socket and write the process id
	unixsocketclient	removehandoffsockun;
	removehandoffsockun.connect(removehandoffsockname,-1,-1,0,1);
	removehandoffsockun.dontUseNaglesAlgorithm();
	removehandoffsockun.write((uint32_t)process::getProcessId());
	removehandoffsockun.flushWriteBuffer(-1,-1);

	logDebugMessage("done de-registering for handoff");

	delete[] removehandoffsockname;
}

int32_t sqlrcontroller_svr::waitForClient() {

	logDebugMessage("waiting for client...");

	updateState(WAIT_CLIENT);

	// reset proxy mode flag
	proxymode=false;

	if (!suspendedsession) {

		// If we're not in the middle of a suspended session,
		// talk to the listener...


		// the client file descriptor
		int32_t	descriptor;

		// What is this loop all aboout?
		// If the listener is proxying clients, it can't tell whether
		// the client succeeded in transmitting an END_SESSION or
		// whether it even tried, so it sends one when the client
		// disconnects no matter what.  If the client did send one then
		// we'll receive a second END_SESSION here.  Depending on the
		// byte order of the host, we'll receive either a 1536 or 6.
		// If we got an END_SESION then just loop back and read again,
		// the command will follow.
		uint16_t	command;
		do {
			// get the command
			if (handoffsockun.read(&command)!=sizeof(uint16_t)) {
				logInternalError(NULL,
					"read handoff command failed");
				logDebugMessage("done waiting for client");
				// If this fails, then the listener most likely
				// died because sqlr-stop was run.  Arguably
				// this condition should initiate a shut down
				// of this process as well, but for now we'll
				// just wait to be shut down manually.
				// Unfortunatley, that means looping over and
				// over, with that read failing every time.
				// We'll sleep so as not to slam the machine
				// while we loop.
				snooze::microsnooze(0,100000);
				return -1;
			}
		} while (command==1536 || command==6);

		if (command==HANDOFF_RECONNECT) {

			// if we're supposed to reconnect, then just do that...
			return 2;

		} else if (command==HANDOFF_PASS) {

			// Receive the client file descriptor and use it.
			if (!handoffsockun.receiveSocket(&descriptor)) {
				logInternalError(NULL,"failed to receive "
						"client file descriptor");
				logDebugMessage("done waiting for client");
				// If this fails, then the listener most likely
				// died because sqlr-stop was run.  Arguably
				// this condition should initiate a shut down
				// of this process as well, but for now we'll
				// just wait to be shut down manually.
				// Unfortunatley, that means looping over and
				// over, with that read above failing every
				// time, thus the  sleep so as not to slam the
				// machine while we loop.
				return -1;
			}

		} else if (command==HANDOFF_PROXY) {

			logDebugMessage("listener is proxying the client");

			// get the listener's pid
			if (handoffsockun.read(&proxypid)!=sizeof(uint32_t)) {
				logInternalError(NULL,
						"failed to read process "
						"id during proxy handoff");
				return -1;
			}

			debugstr.clear();
			debugstr.append("listener pid: ")->append(proxypid);
			logDebugMessage(debugstr.getString());

			// acknowledge
			#define ACK	6
			handoffsockun.write((unsigned char)ACK);
			handoffsockun.flushWriteBuffer(-1,-1);

			descriptor=handoffsockun.getFileDescriptor();

			proxymode=true;

		} else {

			logInternalError(NULL,"received invalid handoff mode");
			return -1;
		}

		// set up the client socket
		clientsock=new socketclient;
		clientsock->setFileDescriptor(descriptor);

		// On most systems, the file descriptor is in whatever mode
		// it was in the other process, but on FreeBSD < 5.0 and
		// possibly other systems, it ends up in non-blocking mode
		// in this process, independent of its mode in the other
		// process.  So, we force it to blocking mode here.
		clientsock->useBlockingMode();

		logDebugMessage("done waiting for client");

	} else {

		// If we're in the middle of a suspended session, wait for
		// a client to reconnect...


		if (listener::listen(accepttimeout,0)<1) {
			logInternalError(NULL,"wait for client connect failed");
			return 0;
		}

		// get the first socket that had data available...
		filedescriptor	*fd=getReadReadyList()->getFirst()->getValue();

		inetsocketserver	*iss=NULL;
		for (uint64_t index=0; index<serversockincount; index++) {
			if (fd==serversockin[index]) {
				iss=serversockin[index];
			}
		}
		if (iss) {
			clientsock=iss->accept();
		} else if (fd==serversockun) {
			clientsock=serversockun->accept();
		}

		if (fd) {
			logDebugMessage("client reconnect succeeded");
		} else {
			logInternalError(NULL,"client reconnect failed");
		}
		logDebugMessage("done waiting for client");

		if (!fd) {
			return 0;
		}
	}

	// set up the socket
	clientsock->translateByteOrder();
	clientsock->dontUseNaglesAlgorithm();
	clientsock->setReadBufferSize(65536);
	//clientsock->setTcpReadBufferSize(65536);
	clientsock->setWriteBufferSize(65536);
	//clientsock->setTcpWriteBufferSize(65536);
	return 1;
}

void sqlrcontroller_svr::clientSession() {

	// determine client protocol
	sqlrprotocol_t	protocol=getClientProtocol();
	if (protocol==SQLRPROTOCOL_UNKNOWN) {
		closeClientConnection(0);
		return;
	}

	logDebugMessage("client session...");

	inclientsession=true;

	// update various stats
	updateState(SESSION_START);
	updateClientAddr();
	updateClientSessionStartTime();
	incrementOpenClientConnections();

	logClientConnected();

	// have client session using the appropriate protocol
	sqlrprotocol		*proto=sqlrp[protocol];
	sqlrclientexitstatus_t	exitstatus=SQLRCLIENTEXITSTATUS_ERROR;
	if (proto) {
		proto->setClientSocket(clientsock);
		exitstatus=proto->clientSession();
	} else {
		closeClientConnection(0);
	}

	closeSuspendedSessionSockets();

	const char	*info;
	switch (exitstatus) {
		case SQLRCLIENTEXITSTATUS_CLOSED_CONNECTION:
			info="client closed connection";
			break;
		case SQLRCLIENTEXITSTATUS_ENDED_SESSION:
			info="client ended the session";
			break;
		case SQLRCLIENTEXITSTATUS_SUSPENDED_SESSION:
			info="client suspended the session";
			break;
		default:
			info="an error occurred";
			break;
	}
	logClientDisconnected(info);

	decrementOpenClientConnections();

	inclientsession=false;

	logDebugMessage("done with client session");
}

sqlrprotocol_t sqlrcontroller_svr::getClientProtocol() {

	uint16_t	value=0;
	ssize_t		result=0;

	// get the first 2 bytes
	result=clientsock->read(&value,idleclienttimeout,0);
	if (result!=sizeof(value)) {
		return SQLRPROTOCOL_UNKNOWN;
	}

	// check for sqlrclient protocol
	if (value!=SQLRCLIENT_PROTOCOL) {
		return SQLRPROTOCOL_UNKNOWN;
	}

	// get the next 2 bytes
	result=clientsock->read(&value,idleclienttimeout,0);
	if (result!=sizeof(value)) {
		return SQLRPROTOCOL_UNKNOWN;
	}

	// check for version 1
	if (value!=1) {
		return SQLRPROTOCOL_UNKNOWN;
	}

	return SQLRPROTOCOL_SQLRCLIENT;
}

sqlrcursor_svr *sqlrcontroller_svr::getCursor(uint16_t id) {

	// get the specified cursor
	for (uint16_t i=0; i<cursorcount; i++) {
		if (cur[i]->getId()==id) {
			incrementTimesCursorReused(); 
			return cur[i];
		}
	}

	debugstr.clear();
	debugstr.append("get cursor failed: "
			"client requested an invalid cursor: ");
	debugstr.append(id);
	logClientProtocolError(NULL,debugstr.getString(),1);

	return NULL;
}

sqlrcursor_svr *sqlrcontroller_svr::getCursor() {

	// find an available cursor
	for (uint16_t i=0; i<cursorcount; i++) {
		if (cur[i]->getState()==SQLRCURSORSTATE_AVAILABLE) {
			debugstr.clear();
			debugstr.append("available cursor: ")->append(i);
			logDebugMessage(debugstr.getString());
			cur[i]->setState(SQLRCURSORSTATE_BUSY);
			incrementTimesNewCursorUsed();
			return cur[i];
		}
	}

	// apparently there weren't any available cursors...

	// if we can't create any new cursors then return an error
	if (cursorcount==maxcursorcount) {
		logDebugMessage("all cursors are busy");
		return NULL;
	}

	// create new cursors
	uint16_t	expandto=cursorcount+cfgfl->getCursorsGrowBy();
	if (expandto>=maxcursorcount) {
		expandto=maxcursorcount;
	}
	uint16_t	firstnewcursor=cursorcount;
	do {
		cur[cursorcount]=newCursor(cursorcount);
		cur[cursorcount]->setState(SQLRCURSORSTATE_AVAILABLE);
		if (!cur[cursorcount]->open()) {
			debugstr.clear();
			debugstr.append("cursor init failure: ");
			debugstr.append(cursorcount);
			logInternalError(NULL,debugstr.getString());
			return NULL;
		}
		cursorcount++;
	} while (cursorcount<expandto);
	
	// return the first new cursor that we created
	cur[firstnewcursor]->setState(SQLRCURSORSTATE_BUSY);
	incrementTimesNewCursorUsed();
	return cur[firstnewcursor];
}

bool sqlrcontroller_svr::authenticate(const char *userbuffer,
						const char *passwordbuffer) {

	logDebugMessage("authenticate...");

	// authenticate on the approprite tier
	if (cfgfl->getAuthOnDatabase() && conn->supportsAuthOnDatabase()) {
		return databaseBasedAuth(userbuffer,passwordbuffer);
	}
	return connectionBasedAuth(userbuffer,passwordbuffer);
}

bool sqlrcontroller_svr::connectionBasedAuth(const char *userbuffer,
						const char *passwordbuffer) {

	// handle connection-based authentication
	bool	retval=
		(authenticateLocal(userbuffer,passwordbuffer) ||
		(sqlra && sqlra->authenticate(userbuffer,passwordbuffer)));
	if (retval) {
		logDebugMessage("auth succeeded on connection");
	} else {
		logClientConnectionRefused("auth failed on connection: "
						"invalid user/password");
	}
	return retval;
}

void sqlrcontroller_svr::initLocalAuthentication() {

	// get the list of users from the config file
	linkedlist< usercontainer * >	*userlist=cfgfl->getUserList();
	usercount=userlist->getLength();

	// create an array of users and passwords and store the
	// users and passwords from the config file in them
	users=new char *[usercount];
	passwords=new char *[usercount];
	passwordencryptions=new char *[usercount];

	usernode	*current=userlist->getFirst();
	for (uint32_t i=0; i<usercount; i++) {
		users[i]=charstring::duplicate(
				current->getValue()->getUser());
		passwords[i]=charstring::duplicate(
				current->getValue()->getPassword());
		passwordencryptions[i]=charstring::duplicate(
				current->getValue()->getPasswordEncryption());
		current=current->getNext();
	}
}

bool sqlrcontroller_svr::authenticateLocal(const char *user,
						const char *password) {

	// run through the user/password arrays...
	for (uint32_t i=0; i<usercount; i++) {

		// if the user matches...
		if (!charstring::compare(user,users[i])) {

			if (sqlrpe &&
				charstring::length(passwordencryptions[i])) {

				// if password encryption is being used...

				// get the module
				sqlrpwdenc	*pe=
					sqlrpe->getPasswordEncryptionById(
							passwordencryptions[i]);
				if (!pe) {
					return false;
				}

				// For one-way encryption, encrypt the password
				// that was passed in and compare it to the
				// encrypted password in the config file.
				// For two-way encryption, decrypt the password
				// from the config file and compare ot to the
				// password that was passed in...

				bool	retval=false;
				char	*pwd=NULL;
				if (pe->oneWay()) {

					// encrypt the password
					// that was passed in
					pwd=pe->encrypt(password);

					// compare it to the encrypted
					// password from the config file
					retval=!charstring::compare(
							pwd,passwords[i]);

				} else {

					// decrypt the password
					// from the config file
					pwd=pe->decrypt(passwords[i]);

					// compare it to the password
					// that was passed in
					retval=!charstring::compare(
							password,pwd);
				}

				// clean up
				delete[] pwd;

				// return true/false
				return retval;

			} else {

				// if password encryption isn't being used,
				// return true if the passwords match
				return !charstring::compare(password,
								passwords[i]);
			}
		}
	}
	return false;
}

bool sqlrcontroller_svr::databaseBasedAuth(const char *userbuffer,
						const char *passwordbuffer) {

	// if the user we want to change to is different from the
	// user that's currently proxied, try to change to that user
	bool	authsuccess;
	if ((!lastuserbuffer[0] && !lastpasswordbuffer[0]) || 
		charstring::compare(lastuserbuffer,userbuffer) ||
		charstring::compare(lastpasswordbuffer,passwordbuffer)) {

		// change authentication 
		logDebugMessage("change user");
		authsuccess=conn->changeUser(userbuffer,passwordbuffer);

		// keep a record of which user we're changing to
		// and whether that user was successful in 
		// authenticating
		charstring::copy(lastuserbuffer,userbuffer);
		charstring::copy(lastpasswordbuffer,passwordbuffer);
		lastauthsuccess=authsuccess;
	}

	if (lastauthsuccess) {
		logDebugMessage("auth succeeded on database");
	} else {
		logClientConnectionRefused("auth failed on database: "
						"invalid user/password");
	}
	return lastauthsuccess;
}

bool sqlrcontroller_svr::changeUser(const char *newuser,
					const char *newpassword) {
	closeCursors(false);
	logOut();
	setUser(newuser);
	setPassword(newpassword);
	return (logIn(false) && initCursors(cursorcount));
}

void sqlrcontroller_svr::beginSession() {
	sessionStartQueries();
}

void sqlrcontroller_svr::suspendSession(const char **unixsocket,
						uint16_t *inetport) {

	// mark the session suspended
	suspendedsession=true;

	// we can't wait forever for the client to resume, set a timeout
	accepttimeout=cfgfl->getSessionTimeout();

	// abort all cursors that aren't suspended...
	logDebugMessage("aborting busy cursors...");
	for (int32_t i=0; i<cursorcount; i++) {
		if (cur[i]->getState()==SQLRCURSORSTATE_BUSY) {
			cur[i]->abort();
		}
	}
	logDebugMessage("done aborting busy cursors");

	// open sockets to resume on
	logDebugMessage("opening sockets to resume on...");
	*unixsocket=NULL;
	*inetport=0;
	if (openSockets()) {
		if (serversockun) {
			*unixsocket=this->unixsocket;
		}
		*inetport=this->inetport;
	}
	logDebugMessage("done opening sockets to resume on");
}

bool sqlrcontroller_svr::autoCommitOn() {
	autocommitforthissession=true;
	return conn->autoCommitOn();
}

bool sqlrcontroller_svr::autoCommitOff() {
	autocommitforthissession=false;
	return conn->autoCommitOff();
}

bool sqlrcontroller_svr::begin() {
	// if we're faking transaction blocks, do that,
	// otherwise run an actual begin query
	return (faketransactionblocks)?
			beginFakeTransactionBlock():conn->begin();
}

bool sqlrcontroller_svr::beginFakeTransactionBlock() {

	// save the current autocommit state
	faketransactionblocksautocommiton=autocommitforthissession;

	// if autocommit is on, turn it off
	if (autocommitforthissession) {
		if (!autoCommitOff()) {
			return false;
		}
	}
	intransactionblock=true;
	return true;
}

bool sqlrcontroller_svr::commit() {
	if (conn->commit()) {
		endFakeTransactionBlock();
		return true;
	}
	return false;
}

bool sqlrcontroller_svr::endFakeTransactionBlock() {

	// if we're faking begins and autocommit is on,
	// reset autocommit behavior
	if (faketransactionblocks && faketransactionblocksautocommiton) {
		if (!autoCommitOn()) {
			return false;
		}
	}
	intransactionblock=false;
	return true;
}

bool sqlrcontroller_svr::rollback() {
	if (conn->rollback()) {
		endFakeTransactionBlock();
		return true;
	}
	return false;
}

bool sqlrcontroller_svr::selectDatabase(const char *db) {
	return (cfgfl->getIgnoreSelectDatabase())?true:conn->selectDatabase(db);
}

void sqlrcontroller_svr::dbHasChanged() {
	this->dbchanged=true;
}

char *sqlrcontroller_svr::getCurrentDatabase() {
	return conn->getCurrentDatabase();
}

bool sqlrcontroller_svr::getLastInsertId(uint64_t *id) {
	return conn->getLastInsertId(id);
}

bool sqlrcontroller_svr::setIsolationLevel(const char *isolevel) {
	return conn->setIsolationLevel(isolevel);
}

bool sqlrcontroller_svr::ping() {
	return conn->ping();
}

bool sqlrcontroller_svr::getListsByApiCalls() {
	return conn->getListsByApiCalls();
}

bool sqlrcontroller_svr::getDatabaseList(sqlrcursor_svr *cursor,
						const char *wild) {
	return conn->getDatabaseList(cursor,wild);
}

bool sqlrcontroller_svr::getTableList(sqlrcursor_svr *cursor,
						const char *wild) {
	return conn->getTableList(cursor,wild);
}

bool sqlrcontroller_svr::getColumnList(sqlrcursor_svr *cursor,
						const char *table,
						const char *wild) {
	return conn->getColumnList(cursor,table,wild);
}

const char *sqlrcontroller_svr::getDatabaseListQuery(bool wild) {
	return conn->getDatabaseListQuery(wild);
}

const char *sqlrcontroller_svr::getTableListQuery(bool wild) {
	return conn->getTableListQuery(wild);
}

const char *sqlrcontroller_svr::getColumnListQuery(const char *table,
							bool wild) {
	return conn->getColumnListQuery(table,wild);
}

void sqlrcontroller_svr::errorMessage(char *errorbuffer,
						uint32_t errorbuffersize,
						uint32_t *errorlength,
						int64_t *errorcode,
						bool *liveconnection) {
	return conn->errorMessage(errorbuffer,errorbuffersize,
					errorlength,errorcode,liveconnection);
}

void sqlrcontroller_svr::clearError() {
	conn->clearError();
}

void sqlrcontroller_svr::setError(const char *err,
					int64_t errn,
					bool liveconn) {
	conn->setError(err,errn,liveconn);
}

char *sqlrcontroller_svr::getErrorBuffer() {
	return conn->getErrorBuffer();
}

uint32_t sqlrcontroller_svr::getErrorLength() {
	return conn->getErrorLength();
}

void sqlrcontroller_svr::setErrorLength(uint32_t errorlength) {
	conn->setErrorLength(errorlength);
}

uint32_t sqlrcontroller_svr::getErrorNumber() {
	return conn->getErrorNumber();
}

void sqlrcontroller_svr::setErrorNumber(uint32_t errnum) {
	conn->setErrorNumber(errnum);
}

bool sqlrcontroller_svr::getLiveConnection() {
	return conn->getLiveConnection();
}

void sqlrcontroller_svr::setLiveConnection(bool liveconnection) {
	return conn->setLiveConnection(liveconnection);
}

bool sqlrcontroller_svr::interceptQuery(sqlrcursor_svr *cursor,
						bool *querywasintercepted) {

	*querywasintercepted=false;

	// for now the only queries we're intercepting are related to
	// faking transaction blocks, so we can ignore all of this if
	// that's disabled
	if (!faketransactionblocks) {
		return false;
	}

	// Intercept begins and handle them.  If we're faking begins, commit
	// and rollback queries also need to be intercepted as well, otherwise
	// the query will be sent directly to the db and endFakeBeginTransaction
	// won't get called.
	if (isBeginTransactionQuery(cursor)) {
		*querywasintercepted=true;
		cursor->setInputBindCount(0);
		cursor->setOutputBindCount(0);
		sendcolumninfo=DONT_SEND_COLUMN_INFO;
		if (intransactionblock) {
			cursor->setError(
				"begin while in transaction block",
							999999,false);
			return false;
		}
		return begin();
		// FIXME: if the begin fails and the db api doesn't support
		// a begin command then the connection-level error needs to
		// be copied to the cursor so queryOrBindCursor can report it
	} else if (isCommitQuery(cursor)) {
		*querywasintercepted=true;
		cursor->setInputBindCount(0);
		cursor->setOutputBindCount(0);
		sendcolumninfo=DONT_SEND_COLUMN_INFO;
		if (!intransactionblock) {
			cursor->setError(
				"commit while not in transaction block",
								999998,false);
			return false;
		}
		return commit();
		// FIXME: if the commit fails and the db api doesn't support
		// a commit command then the connection-level error needs to
		// be copied to the cursor so queryOrBindCursor can report it
	} else if (isRollbackQuery(cursor)) {
		*querywasintercepted=true;
		cursor->setInputBindCount(0);
		cursor->setOutputBindCount(0);
		sendcolumninfo=DONT_SEND_COLUMN_INFO;
		if (!intransactionblock) {
			cursor->setError(
				"rollback while not in transaction block",
								999997,false);
			return false;
		}
		return rollback();
		// FIXME: if the rollback fails and the db api doesn't support
		// a rollback command then the connection-level error needs to
		// be copied to the cursor so queryOrBindCursor can report it
	}
	return false;
}

bool sqlrcontroller_svr::isBeginTransactionQuery(sqlrcursor_svr *cursor) {

	// find the start of the actual query
	const char	*ptr=skipWhitespaceAndComments(
					cursor->getQueryBuffer());

	// See if it was any of the different queries used to start a
	// transaction.  IMPORTANT: don't just look for the first 5 characters
	// to be "BEGIN", make sure it's the entire query.  Many db's use
	// "BEGIN" to start a stored procedure block, but in those cases,
	// something will follow it.
	if (!charstring::compareIgnoringCase(ptr,"BEGIN",5)) {

		// make sure there are only spaces, comments or the word "work"
		// after the begin
		const char	*spaceptr=skipWhitespaceAndComments(ptr+5);
		
		if (!charstring::compareIgnoringCase(spaceptr,"WORK",4) ||
			*spaceptr=='\0') {
			return true;
		}
		return false;

	} else if (!charstring::compareIgnoringCase(ptr,"START ",6)) {
		return true;
	}
	return false;
}

bool sqlrcontroller_svr::isCommitQuery(sqlrcursor_svr *cursor) {
	return !charstring::compareIgnoringCase(
			skipWhitespaceAndComments(cursor->getQueryBuffer()),
			"commit",6);
}

bool sqlrcontroller_svr::isRollbackQuery(sqlrcursor_svr *cursor) {
	return !charstring::compareIgnoringCase(
			skipWhitespaceAndComments(cursor->getQueryBuffer()),
			"rollback",8);
}

bool sqlrcontroller_svr::skipComment(const char **ptr, const char *endptr) {
	while (*ptr<endptr && !charstring::compare(*ptr,"--",2)) {
		while (**ptr && **ptr!='\n') {
			(*ptr)++;
		}
	}
	return *ptr!=endptr;
}

bool sqlrcontroller_svr::skipWhitespace(const char **ptr, const char *endptr) {
	while ((**ptr==' ' || **ptr=='\n' || **ptr=='	') && *ptr<endptr) {
		(*ptr)++;
	}
	return *ptr!=endptr;
}

const char *sqlrcontroller_svr::skipWhitespaceAndComments(const char *query) {
	const char	*ptr=query;
	while (*ptr && 
		(*ptr==' ' || *ptr=='\n' || *ptr=='	' || *ptr=='-')) {
		if (*ptr=='-') {
			while (*ptr && *ptr!='\n') {
				ptr++;
			}
		}
		ptr++;
	}
	return ptr;
}

bool sqlrcontroller_svr::translateQuery(sqlrcursor_svr *cursor) {

	// get the query buffer
	char	*querybuffer=cursor->getQueryBuffer();

	if (debugsqlrtranslation) {
		stdoutput.printf("original:\n\"%s\"\n\n",querybuffer);
	}

	// parse the query
	bool	parsed=sqlp->parse(querybuffer);

	// get the parsed tree
	cursor->clearQueryTree();
	cursor->setQueryTree(sqlp->detachTree());
	if (!cursor->getQueryTree()) {
		return false;
	}

	if (debugsqlrtranslation) {
		stdoutput.printf("before translation:\n");
		cursor->getQueryTree()->getRootNode()->print(&stdoutput);
		stdoutput.printf("\n");
	}

	if (!parsed) {
		if (debugsqlrtranslation) {
			stdoutput.printf(
				"parse failed, using original:\n\"%s\"\n\n",
								querybuffer);
		}
		cursor->clearQueryTree();
		return false;
	}

	// apply translation rules
	if (!sqlrt->runTranslations(conn,cursor,cursor->getQueryTree())) {
		return false;
	}

	if (debugsqlrtranslation) {
		stdoutput.printf("after translation:\n");
		cursor->getQueryTree()->getRootNode()->print(&stdoutput);
		stdoutput.printf("\n");
	}

	// write the query back out
	stringbuffer	translatedquery;
	if (!sqlw->write(conn,cursor,cursor->getQueryTree(),&translatedquery)) {
		return false;
	}

	if (debugsqlrtranslation) {
		stdoutput.printf("translated:\n\"%s\"\n\n",
					translatedquery.getString());
	}

	// bail if the translated query is too large
	if (translatedquery.getStringLength()>maxquerysize) {
		if (debugsqlrtranslation) {
			stdoutput.printf("translated query too large\n");
		}
		return false;
	}

	// write the translated query to the cursor's query buffer
	// so it'll be there if we decide to re-execute it later
	charstring::copy(cursor->getQueryBuffer(),
			translatedquery.getString(),
			translatedquery.getStringLength());
	cursor->setQueryLength(
			translatedquery.getStringLength());
	cursor->getQueryBuffer()[cursor->getQueryLength()]='\0';
	return true;
}

enum queryparsestate_t {
	IN_QUERY=0,
	IN_QUOTES,
	BEFORE_BIND,
	IN_BIND
};

void sqlrcontroller_svr::translateBindVariables(sqlrcursor_svr *cursor) {

	// clear bind mappings
	inbindmappings->clear();
	outbindmappings->clear();
	bindmappingspool->deallocate();

	// get query buffer
	char	*querybuffer=cursor->getQueryBuffer();

	// debug
	if (debugbindtranslation) {
		stdoutput.printf("translating bind variables...\n");
		stdoutput.printf("before translation:\n%s\n",querybuffer);
	}
	if (logEnabled()) {
		logDebugMessage("translating bind variables...");
		logDebugMessage("before translation:");
		logDebugMessage(querybuffer);
	}

	// convert queries from whatever bind variable format they currently
	// use to the format required by the database...

	bool			translated=false;
	queryparsestate_t	parsestate=IN_QUERY;
	stringbuffer		newquery;
	stringbuffer		currentbind;
	const char		*endptr=querybuffer+cursor->getQueryLength()-1;

	// use 1-based index for bind variables
	uint16_t	bindindex=1;
	
	// run through the querybuffer...
	char *c=querybuffer;
	do {

		// if we're in the query...
		if (parsestate==IN_QUERY) {

			// if we find a quote, we're in quotes
			if (*c=='\'') {
				parsestate=IN_QUOTES;
			}

			// if we find whitespace or a couple of other things
			// then the next thing could be a bind variable
			if (character::isWhitespace(*c) ||
					*c==',' || *c=='(' || *c=='=') {
				parsestate=BEFORE_BIND;
			}

			// append the character
			newquery.append(*c);
			c++;
			continue;
		}

		// copy anything in quotes verbatim
		if (parsestate==IN_QUOTES) {
			if (*c=='\'') {
				parsestate=IN_QUERY;
			}
			newquery.append(*c);
			c++;
			continue;
		}

		if (parsestate==BEFORE_BIND) {

			// if we find a bind variable...
			// (make sure to catch :'s but not :='s)
			// (make sure to catch @'s but not @@'s)
			if (*c=='?' ||
				(*c==':' && *(c+1)!='=') ||
				(*c=='@' && *(c+1)!='@') ||
				*c=='$') {
				parsestate=IN_BIND;
				currentbind.clear();
				continue;
			}

			// if we didn't find a bind variable then we're just
			// back in the query
			parsestate=IN_QUERY;
			continue;
		}

		// if we're in a bind variable...
		if (parsestate==IN_BIND) {

			// If we find whitespace or a few other things
			// then we're done with the bind variable.  Process it.
			// Otherwise get the variable itself in another buffer.
			bool	endofbind=(character::isWhitespace(*c) ||
						*c==',' || *c==')' || *c==';' ||
						(*c==':' && *(c+1)=='='));
			if (endofbind || c==endptr) {

				// special case if we hit the end of the string
				// an it's not one of the special chars
				if (c==endptr && !endofbind) {
					currentbind.append(*c);
					c++;
				}

				// if the current bind variable format doesn't
				// match the db bind format...
				if (!matchesNativeBindFormat(
						currentbind.getString())) {

					// translate...
					translated=true;
					translateBindVariableInStringAndMap(
								cursor,
								&currentbind,
								bindindex,
								&newquery);
				} else {
					newquery.append(
						currentbind.getString());
				}
				bindindex++;

				parsestate=IN_QUERY;

			} else {
				currentbind.append(*c);
				c++;
			}
			continue;
		}

	} while (c<=endptr);


	// if no translation was performed
	if (!translated) {
		if (debugbindtranslation) {
			stdoutput.printf("\nno bind translation performed\n\n");
		}
		logDebugMessage("no bind translation performed");
		return;
	}


	// if we made it here then some conversion
	// was done - update the querybuffer...
	const char	*newq=newquery.getString();
	uint32_t	newqlen=newquery.getStringLength();
	if (newqlen>maxquerysize) {
		newqlen=maxquerysize;
	}
	charstring::copy(querybuffer,newq,newqlen);
	querybuffer[newqlen]='\0';
	cursor->setQueryLength(newqlen);


	// debug
	if (debugbindtranslation) {
		stdoutput.printf("\nafter translation:\n%s\n\n",querybuffer);
	}
	if (logEnabled()) {
		logDebugMessage("translated:");
		logDebugMessage(querybuffer);
	}
}

bool sqlrcontroller_svr::matchesNativeBindFormat(const char *bind) {

	const char	*bindformat=conn->bindFormat();
	size_t		bindformatlen=charstring::length(bindformat);

	// the bind variable name matches the format if...
	// * the first character of the bind variable name matches the 
	//   first character of the bind format
	//
	//	and...
	//
	// * the format is just a single character
	// 	or..
	// * the second character of the format is a 1 and the second character
	//   of the bind variable name is a digit
	// 	or..
	// * the second character of the format is a * and the second character
	//   of the bind varaible name is alphanumeric
	return (bind[0]==bindformat[0]  &&
		(bindformatlen==1 ||
		(bindformat[1]=='1' && character::isDigit(bind[1])) ||
		(bindformat[1]=='*' && character::isAlphanumeric(bind[1]))));
}

void sqlrcontroller_svr::translateBindVariableInStringAndMap(
						sqlrcursor_svr *cursor,
						stringbuffer *currentbind,
						uint16_t bindindex,
						stringbuffer *newquery) {

	const char	*bindformat=conn->bindFormat();
	size_t		bindformatlen=charstring::length(bindformat);

	// append the first character of the bind format to the new query
	newquery->append(bindformat[0]);

	if (bindformatlen==1) {

		// This section handles single-character bind variable
		// placeholder such as ?'s. (mysql, db2 and firebird format)

		// replace bind variable itself with number
		mapBindVariable(cursor,currentbind->getString(),bindindex);

	} else if (bindformat[1]=='1' &&
			!charstring::isNumber(currentbind->getString()+1)) {

		// This section handles 2-character placeholders where the
		// second position is a 1, such as $1 (postgresql-format).

		// replace bind variable in string with number
		newquery->append(bindindex);

		// replace bind variable itself with number
		mapBindVariable(cursor,currentbind->getString(),bindindex);

	} else {

		// This section handles everything else, such as :*, @*.
		// (oracle, sybase and ms sql server formats)

		// If the current bind variable was a single character
		// placeholder (such as a ?) then replace it with a delimited
		// number.  Otherwise use it as-is...

		if (currentbind->getStringLength()==1) {

			// replace bind variable in string with number
			newquery->append(bindindex);

			// replace bind variable itself with number
			mapBindVariable(cursor,
					currentbind->getString(),bindindex);
		} else {
			newquery->append(currentbind->getString()+1,
					currentbind->getStringLength()-1);
		}
	}
}

void sqlrcontroller_svr::mapBindVariable(sqlrcursor_svr *cursor,
						const char *bindvariable,
						uint16_t bindindex) {

	// if the current bind variable is a ? then just
	// set it NULL for special handling later
	if (!charstring::compare(bindvariable,"?")) {
		bindvariable=NULL;
	}

	// run two passes, first for input binds, second for output binds
	for (uint16_t i=0; i<2; i++) {

		// first pass for input binds, second pass for output binds
		uint16_t	count=(!i)?cursor->getInputBindCount():
						cursor->getOutputBindCount();
		bindvar_svr	*vars=(!i)?cursor->getInputBinds():
						cursor->getOutputBinds();
		namevaluepairs	*mappings=(!i)?inbindmappings:outbindmappings;

		for (uint16_t j=0; j<count; j++) {

			// get the bind var
			bindvar_svr	*b=&(vars[j]);

			// If a bind var name was passed in, look for a bind
			// variable with a matching name.
			// If no name was passed in then the bind vars are
			// numeric; get the variable who's numeric name matches
			// the index passed in.
			if ((bindvariable &&
				!charstring::compare(bindvariable,
							b->variable)) ||
				(!bindvariable &&
				charstring::toInteger((b->variable)+1)==
								bindindex)) {

				// create the new bind var
				// name and get its length
				char		*tempnumber=charstring::
							parseNumber(bindindex);
				uint16_t	tempnumberlen=charstring::
							length(tempnumber);

				// allocate memory for the new name
				char	*newvariable=
					(char *)bindmappingspool->
						allocate(tempnumberlen+2);

				// replace the existing bind var name and size
				newvariable[0]=conn->bindVariablePrefix();
				charstring::copy(newvariable+1,tempnumber);
				newvariable[tempnumberlen+1]='\0';

				// map existing name to new name
				mappings->setValue(b->variable,newvariable);
				
				// clean up
				delete[] tempnumber;
			}
		}
	}
}

void sqlrcontroller_svr::translateBindVariablesFromMappings(
						sqlrcursor_svr *cursor) {

	// index variable
	uint16_t	i=0;

	// debug and logging
	if (debugbindtranslation) {
		stdoutput.printf("remapping bind variables:\n");
		stdoutput.printf("  input binds:\n");
		for (i=0; i<cursor->getInputBindCount(); i++) {
			stdoutput.printf("    %s\n",
					cursor->getInputBinds()[i].variable);
		}
		stdoutput.printf("  output binds:\n");
		for (i=0; i<cursor->getOutputBindCount(); i++) {
			stdoutput.printf("    %s\n",
					cursor->getOutputBinds()[i].variable);
		}
		stdoutput.printf("\n");
	}
	if (logEnabled()) {
		logDebugMessage("remapping bind variables...");
		logDebugMessage("input binds:");
		for (i=0; i<cursor->getInputBindCount(); i++) {
			logDebugMessage(cursor->getInputBinds()[i].variable);
		}
		logDebugMessage("output binds:");
		for (i=0; i<cursor->getOutputBindCount(); i++) {
			logDebugMessage(cursor->getOutputBinds()[i].variable);
		}
	}

	// run two passes, first for input binds, second for output binds
	bool	remapped=false;
	for (i=0; i<2; i++) {

		// first pass for input binds, second pass for output binds
		uint16_t	count=(!i)?cursor->getInputBindCount():
						cursor->getOutputBindCount();
		bindvar_svr	*vars=(!i)?cursor->getInputBinds():
						cursor->getOutputBinds();
		namevaluepairs	*mappings=(!i)?inbindmappings:outbindmappings;

		for (uint16_t j=0; j<count; j++) {

			// get the bind var
			bindvar_svr	*b=&(vars[j]);

			// remap it
			char	*newvariable;
			if (mappings->getValue(b->variable,&newvariable)) {
				b->variable=newvariable;
				b->variablesize=charstring::length(b->variable);
				remapped=true;
			}
		}
	}

	// if no remapping was performed
	if (!remapped) {
		if (debugbindtranslation) {
			stdoutput.printf("  no variables remapped\n\n");
		}
		logDebugMessage("no variables remapped");
		return;
	}

	// debug and logging
	if (debugbindtranslation) {
		stdoutput.printf("  remapped input binds:\n");
		for (i=0; i<cursor->getInputBindCount(); i++) {
			stdoutput.printf("    %s\n",
					cursor->getInputBinds()[i].variable);
		}
		stdoutput.printf("  remapped output binds:\n");
		for (i=0; i<cursor->getOutputBindCount(); i++) {
			stdoutput.printf("    %s\n",
					cursor->getOutputBinds()[i].variable);
		}
		stdoutput.printf("\n");
	}
	if (logEnabled()) {
		logDebugMessage("remapped input binds:");
		for (i=0; i<cursor->getInputBindCount(); i++) {
			logDebugMessage(cursor->getInputBinds()[i].variable);
		}
		logDebugMessage("remapped output binds:");
		for (i=0; i<cursor->getOutputBindCount(); i++) {
			logDebugMessage(cursor->getOutputBinds()[i].variable);
		}
	}
}

void sqlrcontroller_svr::translateBeginTransaction(sqlrcursor_svr *cursor) {

	// get query buffer
	char	*querybuffer=cursor->getQueryBuffer();

	// debug
	logDebugMessage("translating begin tx query...");
	logDebugMessage("original:");
	logDebugMessage(querybuffer);

	// translate query
	const char	*beginquery=conn->beginTransactionQuery();
	uint32_t	querylength=charstring::length(beginquery);
	charstring::copy(querybuffer,beginquery,querylength);
	querybuffer[querylength]='\0';
	cursor->setQueryLength(querylength);

	// debug
	logDebugMessage("converted:");
	logDebugMessage(querybuffer);
}

void sqlrcontroller_svr::initNewQuery(sqlrcursor_svr *cursor) {

	// if we're using a custom cursor then close it
	sqlrcursor_svr	*customcursor=cursor->getCustomQueryCursor();
	if (customcursor) {
		customcursor->close();
		cursor->clearCustomQueryCursor();
	}

	// re-init fake bind flag
	cursor->setFakeInputBindsForThisQuery(fakeinputbinds);
}

sqlrcursor_svr	*sqlrcontroller_svr::initReExecuteQuery(
					sqlrcursor_svr *cursor) {

	// if we're using a custom cursor then operate on it
	sqlrcursor_svr	*customcursor=cursor->getCustomQueryCursor();
	if (customcursor) {
		return customcursor;
	}
	return cursor;
}

sqlrcursor_svr *sqlrcontroller_svr::getCustomQueryCursor(	
						sqlrcursor_svr *cursor) {

	// do we need to use a custom query cursor for this query?

	// bail right away if custom queries aren't enabled
	if (!sqlrq) {
		return cursor;
	}

	// see if the query matches one of the custom queries
	// FIXME: the 0 isn't safe, none of the custom queries do anything with
	// the id, but they might in the future so it needs to be unique
	cursor->setCustomQueryCursor(
			sqlrq->match(conn,
				cursor->getQueryBuffer(),
				cursor->getQueryLength(),
				0));
				
	// if so...
	if (cursor->getCustomQueryCursor()) {

		// open the cursor
		cursor->getCustomQueryCursor()->open();

		// copy the query that we just got into the custom query cursor
		charstring::copy(
			cursor->getCustomQueryCursor()->getQueryBuffer(),
			cursor->getQueryBuffer());
		cursor->getCustomQueryCursor()->setQueryLength(
						cursor->getQueryLength());

		// set the cursor state
		cursor->getCustomQueryCursor()->setState(SQLRCURSORSTATE_BUSY);

		// reset the cursor
		cursor=cursor->getCustomQueryCursor();

	}

	return cursor;
}

bool sqlrcontroller_svr::handleBinds(sqlrcursor_svr *cursor) {

	bindvar_svr	*bind=NULL;
	
	// iterate through the arrays, binding values to variables
	for (int16_t in=0; in<cursor->getInputBindCount(); in++) {

		bind=&cursor->getInputBinds()[in];

		// bind the value to the variable
		if (bind->type==STRING_BIND || bind->type==NULL_BIND) {
			if (!cursor->inputBind(
					bind->variable,
					bind->variablesize,
					bind->value.stringval,
					bind->valuesize,
					&bind->isnull)) {
				return false;
			}
		} else if (bind->type==INTEGER_BIND) {
			if (!cursor->inputBind(
					bind->variable,
					bind->variablesize,
					&bind->value.integerval)) {
				return false;
			}
		} else if (bind->type==DOUBLE_BIND) {
			if (!cursor->inputBind(
					bind->variable,
					bind->variablesize,
					&bind->value.doubleval.value,
					bind->value.doubleval.precision,
					bind->value.doubleval.scale)) {
				return false;
			}
		} else if (bind->type==DATE_BIND) {
			if (!cursor->inputBind(
					bind->variable,
					bind->variablesize,
					bind->value.dateval.year,
					bind->value.dateval.month,
					bind->value.dateval.day,
					bind->value.dateval.hour,
					bind->value.dateval.minute,
					bind->value.dateval.second,
					bind->value.dateval.microsecond,
					bind->value.dateval.tz,
					bind->value.dateval.buffer,
					bind->value.dateval.buffersize,
					&bind->isnull)) {
				return false;
			}
		} else if (bind->type==BLOB_BIND) {
			if (!cursor->inputBindBlob(
					bind->variable,
					bind->variablesize,
					bind->value.stringval,
					bind->valuesize,
					&bind->isnull)) {
				return false;
			}
		} else if (bind->type==CLOB_BIND) {
			if (!cursor->inputBindClob(
					bind->variable,
					bind->variablesize,
					bind->value.stringval,
					bind->valuesize,
					&bind->isnull)) {
				return false;
			}
		}
	}

	for (int16_t out=0; out<cursor->getOutputBindCount(); out++) {

		bind=&cursor->getOutputBinds()[out];

		// bind the value to the variable
		if (bind->type==STRING_BIND) {
			if (!cursor->outputBind(
					bind->variable,
					bind->variablesize,
					bind->value.stringval,
					bind->valuesize+1,
					&bind->isnull)) {
				return false;
			}
		} else if (bind->type==INTEGER_BIND) {
			if (!cursor->outputBind(
					bind->variable,
					bind->variablesize,
					&bind->value.integerval,
					&bind->isnull)) {
				return false;
			}
		} else if (bind->type==DOUBLE_BIND) {
			if (!cursor->outputBind(
					bind->variable,
					bind->variablesize,
					&bind->value.doubleval.value,
					&bind->value.doubleval.precision,
					&bind->value.doubleval.scale,
					&bind->isnull)) {
				return false;
			}
		} else if (bind->type==DATE_BIND) {
			if (!cursor->outputBind(
					bind->variable,
					bind->variablesize,
					&bind->value.dateval.year,
					&bind->value.dateval.month,
					&bind->value.dateval.day,
					&bind->value.dateval.hour,
					&bind->value.dateval.minute,
					&bind->value.dateval.second,
					&bind->value.dateval.microsecond,
					(const char **)&bind->value.dateval.tz,
					bind->value.dateval.buffer,
					bind->value.dateval.buffersize,
					&bind->isnull)) {
				return false;
			}
		} else if (bind->type==BLOB_BIND) {
			if (!cursor->outputBindBlob(
					bind->variable,
					bind->variablesize,out,
					&bind->isnull)) {
				return false;
			}
		} else if (bind->type==CLOB_BIND) {
			if (!cursor->outputBindClob(
					bind->variable,
					bind->variablesize,out,
					&bind->isnull)) {
				return false;
			}
		} else if (bind->type==CURSOR_BIND) {

			bool	found=false;

			// find the cursor that we acquired earlier...
			for (uint16_t j=0; j<cursorcount; j++) {

				if (cur[j]->getId()==bind->value.cursorid) {
					found=true;

					// bind the cursor
					if (!cursor->outputBindCursor(
							bind->variable,
							bind->variablesize,
							cur[j])) {
						return false;
					}
					break;
				}
			}

			// this shouldn't happen, but if it does, return false
			if (!found) {
				return false;
			}
		}
	}
	return true;
}

bool sqlrcontroller_svr::prepareQuery(sqlrcursor_svr *cursor,
						const char *query,
						uint32_t querylen) {
	return prepareQuery(cursor,query,querylen,false);
}

bool sqlrcontroller_svr::prepareQuery(sqlrcursor_svr *cursor,
						const char *query,
						uint32_t querylen,
						bool enabletranslations) {

	// clean up the previous result set
	closeResultSet(cursor);

	// re-init error data
	clearError(cursor);

	// reset some flags
	executedsinceprepare=false;
	querywasintercepted=false;
	bindswerefaked=false;

	// intercept some queries for special handling
	bool	success=interceptQuery(cursor,&querywasintercepted);
	if (querywasintercepted) {
		return success;
	}

	logDebugMessage("preparing query...");

	// set state
	updateState((isCustomQuery(cursor))?PROCESS_CUSTOM:PROCESS_SQL);

	// sanity check
	if (querylen>maxquerysize) {
		querylen=maxquerysize;
	}

	// copy query to cursor's query buffer if necessary
	if (query!=cursor->getQueryBuffer()) {
		charstring::copy(cursor->getQueryBuffer(),query,querylen);
		cursor->setQueryLength(querylen);
		cursor->getQueryBuffer()[cursor->getQueryLength()]='\0';
	}

	// translate query
	if (enabletranslations && sqlp && sqlrt && sqlw) {
		translateQuery(cursor);
	}

	// translate bind variables
	if (translatebinds) {
		translateBindVariables(cursor);
	}

	// translate "begin" queries
	if (conn->supportsTransactionBlocks() &&
			isBeginTransactionQuery(cursor)) {
		translateBeginTransaction(cursor);
	}

	// get the query
	query=cursor->getQueryBuffer();
	querylen=cursor->getQueryLength();

	// fake input binds if necessary
	querywithfakebinds.clear();
	if (!cursor->supportsNativeBinds(query) ||
		cursor->getFakeInputBindsForThisQuery()) {

		logDebugMessage("faking binds...");

		if (cursor->fakeInputBinds(&querywithfakebinds)) {
			// don't copy the rewritten query to the cursor's
			// query buffer, we need the original if we decide
			// to re-execute it later
			query=querywithfakebinds.getString();
			querylen=querywithfakebinds.getStringLength();
			if (debugsqlrtranslation) {
				stdoutput.printf(
				"after faking input binds:\n%s\n\n",query);
			}
			bindswerefaked=true;
		}
	}

	// prepare the query
	success=cursor->prepareQuery(query,querylen);

	// log result
	logDebugMessage((success)?"prepare query succeeded":
					"prepare query failed");
	logDebugMessage("done with prepare query");

	return success;
}

bool sqlrcontroller_svr::executeQuery(sqlrcursor_svr *cursor) {
	return executeQuery(cursor,false,false);
}

bool sqlrcontroller_svr::executeQuery(sqlrcursor_svr *cursor,
						bool enabletranslations,
						bool enabletriggers) {

	// if we're re-executing...
	if (executedsinceprepare) {

		// if we're faking binds then the original
		// query must be re-prepared
		if (!cursor->supportsNativeBinds() ||
			cursor->getFakeInputBindsForThisQuery()) {

			if (!prepareQuery(cursor,
					cursor->getQueryBuffer(),
					cursor->getQueryLength(),
					enabletranslations)) {
				return false;
			}

		} else {

			// these are done by prepare, but we need to do them
			// here if we're re-executing and not re-preparing...

			// clean up the previous result set
			closeResultSet(cursor);

			// re-init error data
			clearError(cursor);
		}
	}

	// if the query was intercepted during the
	// prepare then we don't need to do anything
	if (querywasintercepted) {
		return true;
	}

	logDebugMessage("executing query...");

	// set state
	updateState((isCustomQuery(cursor))?PROCESS_CUSTOM:PROCESS_SQL);

	// translate bind variables
	translateBindVariablesFromMappings(cursor);

	// handle binds (unless they were faked during the prepare)
	if (!bindswerefaked) {
		if (!handleBinds(cursor)) {
			return false;
		}
	}

	// handle before-triggers
	if (enabletriggers && sqlrtr) {
		sqlrtr->runBeforeTriggers(conn,cursor,cursor->getQueryTree());
	}

	// get the query start time
	datetime	dt;
	dt.getSystemDateAndTime();
	cursor->setQueryStart(dt.getSeconds(),dt.getMicroseconds());

	// get the query
	// if binds were faked, then make sure to send the query containing
	// the fake binds rather then the original query
	const char	*query;
	uint32_t	querylen;
	if (!bindswerefaked) {
		query=cursor->getQueryBuffer();
		querylen=cursor->getQueryLength();
	} else {
		query=querywithfakebinds.getString();
		querylen=querywithfakebinds.getStringLength();
	}

	// execute the query
	bool	success=cursor->executeQuery(query,querylen);

	// set flag indicating that query has been
	// executed since it was prepared
	executedsinceprepare=true;

	// get the query end time
	dt.getSystemDateAndTime();
	cursor->setQueryEnd(dt.getSeconds(),dt.getMicroseconds());

	// reset total rows fetched
	cursor->clearTotalRowsFetched();

	// update query and error counts
	incrementQueryCounts(cursor->queryType(query,querylen));
	if (!success) {
		incrementTotalErrors();
	}

	// handle after-triggers
	if (enabletriggers && sqlrtr) {
		sqlrtr->runAfterTriggers(conn,cursor,
					cursor->getQueryTree(),true);
	}

	// was the query a commit or rollback?
	commitOrRollback(cursor);

	// On success, autocommit if necessary.
	// Connection classes could override autoCommitOn() and autoCommitOff()
	// to do database API-specific things, but will not set 
	// fakeautocommit, so this code won't get called at all for those 
	// connections.
	// FIXME: when faking autocommit, a BEGIN on a db that supports them
	// could cause commit to be called immediately
	if (success && conn->isTransactional() &&
			needcommitorrollback &&
			conn->getFakeAutoCommit() &&
			conn->getAutoCommit()) {
		logDebugMessage("commit necessary...");
		success=commit();
	}

	// on failure get the error (unless it's already been set)
	if (!success && !cursor->getErrorNumber()) {
		uint32_t	errorlength;
		int64_t		errnum;
		bool		liveconnection;
		errorMessage(cursor,
				cursor->getErrorBuffer(),
				maxerrorlength,
				&errorlength,&errnum,&liveconnection);
		cursor->setErrorLength(errorlength);
		cursor->setErrorNumber(errnum);
		cursor->setLiveConnection(liveconnection);
	}
	
	logDebugMessage((success)?"executing query succeeded":
					"executing query failed");
	logDebugMessage("done executing query");

	return success;
}

void sqlrcontroller_svr::commitOrRollbackIsNeeded() {
	needcommitorrollback=true;
}

void sqlrcontroller_svr::commitOrRollbackIsNotNeeded() {
	needcommitorrollback=false;
}

void sqlrcontroller_svr::commitOrRollback(sqlrcursor_svr *cursor) {

	logDebugMessage("commit or rollback check...");

	// if the query was a commit or rollback, set a flag indicating so
	if (conn->isTransactional()) {
		// FIXME: if db has been put in the repeatable-read isolation
		// level then needcommitorrollback=true needs to be set no
		// matter what the query was
		if (cursor->queryIsCommitOrRollback()) {
			logDebugMessage("commit or rollback not needed");
			needcommitorrollback=false;
		} else if (cursor->queryIsNotSelect()) {
			logDebugMessage("commit or rollback needed");
			needcommitorrollback=true;
		}
	}

	logDebugMessage("done with commit or rollback check");
}

uint16_t sqlrcontroller_svr::getSendColumnInfo() {
	return sendcolumninfo;
}

void sqlrcontroller_svr::setSendColumnInfo(uint16_t sendcolumninfo) {
	this->sendcolumninfo=sendcolumninfo;
}

bool sqlrcontroller_svr::skipRows(sqlrcursor_svr *cursor, uint64_t rows) {

	if (sqlrlg) {
		debugstr.clear();
		debugstr.append("skipping ");
		debugstr.append(rows);
		debugstr.append(" rows...");
		logDebugMessage(debugstr.getString());
	}

	for (uint64_t i=0; i<rows; i++) {

		logDebugMessage("skip...");

		if (!cursor->skipRow()) {
			logDebugMessage("skipping rows hit the "
					"end of the result set");
			return false;
		}

		cursor->incrementTotalRowsFetched();
	}

	logDebugMessage("done skipping rows");
	return true;
}

void sqlrcontroller_svr::reformatField(sqlrcursor_svr *cursor,
						uint16_t index,
						const char *field,
						uint32_t fieldlength,
						const char **newfield,
						uint32_t *newfieldlength) {

	// initialize return values
	*newfield=field;
	*newfieldlength=fieldlength;

	// handle old-school date translation first
	if (reformatdatetimes) {
		bool		ddmm=cfgfl->getDateDdMm();
		bool		yyyyddmm=cfgfl->getDateYyyyDdMm();
		const char	*datetimeformat=cfgfl->getDateTimeFormat();
		const char	*dateformat=cfgfl->getDateFormat();
		const char	*timeformat=cfgfl->getTimeFormat();
		reformatDateTimes(cursor,index,
					field,fieldlength,
					newfield,newfieldlength,
					ddmm,yyyyddmm,
					datetimeformat,
					dateformat,timeformat);
	}

	// run translations
	if (sqlrrst) {
		sqlrrst->runResultSetTranslations(conn,cursor,
							index,
							field,
							fieldlength,
							newfield,
							newfieldlength);
	}
}

void sqlrcontroller_svr::reformatDateTimes(sqlrcursor_svr *cursor,
						uint16_t index,
						const char *field,
						uint32_t fieldlength,
						const char **newfield,
						uint32_t *newfieldlength,
						bool ddmm, bool yyyyddmm,
						const char *datetimeformat,
						const char *dateformat,
						const char *timeformat) {

	// This weirdness is mainly to address a FreeTDS/MSSQL
	// issue.  See the code for the method
	// freetdscursor::ignoreDateDdMmParameter() for more info.
	if (cursor->ignoreDateDdMmParameter(index,field,fieldlength)) {
		ddmm=false;
		yyyyddmm=false;
	}

	int16_t	year=-1;
	int16_t	month=-1;
	int16_t	day=-1;
	int16_t	hour=-1;
	int16_t	minute=-1;
	int16_t	second=-1;
	int16_t	fraction=-1;
	if (!parseDateTime(field,ddmm,yyyyddmm,true,
				&year,&month,&day,
				&hour,&minute,&second,
				&fraction)) {
		return;
	}

	// decide which format to use based on what parts
	// were detected in the date/time
	const char	*format=datetimeformat;
	if (hour==-1) {
		format=dateformat;
	} else if (day==-1) {
		format=timeformat;
	}

	// convert to the specified format
	delete[] reformattedfield;
	reformattedfield=convertDateTime(format,
					year,month,day,
					hour,minute,second,
					fraction);
	reformattedfieldlength=charstring::length(reformattedfield);

	if (debugsqlrtranslation) {
		stdoutput.printf("converted date: "
			"\"%s\" to \"%s\" using ddmm=%d\n",
			field,reformattedfield,ddmm);
	}

	// set return values
	*newfield=reformattedfield;
	*newfieldlength=reformattedfieldlength;
}

void sqlrcontroller_svr::closeAllResultSets() {
	logDebugMessage("closing result sets for all cursors...");
	for (int32_t i=0; i<cursorcount; i++) {
		if (cur[i]) {
			cur[i]->closeResultSet();
		}
	}
	logDebugMessage("done closing result sets for all cursors...");
}

void sqlrcontroller_svr::endSession() {

	logDebugMessage("ending session...");

	updateState(SESSION_END);

	logDebugMessage("aborting all cursors...");
	for (int32_t i=0; i<cursorcount; i++) {
		if (cur[i]) {
			cur[i]->abort();
		}
	}
	logDebugMessage("done aborting all cursors");

	// must set suspendedsession to false here so resumed sessions won't 
	// automatically re-suspend
	suspendedsession=false;

	// Run end-of-session rollback or commit before dropping tables and
	// running session-end-queries.  Some queries, including drop table,
	// cause an implicit commit.  If we need to rollback, then make sure
	// that's done first.
	if (intransactionblock) {

		// if we're faking transaction blocks and the session was ended
		// but we haven't ended the transaction block, then we need to
		// rollback and end the block
		rollback();
		intransactionblock=false;

	} else if (conn->isTransactional() && needcommitorrollback) {

		// otherwise, commit or rollback as necessary
		if (cfgfl->getEndOfSessionCommit()) {
			logDebugMessage("committing...");
			commit();
			logDebugMessage("done committing...");
		} else {
			logDebugMessage("rolling back...");
			rollback();
			logDebugMessage("done rolling back...");
		}
	}

	// truncate/drop temp tables
	// (Do this before running the end-session queries becuase
	// with oracle, it may be necessary to log out and log back in to
	// drop a temp table.  With each log-out the session end queries
	// are run and with each log-in the session start queries are run.)
	truncateTempTables(cur[0]);
	dropTempTables(cur[0]);

	// run session-end queries
	sessionEndQueries();

	// reset database/schema
	if (dbchanged) {
		// FIXME: we're ignoring the result and error,
		// should we do something if there's an error?
		conn->selectDatabase(originaldb);
		dbchanged=false;
	}

	// reset autocommit behavior
	setAutoCommit(conn->getAutoCommit());

	// set isolation level
	conn->setIsolationLevel(isolationlevel);

	// reset sql translation
	if (sqlrt) {
		sqlrt->endSession();
	}

	// shrink the cursor array, if necessary
	// FIXME: it would probably be more efficient to scale
	// these down gradually rather than all at once
	while (cursorcount>mincursorcount) {
		cursorcount--;
		cur[cursorcount]->close();
		deleteCursor(cur[cursorcount]);
		cur[cursorcount]=NULL;
	}

	// end the session
	conn->endSession();

	// if the db is behind a load balancer, re-login
	// periodically to redistribute connections
	if (constr->getBehindLoadBalancer()) {
		logDebugMessage("relogging in to "
				"redistribute connections");
		datetime	dt;
		if (dt.getSystemDateAndTime()) {
			if (dt.getEpoch()>=relogintime) {
				reLogIn();
			}
		}
		logDebugMessage("done relogging in to "
				"redistribute connections");
	}

	logDebugMessage("done ending session");
}

void sqlrcontroller_svr::dropTempTables(sqlrcursor_svr *cursor) {

	// some databases require us to re-login before dropping temp tables
	if (sessiontemptablesfordrop.getLength() &&
			conn->tempTableDropReLogIn()) {
		reLogIn();
	}

	// run through the temp table list, dropping tables
	for (singlylinkedlistnode< char * >
				*sln=sessiontemptablesfordrop.getFirst();
						sln; sln=sln->getNext()) {
		dropTempTable(cursor,sln->getValue());
		delete[] sln->getValue();
	}
	sessiontemptablesfordrop.clear();
}

void sqlrcontroller_svr::dropTempTable(sqlrcursor_svr *cursor,
					const char *tablename) {

	stringbuffer	dropquery;
	dropquery.append("drop table ");
	dropquery.append(conn->tempTableDropPrefix());
	dropquery.append(tablename);

	// FIXME: I need to refactor all of this so that this just gets
	// run as a matter of course instead of explicitly getting run here
	// FIXME: freetds/sybase override this method but don't do this
	if (sqlrtr) {
		if (sqlp->parse(dropquery.getString())) {
			sqlrtr->runBeforeTriggers(conn,cursor,sqlp->getTree());
		}
	}

	// kind of a kluge...
	// The cursor might already have a querytree associated with it and
	// if it does then executeQuery below might cause some triggers to
	// be run on that tree rather than on the tree for the drop query
	// we intend to run.
	cursor->clearQueryTree();

	if (prepareQuery(cursor,dropquery.getString(),
					dropquery.getStringLength())) {
		executeQuery(cursor);
	}
	cursor->closeResultSet();

	// FIXME: I need to refactor all of this so that this just gets
	// run as a matter of course instead of explicitly getting run here
	// FIXME: freetds/sybase override this method but don't do this
	if (sqlrtr) {
		sqlrtr->runAfterTriggers(conn,cursor,sqlp->getTree(),true);
	}
}

void sqlrcontroller_svr::truncateTempTables(sqlrcursor_svr *cursor) {

	// run through the temp table list, truncating tables
	for (singlylinkedlistnode< char * >
			*sln=sessiontemptablesfortrunc.getFirst();
						sln; sln=sln->getNext()) {
		truncateTempTable(cursor,sln->getValue());
		delete[] sln->getValue();
	}
	sessiontemptablesfortrunc.clear();
}

void sqlrcontroller_svr::truncateTempTable(sqlrcursor_svr *cursor,
						const char *tablename) {
	stringbuffer	truncatequery;
	truncatequery.append("delete from ")->append(tablename);
	if (prepareQuery(cursor,truncatequery.getString(),
					truncatequery.getStringLength())) {
		executeQuery(cursor);
	}
	cursor->closeResultSet();
}

void sqlrcontroller_svr::closeClientConnection(uint32_t bytes) {

	// Sometimes the server sends the result set and closes the socket
	// while part of it is buffered but not yet transmitted.  This causes
	// the client to receive a partial result set or error.  Telling the
	// socket to linger doesn't always fix it.  Doing a read here should 
	// guarantee that the client will close its end of the connection 
	// before the server closes its end; the server will wait for data 
	// from the client (which it will never receive) and when the client 
	// closes its end (which it will only do after receiving the entire
	// result set) the read will fall through.  This should guarantee 
	// that the client will get the the entire result set without
	// requiring the client to send data back indicating so.
	//
	// Also, if authentication fails, the client could send an entire query
	// and bind vars before it reads the error and closes the socket.
	// We have to absorb all of that data.  We shouldn't just loop forever
	// though, that would provide a point of entry for a DOS attack.  We'll
	// read the maximum number of bytes that could be sent.
	logDebugMessage("waiting for client to close the connection...");
	uint16_t	dummy;
	uint32_t	counter=0;
	clientsock->useNonBlockingMode();
	while (clientsock->read(&dummy,idleclienttimeout,0)>0 &&
							counter<bytes) {
		counter++;
	}
	clientsock->useBlockingMode();
	
	logDebugMessage("done waiting for client to close the connection");

	// close the client socket
	logDebugMessage("closing the client socket...");
	clientsock->close();
	delete clientsock;
	logDebugMessage("done closing the client socket");

	// in proxy mode, the client socket is pointed at the handoff
	// socket which now needs to be reestablished
	if (proxymode) {
		registerForHandoff();
	}
}

void sqlrcontroller_svr::closeSuspendedSessionSockets() {

	if (suspendedsession) {
		return;
	}

	// If we're no longer in a suspended session but had to open a set of
	// sockets to handle a suspended session, close those sockets here.
	if (serversockun || serversockin) {
		logDebugMessage("closing sockets from "
				"a previously suspended session...");
	}
	if (serversockun) {
		removeFileDescriptor(serversockun);
		delete serversockun;
		serversockun=NULL;
	}
	if (serversockin) {
		for (uint64_t index=0;
				index<serversockincount;
				index++) {
			removeFileDescriptor(serversockin[index]);
			delete serversockin[index];
			serversockin[index]=NULL;
		}
		delete[] serversockin;
		serversockin=NULL;
		serversockincount=0;
	}
	if (serversockun || serversockin) {
		logDebugMessage("done closing sockets from "
				"a previously suspended session...");
	}
}

void sqlrcontroller_svr::shutDown() {

	logDebugMessage("closing connection...");

	if (inclientsession) {
		endSession();
		decrementOpenClientConnections();
		inclientsession=false;
	}

	// decrement the connection counter
	if (decrementonclose && cfgfl->getDynamicScaling() &&
						semset && shmem) {
		decrementConnectionCount();
	}

	// deregister and close the handoff socket if necessary
	if (connected) {
		deRegisterForHandoff();
	}

	// close the cursors
	closeCursors(true);

	// try to log out
	logOut();

	// clear the pool
	removeAllFileDescriptors();

	// close, clean up all sockets
	delete serversockun;

	for (uint64_t index=0; index<serversockincount; index++) {
		delete serversockin[index];
	}
	delete[] serversockin;

	logDebugMessage("done closing connection");
}

void sqlrcontroller_svr::closeCursors(bool destroy) {

	logDebugMessage("closing cursors...");

	if (cur) {
		while (cursorcount) {
			cursorcount--;

			if (cur[cursorcount]) {
				cur[cursorcount]->closeResultSet();
				cur[cursorcount]->close();
				if (destroy) {
					deleteCursor(cur[cursorcount]);
					cur[cursorcount]=NULL;
				}
			}
		}
		if (destroy) {
			delete[] cur;
			cur=NULL;
		}
	}

	logDebugMessage("done closing cursors...");
}

void sqlrcontroller_svr::deleteCursor(sqlrcursor_svr *curs) {
	conn->deleteCursor(curs);
	decrementOpenDatabaseCursors();
}

bool sqlrcontroller_svr::createSharedMemoryAndSemaphores(const char *id) {

	size_t	idfilenamelen=tmpdir->getLength()+5+
					charstring::length(id)+1;
	char	*idfilename=new char[idfilenamelen];
	charstring::printf(idfilename,idfilenamelen,"%s/ipc/%s",
						tmpdir->getString(),id);

	debugstr.clear();
	debugstr.append("attaching to shared memory and semaphores ");
	debugstr.append("id filename: ")->append(idfilename);
	logDebugMessage(debugstr.getString());

	// connect to the shared memory
	logDebugMessage("attaching to shared memory...");
	shmem=new sharedmemory();
	if (!shmem->attach(file::generateKey(idfilename,1),
						sizeof(shmdata))) {
		char	*err=error::getErrorString();
		stderror.printf("Couldn't attach to shared memory segment: ");
		stderror.printf("%s\n",err);
		delete[] err;
		delete shmem;
		shmem=NULL;
		delete[] idfilename;
		return false;
	}
	shm=(shmdata *)shmem->getPointer();
	if (!shm) {
		stderror.printf("failed to get pointer to shmdata\n");
		delete shmem;
		shmem=NULL;
		delete[] idfilename;
		return false;
	}

	// connect to the semaphore set
	logDebugMessage("attaching to semaphores...");
	semset=new semaphoreset();
	if (!semset->attach(file::generateKey(idfilename,1),11)) {
		char	*err=error::getErrorString();
		stderror.printf("Couldn't attach to semaphore set: ");
		stderror.printf("%s\n",err);
		delete[] err;
		delete semset;
		delete shmem;
		semset=NULL;
		shmem=NULL;
		delete[] idfilename;
		return false;
	}

	logDebugMessage("done attaching to shared memory and semaphores");

	delete[] idfilename;

	return true;
}

shmdata *sqlrcontroller_svr::getAnnounceBuffer() {
	return (shmdata *)shmem->getPointer();
}

void sqlrcontroller_svr::decrementConnectedClientCount() {

	logDebugMessage("decrementing session count...");

	if (!semset->waitWithUndo(5)) {
		// FIXME: bail somehow
	}

	// increment the connections-in-use count
	if (shm->connectedclients) {
		shm->connectedclients--;
	}

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

	if (!semset->signalWithUndo(5)) {
		// FIXME: bail somehow
	}

	logDebugMessage("done decrementing session count");
}

bool sqlrcontroller_svr::acquireAnnounceMutex() {

	logDebugMessage("acquiring announce mutex");

	updateState(WAIT_SEMAPHORE);

	// FIXME: It's possible that the alarm could ring prior to the wait
	// below and not interrupt it.  If that happens, the ttl would be
	// ignored.

	// Loop, waiting.  Retry the wait if it was interrupted by a signal
	// other than an alarm, but bail if an alarm interrupted it.
	semset->dontRetryInterruptedOperations();
	bool	result=true;
	do {
		result=semset->waitWithUndo(0);
	} while (!result && error::getErrorNumber()==EINTR && alarmrang!=1);
	semset->retryInterruptedOperations();

	// handle alarm...
	if (alarmrang) {
		logDebugMessage("ttl reached, aborting "
				"acquiring announce mutex");
		return false;
	}

	logDebugMessage("done acquiring announce mutex");
	return true;
}

void sqlrcontroller_svr::releaseAnnounceMutex() {
	logDebugMessage("releasing announce mutex");
	semset->signalWithUndo(0);
	logDebugMessage("done releasing announce mutex");
}

void sqlrcontroller_svr::signalListenerToRead() {
	logDebugMessage("signalling listener to read");
	semset->signalWithUndo(2);
	logDebugMessage("done signalling listener to read");
}

bool sqlrcontroller_svr::waitForListenerToFinishReading() {

	logDebugMessage("waiting for listener");

	// It's possible that the alarm could ring prior to the wait below and
	// not interrupt it.  If that happens, the ttl would be ignored.  This
	// is highly unlikely as the minimum ttl is 1 second, but I guess if a
	// machine was super, super busy, then it might happen.

	// Loop, waiting.  Retry the wait if it was interrupted by a signal
	// other than an alarm, but bail if an alarm interrupted it.
	semset->dontRetryInterruptedOperations();
	bool	result=true;
	do {
		result=semset->wait(3);
	} while (!result && error::getErrorNumber()==EINTR && alarmrang!=1);
	semset->retryInterruptedOperations();

	// Reset this semaphore to 0.
	// It can get left incremented if another sqlr-connection is killed
	// between calls to signalListenerToRead() and this method.
	// It's ok to reset it here becuase no one except this process has
	// access to this semaphore at this time because of the lock on
	// AnnounceMutex (semaphore 0).
	semset->setValue(3,0);

	// handle alarm...
	if (alarmrang) {
		logDebugMessage("ttl reached, aborting waiting for listener");
		return false;
	}

	logDebugMessage("done waiting for listener");
	return true;
}

void sqlrcontroller_svr::acquireConnectionCountMutex() {
	logDebugMessage("acquiring connection count mutex");
	semset->waitWithUndo(4);
	logDebugMessage("done acquiring connection count mutex");
}

void sqlrcontroller_svr::releaseConnectionCountMutex() {
	logDebugMessage("releasing connection count mutex");
	semset->signalWithUndo(4);
	logDebugMessage("done releasing connection count mutex");
}

void sqlrcontroller_svr::signalScalerToRead() {
	logDebugMessage("signalling scaler to read");
	semset->signal(8);
	logDebugMessage("done signalling scaler to read");
}

void sqlrcontroller_svr::initConnStats() {

	semset->waitWithUndo(9);

	// Find an available location in the connstats array.
	// It shouldn't be possible for sqlr-start or sqlr-scaler to start
	// more than MAXCONNECTIONS, so unless someone started one manually,
	// it should always be possible to find an open one.
	for (uint32_t i=0; i<MAXCONNECTIONS; i++) {
		connstats=&shm->connstats[i];
		if (!connstats->processid) {

			semset->signalWithUndo(9);

			// initialize the connection stats
			clearConnStats();
			updateState(INIT);
			connstats->index=i;
			connstats->processid=process::getProcessId();
			connstats->loggedinsec=loggedinsec;
			connstats->loggedinusec=loggedinusec;
			return;
		}
	}

	semset->signalWithUndo(9);

	// in case someone started a connection manually and
	// exceeded MAXCONNECTIONS, set this NULL here
	connstats=NULL;
}

void sqlrcontroller_svr::clearConnStats() {
	if (!connstats) {
		return;
	}
	bytestring::zero(connstats,sizeof(struct sqlrconnstatistics));
}

void sqlrcontroller_svr::updateState(enum sqlrconnectionstate_t state) {
	if (!connstats) {
		return;
	}
	connstats->state=state;
	datetime	dt;
	dt.getSystemDateAndTime();
	connstats->statestartsec=dt.getSeconds();
	connstats->statestartusec=dt.getMicroseconds();
}

void sqlrcontroller_svr::updateClientSessionStartTime() {
	if (!connstats) {
		return;
	}
	datetime	dt;
	dt.getSystemDateAndTime();
	connstats->clientsessionsec=dt.getSeconds();
	connstats->clientsessionusec=dt.getMicroseconds();
}

void sqlrcontroller_svr::updateCurrentQuery(const char *query,
						uint32_t querylen) {
	if (!connstats) {
		return;
	}
	uint32_t	len=querylen;
	if (len>STATSQLTEXTLEN-1) {
		len=STATSQLTEXTLEN-1;
	}
	charstring::copy(connstats->sqltext,query,len);
	connstats->sqltext[len]='\0';
}

void sqlrcontroller_svr::updateClientInfo(const char *info, uint32_t infolen) {
	if (!connstats) {
		return;
	}
	uint64_t	len=infolen;
	if (len>STATCLIENTINFOLEN-1) {
		len=STATCLIENTINFOLEN-1;
	}
	charstring::copy(connstats->clientinfo,info,len);
	connstats->clientinfo[len]='\0';
}

void sqlrcontroller_svr::updateClientAddr() {
	if (!connstats) {
		return;
	}
	if (clientsock) {
		char	*clientaddrbuf=clientsock->getPeerAddress();
		if (clientaddrbuf) {
			charstring::copy(connstats->clientaddr,clientaddrbuf);
			delete[] clientaddrbuf;
		} else {
			charstring::copy(connstats->clientaddr,"UNIX");
		}
	} else {
		charstring::copy(connstats->clientaddr,"internal");
	}
}

void sqlrcontroller_svr::incrementOpenDatabaseConnections() {
	semset->waitWithUndo(9);
	shm->open_db_connections++;
	shm->opened_db_connections++;
	semset->signalWithUndo(9);
}

void sqlrcontroller_svr::decrementOpenDatabaseConnections() {
	semset->waitWithUndo(9);
	if (shm->open_db_connections) {
		shm->open_db_connections--;
	}
	semset->signalWithUndo(9);
}

void sqlrcontroller_svr::incrementOpenClientConnections() {
	semset->waitWithUndo(9);
	shm->open_cli_connections++;
	shm->opened_cli_connections++;
	semset->signalWithUndo(9);
	if (!connstats) {
		return;
	}
	connstats->nconnect++;
}

void sqlrcontroller_svr::decrementOpenClientConnections() {
	semset->waitWithUndo(9);
	if (shm->open_cli_connections) {
		shm->open_cli_connections--;
	}
	semset->signalWithUndo(9);
}

void sqlrcontroller_svr::incrementOpenDatabaseCursors() {
	semset->waitWithUndo(9);
	shm->open_db_cursors++;
	shm->opened_db_cursors++;
	semset->signalWithUndo(9);
}

void sqlrcontroller_svr::decrementOpenDatabaseCursors() {
	semset->waitWithUndo(9);
	if (shm->open_db_cursors) {
		shm->open_db_cursors--;
	}
	semset->signalWithUndo(9);
}

void sqlrcontroller_svr::incrementTimesNewCursorUsed() {
	semset->waitWithUndo(9);
	shm->times_new_cursor_used++;
	semset->signalWithUndo(9);
}

void sqlrcontroller_svr::incrementTimesCursorReused() {
	semset->waitWithUndo(9);
	shm->times_cursor_reused++;
	semset->signalWithUndo(9);
}

void sqlrcontroller_svr::incrementQueryCounts(sqlrquerytype_t querytype) {

	semset->waitWithUndo(9);

	// update total queries
	shm->total_queries++;

	// update queries-per-second stats...

	// re-init stats if necessary
	datetime	dt;
	dt.getSystemDateAndTime();
	time_t	now=dt.getEpoch();
	int	index=now%STATQPSKEEP;
	if (shm->timestamp[index]!=now) {
		shm->timestamp[index]=now;
		shm->qps_select[index]=0;
		shm->qps_update[index]=0;
		shm->qps_insert[index]=0;
		shm->qps_delete[index]=0;
		shm->qps_create[index]=0;
		shm->qps_drop[index]=0;
		shm->qps_alter[index]=0;
		shm->qps_custom[index]=0;
		shm->qps_etc[index]=0;
	}

	// increment per-query-type stats
	switch (querytype) {
		case SQLRQUERYTYPE_SELECT:
			shm->qps_select[index]++;
			break;
		case SQLRQUERYTYPE_INSERT:
			shm->qps_insert[index]++;
			break;
		case SQLRQUERYTYPE_UPDATE:
			shm->qps_update[index]++;
			break;
		case SQLRQUERYTYPE_DELETE:
			shm->qps_delete[index]++;
			break;
		case SQLRQUERYTYPE_CREATE:
			shm->qps_create[index]++;
			break;
		case SQLRQUERYTYPE_DROP:
			shm->qps_drop[index]++;
			break;
		case SQLRQUERYTYPE_ALTER:
			shm->qps_alter[index]++;
			break;
		case SQLRQUERYTYPE_CUSTOM:
			shm->qps_custom[index]++;
			break;
		case SQLRQUERYTYPE_ETC:
		default:
			shm->qps_etc[index]++;
			break;
	}

	semset->signalWithUndo(9);

	if (!connstats) {
		return;
	}
	if (querytype==SQLRQUERYTYPE_CUSTOM) {
		connstats->ncustomsql++;
	} else {
		connstats->nsql++;
	}
}

void sqlrcontroller_svr::incrementTotalErrors() {
	semset->waitWithUndo(9);
	shm->total_errors++;
	semset->signalWithUndo(9);
}

void sqlrcontroller_svr::incrementAuthenticateCount() {
	if (!connstats) {
		return;
	}
	connstats->nauthenticate++;
}

void sqlrcontroller_svr::incrementSuspendSessionCount() {
	if (!connstats) {
		return;
	}
	connstats->nsuspend_session++;
}

void sqlrcontroller_svr::incrementEndSessionCount() {
	if (!connstats) {
		return;
	}
	connstats->nend_session++;
}

void sqlrcontroller_svr::incrementPingCount() {
	if (!connstats) {
		return;
	}
	connstats->nping++;
}

void sqlrcontroller_svr::incrementIdentifyCount() {
	if (!connstats) {
		return;
	}
	connstats->nidentify++;
}

void sqlrcontroller_svr::incrementAutocommitCount() {
	if (!connstats) {
		return;
	}
	connstats->nautocommit++;
}

void sqlrcontroller_svr::incrementBeginCount() {
	if (!connstats) {
		return;
	}
	connstats->nbegin++;
}

void sqlrcontroller_svr::incrementCommitCount() {
	if (!connstats) {
		return;
	}
	connstats->ncommit++;
}

void sqlrcontroller_svr::incrementRollbackCount() {
	if (!connstats) {
		return;
	}
	connstats->nrollback++;
}

void sqlrcontroller_svr::incrementDbVersionCount() {
	if (!connstats) {
		return;
	}
	connstats->ndbversion++;
}

void sqlrcontroller_svr::incrementBindFormatCount() {
	if (!connstats) {
		return;
	}
	connstats->nbindformat++;
}

void sqlrcontroller_svr::incrementServerVersionCount() {
	if (!connstats) {
		return;
	}
	connstats->nserverversion++;
}

void sqlrcontroller_svr::incrementSelectDatabaseCount() {
	if (!connstats) {
		return;
	}
	connstats->nselectdatabase++;
}

void sqlrcontroller_svr::incrementGetCurrentDatabaseCount() {
	if (!connstats) {
		return;
	}
	connstats->ngetcurrentdatabase++;
}

void sqlrcontroller_svr::incrementGetLastInsertIdCount() {
	if (!connstats) {
		return;
	}
	connstats->ngetlastinsertid++;
}

void sqlrcontroller_svr::incrementDbHostNameCount() {
	if (!connstats) {
		return;
	}
	connstats->ndbhostname++;
}

void sqlrcontroller_svr::incrementDbIpAddressCount() {
	if (!connstats) {
		return;
	}
	connstats->ndbipaddress++;
}

void sqlrcontroller_svr::incrementNewQueryCount() {
	if (!connstats) {
		return;
	}
	connstats->nnewquery++;
}

void sqlrcontroller_svr::incrementReexecuteQueryCount() {
	if (!connstats) {
		return;
	}
	connstats->nreexecutequery++;
}

void sqlrcontroller_svr::incrementFetchFromBindCursorCount() {
	if (!connstats) {
		return;
	}
	connstats->nfetchfrombindcursor++;
}

void sqlrcontroller_svr::incrementFetchResultSetCount() {
	if (!connstats) {
		return;
	}
	connstats->nfetchresultset++;
}

void sqlrcontroller_svr::incrementAbortResultSetCount() {
	if (!connstats) {
		return;
	}
	connstats->nabortresultset++;
}

void sqlrcontroller_svr::incrementSuspendResultSetCount() {
	if (!connstats) {
		return;
	}
	connstats->nsuspendresultset++;
}

void sqlrcontroller_svr::incrementResumeResultSetCount() {
	if (!connstats) {
		return;
	}
	connstats->nresumeresultset++;
}

void sqlrcontroller_svr::incrementGetDbListCount() {
	if (!connstats) {
		return;
	}
	connstats->ngetdblist++;
}

void sqlrcontroller_svr::incrementGetTableListCount() {
	if (!connstats) {
		return;
	}
	connstats->ngettablelist++;
}

void sqlrcontroller_svr::incrementGetColumnListCount() {
	if (!connstats) {
		return;
	}
	connstats->ngetcolumnlist++;
}

void sqlrcontroller_svr::incrementGetQueryTreeCount() {
	if (!connstats) {
		return;
	}
	connstats->ngetquerytree++;
}

void sqlrcontroller_svr::incrementReLogInCount() {
	if (!connstats) {
		return;
	}
	connstats->nrelogin++;
}

void sqlrcontroller_svr::sessionStartQueries() {
	// run a configurable set of queries at the start of each session
	for (linkedlistnode< char * > *node=
		cfgfl->getSessionStartQueries()->getFirst();
					node; node=node->getNext()) {
		sessionQuery(node->getValue());
	}
}

void sqlrcontroller_svr::sessionEndQueries() {
	// run a configurable set of queries at the end of each session
	for (linkedlistnode< char * > *node=
		cfgfl->getSessionEndQueries()->getFirst();
					node; node=node->getNext()) {
		sessionQuery(node->getValue());
	}
}

void sqlrcontroller_svr::sessionQuery(const char *query) {

	// create the select database query
	size_t	querylen=charstring::length(query);

	sqlrcursor_svr	*cur=newCursor();
	if (open(cur) &&
		prepareQuery(cur,query,querylen) && executeQuery(cur)) {
		closeResultSet(cur);
	}
	close(cur);
	deleteCursor(cur);
}

const char *sqlrcontroller_svr::getConnectStringValue(const char *variable) {

	// If we're using password encryption and the password is requested,
	// and the password encryption module supports two-way encryption,
	// then return the decrypted version of the password, otherwise just
	// return the value as-is.
	const char	*peid=constr->getPasswordEncryption();
	if (sqlrpe && charstring::length(peid) &&
			!charstring::compare(variable,"password")) {
		sqlrpwdenc	*pe=sqlrpe->getPasswordEncryptionById(peid);
		if (!pe->oneWay()) {
			delete[] decrypteddbpassword;
			decrypteddbpassword=pe->decrypt(
				constr->getConnectStringValue(variable));
			return decrypteddbpassword;
		}
	}
	return constr->getConnectStringValue(variable);
}

void sqlrcontroller_svr::setUser(const char *user) {
	this->user=user;
}

void sqlrcontroller_svr::setPassword(const char *password) {
	this->password=password;
}

const char *sqlrcontroller_svr::getUser() {
	return user;
}

const char *sqlrcontroller_svr::getPassword() {
	return password;
}
void sqlrcontroller_svr::setAutoCommitBehavior(bool ac) {
	conn->setAutoCommit(ac);
}

void sqlrcontroller_svr::setFakeTransactionBlocksBehavior(bool ftb) {
	faketransactionblocks=ftb;
}

const char *sqlrcontroller_svr::bindFormat() {
	return conn->bindFormat();
}

int16_t sqlrcontroller_svr::nonNullBindValue() {
	return conn->nonNullBindValue();
}

int16_t sqlrcontroller_svr::nullBindValue() {
	return conn->nullBindValue();
}

char sqlrcontroller_svr::bindVariablePrefix() {
	return conn->bindVariablePrefix();
}

bool sqlrcontroller_svr::bindValueIsNull(int16_t isnull) {
	return conn->bindValueIsNull(isnull);
}

void sqlrcontroller_svr::fakeInputBinds() {
	fakeinputbinds=true;
}

bool sqlrcontroller_svr::getColumnNames(const char *query,
					stringbuffer *output) {

	// sanity check on the query
	if (!query) {
		return false;
	}

	size_t		querylen=charstring::length(query);

	bool	retval=false;
	sqlrcursor_svr	*gcncur=newCursor();
	if (open(gcncur) &&
		prepareQuery(gcncur,query,querylen) && executeQuery(gcncur)) {

		// build column list...
		retval=gcncur->getColumnNameList(output);
	}
	closeResultSet(gcncur);
	close(gcncur);
	deleteCursor(gcncur);
	return retval;
}

void sqlrcontroller_svr::addSessionTempTableForDrop(const char *table) {
	sessiontemptablesfordrop.append(charstring::duplicate(table));
}

void sqlrcontroller_svr::addTransactionTempTableForDrop(const char *table) {
	transtemptablesfordrop.append(charstring::duplicate(table));
}

void sqlrcontroller_svr::addSessionTempTableForTrunc(const char *table) {
	sessiontemptablesfortrunc.append(charstring::duplicate(table));
}

void sqlrcontroller_svr::addTransactionTempTableForTrunc(const char *table) {
	transtemptablesfortrunc.append(charstring::duplicate(table));
}

bool sqlrcontroller_svr::logEnabled() {
	return (sqlrlg!=NULL);
}

void sqlrcontroller_svr::logDebugMessage(const char *info) {
	if (!sqlrlg) {
		return;
	}
	sqlrlg->runLoggers(NULL,conn,NULL,
			SQLRLOGGER_LOGLEVEL_DEBUG,
			SQLRLOGGER_EVENTTYPE_DEBUG_MESSAGE,
			info);
}

void sqlrcontroller_svr::logClientConnected() {
	if (!sqlrlg) {
		return;
	}
	sqlrlg->runLoggers(NULL,conn,NULL,
			SQLRLOGGER_LOGLEVEL_INFO,
			SQLRLOGGER_EVENTTYPE_CLIENT_CONNECTED,
			NULL);
}

void sqlrcontroller_svr::logClientConnectionRefused(const char *info) {
	if (!sqlrlg) {
		return;
	}
	sqlrlg->runLoggers(NULL,conn,NULL,
			SQLRLOGGER_LOGLEVEL_WARNING,
			SQLRLOGGER_EVENTTYPE_CLIENT_CONNECTION_REFUSED,
			info);
}

void sqlrcontroller_svr::logClientDisconnected(const char *info) {
	if (!sqlrlg) {
		return;
	}
	sqlrlg->runLoggers(NULL,conn,NULL,
			SQLRLOGGER_LOGLEVEL_INFO,
			SQLRLOGGER_EVENTTYPE_CLIENT_DISCONNECTED,
			info);
}

void sqlrcontroller_svr::logClientProtocolError(sqlrcursor_svr *cursor,
							const char *info,
							ssize_t result) {
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
	sqlrlg->runLoggers(NULL,conn,cursor,
			SQLRLOGGER_LOGLEVEL_ERROR,
			SQLRLOGGER_EVENTTYPE_CLIENT_PROTOCOL_ERROR,
			errorbuffer.getString());
}

void sqlrcontroller_svr::logDbLogIn() {
	if (!sqlrlg) {
		return;
	}
	sqlrlg->runLoggers(NULL,conn,NULL,
			SQLRLOGGER_LOGLEVEL_INFO,
			SQLRLOGGER_EVENTTYPE_DB_LOGIN,
			NULL);
}

void sqlrcontroller_svr::logDbLogOut() {
	if (!sqlrlg) {
		return;
	}
	sqlrlg->runLoggers(NULL,conn,NULL,
			SQLRLOGGER_LOGLEVEL_INFO,
			SQLRLOGGER_EVENTTYPE_DB_LOGOUT,
			NULL);
}

void sqlrcontroller_svr::logDbError(sqlrcursor_svr *cursor, const char *info) {
	if (!sqlrlg) {
		return;
	}
	sqlrlg->runLoggers(NULL,conn,cursor,
			SQLRLOGGER_LOGLEVEL_ERROR,
			SQLRLOGGER_EVENTTYPE_DB_ERROR,
			info);
}

void sqlrcontroller_svr::logQuery(sqlrcursor_svr *cursor) {
	if (!sqlrlg) {
		return;
	}
	sqlrlg->runLoggers(NULL,conn,cursor,
			SQLRLOGGER_LOGLEVEL_INFO,
			SQLRLOGGER_EVENTTYPE_QUERY,
			NULL);
}

void sqlrcontroller_svr::logInternalError(sqlrcursor_svr *cursor,
							const char *info) {
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
	sqlrlg->runLoggers(NULL,conn,cursor,
			SQLRLOGGER_LOGLEVEL_ERROR,
			SQLRLOGGER_EVENTTYPE_INTERNAL_ERROR,
			errorbuffer.getString());
}

void sqlrcontroller_svr::alarmHandler(int32_t signum) {
	alarmrang=1;
}

const char *sqlrcontroller_svr::dbHostName() {
	return dbhostname;
}

const char *sqlrcontroller_svr::dbIpAddress() {
	return dbipaddress;
}

const char *sqlrcontroller_svr::identify() {
	return conn->identify();
}

const char *sqlrcontroller_svr::dbVersion() {
	return conn->dbVersion();
}

memorypool *sqlrcontroller_svr::getBindMappingsPool() {
	return bindmappingspool;
}

const char *sqlrcontroller_svr::translateTableName(const char *table) {
	if (sqlrt) {
		const char	*newname=NULL;
		if (sqlrt->getReplacementTableName(NULL,NULL,table,&newname)) {
			return newname;
		}
	}
	return NULL;
}

bool sqlrcontroller_svr::removeReplacementTable(const char *database,
							const char *schema,
							const char *table) {
	if (sqlrt) {
		return sqlrt->removeReplacementTable(database,schema,table);
	}
	return false;
}

bool sqlrcontroller_svr::removeReplacementIndex(const char *database,
							const char *schema,
							const char *table) {
	if (sqlrt) {
		return sqlrt->removeReplacementIndex(database,schema,table);
	}
	return false;
}

const char *sqlrcontroller_svr::getId() {
	return cmdl->getId();
}

const char *sqlrcontroller_svr::getLogDir() {
	return logdir;
}

const char *sqlrcontroller_svr::getDebugDir() {
	return debugdir;
}

bool sqlrcontroller_svr::isCustomQuery(sqlrcursor_svr *cursor) {
	return cursor->isCustomQuery();
}

bool sqlrcontroller_svr::getLobOutputBindLength(sqlrcursor_svr *cursor,
							uint16_t index,
							uint64_t *length) {
	return cursor->getLobOutputBindLength(index,length);
}

bool sqlrcontroller_svr::getLobOutputBindSegment(sqlrcursor_svr *cursor,
							uint16_t index,
							char *buffer,
							uint64_t buffersize,
							uint64_t offset,
							uint64_t charstoread,
							uint64_t *charsread) {
	return cursor->getLobOutputBindSegment(index,buffer,buffersize,
						offset,charstoread,charsread);
}

void sqlrcontroller_svr::closeLobOutputBind(sqlrcursor_svr *cursor,
							uint16_t index) {
	cursor->closeLobOutputBind(index);
}

bool sqlrcontroller_svr::fetchFromBindCursor(sqlrcursor_svr *cursor) {

	// set state
	updateState(PROCESS_SQL);

	logDebugMessage("fetching from bind cursor...");

	// clear query buffer just so some future operation doesn't
	// get confused into thinking this cursor actually ran one
	cursor->getQueryBuffer()[0]='\0';
	cursor->setQueryLength(0);

	bool	success=cursor->fetchFromBindCursor();
	
	// reset total rows fetched
	cursor->clearTotalRowsFetched();

	// on failure get the error (unless it's already been set)
	if (!success && !cursor->getErrorNumber()) {
		uint32_t	errorlength;
		int64_t		errnum;
		bool		liveconnection;
		errorMessage(cursor,
				cursor->getErrorBuffer(),
				maxerrorlength,
				&errorlength,&errnum,&liveconnection);
		cursor->setErrorLength(errorlength);
		cursor->setErrorNumber(errnum);
		cursor->setLiveConnection(liveconnection);
	}

	logDebugMessage((success)?"fetching from bind cursor succeeded":
					"fetching from bind cursor failed");
	logDebugMessage("done fetching from bind cursor");

	return success;
}

void sqlrcontroller_svr::errorMessage(sqlrcursor_svr *cursor,
						char *errorbuffer,
						uint32_t errorbuffersize,
						uint32_t *errorlength,
						int64_t *errorcode,
						bool *liveconnection) {
	return cursor->errorMessage(errorbuffer,errorbuffersize,
					errorlength,errorcode,liveconnection);
}

bool sqlrcontroller_svr::knowsRowCount(sqlrcursor_svr *cursor) {
	return cursor->knowsRowCount();
}

uint64_t sqlrcontroller_svr::rowCount(sqlrcursor_svr *cursor) {
	return cursor->rowCount();
}

bool sqlrcontroller_svr::knowsAffectedRows(sqlrcursor_svr *cursor) {
	return cursor->knowsAffectedRows();
}

uint64_t sqlrcontroller_svr::affectedRows(sqlrcursor_svr *cursor) {
	return cursor->affectedRows();
}

uint32_t sqlrcontroller_svr::colCount(sqlrcursor_svr *cursor) {
	return cursor->colCount();
}

uint16_t sqlrcontroller_svr::columnTypeFormat(sqlrcursor_svr *cursor) {
	return cursor->columnTypeFormat();
}

const char *sqlrcontroller_svr::getColumnName(sqlrcursor_svr *cursor,
							uint32_t col) {
	return cursor->getColumnName(col);
}

uint16_t sqlrcontroller_svr::getColumnNameLength(sqlrcursor_svr *cursor,
							uint32_t col) {
	return cursor->getColumnNameLength(col);
}

uint16_t sqlrcontroller_svr::getColumnType(sqlrcursor_svr *cursor,
							uint32_t col) {
	return cursor->getColumnType(col);
}

const char *sqlrcontroller_svr::getColumnTypeName(sqlrcursor_svr *cursor,
							uint32_t col) {
	return cursor->getColumnTypeName(col);
}

uint16_t sqlrcontroller_svr::getColumnTypeNameLength(sqlrcursor_svr *cursor,
							uint32_t col) {
	return cursor->getColumnTypeNameLength(col);
}

uint32_t sqlrcontroller_svr::getColumnLength(sqlrcursor_svr *cursor,
							uint32_t col) {
	return cursor->getColumnLength(col);
}

uint32_t sqlrcontroller_svr::getColumnPrecision(sqlrcursor_svr *cursor,
							uint32_t col) {
	return cursor->getColumnPrecision(col);
}

uint32_t sqlrcontroller_svr::getColumnScale(sqlrcursor_svr *cursor,
							uint32_t col) {
	return cursor->getColumnScale(col);
}

uint16_t sqlrcontroller_svr::getColumnIsNullable(sqlrcursor_svr *cursor,
							uint32_t col) {
	return cursor->getColumnIsNullable(col);
}

uint16_t sqlrcontroller_svr::getColumnIsPrimaryKey(sqlrcursor_svr *cursor,
							uint32_t col) {
	return cursor->getColumnIsPrimaryKey(col);
}

uint16_t sqlrcontroller_svr::getColumnIsUnique(sqlrcursor_svr *cursor,
							uint32_t col) {
	return cursor->getColumnIsUnique(col);
}

uint16_t sqlrcontroller_svr::getColumnIsPartOfKey(sqlrcursor_svr *cursor,
							uint32_t col) {
	return cursor->getColumnIsPartOfKey(col);
}

uint16_t sqlrcontroller_svr::getColumnIsUnsigned(sqlrcursor_svr *cursor,
							uint32_t col) {
	return cursor->getColumnIsUnsigned(col);
}

uint16_t sqlrcontroller_svr::getColumnIsZeroFilled(sqlrcursor_svr *cursor,
							uint32_t col) {
	return cursor->getColumnIsZeroFilled(col);
}

uint16_t sqlrcontroller_svr::getColumnIsBinary(sqlrcursor_svr *cursor,
							uint32_t col) {
	return cursor->getColumnIsBinary(col);
}

uint16_t sqlrcontroller_svr::getColumnIsAutoIncrement(sqlrcursor_svr *cursor,
							uint32_t col) {
	return cursor->getColumnIsAutoIncrement(col);
}

bool sqlrcontroller_svr::noRowsToReturn(sqlrcursor_svr *cursor) {
	return cursor->noRowsToReturn();
}

bool sqlrcontroller_svr::skipRow(sqlrcursor_svr *cursor) {
	return cursor->skipRow();
}

bool sqlrcontroller_svr::fetchRow(sqlrcursor_svr *cursor) {
	if (cursor->fetchRow()) {
		cursor->incrementTotalRowsFetched();
		return true;
	}
	return false;
}

void sqlrcontroller_svr::nextRow(sqlrcursor_svr *cursor) {
	return cursor->nextRow();
}

void sqlrcontroller_svr::getField(sqlrcursor_svr *cursor,
						uint32_t col,
						const char **field,
						uint64_t *fieldlength,
						bool *blob,
						bool *null) {
	cursor->getField(col,field,fieldlength,blob,null);
}

bool sqlrcontroller_svr::getLobFieldLength(sqlrcursor_svr *cursor,
							uint32_t col,
							uint64_t *length) {
	return cursor->getLobFieldLength(col,length);
}

bool sqlrcontroller_svr::getLobFieldSegment(sqlrcursor_svr *cursor,
							uint32_t col,
							char *buffer,
							uint64_t buffersize,
							uint64_t offset,
							uint64_t charstoread,
							uint64_t *charsread) {
	return cursor->getLobFieldSegment(col,buffer,buffersize,
						offset,charstoread,charsread);
}

void sqlrcontroller_svr::closeLobField(sqlrcursor_svr *cursor,
							uint32_t col) {
	cursor->closeLobField(col);
}

void sqlrcontroller_svr::closeResultSet(sqlrcursor_svr *cursor) {
	cursor->closeResultSet();
}

uint16_t sqlrcontroller_svr::getId(sqlrcursor_svr *cursor) {
	return cursor->getId();
}

void sqlrcontroller_svr::setFakeInputBindsForThisQuery(
						sqlrcursor_svr *cursor,
						bool fake) {
	return cursor->setFakeInputBindsForThisQuery(fake);
}

bool sqlrcontroller_svr::getFakeInputBindsForThisQuery(
						sqlrcursor_svr *cursor) {
	return cursor->getFakeInputBindsForThisQuery();
}
	
void sqlrcontroller_svr::setInputBindCount(sqlrcursor_svr *cursor,
						uint16_t inbindcount) {
	cursor->setInputBindCount(inbindcount);
}

uint16_t sqlrcontroller_svr::getInputBindCount(sqlrcursor_svr *cursor) {
	return cursor->getInputBindCount();
}

bindvar_svr *sqlrcontroller_svr::getInputBinds(sqlrcursor_svr *cursor) {
	return cursor->getInputBinds();
}

void sqlrcontroller_svr::setOutputBindCount(sqlrcursor_svr *cursor,
						uint16_t outbindcount) {
	return cursor->setOutputBindCount(outbindcount);
}

uint16_t sqlrcontroller_svr::getOutputBindCount(sqlrcursor_svr *cursor) {
	return cursor->getOutputBindCount();
}

bindvar_svr *sqlrcontroller_svr::getOutputBinds(sqlrcursor_svr *cursor) {
	return cursor->getOutputBinds();
}

bool sqlrcontroller_svr::open(sqlrcursor_svr *cursor) {
	return cursor->open();
}

bool sqlrcontroller_svr::close(sqlrcursor_svr *cursor) {
	return cursor->close();
}

void sqlrcontroller_svr::suspendResultSet(sqlrcursor_svr *cursor) {
	cursor->setState(SQLRCURSORSTATE_SUSPENDED);
	if (cursor->getCustomQueryCursor()) {
		cursor->getCustomQueryCursor()->
			setState(SQLRCURSORSTATE_SUSPENDED);
	}
}

void sqlrcontroller_svr::abort(sqlrcursor_svr *cursor) {
	cursor->abort();
}

char *sqlrcontroller_svr::getQueryBuffer(sqlrcursor_svr *cursor) {
	return cursor->getQueryBuffer();
}

uint32_t  sqlrcontroller_svr::getQueryLength(sqlrcursor_svr *cursor) {
	return cursor->getQueryLength();
}

void sqlrcontroller_svr::setQueryLength(sqlrcursor_svr *cursor,
						uint32_t querylength) {
	cursor->setQueryLength(querylength);
}

xmldom *sqlrcontroller_svr::getQueryTree(sqlrcursor_svr *cursor) {
	return cursor->getQueryTree();
}

void sqlrcontroller_svr::setCommandStart(sqlrcursor_svr *cursor,
						uint64_t sec, uint64_t usec) {
	cursor->setCommandStart(sec,usec);
}

uint64_t sqlrcontroller_svr::getCommandStartSec(sqlrcursor_svr *cursor) {
	return cursor->getCommandStartSec();
}

uint64_t sqlrcontroller_svr::getCommandStartUSec(sqlrcursor_svr *cursor) {
	return cursor->getCommandStartUSec();
}

void sqlrcontroller_svr::setCommandEnd(sqlrcursor_svr *cursor,
						uint64_t sec, uint64_t usec) {
	cursor->setCommandEnd(sec,usec);
}

uint64_t sqlrcontroller_svr::getCommandEndSec(sqlrcursor_svr *cursor) {
	return cursor->getCommandEndSec();
}

uint64_t sqlrcontroller_svr::getCommandEndUSec(sqlrcursor_svr *cursor) {
	return cursor->getCommandEndUSec();
}

void sqlrcontroller_svr::setQueryStart(sqlrcursor_svr *cursor,
						uint64_t sec, uint64_t usec) {
	cursor->setQueryStart(sec,usec);
}

uint64_t sqlrcontroller_svr::getQueryStartSec(sqlrcursor_svr *cursor) {
	return cursor->getQueryStartSec();
}

uint64_t sqlrcontroller_svr::getQueryStartUSec(sqlrcursor_svr *cursor) {
	return cursor->getQueryStartUSec();
}

void sqlrcontroller_svr::setQueryEnd(sqlrcursor_svr *cursor,
						uint64_t sec, uint64_t usec) {
	return cursor->setQueryEnd(sec,usec);
}

uint64_t sqlrcontroller_svr::getQueryEndSec(sqlrcursor_svr *cursor) {
	return cursor->getQueryEndSec();
}

uint64_t sqlrcontroller_svr::getQueryEndUSec(sqlrcursor_svr *cursor) {
	return cursor->getQueryEndUSec();
}

void sqlrcontroller_svr::setState(sqlrcursor_svr *cursor,
						sqlrcursorstate_t state) {
	cursor->setState(state);
}

sqlrcursorstate_t sqlrcontroller_svr::getState(sqlrcursor_svr *cursor) {
	return cursor->getState();
}

uint64_t sqlrcontroller_svr::getTotalRowsFetched(sqlrcursor_svr *cursor) {
	return cursor->getTotalRowsFetched();
}

void sqlrcontroller_svr::clearError(sqlrcursor_svr *cursor) {
	cursor->clearError();
}

void sqlrcontroller_svr::setError(sqlrcursor_svr *cursor,
				const char *err, int64_t errn, bool liveconn) {
	cursor->setError(err,errn,liveconn);
}

char *sqlrcontroller_svr::getErrorBuffer(sqlrcursor_svr *cursor) {
	return cursor->getErrorBuffer();
}

uint32_t sqlrcontroller_svr::getErrorLength(sqlrcursor_svr *cursor) {
	return cursor->getErrorLength();
}

void sqlrcontroller_svr::setErrorLength(sqlrcursor_svr *cursor,
						uint32_t errorlength) {
	cursor->setErrorLength(errorlength);
}

uint32_t sqlrcontroller_svr::getErrorNumber(sqlrcursor_svr *cursor) {
	return cursor->getErrorNumber();
}

void sqlrcontroller_svr::setErrorNumber(sqlrcursor_svr *cursor,
						uint32_t errnum) {
	cursor->setErrorNumber(errnum);
}

bool sqlrcontroller_svr::getLiveConnection(sqlrcursor_svr *cursor) {
	return cursor->getLiveConnection();
}

void sqlrcontroller_svr::setLiveConnection(sqlrcursor_svr *cursor,
						bool liveconnection) {
	return cursor->setLiveConnection(liveconnection);
}
