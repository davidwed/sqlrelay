// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

int	sqlrconnection::newQueryCommand() {

	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"new query");
	#endif

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
