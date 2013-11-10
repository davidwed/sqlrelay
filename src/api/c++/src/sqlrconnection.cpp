// Copyright (c) 1999-2013  David Muse
// See the file COPYING for more information

#include <config.h>
#include <sqlrelay/sqlrclient.h>
#include <rudiments/environment.h>
#include <rudiments/charstring.h>
#include <rudiments/stdio.h>
#include <rudiments/error.h>
#include <rudiments/permissions.h>

#include <defines.h>

sqlrconnection::sqlrconnection(const char *server, uint16_t port,
					const char *socket,
					const char *user, const char *password, 
					int32_t retrytime, int32_t tries,
					bool copyreferences) {
	init(server,port,socket,user,password,retrytime,tries,copyreferences);
}

sqlrconnection::sqlrconnection(const char *server, uint16_t port,
					const char *socket,
					const char *user, const char *password, 
					int32_t retrytime, int32_t tries) {
	init(server,port,socket,user,password,retrytime,tries,false);
}

void sqlrconnection::init(const char *server, uint16_t port,
					const char *socket,
					const char *user, const char *password, 
					int32_t retrytime, int32_t tries,
					bool copyreferences) {

	copyrefs=copyreferences;

	// retry reads if they get interrupted by signals
	ucs.translateByteOrder();
	ucs.retryInterruptedReads();
	ics.retryInterruptedReads();
	cs=&ucs;

	// connection
	this->server=(copyrefs)?
			charstring::duplicate(server):
			(char *)server;
	listenerinetport=port;
	listenerunixport=(copyrefs)?
				charstring::duplicate(socket):
				(char *)socket;
	this->retrytime=retrytime;
	this->tries=tries;

	// initialize timeouts
	setTimeoutFromEnv("SQLR_CLIENT_CONNECT_TIMEOUT",
				&connecttimeoutsec,&connecttimeoutusec);
	setTimeoutFromEnv("SQLR_CLIENT_AUTHENTICATION_TIMEOUT",
				&authtimeoutsec,&authtimeoutusec);
	setTimeoutFromEnv("SQLR_CLIENT_RESPONSE_TIMEOUT",
				&responsetimeoutsec,&responsetimeoutusec);

	// authentication
	this->user=(copyrefs)?
			charstring::duplicate(user):
			(char *)user;
	this->password=(copyrefs)?
			charstring::duplicate(password):
			(char *)password;
	userlen=charstring::length(user);
	passwordlen=charstring::length(password);

	// database id
	id=NULL;

	// db version
	dbversion=NULL;

	// db host name
	dbhostname=NULL;

	// db ip address
	dbipaddress=NULL;

	// server version
	serverversion=NULL;

	// current database name
	currentdbname=NULL;

	// bind format
	bindformat=NULL;

	// client info
	clientinfo=NULL;
	clientinfolen=0;

	// session state
	connected=false;
	clearSessionFlags();

	// debug print function
	printfunction=NULL;

	// enable/disable debug
	const char	*sqlrdebug=environment::getValue("SQLRDEBUG");
	if (!sqlrdebug || !*sqlrdebug) {
		sqlrdebug=environment::getValue("SQLR_CLIENT_DEBUG");
	}
	const char	*yesset[]={"YES","ON","TRUE","Y","T","1",
					"yes","on","true","y","t",
					"Yes","On","True",NULL};
	const char	*noset[]={"NO","OFF","FALSE","N","F","0",
					"no","off","false","n","f",
					"No","Off","False",NULL};
	debug=(sqlrdebug && *sqlrdebug && !charstring::inSet(sqlrdebug,noset));
	if (debug && !charstring::inSet(sqlrdebug,yesset) &&
			!charstring::inSet(sqlrdebug,noset)) {
		setDebugFile(sqlrdebug);
	}
	webdebug=-1;

	// copy references, delete cursors flags
	copyrefs=false;

	// error
	errorno=0;
	error=NULL;

	// cursor list
	firstcursor=NULL;
	lastcursor=NULL;
}

