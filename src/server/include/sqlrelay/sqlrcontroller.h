// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#ifndef SQLRCONTROLLER_H
#define SQLRCONTROLLER_H

#include <sqlrelay/private/sqlrserverdll.h>

#include <rudiments/listener.h>
#include <rudiments/unixsocketserver.h>
#include <rudiments/inetsocketserver.h>
#include <rudiments/unixsocketclient.h>
#include <rudiments/memorypool.h>
#include <rudiments/stringbuffer.h>
#include <rudiments/regularexpression.h>
#include <rudiments/sharedmemory.h>
#include <rudiments/semaphoreset.h>
#include <rudiments/signalclasses.h>
#include <rudiments/datetime.h>
#include <rudiments/singlylinkedlist.h>

#include <tempdir.h>

#include <sqlrconfigfile.h>
#include <sqlrelay/sqlrconnection.h>
#include <sqlrelay/sqlrcursor.h>
#include <sqlrelay/sqlrprotocol.h>
#include <sqlrelay/sqlparser.h>
#include <sqlrelay/sqlrtranslations.h>
#include <sqlrelay/sqlrresultsettranslations.h>
#include <sqlrelay/sqlwriter.h>
#include <sqlrelay/sqlrtriggers.h>
#include <sqlrelay/sqlrloggers.h>
#include <sqlrelay/sqlrqueries.h>
#include <sqlrelay/sqlrpwdencs.h>
#include <sqlrelay/sqlrauths.h>

#include <sqlrelay/private/sqlrshmdata.h>

#include <cmdline.h>

class SQLRSERVER_DLLSPEC sqlrcontroller_svr : public listener {
	public:
			sqlrcontroller_svr();
		virtual	~sqlrcontroller_svr();

		// main program methods
		bool	init(int argc, const char **argv);
		bool	listen();
		void	closeConnection();

		// connect string 
		const char	*connectStringValue(const char *variable);

		// environment
		const char	*getId();
		const char	*getLogDir();
		const char	*getDebugDir();

		// log in/out of the database
		bool	logIn(bool printerrors);
		void	logOut();
		void	reLogIn();

		// client authentication
		void		setUser(const char *user);
		void		setPassword(const char *password);
		const char	*getUser();
		const char	*getPassword();
		bool		authenticate(const char *userbuffer,
						const char *passwordbuffer);
		bool		changeUser(const char *newuser,
						const char *newpassword);

		// session management
		void	beginSession();
		void	suspendSession(const char **unixsocket,
						uint16_t *inetportnumber);
		void	endSession();
		filedescriptor	*getClientSocket();
		void		closeClientSocket(uint32_t bytes);

		// database info
		const char	*getDbHostName();
		const char	*getDbIpAddress();

		// selecting which db to use
		bool	selectDatabase(const char *db);
		bool	getDbSelected();
		void	setDbSelected(bool dbselected);

		// transactions
		void	setFakeTransactionBlocksBehavior(bool ftb);
		void	setAutoCommitBehavior(bool ac);
		void	setNeedCommitOrRollback(bool needcommitorrollback);

		bool	autoCommitOn();
		bool	autoCommitOff();

		bool	begin();
		bool	commit();
		bool	rollback();

		// cursor management
		sqlrcursor_svr	*newCursor();
		sqlrcursor_svr	*getCursor(uint16_t id);
		sqlrcursor_svr	*getCursor();
		uint16_t	getCursorCount();
		void		deleteCursor(sqlrcursor_svr *curs);

		// bind variables
		void		setFakeInputBinds(bool fake);
		memorypool	*getBindMappingsPool();

		// running queries
		sqlrcursor_svr	*initNewQuery(sqlrcursor_svr *cursor);
		sqlrcursor_svr	*initReExecuteQuery(sqlrcursor_svr *cursor);
		sqlrcursor_svr	*initListQuery(sqlrcursor_svr *cursor);
		sqlrcursor_svr	*initBindCursor(sqlrcursor_svr *cursor);
		sqlrcursor_svr	*initQueryOrBindCursor(sqlrcursor_svr *cursor,
							bool reexecute,
							bool bindcursor,
							bool reinitbuffers);
		bool	processQueryOrBindCursor(sqlrcursor_svr *cursor,
							bool reexecute,
							bool bindcursor);
		bool	executeQuery(sqlrcursor_svr *curs,
							const char *query,
							uint32_t length);

		// custom query handlers
		sqlrcursor_svr	*getCustomQueryHandler(sqlrcursor_svr *cursor);

		// column info
		bool		sendColumnInfo();
		uint16_t	getSendColumnInfo();
		void		setSendColumnInfo(uint16_t sendcolumninfo);
		bool		getColumnNames(const char *query,
						stringbuffer *output);

		// temp tables
		void	addSessionTempTableForDrop(const char *tablename);
		void	addSessionTempTableForTrunc(const char *tablename);
		void	addTransactionTempTableForDrop(const char *tablename);
		void	addTransactionTempTableForTrunc(const char *tablename);

		// table name remapping
		const char	*translateTableName(const char *table);
		bool		removeReplacementTable(const char *database,
							const char *schema,
							const char *table);
		bool		removeReplacementIndex(const char *database,
							const char *schema,
							const char *table);

