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

// for shmdata

class SQLRSERVER_DLLSPEC sqlrcontroller_svr : public listener {
	public:
			sqlrcontroller_svr();
		virtual	~sqlrcontroller_svr();

		const char	*connectStringValue(const char *variable);
		void		setAutoCommitBehavior(bool ac);
		void		setFakeTransactionBlocksBehavior(bool ftb);
		void		setFakeInputBinds(bool fake);
		void		setUser(const char *user);
		void		setPassword(const char *password);
		const char	*getUser();
		const char	*getPassword();
		bool		sendColumnInfo();
		void		addSessionTempTableForDrop(
						const char *tablename);
		void		addSessionTempTableForTrunc(
						const char *tablename);
		void		addTransactionTempTableForDrop(
						const char *tablename);
		void		addTransactionTempTableForTrunc(
						const char *tablename);
		virtual bool	createSharedMemoryAndSemaphores(const char *id);
		void		cleanUpAllCursorData();

		bool		getColumnNames(const char *query,
						stringbuffer *output);

		bool	init(int argc, const char **argv);
		sqlrconnection_svr	*initConnection(const char *dbase);
		bool	listen();
		void	closeConnection();

		bool	logIn(bool printerrors);
		void	logOut();
		void	reLogIn();

		sqlrcursor_svr	*initCursor();
		void	deleteCursor(sqlrcursor_svr *curs);
		bool	executeQuery(sqlrcursor_svr *curs,
						const char *query,
						uint32_t length);

		void	setUserAndGroup();
		bool	initCursors(int32_t count);
		void	incrementConnectionCount();
		void	decrementConnectionCount();
		void	decrementConnectedClientCount();
		bool	announceAvailability(const char *unixsocket,
						uint16_t inetport,
						const char *connectionid);
		void	registerForHandoff();
		void	deRegisterForHandoff();
		bool	getUnixSocket();
		bool	openSequenceFile(file *sockseq);
		bool	lockSequenceFile(file *sockseq);
		bool	getAndIncrementSequenceNumber(file *sockseq);
		bool	unLockSequenceFile(file *sockseq);


		bool		acquireAnnounceMutex();
		shmdata		*getAnnounceBuffer();
		void		signalListenerToRead();
		bool		waitForListenerToFinishReading();
		void		releaseAnnounceMutex();
		void		acquireConnectionCountMutex();
		void		signalScalerToRead();
		void		releaseConnectionCountMutex();


		sqlrcursor_svr	*getCursorById(uint16_t id);
		sqlrcursor_svr	*findAvailableCursor();
		void	closeCursors(bool destroy);
		void	setUnixSocketDirectory();
		bool	handlePidFile();
		void	initSession();
		int32_t	waitForClient();
		void	clientSession();
		sqlrprotocol_t	getClientProtocol();
		bool	autoCommitOn();
		bool	autoCommitOff();
		void	translateBeginTransaction(sqlrcursor_svr *cursor);
		bool	handleFakeTransactionQueries(sqlrcursor_svr *cursor,
						bool *wasfaketransactionquery);
		bool	beginFakeTransactionBlock();
		bool	endFakeTransactionBlock();
		bool	isBeginTransactionQuery(sqlrcursor_svr *cursor);
		bool	isCommitQuery(sqlrcursor_svr *cursor);
		bool	isRollbackQuery(sqlrcursor_svr *cursor);
		bool	begin();
		bool	commit();
		bool	rollback();
		bool	selectDatabase(const char *db);
		void	closeClientSocket(uint32_t bytes);
		void	closeSuspendedSessionSockets();
		bool	authenticate(const char *userbuffer,
						const char *passwordbuffer);
		bool	connectionBasedAuth(const char *userbuffer,
						const char *passwordbuffer);
		void	initLocalAuthentication();
		bool	authenticateLocal(const char *user,
						const char *password);
		bool	databaseBasedAuth(const char *userbuffer,
						const char *passwordbuffer);
		void	suspendSession(const char **unixsocket,
						uint16_t *inetportnumber);
		void	endSession();
		sqlrcursor_svr	*initQueryOrBindCursor(sqlrcursor_svr *cursor,
							bool reexecute,
							bool bindcursor,
							bool getquery);
		sqlrcursor_svr	*useCustomQueryHandler(sqlrcursor_svr *cursor);
		bool	handleBinds(sqlrcursor_svr *cursor);
		bool	processQueryOrBindCursor(sqlrcursor_svr *cursor,
							bool reexecute,
							bool bindcursor);
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
		void	translateBindVariablesFromMappings(
						sqlrcursor_svr *cursor);
		void	commitOrRollback(sqlrcursor_svr *cursor);
		void	returnResultSet();
		void	returnOutputBindValues(sqlrcursor_svr *cursor);
		void	returnOutputBindBlob(sqlrcursor_svr *cursor,
							uint16_t index);
		void	returnOutputBindClob(sqlrcursor_svr *cursor,
							uint16_t index);
		void	sendLobOutputBind(sqlrcursor_svr *cursor,
							uint16_t index);
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

