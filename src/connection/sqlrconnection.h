// Copyright (c) 1999-2001  David Muse
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
#include <authenticator.h>

#include <debugfile.h>
#include <tempdir.h>

#include <connection/connectioncmdline.h>
#include <connection/ipc.h>
#include <connection/scalercomm.h>
#include <connection/unixsocketseqfile.h>
#include <connection/listenercomm.h>
#include <sqlrcursor.h>


#include <defines.h>

class sqlrconnection : public daemonprocess, public listener, public debugfile {
	friend class sqlrcursor;
	public:
			sqlrconnection();
		virtual	~sqlrconnection();

		int	initConnection(int argc, const char **argv,
						int detachbeforeloggingin);
		void	listen();
		void	closeConnection();


	protected:
		// interface definition
		virtual	int	getNumberOfConnectStringVars()=0;
		virtual	void	handleConnectString()=0;
		virtual	int	logIn()=0;
		virtual	void	logOut()=0;
		virtual	int	changeUser(const char *newuser,
						const char *newpassword);
		virtual unsigned short	autoCommitOn();
		virtual unsigned short	autoCommitOff();
		virtual int	commit();
		virtual int	rollback();
		virtual char	*pingQuery();
		virtual int	ping();
		virtual char	*identify()=0;
		virtual sqlrcursor	*initCursor()=0;
		virtual void	deleteCursor(sqlrcursor *curs)=0;
		virtual	short	nonNullBindValue();
		virtual	short	nullBindValue();
		virtual char	bindVariablePrefix();
		virtual int	bindValueIsNull(short isnull);
		virtual	int	skipRows(int rows);
		virtual int	isTransactional();
		virtual void	setUser(const char *user);
		virtual void	setPassword(const char *password);
		virtual char	*getUser();
		virtual char	*getPassword();

	public:
		// methods used by derived classes
		char	*connectStringValue(const char *variable);
		void	setAutoCommitBehavior(int ac);
		int	getAutoCommitBehavior();
		int	sendColumnInfo();
		void	sendRowCounts(long actual, long affected);
		void	sendColumnCount(unsigned long ncols);
		void	sendColumnTypeFormat(unsigned short format);
		void	sendColumnDefinition(const char *name, 
						unsigned short namelen, 
						unsigned short type, 
						unsigned long size);
		void	sendColumnDefinitionString(const char *name, 
						unsigned short namelen, 
						const char *type, 
						unsigned short typelen, 
						unsigned long size);
		void	sendField(const char *data, unsigned long size);
		void	sendNullField();
		void	startSendingLong();
		void	sendLongSegment(const char *data, unsigned long size);
		void	endSendingLong();
		void	addSessionTempTable(const char *tablename);
		void	addTransactionTempTable(const char *tablename);

	private:
		// methods used internally
		int	setUserAndGroup();
		void	createCursorArray();
		int	initCursors(int create);
		int	getCursor();
		int	findAvailableCursor();
		void	closeCursors(int destroy);
		void	setUnixSocketDirectory();
		int	handlePidFile();
		void	initSharedMemoryAndSemaphoreFileName();
		int	createSharedMemoryAndSemaphores();
		void	reLogIn();
		void	initSession();
		int	waitForClient();
		void	clientSession();
		int	authenticateCommand();
		void	suspendSessionCommand();
		void	endSessionCommand();
		void	pingCommand();
		void	identifyCommand();
		void	autoCommitCommand();
		void	commitCommand();
		void	rollbackCommand();
		int	newQueryCommand();
		int	reExecuteQueryCommand();
		int	fetchFromBindCursorCommand();
		int	fetchResultSetCommand();
		void	abortResultSetCommand();
		void	suspendResultSetCommand();
		int	resumeResultSetCommand();
		void	waitForClientClose();
		void	closeSuspendedSessionSockets();
		int	authenticate();
		int	getUserFromClient();
		int	getPasswordFromClient();
		int	connectionBasedAuth(const char *userbuffer,
						const char *passwordbuffer);
		int	databaseBasedAuth(const char *userbuffer,
						const char *passwordbuffer);
		int	handleQuery(int reexecute, int reallyexecute);
		int	getQueryFromClient(int reexecute);
		void	resumeResultSet();
		void	suspendSession();
		void	endSession();
		void	dropTempTables(stringlist *tablelist);
		void	dropTempTable(const char *tablename);
		void	truncateTempTables(stringlist *tablelist);
		void	truncateTempTable(const char *tablename);
		int	getCommand(unsigned short *command);
		void	noAvailableCursors();
		int	getQuery();
		int	getInputBinds();
		int	getOutputBinds();
		int	getBindVarCount(unsigned short *count);
		int	getBindVarName(bindvar *bv);
		int	getBindVarType(bindvar *bv);
		void	getNullBind(bindvar *bv);
		int	getBindSize(bindvar *bv, unsigned long maxsize);
		int	getStringBind(bindvar *bv);
		int	getLongBind(bindvar *bv);
		int	getDoubleBind(bindvar *bv);
		int	getLobBind(bindvar *bv);
		int	getSendColumnInfo();
		int	processQuery(int reexecute, int reallyexecute);
		int	handleBinds();
		void	commitOrRollback();
		int	handleError();
		int	returnError();
		void	returnResultSet();
		void	returnOutputBindValues();
		void	returnResultSetHeader();
		int	returnResultSetData();

		long	rowsToFetch();
		long	rowsToSkip();

		void	initDatabaseAvailableFileName();
		void	waitForAvailableDatabase();
		int	availableDatabase();
		void	markDatabaseUnavailable();
		void	markDatabaseAvailable();

		void	blockSignals();
		int	attemptLogIn();
		void	setInitialAutoCommitBehavior();
		int	openSockets();

		connectioncmdline	*cmdl;
		sqlrconfigfile		*cfgfl;
		ipc			*ipcptr;
		listenercomm		*lsnrcom;
		scalercomm		*sclrcom;
		unixsocketseqfile	*ussf;

		char			*user;
		char			*password;

		tempdir			*tmpdir;

		connectstringnode	*connectstring;

		char		*updown;

		unsigned short	inetport;
		char		*unixsocket;
		char		*unixsocketptr;

		unsigned short	sendcolumninfo;

		int		init;

		authenticator	*authc;

		char		userbuffer[USERSIZE+1];
		char		passwordbuffer[USERSIZE+1];

		char		lastuserbuffer[USERSIZE+1];
		char		lastpasswordbuffer[USERSIZE+1];
		int		lastauthsuccess;

		int		commitorrollback;
		int		autocommit;
		int		checkautocommit;
		int		performautocommit;
		int		accepttimeout;
		int		suspendedsession;
		long		lastrow;

		inetserversocket	*serversockin;
		unixserversocket	*serversockun;
		datatransport		*clientsock;

		memorypool	*bindpool;

		sqlrcursor	**cur;
		short		currentcur;

		stringlist	sessiontemptables;
		stringlist	transtemptables;

	protected:
#ifdef SERVER_DEBUG
		stringbuffer	*debugstr;
#endif
};


#endif
