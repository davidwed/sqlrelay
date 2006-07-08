// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrclient.h>
#include <defines.h>

bool sqlrconnection::authenticateWithListener() {
	if (debug) {
		debugPreStart();
		debugPrint("Authenticating with listener : ");
	}
	return genericAuthentication();
}

bool sqlrconnection::authenticateWithConnection() {

	cs->write((uint16_t)AUTHENTICATE);
	if (debug) {
		debugPreStart();
		debugPrint("Authenticating with connection : ");
	}
	return genericAuthentication();
}

bool sqlrconnection::genericAuthentication() {

	if (debug) {
		debugPrint(user);
		debugPrint(":");
		debugPrint(password);
		debugPrint("\n");
		debugPreEnd();
	}

	cs->write(userlen);
	cs->write(user,userlen);

	cs->write(passwordlen);
	cs->write(password,passwordlen);

	flushWriteBuffer();

	if (debug) {
		debugPreStart();
		debugPrint("Waiting for auth success/failure...\n");
		debugPreEnd();
	}

	// check whether authentication was successful or not
	uint16_t	authsuccess;
	if (cs->read(&authsuccess)!=sizeof(uint16_t)) {
		setError("Failed to authenticate.\n A network error may have ocurred.");
		return false;
	}
	if (authsuccess==ERROR) {

		// get the error or set a generic error if we can't get one
		bool		goterror=true;
		char		*err=NULL;
		uint16_t	size;
		if (cs->read(&size)!=sizeof(uint16_t)) {
			goterror=false;
		} else {
			err=new char[size+1];
			if (cs->read(err,size)!=size) {
				delete[] err;
				goterror=false;
			}
			err[size]='\0';
		}
		if (!goterror) {
			err=charstring::duplicate("Authentication Error.");
		}
		
		// clear all result sets
		sqlrcursor	*currentcursor=firstcursor;
		while (currentcursor) {
			currentcursor->clearResultSet();
			currentcursor=currentcursor->next;
		}

		if (debug) {
			debugPreStart();
			debugPrint(err);
			debugPrint("\n");
			debugPreEnd();
		}

		setError(err);
		delete[] err;
		return false;
	}
	if (debug) {
		debugPreStart();
		debugPrint("No authentication error.\n");
		debugPreEnd();
	}
	return true;
}
