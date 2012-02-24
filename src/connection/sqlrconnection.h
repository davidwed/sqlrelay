// Copyright (c) 1999-2004  David Muse
// See the file COPYING for more information

#ifndef SQLRCONNECTION_H
#define SQLRCONNECTION_H

// Devananda vdv
//#define RETURN_QUERY_WITH_ERROR

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

#include <sqlrcursor.h>
#include <sqlparser.h>
#include <sqltranslator.h>
#include <sqlwriter.h>
#include <sqltriggers.h>

#include <cmdline.h>

#include <defines.h>

#include <sqlrelay/sqlrclient.h>

#ifdef RUDIMENTS_NAMESPACE
using namespace rudiments;
#endif

class sqlrconnection_svr : public daemonprocess, public listener {
	friend class sqlrcursor_svr;
	friend class statusconnection;
	public:
			sqlrconnection_svr();
		virtual	~sqlrconnection_svr();

		static	int main(int argc, const char **argv,
					sqlrconnection_svr *c);

	protected:
		// interface definition
		virtual bool	supportsAuthOnDatabase();
		virtual	uint16_t	getNumberOfConnectStringVars()=0;
		virtual	void	handleConnectString()=0;
		virtual	bool	logIn(bool printerrors)=0;
		virtual void	reLogIn();
		virtual	void	logOut()=0;
		virtual	bool	changeUser(const char *newuser,
						const char *newpassword);
		virtual bool	autoCommitOn();
		virtual bool	autoCommitOff();
		virtual bool	commit();
		virtual bool	rollback();
		virtual bool	supportsTransactionBlocks();
		virtual bool		selectDatabase(const char *database,
								char **error);
		virtual const char	*selectDatabaseQuery();
		virtual char		*getCurrentDatabase();
		virtual const char	*getCurrentDatabaseQuery();
		virtual bool		getLastInsertId(uint64_t *id,
								char **error);
		virtual const char	*getLastInsertIdQuery();
		virtual bool		setIsolationLevel(const char *isolevel);
		virtual const char	*setIsolationLevelQuery();
		virtual const char	*pingQuery();
		virtual const char	*beginTransactionQuery();
		virtual bool		ping();
		virtual const char	*identify()=0;
		virtual	const char	*dbVersion()=0;
		virtual const char	*getDatabaseListQuery(bool wild);
		virtual const char	*getTableListQuery(bool wild);
		virtual const char	*getColumnListQuery(bool wild);
		virtual bool		getDatabaseList(
						sqlrcursor_svr *cursor,
						const char *wild,
						char ***cols,
						uint32_t *colcount,
						char ****rows,
						uint64_t *rowcount);
		virtual bool		getTableList(
						sqlrcursor_svr *cursor,
						const char *wild,
						char ***cols,
						uint32_t *colcount,
						char ****rows,
						uint64_t *rowcount);
		virtual bool		getColumnList(
						sqlrcursor_svr *cursor,
						const char *table,
						const char *wild,
						char ***cols,
						uint32_t *colcount,
						char ****rows,
						uint64_t *rowcount);
		virtual	const char	*bindFormat();
		virtual sqlrcursor_svr	*initCursor()=0;
		virtual void	deleteCursor(sqlrcursor_svr *curs)=0;
		virtual	int16_t	nonNullBindValue();
		virtual	int16_t	nullBindValue();
		virtual char	bindVariablePrefix();
		virtual bool	bindValueIsNull(int16_t isnull);
		virtual	bool	skipRows(sqlrcursor_svr *cursor, uint64_t rows);
		virtual bool	isTransactional();
		virtual void	setUser(const char *user);
		virtual void	setPassword(const char *password);
		virtual const char	*getUser();
		virtual const char	*getPassword();
		virtual void	dropTempTables(sqlrcursor_svr *cursor,
						stringlist *tablelist);
		virtual void	dropTempTable(sqlrcursor_svr *cursor,
						const char *tablename);
		virtual void	truncateTempTables(sqlrcursor_svr *cursor,
						stringlist *tablelist);
		virtual void	truncateTempTable(sqlrcursor_svr *cursor,
						const char *tablename);
		virtual void	endSession();

		virtual sqltranslator	*getSqlTranslator();
		virtual sqlwriter	*getSqlWriter();

