// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#ifndef SQLRLISTENER_H
#define SQLRLISTENER_H

#include <config.h>
#include <defaults.h>
#include <rudiments/signalclasses.h>
#include <rudiments/daemonprocess.h>
#include <rudiments/listener.h>
#include <rudiments/unixserversocket.h>
#include <rudiments/inetserversocket.h>
#include <rudiments/datatransport.h>
#include <rudiments/semaphoreset.h>
#include <rudiments/sharedmemory.h>
#include <authenticator.h>
#include <rudiments/regularexpression.h>

#include <cmdline.h>
#include <debugfile.h>
#include <tempdir.h>
#include <sqlrconfigfile.h>

#ifdef SERVER_DEBUG
	#include <rudiments/logger.h>
#endif


#include <defines.h>

class handoffsocketnode {
	friend class sqlrlistener;
	private:
		unsigned long		pid;
		datatransport		*sock;
};

class sqlrlistener : public daemonprocess, public listener, public debugfile {
	public:
			sqlrlistener();
			~sqlrlistener();
		int	initListener(int argc, const char **argv);
		void	listen();
	private:
		void	cleanUp();
		void	setUserAndGroup(sqlrconfigfile *cfgfl);
		int	handlePidFile(tempdir *tmpdir, const char *id);
		void	handleDynamicScaling(sqlrconfigfile *cfgfl);
		void	setHandoffMethod(sqlrconfigfile *cfgfl);
		void	setIpPermissions(sqlrconfigfile *cfgfl);
		int	createSharedMemoryAndSemaphores(tempdir *tmpdir,
								const char *id);
		void	ipcFileError(const char *idfilename);
		void	ftokError(const char *idfilename);
		void	shmError(const char *id, int shmid);
		void	semError(const char *id, int semid);
		int	listenOnClientSockets(sqlrconfigfile *cfgfl);
		int	listenOnHandoffSocket(tempdir *tmpdir, const char *id);
		int	listenOnDeregistrationSocket(tempdir *tmpdir,
								const char *id);
		void	blockSignals();
		int	waitForData();
		void	handleClientConnection(int fd);
		void	registerHandoff(datatransport *sock);
		void	deRegisterHandoff(datatransport *sock);
		int	deniedIp();
		void	disconnectClient();
		void	forkChild();
		void	clientSession();
		int	getAuth();
		void	incrementSessionCount();
		int	handOffClient();
		void	getAConnection();
		int	connectionIsUp(char *connectionid);
		void	pingDatabase();
		datatransport	*connectToConnection();
		int	disconnectFromConnection(datatransport *connsock);
		int	passDescriptor();
		void	waitForClientClose(int authstatus, int passstatus);

		char		*pidfile;

		int		passdescriptor;

		int		maxconnections;
		int		dynamicscaling;

		char		*unixport;
		unsigned short	inetport;

		authenticator	*authc;

		char		unixportstr[MAXPATHLEN+1];
		unsigned short	unixportstrlen;

		semaphoreset	*semset;
		sharedmemory	*idmemory;

		int		init;

		unixserversocket	*clientsockun;
		inetserversocket	*clientsockin;
		unixserversocket	*handoffsockun;
		unixserversocket	*removehandoffsockun;
		datatransport		*clientsock;

		handoffsocketnode	**handoffsocklist;

		regularexpression	*allowed;
		regularexpression	*denied;

		unsigned long		connectionpid;

		cmdline			*cmdl;
};

#endif
