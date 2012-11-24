// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#ifndef SQLRCONTROLLER_H
#define SQLRCONTROLLER_H

#include <config.h>
#include <defaults.h>
#include <rudiments/signalclasses.h>
#include <rudiments/daemonprocess.h>
#include <rudiments/listener.h>
#include <rudiments/unixserversocket.h>
#include <rudiments/inetserversocket.h>
#include <rudiments/unixclientsocket.h>
#include <rudiments/memorypool.h>
#include <rudiments/stringbuffer.h>
#include <rudiments/regularexpression.h>
#include <rudiments/sharedmemory.h>
#include <rudiments/semaphoreset.h>

#include <authenticator.h>
#include <debugfile.h>
#include <tempdir.h>

#include <sqlrconnection.h>
#include <sqlrcursor.h>
#include <sqlparser.h>
#include <sqltranslations.h>
#include <sqlwriter.h>
#include <sqltriggers.h>
#include <sqlrloggers.h>
#include <sqlrqueries.h>

#include <cmdline.h>

#include <defines.h>

#ifdef RUDIMENTS_NAMESPACE
using namespace rudiments;
#endif

class sqlrcontroller_svr : public daemonprocess, public listener {
	public:
		static	void main(int argc, const char **argv,
					sqlrconnection_svr *conn);

		static void	cleanUp();
		static void	shutDown(int32_t signum);

		static	sqlrcontroller_svr	*staticcont;
		static	signalhandler		*sigh;
		static volatile sig_atomic_t	shutdowninprogress;

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
		virtual bool	createSharedMemoryAndSemaphores(
							const char *tmpdir,
							const char *id);
		void		cleanUpAllCursorData(bool freeresult,
							bool freebinds);

		bool		getColumnNames(const char *query,
						stringbuffer *output);

		signalhandler	*handleSignals(
					void (*shutdownfunction)(int32_t));
		bool	init(int argc, const char **argv,
					sqlrconnection_svr *conn);
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
		void	decrementSessionCount();
		void	incrementClientSessionCount();
		void	decrementClientSessionCount();
		void	announceAvailability(const char *tmpdir,
					bool passdescriptor,
					const char *unixsocket,
					uint16_t inetport,
					const char *connectionid);
		void	registerForHandoff(const char *tmpdir);
		bool	receiveFileDescriptor(int32_t *descriptor);
		void	deRegisterForHandoff(const char *tmpdir);
		bool	getUnixSocket(const char *tmpdir,
						char *unixsocketptr);
		bool	openSequenceFile(file *sockseq,
						const char *tmpdir,
						char *unixsocketptr);
		bool	lockSequenceFile(file *sockseq);
		bool	getAndIncrementSequenceNumber(file *sockseq,
							char *unixsocketptr);
		bool	unLockSequenceFile(file *sockseq);


		void		waitForListenerToRequireAConnection();
		void		acquireAnnounceMutex();
		shmdata		*getAnnounceBuffer();
		void		signalListenerToRead();
		void		waitForListenerToFinishReading();
		void		releaseAnnounceMutex();
		void		acquireConnectionCountMutex();
		int32_t		*getConnectionCountBuffer();
		void		signalScalerToRead();
		void		releaseConnectionCountMutex();
		void		acquireSessionCountMutex();
		int32_t		*getSessionCountBuffer();
		void		releaseSessionCountMutex();


