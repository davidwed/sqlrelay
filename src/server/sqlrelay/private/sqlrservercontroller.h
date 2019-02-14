// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

	private:
		void	setUserAndGroup();

		sqlrserverconnection	*initConnection(const char *dbase);

		bool	handlePidFile();

		void	initDatabaseAvailableFileName();

		bool	attemptLogIn(bool printerrors);
		bool	logIn(bool printerrors);
		void	logOut();

		void	setAutoCommit(bool ac);

		bool			initCursors(uint16_t count);
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
		void	endTransaction(bool commit);
		bool	endFakeTransactionBlock();
		bool	checkInterceptQuery(sqlrservercursor *cursor);
		bool	interceptQuery(sqlrservercursor *cursor);
		bool	isBeginTransactionQuery(sqlrservercursor *cursor);
		bool	isBeginTransactionQuery(const char *query);
		bool	isCommitQuery(const char *query);
		bool	isRollbackQuery(const char *query);
		bool	isAutoCommitOnQuery(const char *query);
		bool	isAutoCommitOffQuery(const char *query);
		bool	isAutoCommitQuery(const char *query, bool on);
		bool	isSetIncludingAutoCommitQuery(const char *query,
								bool *on);

		void	translateBindVariablesFromMappings(
						sqlrservercursor *cursor);
		bool	applyDirectives(sqlrservercursor *cursor);
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
					uint64_t variablenamelen,
					uint16_t bindindex);

		void	translateBeginTransaction(sqlrservercursor *cursor);

		bool	filterQuery(sqlrservercursor *cursor, bool before);

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

		void	setClientSessionStartTime();
		void	setClientAddr();


		void	sessionStartQueries();
		void	sessionEndQueries();
		void	sessionQuery(const char *query);

		static void     alarmHandler(int32_t signum);

		sqlrservercontrollerprivate	*pvt;
