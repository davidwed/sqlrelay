// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void sqlrconnection::endSessionCommand() {
	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"end session");
	#endif
	endSession();
}

void sqlrconnection::endSession() {

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"ending session...");
	#endif

	// must set suspendedsession to false here so resumed sessions won't 
	// automatically re-suspend
	suspendedsession=false;

	// abort all cursors
	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"aborting all busy cursors...");
	#endif
	for (int32_t i=0; i<cfgfl->getCursors(); i++) {
		if (cur[i]->busy) {

			#ifdef SERVER_DEBUG
			debugPrint("connection",3,i);
			#endif

			// It's ok to call cleanUpData() here, ordinarily we
			// wouldn't so that result sets that were suspended
			// after the entire result set was fetched would be
			// able to return column data when resumed, but since
			// we're ending the session, we don't care...
			cur[i]->cleanUpData(true,true);
			cur[i]->abort();
		}
	}
	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"done aborting all busy cursors");
	#endif

	// truncate/drop temp tables
	truncateTempTables(cur[0],&sessiontemptablesfortrunc);
	dropTempTables(cur[0],&sessiontemptablesfordrop);

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