		sqlrcursor_svr	*getCursor(uint16_t command);
		sqlrcursor_svr	*findAvailableCursor();
		void	closeCursors(bool destroy);
		void	setUnixSocketDirectory();
		bool	handlePidFile();
		void	initSession();
		int32_t	waitForClient();
		void	clientSession();
		bool	authenticateCommand();
		void	suspendSessionCommand();
		void	selectDatabaseCommand();
		void	getCurrentDatabaseCommand();
		void	getLastInsertIdCommand();
		void	pingCommand();
		void	identifyCommand();
		void	autoCommitCommand();
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
		void	beginCommand();
		bool	begin();
		void	commitCommand();
		bool	commit();
		void	rollbackCommand();
		bool	rollback();
		void	dbVersionCommand();
		void	serverVersionCommand();
		void	bindFormatCommand();
		bool	newQueryCommand(sqlrcursor_svr *cursor);
		bool	getDatabaseListCommand(sqlrcursor_svr *cursor);
		bool	getTableListCommand(sqlrcursor_svr *cursor);
		bool	getColumnListCommand(sqlrcursor_svr *cursor);
		bool	getListCommand(sqlrcursor_svr *cursor,
						int which, bool gettable);
		bool	getListByApiCall(sqlrcursor_svr *cursor,
							int which,
							const char *table,
							const char *wild);
		bool	getListByQuery(sqlrcursor_svr *cursor,
							int which,
							const char *table,
							const char *wild);
		bool	buildListQuery(sqlrcursor_svr *cursor,
							const char *query,
							const char *table,
							const char *wild);
		void	escapeParameter(stringbuffer *buffer,
							const char *parameter);
		bool	reExecuteQueryCommand(sqlrcursor_svr *cursor);
		bool	fetchFromBindCursorCommand(sqlrcursor_svr *cursor);
		bool	fetchResultSetCommand(sqlrcursor_svr *cursor);
		void	abortResultSetCommand(sqlrcursor_svr *cursor);
		void	suspendResultSetCommand(sqlrcursor_svr *cursor);
		bool	resumeResultSetCommand(sqlrcursor_svr *cursor);
		void	closeClientSocket();
		void	closeSuspendedSessionSockets();
		bool	authenticate();
		bool	getUserFromClient();
		bool	getPasswordFromClient();
		bool	connectionBasedAuth(const char *userbuffer,
						const char *passwordbuffer);
		bool	databaseBasedAuth(const char *userbuffer,
						const char *passwordbuffer);
		bool	handleQueryOrBindCursor(sqlrcursor_svr *cursor,
						bool reexecute,
						bool bindcursor,
						bool getquery);
		void	endSession();
		bool	getCommand(uint16_t *command);
		void	noAvailableCursors(uint16_t command);
		bool	getClientInfo(sqlrcursor_svr *cursor);
		bool	getQuery(sqlrcursor_svr *cursor);
		bool	getInputBinds(sqlrcursor_svr *cursor);
		bool	getOutputBinds(sqlrcursor_svr *cursor);
		bool	getBindVarCount(sqlrcursor_svr *cursor,
						uint16_t *count);
		bool	getBindVarName(sqlrcursor_svr *cursor,
						bindvar_svr *bv);
		bool	getBindVarType(bindvar_svr *bv);
		void	getNullBind(bindvar_svr *bv);
		bool	getBindSize(sqlrcursor_svr *cursor,
					bindvar_svr *bv, uint32_t *maxsize);
		bool	getStringBind(sqlrcursor_svr *cursor, bindvar_svr *bv);
		bool	getIntegerBind(bindvar_svr *bv);
		bool	getDoubleBind(bindvar_svr *bv);
		bool	getDateBind(bindvar_svr *bv);
		bool	getLobBind(sqlrcursor_svr *cursor, bindvar_svr *bv);
		bool	getSendColumnInfo();
		bool	handleBinds(sqlrcursor_svr *cursor);
		bool	processQuery(sqlrcursor_svr *cursor,
						bool reexecute,
						bool bindcursor);
		void	rewriteQuery(sqlrcursor_svr *cursor);
		bool	translateQuery(sqlrcursor_svr *cursor);
		void	printQueryTree(xmldom *tree);
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
		void	returnResultSetHeader(sqlrcursor_svr *cursor);
		void	returnColumnInfo(sqlrcursor_svr *cursor,
						uint16_t format);
		bool	returnResultSetData(sqlrcursor_svr *cursor);
		void	sendRowCounts(bool knowsactual,
						uint64_t actual,
						bool knowsaffected,
						uint64_t affected);
		void	sendColumnDefinition(const char *name, 
						uint16_t namelen, 
						uint16_t type, 
						uint32_t size,
						uint32_t precision,
						uint32_t scale,
						uint16_t nullable,
						uint16_t primarykey,
						uint16_t unique,
						uint16_t partofkey,
						uint16_t unsignednumber,
						uint16_t zerofill,
						uint16_t binary,
						uint16_t autoincrement);
		void	sendColumnDefinitionString(const char *name, 
						uint16_t namelen, 
						const char *type, 
						uint16_t typelen, 
						uint32_t size,
						uint32_t precision,
						uint32_t scale,
						uint16_t nullable,
						uint16_t primarykey,
						uint16_t unique,
						uint16_t partofkey,
						uint16_t unsignednumber,
						uint16_t zerofill,
						uint16_t binary,
						uint16_t autoincrement);
		bool	skipRows(sqlrcursor_svr *cursor, uint64_t rows);
		void	returnRow(sqlrcursor_svr *cursor);
		void	sendNullField();
		void	sendField(const char *data, uint32_t size);
		void	sendLobField(sqlrcursor_svr *cursor, uint32_t col);
		void	startSendingLong(uint64_t longlength);
		void	sendLongSegment(const char *data, uint32_t size);
		void	endSendingLong();
		void	returnError(bool disconnect);
		void	returnError(sqlrcursor_svr *cursor, bool disconnect);

