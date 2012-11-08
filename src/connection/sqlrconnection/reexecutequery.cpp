// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

bool sqlrconnection_svr::reExecuteQueryCommand(sqlrcursor_svr *cursor) {
	dbgfile.debugPrint("connection",1,"rexecute query");
	return handleQuery(cursor,true,false,true,true);
}
