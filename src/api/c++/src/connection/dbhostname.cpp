// Copyright (c) 2007  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrclient.h>
#include <defines.h>

const char *sqlrconnection::dbHostName() {

	if (!openSession()) {
		return NULL;
	}

	if (debug) {
		debugPreStart();
		debugPrint("DB Host Name...");
		debugPrint("\n");
		debugPreEnd();
	}

	cs->write((uint16_t)DBHOSTNAME);
	flushWriteBuffer();

	// get the db host name
	uint16_t	size;
	if (cs->read(&size,responsetimeoutsec,
				responsetimeoutusec)==sizeof(uint16_t)) {
		delete[] dbhostname;
		dbhostname=new char[size+1];
		if (cs->read(dbhostname,size)!=size) {
			setError("Failed to get DB host name.\n A network error may have ocurred.");
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
	} else {
		setError("Failed to get DB host name.\n A network error may have ocurred.");
		return NULL;
	}
	return dbhostname;
}

const char *sqlrconnection::dbIpAddress() {

	if (!openSession()) {
		return NULL;
	}

	if (debug) {
		debugPreStart();
		debugPrint("DB Ip Address...");
		debugPrint("\n");
		debugPreEnd();
	}

	cs->write((uint16_t)DBIPADDRESS);
	flushWriteBuffer();

	// get the db ip address
	uint16_t	size;
	if (cs->read(&size,responsetimeoutsec,
				responsetimeoutusec)==sizeof(uint16_t)) {
		delete[] dbipaddress;
		dbipaddress=new char[size+1];
		if (cs->read(dbipaddress,size)!=size) {
			setError("Failed to get DB ip address.\n A network error may have ocurred.");
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
	} else {
		setError("Failed to get DB ip address.\n A network error may have ocurred.");
		return NULL;
	}
	return dbipaddress;
}