		void	dropTempTables(sqlrcursor_svr *cursor,
						stringlist *tablelist);
		void	dropTempTable(sqlrcursor_svr *cursor,
						const char *tablename);
		void	truncateTempTables(sqlrcursor_svr *cursor,
						stringlist *tablelist);
		void	truncateTempTable(sqlrcursor_svr *cursor,
						const char *tablename);

		void	initDatabaseAvailableFileName();
		void	waitForAvailableDatabase();
		bool	availableDatabase();
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

		void	flushWriteBuffer();

		void	initConnStats();
		void	clearConnStats();
		void	setState(enum sqlrconnectionstate state);
		void	setCurrentQuery(sqlrcursor_svr *cursor);
		void	setClientInfo();
		void	setClientAddr();
		void	incrementOpenServerConnections();
		void	decrementOpenServerConnections();
		void	incrementOpenClientConnections();
		void	decrementOpenClientConnections();
		void	incrementOpenServerCursors();
		void	decrementOpenServerCursors();
		void	incrementTimesNewCursorUsed();
		void	incrementTimesCursorReused();
		void	incrementTotalQueries();
		void	incrementTotalErrors();

		const char	*user;
		const char	*password;

		bool		dbselected;
		char		*originaldb;

		tempdir			*tmpdir;

		connectstringcontainer	*constr;

		char		*updown;

		uint16_t	inetport;
		char		*unixsocket;
		char		*unixsocketptr;

		uint16_t	sendcolumninfo;

		authenticator	*authc;

		char		userbuffer[USERSIZE];
		char		passwordbuffer[USERSIZE];

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
		bool		ignoreselectdb;

		int32_t		accepttimeout;
		bool		suspendedsession;

		inetserversocket	**serversockin;
		uint64_t		serversockincount;
		unixserversocket	*serversockun;

		uint32_t	handoffindex;

		filedescriptor	*clientsock;

		memorypool	*bindpool;
		memorypool	*bindmappingspool;
		namevaluepairs	*inbindmappings;
		namevaluepairs	*outbindmappings;

		bool		debugsqltranslation;
		bool		debugtriggers;

		sqlrconnection_svr	*conn;

		uint16_t	cursorcount;
		uint16_t	mincursorcount;
		uint16_t	maxcursorcount;
		sqlrcursor_svr	**cur;

		sqlparser	*sqlp;
		sqltranslations	*sqlt;
		sqlwriter	*sqlw;
		sqltriggers	*sqltr;
		sqlrloggers	*sqlrlg;
		sqlrqueries	*sqlrq;

		unixclientsocket	handoffsockun;

		bool		connected;
		bool		inclientsession;
		bool		loggedin;

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
		sqlrstatistics		*stats;
		sqlrconnstatistics	*connstats;

		char		*clientinfo;
		uint64_t	clientinfolen;

		stringlist	sessiontemptablesfordrop;
		stringlist	sessiontemptablesfortrunc;
		stringlist	transtemptablesfordrop;
		stringlist	transtemptablesfortrunc;

		int32_t		idleclienttimeout;

		bool		decrementonclose;
		bool		silent;

		stringbuffer	*debugstr;

		debugfile	dbgfile;
		uint64_t	maxclientinfolength;
		uint32_t	maxquerysize;
		uint16_t	maxbindcount;
		uint16_t	maxbindnamelength;
		uint32_t	maxstringbindvaluelength;
		uint32_t	maxlobbindvaluelength;
		uint32_t	maxerrorlength;

		int64_t		loggedinsec;
		int64_t		loggedinusec;
};


#endif
