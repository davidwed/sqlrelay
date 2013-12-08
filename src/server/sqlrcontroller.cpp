// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <config.h>

#include <sqlrcontroller.h>
#include <sqlrclientprotocol.h>
#include <rudiments/file.h>
#include <rudiments/rawbuffer.h>
#include <rudiments/passwdentry.h>
#include <rudiments/groupentry.h>
#include <rudiments/process.h>
#include <rudiments/permissions.h>
#include <rudiments/snooze.h>
#include <rudiments/error.h>
#include <rudiments/signalclasses.h>
#include <rudiments/datetime.h>
#include <rudiments/character.h>
#include <rudiments/charstring.h>
#include <rudiments/stdio.h>

#include <defines.h>
#include <datatypes.h>
#define NEED_CONVERT_DATE_TIME
#include <parsedatetime.h>

#ifndef SQLRELAY_ENABLE_SHARED
	extern "C" {
		#include "sqlrconnectiondeclarations.cpp"
	}
#endif

sqlrcontroller_svr::sqlrcontroller_svr() : listener() {

	conn=NULL;

	cmdl=NULL;
	cfgfl=NULL;
	semset=NULL;
	idmemory=NULL;
	connstats=NULL;

	updown=NULL;

	dbselected=false;
	originaldb=NULL;

	tmpdir=NULL;

	unixsocket=NULL;
	unixsocketptr=NULL;
	unixsocketptrlen=0;
	serversockun=NULL;
	serversockin=NULL;
	serversockincount=0;

	inetport=0;
	authc=NULL;
	lastuserbuffer[0]='\0';
	lastpasswordbuffer[0]='\0';
	lastauthsuccess=false;

	commitorrollback=false;

	autocommitforthissession=false;

	translatebegins=true;
	faketransactionblocks=false;
	faketransactionblocksautocommiton=false;
	intransactionblock=false;

	fakeinputbinds=false;
	translatebinds=false;

	isolationlevel=NULL;

	maxquerysize=0;
	maxbindcount=0;
	maxerrorlength=0;
	idleclienttimeout=-1;

	connected=false;
	inclientsession=false;
	loggedin=false;

	// maybe someday these parameters will be configurable
	bindmappingspool=new memorypool(512,128,100);
	inbindmappings=new namevaluepairs;
	outbindmappings=new namevaluepairs;

	sqlp=NULL;
	sqlt=NULL;
	sqlw=NULL;
	sqltr=NULL;
	sqlrlg=NULL;
	sqlrq=NULL;
	sqlrpe=NULL;

	decrypteddbpassword=NULL;

	debugsqltranslation=false;
	debugtriggers=false;

	cur=NULL;

	pidfile=NULL;

	clientinfo=NULL;
	clientinfolen=0;

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

	if (connstats) {
		rawbuffer::zero(connstats,sizeof(sqlrconnstatistics));
	}

	delete cmdl;
	delete cfgfl;

	delete[] updown;

	delete[] originaldb;

	delete tmpdir;

	delete authc;

	delete idmemory;

	delete semset;

	if (unixsocket) {
		file::remove(unixsocket);
		delete[] unixsocket;
	}

	delete bindmappingspool;
	delete inbindmappings;
	delete outbindmappings;

	delete sqlp;
	delete sqlt;
	delete sqlw;
	delete sqltr;
	delete sqlrlg;
	delete sqlrq;
	delete sqlrpe;

	delete[] decrypteddbpassword;

	delete[] clientinfo;

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

	// get password encryptions
	const char	*pwdencs=cfgfl->getPasswordEncryptions();
	if (pwdencs && pwdencs[0]) {
		sqlrpe=new sqlrpwdencs;
		sqlrpe->loadPasswordEncryptions(pwdencs);
	}	

	// load the authenticator
	authc=new sqlrauthenticator(cfgfl,sqlrpe);

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
	bool	reloginatstart=cfgfl->getReLoginAtStart();
	if (!reloginatstart) {
		if (!attemptLogIn(!silent)) {
			return false;
		}
	}
	if (!cmdl->found("-nodetach")) {
		process::detach();
	}
	if (reloginatstart) {
		while (!attemptLogIn(false)) {
			snooze::macrosnooze(5);
		}
	}
	initConnStats();

	// Get the query translators.  Do it after logging in, as
	// getSqlTranslator might return a different class depending on what
	// version of the db it gets logged into
	const char	*translations=cfgfl->getTranslations();
	if (translations && translations[0]) {
		sqlp=new sqlparser;
		sqlt=conn->getSqlTranslations();
		sqlt->loadTranslations(translations);
		sqlw=conn->getSqlWriter();
	}
	debugsqltranslation=cfgfl->getDebugTranslations();

	// get the triggers
	const char	*triggers=cfgfl->getTriggers();
	if (triggers && triggers[0]) {
		// for triggers, we'll need an sqlparser as well
		if (!sqlp) {
			sqlp=new sqlparser;
		}
		sqltr=new sqltriggers;
		sqltr->loadTriggers(triggers);
	}
	debugtriggers=cfgfl->getDebugTriggers();

	// update various configurable parameters
	maxclientinfolength=cfgfl->getMaxClientInfoLength();
	maxquerysize=cfgfl->getMaxQuerySize();
	maxbindcount=cfgfl->getMaxBindCount();
	maxerrorlength=cfgfl->getMaxErrorLength();
	idleclienttimeout=cfgfl->getIdleClientTimeout();
	reformatdatetimes=(cfgfl->getDateTimeFormat() ||
				cfgfl->getDateFormat() ||
				cfgfl->getTimeFormat());

	// set autocommit behavior
	setAutoCommit(conn->autocommit);

	// get fake input bind variable behavior
	// (this may have already been set true by the connect string)
	fakeinputbinds=(fakeinputbinds || cfgfl->getFakeInputBindVariables());

	// get translate bind variable behavior
	translatebinds=cfgfl->getTranslateBindVariables();

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

	// create clientinfo buffer
	clientinfo=new char[maxclientinfolength+1];

	// create error buffer
	// FIXME: this should definitely be done inside the connection class
	conn->error=new char[maxerrorlength+1];

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
	sqlrp[SQLRPROTOCOL_SQLRCLIENT]=new sqlrclientprotocol(this,conn,cfgfl);
	
	return true;
}

