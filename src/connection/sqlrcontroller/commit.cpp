// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrcontroller.h>

void sqlrcontroller_svr::commitCommand() {
	dbgfile.debugPrint("connection",1,"commit...");
	if (commit()) {
		dbgfile.debugPrint("connection",1,"commit succeeded");
		clientsock->write((uint16_t)NO_ERROR_OCCURRED);
	} else {
		dbgfile.debugPrint("connection",1,"commit failed");
		returnError(!conn->liveconnection);
	}
	flushWriteBuffer();
}

bool sqlrcontroller_svr::commit() {
	if (conn->commit()) {
		endFakeTransactionBlock();
		return true;
	}
	return false;
}
