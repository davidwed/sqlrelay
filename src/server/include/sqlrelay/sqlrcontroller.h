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

		bool	init(int argc, const char **argv);
		bool	listen();


		// connection api...

		// connect string 
		const char	*getConnectStringValue(const char *variable);

		// environment
		const char	*getId();
		const char	*getLogDir();
		const char	*getDebugDir();

		// re-login to the database
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

		// close client connection
		void	closeClientConnection(uint32_t bytes);

		// session management
		void	beginSession();
		void	suspendSession(const char **unixsocket,
						uint16_t *inetport);
		void	endSession();

		// ping
		bool	ping();

		// database info
		const char	*identify();
		const char	*dbVersion();
		const char	*dbHostName();
		const char	*dbIpAddress();

		// bind variables
		const char	*bindFormat();
		char		bindVariablePrefix();
		int16_t		nonNullBindValue();
		int16_t		nullBindValue();
		bool		bindValueIsNull(int16_t isnull);
		void		fakeInputBinds();
		memorypool	*getBindMappingsPool();

		// db selection
		bool	selectDatabase(const char *db);
		void	dbHasChanged();
		char	*getCurrentDatabase();

		// column names
		bool	getColumnNames(const char *query, stringbuffer *output);

		// last insert id
		bool	getLastInsertId(uint64_t *id);

		// transactions
		bool	begin();
		bool	commit();
		bool	rollback();
		bool	autoCommitOn();
		bool	autoCommitOff();
		void	commitOrRollbackIsNeeded();
		void	commitOrRollbackIsNotNeeded();
		bool	setIsolationLevel(const char *isolevel);
		void	setFakeTransactionBlocksBehavior(bool ftb);
		void	setAutoCommitBehavior(bool ac);

		// errors
		void		errorMessage(char *errorbuffer,
					uint32_t errorbuffersize,
					uint32_t *errorlength,
					int64_t *errorcode,
					bool *liveconnection);
		void		clearError();
		void		setError(const char *err,
					int64_t errn, bool liveconn);
		char		*getErrorBuffer();
		uint32_t	getErrorLength();
		void		setErrorLength(uint32_t errorlength);
		uint32_t	getErrorNumber();
		void		setErrorNumber(uint32_t errnum);
		bool		getLiveConnection();
		void		setLiveConnection(bool liveconnection);

		// connection state
		void	updateState(enum sqlrconnectionstate_t state);
		void	updateCurrentQuery(const char *query,
						uint32_t querylen);
		void	updateClientInfo(const char *info,
						uint32_t infolen);


		// statistics api...
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


		// logging api...
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


		// cursor api...

		// cursor management
		sqlrcursor_svr	*newCursor();
		sqlrcursor_svr	*getCursor();
		sqlrcursor_svr	*getCursor(uint16_t id);
		uint16_t	getId(sqlrcursor_svr *cursor);
		bool		open(sqlrcursor_svr *cursor);
		bool		close(sqlrcursor_svr *cursor);
		void		suspendResultSet(sqlrcursor_svr *cursor);
		void		abort(sqlrcursor_svr *cursor);
		void		deleteCursor(sqlrcursor_svr *curs);

		// command stats
		void		setCommandStart(sqlrcursor_svr *cursor,
						uint64_t sec, uint64_t usec);
		uint64_t	getCommandStartSec(sqlrcursor_svr *cursor);
		uint64_t	getCommandStartUSec(sqlrcursor_svr *cursor);
		void		setCommandEnd(sqlrcursor_svr *cursor,
						uint64_t sec, uint64_t usec);
		uint64_t	getCommandEndSec(sqlrcursor_svr *cursor);
		uint64_t	getCommandEndUSec(sqlrcursor_svr *cursor);

		// query stats
		void		setQueryStart(sqlrcursor_svr *cursor,
						uint64_t sec, uint64_t usec);
		uint64_t	getQueryStartSec(sqlrcursor_svr *cursor);
		uint64_t	getQueryStartUSec(sqlrcursor_svr *cursor);
		void		setQueryEnd(sqlrcursor_svr *cursor,
						uint64_t sec, uint64_t usec);
		uint64_t	getQueryEndSec(sqlrcursor_svr *cursor);
		uint64_t	getQueryEndUSec(sqlrcursor_svr *cursor);

		// query buffer
		char		*getQueryBuffer(sqlrcursor_svr *cursor);
		uint32_t 	getQueryLength(sqlrcursor_svr *cursor);
		void		setQueryLength(sqlrcursor_svr *cursor,
						uint32_t querylength);

		// query tree
		xmldom		*getQueryTree(sqlrcursor_svr *cursor);

		// running queries
		void		initNewQuery(sqlrcursor_svr *cursor);
		sqlrcursor_svr	*initReExecuteQuery(sqlrcursor_svr *cursor);
		bool	prepareQuery(sqlrcursor_svr *cursor,
						const char *query,
						uint32_t length);
		bool	prepareQuery(sqlrcursor_svr *cursor,
						const char *query,
						uint32_t length,
						bool enabletranslations);
		bool	executeQuery(sqlrcursor_svr *cursor);
		bool	executeQuery(sqlrcursor_svr *cursor,
						bool enabletranslations,
						bool enabletriggers);
		bool	fetchFromBindCursor(sqlrcursor_svr *cursor);

		// input bind variables
		void		setFakeInputBindsForThisQuery(
						sqlrcursor_svr *cursor,
						bool fake);
		bool		getFakeInputBindsForThisQuery(
						sqlrcursor_svr *cursor);
		void		setInputBindCount(sqlrcursor_svr *cursor,
						uint16_t inbindcount);
		uint16_t	getInputBindCount(sqlrcursor_svr *cursor);
		bindvar_svr	*getInputBinds(sqlrcursor_svr *cursor);

		// output bind variables
		void		setOutputBindCount(sqlrcursor_svr *cursor,
						uint16_t outbindcount);
		uint16_t	getOutputBindCount(sqlrcursor_svr *cursor);
		bindvar_svr	*getOutputBinds(sqlrcursor_svr *cursor);
		bool		getLobOutputBindLength(sqlrcursor_svr *cursor,
							uint16_t index,
							uint64_t *length);
		bool		getLobOutputBindSegment(sqlrcursor_svr *cursor,
							uint16_t index,
							char *buffer,
							uint64_t buffersize,
							uint64_t offset,
							uint64_t charstoread,
							uint64_t *charsread);
		void		closeLobOutputBind(sqlrcursor_svr *cursor,
							uint16_t index);

		// custom queries
		bool		isCustomQuery(sqlrcursor_svr *cursor);
		sqlrcursor_svr	*getCustomQueryCursor(sqlrcursor_svr *cursor);

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

		// db, table, column lists
		bool		getListsByApiCalls();
		bool		getDatabaseList(sqlrcursor_svr *cursor,
							const char *wild);
		bool		getTableList(sqlrcursor_svr *cursor,
							const char *wild);
		bool		getColumnList(sqlrcursor_svr *cursor,
							const char *table,
							const char *wild);
		const char	*getDatabaseListQuery(bool wild);
		const char	*getTableListQuery(bool wild);
		const char	*getColumnListQuery(const char *table,
							bool wild);

		// column info
		uint16_t	getSendColumnInfo();
		void		setSendColumnInfo(uint16_t sendcolumninfo);
		uint32_t	colCount(sqlrcursor_svr *cursor);
		uint16_t	columnTypeFormat(sqlrcursor_svr *cursor);
		const char	*getColumnName(sqlrcursor_svr *cursor,
							uint32_t col);
		uint16_t	getColumnNameLength(sqlrcursor_svr *cursor,
							uint32_t col);
		uint16_t	getColumnType(sqlrcursor_svr *cursor,
							uint32_t col);
		const char	*getColumnTypeName(sqlrcursor_svr *cursor,
							uint32_t col);
		uint16_t	getColumnTypeNameLength(sqlrcursor_svr *cursor,
							uint32_t col);
		uint32_t	getColumnLength(sqlrcursor_svr *cursor,
							uint32_t col);
		uint32_t	getColumnPrecision(sqlrcursor_svr *cursor,
							uint32_t col);
		uint32_t	getColumnScale(sqlrcursor_svr *cursor,
							uint32_t col);
		uint16_t	getColumnIsNullable(sqlrcursor_svr *cursor,
							uint32_t col);
		uint16_t	getColumnIsPrimaryKey(sqlrcursor_svr *cursor,
							uint32_t col);
		uint16_t	getColumnIsUnique(sqlrcursor_svr *cursor,
							uint32_t col);
		uint16_t	getColumnIsPartOfKey(sqlrcursor_svr *cursor,
							uint32_t col);
		uint16_t	getColumnIsUnsigned(sqlrcursor_svr *cursor,
							uint32_t col);
		uint16_t	getColumnIsZeroFilled(sqlrcursor_svr *cursor,
							uint32_t col);
		uint16_t	getColumnIsBinary(sqlrcursor_svr *cursor,
							uint32_t col);
		uint16_t	getColumnIsAutoIncrement(sqlrcursor_svr *cursor,
							uint32_t col);

		// result set navigation
		bool		knowsRowCount(sqlrcursor_svr *cursor);
		uint64_t	rowCount(sqlrcursor_svr *cursor);
		bool		knowsAffectedRows(sqlrcursor_svr *cursor);
		uint64_t	affectedRows(sqlrcursor_svr *cursor);
		bool		noRowsToReturn(sqlrcursor_svr *cursor);
		bool		skipRow(sqlrcursor_svr *cursor);
		bool		skipRows(sqlrcursor_svr *cursor, uint64_t rows);
		bool		fetchRow(sqlrcursor_svr *cursor);
		void		nextRow(sqlrcursor_svr *cursor);
		uint64_t	getTotalRowsFetched(sqlrcursor_svr *cursor);
		void		closeResultSet(sqlrcursor_svr *cursor);
		void		closeAllResultSets();

		// fields
		void	getField(sqlrcursor_svr *cursor,
						uint32_t col,
						const char **field,
						uint64_t *fieldlength,
						bool *blob,
						bool *null);
		bool	getLobFieldLength(sqlrcursor_svr *cursor,
						uint32_t col,
						uint64_t *length);
		bool	getLobFieldSegment(sqlrcursor_svr *cursor,
						uint32_t col,
						char *buffer,
						uint64_t buffersize,
						uint64_t offset,
						uint64_t charstoread,
						uint64_t *charsread);
		void	closeLobField(sqlrcursor_svr *cursor,
						uint32_t col);
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

		// errors
		void		errorMessage(sqlrcursor_svr *cursor,
						char *errorbuffer,
						uint32_t errorbuffersize,
						uint32_t *errorlength,
						int64_t *errorcode,
						bool *liveconnection);
		void		clearError(sqlrcursor_svr *cursor);
		void		setError(sqlrcursor_svr *cursor,
						const char *err,
						int64_t errn,
						bool liveconn);
		char		*getErrorBuffer(sqlrcursor_svr *cursor);
		uint32_t	getErrorLength(sqlrcursor_svr *cursor);
		void		setErrorLength(sqlrcursor_svr *cursor,
							uint32_t errorlength);
		uint32_t	getErrorNumber(sqlrcursor_svr *cursor);
		void		setErrorNumber(sqlrcursor_svr *cursor,
							uint32_t errnum);
		bool		getLiveConnection(sqlrcursor_svr *cursor);
		void		setLiveConnection(sqlrcursor_svr *cursor,
							bool liveconnection);

		// cursor state
		void			setState(sqlrcursor_svr *cursor,
						sqlrcursorstate_t state);
		sqlrcursorstate_t	getState(sqlrcursor_svr *cursor);

		// utilities
		bool		skipComment(const char **ptr,
						const char *endptr);
		bool		skipWhitespace(const char **ptr,
						const char *endptr);
		const char	*skipWhitespaceAndComments(const char *query);

		// config file
		sqlrconfigfile	*cfgfl;

		// statistics
		shmdata			*shm;
		sqlrconnstatistics	*connstats;

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

		bool		bindswerefaked;
		bool		querywasintercepted;
		bool		executedsinceprepare;

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
};

#endif
