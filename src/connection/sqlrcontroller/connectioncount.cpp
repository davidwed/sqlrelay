// Copyright (c) 1999-2004  David Muse
// See the file COPYING for more information

#include <sqlrcontroller.h>

void sqlrcontroller_svr::incrementConnectionCount() {

	dbgfile.debugPrint("connection",0,"incrementing connection count...");

	if (scalerspawned) {

		dbgfile.debugPrint("connection",0,"scaler will do the job");
		signalScalerToRead();

	} else {

		acquireConnectionCountMutex();

		// increment the counter
		int32_t	*connectioncount=getConnectionCountBuffer();
		(*connectioncount)++;
		decrementonclose=true;

		dbgfile.debugPrint("connection",1,(*connectioncount));

		releaseConnectionCountMutex();
	}

	dbgfile.debugPrint("connection",0,"done incrementing connection count");
}

void sqlrcontroller_svr::decrementConnectionCount() {

	dbgfile.debugPrint("connection",0,"decrementing connection count...");

	if (scalerspawned) {

		dbgfile.debugPrint("connection",0,"scaler will do the job");

	} else {

		acquireConnectionCountMutex();

		// decrement the counter
		int32_t	*connectioncount=getConnectionCountBuffer();
		if (--(*connectioncount)<0) {
			(*connectioncount)=0;
		}
		decrementonclose=false;

		dbgfile.debugPrint("connection",1,(*connectioncount));

		releaseConnectionCountMutex();
	}

	dbgfile.debugPrint("connection",0,"done decrementing connection count");
}

void sqlrcontroller_svr::decrementSessionCount() {

	dbgfile.debugPrint("connection",0,"decrementing session count...");

	acquireSessionCountMutex();

	// decrement the counter
	int32_t	*sessioncount=getSessionCountBuffer();
	if (--(*sessioncount)<0) {
		(*sessioncount)=0;
	}

	dbgfile.debugPrint("connection",1,(*sessioncount));

	releaseSessionCountMutex();

	dbgfile.debugPrint("connection",0,"done decrementing session count");
}
