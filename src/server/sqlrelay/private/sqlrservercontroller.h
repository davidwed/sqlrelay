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

		bool	connectionBasedAuth(const char *userbuffer,
						const char *passwordbuffer);
		void	initLocalAuthentication();
		bool	authenticateLocal(const char *user,
						const char *password);
		bool	databaseBasedAuth(const char *userbuffer,
						const char *passwordbuffer);

		bool	beginFakeTransactionBlock();
		bool	endFakeTransactionBlock();
		bool	interceptQuery(sqlrservercursor *cursor,
						bool *querywasintercepted);
		bool	isBeginTransactionQuery(sqlrservercursor *cursor);
		bool	isCommitQuery(sqlrservercursor *cursor);
		bool	isRollbackQuery(sqlrservercursor *cursor);

		void	translateBindVariablesFromMappings(
						sqlrservercursor *cursor);
		void	rewriteQuery(sqlrservercursor *cursor);
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
		sqlrparser	*newParser(const char *module);

		void	updateClientSessionStartTime();
		void	updateClientAddr();


		void	sessionStartQueries();
		void	sessionEndQueries();
		void	sessionQuery(const char *query);

		static void     alarmHandler(int32_t signum);

		sqlrcmdline	*cmdl;

		semaphoreset	*semset;
		sharedmemory	*shmem;

		sqlrprotocols			*sqlrpr;
		sqlrparser			*sqlrp;
		sqlrtranslations		*sqlrt;
		sqlrresultsettranslations	*sqlrrst;
		sqlrtriggers			*sqlrtr;
		sqlrloggers			*sqlrlg;
		sqlrqueries			*sqlrq;
		sqlrpwdencs			*sqlrpe;
		sqlrauths			*sqlra;

		filedescriptor	*clientsock;

		char		*protocol;

		const char	*user;
		const char	*password;

		bool		dbchanged;
		char		*originaldb;

		sqlrtempdir	*tmpdir;

		connectstringcontainer	*constr;

		char		*updown;

		uint16_t	inetport;
		char		*unixsocket;
		char		*unixsocketptr;
		size_t		unixsocketptrlen;

		uint32_t	usercount;
		char		**users;
		char		**passwords;
		char		**passwordencryptions;

		char		lastuserbuffer[USERSIZE];
		char		lastpasswordbuffer[USERSIZE];
		bool		lastauthsuccess;

		bool		autocommitforthissession;

		bool		translatebegins;
		bool		faketransactionblocks;
		bool		faketransactionblocksautocommiton;
		bool		intransactionblock;

		bool		needcommitorrollback;

		bool		fakeinputbinds;
		bool		translatebinds;

		const char	*isolationlevel;

		uint16_t	sendcolumninfo;

		int32_t		accepttimeout;
		bool		suspendedsession;

		inetsocketserver	**serversockin;
		uint64_t		serversockincount;
		unixsocketserver	*serversockun;

		memorypool	*bindmappingspool;
		namevaluepairs	*inbindmappings;
		namevaluepairs	*outbindmappings;

		bool		debugsqlrtranslation;
		bool		debugtriggers;
		bool		debugbindtranslation;

		dynamiclib	conndl;
		dynamiclib	sqlrpdl;

		uint16_t	cursorcount;
		uint16_t	mincursorcount;
		uint16_t	maxcursorcount;
		sqlrservercursor	**cur;

		char		*decrypteddbpassword;

		unixsocketclient	handoffsockun;
		bool			proxymode;
		uint32_t		proxypid;

		bool		connected;
		bool		inclientsession;
		bool		loggedin;
		uint32_t	reloginseed;
		time_t		relogintime;

		bool		scalerspawned;
		const char	*connectionid;
		int32_t		ttl;

		char		*pidfile;

		int32_t		idleclienttimeout;

		bool		decrementonclose;
		bool		silent;

		char		*logdir;

		char		*debugdir;
		stringbuffer	debugstr;

		uint32_t	maxquerysize;
		uint16_t	maxbindcount;
		uint32_t	maxerrorlength;

		int64_t		loggedinsec;
		int64_t		loggedinusec;

		const char	*dbhostname;
		const char	*dbipaddress;

		bool		reformatdatetimes;
		char		*reformattedfield;
		uint32_t	reformattedfieldlength;

		static  signalhandler           alarmhandler;
		static  volatile sig_atomic_t   alarmrang;

		singlylinkedlist< char * >	sessiontemptablesfordrop;
		singlylinkedlist< char * >	sessiontemptablesfortrunc;
		singlylinkedlist< char * >	transtemptablesfordrop;
		singlylinkedlist< char * >	transtemptablesfortrunc;

		dictionary< uint32_t, uint32_t >	*columnmap;
		dictionary< uint32_t, uint32_t >	mysqldatabasecolumnmap;
		dictionary< uint32_t, uint32_t >	mysqltablescolumnmap;
		dictionary< uint32_t, uint32_t >	mysqlcolumnscolumnmap;
		dictionary< uint32_t, uint32_t >	odbcdatabasecolumnmap;
		dictionary< uint32_t, uint32_t >	odbctablescolumnmap;
		dictionary< uint32_t, uint32_t >	odbccolumnscolumnmap;
