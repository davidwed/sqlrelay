// Copyright (c) 1999-2004  David Muse
// See the file COPYING for more information

#ifndef SQLRCONNECTION_H
#define SQLRCONNECTION_H

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

#ifdef RUDIMENTS_NAMESPACE
using namespace rudiments;
#endif

class sqlrconnection : public daemonprocess, public listener, public debugfile {
	friend class sqlrcursor;
	public:
			sqlrconnection();
		virtual	~sqlrconnection();

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
		virtual sqlrcursor	*initCursor()=0;
		virtual void	deleteCursor(sqlrcursor *curs)=0;
		virtual	int16_t	nonNullBindValue();
		virtual	int16_t	nullBindValue();
		virtual char	bindVariablePrefix();
		virtual bool	bindValueIsNull(int16_t isnull);
		virtual	bool	skipRows(sqlrcursor *cursor, uint64_t rows);
		virtual bool	isTransactional();
		virtual void	setUser(const char *user);
		virtual void	setPassword(const char *password);
		virtual const char	*getUser();
		virtual const char	*getPassword();
		virtual void	dropTempTables(sqlrcursor *cursor,
						stringlist *tablelist);
		virtual void	dropTempTable(sqlrcursor *cursor,
						const char *tablename);
		virtual void	truncateTempTables(sqlrcursor *cursor,
						stringlist *tablelist);
		virtual void	truncateTempTable(sqlrcursor *cursor,
						const char *tablename);

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
		void		sendColumnCount(uint32_t ncols);
		void		sendColumnTypeFormat(uint16_t format);
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

	private:
		// methods used internally
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


		bool		createSharedMemoryAndSemaphores(
							const char *tmpdir,
							const char *id);
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


		sqlrcursor	*getCursor(uint16_t command);
		sqlrcursor	*findAvailableCursor();
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
		bool	newQueryCommand(sqlrcursor *cursor);
		bool	reExecuteQueryCommand(sqlrcursor *cursor);
		bool	fetchFromBindCursorCommand(sqlrcursor *cursor);
		bool	fetchResultSetCommand(sqlrcursor *cursor);
		void	abortResultSetCommand(sqlrcursor *cursor);
		void	suspendResultSetCommand(sqlrcursor *cursor);
		bool	resumeResultSetCommand(sqlrcursor *cursor);
		void	waitForClientClose();
		void	closeSuspendedSessionSockets();
		bool	authenticate();
		bool	getUserFromClient();
		bool	getPasswordFromClient();
		bool	connectionBasedAuth(const char *userbuffer,
						const char *passwordbuffer);
		bool	databaseBasedAuth(const char *userbuffer,
						const char *passwordbuffer);
		int32_t	handleQuery(sqlrcursor *cursor,
					bool reexecute,
					bool bindcursor,
					bool reallyexecute);
		bool	getQueryFromClient(sqlrcursor *cursor,
						bool reexecute,
						bool bindcursor);
		void	resumeResultSet(sqlrcursor *cursor);
		void	suspendSession();
		void	endSession();
		bool	getCommand(uint16_t *command);
		void	noAvailableCursors(uint16_t command);
		bool	getQuery(sqlrcursor *cursor);
		bool	getInputBinds(sqlrcursor *cursor);
		bool	getOutputBinds(sqlrcursor *cursor);
		bool	getBindVarCount(uint16_t *count);
		bool	getBindVarName(bindvar *bv);
		bool	getBindVarType(bindvar *bv);
		void	getNullBind(bindvar *bv);
		bool	getBindSize(bindvar *bv, uint32_t maxsize);
		bool	getStringBind(bindvar *bv);
		bool	getLongBind(bindvar *bv);
		bool	getDoubleBind(bindvar *bv);
		bool	getLobBind(bindvar *bv);
		bool	getSendColumnInfo();
		bool	processQuery(sqlrcursor *cursor,
						bool reexecute,
						bool bindcursor,
						bool reallyexecute);
		void	commitOrRollback(sqlrcursor *cursor);
		bool	handleError(sqlrcursor *cursor);
		bool	returnError(sqlrcursor *cursor);
		void	returnResultSet();
		void	returnOutputBindValues(sqlrcursor *cursor);
		void	returnResultSetHeader(sqlrcursor *cursor);
		bool	returnResultSetData(sqlrcursor *cursor);

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

		cmdline			*cmdl;
		sqlrconfigfile		*cfgfl;

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

		inetserversocket	*serversockin;
		unixserversocket	*serversockun;
		filedescriptor		*clientsock;

		memorypool	*bindpool;

		sqlrcursor	**cur;

		stringlist	sessiontemptablesfordrop;
		stringlist	sessiontemptablesfortrunc;
		stringlist	transtemptablesfordrop;
		stringlist	transtemptablesfortrunc;

		unixclientsocket	handoffsockun;
		bool			connected;

		const char	*connectionid;
		unsigned int	ttl;

		semaphoreset	*semset;
		sharedmemory	*idmemory;

	protected:

		uint32_t	maxquerysize;
		uint32_t	maxstringbindvaluelength;
		uint32_t	maxlobbindvaluelength;
		int32_t		idleclienttimeout;

#ifdef SERVER_DEBUG
		stringbuffer	*debugstr;
#endif
};


#endif
