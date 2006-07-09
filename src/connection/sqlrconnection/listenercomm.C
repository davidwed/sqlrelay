// Copyright (c) 1999-2004  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>
#include <rudiments/snooze.h>
#include <rudiments/process.h>
#include <rudiments/signalclasses.h>

// for sprintf
#include <stdio.h>

#include <defines.h>

void sqlrconnection_svr::announceAvailability(const char *tmpdir,
					bool passdescriptor,
					const char *unixsocket,
					unsigned short inetport,
					const char *connectionid) {

	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"announcing availability...");
	#endif

	// if we're passing around file descriptors, connect to listener 
	// if we haven't already and pass it this process's pid
	if (passdescriptor && !connected) {
		registerForHandoff(tmpdir);
	}

	// handle time-to-live
	signalmanager::alarm(ttl);

	acquireAnnounceMutex();

	// cancel time-to-live alarm
	//
	// It's important to cancel the alarm here.
	//
	// Connections which successfully announce themselves to the listener
	// cannot then die off.
	//
	// If handoff=pass, the listener can handle it if a connection dies off,
	// but not if handoff=reconnect, there's no way for it to know the
	// connection is gone.
	//
	// But, if the connection signals the listener to read the registration
	// and dies off before it receives a return signal from the listener
	// indicating that the listener has read it, then it can cause
	// problems.  And we can't simply call waitWithUndo() and
	// signalWithUndo().  Not only could the undo counter could overflow,
	// but we really only want to undo the signal() if the connection shuts
	// down before doing the wait() and there's no way to optionally
	// undo a semaphore.
	//
	// What a mess.
	signalmanager::alarm(0);

	// get a pointer to the shared memory segment
	shmdata	*idmemoryptr=getAnnounceBuffer();

	// first, write the connectionid into the segment
	charstring::copy(idmemoryptr->connectionid,
				connectionid,MAXCONNECTIONIDLEN);

	// if we're passing descriptors around, write the 
	// pid to the segment otherwise write ports
	if (passdescriptor) {

		#ifdef SERVER_DEBUG
		debugPrint("connection",1,"handoff=pass");
		#endif

		// write the pid into the segment
		idmemoryptr->connectioninfo.connectionpid=
						process::getProcessId();

	} else {

		#ifdef SERVER_DEBUG
		debugPrint("connection",1,"handoff=reconnect");
		#endif

		// convert the port to network byte order and write it into
		// the segment.
		idmemoryptr->connectioninfo.sockets.inetport=inetport;

		// write the unix socket name into the segment
		if (unixsocket && unixsocket[0]) {
			charstring::copy(idmemoryptr->connectioninfo.
							sockets.unixsocket,
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

void sqlrconnection_svr::registerForHandoff(const char *tmpdir) {

	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"registering for handoff...");
	#endif

	// construct the name of the socket to connect to
	size_t	handoffsocknamelen=charstring::length(tmpdir)+9+
				charstring::length(cmdl->getId())+8+1;
	char	*handoffsockname=new char[handoffsocknamelen];
	snprintf(handoffsockname,handoffsocknamelen,
			"%s/sockets/%s-handoff",tmpdir,cmdl->getId());

	#ifdef SERVER_DEBUG
	size_t	stringlen=17+charstring::length(handoffsockname)+1;
	char	*string=new char[stringlen];
	snprintf(string,stringlen,"handoffsockname: %s",handoffsockname);
	debugPrint("connection",1,string);
	delete[] string;
	#endif

	// Try to connect over and over forever on 1 second intervals.
	// If the connect succeeds but the write fails, loop back and
	// try again.
	connected=false;
	for (;;) {

		#ifdef SERVER_DEBUG
		debugPrint("connection",1,"trying...");
		#endif

		if (handoffsockun.connect(handoffsockname,-1,-1,1,0)==
							RESULT_SUCCESS) {
			if (handoffsockun.write(
				(uint32_t)process::getProcessId())==
							sizeof(uint32_t)) {
				connected=true;
				break;
			}
			deRegisterForHandoff(tmpdir);
		}
		snooze::macrosnooze(1);
	}

	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"done registering for handoff");
	#endif

	delete[] handoffsockname;
}

bool sqlrconnection_svr::receiveFileDescriptor(int *descriptor) {
	bool	retval=handoffsockun.receiveFileDescriptor(descriptor);
	if (!retval) {
		handoffsockun.close();
		connected=false;
	}
	return retval;
}

void sqlrconnection_svr::deRegisterForHandoff(const char *tmpdir) {
	
	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"de-registering for handoff...");
	#endif

	// construct the name of the socket to connect to
	size_t	removehandoffsocknamelen=charstring::length(tmpdir)+9+
					charstring::length(cmdl->getId())+14+1;
	char	*removehandoffsockname=new char[removehandoffsocknamelen];
	snprintf(removehandoffsockname,removehandoffsocknamelen,
			"%s/sockets/%s-removehandoff",tmpdir,cmdl->getId());

	#ifdef SERVER_DEBUG
	size_t	stringlen=23+charstring::length(removehandoffsockname)+1;
	char	*string=new char[stringlen];
	snprintf(string,stringlen,
			"removehandoffsockname: %s",removehandoffsockname);
	debugPrint("connection",1,string);
	delete[] string;
	#endif

	// attach to the socket and write the process id
	unixclientsocket	removehandoffsockun;
	removehandoffsockun.connect(removehandoffsockname,-1,-1,0,1);
	removehandoffsockun.write((uint32_t)process::getProcessId());

	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"done de-registering for handoff");
	#endif

	delete[] removehandoffsockname;
}
