// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

bool sqlrconnection_svr::getSendColumnInfo() {

	dbgfile.debugPrint("connection",2,"getting send column info...");

	if (clientsock->read(&sendcolumninfo,
				idleclienttimeout,0)!=sizeof(uint16_t)) {
		dbgfile.debugPrint("connection",2,"getting send column info failed");
		return false;
	}

	if (sendcolumninfo==SEND_COLUMN_INFO) {
		dbgfile.debugPrint("connection",3,"send column info");
	} else {
		dbgfile.debugPrint("connection",3,"don't send column info");
	}
	dbgfile.debugPrint("connection",2,"done getting send column info...");

	return true;
}
