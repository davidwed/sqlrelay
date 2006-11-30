// Copyright (c) 1999-2004  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void sqlrconnection_svr::incrementConnectionCount() {

	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"incrementing connection count...");
	#endif

	acquireConnectionCountMutex();

	// increment the counter
	uint32_t	*connectioncount=getConnectionCountBuffer();
	(*connectioncount)++;
	decrementonclose=true;

	#ifdef SERVER_DEBUG
	debugPrint("connection",1,(int32_t)(*connectioncount));
	#endif

	signalScalerToRead();

	releaseConnectionCountMutex();

	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"done incrementing connection count");
	#endif
}

void sqlrconnection_svr::decrementConnectionCount() {

	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"decrementing connection count...");
	#endif

	acquireConnectionCountMutex();

	// decrement the counter
	uint32_t	*connectioncount=getConnectionCountBuffer();
	(*connectioncount)--;
	if ((*connectioncount)<0) {
		(*connectioncount)=0;
	}
	decrementonclose=false;

	#ifdef SERVER_DEBUG
	debugPrint("connection",1,(int32_t)(*connectioncount));
	#endif

	releaseConnectionCountMutex();

	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"done decrementing connection count");
	#endif
}

void sqlrconnection_svr::decrementSessionCount() {

	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"decrementing session count...");
	#endif

	acquireSessionCountMutex();

	// decrement the counter
	uint32_t	*sessioncount=getSessionCountBuffer();
	(*sessioncount)--;

	#ifdef SERVER_DEBUG
	debugPrint("connection",1,(int32_t)(*sessioncount));
	#endif

	releaseSessionCountMutex();

	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"done decrementing session count");
	#endif
}