		void	dropTempTables(sqlrcursor_svr *cursor);
		void	dropTempTable(sqlrcursor_svr *cursor,
						const char *tablename);
		void	truncateTempTables(sqlrcursor_svr *cursor);
		void	truncateTempTable(sqlrcursor_svr *cursor,
						const char *tablename);

		void	initDatabaseAvailableFileName();
		void	waitForAvailableDatabase();
		void	markDatabaseUnavailable();
		void	markDatabaseAvailable();

		bool	attemptLogIn(bool printerrors);
		void	setAutoCommit(bool ac);
		bool	openSockets();

		void	sessionStartQueries();
		void	sessionEndQueries();
		void	sessionQuery(const char *query);

		bool	initQueryLog();
		bool	writeQueryLog(sqlrcursor_svr *cursor);

		void	initConnStats();
		void	clearConnStats();
		void	updateState(enum sqlrconnectionstate_t state);
		void	updateClientSessionStartTime();
		void	updateCurrentQuery(const char *query,
						uint32_t querylen);
		void	updateClientInfo(const char *info,
						uint32_t infolen);
		void	updateClientAddr();
		void	incrementOpenDatabaseConnections();
		void	decrementOpenDatabaseConnections();
		void	incrementOpenDatabaseCursors();
		void	decrementOpenDatabaseCursors();
		void	incrementOpenClientConnections();
		void	decrementOpenClientConnections();
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

		static void	alarmHandler(int32_t signum);

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

		uint16_t	sendcolumninfo;

		uint32_t	usercount;
		char		**users;
		char		**passwords;
		char		**passwordencryptions;

		char		lastuserbuffer[USERSIZE];
		char		lastpasswordbuffer[USERSIZE];
		bool		lastauthsuccess;

		bool		commitorrollback;

		bool		autocommitforthissession;

		bool		translatebegins;
		bool		faketransactionblocks;
		bool		faketransactionblocksautocommiton;
		bool		intransactionblock;

		bool		translatebinds;

		const char	*isolationlevel;

		int32_t		accepttimeout;
		bool		suspendedsession;

		inetsocketserver	**serversockin;
		uint64_t		serversockincount;
		unixsocketserver	*serversockun;

		filedescriptor	*clientsock;

		memorypool	*bindmappingspool;
		namevaluepairs	*inbindmappings;
		namevaluepairs	*outbindmappings;

		bool		debugsqlrtranslation;
		bool		debugtriggers;

		dynamiclib		dl;
		sqlrconnection_svr	*conn;

		uint16_t	cursorcount;
		uint16_t	mincursorcount;
		uint16_t	maxcursorcount;
		sqlrcursor_svr	**cur;

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

		semaphoreset	*semset;
		sharedmemory	*idmemory;
		cmdline		*cmdl;
		sqlrconfigfile	*cfgfl;

		shmdata			*shm;
		sqlrconnstatistics	*connstats;

		char		*clientinfo;
		uint64_t	clientinfolen;

		singlylinkedlist< char * >	sessiontemptablesfordrop;
		singlylinkedlist< char * >	sessiontemptablesfortrunc;
		singlylinkedlist< char * >	transtemptablesfordrop;
		singlylinkedlist< char * >	transtemptablesfortrunc;

		int32_t		idleclienttimeout;

		bool		decrementonclose;
		bool		silent;

		stringbuffer	debugstr;

		uint64_t	maxclientinfolength;
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
};

#endif