		// processing result sets
		bool	skipRows(sqlrcursor_svr *cursor, uint64_t rows);
		void	reformatField(sqlrcursor_svr *cursor,
						uint16_t index,
						const char *field,
						uint32_t fieldlength,
						const char **newfield,
						uint32_t *newfieldlength);
		void	reformatDateTimes(sqlrcursor_svr *cursor,
						uint16_t index,
						const char *field,
						uint32_t fieldlength,
						const char **newfield,
						uint32_t *newfieldlength,
						bool ddmm, bool yyyyddmm,
						const char *datetimeformat,
						const char *dateformat,
						const char *timeformat);
		void	closeAllResultSets();


		// states
		void	updateState(enum sqlrconnectionstate_t state);


		// logging
		bool	logEnabled();
		void	logDebugMessage(const char *info);
		void	logClientConnected();
		void	logClientConnectionRefused(const char *info);
		void	logClientDisconnected(const char *info);
		void	logClientProtocolError(sqlrcursor_svr *cursor,
							const char *info,
							ssize_t result);
		void	logDbLogIn();
		void	logDbLogOut();
		void	logDbError(sqlrcursor_svr *cursor, const char *info);
		void	logQuery(sqlrcursor_svr *cursor);
		void	logInternalError(sqlrcursor_svr *cursor,
							const char *info);

		// statistics
		void	incrementOpenDatabaseConnections();
		void	decrementOpenDatabaseConnections();
		void	incrementOpenClientConnections();
		void	decrementOpenClientConnections();
		void	incrementOpenDatabaseCursors();
		void	decrementOpenDatabaseCursors();
		void	incrementTimesNewCursorUsed();
		void	incrementTimesCursorReused();
		void	incrementQueryCounts(sqlrquerytype_t querytype);
		void	incrementTotalErrors();
		void	incrementAuthenticateCount();
		void	incrementSuspendSessionCount();
		void	incrementEndSessionCount();
		void	incrementPingCount();
		void	incrementIdentifyCount();
		void	incrementAutocommitCount();
		void	incrementBeginCount();
		void	incrementCommitCount();
		void	incrementRollbackCount();
		void	incrementDbVersionCount();
		void	incrementBindFormatCount();
		void	incrementServerVersionCount();
		void	incrementSelectDatabaseCount();
		void	incrementGetCurrentDatabaseCount();
		void	incrementGetLastInsertIdCount();
		void	incrementDbHostNameCount();
		void	incrementDbIpAddressCount();
		void	incrementNewQueryCount();
		void	incrementReexecuteQueryCount();
		void	incrementFetchFromBindCursorCount();
		void	incrementFetchResultSetCount();
		void	incrementAbortResultSetCount();
		void	incrementSuspendResultSetCount();
		void	incrementResumeResultSetCount();
		void	incrementGetDbListCount();
		void	incrementGetTableListCount();
		void	incrementGetColumnListCount();
		void	incrementGetQueryTreeCount();
		void	incrementReLogInCount();
		void	updateCurrentQuery(const char *query,
						uint32_t querylen);
		void	updateClientInfo(const char *info,
						uint32_t infolen);

		// config file
		sqlrconfigfile	*cfgfl;

		// statistics
		shmdata			*shm;
		sqlrconnstatistics	*connstats;

	protected:
		virtual bool	createSharedMemoryAndSemaphores(const char *id);

		cmdline		*cmdl;

		semaphoreset	*semset;
		sharedmemory	*idmemory;

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

		void	setAutoCommit(bool ac);

		bool	initCursors(uint16_t count);

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
		bool	handleFakeTransactionQueries(sqlrcursor_svr *cursor,
						bool *wasfaketransactionquery);
		bool	isBeginTransactionQuery(sqlrcursor_svr *cursor);
		bool	isCommitQuery(sqlrcursor_svr *cursor);
		bool	isRollbackQuery(sqlrcursor_svr *cursor);

		void	translateBindVariablesFromMappings(
						sqlrcursor_svr *cursor);
		void	rewriteQuery(sqlrcursor_svr *cursor);
		bool	translateQuery(sqlrcursor_svr *cursor);
		void	translateBindVariables(sqlrcursor_svr *cursor);
		bool	matchesNativeBindFormat(const char *bind);
		void	translateBindVariableInStringAndArray(
					sqlrcursor_svr *cursor,
					stringbuffer *currentbind,
					uint16_t bindindex,
					stringbuffer *newquery);
		void	translateBindVariableInArray(
						sqlrcursor_svr *cursor,
						const char *currentbind,
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

		void	closeCursors(bool destroy);

		shmdata		*getAnnounceBuffer();

		void	decrementConnectedClientCount();

		bool		acquireAnnounceMutex();
		void		releaseAnnounceMutex();

		void		signalListenerToRead();
		bool		waitForListenerToFinishReading();

		void		acquireConnectionCountMutex();
		void		releaseConnectionCountMutex();

		void		signalScalerToRead();

		void	initConnStats();
		void	clearConnStats();

		void	updateClientSessionStartTime();
		void	updateClientAddr();


		void	sessionStartQueries();
		void	sessionEndQueries();
		void	sessionQuery(const char *query);

		static void	alarmHandler(int32_t signum);

		sqlrconnection_svr	*conn;

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

		bool		dbselected;
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

		bool		fakeinputbinds;

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
};

#endif
