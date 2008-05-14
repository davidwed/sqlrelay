// Copyright (c) 1999-2004  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void sqlrconnection_svr::incrementConnectionCount() {

	dbgfile.debugPrint("connection",0,"incrementing connection count...");

	acquireConnectionCountMutex();

	// increment the counter
	uint32_t	*connectioncount=getConnectionCountBuffer();
	(*connectioncount)++;
	decrementonclose=true;

	dbgfile.debugPrint("connection",1,(int32_t)(*connectioncount));

	signalScalerToRead();

	releaseConnectionCountMutex();

	dbgfile.debugPrint("connection",0,"done incrementing connection count");
}

void sqlrconnection_svr::decrementConnectionCount() {

	dbgfile.debugPrint("connection",0,"decrementing connection count...");

	acquireConnectionCountMutex();

	// decrement the counter
	uint32_t	*connectioncount=getConnectionCountBuffer();
	(*connectioncount)--;
	if ((*connectioncount)<0) {
		(*connectioncount)=0;
	}
	decrementonclose=false;

	dbgfile.debugPrint("connection",1,(int32_t)(*connectioncount));

	releaseConnectionCountMutex();

	dbgfile.debugPrint("connection",0,"done decrementing connection count");
}

void sqlrconnection_svr::decrementSessionCount() {

	dbgfile.debugPrint("connection",0,"decrementing session count...");

	acquireSessionCountMutex();

	// decrement the counter
	uint32_t	*sessioncount=getSessionCountBuffer();
	(*sessioncount)--;

	dbgfile.debugPrint("connection",1,(int32_t)(*sessioncount));

	releaseSessionCountMutex();

	dbgfile.debugPrint("connection",0,"done decrementing session count");
}
