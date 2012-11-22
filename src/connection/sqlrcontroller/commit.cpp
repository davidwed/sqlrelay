// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void sqlrconnection_svr::commitCommand() {
	dbgfile.debugPrint("connection",1,"commit");
	if (commitInternal()) {
		clientsock->write((uint16_t)NO_ERROR_OCCURRED);
	} else {
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
