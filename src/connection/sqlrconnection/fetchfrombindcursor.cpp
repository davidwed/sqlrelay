// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

bool sqlrconnection_svr::fetchFromBindCursorCommand(sqlrcursor_svr *cursor) {
	dbgfile.debugPrint("connection",1,"fetch from bind cursor");
	return handleQueryOrBindCursor(cursor,false,true,true);
}
