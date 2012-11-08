// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

bool sqlrconnection_svr::newQueryCommand(sqlrcursor_svr *cursor) {
	return newQueryInternal(cursor,true);
}

bool sqlrconnection_svr::newQueryInternal(sqlrcursor_svr *cursor,
							bool getquery) {

	dbgfile.debugPrint("connection",1,"new query");

	// handleQuery() will return:
	//	1 for success,
	//	0 for network error
	//	-1 for a query error
	int32_t	querystatus=handleQuery(cursor,false,false,true,getquery);
	if (querystatus==1) {

		// reinit lastrow
		lastrowvalid=false;
		bool	success=returnResultSetData(cursor);
		writeQueryLog(cursor,true);
		if (!success) {
			endSession();
			return false;
		}
		return true;

	} else if (!querystatus) {
		endSession();
		return false;
	} else {
		writeQueryLog(cursor,false);
		return true;
	}
}
