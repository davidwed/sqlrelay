// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <config.h>
#include <sqlrconnection.h>

bool sqlrconnection::authenticateCommand() {

	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"authenticate");
	#endif

	if (!authenticate()) {
		// indicate that an error has occurred
		clientsock->write((unsigned short)ERROR);
		flushWriteBuffer();
		endSession();
		return false;
	}
	// indicate that no error has occurred
	clientsock->write((unsigned short)NO_ERROR);
	flushWriteBuffer();
	return true;
}

bool sqlrconnection::authenticate() {

	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"authenticate...");
	#endif

	// get the user/password from the client
	if (!getUserFromClient() || !getPasswordFromClient()) {
		return false;
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
	return true;
}

bool sqlrconnection::getUserFromClient() {
	unsigned long size=0;
	if (clientsock->read(&size)==sizeof(unsigned long) &&
		size<=(unsigned long)USERSIZE &&
		(unsigned long)(clientsock->read(userbuffer,size))==size) {
		userbuffer[size]=(char)NULL;
		return true;
	}
	#ifdef SERVER_DEBUG
	debugPrint("connection",1,
		"authentication failed: user size is wrong");
	#endif
	return false;
}

bool sqlrconnection::getPasswordFromClient() {
	unsigned long size=0;
	if (clientsock->read(&size)==sizeof(unsigned long) &&
		size<=(unsigned long)USERSIZE &&
		(unsigned long)(clientsock->read(passwordbuffer,size))==size) {
		passwordbuffer[size]=(char)NULL;
		return true;
	}
	#ifdef SERVER_DEBUG
	debugPrint("connection",1,
		"authentication failed: password size is wrong");
	#endif
	return false;
}

bool sqlrconnection::connectionBasedAuth(const char *userbuffer,
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

bool sqlrconnection::databaseBasedAuth(const char *userbuffer,
						const char *passwordbuffer) {

	// if the user we want to change to is different from the
	// user that's currently proxied, try to change to that user
	bool	authsuccess;
	if ((!lastuserbuffer[0] && !lastpasswordbuffer[0]) || 
		charstring::compare(lastuserbuffer,userbuffer) ||
		charstring::compare(lastpasswordbuffer,passwordbuffer)) {

		// change authentication 
		authsuccess=changeUser(userbuffer,passwordbuffer);

		// keep a record of which user we're changing to
		// and whether that user was successful in 
		// authenticating
		charstring::copy(lastuserbuffer,userbuffer);
		charstring::copy(lastpasswordbuffer,passwordbuffer);
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
