#include <connection/listenercomm.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <netinet/in.h>

#include <defines.h>

listenercomm::listenercomm(ipc *ipcptr, connectioncmdline *cmdlineptr) {
	this->ipcptr=ipcptr;
	this->cmdlineptr=cmdlineptr;
	handoffsockun=NULL;
}

listenercomm::~listenercomm() {
	delete handoffsockun;
}

#ifdef SERVER_DEBUG
void	listenercomm::setDebugLogger(logger *dl) {
	this->dl=dl;
}
#endif

void	listenercomm::announceAvailability(char *tmpdir,
					int passdescriptor,
					char *unixsocket,
					unsigned short inetport,
					char *connectionid) {

	#ifdef SERVER_DEBUG
	dl->write("connection",0,"announcing availability...");
	#endif

	// if we're passing around file descriptors, connect to listener 
	// if we haven't already and pass it this process's pid
	if (passdescriptor && !handoffsockun) {
		registerForHandoff(tmpdir);
	}

	// handle time-to-live
	alarm(cmdlineptr->getTtl());

	ipcptr->acquireAnnounceMutex();

	// cancel time-to-live alarm
	alarm(0);

	// get a pointer to the shared memory segment
	shmdata	*idmemoryptr=ipcptr->getAnnounceBuffer();

	// first, write the connectionid into the segment
	strncpy(idmemoryptr->connectionid,connectionid,MAXCONNECTIONIDLEN);

	// if we're passing descriptors around, write the 
	// pid to the segment otherwise write ports
	if (passdescriptor) {

		#ifdef SERVER_DEBUG
		dl->write("connection",1,"handoff=pass");
		#endif

		// write the pid into the segment
		idmemoryptr->connectioninfo.connectionpid=
						(unsigned long)getpid();

	} else {

		#ifdef SERVER_DEBUG
		dl->write("connection",1,"handoff=reconnect");
		#endif

		// convert the port to network byte order and write it into
		// the segment.
		idmemoryptr->connectioninfo.sockets.inetport=inetport;

		// write the unix socket name into the segment
		if (unixsocket && unixsocket[0]) {
			strncpy(idmemoryptr->connectioninfo.sockets.unixsocket,
							unixsocket,MAXPATHLEN);
		}
	}

	ipcptr->signalListenerToRead();

	ipcptr->waitForListenerToFinishReading();

	ipcptr->releaseAnnounceMutex();

	#ifdef SERVER_DEBUG
	dl->write("connection",0,"done announcing availability...");
	#endif
}

void	listenercomm::registerForHandoff(char *tmpdir) {

	#ifdef SERVER_DEBUG
	dl->write("connection",0,"registering for handoff...");
	#endif

	// construct the name of the socket to connect to
	char	*handoffsockname=new char[strlen(tmpdir)+1+
					strlen(cmdlineptr->getId())+8+1];
	sprintf(handoffsockname,"%s/%s-handoff",tmpdir,cmdlineptr->getId());

	#ifdef SERVER_DEBUG
	char	*string=new char[17+strlen(handoffsockname)+1];
	sprintf(string,"handoffsockname: %s",handoffsockname);
	dl->write("connection",1,string);
	delete[] string;
	#endif

	// Try to connect over and over forever on 1 second intervals.
	// If the connect succeeds but the write fails, loop back and
	// try again.
	while (1) {

		#ifdef SERVER_DEBUG
		dl->write("connection",1,"trying...");
		#endif

		handoffsockun=new unixclientsocket();
		handoffsockun->connectToServer(handoffsockname,1,0);
		if (handoffsockun->write((unsigned long)getpid())==
						sizeof(unsigned long)) {
			break;
		}
		deRegisterForHandoff(tmpdir);
		delete handoffsockun;
	}

	// clean up
	delete[] handoffsockname;

	#ifdef SERVER_DEBUG
	dl->write("connection",0,"done registering for handoff");
	#endif
}

int	listenercomm::receiveFileDescriptor(int *descriptor) {
	int	retval=handoffsockun->receiveFileDescriptor(descriptor);
	if (!retval) {
		delete handoffsockun;
		handoffsockun=NULL;
	}
	return retval;
}

void	listenercomm::deRegisterForHandoff(char *tmpdir) {
	
	#ifdef SERVER_DEBUG
	dl->write("connection",0,"de-registering for handoff...");
	#endif

	// construct the name of the socket to connect to
	char	*removehandoffsockname=new char[strlen(tmpdir)+1+
					strlen(cmdlineptr->getId())+14+1];
	sprintf(removehandoffsockname,"%s/%s-removehandoff",
					tmpdir,cmdlineptr->getId());

	#ifdef SERVER_DEBUG
	char	*string=new char[23+strlen(removehandoffsockname)+1];
	sprintf(string,"removehandoffsockname: %s",removehandoffsockname);
	dl->write("connection",1,string);
	delete[] string;
	#endif

	// attach to the socket and write the process id
	unixclientsocket	*removehandoffsockun=new unixclientsocket();
	removehandoffsockun->connectToServer(removehandoffsockname,0,1);
	removehandoffsockun->write((unsigned long)getpid());

	// clean up
	delete removehandoffsockun;
	delete[] removehandoffsockname;

	#ifdef SERVER_DEBUG
	dl->write("connection",0,"done de-registering for handoff");
	#endif
}
