// Copyright (c) 1999-2014  David Muse
// See the file COPYING for more information

	private:
		void	cleanUp();
		void	setUserAndGroup();
		bool	verifyAccessToConfigUrl(const char *url);
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
		bool	listenOnClientSocket(uint16_t protocolindex,
							xmldomnode *ln);
		bool	listenOnHandoffSocket(const char *id);
		bool	listenOnDeregistrationSocket(const char *id);
		bool	listenOnFixupSocket(const char *id);
		filedescriptor	*waitForTraffic();
		bool	handleTraffic(filedescriptor *fd);
		bool	registerHandoff(filedescriptor *sock);
		bool	deRegisterHandoff(filedescriptor *sock);
		bool	fixup(filedescriptor *sock);
		bool	deniedIp(filedescriptor *clientsock);
		void	forkChild(filedescriptor *clientsock,
					uint16_t protocolindex);
		static void	clientSessionThread(void *attr);
		void	clientSession(filedescriptor *clientsock,
					uint16_t protocolindex,
					thread *thr);
		void    errorClientSession(filedescriptor *clientsock,
					int64_t errnum, const char *err);
		bool	acquireShmAccess(thread *thr, bool *timeout);
		bool	releaseShmAccess();
		bool	acceptAvailableConnection(thread *thr,
							bool *alldbsdown,
							bool *timeout);
		bool	doneAcceptingAvailableConnection();
		void	waitForConnectionToBeReadyForHandoff();
		bool	handOffOrProxyClient(filedescriptor *sock,
					uint16_t protocolindex,
					thread *thr);
		bool	getAConnection(uint32_t *connectionpid,
					uint16_t *inetport,
					char *unixportstr,
					uint16_t *unixportstrlen,
					filedescriptor *sock,
					thread *thr);
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

		listener	lsnr;

		uint32_t	maxconnections;
		bool		dynamicscaling;

		int64_t		maxlisteners;
		uint64_t	listenertimeout;

		char		*pidfile;

		sqlrcmdline	*cmdl;
		sqlrpaths	*sqlrpth;
		sqlrconfigs	*sqlrcfgs;
		sqlrconfig	*cfg;

		sqlrloggers	*sqlrlg;

		stringbuffer	debugstr;

		semaphoreset	*semset;
		sharedmemory	*shmem;
		shmdata		*shm;
		char		*idfilename;

		bool	initialized;

		inetsocketserver	**clientsockin;
		uint16_t		*clientsockinprotoindex;
		uint64_t		clientsockincount;
		uint64_t		clientsockinindex;

		unixsocketserver	**clientsockun;
		uint16_t		*clientsockunprotoindex;
		uint64_t		clientsockuncount;
		uint64_t		clientsockunindex;

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

		uint32_t	maxquerysize;
		uint16_t	maxbindcount;
		uint16_t	maxbindnamelength;
		int32_t		idleclienttimeout;

		bool	isforkedchild;
		bool	isforkedthread;

		static	signalhandler		alarmhandler;
		static	volatile sig_atomic_t	alarmrang;

		bool	usethreads;
