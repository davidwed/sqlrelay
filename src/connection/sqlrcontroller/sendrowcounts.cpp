// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrcontroller.h>

void sqlrcontroller_svr::sendRowCounts(bool knowsactual, uint64_t actual,
					bool knowsaffected, uint64_t affected) {

	dbgfile.debugPrint("connection",2,"sending row counts...");

	// send actual rows, if that is known
	if (knowsactual) {

		char	string[30];
		snprintf(string,30,"actual rows: %lld",(long long)actual);
		dbgfile.debugPrint("connection",3,string);

		clientsock->write((uint16_t)ACTUAL_ROWS);
		clientsock->write(actual);

	} else {

		dbgfile.debugPrint("connection",3,"actual rows unknown");

		clientsock->write((uint16_t)NO_ACTUAL_ROWS);
	}

	
	// send affected rows, if that is known
	if (knowsaffected) {

		char	string[46];
		snprintf(string,46,"affected rows: %lld",(long long)affected);
		dbgfile.debugPrint("connection",3,string);

		clientsock->write((uint16_t)AFFECTED_ROWS);
		clientsock->write(affected);

	} else {

		dbgfile.debugPrint("connection",3,"affected rows unknown");

		clientsock->write((uint16_t)NO_AFFECTED_ROWS);
	}

	dbgfile.debugPrint("connection",2,"done sending row counts");
}
