// Copyright (c) 1999-2004  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <netinet/in.h>

#include <defines.h>

void sqlrconnection::announceAvailability(char *tmpdir,
					bool passdescriptor,
					char *unixsocket,
					unsigned short inetport,
					char *connectionid) {

	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"announcing availability...");
	#endif

	// if we're passing around file descriptors, connect to listener 
	// if we haven't already and pass it this process's pid
	if (passdescriptor && !connected) {
		registerForHandoff(tmpdir);
	}

	// handle time-to-live
	alarm(ttl);

	acquireAnnounceMutex();

	// cancel time-to-live alarm
	alarm(0);

	// get a pointer to the shared memory segment
	shmdata	*idmemoryptr=getAnnounceBuffer();

	// first, write the connectionid into the segment
	strncpy(idmemoryptr->connectionid,connectionid,MAXCONNECTIONIDLEN);

	// if we're passing descriptors around, write the 
	// pid to the segment otherwise write ports
	if (passdescriptor) {

		#ifdef SERVER_DEBUG
		debugPrint("connection",1,"handoff=pass");
		#endif

		// write the pid into the segment
		idmemoryptr->connectioninfo.connectionpid=
						(unsigned long)getpid();

	} else {

		#ifdef SERVER_DEBUG
		debugPrint("connection",1,"handoff=reconnect");
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

	signalListenerToRead();

	waitForListenerToFinishReading();

	releaseAnnounceMutex();

	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"done announcing availability...");
	#endif
}

void sqlrconnection::registerForHandoff(char *tmpdir) {

	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"registering for handoff...");
	#endif

	// construct the name of the socket to connect to
	char	handoffsockname[strlen(tmpdir)+1+strlen(cmdl->getId())+8+1];
	sprintf(handoffsockname,"%s/%s-handoff",tmpdir,cmdl->getId());

	#ifdef SERVER_DEBUG
	char	string[17+strlen(handoffsockname)+1];
	sprintf(string,"handoffsockname: %s",handoffsockname);
	debugPrint("connection",1,string);
	#endif

	// Try to connect over and over forever on 1 second intervals.
	// If the connect succeeds but the write fails, loop back and
	// try again.
	for (;;) {

		#ifdef SERVER_DEBUG
		debugPrint("connection",1,"trying...");
		#endif

		handoffsockun.connect(handoffsockname,-1,-1,1,0);
		if (handoffsockun.write((unsigned long)getpid())==
						sizeof(unsigned long)) {
			connected=true;
			break;
		}
		deRegisterForHandoff(tmpdir);
		connected=false;
	}

	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"done registering for handoff");
	#endif
}

bool sqlrconnection::receiveFileDescriptor(int *descriptor) {
	bool	retval=handoffsockun.receiveFileDescriptor(descriptor);
	if (!retval) {
		handoffsockun.close();
		connected=false;
	}
	return retval;
}

void sqlrconnection::deRegisterForHandoff(char *tmpdir) {
	
	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"de-registering for handoff...");
	#endif

	// construct the name of the socket to connect to
	char	removehandoffsockname[strlen(tmpdir)+1+
					strlen(cmdl->getId())+14+1];
	sprintf(removehandoffsockname,"%s/%s-removehandoff",
					tmpdir,cmdl->getId());

	#ifdef SERVER_DEBUG
	char	string[23+strlen(removehandoffsockname)+1];
	sprintf(string,"removehandoffsockname: %s",removehandoffsockname);
	debugPrint("connection",1,string);
	#endif

	// attach to the socket and write the process id
	unixclientsocket	removehandoffsockun;
	removehandoffsockun.connect(removehandoffsockname,-1,-1,0,1);
	removehandoffsockun.write((unsigned long)getpid());

	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"done de-registering for handoff");
	#endif
}
