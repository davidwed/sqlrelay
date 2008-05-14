// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <config.h>
#include <sqlrconnection.h>

bool sqlrconnection_svr::authenticateCommand() {

	dbgfile.debugPrint("connection",1,"authenticate");

	if (!authenticate()) {
		// indicate that an error has occurred
		clientsock->write((uint16_t)ERROR);
		clientsock->write((uint16_t)21);
		clientsock->write("Authentication Error.");
		flushWriteBuffer();
		endSession();
		return false;
	}
	// indicate that no error has occurred
	clientsock->write((uint16_t)NO_ERROR);
	flushWriteBuffer();
	return true;
}

bool sqlrconnection_svr::authenticate() {

	dbgfile.debugPrint("connection",1,"authenticate...");

	// get the user/password from the client
	if (!getUserFromClient() || !getPasswordFromClient()) {
		return false;
	}

	// authenticate on the approprite tier
	bool	authondb=(cfgfl->getAuthOnDatabase() &&
				supportsAuthOnDatabase());
	bool	authonconnection=(cfgfl->getAuthOnConnection() ||
					(cfgfl->getAuthOnDatabase() &&
						!supportsAuthOnDatabase()));
	if (authonconnection) {
		return connectionBasedAuth(userbuffer,passwordbuffer);
	} else if (authondb) {
		return databaseBasedAuth(userbuffer,passwordbuffer);
	}

	dbgfile.debugPrint("connection",1,"authentication was done on listener");
	return true;
}

bool sqlrconnection_svr::getUserFromClient() {
	uint32_t	size=0;
	if (clientsock->read(&size,idleclienttimeout,0)==sizeof(uint32_t) &&
		size<=(uint32_t)USERSIZE &&
		(uint32_t)(clientsock->read(userbuffer,size,
						idleclienttimeout,0))==size) {
		userbuffer[size]=(char)NULL;
		return true;
	}
	dbgfile.debugPrint("connection",1,
		"authentication failed: user size is wrong");
	return false;
}

bool sqlrconnection_svr::getPasswordFromClient() {
	uint32_t size=0;
	if (clientsock->read(&size,idleclienttimeout,0)==sizeof(uint32_t) &&
		size<=(uint32_t)USERSIZE &&
		(uint32_t)(clientsock->read(passwordbuffer,size,
						idleclienttimeout,0))==size) {
		passwordbuffer[size]=(char)NULL;
		return true;
	}
	dbgfile.debugPrint("connection",1,
		"authentication failed: password size is wrong");
	return false;
}

bool sqlrconnection_svr::connectionBasedAuth(const char *userbuffer,
						const char *passwordbuffer) {

	// handle connection-based authentication
	int	retval=authc->authenticate(userbuffer,passwordbuffer);
	if (retval) {
		dbgfile.debugPrint("connection",1,
			"connection-based authentication succeeded");
	} else {
		dbgfile.debugPrint("connection",1,
			"connection-based authentication failed: invalid user/password");
	}
	return retval;
}

bool sqlrconnection_svr::databaseBasedAuth(const char *userbuffer,
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

	if (lastauthsuccess) {
		dbgfile.debugPrint("connection",1,
			"database-based authentication succeeded");
	} else {
		dbgfile.debugPrint("connection",1,
			"database-based authentication failed: invalid user/password");
	}
	return lastauthsuccess;
}

bool sqlrconnection_svr::supportsAuthOnDatabase() {
	return true;
}
