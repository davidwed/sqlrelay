// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void sqlrconnection::sendRowCounts(bool knowsactual, uint64_t actual,
					bool knowsaffected, uint64_t affected) {

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"sending row counts...");
	#endif

	// send actual rows, if that is known
	if (knowsactual) {

		#ifdef SERVER_DEBUG
		char	string[30];
		snprintf(string,30,"actual rows: %lld",actual);
		debugPrint("connection",3,string);
		#endif

		clientsock->write((uint16_t)ACTUAL_ROWS);
		clientsock->write(actual);

	} else {

		#ifdef SERVER_DEBUG
		debugPrint("connection",3,"actual rows unknown");
		#endif

		clientsock->write((uint16_t)NO_ACTUAL_ROWS);
	}

	
	// send affected rows, if that is known
	if (knowsaffected) {

		#ifdef SERVER_DEBUG
		char	string[46];
		snprintf(string,46,"affected rows: %lld",affected);
		debugPrint("connection",3,string);
		#endif

		clientsock->write((uint16_t)AFFECTED_ROWS);
		clientsock->write(affected);

	} else {

		#ifdef SERVER_DEBUG
		debugPrint("connection",3,"affected rows unknown");
		#endif

		clientsock->write((uint16_t)NO_AFFECTED_ROWS);
	}

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"done sending row counts");
	#endif
}
