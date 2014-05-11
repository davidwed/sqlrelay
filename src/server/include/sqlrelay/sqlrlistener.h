// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#ifndef SQLRLISTENER_H
#define SQLRLISTENER_H

#include <sqlrelay/private/sqlrserverdll.h>

#include <cmdline.h>
#include <tempdir.h>
#include <sqlrconfigfile.h>
#include <sqlrelay/sqlrloggers.h>

#include <rudiments/signalclasses.h>
#include <rudiments/listener.h>
#include <rudiments/unixsocketserver.h>
#include <rudiments/inetsocketserver.h>
#include <rudiments/filedescriptor.h>
#include <rudiments/semaphoreset.h>
#include <rudiments/sharedmemory.h>
#include <rudiments/regularexpression.h>

#include <sqlrelay/private/sqlrshmdata.h>

class SQLRSERVER_DLLSPEC handoffsocketnode {
	friend class sqlrlistener;
	private:
		uint32_t	pid;
		filedescriptor	*sock;
};

class SQLRSERVER_DLLSPEC sqlrlistener : public listener {
	public:
			sqlrlistener();
			~sqlrlistener();
		bool	initListener(int argc, const char **argv);
		void	listen();
	private:
		void	cleanUp();
		void	setUserAndGroup();
		bool	verifyAccessToConfigFile(const char *configfile);
		bool	handlePidFile(const char *id);
		void	handleDynamicScaling();
		void	setSessionHandlerMethod();
		void	setHandoffMethod();
		void	setIpPermissions();
		bool	createSharedMemoryAndSemaphores(const char *id);
		void	ipcFileError(const char *idfilename);
		void	keyError(const char *idfilename);
		void	shmError(const char *id, int shmid);
		void	semError(const char *id, int semid);
		void	setStartTime();
		bool	listenOnClientSockets();
		bool	listenOnHandoffSocket(const char *id);
		bool	listenOnDeregistrationSocket(const char *id);
		bool	listenOnFixupSocket(const char *id);
		filedescriptor	*waitForTraffic();
		bool	handleTraffic(filedescriptor *fd);
		bool	registerHandoff(filedescriptor *sock);
		bool	deRegisterHandoff(filedescriptor *sock);
		bool	fixup(filedescriptor *sock);
		bool	deniedIp(filedescriptor *clientsock);
		void	forkChild(filedescriptor *clientsock);
		static void	clientSessionThread(void *attr);
		void	clientSession(filedescriptor *clientsock);
		void    errorClientSession(filedescriptor *clientsock,
					int64_t errnum, const char *err);
		bool	acquireShmAccess();
		bool	releaseShmAccess();
		bool	acceptAvailableConnection(bool *alldbsdown);
		bool	doneAcceptingAvailableConnection();
		bool	handOffOrProxyClient(filedescriptor *sock);
		bool	getAConnection(uint32_t *connectionpid,
					uint16_t *inetport,
					char *unixportstr,
					uint16_t *unixportstrlen,
					filedescriptor *sock);
		bool	findMatchingSocket(uint32_t connectionpid,
					filedescriptor *connectionsock);
		bool	requestFixup(uint32_t connectionpid,
					filedescriptor *connectionsock);
		bool	proxyClient(pid_t connectionpid,
					filedescriptor *connectionsock,
					filedescriptor *clientsock);
		bool	connectionIsUp(const char *connectionid);
		void	pingDatabase(uint32_t connectionpid,
					const char *unixportstr,
					uint16_t inetport);
		static void	pingDatabaseThread(void *attr);
		void	pingDatabaseInternal(uint32_t connectionpid,
						const char *unixportstr,
						uint16_t inetport);
		void	waitForClientClose(bool passstatus,
					filedescriptor *clientsock);

		void		setMaxListeners(uint32_t maxlisteners);
		void		incrementMaxListenersErrors();
		void		incrementConnectedClientCount();
		void		decrementConnectedClientCount();
		uint32_t	incrementForkedListeners();
		uint32_t	decrementForkedListeners();
		void		incrementBusyListeners();
		void		decrementBusyListeners();
		int32_t		getBusyListeners();

		void	logDebugMessage(const char *info);
		void	logClientProtocolError(const char *info,
						ssize_t result);
		void	logClientConnectionRefused(const char *info);
		void	logInternalError(const char *info);

		static void	alarmHandler(int32_t signum);

	public:
		uint32_t	maxconnections;
		bool		dynamicscaling;

		int64_t		maxlisteners;
		uint64_t	listenertimeout;

		char		*pidfile;
		tempdir		*tmpdir;

		sqlrloggers		*sqlrlg;

		stringbuffer	debugstr;

		// FIXME: these shouldn't have to be pointers, right, but
		// it appears that they do have to be or their destructors don't
		// get called for some reason.
		semaphoreset	*semset;
		sharedmemory	*idmemory;
		shmdata		*shm;
		char		*idfilename;

		bool	init;

		unixsocketserver	*clientsockun;
		inetsocketserver	**clientsockin;
		uint64_t		clientsockincount;

		char	*unixport;

		unixsocketserver	*handoffsockun;
		char			*handoffsockname;
		unixsocketserver	*removehandoffsockun;
		char			*removehandoffsockname;
		unixsocketserver	*fixupsockun;
		char			*fixupsockname;

		uint16_t		handoffmode;
		handoffsocketnode	*handoffsocklist;

		regularexpression	*allowed;
		regularexpression	*denied;

		cmdline		*cmdl;

		uint32_t	maxquerysize;
		uint16_t	maxbindcount;
		uint16_t	maxbindnamelength;
		int32_t		idleclienttimeout;

		bool	isforkedchild;

		sqlrconfigfile		cfgfl;

		uint32_t	runningconnections;

		static	signalhandler		alarmhandler;
		static	volatile sig_atomic_t	alarmrang;

		bool	usethreads;
};

#endif