	public:
		// methods used by derived classes
		const char	*connectStringValue(const char *variable);
		void		setAutoCommitBehavior(bool ac);
		bool		getAutoCommitBehavior();
		void		setFakeTransactionBlocksBehavior(bool ftb);
		void		setFakeInputBinds(bool fake);
		bool		sendColumnInfo();
		void		sendRowCounts(bool knowsactual,
						uint64_t actual,
						bool knowsaffected,
						uint64_t affected);
		void		sendColumnDefinition(const char *name, 
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
		void		sendColumnDefinitionString(const char *name, 
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
		void		sendField(const char *data, uint32_t size);
		void		sendNullField();
		void		startSendingLong(uint64_t longlength);
		void		sendLongSegment(const char *data,
						uint32_t size);
		void		endSendingLong();
		void		addSessionTempTableForDrop(
						const char *tablename);
		void		addSessionTempTableForTrunc(
						const char *tablename);
		void		addTransactionTempTableForDrop(
						const char *tablename);
		void		addTransactionTempTableForTrunc(
						const char *tablename);
		void		abortAllCursors();
		bool		createSharedMemoryAndSemaphores(
							const char *tmpdir,
							const char *id);
		void		cleanUpAllCursorData(bool freeresult,
							bool freebinds);
		virtual signalhandler	*handleSignals(
					void (*shutdownfunction)(int32_t));

		virtual	bool		getColumnNames(const char *query,
						stringbuffer *output);

	private:
		// methods used internally
		bool	initConnection(int argc, const char **argv);
		bool	listen();
		void	closeConnection();
		static void	cleanUp();
		static void	shutDown(int32_t signum);

		bool	logInUpdateStats(bool printerrors);
		void	logOutUpdateStats();

	// ideally these would be private but the
	// translators and triggers need to access them (for now)
	public:
		sqlrcursor_svr	*initCursorUpdateStats();
		void	deleteCursorUpdateStats(sqlrcursor_svr *curs);
		bool	executeQueryUpdateStats(sqlrcursor_svr *curs,
							const char *query,
							uint32_t length,
							bool execute);

	private:

		void	setUserAndGroup();
		bool	initCursors();
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
		void	endSessionCommand();
		void	selectDatabaseCommand();
		void	getCurrentDatabaseCommand();
		void	getLastInsertIdCommand();
		void	pingCommand();
		void	identifyCommand();
		void	autoCommitCommand();
		bool	autoCommitOnInternal();
		bool	autoCommitOffInternal();
		void	translateBeginTransaction(sqlrcursor_svr *cursor);
		bool	handleFakeTransactionQueries(sqlrcursor_svr *cursor,
						bool *wasfaketransactionquery,
						const char **error,
						int64_t *errno);
		bool	beginFakeTransactionBlock();
		bool	endFakeTransactionBlock();
		bool	isBeginTransactionQuery(sqlrcursor_svr *cursor);
		bool	isCommitQuery(sqlrcursor_svr *cursor);
		bool	isRollbackQuery(sqlrcursor_svr *cursor);
		void	commitCommand();
		bool	commitInternal();
		void	rollbackCommand();
		bool	rollbackInternal();
		void	dbVersionCommand();
		void	serverVersionCommand();
		void	bindFormatCommand();
		bool	newQueryCommand(sqlrcursor_svr *cursor);
		bool	newQueryInternal(sqlrcursor_svr *cursor, bool getquery);
		bool	getDatabaseListCommand(sqlrcursor_svr *cursor);
		bool	getTableListCommand(sqlrcursor_svr *cursor);
		bool	getColumnListCommand(sqlrcursor_svr *cursor);
		bool	getListCommand(sqlrcursor_svr *cursor,
						int which, bool gettable);
		bool	buildListQuery(sqlrcursor_svr *cursor,
							const char *query,
							const char *table,
							const char *wild);
		bool	getListThroughApiCall(sqlrcursor_svr *cursor,
							int which,
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
		void	waitForClientClose();
		void	closeSuspendedSessionSockets();
		bool	authenticate();
		bool	getUserFromClient();
		bool	getPasswordFromClient();
		bool	connectionBasedAuth(const char *userbuffer,
						const char *passwordbuffer);
		bool	databaseBasedAuth(const char *userbuffer,
						const char *passwordbuffer);
		int32_t	handleQuery(sqlrcursor_svr *cursor,
					bool reexecute,
					bool bindcursor,
					bool reallyexecute,
					bool getquery);
		void	clearBindMappings();
		bool	getQueryFromClient(sqlrcursor_svr *cursor,
						bool reexecute,
						bool bindcursor);
		void	resumeResultSet(sqlrcursor_svr *cursor);
		void	suspendSession();
		void	endSessionInternal();
		bool	getCommand(uint16_t *command);
		void	noAvailableCursors(uint16_t command);
		bool	getQuery(sqlrcursor_svr *cursor);
		bool	getInputBinds(sqlrcursor_svr *cursor);
		bool	getOutputBinds(sqlrcursor_svr *cursor);
		bool	getBindVarCount(uint16_t *count);
		bool	getBindVarName(bindvar_svr *bv);
		bool	getBindVarType(bindvar_svr *bv);
		void	getNullBind(bindvar_svr *bv);
		bool	getBindSize(bindvar_svr *bv, uint32_t maxsize);
		bool	getStringBind(bindvar_svr *bv);
		bool	getIntegerBind(bindvar_svr *bv);
		bool	getDoubleBind(bindvar_svr *bv);
		bool	getLobBind(bindvar_svr *bv);
		bool	getSendColumnInfo();
		bool	processQuery(sqlrcursor_svr *cursor,
						bool reexecute,
						bool bindcursor,
						bool reallyexecute);
		void	rewriteQuery(sqlrcursor_svr *cursor);
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
		bool	handleError(sqlrcursor_svr *cursor);
		void	returnError(sqlrcursor_svr *cursor,
						const char *error,
						int64_t errno);
		void	returnResultSet();
		void	returnOutputBindValues(sqlrcursor_svr *cursor);
		void	returnResultSetHeader(sqlrcursor_svr *cursor);
		bool	returnResultSetData(sqlrcursor_svr *cursor);

		void	initDatabaseAvailableFileName();
		void	waitForAvailableDatabase();
		bool	availableDatabase();
		void	markDatabaseUnavailable();
		void	markDatabaseAvailable();

		bool	attemptLogIn(bool printerrors);
		void	setInitialAutoCommitBehavior();
		bool	openSockets();

		void	sessionStartQueries();
		void	sessionEndQueries();
		void	sessionQuery(const char *query);

		void	flushWriteBuffer();

		static	sqlrconnection_svr	*conn;
		static	signalhandler		*sigh;
		static volatile sig_atomic_t	shutdowninprogress;

		char		*user;
		char		*password;

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

		char		userbuffer[USERSIZE+1];
		char		passwordbuffer[USERSIZE+1];

		char		lastuserbuffer[USERSIZE+1];
		char		lastpasswordbuffer[USERSIZE+1];
		bool		lastauthsuccess;

		bool		commitorrollback;

		bool		autocommit;
		bool		fakeautocommit;

		bool		translatebegins;
		bool		faketransactionblocks;
		bool		faketransactionblocksautocommiton;
		bool		intransactionblock;

		bool		translatebinds;

		const char	*isolationlevel;
		bool		ignoreselectdb;

		int32_t		accepttimeout;
		bool		suspendedsession;
		bool		lastrowvalid;
		uint64_t	lastrow;

		inetserversocket	**serversockin;
		uint64_t		serversockincount;
		unixserversocket	*serversockun;
		filedescriptor		*clientsock;

	// ideally these would be private but the
	// translators and triggers need to access them (for now)
	public:
		memorypool	*bindpool;
		memorypool	*bindmappingspool;
		namevaluepairs	*inbindmappings;
		namevaluepairs	*outbindmappings;

		bool		debugsqltranslation;
		bool		debugtriggers;

		uint16_t	cursorcount;

	private:

		sqlparser	*sqlp;
		sqltranslator	*sqlt;
		sqlwriter	*sqlw;
		sqltriggers	*sqltr;

		sqlrcursor_svr	**cur;

		unixclientsocket	handoffsockun;
		bool			connected;
		bool			inclientsession;
		bool			loggedin;

		bool		scalerspawned;
		const char	*connectionid;
		unsigned int	ttl;

	private:
		sqlrstatistics	*statistics;
		semaphoreset	*semset;

		sqlrconnection	*sid_sqlrcon;

		char		*pidfile;

	protected:
		bool		fakeinputbinds;

		sharedmemory		*idmemory;
		cmdline			*cmdl;
		sqlrconfigfile		*cfgfl;

		stringlist	sessiontemptablesfordrop;
		stringlist	sessiontemptablesfortrunc;
		stringlist	transtemptablesfordrop;
		stringlist	transtemptablesfortrunc;

		int32_t		idleclienttimeout;

		bool		decrementonclose;
		bool		silent;

		file		querylog;

		stringbuffer	*debugstr;
		debugfile	dbgfile;

	public:
		// derived cursor classess may need to access these
		uint32_t	maxquerysize;
		uint32_t	maxstringbindvaluelength;
		uint32_t	maxlobbindvaluelength;
};


#endif
