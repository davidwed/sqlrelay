// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrcontroller.h>

bool sqlrcontroller_svr::fetchResultSetCommand(sqlrcursor_svr *cursor) {
	dbgfile.debugPrint("connection",1,"fetching result set...");
	bool	retval=returnResultSetData(cursor);
	dbgfile.debugPrint("connection",1,"done fetching result set");
	return retval;
}