void sqlrconnection::clearSessionFlags() {

	// indicate that the session hasn't been suspended or ended
	endsessionsent=false;
	suspendsessionsent=false;
}

sqlrconnection::~sqlrconnection() {

	// unless it was already ended or suspended, end the session
	if (!endsessionsent && !suspendsessionsent) {
		endSession();
	}

	// deallocate id
	delete[] id;

	// deallocate dbversion
	delete[] dbversion;

	// deallocate db host name
	delete[] dbhostname;

	// deallocate db ip address
	delete[] dbipaddress;

	// deallocate bindformat
	delete[] bindformat;

	// deallocate client info
	delete[] clientinfo;

	// deallocate copied references
	if (copyrefs) {
		delete[] server;
		delete[] listenerunixport;
		delete[] user;
		delete[] password;
	}

	// detach all cursors attached to this client
	sqlrcursor	*currentcursor=firstcursor;
	while (currentcursor) {
		firstcursor=currentcursor;
		currentcursor=currentcursor->next;
		firstcursor->sqlrc=NULL;
	}

	if (debug) {
		debugPreStart();
		debugPrint("Deallocated connection\n");
		debugPreEnd();
	}
}

void sqlrconnection::setConnectTimeout(int32_t timeoutsec,
					int32_t timeoutusec) {
	connecttimeoutsec=timeoutsec;
	connecttimeoutusec=timeoutusec;
}

void sqlrconnection::setAuthenticationTimeout(int32_t timeoutsec,
						int32_t timeoutusec) {
	authtimeoutsec=timeoutsec;
	authtimeoutusec=timeoutusec;
}

void sqlrconnection::setResponseTimeout(int32_t timeoutsec,
						int32_t timeoutusec) {
	responsetimeoutsec=timeoutsec;
	responsetimeoutusec=timeoutusec;
}

void sqlrconnection::setTimeoutFromEnv(const char *var,
					int32_t *timeoutsec,
					int32_t *timeoutusec) {
	const char	*timeout=environment::getValue(var);
	if (charstring::isNumber(timeout)) {
		*timeoutsec=charstring::toInteger(timeout);
		long double	dbl=charstring::toFloat(timeout);
		dbl=dbl-(long double)(*timeoutsec);
		*timeoutusec=(int32_t)(dbl*1000000.0);
	} else {
		*timeoutsec=-1;
		*timeoutusec=-1;
	}
}

void sqlrconnection::endSession() {

	if (debug) {
		debugPreStart();
		debugPrint("Ending Session\n");
		debugPreEnd();
	}

	// abort each cursor's result set
	sqlrcursor	*currentcursor=firstcursor;
	while (currentcursor) {
		// FIXME: do we need to clearResultSet() here too?
		if (!currentcursor->endofresultset) {
			currentcursor->closeResultSet(false);
		}
		currentcursor->havecursorid=false;
		currentcursor=currentcursor->next;
	}

	// write an END_SESSION to the connection
	if (connected) {
		cs->write((uint16_t)END_SESSION);
		flushWriteBuffer();
		endsessionsent=true;
		closeConnection();
	}
}

void sqlrconnection::flushWriteBuffer() {
	cs->flushWriteBuffer(-1,-1);
}

void sqlrconnection::closeConnection() {
	cs->close();
	connected=false;
}

bool sqlrconnection::suspendSession() {

	if (!openSession()) {
		return 0;
	}

	clearError();

	if (debug) {
		debugPreStart();
		debugPrint("Suspending Session\n");
		debugPreEnd();
	}

	// suspend the session
	cs->write((uint16_t)SUSPEND_SESSION);
	flushWriteBuffer();
	suspendsessionsent=true;

	// check for error
	if (gotError()) {
		return false;
	}

	// get port to resume on
	bool	retval=getNewPort();

	closeConnection();

	return retval;
}

