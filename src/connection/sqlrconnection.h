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
		virtual	int	getNumberOfConnectStringVars()=0;
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
		virtual	short	nonNullBindValue();
		virtual	short	nullBindValue();
		virtual char	bindVariablePrefix();
		virtual bool	bindValueIsNull(short isnull);
		virtual	bool	skipRows(sqlrcursor *cursor,
						unsigned long rows);
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
		void		sendRowCounts(long actual, long affected);
		void		sendColumnCount(unsigned long ncols);
		void		sendColumnTypeFormat(unsigned short format);
		void		sendColumnDefinition(const char *name, 
						unsigned short namelen, 
						unsigned short type, 
						unsigned long size,
						unsigned long precision,
						unsigned long scale,
						unsigned short nullable,
						unsigned short primarykey,
						unsigned short unique,
						unsigned short partofkey,
						unsigned short unsignednumber,
						unsigned short zerofill,
						unsigned short binary,
						unsigned short autoincrement);
		void		sendColumnDefinitionString(const char *name, 
						unsigned short namelen, 
						const char *type, 
						unsigned short typelen, 
						unsigned long size,
						unsigned long precision,
						unsigned long scale,
						unsigned short nullable,
						unsigned short primarykey,
						unsigned short unique,
						unsigned short partofkey,
						unsigned short unsignednumber,
						unsigned short zerofill,
						unsigned short binary,
						unsigned short autoincrement);
		void		sendField(const char *data, unsigned long size);
		void		sendNullField();
		void		startSendingLong();
		void		sendLongSegment(const char *data,
						unsigned long size);
		void		endSendingLong();
		void		addSessionTempTableForDrop(
						const char *tablename);
		void		addSessionTempTableForTrunc(
						const char *tablename);
		void		addTransactionTempTableForDrop(
						const char *tablename);
		void		addTransactionTempTableForTrunc(
						const char *tablename);

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
					unsigned short inetport,
					const char *connectionid);
		void	registerForHandoff(const char *tmpdir);
		bool	receiveFileDescriptor(int *descriptor);
		void	deRegisterForHandoff(const char *tmpdir);
		bool	getUnixSocket(const char *tmpdir,
						char *unixsocketptr);
		int	openSequenceFile(const char *tmpdir,
						char *unixsocketptr);
		bool	lockSequenceFile();
		bool	getAndIncrementSequenceNumber(char *unixsocketptr);
		bool	unLockSequenceFile();


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
		unsigned int	*getConnectionCountBuffer();
		void		signalScalerToRead();
		void		releaseConnectionCountMutex();
		void		acquireSessionCountMutex();
		unsigned int	*getSessionCountBuffer();
		void		releaseSessionCountMutex();


		sqlrcursor	*getCursor(unsigned short command);
		sqlrcursor	*findAvailableCursor();
		void	closeCursors(bool destroy);
		void	setUnixSocketDirectory();
		bool	handlePidFile();
		void	reLogIn();
		void	initSession();
		int	waitForClient();
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
		int	handleQuery(sqlrcursor *cursor,
					bool reexecute,
					bool bindcursor,
					bool reallyexecute);
		bool	getQueryFromClient(sqlrcursor *cursor,
						bool reexecute,
						bool bindcursor);
		void	resumeResultSet(sqlrcursor *cursor);
		void	suspendSession();
		void	endSession();
		bool	getCommand(unsigned short *command);
		void	noAvailableCursors(unsigned short command);
		bool	getQuery(sqlrcursor *cursor);
		bool	getInputBinds(sqlrcursor *cursor);
		bool	getOutputBinds(sqlrcursor *cursor);
		bool	getBindVarCount(unsigned short *count);
		bool	getBindVarName(bindvar *bv);
		bool	getBindVarType(bindvar *bv);
		void	getNullBind(bindvar *bv);
		bool	getBindSize(bindvar *bv, unsigned long maxsize);
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

		long	rowsToFetch();
		long	rowsToSkip();

		void	initDatabaseAvailableFileName();
		void	waitForAvailableDatabase();
		bool	availableDatabase();
		void	markDatabaseUnavailable();
		void	markDatabaseAvailable();

		void	blockSignals();
		bool	attemptLogIn();
		void	setInitialAutoCommitBehavior();
		bool	openSockets();

		cmdline			*cmdl;
		sqlrconfigfile		*cfgfl;

		char			*user;
		char			*password;

		tempdir			*tmpdir;

		connectstringcontainer	*constr;

		char		*updown;

		unsigned short	inetport;
		char		*unixsocket;
		char		*unixsocketptr;

		unsigned short	sendcolumninfo;

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
		long		accepttimeout;
		bool		suspendedsession;
		long		lastrow;

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
		int		ttl;

		int	sockseq;

		semaphoreset	*semset;
		sharedmemory	*idmemory;

	protected:

		unsigned long	stringbindvaluelength;
		unsigned long	lobbindvaluelength;

#ifdef SERVER_DEBUG
		stringbuffer	*debugstr;
#endif
};


#endif
