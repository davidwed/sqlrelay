// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrclient.h>
#include <defines.h>

int sqlrconnection::authenticateWithListener() {
	if (debug) {
		debugPreStart();
		debugPrint("Authenticating with listener : ");
	}
	return genericAuthentication();
}

int sqlrconnection::authenticateWithConnection() {

	write((unsigned short)AUTHENTICATE);
	if (debug) {
		debugPreStart();
		debugPrint("Authenticating with connection : ");
	}
	return genericAuthentication();
}

int sqlrconnection::genericAuthentication() {

	if (debug) {
		debugPrint(user);
		debugPrint(":");
		debugPrint(password);
		debugPrint("\n");
		debugPreEnd();
	}

	write((unsigned long)userlen);
	write(user,userlen);

	write((unsigned long)passwordlen);
	write(password,passwordlen);

	// check whether authentication was successful or not
	unsigned short	authsuccess;
	if (read(&authsuccess)!=sizeof(unsigned short)) {
		setError("Failed to authenticate.\n A network error may have ocurred.");
		return -1;
	}
	if (authsuccess==ERROR) {
		
		// clear all result sets
		sqlrcursor	*currentcursor=firstcursor;
		while (currentcursor) {
			currentcursor->clearResultSet();
			currentcursor=currentcursor->next;
		}

		setError("Authentication Error.");
		return 0;
	}
	if (debug) {
		debugPreStart();
		debugPrint("No authentication error.\n");
		debugPreEnd();
	}
	return 1;
}
