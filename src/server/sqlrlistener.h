// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#ifndef SQLRLISTENER_H
#define SQLRLISTENER_H

#include <config.h>

#include <cmdline.h>
#include <debugfile.h>
#include <tempdir.h>
#include <sqlrconfigfile.h>
#include <sqlrauthenticator.h>
#include <sqlrpwdencs.h>

#include <rudiments/signalclasses.h>
#include <rudiments/daemonprocess.h>
#include <rudiments/listener.h>
#include <rudiments/unixserversocket.h>
#include <rudiments/inetserversocket.h>
#include <rudiments/filedescriptor.h>
#include <rudiments/semaphoreset.h>
#include <rudiments/sharedmemory.h>
#include <rudiments/regularexpression.h>

#include <defaults.h>
#include <defines.h>

class handoffsocketnode {
	friend class sqlrlistener;
	private:
		uint32_t			pid;
		rudiments::filedescriptor	*sock;
};

class sqlrlistener : public rudiments::daemonprocess,
				public rudiments::listener {
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
		void	blockSignals();
		rudiments::filedescriptor	*waitForData();
		bool	handleClientConnection(rudiments::filedescriptor *fd);
		bool	registerHandoff(rudiments::filedescriptor *sock);
		bool	deRegisterHandoff(rudiments::filedescriptor *sock);
		bool	fixup(rudiments::filedescriptor *sock);
		bool	deniedIp(rudiments::filedescriptor *clientsock);
		void	forkChild(rudiments::filedescriptor *clientsock);
		void	clientSession(rudiments::filedescriptor *clientsock);
		void	sqlrelayClientSession(
				rudiments::filedescriptor *clientsock);
		void	mysqlClientSession(
				rudiments::filedescriptor *clientsock);
		int32_t	getAuth(rudiments::filedescriptor *clientsock);
		int32_t	getMySQLAuth(rudiments::filedescriptor *clientsock);
		void    errorClientSession(
				rudiments::filedescriptor *clientsock,
				int64_t errnum, const char *err);
		bool	acquireShmAccess();
		bool	releaseShmAccess();
		bool	acceptAvailableConnection(bool *alldbsdown);
		bool	doneAcceptingAvailableConnection();
		bool	isAlarmRang();
		bool	handOffClient(rudiments::filedescriptor *sock);
		bool	getAConnection(uint32_t *connectionpid,
					uint16_t *inetport,
					char *unixportstr,
					uint16_t *unixportstrlen,
					rudiments::filedescriptor *sock);
		bool	findMatchingSocket(uint32_t connectionpid,
				rudiments::filedescriptor *connectionsock);
		bool	requestFixup(uint32_t connectionpid,
				rudiments::filedescriptor *connectionsock);
		bool	connectionIsUp(const char *connectionid);
		void	pingDatabase(uint32_t connectionpid,
					const char *unixportstr,
					uint16_t inetport);
		rudiments::filedescriptor *connectToConnection(
					uint32_t connectionpid,
					const char *unixportstr,
					uint16_t inetport);
		bool	passClientFileDescriptorToConnection(
				rudiments::filedescriptor *connectionsock,
				int fd);
		void	waitForClientClose(int32_t authstatus,
				bool passstatus,
				rudiments::filedescriptor *clientsock);
		void	flushWriteBuffer(rudiments::filedescriptor *fd);

		static void	alarmHandler(int32_t signum);


		void		setMaxListeners(uint32_t maxlisteners);
		void		incrementMaxListenersErrors();
		void		incrementConnectedClientCount();
		void		decrementConnectedClientCount();
		uint32_t	incrementForkedListeners();
		uint32_t	decrementForkedListeners();
		void		incrementBusyListeners();
		void		decrementBusyListeners();
		int32_t		getBusyListeners();


		bool		passdescriptor;

		uint32_t	maxconnections;
		bool		dynamicscaling;

		int64_t		maxlisteners;
		uint64_t	listenertimeout;

		char		*pidfile;
		tempdir		*tmpdir;

		sqlrauthenticator	*authc;
		sqlrpwdencs		*sqlrpe;

		// FIXME: these shouldn't have to be pointers, right, but
		// it appears that they do have to be or their destructors don't
		// get called for some reason.
		rudiments::semaphoreset	*semset;
		rudiments::sharedmemory	*idmemory;
		shmdata			*shm;

		bool	init;

		rudiments::unixserversocket	*clientsockun;
		rudiments::inetserversocket	**clientsockin;
		uint64_t			clientsockincount;

		rudiments::unixserversocket	*mysqlclientsockun;
		rudiments::inetserversocket	**mysqlclientsockin;
		uint64_t			mysqlclientsockincount;

		char	*unixport;
		char	*mysqlunixport;

		clientsessiontype_t	sessiontype;

		rudiments::unixserversocket	*handoffsockun;
		rudiments::unixserversocket	*removehandoffsockun;
		rudiments::unixserversocket	*fixupsockun;
		char				*fixupsockname;

		handoffsocketnode	*handoffsocklist;

		rudiments::regularexpression	*allowed;
		rudiments::regularexpression	*denied;

		cmdline		*cmdl;

		uint32_t	maxquerysize;
		uint16_t	maxbindcount;
		uint16_t	maxbindnamelength;
		int32_t		idleclienttimeout;

		bool	isforkedchild;

		static	rudiments::signalhandler	alarmhandler;
		static	volatile sig_atomic_t		alarmrang;

		sqlrconfigfile		cfgfl;

		uint32_t	runningconnections;

		debugfile	dbgfile;
};

#endif
