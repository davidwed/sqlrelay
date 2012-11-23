// Copyright (c) 2011  David Muse
// See the file COPYING for more information

#include <sqlrcontroller.h>

void sqlrcontroller_svr::beginCommand() {
	dbgfile.debugPrint("connection",1,"begin...");
	if (begin()) {
		dbgfile.debugPrint("connection",1,"begin succeeded");
		clientsock->write((uint16_t)NO_ERROR_OCCURRED);
	} else {
		dbgfile.debugPrint("connection",1,"begin failed");
		returnError(!conn->liveconnection);
	}
	flushWriteBuffer();
}

bool sqlrcontroller_svr::begin() {
	// if we're faking transaction blocks, do that,
	// otherwise run an actual begin query
	return (faketransactionblocks)?
			beginFakeTransactionBlock():conn->begin();
}
