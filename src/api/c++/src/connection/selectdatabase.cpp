// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrclient.h>
#include <defines.h>

bool sqlrconnection::selectDatabase(const char *database) {

	if (!charstring::length(database)) {
		return true;
	}

	if (!openSession()) {
		return 0;
	}

	if (debug) {
		debugPreStart();
		debugPrint("Selecting database ");
		debugPrint(database);
		debugPrint("...\n");
		debugPreEnd();
	}

	// tell the server we want to select a db
	cs->write((uint16_t)SELECT_DATABASE);

	// send the database name
	uint32_t	len=charstring::length(database);
	cs->write(len);
	if (len) {
		cs->write(database,len);
	}

	flushWriteBuffer();

	// get the result
	bool	result;
	if (cs->read(&result,responsetimeoutsec,
				responsetimeoutusec)!=sizeof(bool)) {
		debugPrint("Error selecting database.\n");
		setError("Failed to select database.\n "
				"A network error may have ocurred.");
		return false;
	}

	// if there was an error, get the error
	if (!result) {

		uint16_t	errorlength;
		cs->read(&errorlength);

		delete[] error;
		error=new char[errorlength+1];
		cs->read(error,errorlength);
		error[errorlength]='\0';

		if (debug) {
			debugPrint("Selecting database failed: ");
			debugPrint(error);
			debugPrint("\n");
		}

	} else if (debug) {
		debugPrint("Selecting database succeeded\n");
	}

	return result;
}

const char *sqlrconnection::getCurrentDatabase() {

	if (!openSession()) {
		return NULL;
	}

	if (debug) {
		debugPreStart();
		debugPrint("Getting the current database...\n");
		debugPreEnd();
	}

	clearError();

	// tell the server we want to select a db
	cs->write((uint16_t)GET_CURRENT_DATABASE);
	flushWriteBuffer();

	// get the current db name
	uint16_t	size;
	if (cs->read(&size,responsetimeoutsec,
				responsetimeoutusec)==sizeof(uint16_t)) {
		delete[] currentdbname;
		currentdbname=new char[size+1];
		if (cs->read(currentdbname,size)!=size) {
			setError("Failed to get the current database.\n A network error may have ocurred.");
			delete[] currentdbname;
			currentdbname=NULL;
			return NULL;
		}
		currentdbname[size]='\0';

		if (debug) {
			debugPreStart();
			debugPrint(currentdbname);
			debugPrint("\n");
			debugPreEnd();
		}
	} else {
		setError("Failed to get the current database.\n A network error may have ocurred.");
		return NULL;
	}
	return currentdbname;
}
