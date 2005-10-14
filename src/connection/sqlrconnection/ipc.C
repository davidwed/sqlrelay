// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>
#include <rudiments/error.h>
#include <rudiments/file.h>

// for sprintf
#include <stdio.h>

#include <config.h>

bool sqlrconnection::createSharedMemoryAndSemaphores(const char *tmpdir,
							const char *id) {

	size_t	idfilenamelen=charstring::length(tmpdir)+5+
					charstring::length(id)+1;
	char	*idfilename=new char[idfilenamelen];
	snprintf(idfilename,idfilenamelen,"%s/ipc/%s",tmpdir,id);

	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"attaching to shared memory and semaphores");
	debugPrint("connection",0,"id filename: ");
	debugPrint("connection",0,idfilename);
	#endif

	// initialize shared memory segment for passing port
	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"attaching to shared memory...");
	#endif
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
	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"attaching to semaphores...");
	#endif
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

	#ifdef SERVER_DEBUG
	debugPrint("connection",0,
			"done attaching to shared memory and semaphores");
	#endif

	delete[] idfilename;

	return true;
}

void sqlrconnection::waitForListenerToRequireAConnection() {
	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"waiting for the listener to require a connection");
	#endif
	semset->wait(11);
	//semset->waitWithUndo(11);
	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"done waiting for the listener to require a connection");
	#endif
}

void sqlrconnection::acquireAnnounceMutex() {
	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"acquiring announce mutex");
	#endif
	semset->waitWithUndo(0);
	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"done acquiring announce mutex");
	#endif
}

shmdata *sqlrconnection::getAnnounceBuffer() {
	return (shmdata *)idmemory->getPointer();
}

void sqlrconnection::releaseAnnounceMutex() {
	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"releasing announce mutex");
	#endif
	semset->signalWithUndo(0);
	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"done releasing announce mutex");
	#endif
}

void sqlrconnection::signalListenerToRead() {
	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"signalling listener to read");
	#endif
	semset->signal(2);
	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"done signalling listener to read");
	#endif
}

void sqlrconnection::waitForListenerToFinishReading() {
	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"waiting for listener");
	#endif
	semset->wait(3);
	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"done waiting for listener");
	#endif
}

void sqlrconnection::acquireConnectionCountMutex() {
	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"acquiring connection count mutex");
	#endif
	semset->waitWithUndo(4);
	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"done acquiring connection count mutex");
	#endif
}

uint32_t *sqlrconnection::getConnectionCountBuffer() {
	return (uint32_t *)idmemory->getPointer();
}

void sqlrconnection::releaseConnectionCountMutex() {
	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"releasing connection count mutex");
	#endif
	semset->signalWithUndo(4);
	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"done releasing connection count mutex");
	#endif
}

void sqlrconnection::acquireSessionCountMutex() {
	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"acquiring session count mutex");
	#endif
	semset->wait(5);
	//semset->waitWithUndo(5);
	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"done acquiring session count mutex");
	#endif
}

uint32_t *sqlrconnection::getSessionCountBuffer() {
	return (uint32_t *)((long)idmemory->getPointer()+sizeof(uint32_t));
}

void sqlrconnection::releaseSessionCountMutex() {
	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"releasing session count mutex");
	#endif
	semset->signal(5);
	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"done releasing session count mutex");
	#endif
}

void sqlrconnection::signalScalerToRead() {
	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"signalling scaler to read");
	#endif
	semset->signal(8);
	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"done signalling scaler to read");
	#endif
}
