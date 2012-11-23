// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrcontroller.h>

void sqlrcontroller_svr::suspendResultSetCommand(sqlrcursor_svr *cursor) {
	dbgfile.debugPrint("connection",1,"suspend result set...");
	cursor->state=SQLRCURSOR_STATE_SUSPENDED;
	if (cursor->customquerycursor) {
		cursor->state=SQLRCURSOR_STATE_SUSPENDED;
	}
	dbgfile.debugPrint("connection",1,"done suspending result set");
}
