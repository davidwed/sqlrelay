// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void	sqlrconnection::endSessionCommand() {
	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"end session");
	#endif
	endSession();
}

void	sqlrconnection::endSession() {

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"ending session...");
	#endif

	dropTempTables(&sessiontemptables);

	// must set suspendedsession to 0 here so resumed sessions won't 
	// automatically re-suspend
	suspendedsession=0;

	// abort all cursors
	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"aborting all busy cursors...");
	#endif
	for (int i=0; i<cfgfl->getCursors(); i++) {
		if (cur[i]->busy) {
			#ifdef SERVER_DEBUG
			debugPrint("connection",3,(long)i);
			#endif
			cur[i]->abort();
		}
	}
	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"done aborting all busy cursors");
	#endif

	// commit or rollback if necessary
	if (isTransactional() && commitorrollback) {
		if (cfgfl->getEndOfSessionCommit()) {
			#ifdef SERVER_DEBUG
			debugPrint("connection",2,"committing...");
			#endif
			commit();
			#ifdef SERVER_DEBUG
			debugPrint("connection",2,"done committing...");
			#endif
		} else {
			#ifdef SERVER_DEBUG
			debugPrint("connection",2,"rolling back...");
			#endif
			rollback();
			#ifdef SERVER_DEBUG
			debugPrint("connection",2,"done rolling back...");
			#endif
		}
	}

	// reset autocommit behavior
	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"resetting autocommit behavior...");
	#endif
	if (autocommit) {
		#ifdef SERVER_DEBUG
		debugPrint("connection",3,"setting autocommit on...");
		#endif
		autoCommitOn();
		#ifdef SERVER_DEBUG
		debugPrint("connection",3,"done setting autocommit on...");
		#endif
	} else {
		#ifdef SERVER_DEBUG
		debugPrint("connection",3,"setting autocommit off...");
		#endif
		autoCommitOff();
		#ifdef SERVER_DEBUG
		debugPrint("connection",3,"done setting autocommit off...");
		#endif
	}
	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"done resetting autocommit behavior...");
	debugPrint("connection",1,"done ending session");
	#endif
}
