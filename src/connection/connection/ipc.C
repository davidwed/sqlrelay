#include <connection/ipc.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <errno.h>

#include <config.h>

ipc::ipc() {
	semset=NULL;
	idmemory=NULL;
	#ifdef SERVER_DEBUG
	dl=NULL;
	#endif
}

ipc::~ipc() {
	#ifdef SERVER_DEBUG
	dl->write("connection",0,"deleting idmemory...");
	#endif
	delete idmemory;
	#ifdef SERVER_DEBUG
	dl->write("connection",0,"done deleting idmemory");
	#endif

	#ifdef SERVER_DEBUG
	dl->write("connection",0,"deleting semset...");
	#endif
	delete semset;
	#ifdef SERVER_DEBUG
	dl->write("connection",0,"done deleting semset");
	#endif
}

#ifdef SERVER_DEBUG
void	ipc::setDebugLogger(logger *dl) {
	this->dl=dl;
}
#endif

int	ipc::createSharedMemoryAndSemaphores(char *tmpdir, char *id) {

	char	*idfilename=new char[strlen(tmpdir)+1+strlen(id)+1];
	sprintf(idfilename,"%s/%s",tmpdir,id);

	#ifdef SERVER_DEBUG
	dl->write("connection",0,
				"attaching to shared memory and semaphores");
	dl->write("connection",0,"id filename: ");
	dl->write("connection",0,idfilename);
	#endif

	// initialize shared memory segment for passing port
	#ifdef SERVER_DEBUG
	dl->write("connection",1,"attaching to shared memory...");
	#endif
	idmemory=new sharedmemory();
	if (!idmemory->attach(ftok(idfilename,0))) {
		fprintf(stderr,"Couldn't attach to shared memory segment: ");
		fprintf(stderr,"%s\n",strerror(errno));
		delete idmemory;
		idmemory=NULL;
		delete[] idfilename;
		return 0;
	}


	// initialize the announce semaphore
	#ifdef SERVER_DEBUG
	dl->write("connection",1,"attaching to semaphores...");
	#endif
	semset=new semaphoreset();
	if (!semset->attach(ftok(idfilename,0),10)) {
		fprintf(stderr,"Couldn't attach to semaphore set: ");
		fprintf(stderr,"%s\n",strerror(errno));
		delete semset;
		delete idmemory;
		semset=NULL;
		idmemory=NULL;
		delete[] idfilename;
		return 0;
	}

	#ifdef SERVER_DEBUG
	dl->write("connection",0,
			"done attaching to shared memory and semaphores");
	#endif

	delete[] idfilename;

	return 1;
}

int	ipc::initialized() {
	return (semset && idmemory);
}

void	ipc::acquireAnnounceMutex() {
	semset->wait(0);
}

unsigned char	*ipc::getAnnounceBuffer() {
	return (unsigned char *)idmemory->getPointer()+(2*sizeof(unsigned int));
}

void	ipc::releaseAnnounceMutex() {
	semset->signal(0);
}

void	ipc::signalListenerToRead() {
	semset->signal(2);
}

void	ipc::waitForListenerToFinishReading() {
	semset->wait(3);
}

void	ipc::acquireConnectionCountMutex() {
	semset->wait(4);
}

unsigned int	*ipc::getConnectionCountBuffer() {
	return (unsigned int *)idmemory->getPointer();
}

void	ipc::releaseConnectionCountMutex() {
	semset->signal(4);
}

void	ipc::acquireSessionCountMutex() {
	semset->wait(5);
}

unsigned int	*ipc::getSessionCountBuffer() {
	return (unsigned int *)((long)idmemory->getPointer()+
						sizeof(unsigned int));
}

void	ipc::releaseSessionCountMutex() {
	semset->signal(5);
}

void	ipc::signalScalerToRead() {
	semset->signal(8);
}
