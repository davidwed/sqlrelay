// Copyright (c) 1999-2018 David Muse
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
							domnode *ln);
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
		bool	semWait(int32_t index, thread *thr,
					bool withundo, bool *timeout);
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

		void	raiseDebugMessageEvent(const char *info);
		void	raiseClientProtocolErrorEvent(const char *info,
							ssize_t result);
		void	raiseClientConnectionRefusedEvent(const char *info);
		void	raiseInternalErrorEvent(const char *info);

		static void	alarmHandler(int32_t signum);

		sqlrlistenerprivate	*pvt;
