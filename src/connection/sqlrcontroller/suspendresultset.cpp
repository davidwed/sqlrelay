// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void sqlrconnection_svr::suspendResultSetCommand(sqlrcursor_svr *cursor) {
	dbgfile.debugPrint("connection",1,"suspend result set");
	cursor->suspendresultset=true;
}
