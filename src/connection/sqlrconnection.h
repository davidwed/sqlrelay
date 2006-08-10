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

#include <cmdline.h>

#include <defines.h>

#include <sqlrelay/sqlrclient.h>

#ifdef RUDIMENTS_NAMESPACE
using namespace rudiments;
#endif

class sqlrconnection_svr :
		public daemonprocess, public listener, public debugfile {
	friend class sqlrcursor_svr;
	public:
			sqlrconnection_svr();
		virtual	~sqlrconnection_svr();

		bool	initConnection(int argc, const char **argv,
						bool detachbeforeloggingin);
		void	listen();
		void	closeConnection();

	protected:
		// interface definition
		virtual	uint16_t	getNumberOfConnectStringVars()=0;
		virtual	void	handleConnectString()=0;
		virtual	bool	logIn()=0;
		virtual	void	logOut()=0;
		virtual	bool	changeUser(const char *newuser,
						const char *newpassword);
		virtual bool	autoCommitOn();
		virtual bool	autoCommitOff();
		virtual bool	commit();
		virtual bool	rollback();
		virtual const char	*pingQuery();
		virtual bool		ping();
		virtual const char	*identify()=0;
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

	public:
		// methods used by derived classes
		const char	*connectStringValue(const char *variable);
		void		setAutoCommitBehavior(bool ac);
		bool		getAutoCommitBehavior();
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

	private:
		// methods used internally
		bool	logInUpdateStats();
		void	logOutUpdateStats();
		sqlrcursor_svr	*initCursorUpdateStats();
		void	deleteCursorUpdateStats(sqlrcursor_svr *curs);
		bool	executeQueryUpdateStats(sqlrcursor_svr *curs,
							const char *query,
							uint32_t length,
							bool execute);

		void	setUserAndGroup();
		bool	initCursors(bool create);
		void	incrementConnectionCount();
		void	decrementConnectionCount();
		void	decrementSessionCount();
		void	announceAvailability(const char *tmpdir,
					bool passdescriptor,
					const char *unixsocket,
					uint16_t inetport,
					const char *connectionid);
		void	registerForHandoff(const char *tmpdir);
		bool	receiveFileDescriptor(int *descriptor);
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
		uint32_t	*getConnectionCountBuffer();
		void		signalScalerToRead();
		void		releaseConnectionCountMutex();
		void		acquireSessionCountMutex();
		uint32_t	*getSessionCountBuffer();
		void		releaseSessionCountMutex();


		sqlrcursor_svr	*getCursor(uint16_t command);
		sqlrcursor_svr	*findAvailableCursor();
		void	closeCursors(bool destroy);
		void	setUnixSocketDirectory();
		bool	handlePidFile();
		void	reLogIn();
		void	initSession();
		int32_t	waitForClient();
		void	clientSession();
		bool	authenticateCommand();
		void	suspendSessionCommand();
		void	endSessionCommand();
		void	pingCommand();
		void	identifyCommand();
		void	autoCommitCommand();
		void	commitCommand();
		void	rollbackCommand();
		bool	newQueryCommand(sqlrcursor_svr *cursor);
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
					bool reallyexecute);
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
		void	commitOrRollback(sqlrcursor_svr *cursor);
		bool	handleError(sqlrcursor_svr *cursor);
		bool	returnError(sqlrcursor_svr *cursor);
		void	returnResultSet();
		void	returnOutputBindValues(sqlrcursor_svr *cursor);
		void	returnResultSetHeader(sqlrcursor_svr *cursor);
		bool	returnResultSetData(sqlrcursor_svr *cursor);

		void	initDatabaseAvailableFileName();
		void	waitForAvailableDatabase();
		bool	availableDatabase();
		void	markDatabaseUnavailable();
		void	markDatabaseAvailable();

		void	blockSignals();
		bool	attemptLogIn();
		void	setInitialAutoCommitBehavior();
		bool	openSockets();

		void	flushWriteBuffer();

		char			*user;
		char			*password;

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
		bool		checkautocommit;
		bool		performautocommit;
		int32_t		accepttimeout;
		bool		suspendedsession;
		bool		lastrowvalid;
		uint64_t	lastrow;

		inetserversocket	**serversockin;
		uint64_t		serversockincount;
		unixserversocket	*serversockun;
		filedescriptor		*clientsock;

		memorypool	*bindpool;

		sqlrcursor_svr	**cur;

		stringlist	sessiontemptablesfordrop;
		stringlist	sessiontemptablesfortrunc;
		stringlist	transtemptablesfordrop;
		stringlist	transtemptablesfortrunc;

		unixclientsocket	handoffsockun;
		bool			connected;

		const char	*connectionid;
		unsigned int	ttl;

		semaphoreset	*semset;

		sqlrconnection	*sid_sqlrcon;

		char		*pidfile;

	protected:

		sharedmemory		*idmemory;
		cmdline			*cmdl;
		sqlrconfigfile		*cfgfl;

		uint32_t	maxquerysize;
		uint32_t	maxstringbindvaluelength;
		uint32_t	maxlobbindvaluelength;
		int32_t		idleclienttimeout;

#ifdef SERVER_DEBUG
		stringbuffer	*debugstr;
#endif

	public:
		sqlrstatistics	*statistics;
};


#endif
