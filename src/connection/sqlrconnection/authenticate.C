// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <config.h>
#include <sqlrconnection.h>

#include <sys/types.h>
#include <unistd.h>

#include <string.h>
#ifdef HAVE_STRINGS_H
	#include <strings.h>
#endif

int	sqlrconnection::authenticateCommand() {

	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"authenticate");
	#endif

	if (!authenticate()) {
		// indicate that an error has occurred
		clientsock->write((unsigned short)ERROR);
		endSession();
		return 0;
	}
	// indicate that no error has occurred
	clientsock->write((unsigned short)NO_ERROR);
	return 1;
}

int	sqlrconnection::authenticate() {

	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"authenticate...");
	#endif

	// get the user/password from the client
	if (!getUserFromClient() || !getPasswordFromClient()) {
		return 0;
	}

	// authenticate on the approprite tier
	if (cfgfl->getAuthOnConnection()) {
		return connectionBasedAuth(userbuffer,passwordbuffer);
	} else if (cfgfl->getAuthOnDatabase()) {
		return databaseBasedAuth(userbuffer,passwordbuffer);
	}

	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"authentication was done on listener");
	#endif
	return 1;
}

int	sqlrconnection::getUserFromClient() {
	unsigned long size=0;
	clientsock->read(&size);
	if (size>(unsigned long)USERSIZE ||
		(unsigned long)(clientsock->read(userbuffer,size))!=size) {
		#ifdef SERVER_DEBUG
		debugPrint("connection",1,
			"authentication failed: user size is wrong");
		#endif
		return 0;
	}
	userbuffer[size]=(char)NULL;
	return 1;
}

int	sqlrconnection::getPasswordFromClient() {
	unsigned long size=0;
	clientsock->read(&size);
	if (size>(unsigned long)USERSIZE ||
		(unsigned long)(clientsock->read(passwordbuffer,size))!=size) {
		#ifdef SERVER_DEBUG
		debugPrint("connection",1,
			"authentication failed: password size is wrong");
		#endif
		return 0;
	}
	passwordbuffer[size]=(char)NULL;
	return 1;
}

int	sqlrconnection::connectionBasedAuth(const char *userbuffer,
						const char *passwordbuffer) {

	// handle connection-based authentication
	int	retval=authc->authenticate(userbuffer,passwordbuffer);
	#ifdef SERVER_DEBUG
	if (retval) {
		debugPrint("connection",1,
			"connection-based authentication succeeded");
	} else {
		debugPrint("connection",1,
			"connection-based authentication failed: invalid user/password");
	}
	#endif
	return retval;
}

int	sqlrconnection::databaseBasedAuth(const char *userbuffer,
						const char *passwordbuffer) {

	// if the user we want to change to is different from the
	// user that's currently proxied, try to change to that user
	int	authsuccess;
	if ((!lastuserbuffer[0] && !lastpasswordbuffer[0]) || 
		strcmp(lastuserbuffer,userbuffer) ||
		strcmp(lastpasswordbuffer,passwordbuffer)) {

		// change authentication 
		authsuccess=changeUser(userbuffer,passwordbuffer);

		// keep a record of which user we're changing to
		// and whether that user was successful in 
		// authenticating
		strcpy(lastuserbuffer,userbuffer);
		strcpy(lastpasswordbuffer,passwordbuffer);
		lastauthsuccess=authsuccess;
	}

	#ifdef SERVER_DEBUG
	if (lastauthsuccess) {
		debugPrint("connection",1,
			"database-based authentication succeeded");
	} else {
		debugPrint("connection",1,
			"database-based authentication failed: invalid user/password");
	}
	#endif
	return lastauthsuccess;
}
