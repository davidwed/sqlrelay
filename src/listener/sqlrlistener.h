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
		bool	initListener(int argc, const char **argv);
		void	listen();
	private:
		void	cleanUp();
		void	setUserAndGroup(sqlrconfigfile *cfgfl);
		bool	handlePidFile(tempdir *tmpdir, const char *id);
		void	handleDynamicScaling(sqlrconfigfile *cfgfl);
		void	setHandoffMethod(sqlrconfigfile *cfgfl);
		void	setIpPermissions(sqlrconfigfile *cfgfl);
		bool	createSharedMemoryAndSemaphores(tempdir *tmpdir,
								const char *id);
		void	ipcFileError(const char *idfilename);
		void	ftokError(const char *idfilename);
		void	shmError(const char *id, int shmid);
		void	semError(const char *id, int semid);
		bool	listenOnClientSockets(sqlrconfigfile *cfgfl);
		bool	listenOnHandoffSocket(tempdir *tmpdir, const char *id);
		bool	listenOnDeregistrationSocket(tempdir *tmpdir,
								const char *id);
		bool	listenOnFixupSocket(tempdir *tmpdir, const char *id);
		void	blockSignals();
		filedescriptor	*waitForData();
		bool	handleClientConnection(filedescriptor *fd);
		bool	registerHandoff(datatransport *sock);
		bool	deRegisterHandoff(datatransport *sock);
		bool	fixup(datatransport *sock);
		bool	deniedIp();
		void	disconnectClient(datatransport *sock);
		void	forkChild(datatransport *sock);
		void	clientSession(datatransport *sock);
		int	getAuth(datatransport *sock);
		void	incrementSessionCount();
		bool	handOffClient(datatransport *sock);
		void	getAConnection(unsigned long *connectionpid,
					unsigned short *inetport,
					char *unixportstr,
					unsigned short *unixportstrlen);
		bool	findMatchingSocket(unsigned long connectionpid,
						unixsocket *connectionsock);
		bool	requestFixup(unsigned long connectionpid,
					unixsocket *connectionsock);
		bool	connectionIsUp(const char *connectionid);
		void	pingDatabase(unsigned long connectionpid,
					const char *unixportstr,
					unsigned short inetport);
		datatransport	*connectToConnection(
						unsigned long connectionpid,
						const char *unixportstr,
						unsigned short inetport);
		void	disconnectFromConnection(datatransport *sock);
		bool	passClientFileDescriptorToConnection(
						unixsocket *connectionsock,
						int fd);
		void	waitForClientClose(int authstatus, bool passstatus,
						datatransport *sock);

		bool		passdescriptor;

		int		maxconnections;
		bool		dynamicscaling;

		char		*unixport;
		char		*pidfile;

		authenticator	*authc;

		// FIXME: these shouldn't have to be pointers, right, but
		// it appears that they do have to be or their destructors don't
		// get called.
		semaphoreset	*semset;
		sharedmemory	*idmemory;

		bool		init;

		unixserversocket	*clientsockun;
		inetserversocket	*clientsockin;
		unixserversocket	*handoffsockun;
		unixserversocket	*removehandoffsockun;
		unixserversocket	*fixupsockun;
		char			*fixupsockname;

		handoffsocketnode	**handoffsocklist;

		regularexpression	*allowed;
		regularexpression	*denied;

		cmdline			*cmdl;
};

#endif
