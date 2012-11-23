// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void sqlrconnection_svr::rollbackCommand() {
	dbgfile.debugPrint("connection",1,"rollback...");
	if (rollbackInternal()) {
		dbgfile.debugPrint("connection",1,"rollback succeeded");
		clientsock->write((uint16_t)NO_ERROR_OCCURRED);
	} else {
		dbgfile.debugPrint("connection",1,"rollback failed");
		returnError(!liveconnection);
	}
	flushWriteBuffer();
}

bool sqlrconnection_svr::rollbackInternal() {

	if (rollback()) {
		endFakeTransactionBlock();
		return true;
	}
	return false;
}
