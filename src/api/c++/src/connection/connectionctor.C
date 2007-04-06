// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrclient.h>

sqlrconnection::sqlrconnection(const char *server, uint16_t port,
					const char *socket,
					const char *user, const char *password, 
					int32_t retrytime, int32_t tries) {

	// initialize...
	setTimeout(-1,-1);

	// retry reads if they get interrupted by signals
	ucs.translateByteOrder();
	ucs.retryInterruptedReads();
	ics.retryInterruptedReads();
	cs=&ucs;

	// connection
	this->server=(char *)server;
	listenerinetport=port;
	listenerunixport=(char *)socket;
	this->retrytime=retrytime;
	this->tries=tries;

	// authentication
	this->user=(char *)user;
	this->password=(char *)password;
	userlen=charstring::length(user);
	passwordlen=charstring::length(password);
	reconnect=false;

	// database id
	id=NULL;

	// db version
	dbversion=NULL;

	// session state
	connected=false;
	clearSessionFlags();

	// debug print function
	printfunction=NULL;

	// debug off
	debug=false;
	webdebug=-1;

	// copy references, delete cursors flags
	copyrefs=false;

	// error string
	error=NULL;

	// cursor list
	firstcursor=NULL;
	lastcursor=NULL;
}

void sqlrconnection::setTimeout(int32_t timeoutsec, int32_t timeoutusec) {
	this->timeoutsec=timeoutsec;
	this->timeoutusec=timeoutusec;
}
