// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void sqlrconnection_svr::commitCommand() {
	dbgfile.debugPrint("connection",1,"commit...");
	if (commitInternal()) {
		dbgfile.debugPrint("connection",1,"commit succeeded");
		clientsock->write((uint16_t)NO_ERROR_OCCURRED);
	} else {
		dbgfile.debugPrint("connection",1,"commit failed");
		returnError(!liveconnection);
	}
	flushWriteBuffer();
}

bool sqlrconnection_svr::commitInternal() {
	if (commit()) {
		endFakeTransactionBlock();
		return true;
	}
	return false;
}
