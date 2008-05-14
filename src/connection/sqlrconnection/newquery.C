// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

bool sqlrconnection_svr::newQueryCommand(sqlrcursor_svr *cursor) {

	dbgfile.debugPrint("connection",1,"new query");

	// handleQuery() will return:
	//	1 for success,
	//	0 for network error
	//	-1 for a bad query
	int32_t	querystatus=handleQuery(cursor,false,false,true);
	if (querystatus==1) {

		// reinit lastrow
		lastrowvalid=false;
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
