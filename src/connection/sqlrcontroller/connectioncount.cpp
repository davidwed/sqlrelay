// Copyright (c) 1999-2004  David Muse
// See the file COPYING for more information

#include <sqlrcontroller.h>
#include <rudiments/datetime.h>

void sqlrcontroller_svr::incrementConnectionCount() {

	dbgfile.debugPrint("connection",0,"incrementing connection count...");

	if (scalerspawned) {

		dbgfile.debugPrint("connection",0,"scaler will do the job");
		signalScalerToRead();

	} else {

		acquireConnectionCountMutex();

		// increment the counter
		shm->totalconnections++;
		decrementonclose=true;

		dbgfile.debugPrint("connection",1,shm->totalconnections);

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

		if (shm->totalconnections) {
			shm->totalconnections--;
		}
		decrementonclose=false;

		dbgfile.debugPrint("connection",1,shm->totalconnections);

		releaseConnectionCountMutex();
	}

	dbgfile.debugPrint("connection",0,"done decrementing connection count");
}

void sqlrcontroller_svr::decrementSessionCount() {

	dbgfile.debugPrint("connection",0,"decrementing session count...");

	if (!semset->waitWithUndo(5)) {
		// FIXME: bail somehow
	}

	// increment the connections-in-use count
	if (shm->connectionsinuse) {
		shm->connectionsinuse--;
	}

	// update the peak connections-in-use count
	if (shm->connectionsinuse>shm->peak_connectionsinuse) {
		shm->peak_connectionsinuse=shm->connectionsinuse;
	}

	// update the peak connections-in-use over the previous minute count
	datetime	dt;
	dt.getSystemDateAndTime();
	if (shm->connectionsinuse>shm->peak_connectionsinuse_1min ||
		dt.getEpoch()/60>shm->peak_connectionsinuse_1min_time/60) {
		shm->peak_connectionsinuse_1min=shm->connectionsinuse;
		shm->peak_connectionsinuse_1min_time=dt.getEpoch();
	}

	dbgfile.debugPrint("connection",1,shm->connectionsinuse);

	if (!semset->signalWithUndo(5)) {
		// FIXME: bail somehow
	}

	dbgfile.debugPrint("connection",0,"done decrementing session count");
}
