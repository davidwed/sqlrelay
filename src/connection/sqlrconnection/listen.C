// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>
#include <unistd.h>

void	sqlrconnection::listen() {

bool	firsttime=true;
	for (;;) {
printf("%d: 	loop 1\n",getpid());

printf("%d: waitForAvailableDatabase()\n",getpid());
		waitForAvailableDatabase();
printf("%d: initSession()\n",getpid());
		initSession();
printf("%d: announceAvailability()\n",getpid());
		lsnrcom->announceAvailability(tmpdir->getString(),
						cfgfl->getPassDescriptor(),
						unixsocket,
						inetport,
						cmdl->getConnectionId());

		// loop to handle suspended sessions
		for (;;) {
printf("%d: 	loop 2\n",getpid());
if (!firsttime) {
	char	ch;
	while (lsnrcom->handoffsockun->read(&ch)==0) { printf("looping\n"); }
} else {
	firsttime=false;
}
printf("%d: 	waitForClient()\n",getpid());
			int	success=waitForClient();
			if (success==1) {

				suspendedsession=0;

				// have a session with the client
printf("%d: 	clientSession()\n",getpid());
				clientSession();

				// break out of the loop unless the client
				// suspended the session
				if (!suspendedsession) {
					break;
				}

			} else if (success==-1) {

				// if waitForClient() errors out, break out of
				// the suspendedsession loop and loop back
				// for another session
				break;

			} else {

				// if waitForClient() times out waiting for
				// someone to pick up the suspended
				// session, roll it back and kill it
				if (suspendedsession) {
					if (isTransactional()) {
						rollback();
					}
					suspendedsession=0;
				}
			}
printf("%d: 	end of loop 2\n",getpid());
		}

printf("%d:	decrementSessionCount()\n",getpid());
		if (cfgfl->getDynamicScaling()) {
			sclrcom->decrementSessionCount();
		}
printf("%d: 	end of loop 1\n",getpid());
	}
}

void	sqlrconnection::waitForAvailableDatabase() {

	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"waiting for available database...");
	#endif

	if (!availableDatabase()) {
		reLogIn();
		markDatabaseAvailable();
	}

	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"database is available");
	#endif
}

int	sqlrconnection::availableDatabase() {

	// return whether the file "updown" is there or not
	#ifdef SERVER_DEBUG
		if (!file::exists(updown)) {
			getDebugLogger()->write("connection",0,"database is not available");
			return 0;
		} else {
			getDebugLogger()->write("connection",0,"database is available");
			return 1;
		}
	#else
		return file::exists(updown);
	#endif
}

void	sqlrconnection::initSession() {

	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"initializing session...");
	#endif

	commitorrollback=0;
	suspendedsession=0;
	for (int i=0; i<cfgfl->getCursors(); i++) {
		cur[i]->suspendresultset=0;
	}
	accepttimeout=5;

	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"done initializing session...");
	#endif
}

int	sqlrconnection::waitForClient() {

	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"waiting for client...");
	#endif

	// Unless we're in the middle of a suspended session, if we're passing 
	// file descriptors around, wait for one to be passed to us, otherwise,
	// accept on the unix/inet sockets. 
	if (!suspendedsession && cfgfl->getPassDescriptor()) {

		// receive the descriptor and use it, if we failed to get the
		// descriptor, delete the socket and return failure
		int	descriptor;
		if (!lsnrcom->receiveFileDescriptor(&descriptor)) {
printf("pass failed\n");

			#ifdef SERVER_DEBUG
			debugPrint("connection",1,"pass failed");
			debugPrint("connection",0,"done waiting for client");
			#endif

			return -1;
		}

		clientsock=new datatransport(descriptor);

		#ifdef SERVER_DEBUG
		debugPrint("connection",1,"pass succeeded");
		debugPrint("connection",0,"done waiting for client");
		#endif

	} else {

		if (waitForNonBlockingRead(accepttimeout,0)<1) {
			#ifdef SERVER_DEBUG
			debugPrint("connection",0,"wait for non blocking read failed");
			#endif
			return -1;
		}

		// get the first socket that had data available...
		filedescriptor	*fd=NULL;
		if (!getReadyList()->getDataByIndex(0,&fd)) {
			#ifdef SERVER_DEBUG
			debugPrint("connection",0,"ready list was empty");
			#endif
			return -1;
		}

		if (fd==serversockin) {
			clientsock=serversockin->acceptClientConnection();
		} else if (fd==serversockun) {
			clientsock=serversockun->acceptClientConnection();
		}

		#ifdef SERVER_DEBUG
		if (fd) {
			debugPrint("connection",1,"reconnect succeeded");
		} else {
			debugPrint("connection",1,"reconnect failed");
		}
		debugPrint("connection",0,"done waiting for client");
		#endif

		if (!fd) {
			return -1;
		}
	}
	return 1;
}