bool sqlrconnection::openSession() {

	if (connected) {
		return true;
	}

	if (debug) {
		debugPreStart();
		debugPrint("Connecting to listener...");
		debugPrint("\n");
		debugPreEnd();
	}

	// open a connection to the listener
	int	openresult=RESULT_ERROR;

	// first, try for a unix connection
	if (listenerunixport && listenerunixport[0]) {

		if (debug) {
			debugPreStart();
			debugPrint("Unix socket: ");
			debugPrint(listenerunixport);
			debugPrint("\n");
			debugPreEnd();
		}

		openresult=ucs.connect(listenerunixport,
						connecttimeoutsec,
						connecttimeoutusec,
						retrytime,tries);
		if (openresult==RESULT_SUCCESS) {
			cs=&ucs;
		}
	}

	// then try for an inet connection
	if (openresult!=RESULT_SUCCESS && listenerinetport) {

		if (debug) {
			debugPreStart();
			debugPrint("Inet socket: ");
			debugPrint(server);
			debugPrint(":");
			debugPrint((int64_t)listenerinetport);
			debugPrint("\n");
			debugPreEnd();
		}

		openresult=ics.connect(server,listenerinetport,
						connecttimeoutsec,
						connecttimeoutusec,
						retrytime,tries);
		if (openresult==RESULT_SUCCESS) {
			cs=&ics;
		}
	}

	// handle failure to connect to listener
	if (openresult!=RESULT_SUCCESS) {
		setError("Couldn't connect to the listener.");
		return false;
	}

	// use 8k read and write buffers
	cs->dontUseNaglesAlgorithm();
	// FIXME: make these buffer sizes user-configurable
	// SO_SNDBUF=0 causes no data to ever be sent on openbsd
	//cs->setTcpReadBufferSize(8192);
	//cs->setTcpWriteBufferSize(0);
	cs->setReadBufferSize(8192);
	cs->setWriteBufferSize(8192);

	// send protocol info
	protocol();

	// authenticate
	authenticate();

	// if we made it here then everything went well and we are successfully
	// connected and authenticated with the connection daemon
	connected=true;
	return true;
}

void sqlrconnection::protocol() {

	if (debug) {
		debugPreStart();
		debugPrint("Protocol : SQLRCLIENT_PROTOCOL 1\n");
		debugPreEnd();
	}

	cs->write((uint16_t)SQLRCLIENT_PROTOCOL);
	cs->write((uint16_t)1);
}

void sqlrconnection::authenticate() {

	if (debug) {
		debugPreStart();
		debugPrint("Authenticating : ");
		debugPrint(user);
		debugPrint(":");
		debugPrint(password);
		debugPrint("\n");
		debugPreEnd();
	}

	cs->write((uint16_t)AUTHENTICATE);

	cs->write(userlen);
	cs->write(user,userlen);

	cs->write(passwordlen);
	cs->write(password,passwordlen);

	flushWriteBuffer();
}

bool sqlrconnection::getNewPort() {

	// get the size of the unix port string
	uint16_t	size;
	if (cs->read(&size)!=sizeof(uint16_t)) {
		setError("Failed to get the size of the unix connection port.\n A network error may have ocurred.");
		return false;
	}
	
	if (size>MAXPATHLEN) {

		// if size is too big, return an error
		stringbuffer	errstr;
		errstr.append("The pathname of the unix port was too long: ");
		errstr.append(size);
		errstr.append(" bytes.  The maximum size is ");
		errstr.append((uint16_t)MAXPATHLEN);
		errstr.append(" bytes.");
		setError(errstr.getString());
		return false;
	}

	// get the unix port string
	if (size && cs->read(connectionunixportbuffer,size)!=size) {
		setError("Failed to get the unix connection port.\n A network error may have ocurred.");
		return false;
	}
	connectionunixportbuffer[size]='\0';
	connectionunixport=connectionunixportbuffer;

	// get the inet port
	if (cs->read(&connectioninetport)!=sizeof(uint16_t)) {
		setError("Failed to get the inet connection port.\n A network error may have ocurred.");
		return false;
	}

	// the server will send 0 for both the size of the unixport and 
	// the inet port if a server error occurred
	if (!size && !connectioninetport) {
		setError("An error occurred on the server.");
		return false;
	}
	return true;
}

