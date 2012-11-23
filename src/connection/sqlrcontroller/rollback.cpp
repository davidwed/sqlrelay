// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrcontroller.h>

void sqlrcontroller_svr::rollbackCommand() {
	dbgfile.debugPrint("connection",1,"rollback...");
	if (rollbackInternal()) {
		dbgfile.debugPrint("connection",1,"rollback succeeded");
		clientsock->write((uint16_t)NO_ERROR_OCCURRED);
	} else {
		dbgfile.debugPrint("connection",1,"rollback failed");
		returnError(!conn->liveconnection);
	}
	flushWriteBuffer();
}

bool sqlrcontroller_svr::rollbackInternal() {

	if (conn->rollback()) {
		endFakeTransactionBlock();
		return true;
	}
	return false;
}
