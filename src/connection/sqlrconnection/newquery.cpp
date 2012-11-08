// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

bool sqlrconnection_svr::newQueryCommand(sqlrcursor_svr *cursor) {
	dbgfile.debugPrint("connection",1,"new query");
	return handleQuery(cursor,false,false,true,true);
}