uint16_t sqlrconnection::getConnectionPort() {

	if (!suspendsessionsent && !openSession()) {
		return 0;
	}

	if (debug) {
		debugPreStart();
		debugPrint("Getting connection port: ");
		debugPrint((int64_t)connectioninetport);
		debugPrint("\n");
		debugPreEnd();
	}

	return connectioninetport;
}

const char *sqlrconnection::getConnectionSocket() {

	if (!suspendsessionsent && !openSession()) {
		return NULL;
	}

	if (debug) {
		debugPreStart();
		debugPrint("Getting connection socket: ");
		if (connectionunixport) {
			debugPrint(connectionunixport);
		}
		debugPrint("\n");
		debugPreEnd();
	}

	if (connectionunixport) {
		return connectionunixport;
	}
	return NULL;
}

bool sqlrconnection::resumeSession(uint16_t port, const char *socket) {

	// if already connected, end the session
	if (connected) {
		endSession();
	}

	// set the connectionunixport and connectioninetport values
	if (copyrefs) {
		if (charstring::length(socket)<=MAXPATHLEN) {
			charstring::copy(connectionunixportbuffer,socket);
			connectionunixport=connectionunixportbuffer;
		} else {
			connectionunixport="";
		}
	} else {
		connectionunixport=(char *)socket;
	}
	connectioninetport=port;

	// first, try for the unix port
	if (socket && socket[0]) {
		connected=(ucs.connect(socket,-1,-1,
					retrytime,tries)==RESULT_SUCCESS);
		if (connected) {
			cs=&ucs;
		}
	}

	// then try for the inet port
	if (connected!=RESULT_SUCCESS) {
		connected=(ics.connect(server,port,-1,-1,
					retrytime,tries)==RESULT_SUCCESS);
		if (connected) {
			cs=&ics;
		}
	}

	if (debug) {
		debugPreStart();
		debugPrint("Resuming Session: ");
		debugPreEnd();
	}

	if (connected) {

		// use 8k read and write buffers
		cs->dontUseNaglesAlgorithm();
		// FIXME: use bandwidth delay product to tune these
		// SO_SNDBUF=0 causes no data to ever be sent on openbsd
		//cs->setTcpReadBufferSize(8192);
		//cs->setTcpWriteBufferSize(0);
		cs->setReadBufferSize(8192);
		cs->setWriteBufferSize(8192);

		// send protocol info
		protocol();

		if (debug) {
			debugPreStart();
			debugPrint("success");
			debugPrint("\n");
			debugPreEnd();
		}
		clearSessionFlags();
	} else {
		if (debug) {
			debugPreStart();
			debugPrint("failure");
			debugPrint("\n");
			debugPreEnd();
		}
	}

	return connected;
}

bool sqlrconnection::ping() {

	if (!openSession()) {
		return 0;
	}

	clearError();

	if (debug) {
		debugPreStart();
		debugPrint("Pinging...\n");
		debugPreEnd();
	}

	cs->write((uint16_t)PING);
	flushWriteBuffer();

	return !gotError();
}

const char *sqlrconnection::identify() {

	if (!openSession()) {
		return NULL;
	}

	clearError();

	if (debug) {
		debugPreStart();
		debugPrint("Identifying...\n");
		debugPreEnd();
	}

	// tell the server we want the identity of the db
	cs->write((uint16_t)IDENTIFY);
	flushWriteBuffer();

	if (gotError()) {
		return NULL;
	}

	// get the identity size
	uint16_t	size;
	if (cs->read(&size,responsetimeoutsec,
				responsetimeoutusec)!=sizeof(uint16_t)) {
		setError("Failed to identify.\n "
			"A network error may have ocurred.");
		return NULL;
	}

	// get the identity
	delete[] id;
	id=new char[size+1];
	if (cs->read(id,size)!=size) {
		setError("Failed to identify.\n "
			"A network error may have ocurred.");
		delete[] id;
		id=NULL;
		return NULL;
	}
	id[size]='\0';

	if (debug) {
		debugPreStart();
		debugPrint(id);
		debugPrint("\n");
		debugPreEnd();
	}
	return id;
}

