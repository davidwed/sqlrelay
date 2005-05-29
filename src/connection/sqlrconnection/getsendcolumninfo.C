// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

bool sqlrconnection::getSendColumnInfo() {

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"getting send column info...");
	#endif

	if (clientsock->read(&sendcolumninfo)!=sizeof(uint16_t)) {
		#ifdef SERVER_DEBUG
		debugPrint("connection",2,"getting send column info failed");
		#endif
		return false;
	}

	#ifdef SERVER_DEBUG
	if (sendcolumninfo==SEND_COLUMN_INFO) {
		debugPrint("connection",3,"send column info");
	} else {
		debugPrint("connection",3,"don't send column info");
	}
	debugPrint("connection",2,"done getting send column info...");
	#endif

	return true;
}
