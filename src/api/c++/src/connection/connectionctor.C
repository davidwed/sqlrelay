// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrclient.h>
#include <string.h>

sqlrconnection::sqlrconnection(const char *server, int port, const char *socket,
					const char *user, const char *password, 
					int retrytime, int tries) 
: inetclientsocket(), unixclientsocket() {

	// initialize...

	// retry reads if they get interrupted by signals
	retryInterruptedReads();

	// connection
	this->server=(char *)server;
	listenerinetport=(unsigned short)port;
	listenerunixport=(char *)socket;
	this->retrytime=retrytime;
	this->tries=tries;

	// authentication
	this->user=(char *)user;
	this->password=(char *)password;
	userlen=strlen(user);
	passwordlen=strlen(password);
	reconnect=false;

	// database id
	id=NULL;

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
