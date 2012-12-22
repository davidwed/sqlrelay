// Copyright (c) 2007  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrclient.h>
#include <defines.h>

const char *sqlrconnection::dbHostName() {

	if (!openSession()) {
		return NULL;
	}

	clearError();

	if (debug) {
		debugPreStart();
		debugPrint("DB Host Name...");
		debugPrint("\n");
		debugPreEnd();
	}

	// tell the server we want the db host name
	cs->write((uint16_t)DBHOSTNAME);
	flushWriteBuffer();

	if (gotError()) {
		return NULL;
	}

	// get the db host name size
	uint16_t	size;
	if (cs->read(&size,responsetimeoutsec,
				responsetimeoutusec)!=sizeof(uint16_t)) {
		setError("Failed to get DB host name.\n "
				"A network error may have ocurred.");
		return NULL;
	}

	// get the db host name
	delete[] dbhostname;
	dbhostname=new char[size+1];
	if (cs->read(dbhostname,size)!=size) {
		setError("Failed to get DB host name.\n "
				"A network error may have ocurred.");
		delete[] dbhostname;
		dbhostname=NULL;
		return NULL;
	}
	dbhostname[size]='\0';

	if (debug) {
		debugPreStart();
		debugPrint(dbhostname);
		debugPrint("\n");
		debugPreEnd();
	}
	return dbhostname;
}

const char *sqlrconnection::dbIpAddress() {

	if (!openSession()) {
		return NULL;
	}

	clearError();

	if (debug) {
		debugPreStart();
		debugPrint("DB Ip Address...");
		debugPrint("\n");
		debugPreEnd();
	}

	// tell the server we want the db ip address
	cs->write((uint16_t)DBIPADDRESS);
	flushWriteBuffer();

	if (gotError()) {
		return NULL;
	}

	// get the db ip address size
	uint16_t	size;
	if (cs->read(&size,responsetimeoutsec,
				responsetimeoutusec)!=sizeof(uint16_t)) {
		setError("Failed to get DB ip address.\n "
				"A network error may have ocurred.");
		return NULL;
	}

	// get the db ip address
	delete[] dbipaddress;
	dbipaddress=new char[size+1];
	if (cs->read(dbipaddress,size)!=size) {
		setError("Failed to get DB ip address.\n "
				"A network error may have ocurred.");
		delete[] dbipaddress;
		dbipaddress=NULL;
		return NULL;
	}
	dbipaddress[size]='\0';

	if (debug) {
		debugPreStart();
		debugPrint(dbipaddress);
		debugPrint("\n");
		debugPreEnd();
	}
	return dbipaddress;
}