void sqlrcontroller_svr::setUserAndGroup() {

	// get the user that we're currently running as
	char	*currentuser=
		passwdentry::getName(process::getEffectiveUserId());

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

bool sqlrcontroller_svr::initCursors(int32_t count) {

	logDebugMessage("initializing cursors...");

	cursorcount=count;
	if (!cur) {
		cur=new sqlrcursor_svr *[maxcursorcount];
		rawbuffer::zero(cur,maxcursorcount*sizeof(sqlrcursor_svr *));
	}

	for (int32_t i=0; i<cursorcount; i++) {

		if (!cur[i]) {
			cur[i]=initCursor();
		}
		if (!cur[i]->openInternal(i)) {
			debugstr.clear();
			debugstr.append("cursor init failed: ")->append(i);
			logInternalError(NULL,debugstr.getString());
			return false;
		}
	}

	logDebugMessage("done initializing cursors");

	return true;
}

sqlrcursor_svr *sqlrcontroller_svr::initCursor() {
	sqlrcursor_svr	*cursor=conn->initCursor();
	if (cursor) {
		incrementOpenDatabaseCursors();
	}
	return cursor;
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

				addFileDescriptor(serversockun);

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
	
					addFileDescriptor(serversockin[index]);

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
		announceAvailability(unixsocket,inetport,connectionid);

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

			} else {

				// if waitForClient() times out waiting for
				// someone to pick up the suspended
				// session, roll it back and kill it
				if (suspendedsession) {
					if (conn->isTransactional()) {
						rollback();
					}
					suspendedsession=false;
				}
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
	if (conn->autocommit) {
		conn->autoCommitOn();
	} else {
		conn->autoCommitOff();
	}

	// FIXME: restore the isolation level

	markDatabaseAvailable();
}

void sqlrcontroller_svr::initSession() {

	logDebugMessage("initializing session...");

	commitorrollback=false;
	suspendedsession=false;
	for (int32_t i=0; i<cursorcount; i++) {
		cur[i]->state=SQLRCURSORSTATE_AVAILABLE;
	}
	accepttimeout=5;

	logDebugMessage("done initializing session...");
}

void sqlrcontroller_svr::announceAvailability(const char *unixsocket,
						unsigned short inetport,
						const char *connectionid) {

	logDebugMessage("announcing availability...");

	// connect to listener if we haven't already
	// and pass it this process's pid
	if (!connected) {
		registerForHandoff();
	}

	// handle time-to-live
	if (ttl>0) {
		signalmanager::alarm(ttl);
	}

	acquireAnnounceMutex();

	// cancel time-to-live alarm
	//
	// It's important to cancel the alarm here.
	//
	// Connections which successfully announce themselves to the listener
	// cannot then die off.
	//
	// If handoff=pass, the listener can handle it if a connection dies off,
	// but not if handoff=reconnect, there's no way for it to know the
	// connection is gone.
	//
	// But, if the connection signals the listener to read the registration
	// and dies off before it receives a return signal from the listener
	// indicating that the listener has read it, then it can cause
	// problems.  And we can't simply call waitWithUndo() and
	// signalWithUndo().  Not only could the undo counter could overflow,
	// but we really only want to undo the signal() if the connection shuts
	// down before doing the wait() and there's no way to optionally
	// undo a semaphore.
	//
	// What a mess.
	if (ttl>0) {
		signalmanager::alarm(0);
	}

	updateState(ANNOUNCE_AVAILABILITY);

	// get a pointer to the shared memory segment
	shmdata	*idmemoryptr=getAnnounceBuffer();

	// first, write the connectionid into the segment
	charstring::copy(idmemoryptr->connectionid,
				connectionid,MAXCONNECTIONIDLEN);

	// write the pid into the segment
	idmemoryptr->connectioninfo.connectionpid=process::getProcessId();

	signalListenerToRead();

	waitForListenerToFinishReading();

	releaseAnnounceMutex();

	logDebugMessage("done announcing availability...");
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

	// FIXME: listen() checks for 2,1,0 or -1 from this method, but this
	// method only returns 2, 1 or -1.  0 should indicate that a suspended
	// session timed out.

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
			if (!handoffsockun.receiveFileDescriptor(&descriptor)) {
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
		clientsock=new filedescriptor;
		clientsock->setFileDescriptor(descriptor);

		logDebugMessage("done waiting for client");

	} else {

		// If we're in the middle of a suspended session, wait for
		// a client to reconnect...


		if (waitForNonBlockingRead(accepttimeout,0)<1) {
			logInternalError(NULL,"wait for client connect failed");
			// FIXME: I think this should return 0
			return -1;
		}

		// get the first socket that had data available...
		filedescriptor	*fd=getReadyList()->getFirstNode()->getValue();

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
			// FIXME: I think this should return 0
			return -1;
		}
	}

	// set up the socket
	clientsock->translateByteOrder();
	clientsock->dontUseNaglesAlgorithm();
	clientsock->setReadBufferSize(8192);
	clientsock->setWriteBufferSize(8192);
	return 1;
}

