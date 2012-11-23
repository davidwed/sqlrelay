// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrcontroller.h>

bool sqlrcontroller_svr::newQueryCommand(sqlrcursor_svr *cursor) {
	dbgfile.debugPrint("connection",1,"new query");
	return handleQueryOrBindCursor(cursor,false,false,true);
}
