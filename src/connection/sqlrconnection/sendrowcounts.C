// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void	sqlrconnection::sendRowCounts(long actual, long affected) {

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"sending row counts...");
	#endif


	// send actual rows, if that is known
	if (actual>-1) {

		#ifdef SERVER_DEBUG
		char	string[30];
		sprintf(string,"actual rows: %ld",actual);
		debugPrint("connection",3,string);
		#endif

		clientsock->write((unsigned short)ACTUAL_ROWS);
		clientsock->write((unsigned long)actual);
	} else {

		#ifdef SERVER_DEBUG
		debugPrint("connection",3,"actual rows unknown");
		#endif

		clientsock->write((unsigned short)NO_ACTUAL_ROWS);
	}

	
	// send affected rows, if that is known
	if (affected>-1) {

		#ifdef SERVER_DEBUG
		char	string[46];
		sprintf(string,"affected rows: %ld",affected);
		debugPrint("connection",3,string);
		#endif

		clientsock->write((unsigned short)AFFECTED_ROWS);
		clientsock->write((unsigned long)affected);

	} else {

		#ifdef SERVER_DEBUG
		debugPrint("connection",3,"affected rows unknown");
		#endif

		clientsock->write((unsigned short)NO_AFFECTED_ROWS);
	}

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"done sending row counts");
	#endif
}
