// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrclient.h>

const char *sqlrconnection::errorMessage() {
	return error;
}

void sqlrconnection::clearError() {
	delete[] error;
	error=NULL;
}

void sqlrconnection::setError(const char *err) {

	if (debug) {
		debugPreStart();
		debugPrint("Setting Error\n");
		debugPreEnd();
	}

	error=charstring::duplicate(err);

	if (debug) {
		debugPreStart();
		debugPrint(error);
		debugPrint("\n");
		debugPreEnd();
	}
}

bool sqlrconnection::getError() {

	clearError();

	// get the error size
	bool		goterror=true;
	uint16_t	size;
	if (cs->read(&size)!=sizeof(uint16_t)) {
		return false;
	}

	// get the error string
	error=new char[size+1];
	if (cs->read(error,size)!=size) {
		delete[] error;
		error=NULL;
		return false;
	}
	error[size]='\0';
	return true;
}