const char *sqlrconnection::dbVersion() {

	if (!openSession()) {
		return NULL;
	}

	clearError();

	if (debug) {
		debugPreStart();
		debugPrint("DB Version...");
		debugPrint("\n");
		debugPreEnd();
	}

	// tell the server we want the db version
	cs->write((uint16_t)DBVERSION);
	flushWriteBuffer();

	if (gotError()) {
		return NULL;
	}

	// get the db version size
	uint16_t	size;
	if (cs->read(&size,responsetimeoutsec,
				responsetimeoutusec)!=sizeof(uint16_t)) {
		setError("Failed to get DB version.\n "
			"A network error may have ocurred.");
		return NULL;
	} 

	// get the db version
	delete[] dbversion;
	dbversion=new char[size+1];
	if (cs->read(dbversion,size)!=size) {
		setError("Failed to get DB version.\n "
			"A network error may have ocurred.");
		delete[] dbversion;
		dbversion=NULL;
		return NULL;
	}
	dbversion[size]='\0';

	if (debug) {
		debugPreStart();
		debugPrint(dbversion);
		debugPrint("\n");
		debugPreEnd();
	}
	return dbversion;
}

const char *sqlrconnection::dbHostName() {

	if (!openSession()) {
		return NULL;
	}

	clearError();

	if (debug) {
		debugPreStart();
		debugPrint("DB Host Name...");
		debugPrint("\n");
		debugPreEnd();
	}

	// tell the server we want the db host name
	cs->write((uint16_t)DBHOSTNAME);
	flushWriteBuffer();

	if (gotError()) {
		return NULL;
	}

	// get the db host name size
	uint16_t	size;
	if (cs->read(&size,responsetimeoutsec,
				responsetimeoutusec)!=sizeof(uint16_t)) {
		setError("Failed to get DB host name.\n "
				"A network error may have ocurred.");
		return NULL;
	}

	// get the db host name
	delete[] dbhostname;
	dbhostname=new char[size+1];
	if (cs->read(dbhostname,size)!=size) {
		setError("Failed to get DB host name.\n "
				"A network error may have ocurred.");
		delete[] dbhostname;
		dbhostname=NULL;
		return NULL;
	}
	dbhostname[size]='\0';

	if (debug) {
		debugPreStart();
		debugPrint(dbhostname);
		debugPrint("\n");
		debugPreEnd();
	}
	return dbhostname;
}

const char *sqlrconnection::dbIpAddress() {

	if (!openSession()) {
		return NULL;
	}

	clearError();

	if (debug) {
		debugPreStart();
		debugPrint("DB Ip Address...");
		debugPrint("\n");
		debugPreEnd();
	}

	// tell the server we want the db ip address
	cs->write((uint16_t)DBIPADDRESS);
	flushWriteBuffer();

	if (gotError()) {
		return NULL;
	}

	// get the db ip address size
	uint16_t	size;
	if (cs->read(&size,responsetimeoutsec,
				responsetimeoutusec)!=sizeof(uint16_t)) {
		setError("Failed to get DB ip address.\n "
				"A network error may have ocurred.");
		return NULL;
	}

	// get the db ip address
	delete[] dbipaddress;
	dbipaddress=new char[size+1];
	if (cs->read(dbipaddress,size)!=size) {
		setError("Failed to get DB ip address.\n "
				"A network error may have ocurred.");
		delete[] dbipaddress;
		dbipaddress=NULL;
		return NULL;
	}
	dbipaddress[size]='\0';

	if (debug) {
		debugPreStart();
		debugPrint(dbipaddress);
		debugPrint("\n");
		debugPreEnd();
	}
	return dbipaddress;
}

