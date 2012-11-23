// Copyright (c) 2011  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void sqlrconnection_svr::beginCommand() {
	dbgfile.debugPrint("connection",1,"begin...");
	if (beginInternal()) {
		dbgfile.debugPrint("connection",1,"begin succeeded");
		clientsock->write((uint16_t)NO_ERROR_OCCURRED);
	} else {
		dbgfile.debugPrint("connection",1,"begin failed");
		returnError(!liveconnection);
	}
	flushWriteBuffer();
}

bool sqlrconnection_svr::beginInternal() {

	// if we're faking transaction blocks, do that,
	// otherwise run an actual begin query
	return (faketransactionblocks)?beginFakeTransactionBlock():begin();
}
