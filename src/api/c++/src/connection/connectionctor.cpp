// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrclient.h>
#include <rudiments/environment.h>

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

	// initialize...
	setTimeout(-1,-1);

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

	// authentication
	this->user=(copyrefs)?
			charstring::duplicate(user):
			(char *)user;
	this->password=(copyrefs)?
			charstring::duplicate(password):
			(char *)password;
	userlen=charstring::length(user);
	passwordlen=charstring::length(password);
	reconnect=false;

	// database id
	id=NULL;

	// db version
	dbversion=NULL;

	// server version
	serverversion=NULL;

	// current database name
	currentdbname=NULL;

	// bind format
	bindformat=NULL;

	// session state
	connected=false;
	clearSessionFlags();

	// debug print function
	printfunction=NULL;

	// enable/disable debug
	const char	*sqlrdebug=environment::getValue("SQLRDEBUG");
	debug=(charstring::length(sqlrdebug) &&
			charstring::compareIgnoringCase(
				environment::getValue("SQLRDEBUG"),"OFF"));
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

void sqlrconnection::setTimeout(int32_t timeoutsec, int32_t timeoutusec) {
	this->timeoutsec=timeoutsec;
	this->timeoutusec=timeoutusec;
}
