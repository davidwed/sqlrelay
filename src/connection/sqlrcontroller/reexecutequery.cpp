// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrcontroller.h>

bool sqlrcontroller_svr::reExecuteQueryCommand(sqlrcursor_svr *cursor) {
	dbgfile.debugPrint("connection",1,"rexecute query");
	return handleQueryOrBindCursor(cursor,true,false,true);
}
