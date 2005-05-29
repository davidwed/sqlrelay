// Copyright (c) 1999-2004  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void sqlrconnection::incrementConnectionCount() {

	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"incrementing connection count...");
	#endif

	acquireConnectionCountMutex();

	// increment the counter
	uint32_t	*connectioncount=getConnectionCountBuffer();
	(*connectioncount)++;

	#ifdef SERVER_DEBUG
	debugPrint("connection",1,(int32_t)(*connectioncount));
	#endif

	signalScalerToRead();

	releaseConnectionCountMutex();

	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"done incrementing connection count");
	#endif
}

void sqlrconnection::decrementConnectionCount() {

	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"decrementing connection count...");
	#endif

	acquireConnectionCountMutex();

	// decrement the counter
	uint32_t	*connectioncount=getConnectionCountBuffer();
	(*connectioncount)--;

	#ifdef SERVER_DEBUG
	debugPrint("connection",1,(int32_t)(*connectioncount));
	#endif

	releaseConnectionCountMutex();

	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"done decrementing connection count");
	#endif
}

void sqlrconnection::decrementSessionCount() {

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