const char *sqlrconnection::serverVersion() {

	if (!openSession()) {
		return NULL;
	}

	clearError();

	if (debug) {
		debugPreStart();
		debugPrint("Server Version...");
		debugPrint("\n");
		debugPreEnd();
	}

	// tell the server we want the server version
	cs->write((uint16_t)SERVERVERSION);
	flushWriteBuffer();

	if (gotError()) {
		return NULL;
	}

	// get the server version size
	uint16_t	size;
	if (cs->read(&size,responsetimeoutsec,
				responsetimeoutusec)!=sizeof(uint16_t)) {
		setError("Failed to get Server version.\n "
			"A network error may have ocurred.");
		return NULL;
	}

	// get the server version
	delete[] serverversion;
	serverversion=new char[size+1];
	if (cs->read(serverversion,size)!=size) {
		setError("Failed to get Server version.\n "
			"A network error may have ocurred.");
		delete[] serverversion;
		serverversion=NULL;
		return NULL;
	}
	serverversion[size]='\0';

	if (debug) {
		debugPreStart();
		debugPrint(serverversion);
		debugPrint("\n");
		debugPreEnd();
	}
	return serverversion;
}

const char *sqlrconnection::clientVersion() {
	return SQLR_VERSION;
}

const char *sqlrconnection::bindFormat() {

	if (!openSession()) {
		return NULL;
	}

	clearError();

	if (debug) {
		debugPreStart();
		debugPrint("bind format...");
		debugPrint("\n");
		debugPreEnd();
	}

	// tell the server we want the bind format
	cs->write((uint16_t)BINDFORMAT);
	flushWriteBuffer();

	if (gotError()) {
		return NULL;
	}


	// get the bindformat size
	uint16_t	size;
	if (cs->read(&size,responsetimeoutsec,
				responsetimeoutusec)!=sizeof(uint16_t)) {
		setError("Failed to get bind format.\n"
			" A network error may have ocurred.");
		return NULL;
	}

	// get the bindformat
	delete[] bindformat;
	bindformat=new char[size+1];
	if (cs->read(bindformat,size)!=size) {
		setError("Failed to get bind format.\n "
			"A network error may have ocurred.");
		delete[] bindformat;
		bindformat=NULL;
		return NULL;
	}
	bindformat[size]='\0';

	if (debug) {
		debugPreStart();
		debugPrint(bindformat);
		debugPrint("\n");
		debugPreEnd();
	}
	return bindformat;
}

bool sqlrconnection::selectDatabase(const char *database) {

	if (!charstring::length(database)) {
		return true;
	}

	clearError();

	if (!openSession()) {
		return 0;
	}

	if (debug) {
		debugPreStart();
		debugPrint("Selecting database ");
		debugPrint(database);
		debugPrint("...\n");
		debugPreEnd();
	}

	// tell the server we want to select a db
	cs->write((uint16_t)SELECT_DATABASE);

	// send the database name
	uint32_t	len=charstring::length(database);
	cs->write(len);
	if (len) {
		cs->write(database,len);
	}
	flushWriteBuffer();

	return !gotError();
}

const char *sqlrconnection::getCurrentDatabase() {

	if (!openSession()) {
		return NULL;
	}

	clearError();

	if (debug) {
		debugPreStart();
		debugPrint("Getting the current database...\n");
		debugPreEnd();
	}

	clearError();

	// tell the server we want to select a db
	cs->write((uint16_t)GET_CURRENT_DATABASE);
	flushWriteBuffer();

	if (gotError()) {
		return NULL;
	}

	// get the current db name size
	uint16_t	size;
	if (cs->read(&size,responsetimeoutsec,
				responsetimeoutusec)!=sizeof(uint16_t)) {
		setError("Failed to get the current database.\n "
				"A network error may have ocurred.");
		return NULL;
	}

	// get the current db name
	delete[] currentdbname;
	currentdbname=new char[size+1];
	if (cs->read(currentdbname,size)!=size) {
		setError("Failed to get the current database.\n "
				"A network error may have ocurred.");
		delete[] currentdbname;
		currentdbname=NULL;
		return NULL;
	}
	currentdbname[size]='\0';

	if (debug) {
		debugPreStart();
		debugPrint(currentdbname);
		debugPrint("\n");
		debugPreEnd();
	}
	return currentdbname;
}

