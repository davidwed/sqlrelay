// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

bool sqlrconnection::newQueryCommand(sqlrcursor *cursor) {

	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"new query");
	#endif

	// handle query will return 1 for success,
	// 0 for network error and -1 for a bad query
	int	querystatus=handleQuery(cursor,false,false,true);
	if (querystatus==1) {

		// reinit lastrow
		lastrow=-1;
		if (!returnResultSetData(cursor)) {
			endSession();
			return false;
		}
		return true;

	} else if (!querystatus) {
		endSession();
		return false;
	} else {
		return true;
	}
}
