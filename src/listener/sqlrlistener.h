// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#ifndef SQLRLISTENER_H
#define SQLRLISTENER_H

#include <config.h>
#include <defaults.h>
#include <rudiments/daemonprocess.h>
#include <rudiments/listener.h>
#include <rudiments/unixserversocket.h>
#include <rudiments/inetserversocket.h>
#include <rudiments/filedescriptor.h>
#include <rudiments/semaphoreset.h>
#include <rudiments/sharedmemory.h>
#include <rudiments/regularexpression.h>
#include <rudiments/signalclasses.h>
#include <authenticator.h>

#include <cmdline.h>
#include <debugfile.h>
#include <tempdir.h>
#include <sqlrconfigfile.h>

#include <rudiments/logger.h>

#include <defines.h>

#ifdef RUDIMENTS_NAMESPACE
using namespace rudiments;
#endif

class handoffsocketnode {
	friend class sqlrlistener;
	private:
		uint32_t		pid;
		filedescriptor		*sock;
};

class sqlrlistener : public daemonprocess, public listener {
	public:
			sqlrlistener();
			~sqlrlistener();
		bool	initListener(int argc, const char **argv);
		void	listen();
	private:
		void	cleanUp();
		void	setUserAndGroup();
		bool	verifyAccessToConfigFile(const char *configfile);
		bool	handlePidFile(tempdir *tmpdir, const char *id);
		void	handleDynamicScaling();
		void	setHandoffMethod();
		void	setIpPermissions();
		bool	createSharedMemoryAndSemaphores(tempdir *tmpdir,
								const char *id);
		void	ipcFileError(const char *idfilename);
		void	keyError(const char *idfilename);
		void	shmError(const char *id, int shmid);
		void	semError(const char *id, int semid);
		bool	listenOnClientSockets();
		bool	listenOnHandoffSocket(tempdir *tmpdir, const char *id);
		bool	listenOnDeregistrationSocket(tempdir *tmpdir,
								const char *id);
		bool	listenOnFixupSocket(tempdir *tmpdir, const char *id);
		void	blockSignals();
		filedescriptor	*waitForData();
		bool	handleClientConnection(filedescriptor *fd);
		bool	registerHandoff(filedescriptor *sock);
		bool	deRegisterHandoff(filedescriptor *sock);
		bool	fixup(filedescriptor *sock);
		bool	deniedIp(filedescriptor *clientsock);
		void	forkChild(filedescriptor *clientsock);
		void	clientSession(filedescriptor *clientsock);
		void	sqlrelayClientSession(filedescriptor *clientsock);
		void	mysqlClientSession(filedescriptor *clientsock);
		int32_t	getAuth(filedescriptor *clientsock);
		int32_t	getMySQLAuth(filedescriptor *clientsock);
		void    errorClientSession(filedescriptor *clientsock, const char *err);
		void	acquireSessionCountMutex();
		void	releaseSessionCountMutex();
		void	incrementSessionCount();
		void	decrementSessionCount();
		int     incForkedListeners();
		int     decForkedListeners();
		void	incBusyListeners();
		void	decBusyListeners();
		int     getBusyListeners();
		bool	handOffClient(filedescriptor *sock);
		bool	getAConnection(uint32_t *connectionpid,
					uint16_t *inetport,
					char *unixportstr,
					uint16_t *unixportstrlen);
		bool	findMatchingSocket(uint32_t connectionpid,
					filedescriptor *connectionsock);
		bool	requestFixup(uint32_t connectionpid,
					filedescriptor *connectionsock);
		bool	connectionIsUp(const char *connectionid);
		void	pingDatabase(uint32_t connectionpid,
					const char *unixportstr,
					uint16_t inetport);
		filedescriptor	*connectToConnection(uint32_t connectionpid,
						const char *unixportstr,
						uint16_t inetport);
		void	disconnectFromConnection(filedescriptor *sock);
		bool	passClientFileDescriptorToConnection(
					filedescriptor *connectionsock,
					int fd);
		void	waitForClientClose(int32_t authstatus, bool passstatus,
						filedescriptor *clientsock);
		void	flushWriteBuffer(filedescriptor *fd);

		static void	alarmHandler(int32_t signum);

		bool		passdescriptor;

		int32_t		maxconnections;
		bool		dynamicscaling;

		int64_t		maxlisteners;
		uint64_t	listenertimeout;

		char		*pidfile;

		authenticator	*authc;

		// FIXME: these shouldn't have to be pointers, right, but
		// it appears that they do have to be or their destructors don't
		// get called.
		semaphoreset	*semset;
		sharedmemory	*idmemory;

		bool		init;

		unixserversocket	*clientsockun;
		inetserversocket	**clientsockin;
		uint64_t		clientsockincount;

		unixserversocket	*mysqlclientsockun;
		inetserversocket	**mysqlclientsockin;
		uint64_t		mysqlclientsockincount;

		char			*unixport;
		char			*mysqlunixport;

		clientsessiontype_t	sessiontype;

		unixserversocket	*handoffsockun;
		unixserversocket	*removehandoffsockun;
		unixserversocket	*fixupsockun;
		char			*fixupsockname;

		handoffsocketnode	*handoffsocklist;

		regularexpression	*allowed;
		regularexpression	*denied;

		cmdline			*cmdl;

		uint32_t		maxquerysize;
		int32_t			idleclienttimeout;

		bool			isforkedchild;

		signalhandler		alarmhandler;
		static	sqlrlistener	*staticlistener;

		sqlrconfigfile		cfgfl;
		uint32_t		runningconnections;

		debugfile		dbgfile;
};

#endif