void sqlrcontroller_svr::clientSession() {

	// determine client protocol
	sqlrprotocol_t	protocol=getClientProtocol();
	if (protocol==SQLRPROTOCOL_UNKNOWN) {
		closeClientSocket(0);
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
		proto->closeClientSession();
	} else {
		closeClientSocket(0);
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

sqlrcursor_svr *sqlrcontroller_svr::getCursorById(uint16_t id) {

	for (uint16_t i=0; i<cursorcount; i++) {
		if (cur[i]->id==id) {
			return cur[i];
		}
	}
	return NULL;
}

sqlrcursor_svr *sqlrcontroller_svr::findAvailableCursor() {

	// find an available cursor
	for (uint16_t i=0; i<cursorcount; i++) {
		if (cur[i]->state==SQLRCURSORSTATE_AVAILABLE) {
			debugstr.clear();
			debugstr.append("available cursor: ")->append(i);
			logDebugMessage(debugstr.getString());
			cur[i]->state=SQLRCURSORSTATE_BUSY;
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
		cur[cursorcount]=initCursor();
		cur[cursorcount]->state=SQLRCURSORSTATE_AVAILABLE;
		if (!cur[cursorcount]->openInternal(cursorcount)) {
			debugstr.clear();
			debugstr.append("cursor init failure: ");
			debugstr.append(cursorcount);
			logInternalError(NULL,debugstr.getString());
			return NULL;
		}
		cursorcount++;
	} while (cursorcount<expandto);
	
	// return the first new cursor that we created
	cur[firstnewcursor]->state=SQLRCURSORSTATE_BUSY;
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
	bool	retval=authc->authenticate(userbuffer,passwordbuffer);
	if (retval) {
		logDebugMessage("auth succeeded on connection");
	} else {
		logClientConnectionRefused("auth failed on connection: "
						"invalid user/password");
	}
	return retval;
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

void sqlrcontroller_svr::suspendSession(const char **unixsocketname,
						uint16_t *inetportnumber) {

	// mark the session suspended
	suspendedsession=true;

	// we can't wait forever for the client to resume, set a timeout
	accepttimeout=cfgfl->getSessionTimeout();

	// abort all cursors that aren't suspended...
	logDebugMessage("aborting busy cursors...");
	for (int32_t i=0; i<cursorcount; i++) {
		if (cur[i]->state==SQLRCURSORSTATE_BUSY) {
			cur[i]->abort();
		}
	}
	logDebugMessage("done aborting busy cursors");

	// open sockets to resume on
	logDebugMessage("opening sockets to resume on...");
	*unixsocketname=NULL;
	*inetportnumber=0;
	if (openSockets()) {
		if (serversockun) {
			*unixsocketname=unixsocket;
		}
		*inetportnumber=inetport;
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

bool sqlrcontroller_svr::handleFakeTransactionQueries(sqlrcursor_svr *cursor,
						bool *wasfaketransactionquery) {

	*wasfaketransactionquery=false;

	// Intercept begins and handle them.  If we're faking begins, commit
	// and rollback queries also need to be intercepted as well, otherwise
	// the query will be sent directly to the db and endFakeBeginTransaction
	// won't get called.
	if (isBeginTransactionQuery(cursor)) {
		*wasfaketransactionquery=true;
		cursor->inbindcount=0;
		cursor->outbindcount=0;
		sendcolumninfo=DONT_SEND_COLUMN_INFO;
		if (intransactionblock) {
			charstring::copy(cursor->error,
				"begin while in transaction block");
			cursor->errorlength=charstring::length(cursor->error);
			cursor->errnum=999999;
			return false;
		}
		return begin();
		// FIXME: if the begin fails and the db api doesn't support
		// a begin command then the connection-level error needs to
		// be copied to the cursor so queryOrBindCursor can report it
	} else if (isCommitQuery(cursor)) {
		*wasfaketransactionquery=true;
		cursor->inbindcount=0;
		cursor->outbindcount=0;
		sendcolumninfo=DONT_SEND_COLUMN_INFO;
		if (!intransactionblock) {
			charstring::copy(cursor->error,
				"commit while not in transaction block");
			cursor->errorlength=charstring::length(cursor->error);
			cursor->errnum=999998;
			return false;
		}
		return commit();
		// FIXME: if the commit fails and the db api doesn't support
		// a commit command then the connection-level error needs to
		// be copied to the cursor so queryOrBindCursor can report it
	} else if (isRollbackQuery(cursor)) {
		*wasfaketransactionquery=true;
		cursor->inbindcount=0;
		cursor->outbindcount=0;
		sendcolumninfo=DONT_SEND_COLUMN_INFO;
		if (!intransactionblock) {
			charstring::copy(cursor->error,
				"rollback while not in transaction block");
			cursor->errorlength=charstring::length(cursor->error);
			cursor->errnum=999997;
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
	const char	*ptr=cursor->skipWhitespaceAndComments(
						cursor->querybuffer);

	// See if it was any of the different queries used to start a
	// transaction.  IMPORTANT: don't just look for the first 5 characters
	// to be "BEGIN", make sure it's the entire query.  Many db's use
	// "BEGIN" to start a stored procedure block, but in those cases,
	// something will follow it.
	if (!charstring::compareIgnoringCase(ptr,"BEGIN",5)) {

		// make sure there are only spaces, comments or the word "work"
		// after the begin
		const char	*spaceptr=
				cursor->skipWhitespaceAndComments(ptr+5);
		
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
			cursor->skipWhitespaceAndComments(cursor->querybuffer),
			"commit",6);
}

bool sqlrcontroller_svr::isRollbackQuery(sqlrcursor_svr *cursor) {
	return !charstring::compareIgnoringCase(
			cursor->skipWhitespaceAndComments(cursor->querybuffer),
			"rollback",8);
}

bool sqlrcontroller_svr::processQueryOrBindCursor(sqlrcursor_svr *cursor,
					bool reexecute, bool bindcursor) {

	// handle fake transaction queries
	bool	success=false;
	bool	wasfaketransactionquery=false;
	if (!reexecute && !bindcursor && faketransactionblocks) {
		success=handleFakeTransactionQueries(cursor,
					&wasfaketransactionquery);
	}
	if (wasfaketransactionquery) {
		return success;
	}

	// process the query...
	logDebugMessage("processing query...");

	// on reexecute, translate bind variables from mapping
	if (reexecute) {
		translateBindVariablesFromMappings(cursor);
	}

	success=false;
	bool	supportsnativebinds=cursor->supportsNativeBinds();

	if (reexecute &&
		!cursor->fakeinputbindsforthisquery &&
		supportsnativebinds) {

		// if we're reexecuting and not faking binds then
		// the statement doesn't need to be prepared again,
		// just execute it...

		logDebugMessage("re-executing...");
		success=(handleBinds(cursor) && executeQuery(cursor,
							cursor->querybuffer,
							cursor->querylength));

	} else if (bindcursor) {

		// if we're handling a bind cursor...
		logDebugMessage("bind cursor...");
		success=cursor->fetchFromBindCursor();

	} else {

		// otherwise, prepare and execute the query...
		// generally this a first time query but it could also be
		// a reexecute if we're faking binds

		logDebugMessage("preparing/executing...");

		// rewrite the query, if necessary
		rewriteQuery(cursor);

		// buffers and pointers...
		stringbuffer	outputquery;
		const char	*query=cursor->querybuffer;
		uint32_t	querylen=cursor->querylength;

		// fake input binds if necessary
		// NOTE: we can't just overwrite the querybuffer when
		// faking binds or we'll lose the original query and
		// end up rerunning the modified query when reexecuting
		if (cursor->fakeinputbindsforthisquery ||
					!supportsnativebinds) {
			logDebugMessage("faking binds...");
			if (cursor->fakeInputBinds(&outputquery)) {
				query=outputquery.getString();
				querylen=outputquery.getStringLength();
			}
		}

		// prepare
		success=cursor->prepareQuery(query,querylen);

		// if we're not faking binds then
		// handle the binds for real
		if (success &&
			!cursor->fakeinputbindsforthisquery &&
			cursor->supportsNativeBinds()) {
			success=handleBinds(cursor);
		}

		// execute
		if (success) {
			success=executeQuery(cursor,query,querylen);
		}
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
	if (success && conn->isTransactional() && commitorrollback &&
				conn->fakeautocommit && conn->autocommit) {
		logDebugMessage("commit necessary...");
		success=commit();
	}
	
	// if the query failed, get the error (unless it's already been set)
	if (!success && !cursor->errnum) {
		// FIXME: errors for queries run by triggers won't be set here
		cursor->errorMessage(cursor->error,
					maxerrorlength,
					&(cursor->errorlength),
					&(cursor->errnum),
					&(cursor->liveconnection));
	}

	if (success) {
		logDebugMessage("processing query succeeded");
	} else {
		logDebugMessage("processing query failed");
	}
	logDebugMessage("done processing query");

	return success;
}

void sqlrcontroller_svr::translateBindVariablesFromMappings(
						sqlrcursor_svr *cursor) {
	// index variable
	uint16_t	i=0;

	// run two passes
	for (i=0; i<2; i++) {

		// first pass for input binds, second pass for output binds
		uint16_t	count=(!i)?cursor->inbindcount:
						cursor->outbindcount;
		bindvar_svr	*vars=(!i)?cursor->inbindvars:
						cursor->outbindvars;
		namevaluepairs	*mappings=(!i)?inbindmappings:outbindmappings;

		for (uint16_t j=0; j<count; j++) {

			// get the bind var
			bindvar_svr	*b=&(vars[j]);

			// remap it
			char	*newvariable;
			if (mappings->getValue(b->variable,&newvariable)) {
				b->variable=newvariable;
			}
		}
	}

	// debug
	logDebugMessage("remapped input binds:");
	for (i=0; i<cursor->inbindcount; i++) {
		logDebugMessage(cursor->inbindvars[i].variable);
	}
	logDebugMessage("remapped output binds:");
	for (i=0; i<cursor->outbindcount; i++) {
		logDebugMessage(cursor->outbindvars[i].variable);
	}
}

void sqlrcontroller_svr::rewriteQuery(sqlrcursor_svr *cursor) {

	if (sqlp && sqlt && sqlw) {
		if (!translateQuery(cursor)) {
			// FIXME: do something?
		}
	}

	if (translatebinds) {
		translateBindVariables(cursor);
	}

	if (conn->supportsTransactionBlocks()) {
		translateBeginTransaction(cursor);
	}
}

bool sqlrcontroller_svr::translateQuery(sqlrcursor_svr *cursor) {

	if (debugsqltranslation) {
		stdoutput.printf("original:\n\"%s\"\n\n",cursor->querybuffer);
	}

	// parse the query
	bool	parsed=sqlp->parse(cursor->querybuffer);

	// get the parsed tree
	delete cursor->querytree;
	cursor->querytree=sqlp->detachTree();
	if (!cursor->querytree) {
		return false;
	}

	if (debugsqltranslation) {
		stdoutput.printf("before translation:\n");
		cursor->querytree->getRootNode()->print(&stdoutput);
		stdoutput.printf("\n");
	}

	if (!parsed) {
		if (debugsqltranslation) {
			stdoutput.printf(
				"parse failed, using original:\n\"%s\"\n\n",
							cursor->querybuffer);
		}
		delete cursor->querytree;
		cursor->querytree=NULL;
		return false;
	}

	// apply translation rules
	if (!sqlt->runTranslations(conn,cursor,cursor->querytree)) {
		return false;
	}

	if (debugsqltranslation) {
		stdoutput.printf("after translation:\n");
		cursor->querytree->getRootNode()->print(&stdoutput);
		stdoutput.printf("\n");
	}

	// write the query back out
	stringbuffer	translatedquery;
	if (!sqlw->write(conn,cursor,cursor->querytree,&translatedquery)) {
		return false;
	}

	if (debugsqltranslation) {
		stdoutput.printf("translated:\n\"%s\"\n\n",
				translatedquery.getString());
	}

	// copy the translated query into query buffer
	if (translatedquery.getStringLength()>maxquerysize) {
		// the translated query was too large
		return false;
	}
	charstring::copy(cursor->querybuffer,
			translatedquery.getString(),
			translatedquery.getStringLength());
	cursor->querylength=translatedquery.getStringLength();
	cursor->querybuffer[cursor->querylength]='\0';
	return true;
}

enum queryparsestate_t {
	IN_QUERY=0,
	IN_QUOTES,
	BEFORE_BIND,
	IN_BIND
};

void sqlrcontroller_svr::translateBindVariables(sqlrcursor_svr *cursor) {

	// index variable
	uint16_t	i=0;

	// debug
	logDebugMessage("translating bind variables...");
	logDebugMessage("original:");
	logDebugMessage(cursor->querybuffer);
	logDebugMessage("input binds:");
	for (i=0; i<cursor->inbindcount; i++) {
		logDebugMessage(cursor->inbindvars[i].variable);
	}
	logDebugMessage("output binds:");
	for (i=0; i<cursor->outbindcount; i++) {
		logDebugMessage(cursor->outbindvars[i].variable);
	}

	// convert queries from whatever bind variable format they currently
	// use to the format required by the database...

	bool			convert=false;
	queryparsestate_t	parsestate=IN_QUERY;
	stringbuffer	newquery;
	stringbuffer	currentbind;
	const char	*endptr=cursor->querybuffer+cursor->querylength-1;

	// use 1-based index for bind variables
	uint16_t	bindindex=1;
	
	// run through the querybuffer...
	char *c=cursor->querybuffer;
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
			// (make sure to catch @'s but not @@'s
			if (*c=='?' || *c==':' ||
				(*c=='@' && *(c+1)!='@') || *c=='$') {
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

				// Bail if the current bind variable format
				// matches the db bind format.
				if (matchesNativeBindFormat(
						currentbind.getString())) {
					return;
				}

				// translate...
				convert=true;
				translateBindVariableInStringAndArray(cursor,
								&currentbind,
								bindindex,
								&newquery);
				bindindex++;

				parsestate=IN_QUERY;

			} else {
				currentbind.append(*c);
				c++;
			}
			continue;
		}

	} while (c<=endptr);

	if (!convert) {
		return;
	}


	// if we made it here then some conversion
	// was done - update the querybuffer...
	const char	*newq=newquery.getString();
	cursor->querylength=newquery.getStringLength();
	if (cursor->querylength>maxquerysize) {
		cursor->querylength=maxquerysize;
	}
	charstring::copy(cursor->querybuffer,newq,cursor->querylength);
	cursor->querybuffer[cursor->querylength]='\0';


	// debug
	if (debugsqltranslation) {
		stdoutput.printf("bind translation:\n\"%s\"\n",
						cursor->querybuffer);
		for (i=0; i<cursor->inbindcount; i++) {
			stdoutput.printf("  inbind: \"%s\"\n",
					cursor->inbindvars[i].variable);
		}
		for (i=0; i<cursor->outbindcount; i++) {
			stdoutput.printf("  outbind: \"%s\"\n",
					cursor->outbindvars[i].variable);
		}
		stdoutput.printf("\n");
	}
	logDebugMessage("converted:");
	logDebugMessage(cursor->querybuffer);
	logDebugMessage("input binds:");
	for (i=0; i<cursor->inbindcount; i++) {
		logDebugMessage(cursor->inbindvars[i].variable);
	}
	logDebugMessage("output binds:");
	for (i=0; i<cursor->outbindcount; i++) {
		logDebugMessage(cursor->outbindvars[i].variable);
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
		(bindformat[1]=='*' && !character::isAlphanumeric(bind[1]))));
}

void sqlrcontroller_svr::translateBindVariableInStringAndArray(
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
		translateBindVariableInArray(cursor,
					currentbind->getString(),
					bindindex);

	} else if (bindformat[1]=='1' &&
			!charstring::isNumber(currentbind->getString()+1)) {

		// This section handles 2-character placeholders where the
		// second position is a 1, such as $1 (postgresql-format).

		// replace bind variable in string with number
		newquery->append(bindindex);

		// replace bind variable itself with number
		translateBindVariableInArray(cursor,
					currentbind->getString(),
					bindindex);

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
			translateBindVariableInArray(cursor,
						currentbind->getString(),
						bindindex);
		} else {
			newquery->append(currentbind->getString()+1,
					currentbind->getStringLength()-1);
		}
	}
}

void sqlrcontroller_svr::translateBindVariableInArray(sqlrcursor_svr *cursor,
						const char *currentbind,
						uint16_t bindindex) {

	// if the current bind variable is a ? then just
	// set it NULL for special handling later
	if (!charstring::compare(currentbind,"?")) {
		currentbind=NULL;
	}

	// run two passes
	for (uint16_t i=0; i<2; i++) {

		// first pass for input binds, second pass for output binds
		uint16_t	count=(!i)?cursor->inbindcount:
						cursor->outbindcount;
		bindvar_svr	*vars=(!i)?cursor->inbindvars:
						cursor->outbindvars;
		namevaluepairs	*mappings=(!i)?inbindmappings:outbindmappings;

		for (uint16_t j=0; j<count; j++) {

			// get the bind var
			bindvar_svr	*b=&(vars[j]);

			// If a bind var name was passed in, look for a bind
			// variable with a matching name.
			// If no name was passed in then the bind vars are
			// numeric; get the variable who's numeric name matches
			// the index passed in.
			if ((currentbind &&
				!charstring::compare(currentbind,
							b->variable)) ||
				(!currentbind &&
				charstring::toInteger((b->variable)+1)==
								bindindex)) {

				// create the new bind var
				// name and get its length
				char		*tempnumber=charstring::
							parseNumber(bindindex);
				uint16_t	tempnumberlen=charstring::
							length(tempnumber);

				// keep track of the old name
				char	*oldvariable=b->variable;

				// allocate memory for the new name
				b->variable=(char *)bindmappingspool->
						allocate(tempnumberlen+2);

				// replace the existing bind var name and size
				b->variable[0]=conn->bindVariablePrefix();
				charstring::copy(b->variable+1,tempnumber);
				b->variable[tempnumberlen+1]='\0';
				b->variablesize=tempnumberlen+1;

				// create bind variable mappings
				mappings->setValue(oldvariable,b->variable);
				
				// clean up
				delete[] tempnumber;
			}
		}
	}
}

void sqlrcontroller_svr::translateBeginTransaction(sqlrcursor_svr *cursor) {

	if (!isBeginTransactionQuery(cursor)) {
		return;
	}

	// debug
	logDebugMessage("translating begin tx query...");
	logDebugMessage("original:");
	logDebugMessage(cursor->querybuffer);

	// translate query
	const char	*beginquery=conn->beginTransactionQuery();
	cursor->querylength=charstring::length(beginquery);
	charstring::copy(cursor->querybuffer,beginquery,cursor->querylength);
	cursor->querybuffer[cursor->querylength]='\0';

	// debug
	logDebugMessage("converted:");
	logDebugMessage(cursor->querybuffer);
}

sqlrcursor_svr	*sqlrcontroller_svr::initQueryOrBindCursor(
						sqlrcursor_svr *cursor,
						bool reexecute,
						bool bindcursor,
						bool getquery) {

	// decide whether to use the cursor itself
	// or an attached custom query cursor
	if (cursor->customquerycursor) {
		if (reexecute) {
			cursor=cursor->customquerycursor;
		} else {
			cursor->customquerycursor->close();
			delete cursor->customquerycursor;
			cursor->customquerycursor=NULL;
		}
	}

	// re-init error data
	cursor->clearError();

	// clear bind mappings and reset the fake input binds flag
	// (do this here because getInput/OutputBinds uses the bindmappingspool)
	if (!reexecute && !bindcursor) {
		bindmappingspool->deallocate();
		inbindmappings->clear();
		outbindmappings->clear();
		cursor->fakeinputbindsforthisquery=fakeinputbinds;
	}

	// clean up whatever result set the cursor might have been busy with
	cursor->cleanUpData();

	if (getquery) {

		// re-init buffers
		if (!reexecute) {
			if (!bindcursor) {
				clientinfo[0]='\0';
				clientinfolen=0;
			}
			cursor->querybuffer[0]='\0';
			cursor->querylength=0;
		}
		cursor->inbindcount=0;
		cursor->outbindcount=0;
		for (uint16_t i=0; i<maxbindcount; i++) {
			rawbuffer::zero(&(cursor->inbindvars[i]),
						sizeof(bindvar_svr));
			rawbuffer::zero(&(cursor->outbindvars[i]),
						sizeof(bindvar_svr));
		}
	}

	return cursor;
}

sqlrcursor_svr *sqlrcontroller_svr::useCustomQueryHandler(	
						sqlrcursor_svr *cursor) {

	// do we need to use a custom query handler for this query?

	// bail right away if custom queries aren't enabled
	if (!sqlrq) {
		return cursor;
	}

	// see if the query matches one of the custom queries
	cursor->customquerycursor=sqlrq->match(conn,cursor->querybuffer,
							cursor->querylength);
				
	// if so...
	if (cursor->customquerycursor) {

		// open the cursor
		cursor->customquerycursor->openInternal(cursor->id);

		// copy the query that we just got into the custom query cursor
		charstring::copy(cursor->customquerycursor->querybuffer,
							cursor->querybuffer);
		cursor->customquerycursor->querylength=cursor->querylength;

		// set the cursor state
		cursor->customquerycursor->state=SQLRCURSORSTATE_BUSY;

		// reset the cursor
		cursor=cursor->customquerycursor;
	}

	return cursor;
}

bool sqlrcontroller_svr::handleBinds(sqlrcursor_svr *cursor) {

	bindvar_svr	*bind=NULL;
	
	// iterate through the arrays, binding values to variables
	for (int16_t in=0; in<cursor->inbindcount; in++) {

		bind=&cursor->inbindvars[in];

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

	for (int16_t out=0; out<cursor->outbindcount; out++) {

		bind=&cursor->outbindvars[out];

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

			// find the cursor that we acquird earlier...
			for (uint16_t j=0; j<cursorcount; j++) {

				if (cur[j]->id==bind->value.cursorid) {
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

bool sqlrcontroller_svr::executeQuery(sqlrcursor_svr *curs,
						const char *query,
						uint32_t length) {

	// handle before-triggers
	if (sqltr) {
		sqltr->runBeforeTriggers(conn,curs,curs->querytree);
	}

	// get the query start time
	datetime	dt;
	dt.getSystemDateAndTime();
	curs->querystartsec=dt.getSeconds();
	curs->querystartusec=dt.getMicroseconds();

	// execute the query
	curs->queryresult=curs->executeQuery(query,length);

	// get the query end time
	dt.getSystemDateAndTime();
	curs->queryendsec=dt.getSeconds();
	curs->queryendusec=dt.getMicroseconds();

	// update query and error counts
	incrementQueryCounts(curs->queryType(query,length));
	if (!curs->queryresult) {
		incrementTotalErrors();
	}

	// handle after-triggers
	if (sqltr) {
		sqltr->runAfterTriggers(conn,curs,curs->querytree,true);
	}

	return curs->queryresult;
}

void sqlrcontroller_svr::commitOrRollback(sqlrcursor_svr *cursor) {

	logDebugMessage("commit or rollback check...");

	// if the query was a commit or rollback, set a flag indicating so
	if (conn->isTransactional()) {
		// FIXME: if db has been put in the repeatable-read isolation
		// level then commitorrollback=true needs to be set no
		// matter what the query was
		if (cursor->queryIsCommitOrRollback()) {
			logDebugMessage("commit or rollback not needed");
			commitorrollback=false;
		} else if (cursor->queryIsNotSelect()) {
			logDebugMessage("commit or rollback needed");
			commitorrollback=true;
		}
	}

	logDebugMessage("done with commit or rollback check");
}

void sqlrcontroller_svr::setFakeInputBinds(bool fake) {
	fakeinputbinds=fake;
}

bool sqlrcontroller_svr::sendColumnInfo() {
	if (sendcolumninfo==SEND_COLUMN_INFO) {
		return true;
	}
	return false;
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

		if (cursor->lastrowvalid) {
			cursor->lastrow++;
		} else {
			cursor->lastrowvalid=true;
			cursor->lastrow=0;
		}

		if (!cursor->skipRow()) {
			logDebugMessage("skipping rows hit the "
					"end of the result set");
			return false;
		}
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

	// for now this method just reformats dates but in the future it could
	// be extended to to configurable field reformatting...

	// initialize return values
	*newfield=field;
	*newfieldlength=fieldlength;

	// convert date/time values, if configured to do so
	if (!reformatdatetimes) {
		return;
	}

	// are dates going to be in MM/DD or DD/MM format?
	bool	ddmm=cfgfl->getDateDdMm();
	bool	yyyyddmm=cfgfl->getDateYyyyDdMm();

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
	const char	*format=cfgfl->getDateTimeFormat();
	if (hour==-1) {
		format=cfgfl->getDateFormat();
	} else if (day==-1) {
		format=cfgfl->getTimeFormat();
	}

	// convert to the specified format
	delete[] reformattedfield;
	reformattedfield=convertDateTime(format,
					year,month,day,
					hour,minute,second,
					fraction);
	reformattedfieldlength=charstring::length(reformattedfield);

	if (debugsqltranslation) {
		stdoutput.printf("converted date: "
			"\"%s\" to \"%s\" using ddmm=%d\n",
			field,reformattedfield,ddmm);
	}

	// set return values
	*newfield=reformattedfield;
	*newfieldlength=reformattedfieldlength;
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

	} else if (conn->isTransactional() && commitorrollback) {

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
	if (dbselected) {
		// FIXME: we're ignoring the result and error,
		// should we do something if there's an error?
		conn->selectDatabase(originaldb);
		dbselected=false;
	}

	// reset autocommit behavior
	setAutoCommit(conn->autocommit);

	// set isolation level
	conn->setIsolationLevel(isolationlevel);

	// reset sql translation
	if (sqlt) {
		sqlt->endSession();
	}

	// shrink the cursor array, if necessary
	while (cursorcount>mincursorcount) {
		cursorcount--;
		deleteCursor(cur[cursorcount]);
		cur[cursorcount]=NULL;
	}

	// end the session
	conn->endSession();

	logDebugMessage("done ending session");
}

void sqlrcontroller_svr::dropTempTables(sqlrcursor_svr *cursor) {

	// some databases require us to re-login before dropping temp tables
	if (sessiontemptablesfordrop.getLength() &&
			conn->tempTableDropReLogIn()) {
		reLogIn();
	}

	// run through the temp table list, dropping tables
	for (stringlistnode *sln=sessiontemptablesfordrop.getFirstNode();
				sln; sln=(stringlistnode *)sln->getNext()) {
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
	if (sqltr) {
		if (sqlp->parse(dropquery.getString())) {
			sqltr->runBeforeTriggers(conn,cursor,sqlp->getTree());
		}
	}

	// kind of a kluge...
	// The cursor might already have a querytree associated with it and
	// if it does then executeQuery below might cause some triggers to
	// be run on that tree rather than on the tree for the drop query
	// we intend to run.
	delete cursor->querytree;
	cursor->querytree=NULL;

	if (cursor->prepareQuery(dropquery.getString(),
					dropquery.getStringLength())) {
		executeQuery(cursor,dropquery.getString(),
					dropquery.getStringLength());
	}
	cursor->cleanUpData();

	// FIXME: I need to refactor all of this so that this just gets
	// run as a matter of course instead of explicitly getting run here
	// FIXME: freetds/sybase override this method but don't do this
	if (sqltr) {
		sqltr->runAfterTriggers(conn,cursor,sqlp->getTree(),true);
	}
}

void sqlrcontroller_svr::truncateTempTables(sqlrcursor_svr *cursor) {

	// run through the temp table list, truncating tables
	for (stringlistnode *sln=sessiontemptablesfortrunc.getFirstNode();
				sln; sln=(stringlistnode *)sln->getNext()) {
		truncateTempTable(cursor,sln->getValue());
		delete[] sln->getValue();
	}
	sessiontemptablesfortrunc.clear();
}

void sqlrcontroller_svr::truncateTempTable(sqlrcursor_svr *cursor,
						const char *tablename) {
	stringbuffer	truncatequery;
	truncatequery.append("delete from ")->append(tablename);
	if (cursor->prepareQuery(truncatequery.getString(),
					truncatequery.getStringLength())) {
		executeQuery(cursor,truncatequery.getString(),
					truncatequery.getStringLength());
	}
	cursor->cleanUpData();
}

void sqlrcontroller_svr::closeClientSocket(uint32_t bytes) {

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
	if (proxymode) {

		logDebugMessage("(actually just signalling the listener)");

		// we do need to signal the proxy that it
		// needs to close the connection though
		signalmanager::sendSignal(proxypid,SIGUSR1);

		// in proxy mode, the client socket is pointed at the
		// handoff socket which we don't want to actually close
		clientsock->setFileDescriptor(-1);
	}
	clientsock->close();
	delete clientsock;
	logDebugMessage("done closing the client socket");
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

void sqlrcontroller_svr::closeConnection() {

	logDebugMessage("closing connection...");

	if (inclientsession) {
		endSession();
		decrementOpenClientConnections();
		inclientsession=false;
	}

	// decrement the connection counter
	if (decrementonclose && cfgfl->getDynamicScaling() &&
						semset && idmemory) {
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
				cur[cursorcount]->cleanUpData();
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
	idmemory=new sharedmemory();
	if (!idmemory->attach(file::generateKey(idfilename,1),
						sizeof(shmdata))) {
		char	*err=error::getErrorString();
		stderror.printf("Couldn't attach to shared memory segment: ");
		stderror.printf("%s\n",err);
		delete[] err;
		delete idmemory;
		idmemory=NULL;
		delete[] idfilename;
		return false;
	}
	shm=(shmdata *)idmemory->getPointer();
	if (!shm) {
		stderror.printf("failed to get pointer to shmdata\n");
		delete idmemory;
		idmemory=NULL;
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
		delete idmemory;
		semset=NULL;
		idmemory=NULL;
		delete[] idfilename;
		return false;
	}

	logDebugMessage("done attaching to shared memory and semaphores");

	delete[] idfilename;

	return true;
}

shmdata *sqlrcontroller_svr::getAnnounceBuffer() {
	return (shmdata *)idmemory->getPointer();
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

void sqlrcontroller_svr::acquireAnnounceMutex() {
	logDebugMessage("acquiring announce mutex");
	updateState(WAIT_SEMAPHORE);
	semset->waitWithUndo(0);
	logDebugMessage("done acquiring announce mutex");
}

void sqlrcontroller_svr::releaseAnnounceMutex() {
	logDebugMessage("releasing announce mutex");
	semset->signalWithUndo(0);
	logDebugMessage("done releasing announce mutex");
}

void sqlrcontroller_svr::signalListenerToRead() {
	logDebugMessage("signalling listener to read");
	semset->signal(2);
	logDebugMessage("done signalling listener to read");
}

void sqlrcontroller_svr::waitForListenerToFinishReading() {
	logDebugMessage("waiting for listener");
	semset->wait(3);
	// Reset this semaphore to 0.
	// It can get left incremented if another sqlr-connection is killed
	// between calls to signalListenerToRead() and this method.
	// It's ok to reset it here becuase no one except this process has
	// access to this semaphore at this time because of the lock on
	// AnnounceMutex (semaphore 0).
	semset->setValue(3,0);
	logDebugMessage("done waiting for listener");
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
	rawbuffer::zero(connstats,sizeof(struct sqlrconnstatistics));
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
	for (stringlistnode *node=
		cfgfl->getSessionStartQueries()->getFirstNode();
						node; node=node->getNext()) {
		sessionQuery(node->getValue());
	}
}

void sqlrcontroller_svr::sessionEndQueries() {
	// run a configurable set of queries at the end of each session
	for (stringlistnode *node=
		cfgfl->getSessionEndQueries()->getFirstNode();
						node; node=node->getNext()) {
		sessionQuery(node->getValue());
	}
}

void sqlrcontroller_svr::sessionQuery(const char *query) {

	// create the select database query
	size_t	querylen=charstring::length(query);

	sqlrcursor_svr	*cur=initCursor();

	// since we're creating a new cursor for this, make sure it
	// can't have an ID that might already exist
	if (cur->openInternal(cursorcount+1) &&
		cur->prepareQuery(query,querylen) &&
		executeQuery(cur,query,querylen)) {
		cur->cleanUpData();
	}
	cur->close();
	deleteCursor(cur);
}

const char *sqlrcontroller_svr::connectStringValue(const char *variable) {

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
	conn->autocommit=ac;
}

void sqlrcontroller_svr::setFakeTransactionBlocksBehavior(bool ftb) {
	faketransactionblocks=ftb;
}

void sqlrcontroller_svr::cleanUpAllCursorData() {
	logDebugMessage("cleaning up all cursors...");
	for (int32_t i=0; i<cursorcount; i++) {
		if (cur[i]) {
			cur[i]->cleanUpData();
		}
	}
	logDebugMessage("done cleaning up all cursors");
}

bool sqlrcontroller_svr::getColumnNames(const char *query,
					stringbuffer *output) {

	// sanity check on the query
	if (!query) {
		return false;
	}

	size_t		querylen=charstring::length(query);

	sqlrcursor_svr	*gcncur=initCursor();
	// since we're creating a new cursor for this, make sure it can't
	// have an ID that might already exist
	bool	retval=false;
	if (gcncur->openInternal(cursorcount+1) &&
		gcncur->prepareQuery(query,querylen) &&
		executeQuery(gcncur,query,querylen)) {

		// build column list...
		retval=gcncur->getColumnNameList(output);

	}
	gcncur->cleanUpData();
	gcncur->close();
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
			cursor->error);
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
