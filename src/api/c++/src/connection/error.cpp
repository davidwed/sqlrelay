// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrclient.h>
#include <defines.h>

int64_t sqlrconnection::errorNumber() {
	return errorno;
}

const char *sqlrconnection::errorMessage() {
	return error;
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
