// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

	private:
		void	setUserAndGroup();

		sqlrconnection_svr	*initConnection(const char *dbase);

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
		sqlrcursor_svr	*newCursor(uint16_t id);

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
		void	clientSession();

		sqlrprotocol_t	getClientProtocol();

		bool	connectionBasedAuth(const char *userbuffer,
						const char *passwordbuffer);
		void	initLocalAuthentication();
		bool	authenticateLocal(const char *user,
						const char *password);
		bool	databaseBasedAuth(const char *userbuffer,
						const char *passwordbuffer);

		bool	beginFakeTransactionBlock();
		bool	endFakeTransactionBlock();
		bool	interceptQuery(sqlrcursor_svr *cursor,
						bool *querywasintercepted);
		bool	isBeginTransactionQuery(sqlrcursor_svr *cursor);
		bool	isCommitQuery(sqlrcursor_svr *cursor);
		bool	isRollbackQuery(sqlrcursor_svr *cursor);

		void	translateBindVariablesFromMappings(
						sqlrcursor_svr *cursor);
		void	rewriteQuery(sqlrcursor_svr *cursor);
		bool	translateQuery(sqlrcursor_svr *cursor);
		void	translateBindVariables(sqlrcursor_svr *cursor);
		bool	matchesNativeBindFormat(const char *bind);
		void	translateBindVariableInStringAndMap(
					sqlrcursor_svr *cursor,
					stringbuffer *currentbind,
					uint16_t bindindex,
					stringbuffer *newquery);
		void	mapBindVariable(sqlrcursor_svr *cursor,
					const char *variablename,
					uint16_t bindindex);

		void	translateBeginTransaction(sqlrcursor_svr *cursor);

		bool	handleBinds(sqlrcursor_svr *cursor);

		void	commitOrRollback(sqlrcursor_svr *cursor);

		void	dropTempTables(sqlrcursor_svr *cursor);
		void	dropTempTable(sqlrcursor_svr *cursor,
						const char *tablename);
		void	truncateTempTables(sqlrcursor_svr *cursor);
		void	truncateTempTable(sqlrcursor_svr *cursor,
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
		bool	waitForListenerToFinishReading();

		void	acquireConnectionCountMutex();
		void	releaseConnectionCountMutex();

		void	signalScalerToRead();

		void	initConnStats();
		void	clearConnStats();

		void	updateClientSessionStartTime();
		void	updateClientAddr();


		void	sessionStartQueries();
		void	sessionEndQueries();
		void	sessionQuery(const char *query);

		static void	alarmHandler(int32_t signum);

		cmdline		*cmdl;

		sqlrconnection_svr	*conn;

		semaphoreset	*semset;
		sharedmemory	*shmem;

		sqlrprotocol			*sqlrp[SQLRPROTOCOLCOUNT];
		sqlparser			*sqlp;
		sqlrtranslations		*sqlrt;
		sqlrresultsettranslations	*sqlrrst;
		sqlwriter			*sqlw;
		sqlrtriggers			*sqlrtr;
		sqlrloggers			*sqlrlg;
		sqlrqueries			*sqlrq;
		sqlrpwdencs			*sqlrpe;
		sqlrauths			*sqlra;

		filedescriptor	*clientsock;

		const char	*user;
		const char	*password;

		bool		dbchanged;
		char		*originaldb;

		tempdir		*tmpdir;

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

		dynamiclib		dl;

		uint16_t	cursorcount;
		uint16_t	mincursorcount;
		uint16_t	maxcursorcount;
		sqlrcursor_svr	**cur;

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

		static	signalhandler		alarmhandler;
		static	volatile sig_atomic_t	alarmrang;

		singlylinkedlist< char * >	sessiontemptablesfordrop;
		singlylinkedlist< char * >	sessiontemptablesfortrunc;
		singlylinkedlist< char * >	transtemptablesfordrop;
		singlylinkedlist< char * >	transtemptablesfortrunc;