uint64_t sqlrconnection::getLastInsertId() {

	if (!openSession()) {
		return 0;
	}

	clearError();

	if (debug) {
		debugPreStart();
		debugPrint("Getting the last insert id...\n");
		debugPreEnd();
	}

	// tell the server we want the last insert id
	cs->write((uint16_t)GET_LAST_INSERT_ID);
	flushWriteBuffer();

	if (gotError()) {
		return 0;
	}

	// get the last insert id
	uint64_t	id=0;
	if (cs->read(&id)!=sizeof(uint64_t)) {
		setError("Failed to get the last insert id.\n"
				"A network error may have ocurred.");
		return 0;
	}

	if (debug) {
		debugPreStart();
		debugPrint("Got the last insert id: ");
		debugPrint((int64_t)id);
		debugPrint("\n");
		debugPreEnd();
	}
	return id;
}

bool sqlrconnection::autoCommitOn() {
	return autoCommit(true);
}

bool sqlrconnection::autoCommitOff() {
	return autoCommit(false);
}

bool sqlrconnection::autoCommit(bool on) {

	if (!openSession()) {
		return false;
	}

	clearError();

	if (debug) {
		debugPreStart();
		debugPrint((on)?"Setting autocommit on\n":
				"Setting autocommit off\n");
		debugPreEnd();
	}

	cs->write((uint16_t)AUTOCOMMIT);
	cs->write(on);
	flushWriteBuffer();

	return !gotError();
}

bool sqlrconnection::begin() {

	if (!openSession()) {
		return false;
	}

	clearError();

	if (debug) {
		debugPreStart();
		debugPrint("Beginning...\n");
		debugPreEnd();
	}

	cs->write((uint16_t)BEGIN);
	flushWriteBuffer();

	return !gotError();
}

bool sqlrconnection::commit() {

	if (!openSession()) {
		return false;
	}

	clearError();

	if (debug) {
		debugPreStart();
		debugPrint("Committing...\n");
		debugPreEnd();
	}

	cs->write((uint16_t)COMMIT);
	flushWriteBuffer();

	return !gotError();
}

bool sqlrconnection::rollback() {

	if (!openSession()) {
		return false;
	}

	clearError();

	if (debug) {
		debugPreStart();
		debugPrint("Rolling Back...\n");
		debugPreEnd();
	}

	cs->write((uint16_t)ROLLBACK);
	flushWriteBuffer();

	return !gotError();
}

const char *sqlrconnection::errorMessage() {
	return error;
}

int64_t sqlrconnection::errorNumber() {
	return errorno;
}

void sqlrconnection::clearError() {
	delete[] error;
	error=NULL;
	errorno=0;
}

void sqlrconnection::setError(const char *err) {

	if (debug) {
		debugPreStart();
		debugPrint("Setting Error\n");
		debugPreEnd();
	}

	delete[] error;
	error=charstring::duplicate(err);

	if (debug) {
		debugPreStart();
		debugPrint(error);
		debugPrint("\n");
		debugPreEnd();
	}
}

uint16_t sqlrconnection::getError() {

	clearError();

	if (debug) {
		debugPreStart();
		debugPrint("Checking for error\n");
		debugPreEnd();
	}

	// get whether an error occurred or not
	uint16_t	status;
	if (cs->read(&status,responsetimeoutsec,
				responsetimeoutusec)!=sizeof(uint16_t)) {
		setError("Failed to get the error status.\n"
				"A network error may have ocurred.");
		return ERROR_OCCURRED;
	}

	// if no error occurred, return that
	if (status==NO_ERROR_OCCURRED) {
		if (debug) {
			debugPreStart();
			debugPrint("No error occurred\n");
			debugPreEnd();
		}
		return status;
	}

	if (debug) {
		debugPreStart();
		debugPrint("An error occurred\n");
		debugPreEnd();
	}

	// get the error code
	if (cs->read((uint64_t *)&errorno)!=sizeof(uint64_t)) {
		setError("Failed to get the error code.\n"
				"A network error may have ocurred.");
		return status;
	}

	// get the error size
	uint16_t	size;
	if (cs->read(&size)!=sizeof(uint16_t)) {
		setError("Failed to get the error size.\n"
				"A network error may have ocurred.");
		return status;
	}

	// get the error string
	error=new char[size+1];
	if (cs->read(error,size)!=size) {
		setError("Failed to get the error string.\n"
				"A network error may have ocurred.");
		return status;
	}
	error[size]='\0';

	if (debug) {
		debugPreStart();
		debugPrint("Got error:\n");
		debugPrint(errorno);
		debugPrint(": ");
		debugPrint(error);
		debugPrint("\n");
		debugPreEnd();
	}
	return status;
}

