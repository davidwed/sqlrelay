// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

int	sqlrconnection::newQueryCommand() {

	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"new query");
	#endif

	// find an available cursor
	if ((currentcur=findAvailableCursor())==-1) {
		getQueryFromClient(0,0);
		noAvailableCursors();
		return 1;
	}

	// handle query will return 1 for success,
	// 0 for network error and -1 for a bad query
	int	querystatus=handleQuery(0,0,1);
	if (querystatus==1) {

		// reinit lastrow
		lastrow=-1;
		if (!returnResultSetData()) {
			endSession();
			return 0;
		}
		return 1;

	} else if (querystatus==-1) {
		return 1;
	} else {
		endSession();
		return 0;
	}
}

int	sqlrconnection::findAvailableCursor() {

	for (int i=0; i<cfgfl->getCursors(); i++) {
		if (!cur[i]->busy) {
			cur[i]->busy=1;
			#ifdef SERVER_DEBUG
			debugPrint("connection",3,(long)currentcur);
			debugPrint("connection",2,"found a free cursor...");
			debugPrint("connection",2,"done getting a cursor");
			#endif
			return i;
		}
	}
	#ifdef SERVER_DEBUG
	debugPrint("connection",1,
			"find available cursor failed: all cursors are busy");
	#endif
	return -1;
}
