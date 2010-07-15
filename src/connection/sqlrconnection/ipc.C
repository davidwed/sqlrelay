// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>
#include <rudiments/error.h>
#include <rudiments/file.h>

// for sprintf
#include <stdio.h>

#include <config.h>

bool sqlrconnection_svr::createSharedMemoryAndSemaphores(const char *tmpdir,
							const char *id) {

	size_t	idfilenamelen=charstring::length(tmpdir)+5+
					charstring::length(id)+1;
	char	*idfilename=new char[idfilenamelen];
	snprintf(idfilename,idfilenamelen,"%s/ipc/%s",tmpdir,id);

	dbgfile.debugPrint("connection",0,"attaching to shared memory and semaphores");
	dbgfile.debugPrint("connection",0,"id filename: ");
	dbgfile.debugPrint("connection",0,idfilename);

	// initialize shared memory segment for passing port
	dbgfile.debugPrint("connection",1,"attaching to shared memory...");
	idmemory=new sharedmemory();
	if (!idmemory->attach(file::generateKey(idfilename,1))) {
		fprintf(stderr,"Couldn't attach to shared memory segment: ");
		fprintf(stderr,"%s\n",error::getErrorString());
		delete idmemory;
		idmemory=NULL;
		delete[] idfilename;
		return false;
	}


	// initialize the announce semaphore
	dbgfile.debugPrint("connection",1,"attaching to semaphores...");
	semset=new semaphoreset();
	if (!semset->attach(file::generateKey(idfilename,1),11)) {
		fprintf(stderr,"Couldn't attach to semaphore set: ");
		fprintf(stderr,"%s\n",error::getErrorString());
		delete semset;
		delete idmemory;
		semset=NULL;
		idmemory=NULL;
		delete[] idfilename;
		return false;
	}

	dbgfile.debugPrint("connection",0,
			"done attaching to shared memory and semaphores");

	delete[] idfilename;

	return true;
}

void sqlrconnection_svr::waitForListenerToRequireAConnection() {
	dbgfile.debugPrint("connection",1,"waiting for the listener to require a connection");
	semset->wait(11);
	//semset->waitWithUndo(11);
	dbgfile.debugPrint("connection",1,"done waiting for the listener to require a connection");
}

void sqlrconnection_svr::acquireAnnounceMutex() {
	dbgfile.debugPrint("connection",1,"acquiring announce mutex");
	semset->waitWithUndo(0);
	dbgfile.debugPrint("connection",1,"done acquiring announce mutex");
}

shmdata *sqlrconnection_svr::getAnnounceBuffer() {
	return (shmdata *)idmemory->getPointer();
}

void sqlrconnection_svr::releaseAnnounceMutex() {
	dbgfile.debugPrint("connection",1,"releasing announce mutex");
	semset->signalWithUndo(0);
	dbgfile.debugPrint("connection",1,"done releasing announce mutex");
}

void sqlrconnection_svr::signalListenerToRead() {
	dbgfile.debugPrint("connection",1,"signalling listener to read");
	semset->signal(2);
	dbgfile.debugPrint("connection",1,"done signalling listener to read");
}

void sqlrconnection_svr::waitForListenerToFinishReading() {
	dbgfile.debugPrint("connection",1,"waiting for listener");
	semset->wait(3);
	dbgfile.debugPrint("connection",1,"done waiting for listener");
}

void sqlrconnection_svr::acquireConnectionCountMutex() {
	dbgfile.debugPrint("connection",1,"acquiring connection count mutex");
	semset->waitWithUndo(4);
	dbgfile.debugPrint("connection",1,"done acquiring connection count mutex");
}

int32_t *sqlrconnection_svr::getConnectionCountBuffer() {
	return &((shmdata *)idmemory->getPointer())->totalconnections;
}

void sqlrconnection_svr::releaseConnectionCountMutex() {
	dbgfile.debugPrint("connection",1,"releasing connection count mutex");
	semset->signalWithUndo(4);
	dbgfile.debugPrint("connection",1,"done releasing connection count mutex");
}

void sqlrconnection_svr::acquireSessionCountMutex() {
	dbgfile.debugPrint("connection",1,"acquiring session count mutex");
	semset->waitWithUndo(5);
	dbgfile.debugPrint("connection",1,"done acquiring session count mutex");
}

int32_t *sqlrconnection_svr::getSessionCountBuffer() {
	return &((shmdata *)idmemory->getPointer())->connectionsinuse;
}

void sqlrconnection_svr::releaseSessionCountMutex() {
	dbgfile.debugPrint("connection",1,"releasing session count mutex");
	semset->signalWithUndo(5);
	dbgfile.debugPrint("connection",1,"done releasing session count mutex");
}

void sqlrconnection_svr::signalScalerToRead() {
	dbgfile.debugPrint("connection",1,"signalling scaler to read");
	semset->signal(8);
	dbgfile.debugPrint("connection",1,"done signalling scaler to read");
}