bool sqlrconnection::gotError() {
	uint16_t	status=getError();
	if (status==NO_ERROR_OCCURRED) {
		return false;
	}
	if (status==ERROR_OCCURRED_DISCONNECT) {
		endSession();
	}
	return true;
}

void sqlrconnection::debugOn() {
	debug=true;
}

void sqlrconnection::debugOff() {
	debug=false;
}

void sqlrconnection::setDebugFile(const char *filename) {
	debugfile.close();
	error::clearError();
	if (filename && *filename &&
		!debugfile.open(filename,O_WRONLY|O_APPEND) &&
				error::getErrorNumber()==ENOENT) {
		debugfile.create(filename,
				permissions::evalPermString("rw-r--r--"));
	}
}

bool sqlrconnection::getDebug() {
	return debug;
}

void sqlrconnection::debugPreStart() {
	if (webdebug==-1) {
		const char	*docroot=environment::getValue("DOCUMENT_ROOT");
		if (docroot && docroot[0]) {
			webdebug=1;
		} else {
			webdebug=0;
		}
	}
	if (webdebug==1) {
		debugPrint("<pre>\n");
	}
}

void sqlrconnection::debugPreEnd() {
	if (webdebug==1) {
		debugPrint("</pre>\n");
	}
}

void sqlrconnection::debugPrintFunction(
				int (*printfunction)(const char *,...)) {
	this->printfunction=printfunction;
}

void sqlrconnection::debugPrint(const char *string) {
	if (printfunction) {
		(*printfunction)("%s",string);
	} else if (debugfile.getFileDescriptor()!=-1) {
		debugfile.printf("%s",string);
	} else {
		stdoutput.printf("%s",string);
	}
}

void sqlrconnection::debugPrint(int64_t number) {
	if (printfunction) {
		(*printfunction)("%lld",(long long)number);
	} else if (debugfile.getFileDescriptor()!=-1) {
		debugfile.printf("%lld",(long long)number);
	} else {
		stdoutput.printf("%lld",(long long)number);
	}
}

void sqlrconnection::debugPrint(double number) {
	if (printfunction) {
		(*printfunction)("%f",number);
	} else if (debugfile.getFileDescriptor()!=-1) {
		debugfile.printf("%f",number);
	} else {
		stdoutput.printf("%f",number);
	}
}

void sqlrconnection::debugPrint(char character) {
	if (printfunction) {
		(*printfunction)("%c",character);
	} else if (debugfile.getFileDescriptor()!=-1) {
		debugfile.printf("%c",character);
	} else {
		stdoutput.printf("%c",character);
	}
}

void sqlrconnection::debugPrintBlob(const char *blob, uint32_t length) {
	debugPrint('\n');
	int	column=0;
	for (uint32_t i=0; i<length; i++) {
		if (blob[i]>=' ' && blob[i]<='~') {
			debugPrint(blob[i]);
		} else {
			debugPrint('.');
		}
		column++;
		if (column==80) {
			debugPrint('\n');
			column=0;
		}
	}
	debugPrint('\n');
}

void sqlrconnection::debugPrintClob(const char *clob, uint32_t length) {
	debugPrint('\n');
	for (uint32_t i=0; i<length; i++) {
		if (clob[i]=='\0') {
			debugPrint("\\0");
		} else {
			debugPrint(clob[i]);
		}
	}
	debugPrint('\n');
}

void sqlrconnection::setClientInfo(const char *clientinfo) {
	delete[] this->clientinfo;
	this->clientinfo=charstring::duplicate(clientinfo);
	clientinfolen=charstring::length(clientinfo);
}

const char *sqlrconnection::getClientInfo() const {
	return clientinfo;
}
