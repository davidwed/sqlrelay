// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <errno.h>

#include <config.h>

bool sqlrconnection::createSharedMemoryAndSemaphores(char *tmpdir, char *id) {

	char	*idfilename=new char[strlen(tmpdir)+1+strlen(id)+1];
	sprintf(idfilename,"%s/%s",tmpdir,id);

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
	if (!idmemory->attach(ftok(idfilename,0))) {
		fprintf(stderr,"Couldn't attach to shared memory segment: ");
		fprintf(stderr,"%s\n",strerror(errno));
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
	if (!semset->attach(ftok(idfilename,0),11)) {
		fprintf(stderr,"Couldn't attach to semaphore set: ");
		fprintf(stderr,"%s\n",strerror(errno));
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

void sqlrconnection::acquireAnnounceMutex() {
	semset->wait(0);
}

shmdata *sqlrconnection::getAnnounceBuffer() {
	return (shmdata *)idmemory->getPointer();
}

void sqlrconnection::releaseAnnounceMutex() {
	semset->signal(0);
}

void sqlrconnection::signalListenerToRead() {
	semset->signal(2);
}

void sqlrconnection::waitForListenerToFinishReading() {
	semset->wait(3);
}

void sqlrconnection::acquireConnectionCountMutex() {
	semset->wait(4);
}

unsigned int *sqlrconnection::getConnectionCountBuffer() {
	return (unsigned int *)idmemory->getPointer();
}

void sqlrconnection::releaseConnectionCountMutex() {
	semset->signal(4);
}

void sqlrconnection::acquireSessionCountMutex() {
	semset->wait(5);
}

unsigned int *sqlrconnection::getSessionCountBuffer() {
	return (unsigned int *)((long)idmemory->getPointer()+
						sizeof(unsigned int));
}

void sqlrconnection::releaseSessionCountMutex() {
	semset->signal(5);
}

void sqlrconnection::signalScalerToRead() {
	semset->signal(8);
}
