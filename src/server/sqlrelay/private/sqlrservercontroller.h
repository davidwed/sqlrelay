// Copyright (c) 1999-2014  David Muse
// See the file COPYING for more information

	private:
		void	setUserAndGroup();

		sqlrserverconnection	*initConnection(const char *dbase);

		void	setUnixSocketDirectory();

		bool	handlePidFile();

		void	initDatabaseAvailableFileName();

		bool	getUnixSocket();

		bool	openSequenceFile(file *sockseq);
		bool	lockSequenceFile(file *sockseq);
		bool	getAndIncrementSequenceNumber(file *sockseq);
		bool	unLockSequenceFile(file *sockseq);

		bool	attemptLogIn(bool printerrors);
		bool	logIn(bool printerrors);
		void	logOut();

		void	setAutoCommit(bool ac);

		bool		initCursors(uint16_t count);
		sqlrservercursor	*newCursor(uint16_t id);

		void	incrementConnectionCount();
		void	decrementConnectionCount();

		void	markDatabaseAvailable();
		void	markDatabaseUnavailable();

		bool	openSockets();

		void	waitForAvailableDatabase();

		void	initSession();

		bool	announceAvailability(const char *unixsocket,
						uint16_t inetport,
						const char *connectionid);

		void	registerForHandoff();
		void	deRegisterForHandoff();

		int32_t	waitForClient();
		bool	getProtocol();
		void	clientSession();

		bool	beginFakeTransactionBlock();
		bool	endFakeTransactionBlock();
		bool	interceptQuery(sqlrservercursor *cursor,
						bool *querywasintercepted);
		bool	isBeginTransactionQuery(sqlrservercursor *cursor);
		bool	isCommitQuery(sqlrservercursor *cursor);
		bool	isRollbackQuery(sqlrservercursor *cursor);

		void	translateBindVariablesFromMappings(
						sqlrservercursor *cursor);
		bool	translateQuery(sqlrservercursor *cursor);
		bool	translateQueryWithParser(sqlrservercursor *cursor,
						stringbuffer *translatedquery);
		bool	translateQueryWithoutParser(sqlrservercursor *cursor,
						stringbuffer *translatedquery);
		void	translateBindVariables(sqlrservercursor *cursor);
		bool	matchesNativeBindFormat(const char *bind);
		void	translateBindVariableInStringAndMap(
					sqlrservercursor *cursor,
					stringbuffer *currentbind,
					uint16_t bindindex,
					stringbuffer *newquery);
		void	mapBindVariable(sqlrservercursor *cursor,
					const char *variablename,
					uint16_t bindindex);

		void	translateBeginTransaction(sqlrservercursor *cursor);

		bool	filterQuery(sqlrservercursor *cursor);

		bool	handleBinds(sqlrservercursor *cursor);

		void		buildColumnMaps();
		uint32_t	mapColumn(uint32_t col);
		uint32_t	mapColumnCount(uint32_t colcount);

		void	commitOrRollback(sqlrservercursor *cursor);

		void	dropTempTables(sqlrservercursor *cursor);
		void	dropTempTable(sqlrservercursor *cursor,
						const char *tablename);
		void	truncateTempTables(sqlrservercursor *cursor);
		void	truncateTempTable(sqlrservercursor *cursor,
						const char *tablename);

		void	closeSuspendedSessionSockets();

		void	shutDown();

		void	closeCursors(bool destroy);

		bool	createSharedMemoryAndSemaphores(const char *id);
		shmdata	*getAnnounceBuffer();

		void	decrementConnectedClientCount();

		bool	acquireAnnounceMutex();
		void	releaseAnnounceMutex();

		void	signalListenerToRead();
		void	unSignalListenerToRead();
		bool	waitForListenerToFinishReading();
		void	signalListenerToHandoff();

		void	acquireConnectionCountMutex();
		void	releaseConnectionCountMutex();

		void	signalScalerToRead();

		void	initConnStats();
		void	clearConnStats();

		sqlrparser	*newParser();
		sqlrparser	*newParser(const char *module,
						bool errorifnotfound);

		void	setClientSessionStartTime();
		void	setClientAddr();


		void	sessionStartQueries();
		void	sessionEndQueries();
		void	sessionQuery(const char *query);

		static void     alarmHandler(int32_t signum);

		sqlrservercontrollerprivate	*pvt;
