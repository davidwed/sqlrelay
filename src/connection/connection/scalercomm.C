#include <connection/scalercomm.h>

scalercomm::scalercomm(ipc *ipcptr) {
	this->ipcptr=ipcptr;
}

#ifdef SERVER_DEBUG
void scalercomm::setDebugLogger(logger *dl) {
	this->dl=dl;
}
#endif

void scalercomm::incrementConnectionCount() {

	#ifdef SERVER_DEBUG
	dl->write("connection",0,"incrementing connection count...");
	#endif

	ipcptr->acquireConnectionCountMutex();

	// increment the counter
	unsigned int	*connectioncount=ipcptr->getConnectionCountBuffer();
	(*connectioncount)++;

	#ifdef SERVER_DEBUG
	dl->write("connection",1,(long)(*connectioncount));
	#endif

	ipcptr->signalScalerToRead();

	ipcptr->releaseConnectionCountMutex();

	#ifdef SERVER_DEBUG
	dl->write("connection",0,"done incrementing connection count");
	#endif
}

void scalercomm::decrementConnectionCount() {

	#ifdef SERVER_DEBUG
	dl->write("connection",0,"decrementing connection count...");
	#endif

	ipcptr->acquireConnectionCountMutex();

	// decrement the counter
	unsigned int	*connectioncount=ipcptr->getConnectionCountBuffer();
	(*connectioncount)--;

	#ifdef SERVER_DEBUG
	dl->write("connection",1,(long)(*connectioncount));
	#endif

	ipcptr->releaseConnectionCountMutex();

	#ifdef SERVER_DEBUG
	dl->write("connection",0,"done decrementing connection count");
	#endif
}

void scalercomm::decrementSessionCount() {

	#ifdef SERVER_DEBUG
	dl->write("connection",0,"decrementing session count...");
	#endif

	ipcptr->acquireSessionCountMutex();

	// decrement the counter
	unsigned int	*sessioncount=ipcptr->getSessionCountBuffer();
	(*sessioncount)--;

	#ifdef SERVER_DEBUG
	dl->write("connection",1,(long)(*sessioncount));
	#endif

	ipcptr->releaseSessionCountMutex();

	#ifdef SERVER_DEBUG
	dl->write("connection",0,"done decrementing session count");
	#endif
}
