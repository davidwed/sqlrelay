// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

	public:
		sqlrservercontroller();
		~sqlrservercontroller();

		bool	init(int argc, const char **argv);
		bool	listen();

		void	reLogIn();

		bool	bulkLoadBegin(const char *id,
					const char *errorfieldtable,
					const char *errorrowtable,
					uint64_t maxerrorcount,
					bool droperrortables);
		bool	bulkLoadCheckpoint(const char *id);
		bool	bulkLoadPrepareQuery(const char *query,
						uint64_t querysize,
						uint16_t inbindcount,
						sqlrserverbindvar *inbinds);
		bool	bulkLoadCreateErrorTables(const char *query,
						uint64_t querysize,
						const char *errorfieldtable,
						const char *errorrowtable);
		bool	bulkLoadCreateErrorTable1(sqlrservercursor *cursor,
						const char *query,
						uint64_t querysize,
						const char *errorfieldtable);
		bool	bulkLoadCreateErrorTable2(sqlrservercursor *cursor,
						const char *query,
						uint64_t querysize,
						const char *errorrowtable);
		bool	bulkLoadJoin(const char *id);
		bool	bulkLoadInputBind(const byte_t *data,
						uint64_t datasize);
		void	bulkLoadParseInsert(const char *query,
						uint64_t querysize,
						char **table,
                                                linkedlist<char *> *cols,
                                                linkedlist<char *> *binds);
		bool	bulkLoadExecuteQuery();
		void	bulkLoadInitBinds();
		void	bulkLoadBindRow(const byte_t *data,
						uint64_t datasize);
		void	bulkLoadError();
		bool	bulkLoadStoreError(int64_t errorcode,
						const char *error,
						uint32_t errorsize,
						const char *errorfieldtable,
						const char *errorrowtable);
		bool	bulkLoadEnd();
		bool	bulkLoadDropErrorTables(const char *errorfieldtable,
						const char *errorrowtable);

	private:
		void	createPidFile();

		void	setUserAndGroup();

		sqlrserverconnection	*initConnection(const char *dbase);

		bool	waitForListenerStartup();

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

		bool	getAListener(const char *connectionid);

		bool	registerForHandoff();
		void	deRegisterForHandoff();

		int32_t	waitForClient();
		bool	getProtocol();
		void	clientSession();

		void	endTransaction(bool commit);
		void	clearColumnCaches();
		bool	checkInterceptQuery(sqlrservercursor *cursor);
		bool	interceptQuery(sqlrservercursor *cursor);

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
					uint64_t variablenamesize,
					uint16_t bindindex);

		void	translateBeginTransaction(sqlrservercursor *cursor);

		bool	filterQuery(sqlrservercursor *cursor, bool before);

		bool	handleBinds(sqlrservercursor *cursor);

		void		buildColumnMaps();
		void		setColumnMap();
		uint32_t	mapColumn(uint32_t col);
		uint32_t	mapColumnCount(uint32_t colcount);

		bool	handleResultSetHeader(sqlrservercursor *cursor);

		bool	reformatField(sqlrservercursor *cursor,
						const char *name,
						uint32_t index,
						const char **field,
						uint64_t *fieldsize);
		bool	reformatRow(sqlrservercursor *cursor,
						uint32_t colcount,
						const char * const *names,
						const char ***fields,
						uint64_t **fieldsizes);

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

		bool	acquireShmAccess();
		bool	resetSemaphores();
		void	signalListenerToRead();
		bool	waitForListenerToFinishReading();
		void	releaseShmAccess();

		void	acquireConnectionCountMutex();
		void	releaseConnectionCountMutex();

		void	signalScalerToRead();

		void	initConnStats();
		void	clearConnStats();

		sqlrparser	*newParser();

		void	setClientSessionStartTime();
		void	setCurrentUser(const char *user, uint32_t usersize);
		void	setClientAddr();


		void	sessionStartQueries();
		void	sessionEndQueries();
		void	sessionQuery(const char *query);

		void	getColumnsInTable(const char *table, 
					linkedlist<char *> **columns,
					const char **autoinccolumn,
					const char **primarykeycolumn);
		void	getColumnsFromInsertQuery(
					const char *start,
					const char *queryend,
					linkedlist<char *> *columns);
		void	getFirstValuesFromInsertQuery(
					const char *start,
					const char *queryend,
					linkedlist<char *> *values,
					bool *mutiinsert);
		void	deriveColumnsFromInsertQuery(
					linkedlist<char *> *values,
					linkedlist<char *> *allcolumns,
					linkedlist<char *> *columns);

		sqlrdatabaseobject *createDatabaseObject(
						const char *database,
						const char *schema,
						const char *name,
						const char *dependency);
		void	setReplacementName(
				dictionary< sqlrdatabaseobject *, char *> *dict,
				sqlrdatabaseobject *oldobject,
				const char *newobject);
		bool	getReplacementName(
				dictionary< sqlrdatabaseobject *, char *> *dict,
				const char *database,
				const char *schema,
				const char *oldobject,
				const char **newobject);
		bool	removeReplacement(
				dictionary< sqlrdatabaseobject *, char *> *dict,
				const char *database,
				const char *schema,
				const char *object);

		bool	fakePrepareAndExecuteForApiCall(
						sqlrservercursor *cursor);

		sqlrservercontrollerprivate	*pvt;
