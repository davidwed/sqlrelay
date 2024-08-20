// Copyright (c) 1999-2019 David Muse
// See the file COPYING for more information

#ifndef SQLRSERVER_H
#define SQLRSERVER_H

#include <sqlrelay/private/sqlrserverincludes.h>

enum sqlrcursorstate_t {
	SQLRCURSORSTATE_AVAILABLE=0,
	SQLRCURSORSTATE_BUSY,
	SQLRCURSORSTATE_SUSPENDED
};

enum sqlrquerytype_t {
	SQLRQUERYTYPE_SELECT=0,
	SQLRQUERYTYPE_INSERT,
	SQLRQUERYTYPE_INSERTSELECT,
	SQLRQUERYTYPE_SELECTINTO,
	SQLRQUERYTYPE_MULTIINSERT,
	SQLRQUERYTYPE_UPDATE,
	SQLRQUERYTYPE_DELETE,
	SQLRQUERYTYPE_CREATE,
	SQLRQUERYTYPE_DROP,
	SQLRQUERYTYPE_ALTER,
	SQLRQUERYTYPE_CUSTOM,
	SQLRQUERYTYPE_ETC,
	SQLRQUERYTYPE_BEGIN,
	SQLRQUERYTYPE_COMMIT,
	SQLRQUERYTYPE_ROLLBACK,
	SQLRQUERYTYPE_AUTOCOMMIT_ON,
	SQLRQUERYTYPE_AUTOCOMMIT_OFF,
	SQLRQUERYTYPE_SET_INCLUDING_AUTOCOMMIT_ON,
	SQLRQUERYTYPE_SET_INCLUDING_AUTOCOMMIT_OFF
};

enum sqlrquerystatus_t {
	SQLRQUERYSTATUS_SUCCESS=0,
	SQLRQUERYSTATUS_ERROR,
	SQLRQUERYSTATUS_FILTER_VIOLATION
};

enum sqlrserverbindvartype_t {
	SQLRSERVERBINDVARTYPE_NULL=0,
	SQLRSERVERBINDVARTYPE_STRING,
	SQLRSERVERBINDVARTYPE_INTEGER,
	SQLRSERVERBINDVARTYPE_DOUBLE,
	SQLRSERVERBINDVARTYPE_BLOB,
	SQLRSERVERBINDVARTYPE_CLOB,
	SQLRSERVERBINDVARTYPE_CURSOR,
	SQLRSERVERBINDVARTYPE_DATE,

	// special types for bulk load
	SQLRSERVERBINDVARTYPE_DELIMITER,
	SQLRSERVERBINDVARTYPE_NEWLINE
};

enum sqlrserverlistformat_t {
	SQLRSERVERLISTFORMAT_NULL=0,
	SQLRSERVERLISTFORMAT_MYSQL,
	SQLRSERVERLISTFORMAT_POSTGRESQL,
	SQLRSERVERLISTFORMAT_ODBC,
	SQLRSERVERLISTFORMAT_JDBC
};

enum sqlrevent_t {
	SQLREVENT_CLIENT_CONNECTED=0,
	SQLREVENT_CLIENT_CONNECTION_REFUSED,
	SQLREVENT_CLIENT_DISCONNECTED,
	SQLREVENT_CLIENT_PROTOCOL_ERROR,
	SQLREVENT_DB_LOGIN,
	SQLREVENT_DB_LOGOUT,
	SQLREVENT_DB_ERROR,
	SQLREVENT_DB_WARNING,
	SQLREVENT_QUERY_RECEIVED,
	SQLREVENT_QUERY_PREPARED,
	SQLREVENT_QUERY_EXECUTED,
	SQLREVENT_FILTER_VIOLATION,
	SQLREVENT_INTERNAL_ERROR,
	SQLREVENT_INTERNAL_WARNING,
	SQLREVENT_DEBUG_MESSAGE,
	SQLREVENT_SCHEDULE_VIOLATION,
	SQLREVENT_INTEGRITY_VIOLATION,
	SQLREVENT_TRANSLATION_FAILURE,
	SQLREVENT_PARSE_FAILURE,
	SQLREVENT_CURSOR_OPEN,
	SQLREVENT_CURSOR_CLOSE,
	SQLREVENT_BEGIN_TRANSACTION,
	SQLREVENT_COMMIT,
	SQLREVENT_ROLLBACK,
	SQLREVENT_INVALID_EVENT
};

enum sqlrloglevel_t {
	SQLRLOGGER_LOGLEVEL_DEBUG=0,
	SQLRLOGGER_LOGLEVEL_INFO,
	SQLRLOGGER_LOGLEVEL_WARNING,
	SQLRLOGGER_LOGLEVEL_ERROR
};

class SQLRSERVER_DLLSPEC sqlrserverbindvar {
	public:
		char	*variable;
		int16_t	variablesize;
		union {
			char	*stringval;
			int64_t	integerval;
			struct	{
				double		value;
				uint32_t	precision;
				uint32_t	scale;
			} doubleval;
			struct {
				int16_t		year;
				int16_t		month;
				int16_t		day;
				int16_t		hour;
				int16_t		minute;
				int16_t		second;
				int32_t		microsecond;
				char		*tz;
				bool		isnegative;
			} dateval;
			uint16_t	cursorid;
		} value;
		uint32_t		valuesize;
		uint32_t		resultvaluesize;
		sqlrserverbindvartype_t	type;
		byte_t			nativetype;
		int16_t			isnull;
};

class SQLRSERVER_DLLSPEC sqlrserverbase {
	public:
		/** Creates an instance of sqlrserverbase. */
		sqlrserverbase();

		/** Deletes this instance of sqlrserverbase. */
		virtual ~sqlrserverbase();

		/** Sets the debug flag to "debug".  Defaults to false. */
		void	setDebug(bool debug);

		/** Returns the debug flag as set by setDebug() or false if
		 *  setDebug() was never called. */
		bool	getDebug();

		/** Starts a section of debug, writing "..." formatted using
		 *  "title". */
		void	debugStart(const char *title, ...);

		/** Writes "..." formatted using "string" as a single line of
		 *  debug. */
		void	debugWrite(const char *string, ...);

		/** Dumps "size" bytes of "data" as hex. */
		void	debugHexDump(const byte_t *data, uint64_t size);

		/** Ends a section of debug. */
		void	debugEnd();

	#include <sqlrelay/private/sqlrserverbase.h>
};

class SQLRSERVER_DLLSPEC sqlrlistener : public sqlrserverbase {
	public:

		/** Returns the id of this instance. */
		const char	*getId();

		/** Returns the paths for this instance. */
		sqlrpaths	*getPaths();

		/** Returns a string representation of log level "level". */
		const char	*getLogLevel(sqlrloglevel_t level);

		/** Returns the sqlrloglevel_t corresponding to string
 		 *  "level". */
		sqlrloglevel_t	getLogLevel(const char *level);

		/** Returns a string representation of event type "event". */
		const char	*getEventType(sqlrevent_t event);

		/** Returns the sqlrevent_t corresponding to string "event". */
		sqlrevent_t	getEventType(const char *event);

	#include <sqlrelay/private/sqlrlistener.h>
};

class SQLRSERVER_DLLSPEC sqlrservercontroller : public sqlrserverbase {
	public:
		// connect string...

		/** Returns the value of parameter "variable", of the string
		 *  attribute, of the connection tag, in the config file. */
		const char	*getConnectStringValue(const char *variable);

		/** Sets the user that will be used to connect to the database
		 *  to "user".
		 * 
		 *  During initialization of the sqlr-connection, this is set
		 *  to the value of the "user" parameter, of the string
		 *  attribute, of the connection tag, in the config file.
		 *
		 *  This method may be called by a module to override that. */
		void	setUser(const char *user);

		/** Returns the user that will be used to connect to the
		 *  database.
		 *
		 *  Unless overridden by a call to setUser(), this will be the
		 *  value of the "user" parameter, of the string attribute, of
		 *  the connection tag, in the config file. */
		const char	*getUser();

		/** Sets the password that will be used to connect to the
		 *  database to "password".
		 * 
		 *  During initialization of the sqlr-connection, this is set
		 *  to the value of the "password" parameter, of the string
		 *  attribute, of the connection tag, in the config file.
		 *
		 *  This method may be called by a module to override that. */
		void	setPassword(const char *password);

		/** Returns the password that will be used to connect to the
		 *  database.
		 *
		 *  Unless overridden by a call to setPassword(), this will be
		 *  the value of the "password" parameter, of the string
		 *  attribute, of the connection tag, in the config file. */
		const char	*getPassword();

		/** Sets the connect timeout.
		 * 
		 *  During initialization of the sqlr-connection, this is set
		 *  to the value of the "connecttimeout" parameter, of the
		 *  string attribute, of the connection tag, in the config
		 *  file.
		 *
		 *  This method may be called by a module to override that. */
		void	setConnectTimeout(uint64_t connecttimeout);

		/** Returns the connect timeout.
		 *
		 *  Unless overridden by a call to setConnectTimeout(), this
		 *  will be the value of the "connecttimeout" parameter, of the
		 *  string attribute, of the connection tag, in the config
		 *  file. */
		uint64_t	getConnectTimeout();

		/** Sets the query timeout.
		 * 
		 *  During initialization of the sqlr-connection, this is set
		 *  to the value of the "querytimeout" parameter, of the
		 *  string attribute, of the connection tag, in the config
		 *  file.
		 *
		 *  This method may be called by a module to override that. */
		void	setQueryTimeout(uint64_t querytimeout);

		/** Returns the query timeout.
		 *
		 *  Unless overridden by a call to setQueryTimeout(), this
		 *  will be the value of the "querytimeout" attribute, of the
		 *  string attribute, of the connection tag, in the config
		 *  file. */
		uint64_t	getQueryTimeout();

		/** Sets whether or not to execute direct.
		 * 
		 *  During initialization of the sqlr-connection, this is set
		 *  to the value of the "executedirect" parameter, of the
		 *  string attribute, of the connection tag, in the config
		 *  file.
		 *
		 *  This method may be called by a module to override that. */
		void	setExecuteDirect(bool executedirect);

		/** Returns whether or not to execute direct.
		 *
		 *  Unless overridden by a call to setExecuteDirect(), this
		 *  will be the value of the "executedirect" parameter, of the
		 *  string attribute, of the connection tag, in the config
		 *  file. */
		bool	getExecuteDirect();

		/** Sets the database type, overriding the value returned by
		 *  the connection module.
		 *
		 *  Calling setDbType(NULL) causes getDbType() to return the
		 *  value returned by the connection module.
		 * 
		 *  During initialization of the sqlr-connection, this is set
		 *  to the value of the "dbtype" parameter, of the string
		 *  attribute, of the connection tag, in the config file,
		 *  if present. */
		void	setDbType(const char *dbtype);



		// environment...

		/** Returns the id of the connection - the value of the id
		 *  attribute, of the instance tag, in the config file. */
		const char	*getId();

		/** Returns the connection id of the connection - the value
		 *  of the connectionid attribute, of the connection tag,
		 *  in the config file. */
		const char	*getConnectionId();



		// passthrough...

		/** Sends "size" bytes of "data" to the database. */
		bool	send(byte_t *data, size_t size);

		/** Receives "size" bytes from the database into buffer
		 *  "data". */
		bool	recv(byte_t **data, size_t *size);



		// client auth...

		/** Authenticates "cred".
		 *  
		 *  Returns true if authentication was successful and false
		 *  otherwise. */
		bool	auth(sqlrcredentials *cred);

		/** Logs out of the database and back in as "newuser",
		 *  authenticated using password "newpassword".
		 *
		 *  Returns true if login was successful and false otherwise. */
		bool	changeUser(const char *newuser,
						const char *newpassword);

		/** Switches from the current proxied user to "newuser" using
		 *  password "newpassword".
		 *
		 *  Returns true if successful and false otherwise. */
		bool	changeProxiedUser(const char *newuser,
						const char *newpassword);



		// password encryption...

		/** Returns the password encryption module corresponding to
		 *  "id". */
		sqlrpwdenc	*getPasswordEncryptionById(const char *id);



		// close client connection...

		/** Attempts to read "bytes" bytes from the client, in
		 *  non-blocking mode, before closing the connection to the
		 *  client. */
		void	closeClientConnection(uint32_t bytes);



		// session management...

		/** Begins a session with a client. */
		void	beginSession();

		/** Aborts all cursors that haven't already been suspended and
		 *  opens a unix socket and inet port that the client can
		 *  connect to to resume the session later.
		 *
		 *  Returns the unix socket in "unixsocket" and inet port in
		 *  "inetport". */
		void	suspendSession(const char **unixsocket,
						uint16_t *inetport);

		/** Ends the session with a client. */
		void	endSession();



		// ping...

		/** Pings the database using the "ping query" defined by the
		 *  database connection module.
		 *
		 *  Returns true if the ping succeeded and false if it
		 *  failed. */
		bool	ping();



		// database info...

		/** Returns the type of database: oracle, mysql, postgresql,
		 *  odbc, etc. as reported by the connection module, or as
		 *  overridden by a call to setDbType() or by the "dbtype"
		 *  parameter, of the string attribute, of the connection tag,
		 *  in the config file. */
		const char	*getDbType();

		/** Returns the database version. */
		const char	*getDbVersion();

		/** Returns the host name of the server hosting the
		 *  database. */
		const char	*getDbHostName();

		/** Returns the IP address of the server hosting the
		 *  database. */
		const char	*getDbIpAddress();



		// bind variables...

		/** Returns a string representing the bind variable format used
		 *  by the database.  For example:
		 *
		 *  ?  - database uses a ? to represent a bind variable
		 *  @* - database uses a @ followed by any characters to
		 *       represent a bind variable
		 *  $1 - database uses a $ followed by a number to represent a
		 *       bind variable
		 *  :* - database uses a : followed by any characters to
		 *       represent a bind variable
		 *
		 *  Returns :* by default but may be overriden by a child
		 *  class. */
		const char	*getBindFormat();

		/** Returns the value that the database expects or returns in
		 *  the "null indicator" for a non-null bind value.
		 *
		 *  Returns 0 by default, but may be overriden by a child
		 *  class. */
		int16_t	getNonNullBindValue();

		/** Returns the value that the database expects or returns in
		 *  the "null indicator" for a null bind value.
		 *
		 *  Returns -1 by default but may be overriden by a child
		 *  class. */
		int16_t	getNullBindValue();

		/** Returns true if "isnull" matches the value that the database
		 *  expects or returns in the "null indicator" for a null bind
		 *  value. */
		bool	getBindValueIsNull(int16_t isnull);

		/** If "fake" is true then the server will fake input binds by
		 *  rewriting the query, rather than by using actual bind
		 *  variables.  If "fake" is false then the server will use
		 *  actual bind variables. */
		void	setFakeInputBinds(bool fake);

		/** Returns true if the server will fake input binds by
		 *  rewriting the query, or false otherwise. */
		bool	getFakeInputBinds();



		// sequences...

		/** Returns a string representing the format of the sequence
		 *  nextval command used in the database.  The format will
		 *  contain a %s in place of the sequence name.  For example:
		 *
		 *  (nextval for %s)
		 *  next value for %s
		 *  nextval('%s')
		 *  %s.nextval
		 *
		 *  Returns an empty string if the database does not support
		 *  sequences.
		 *
		 *  Returns %s.nextval by default but may be overriden by a
		 *  child class. */
		const char	*getNextvalFormat();



		// fetch info...

		/** Sets the number of rows to fetch at once to
		 *  "fetchatonce". */
		void	setFetchAtOnce(uint32_t fethatonce);

		/** Returns the number of rows that will be fetched at once. */
		uint32_t	getFetchAtOnce();

		/** Sets the maximum number of columns that a result set can
		 *  contain to "maxcolumncount".  Additional columns will be
		 *  truncated. */
		void	setMaxColumnCount(uint32_t maxcolumncount);

		/** Returns the number of columns that a result set can
		 *  contain. */
		uint32_t	getMaxColumnCount();

		/** Sets the maximum number of bytes that a non-LOB field can
		 *  contain to "maxfieldsize".  Additional bytes will be
		 *  truncated. */
		void	setMaxFieldSize(uint32_t maxfieldsize);

		/** Returns the maximum number of bytes that a non-LOB field
		 *  can contain. */
		uint32_t	getMaxFieldSize();



		// db selection...

		/** Selects database "db".
		 *
		 *  Returns true if selection succeeded and false otherwise. */
		bool	selectDatabase(const char *db);

		/** Returns the current database.
		 *
		 *  Note that this method allocates a buffer for the return
		 *  value internally and returns it.  The calling method must
		 *  deallocate this buffer. */
		char	*getCurrentDatabase();

		/** Returns the current database.
		 *
		 *  Note that this method allocates a buffer for the return
		 *  value internally and returns it.  The calling method must
		 *  deallocate this buffer. */
		char	*getCurrentSchema();



		// last insert id...

		/** Gets the last insert it and populates "id" with it.
		 *
		 *  Returns true on success and false on failure. */
		bool	getLastInsertId(uint64_t *id);



		// noop query...

		/** Returns the no-op query for this the database.  That is, a
		 *  query that safely doesn't do anything - doesn't modify any
		 *  tables, return any result set, or set any parameters.  */
		const char	*getNoopQuery();



		// transactions...

		/** Begins a new transaction.
		 *
		 *  Returns true on success and false on failure. */
		bool	begin();

		/** Commits the current transaction.
		 * 
		 *  Returns true on success and false on failure. */
		bool	commit();

		/** Rolls the current transaction back.
		 *
		 *  Returns true on success and false on failure. */
		bool	rollback();

		/** Set auto-commit on.
		 *  
		 *  Returns true on success and false on failure. */
		bool	setAutoCommitOn();

		/** Set auto-commit off.
		 *  
		 *  Returns true on success and false on failure. */
		bool	setAutoCommitOff();

		/** Sets a flag indicating whether a DML query has been run and
		 *  thus whether a commit or rollback is needed under certain
		 *  circumstances.
		 *
		 *  If the flag is set true...
		 *
		 *  During endSession(), for transactional databases, a commit
		 *  or rollback is executed, depending on whether
		 *  endofsession="commit" or endofsession="rollback" is set in
		 *  the instance tag, in the config file.
		 *
		 *  After each query, for transactional databases that don't
		 *  support transaction blocks or auto-commit, if we're faking
		 *  auto-commit then a commit is executed. */
		void	setNeedsCommitOrRollback(bool needed);

		/** Returns whether a commit or rollback is needed, as set by
		 *  setNeedsCommitOrRollback(). */
		bool	getNeedsCommitOrRollback();

		/** Sets the isolation level to "isolevel".
		 *  
		 *  Returns true on success and false on failure. */
		bool	setIsolationLevel(const char *isolevel);

		/** If "ftb" is true then transaction blocks are faked by
		 *  setting autocommit on and off as appropriate.  If "ftb" is
		 *  false then begin and commit/rollback queries are run to
		 *  begin/end transaction blocks. */
		void	setFakeTransactionBlocks(bool ftb);

		/** Returns whether transaction blocks are being faked, as set
		 *  by setFakeTransactionBlocks(). */
		bool	getFakeTransactionBlocks();

		/** If "fac" is true then auto-commit is faked by executing a
		 *  commit after each query.  If "fac" is false then native
		 *  auto-commit is used.
		 *
		 *  See setNeedsCommitOrRollback() for caveats. */
		void	setFakeAutoCommit(bool fac);

		/** Returns whether auto-commit is being faked, as set by
		 *  setFakeAutoCommit(). */
		bool	getFakeAutoCommit();

		/** If "iac" is true then the initial auto-commit behavior is
		 *  set to auto-commit-on.  If "iac" is false then the initial
		 *  auto-commit behavior is set to auto-commit-off.
		 *
		 *  During a session, auto-commit may be turned on or off.
		 *  During endSession() and reLogIn(), the auto-commit behavior
		 *  is reset to the initial auto-commit behavior as defined
		 *  by this method. */
		void	setInitialAutoCommit(bool iac);

		/** Returns whether the initial auto-commit behavior is set to
		 *  auto-commit-on as set by setInitialAutoCommit(). */
		bool	getInitialAutoCommit();

		/** Returns true if we're currently in a transaction and false
		 *  otherwise. */
		bool	getInTransaction();

		// errors...

		/** Fetches the current connection-level error into the
		 *  connection's error buffer, unless there is already an
		 *  error saved in the buffer. */
		void	saveError();

		/** Fetches the current cursor-level error into the
		 *  cursor's error buffer, unless there is already an
		 *  error saved in the buffer.
		 *
		 *  FIXME: how is this distinct from saveError(cursor)? */
		void	saveErrorFromCursor(sqlrservercursor *cursor);

		/** Returns the error message and code by:
		 *
		 *  Setting "errorbuffer" to the connection-level error buffer.
		 *  Populating "errorsize" with the number of bytes in the
		 *  error buffer.
		 *  Populating "errorcode" with the connection-level numeric
		 *  error code.
		 *  Populating "liveconnection" with true if the connection to
		 *  the database is still up.
		 *  Populating "liveconnection" with true if the connection to
		 *  the database is down. */
		void	getError(const char **errorbuffer,
						uint32_t *errorsize,
						int64_t *errorcode,
						bool *liveconnection);

		/** Returns the error message and code by:
		 *
		 *  Copying at most "errorbuffersize" bytes from the
		 *  connection-level error buffer into "errorbuffer".
		 *  Populating "errorsize" with the number of bytes in the
		 *  error buffer.
		 *  Populating "errorcode" with the connection-level numeric
		 *  error code.
		 *  Populating "liveconnection" with true if the connection to
		 *  the database is still up.
		 *  Populating "liveconnection" with true if the connection to
		 *  the database is down. */
		void	getError(char *errorbuffer,
						uint32_t errorbuffersize,
						uint32_t *errorsize,
						int64_t *errorcode,
						bool *liveconnection);

		/** Empties the connection-level error buffer and sets a flag
		 *  indicating that the connection to the database is up. */
		void	clearError();

		/** Copies "errsize" bytes of "err" into the connection-level
		 *  error buffer, sets the connection-level numeric error code
		 *  to "errn" and sets a flag indicating that the connection to
		 *  the database is up. */
		void	setError(const char *err,
					uint32_t errsize,
					int64_t errn,
					bool liveconn);

		/** Copies "err" into the connection-level error buffer,
		 *  sets the connection-level numeric error code to "errn" and
		 *  sets a flag indicating that the connection to the database
		 *  is up. */
		void	setError(const char *err, int64_t errn, bool liveconn);

		/** Returns a pointer to the connection-level error buffer. */
		char	*getErrorBuffer();

		/** Returns the size, in bytes, of the connection-level error
		 *  buffer. */
		uint32_t	getErrorBufferSize();

		/** Sets the number of bytes currently stored in the
		 *  connection-level error buffer to "errorsize". */
		void	setErrorSize(uint32_t errorsize);

		/** Returns the number of bytes currently stored in the
		 *  connection-level error buffer, as set by setErrorSize(). */
		uint32_t	getErrorSize();

		/** Sets the connection-level numeric error code to "errnum". */
		void	setErrorNumber(uint32_t errnum);

		/** Returns the connection-level numeric error code as set by
		 *  setErrorNumber(). */
		uint32_t	getErrorNumber();

		/** Sets a flag indicating whether the connection to the
		 *  database is up to "liveconnection". */
		void	setLiveConnection(bool liveconnection);

		/** Returns the flag indicating whether the connection to the
		 *  database is up, as set by setLiveConnection(). */
		bool	getLiveConnection();



		// connection state...

		/** Sets the current state of the database connection to
		 *  "state". */
		void	setState(enum sqlrconnectionstate_t state);

		/** Returns the state of the database connection as set by the
		 *  most recent call to setState(). */
		enum sqlrconnectionstate_t	getState();



		// instance state...

		/** If "disabled" is true then the instance is marked as
		 *  disabled.  If "disabled" is false, then the instance is
		 *  marked as enabled. */
		void	setInstanceDisabled(bool disabled);

		/** Returns whether the instance is disabled or enabled, as set
		 *  by setInstanceDisabled(). */
		bool	getInstanceDisabled();



		// statistics api...

		/** Returns the current statistics index. */
		uint32_t	getStatisticsIndex();

		/** Returns the number of client connections that are currently
		 *  open. */
		uint32_t	getOpenClientConnections();

		/** Returns the number of client connections that have been
		 *  opened since the instance was started. */
		uint32_t	getOpenedClientConnections();

		/** Increments the number of client connection that are
		 *  currently open as well as the number of client connections
		 *  that have been opened since the instance was started. */
		void	incrementOpenClientConnections();

		/** Decrements the number of client connections that are
		 *  currently open. */
		void	decrementOpenClientConnections();

		/** Returns the number of persistent database connections that
		 *  are currently open. */
		uint32_t	getOpenDatabaseConnections();

		/** Returns the number of persistent database connections that
		 *  have been opened since the instance was started. */
		uint32_t	getOpenedDatabaseConnections();

		/** Increments the number of persistent database connections
		 *  that are currently open as well as the number of persistent
		 *  database connections that have been opened since the
		 *  instance was started. */
		void	incrementOpenDatabaseConnections();

		/** Decrements the number of persistent database connections
		 *  that are currently open. */
		void	decrementOpenDatabaseConnections();

		/** Returns the number of database cursors that are currently
		 *  open. */
		uint32_t	getOpenDatabaseCursors();

		/** Returns the number of database cursors that have been
		 *  opened since the instance was started. */
		uint32_t	getOpenedDatabaseCursors();

		/** Increments the number of database cursors that are
		 *  currently open as well as the number of database cursors
		 *  that have been opened since the instance was started. */
		void	incrementOpenDatabaseCursors();

		/** Decrements the number of database cursors that are
		 *  currently open. */
		void	decrementOpenDatabaseCursors();

		/** Returns the number of times a new cursor was opened and
		 *  used, as opposed to reusing an existing cursor. */
		uint32_t	getTimesNewCursorUsed();

		/** Increments the number of times a new cursor was opened and
		 *  used, as opposed to reusing an existing cursor. */
		void	incrementTimesNewCursorUsed();

		/** Returns the number of times a cursor was reused, as
		 *  opposed to opening and using a new cursor. */
		uint32_t	getTimesCursorReused();

		/** Increments the number of times a cursor was reused, as
		 *  opposed to opening and using a new cursor. */
		void	incrementTimesCursorReused();

		/** Returns the number of queries of type "querytype" that have
		 *  been executed since the instance was started. */
		uint32_t	getQueryCount(sqlrquerytype_t querytype);

		/** Returns the total number of queries that have been executed
		 *  since the instance was started. */
		uint32_t	getTotalQueryCount();

		/** Increments the number of queries of type "querytype" that
		 *  have been executed since the instance was started, as well
		 *  as the total number of queries that have been executed
		 *  since the instance was started, */
		void	incrementQueryCount(sqlrquerytype_t querytype);

		/** Returns the number of errors that have occurred since the
		 *  instance was started. */
		uint32_t	getTotalErrors();

		/** Increments the number of errors that have occurred since
		 *  the instance was started. */
		void	incrementTotalErrors();

		/** Returns the number of authentications that have occurred
		 *  since the instance was started. */
		uint32_t	getAuthCount();

		/** Increments the number of authentications that have occurred
		 *  since the instance was started. */
		void	incrementAuthCount();

		/** Returns the number of sessions that have been suspeneded
		 *  since the instance was started. */
		uint32_t	getSuspendSessionCount();

		/** Increments the number of sessions that have been suspeneded
		 *  since the instance was started. */
		void	incrementSuspendSessionCount();

		/** Returns the number of sessions that have ended normally, as
		 *  opposed to having been suspended, since the instance was
		 *  started. */
		uint32_t	getEndSessionCount();

		/** Increments the number of sessions that have ended normally,
		 *  as opposed to having been suspended, since the instance was
		 *  started. */
		void	incrementEndSessionCount();

		/** Returns the number of pings that have occurred since the
		 *  instance was started. */
		uint32_t	getPingCount();

		/** Increments the number of pings that have occurred since the
		 *  instance was started. */
		void	incrementPingCount();

		/** Returns the number of get database type commands that
		 *  have occurred since the instance was started. */
		uint32_t	getIdentifyCount();

		/** Increments the number of get database type commands that
		 *  have occurred since the instance was started. */
		void	incrementIdentifyCount();

		/** Returns the number of autocommits that have occurred
		 *  since the instance was started. */
		uint32_t	getAutocommitCount();

		/** Increments the number of autocommits that have occurred
		 *  since the instance was started. */
		void	incrementAutocommitCount();

		/** Returns the number of begins that have occurred
		 *  since the instance was started. */
		uint32_t	getBeginCount();

		/** Increments the number of begins that have occurred
		 *  since the instance was started. */
		void	incrementBeginCount();

		/** Returns the number of commits that have occurred
		 *  since the instance was started. */
		uint32_t	getCommitCount();

		/** Increments the number of commits that have occurred
		 *  since the instance was started. */
		void	incrementCommitCount();

		/** Returns the number of rollbacks that have occurred
		 *  since the instance was started. */
		uint32_t	getRollbackCount();

		/** Increments the number of rollbacks that have occurred
		 *  since the instance was started. */
		void	incrementRollbackCount();

		/** Returns the number of get database version commands that
		 *  have occurred since the instance was started. */
		uint32_t	getDbVersionCount();

		/** Increments the number of get database version commands that
		 *  have occurred since the instance was started. */
		void	incrementDbVersionCount();

		/** Returns the number of get bind format commands that
		 *  have occurred since the instance was started. */
		uint32_t	getGetBindFormatCount();

		/** Increments the number of get bind format commands that
		 *  have occurred since the instance was started. */
		void	incrementGetBindFormatCount();

		/** Returns the number of get server version commands that
		 *  have occurred since the instance was started. */
		uint32_t	getGetServerVersionCount();

		/** Increments the number of get server version commands that
		 *  have occurred since the instance was started. */
		void	incrementGetServerVersionCount();

		/** Returns the number of select database commands that
		 *  have occurred since the instance was started. */
		uint32_t	getSelectDatabaseCount();

		/** Increments the number of select database commands that
		 *  have occurred since the instance was started. */
		void	incrementSelectDatabaseCount();

		/** Returns the number of get current database commands that
		 *  have occurred since the instance was started. */
		uint32_t	getGetCurrentDatabaseCount();

		/** Increments the number of get current database commands that
		 *  have occurred since the instance was started. */
		void	incrementGetCurrentDatabaseCount();

		/** Returns the number of get last insert id commands that
		 *  have occurred since the instance was started. */
		uint32_t	getGetLastInsertIdCount();

		/** Increments the number of get last insert id commands that
		 *  have occurred since the instance was started. */
		void	incrementGetLastInsertIdCount();

		/** Returns the number of get database host name commands
		 *  that have occurred since the instance was started. */
		uint32_t	getDbHostNameCount();

		/** Increments the number of get database host name commands
		 *  that have occurred since the instance was started. */
		void	incrementDbHostNameCount();

		/** Returns the number of get database ip address commands
		 *  that have occurred since the instance was started. */
		uint32_t	getDbIpAddressCount();

		/** Increments the number of get database ip address commands
		 *  that have occurred since the instance was started. */
		void	incrementDbIpAddressCount();

		/** Returns the number of new queries, as opposed to
		 *  reexecuted queries, that have been run since the instance
		 *  was started. */
		uint32_t	getNewQueryCount();

		/** Increments the number of new queries, as opposed to
		 *  reexecuted queries, that have been run since the instance
		 *  was started. */
		void	incrementNewQueryCount();

		/** Returns the number of reexecuted queries, as opposed to
		 *  new queries, that have been run since the instance was
		 *  started. */
		uint32_t	getReexecuteQueryCount();

		/** Increments the number of reexecuted queries, as opposed to
		 *  new queries, that have been run since the instance was
		 *  started. */
		void	incrementReexecuteQueryCount();

		/** Returns the number of fetch-from-bind-cursor commands
		 *  that have been run since the instance was started. */
		uint32_t	getFetchFromBindCursorCount();

		/** Increments the number of fetch-from-bind-cursor commands
		 *  that have been run since the instance was started. */
		void	incrementFetchFromBindCursorCount();

		/** Returns the number of result sets that have been fetched
		 *  run since the instance was started. */
		uint32_t	getFetchResultSetCount();

		/** Increments the number of result sets that have been fetched
		 *  run since the instance was started. */
		void	incrementFetchResultSetCount();

		/** Returns the number of result sets that have been aborted
		 *  since the instance was started. */
		uint32_t	getAbortResultSetCount();

		/** Increments the number of result sets that have been aborted
		 *  since the instance was started. */
		void	incrementAbortResultSetCount();

		/** Returns the number of result sets that have been
		 *  suspended since the instance was started. */
		uint32_t	getSuspendResultSetCount();

		/** Increments the number of result sets that have been
		 *  suspended since the instance was started. */
		void	incrementSuspendResultSetCount();

		/** Returns the number of result sets that have been
		 *  resumed since the instance was started. */
		uint32_t	getResumeResultSetCount();

		/** Increments the number of result sets that have been
		 *  resumed since the instance was started. */
		void	incrementResumeResultSetCount();

		/** Returns the number of get-database-list commands that
		 *  have been run since the instance was started. */
		uint32_t	getGetDbListCount();

		/** Increments the number of get-database-list commands that
		 *  have been run since the instance was started. */
		void	incrementGetDbListCount();

		/** Returns the number of get-table-list commands that
		 *  have been run since the instance was started. */
		uint32_t	getGetTableListCount();

		/** Increments the number of get-table-list commands that
		 *  have been run since the instance was started. */
		void	incrementGetTableListCount();

		/** Returns the number of get-column-list commands that
		 *  have been run since the instance was started. */
		uint32_t	getGetColumnListCount();

		/** Increments the number of get-column-list commands that
		 *  have been run since the instance was started. */
		void	incrementGetColumnListCount();

		/** Returns the number of get-query-tree commands that
		 *  have been run since the instance was started. */
		uint32_t	getGetQueryTreeCount();

		/** Increments the number of get-query-tree commands that
		 *  have been run since the instance was started. */
		void	incrementGetQueryTreeCount();

		/** Returns the number of re-logins that have occcurred
		 *  since the instance was started. */
		uint32_t	getReLogInCount();

		/** Increments the number of re-logins that have occcurred
		 *  since the instance was started. */
		void	incrementReLogInCount();

		/** Returns the number of get-next-result-set commands that
		 *  have been run since the instance was started. */
		uint32_t	getNextResultSetCount();

		/** Increments the number of get-next-result-set commands that
		 *  have been run since the instance was started. */
		void	incrementNextResultSetCount();

		/** Returns the number of get-next-result-set-available
		 *  commands that have been run since the instance was
		 *  started. */
		uint32_t	getNextResultSetAvailableCount();

		/** Increments the number of get-next-result-set-available
		 *  commands that have been run since the instance was
		 *  started. */
		void	incrementNextResultSetAvailableCount();

		/** Returns the current user. */
		const char	*getCurrentUser();

		/** Sets the current query by copying "querysize" bytes of
		 *  "query" into the statistics buffer. */
		void	setCurrentQuery(const char *query, uint32_t querysize);

		/** Returns the current query. */
		const char	*getCurrentQuery();

		/** Sets the current client info by copying "infosize" bytes of
		 *  "info" into the statistics buffer. */
                void    setClientInfo(const char *info, uint32_t infosize);

		/** Returns the current client info. */
		const char	*getClientInfo();

		/** Returns the address of the currently connected client. */
		const char	*getClientAddr();

		/** Tells the statistics framework that a command has been
		 *  started.  "sec" and "usec" should be the number of seconds
		 *  and microseconds since the epoch (Jan 1, 1970). */
		void	setCommandStart(sqlrservercursor *cursor,
						uint64_t sec, uint64_t usec);

		/** Returns the seconds-component of the command start time as
		 *  set by setCommandStart(). */
		uint64_t	getCommandStartSec(sqlrservercursor *cursor);

		/** Returns the microseconds-component of the command start
		 *  time as set by setCommandStart(). */
		uint64_t	getCommandStartUSec(sqlrservercursor *cursor);

		/** Tells the statistics framework that a command has ended.
		 *  "sec" and "usec" should be the number of seconds and
		 *  microseconds since the epoch (Jan 1, 1970). */
		void	setCommandEnd(sqlrservercursor *cursor,
						uint64_t sec, uint64_t usec);

		/** Returns the seconds-component of the command end time as
		 *  set by setCommandStart(). */
		uint64_t	getCommandEndSec(sqlrservercursor *cursor);

		/** Returns the microseconds-component of the command end
		 *  time as set by setCommandStart(). */
		uint64_t	getCommandEndUSec(sqlrservercursor *cursor);

		/** Tells the statistics framework that a query has been
		 *  started.  "sec" and "usec" should be the number of seconds
		 *  and microseconds since the epoch (Jan 1, 1970). */
		void	setQueryStart(sqlrservercursor *cursor,
						uint64_t sec, uint64_t usec);

		/** Returns the seconds-component of the query start time as
		 *  set by setCommandStart(). */
		uint64_t	getQueryStartSec(sqlrservercursor *cursor);

		/** Returns the microseconds-component of the query start
		 *  time as set by setCommandStart(). */
		uint64_t	getQueryStartUSec(sqlrservercursor *cursor);

		/** Tells the statistics framework that a query has ended.
		 *  "sec" and "usec" should be the number of seconds and
		 *  microseconds since the epoch (Jan 1, 1970). */
		void	setQueryEnd(sqlrservercursor *cursor,
						uint64_t sec, uint64_t usec);

		/** Returns the seconds-component of the query end time as
		 *  set by setCommandStart(). */
		uint64_t	getQueryEndSec(sqlrservercursor *cursor);

		/** Returns the microseconds-component of the query end
		 *  time as set by setCommandStart(). */
		uint64_t	getQueryEndUSec(sqlrservercursor *cursor);



		// event api...

		/** Returns true if logging is enabled and false otherwise. */
		bool	getLoggingEnabled();

		/** Returns true if notifications are enabled and false
		 *  otherwise. */
		bool	getNotificationsEnabled();

		/** Raises a debug-message event with information "info", which
		 *  may be logged, or which may trigger a notification. */
		void	raiseDebugMessageEvent(const char *info);

		/** Raises a client-connected event, which may be logged, or
		 *  which may trigger a notification. */
		void	raiseClientConnectedEvent();

		/** Raises a client-connection-refused event with information
		 *  "info", which may be logged, or which may trigger a
		 *  notification. */
		void	raiseClientConnectionRefusedEvent(const char *info);

		/** Raises a client-disconnected event with information "info",
		 *  which may be logged, or which may trigger a notification. */
		void	raiseClientDisconnectedEvent(const char *info);

		/** Raises a client-protocol-error event on cursor "cursor",
		 *  with information "info", and with result code "result"
		 *  (one of 0, indicating a closed connection, RESULT_ERROR,
		 *  RESULT_TIMEOUT, or RESULT_ABORT) which may be logged, or
		 *  which may trigger a notification. */
		void	raiseClientProtocolErrorEvent(sqlrservercursor *cursor,
							const char *info,
							ssize_t result);

		/** Raises a database-login event, which may be logged, or
		 *  which may trigger a notification. */
		void	raiseDbLogInEvent();

		/** Raises a database-logout event, which may be logged, or
		 *  which may trigger a notification. */
		void	raiseDbLogOutEvent();

		/** Raises a database-error event on cursor "cursor", with
		 *  information "info", which may be logged, or which may
		 *  trigger a notification. */
		void	raiseDbErrorEvent(sqlrservercursor *cursor,
							const char *info);

		/** Raises a database-warning event on cursor "cursor", with
		 *  information "info", which may be logged, or which may
		 *  trigger a notification. */
		void	raiseDbWarningEvent(sqlrservercursor *cursor,
							const char *info);

		/** Raises a query-received event on cursor "cursor", which may
		 *  be logged, or which may trigger a notification. */
		void	raiseQueryReceivedEvent(sqlrservercursor *cursor);

		/** Raises a query-prepared event on cursor "cursor", which may
		 *  be logged, or which may trigger a notification. */
		void	raiseQueryPreparedEvent(sqlrservercursor *cursor);

		/** Raises a query-executed event on cursor "cursor", which may
		 *  be logged, or which may trigger a notification. */
		void	raiseQueryExecutedEvent(sqlrservercursor *cursor);

		/** Raises a filter-violation event on cursor "cursor", which
		 *  may be logged, or which may trigger a notification. */
		void	raiseFilterViolationEvent(sqlrservercursor *cursor);

		/** Raises an internal-error event on cursor "cursor", with
		 *  information "info", which may be logged, or which may
		 *  trigger a notification. */
		void	raiseInternalErrorEvent(sqlrservercursor *cursor,
							const char *info);

		/** Raises an internal-warning event on cursor "cursor", with
		 *  information "info", which may be logged, or which may
		 *  trigger a notification. */
		void	raiseInternalWarningEvent(sqlrservercursor *cursor,
							const char *info);

		/** Raises a schedule-violation event, with information "info",
		 *  which may be logged, or which may trigger a notification. */
		void	raiseScheduleViolationEvent(const char *info);

		/** Raises a integrity-violation event, with information "info",
		 *  which may be logged, or which may trigger a notification. */
		void	raiseIntegrityViolationEvent(const char *info);

		/** Raises a translation-failure event, with information "info",
		 *  which may be logged, or which may trigger a notification. */
		void	raiseTranslationFailureEvent(sqlrservercursor *cursor,
							const char *info);

		/** Raises a parse-failure event, with information "info",
		 *  which may be logged, or which may trigger a notification. */
		void	raiseParseFailureEvent(sqlrservercursor *cursor,
							const char *info);

		/** Raises a cursor-open event, on cursor "cursor", which may
		 *  be logged, or which may trigger a notification. */
		void	raiseCursorOpenEvent(sqlrservercursor *cursor);

		/** Raises a cursor-close event, on cursor "cursor", which may
		 *  be logged, or which may trigger a notification. */
		void	raiseCursorCloseEvent(sqlrservercursor *cursor);

		/** Raises a begin-transaction event, on cursor "cursor", which
		 *  may be logged, or which may trigger a notification. */
		void	raiseBeginTransactionEvent();

		/** Raises a commit event, on cursor "cursor", which
		 *  may be logged, or which may trigger a notification. */
		void	raiseCommitEvent();

		/** Raises a rollback event, on cursor "cursor", which
		 *  may be logged, or which may trigger a notification. */
		void	raiseRollbackEvent();


		// cursor api...

		// cursor management...

		/** Returns cursor "id" from the pool of already-open cursors
		 *  or NULL if "id" was not a valid cursor id.
		 *
		 *  The size of the cursor pool depends on the value of the
		 *  maxcursors attribute, of the instance tag, in the config
		 *  file.  Valid cursor ids range from 0 to maxcursors-1.
		 *
		 *  However, you will typically only request a cursor by id
		 *  if you already know an id from a previous call to
		 *  getId(cursor) and have a specific reason to reuse that
		 *  cursor. */
		sqlrservercursor	*getCursor(uint16_t id);

		/** Returns an available cursor from the pool of already-open
		 *  cursors from the pool of available cursors, or NULL if no
		 *  cursor was currently available.
		 *
		 *  The size of the cursor pool depends on the value of the
		 *  maxcursors attribute, of the instance tag, in the config
		 *  file. */
		sqlrservercursor	*getCursor();

		/** Allocates a cursor, outside of the cursor pool.  This
		 *  cursor will have an id >= the value of the maxcursors
		 *  attribute, of the instance tag, in the config file.
		 *
		 *  This cursor must be opened by calling open(cursor) before
		 *  it can be used and should be closed by calling close(cursor)
		 *  and deleted by calling deleteCursor(cursor) when you are
		 *  done using it.
		 *
		 *  Returns NULL if a new cursor couldn't be allocated. */
		sqlrservercursor	*newCursor();

		/** Returns the id of "cursor". */
		uint16_t	getId(sqlrservercursor *cursor);

		/** Opens "cursor".
		 *
		 *  Returns true on success and false on failure. */
		bool	open(sqlrservercursor *cursor);

		/** Closes "cursor".
		 *  
		 *  Returns true on success and false on failure. */
		bool	close(sqlrservercursor *cursor);

		/** Closes the result set of "cursor" and clears any custom
		 *  query cursor associated with "cursor". */
		void	abort(sqlrservercursor *cursor);

		/** Releases "cursor" back to the pool of already-open cursors
		 *  and marks it available so that it may be returned by a
		 *  future call to getCursor().
		 *
		 *  Note that "cursor" should have been returned from a call to
		 *  getCursor().  This method should not be called on a cursor
		 *  returned from newCursor(). */
		void	release(sqlrservercursor *cursor);

		/** Deletes "cursor".
		 *
		 *  Note that "cursor" should have been allocated by a call to
		 *  newCursor().  This method should not be called on a cursor
		 *  returned from getCursor() */
		void	deleteCursor(sqlrservercursor *cursor);



		// query buffer...

		/** Returns a pointer to the query buffer of "cursor" */
		char		*getQueryBuffer(sqlrservercursor *cursor);

		/** Sets the size, in bytes, of the query that is currently
		 *  present in the query buffer of "cursor" to "querysize". */
		void		setQuerySize(sqlrservercursor *cursor,
							uint32_t querysize);

		/** Returns the size, in bytes, of the query that is currently
		 *  present in the query buffer of "cursor", as set by
		 *  setQuerySize(). */
		uint32_t	getQuerySize(sqlrservercursor *cursor);



		// query status...

		/** Sets the status of the current query of "cursor"
		 *  to "status". */
		void	setQueryStatus(sqlrservercursor *cursor,
						sqlrquerystatus_t status);

		/** Returns the status of the current query of "cursor",
		 *  as set by setQueryStatus(). */
		sqlrquerystatus_t	getQueryStatus(
						sqlrservercursor *cursor);



		// query translations...

		/** Sets the tree representing the current query of "cursor" to
		 *  "tree". */
		void	setQueryTree(sqlrservercursor *cursor, xmldom *tree);

		/** Returns the tree representing the current query of "cursor"
		 *  as set by setQueryTree(), or NULL if no tree has been
		 *  set since initialization of "cursor" or since the most
		 *  recent call to clearQueryTree(). */
		xmldom	*getQueryTree(sqlrservercursor *cursor);

		/** Sets the tree representing the current query of "cursor" to
		 *  NULL. */
		void	clearQueryTree(sqlrservercursor *cursor);

		/** Returns the translated query buffer of "cursor". */
		stringbuffer	*getTranslatedQueryBuffer(
						sqlrservercursor *cursor);

		/** Returns the query currently stored in the translated quer
		 *  buffer of "cursor". */
		const char	*getTranslatedQuery(sqlrservercursor *cursor);



		// running queries...

		/** Copies "size" bytes of "query" to the query buffer of
		 *  "cursor" and prepares the query, with all directives,
		 *  translations, and filters enabled.
		 *
		 *  Returns true on success and false otherwise. */
		bool	prepareQuery(sqlrservercursor *cursor,
						const char *query,
						uint32_t size);

		/** Copies "size" bytes of "query" to the query buffer of
		 *  "cursor" and prepares the query.
		 *
		 *  Directives are enabled if "enabledirectives" is true or
		 *  disabled if it is false.
		 *  Translations are enabled if "enabletranslations" is true or
		 *  disabled if it is false.
		 *  Filters are enabled if "enablefilters" is true or disabled
		 *  if it is false.
		 *
		 *  Returns true on success and false otherwise. */
		bool	prepareQuery(sqlrservercursor *cursor,
						const char *query,
						uint32_t size,
						bool enabledirectives,
						bool enabletranslations,
						bool enablefilters);

		/** Sets whether the current query is "suppressed" or not.
		 *  Currently may be called by a before-trigger to suppresses
		 *  execution of the query by executeQuery(). */
		void	setQuerySuppressed(sqlrservercursor *cursor,
						bool querysuppressed);

		/** Returns whether the current query is "suppressed" or not
		 *  as set by a call to setQuerySuppressed(). */
		bool	getQuerySuppressed(sqlrservercursor *cursor);

		/** Executes the currently prepared query of "cursor", with all
		 *  directives, translations, and filters enabled.
		 *
		 *  Returns true on success and false otherwise. */
		bool	executeQuery(sqlrservercursor *cursor);

		/** Executes the currently prepraed query of "cursor".
		 *
		 *  Directives are enabled if "enabledirectives" is true or
		 *  disabled if it is false.
		 *  Translations are enabled if "enabletranslations" is true or
		 *  disabled if it is false.
		 *  Filters are enabled if "enablefilters" is true or disabled
		 *  if it is false.
		 *  Triggers are enabled if "enabletriggers" is true or disabled
		 *  if it is false.
		 *
		 *  Returns true on success and false otherwise. */
		bool	executeQuery(sqlrservercursor *cursor,
						bool enabledirectives,
						bool enabletranslations,
						bool enablefilters,
						bool enabletriggers);

		/** Fetches from bind cursor "cursor".
		 *  
		 *  Returns true on success and false otherwise. */
		bool	fetchFromBindCursor(sqlrservercursor *cursor);

		/** Advances to the next result set of "cursor".
		 *
		 *  Returns true and sets "nextresultsetavailable" true if
		 *  another result set was available.
		 *
		 *  Returns true and sets "nextresultsetavailable" false if
		 *  another result set was not available.
		 *
		 *  Returns false if an error occured while checking for
		 *  another resulet set. */
		bool	nextResultSet(sqlrservercursor *cursor,
						bool *nextresultsetavailable);



		// bind variables...

		/** Returns the memory pool of "cursor" used to store bind
		 *  variable names and values. */
		memorypool	*getBindPool(sqlrservercursor *cursor);

		/** Returns the memory pool of "cursor" used to map bind
		 *  variable names when the value of the attribute
		 *  translatebindvariables of the instance tag in the config
		 *  file is set to "yes". */
		memorypool	*getBindMappingsPool(sqlrservercursor *cursor);

		/** Returns the dictionary of bind variable name mappings of
		 *  "cursor".  The keys are the old (original) bind variable
		 *  names and the values are the new (translated) bind variable
		 *  names.
		 *
		 *  The dictionary is populated by prepareQuery() and remains
		 *  populated with the same key-value pairs until the next call
		 *  to prepareQuery().  If it is empty then either bind variable
		 *  translation is disabled or the most recently prepared query
		 *  contained no bind variables. */
		dictionary<char *, char *>	*getBindMappings(
						sqlrservercursor *cursor);

		/** Copies all bind variables from "sourcecur" to "destcur".
		 *
		 *  Note that no attempt is made to resolve bind variable name 
		 *  collisions.
		 *
		 *  Returns true if all bind variables were successfully copied
		 *  and false if the maximum number of bind variables was
		 *  exceeded in destcur before all bind variables were copied.*/
		bool	copyBinds(sqlrservercursor *sourcecur,
						sqlrservercursor *destcur);

		/** Copies input bind variables from "sourcecur" to "destcur".
		 *
		 *  Note that no attempt is made to resolve bind variable name 
		 *  collisions.
		 *
		 *  Returns true if all bind variables were successfully copied
		 *  and false if the maximum number of bind variables was
		 *  exceeded in destcur before all bind variables were copied.*/
		bool	copyInputBinds(sqlrservercursor *sourcecur,
						sqlrservercursor *destcur);

		/** Copies output bind variables from "sourcecur" to "destcur".
		 *
		 *  Note that no attempt is made to resolve bind variable name 
		 *  collisions.
		 *
		 *  Returns true if all bind variables were successfully copied
		 *  and false if the maximum number of bind variables was
		 *  exceeded in destcur before all bind variables were copied.*/
		bool	copyOutputBinds(sqlrservercursor *sourcecur,
						sqlrservercursor *destcur);

		/** Copies input-output bind variables from "sourcecur" to
		 *  "destcur".
		 *
		 *  Note that no attempt is made to resolve bind variable name 
		 *  collisions.
		 *
		 *  Returns true if all bind variables were successfully copied
		 *  and false if the maximum number of bind variables was
		 *  exceeded in destcur before all bind variables were copied.*/
		bool	copyInputOutputBinds(sqlrservercursor *sourcecur,
						sqlrservercursor *destcur);

		/** Copies bind variable "source" to bind variable "dest",
		 *  allocating space for variable names, string variables, and
		 *  dates in "destpool". */
		void	copyBind(sqlrserverbindvar *source,
						sqlrserverbindvar *dest,
						memorypool *destpool);



		// fake input binds...

		/** Sets whether to fake input binds for the current query of
		 *  "cursor", by rewriting the query, to "fake".
		 *
		 *  Note that the behavior of prepareQuery() is as follows:
		 *
		 *    * It initially configures whether input binds should be
		 *      faked or not, based on the value of the
		 *      "fakeinputbindvariables" attribute of the instance tag,
		 *      in the config file.
		 *    * It then processes filters, directives, translations,
		 *      and before-triggers, which can call this method to
		 *      override that.
		 *    * It then checks to see if the query supports native
		 *      binds and sets input binds to be faked if native binds
		 *      are not supported with this query.
		 *
		 *  So, it is possible for this method to hae no effect if:
		 *
		 *    * It is called from a module other than a filter,
		 *      directive, translation, or before-trigger.
		 *    * It sets binds to not be faked, but the query itself
		 *      doesn't support native binds.
		 */
		void	setFakeInputBindsForThisQuery(
						sqlrservercursor *cursor,
						bool fake);

		/** Returns whether or not input binds will be faked for the
		 *  current query of "cursor".  See
		 *  setFakeInputBindsForThisQuery(). */
		bool	getFakeInputBindsForThisQuery(sqlrservercursor *cursor);



		// input bind variables...

		/** Sets the number of valid input binds in "cursor" to
		 *  "inbindcount". */
		void	setInputBindCount(sqlrservercursor *cursor,
							uint16_t inbindcount);

		/** Returns the number of valid input binds in "cursor", as
		 *  set by setInputBindCount(). */
		uint16_t	getInputBindCount(sqlrservercursor *cursor);

		/** Returns the array of input binds in "cursor".  The total
		 *  number of bind variables in the array is equal to the value
		 *  of the maxbindcount attribute of the instance tag in the
		 *  config file.  However, only the first getInputBindCount()
		 *  bind variables are currently valid.  The state of the rest
		 *  are undefined. */
		sqlrserverbindvar	*getInputBinds(
						sqlrservercursor *cursor);



		// output bind variables...

		/** Sets the number of valid output binds in "cursor" to
		 *  "outbindcount". */
		void		setOutputBindCount(sqlrservercursor *cursor,
							uint16_t outbindcount);

		/** Returns the number of valid output binds in "cursor", as
		 *  set by setOutputBindCount(). */
		uint16_t	getOutputBindCount(sqlrservercursor *cursor);

		/** Returns the array of output binds in "cursor".  The total
		 *  number of bind variables in the array is equal to the value
		 *  of the maxbindcount attribute of the instance tag in the
		 *  config file.  However, only the first getOutputBindCount()
		 *  bind variables are currently valid.  The state of the rest
		 *  are undefined. */
		sqlrserverbindvar	*getOutputBinds(
						sqlrservercursor *cursor);

		/** Opens LOB output bind at position "index" in "cursor"
		 *  (unless it is already open) and sets "length" equal to its
		 *  length, in characters.
		 *
		 *  Returns true on success and false otherwise.  Will return
		 *  false if the output bind at position "index" is not a LOB.
		 *
		 *  For example...
		 *
		 *  if
		 *  getOutputBinds(cursor)[index].type==
		 *			SQLRSERVERBINDVARTYPE_BLOB
		 *  or
		 *  getOutputBinds(cursor)[index].type==
		 *			SQLRSERVERBINDVARTYPE_CLOB
		 *  then this method will return true. */
		bool	getLobOutputBindLength(sqlrservercursor *cursor,
							uint16_t index,
							uint64_t *length);

		/** Opens LOB output bind at position "index" in "cursor"
		 *  (unless it is already open) and attempts to fetch
		 *  "charstoread" characters from position "offset" into
		 *  "buffer" of "buffersize" bytes.  Populates "charsread" with
		 *  the number of characters that were actually read.
		 *
		 *  Returns true on success and false otherwise.  Will return
		 *  false if the output bind at position "index" is not a LOB.
		 *
		 *  For example...
		 *
		 *  if
		 *  getOutputBinds(cursor)[index].type==
		 *			SQLRSERVERBINDVARTYPE_BLOB
		 *  or
		 *  getOutputBinds(cursor)[index].type==
		 *			SQLRSERVERBINDVARTYPE_CLOB
		 *  then this method will return true. */
		bool	getLobOutputBindSegment(sqlrservercursor *cursor,
							uint16_t index,
							char *buffer,
							uint64_t buffersize,
							uint64_t offset,
							uint64_t charstoread,
							uint64_t *charsread);

		/** Closes LOB output bind at position "index" of "cursor".
		 *
		 *  For example...
		 *
		 *  if
		 *  getOutputBinds(cursor)[index].type==
		 *			SQLRSERVERBINDVARTYPE_BLOB
		 *  or
		 *  getOutputBinds(cursor)[index].type==
		 *			SQLRSERVERBINDVARTYPE_CLOB
		 *  and the LOB was opened by a call to
		 *  getLobOutputBindLength() or getLobOutputBindSegment()
		 *  then this method will close it. */
		void	closeLobOutputBind(sqlrservercursor *cursor,
							uint16_t index);



		// input/output bind variables...

		/** Sets the number of valid input-output binds in "cursor" to
		 *  "inoutbindcount". */
		void		setInputOutputBindCount(
						sqlrservercursor *cursor,
						uint16_t inoutbindcount);

		/** Returns the number of valid input-output binds in "cursor",
		 *  as set by setInputOutputBindCount(). */
		uint16_t	getInputOutputBindCount(
						sqlrservercursor *cursor);

		/** Returns the array of input-output binds in "cursor".  The
		 *  total number of bind variables in the array is equal to the
		 *  value of the maxbindcount attribute of the instance tag in
		 *  the config file.  However, only the first
		 *  getInputOutputBindCount() bind variables are currently
		 *  valid.  The state of the rest are undefined. */
		sqlrserverbindvar	*getInputOutputBinds(
						sqlrservercursor *cursor);

		/** Opens LOB input-output bind at position "index" in "cursor"
		 *  (unless it is already open) and sets "length" equal to its
		 *  length, in characters.
		 *
		 *  Returns true on success and false otherwise.  Will return
		 *  false if the output bind at position "index" is not a LOB.
		 *
		 *  For example...
		 *
		 *  if
		 *  getOutputBinds(cursor)[index].type==
		 *			SQLRSERVERBINDVARTYPE_BLOB
		 *  or
		 *  getOutputBinds(cursor)[index].type==
		 *			SQLRSERVERBINDVARTYPE_CLOB
		 *  then this method will return true. */
		bool	getLobInputOutputBindLength(sqlrservercursor *cursor,
							uint16_t index,
							uint64_t *length);

		/** Opens LOB input-output bind at position "index" in "cursor"
		 *  (unless it is already open) and attempts to fetch
		 *  "charstoread" characters from position "offset" into
		 *  "buffer" of "buffersize" bytes.  Populates "charsread" with
		 *  the number of characters that were actually read.
		 *
		 *  Returns true on success and false otherwise.  Will return
		 *  false if the output bind at position "index" is not a LOB.
		 *
		 *  For example...
		 *
		 *  if
		 *  getOutputBinds(cursor)[index].type==
		 *			SQLRSERVERBINDVARTYPE_BLOB
		 *  or
		 *  getOutputBinds(cursor)[index].type==
		 *			SQLRSERVERBINDVARTYPE_CLOB
		 *  then this method will return true. */
		bool	getLobInputOutputBindSegment(sqlrservercursor *cursor,
							uint16_t index,
							char *buffer,
							uint64_t buffersize,
							uint64_t offset,
							uint64_t charstoread,
							uint64_t *charsread);

		/** Closes LOB input-output bind at position "index" of
		 *  "cursor".
		 *
		 *  For example...
		 *
		 *  if
		 *  getOutputBinds(cursor)[index].type==
		 *			SQLRSERVERBINDVARTYPE_BLOB
		 *  or
		 *  getOutputBinds(cursor)[index].type==
		 *			SQLRSERVERBINDVARTYPE_CLOB
		 *  and the LOB was opened by a call to
		 *  getLobOutputBindLength() or getLobOutputBindSegment()
		 *  then this method will close it. */
		void	closeLobInputOutputBind(sqlrservercursor *cursor,
							uint16_t index);



		// custom queries...

		/** Determines if the query currently in the query buffer of
		 *  "cursor" needs to be handled with a custom query module.
		 *  If so, it configures "cursor" to use that module and
		 *  returns the custom query cursor.  If not, it just returns
		 *  "cursor". */
		sqlrservercursor	*useCustomQueryCursor(
						sqlrservercursor *cursor);

		/** Returns true if "cursor" is handling the current query
		 *  using a custom query module and false otherwise. */
		bool	isCustomQuery(sqlrservercursor *cursor);



		// temp tables...

		/** Adds global temporary tables defined by "gtts" to the list
		 *  of global temporary tables that will be truncated when the
		 *  client session ends.
		 *
		 *  "gtts" may be either "%", indicating that all global
		 *  temporary tables should be truncated, or a comma separated
		 *  list of tables to truncate such as
		 *  "table1,table2,table3". */
		void	addGlobalTempTables(const char *gtts);

		/** Adds "tablename" to the list of temporary tables that will
		 *  be dropped when the client session ends. */
		void	addTempTableForDrop(const char *tablename);

		/** Adds "tablename" to the list of temporary tables that will
		 *  be truncated when the client session ends. */
		void	addTempTableForTrunc(const char *tablename);



		// table name remapping...

		/** Creates a mapping between old table name
		 *  "database"."schema"."oldtable" and new table name "newtable"
		 *  in the table name replacement map, such "newtable" can be
		 *  retrieved by a call to getReplacementTableName.  This is
		 *  primarly useful for remapping temporary table names, but
		 *  could be used for other things as well. */
		void	setReplacementTableName(
					const char *database,
					const char *schema,
					const char *oldtable,
					const char *newtable);

		/** Creates a mapping between old index name
		 *  "database"."schema"."oldindex" and new index name
		 *  "newindex", which are dependent on table "table", in the
		 *  index name replacement map, such that "newindex" can be
		 *  retrieved by getReplacementIndexName.  This is primarly
		 *  useful for remapping temporary index names, but could be
		 *  used for other things as well. */
		void	setReplacementIndexName(
					const char *database,
					const char *schema,
					const char *oldindex,
					const char *newindex,
					const char *table);

		/** Looks up "database"."schema"."oldtable" in the table name
		 *  replacement map and returns the mapping in "newtable".
		 *
		 *  Returns true if a mapping was found.  Returns false and
		 *  sets "newtable" to NULL if no mapping was found. */
		bool	getReplacementTableName(
					const char *database,
					const char *schema,
					const char *oldtable,
					const char **newtable);

		/** Looks up "database"."schema"."oldindex" in the index name
		 *  replacement map and returns the mapping in "newindex".
		 *
		 *  Returns true of a mapping was found.  Returns false and
		 *  sets "newindex" to NULL if no mapping was found. */
		bool	getReplacementIndexName(const char *database,
						const char *schema,
						const char *oldindex,
						const char **newindex);

		/** Looks through the table name replacement map, removes
		 *  the entry for "database"."schema"."oldtable", and removes
		 *  any dependent indexes from the index name replacement map.
		 *
		 *  Returns true if a mapping was found and removed, and false
		 *  if no mapping was found. */
		bool	removeReplacementTable(const char *database,
							const char *schema,
							const char *oldtable);

		/** Looks through the index name replacement map, removes
		 *  the entry for "database"."schema"."oldindex".
		 *
		 *  Returns true if a mapping was found and removed, and false
		 *  if no mapping was found. */
		bool	removeReplacementIndex(const char *database,
							const char *schema,
							const char *oldindex);



		// db, table, column, procedure bind/column lists...

		/** Returns true if the currently loaded database connection
		 *  module fetches lists (database lists, table lists, column
		 *  lists, etc.) via database API call and false if it fetches
		 *  lists via query. */
		bool	getListsByApiCalls();

		/** Makes the database API call to fetch the list of databases
		 *  that are visible to the user that SQL Relay is logged in
		 *  as.  Only returns database names that match wildcard
		 *  "wild" if "wild" is non-NULL.
		 *
		 *  Returns true on success and false on failure. */
		bool	getDatabaseList(sqlrservercursor *cursor,
							const char *wild);

		/** Makes the database API call to fetch the list of schemas,
		 *  in the current database, that are visible to the user that
		 *  SQL Relay is logged in as.  Only returns schema names that
		 *  match wildcard "wild" if "wild" is non-NULL.
		 *
		 *  Returns true on success and false on failure. */
		bool	getSchemaList(sqlrservercursor *cursor,
							const char *wild);

		/** Makes the database API call to fetch the list of tables (and
		 *  table-like objects), in the current database and schema,
		 *  that are visible to the user that SQL Relay is logged in
		 *  as.  "objecttypes" should be an or-ed set of one or more of
		 *  the following object types:
		 *
		 *  DB_OBJECT_TABLE
		 *  DB_OBJECT_VIEW
		 *  DB_OBJECT_ALIAS
		 *  DB_OBJECT_SYNONYM
		 *
		 *  Only returns table names that match wildcard "wild" if
		 *  "wild" is non-NULL.
		 *
		 *  Returns true on success and false on failure. */
		bool	getTableList(sqlrservercursor *cursor,
							const char *wild,
							uint16_t objecttypes);

		/** Makes the database API call to fetch the list of table type
		 *  names in the current database and schema, that are visible
		 *  to the user that SQL Relay is logged in as.  Only returns
		 *  table type names that match wildcard "wild" if "wild" is
		 *  non-NULL.
		 *
		 *  Returns true on success and false on failure. */
		bool	getTableTypeList(sqlrservercursor *cursor,
							const char *wild);

		/** Makes the database API call to fetch the list of column
		 *  names in "table", where "table" is in the current database
		 *  and schema.  Only returns column names that match wildcard
		 *  "wild" if "wild" is non-NULL.
		 *
		 *  Returns true on success and false on failure. */
		bool	getColumnList(sqlrservercursor *cursor,
							const char *table,
							const char *wild);

		/** Makes the database API call to fetch the list of columns
		 *  that compose the primary key of "table".  Only returns
		 *  primary key column names that match wildcard "wild" if
		 *  "wild" is non-NULL.
		 *
		 *  Returns true on success and false on failure. */
		bool	getPrimaryKeyList(sqlrservercursor *cursor,
							const char *table,
							const char *wild);

		/** Makes the database API call to fetch the indices and
		 *  indexed columns of "table", where "table" is in the current
		 *  database and schema.  Only returns primary key column names
		 *  that match wildcard "wild" if "wild" is non-NULL.
		 *
		 *  Returns true on success and false on failure. */
		bool	getKeyAndIndexList(sqlrservercursor *cursor,
							const char *table,
							const char *wild);

		/** Makes the database API call to fetch the parameter names of
		 *  "proc", where "proc" is in the current database and schema,
		 *  and information about them, such as whether they are input,
		 *  output, or input-output variables.  Only returns parameter
		 *  names that match wildcard "wild" if "wild" is non-NULL.
		 *
		 *  Returns true on success and false on failure. */
		bool	getProcedureParameterList(sqlrservercursor *cursor,
							const char *proc,
							const char *wild);

		/** Makes the database API call to fetch the info about
		 *  datatype "type", where "type" is in the current database
		 *  and schema.  Only returns info for types that match
		 *  wildcard "wild" if "wild" is non-NULL.
		 *
		 *  Returns true on success and false on failure. */
		bool	getTypeInfoList(sqlrservercursor *cursor,
							const char *type,
							const char *wild);

		/** Makes the database API call to fetch the list of stored
		 *  procedures in the current database and schema, and
		 *  information about them, such as the number of input and
		 *  output parameters, the numer of result sets that the
		 *  procdure may retrun, a description of the procedure, and
		 *  the procedure type (procedure or function).  Only returns
		 *  info for procedures that match wildcard "wild" if "wild"
		 *  is non-NULL.
		 *
		 *  Returns true on success and false on failure. */
		bool	getProcedureList(sqlrservercursor *cursor,
							const char *wild);

		/** Returns a query that can be used to fetch the list of
		 *  database names.
		 *
		 *  If "wild" is true the the query also includes a where
		 *  clause that inlcudes a %s which can be used to substitute
		 *  in a wildcard value which can be used to filter the
		 *  results. */
		const char	*getDatabaseListQuery(bool wild);

		/** Returns a query that can be used to fetch the list of
		 *  schema names.
		 *
		 *  If "wild" is true the the query also includes a where
		 *  clause that inlcudes a %s which can be used to substitute
		 *  in a wildcard value which can be use  to filter the
		 *  results.
		 *
		 *  If "currentdbonly" is true then the query only returns
		 *  schemas on the current database. */
		const char	*getSchemaListQuery(bool wild,
							bool currentdbonly);

		/** Returns a query that can be used to fetch the list of
		 *  table names.
		 *
		 *  If "wild" is true the the query also includes a where
		 *  clause that inlcudes a %s which can be used to substitute
		 *  in a wildcard value which can be used  to filter the
		 *  results.
		 *
		 *  If "currentschemaonly" is true then the query only returns
		 *  schemas on the current database. */
		const char	*getTableListQuery(bool wild,
							uint16_t objecttypes,
							bool currentschemaonly);

		/** Returns a query that can be used to fetch the list of
		 *  table types.
		 *
		 *  If "wild" is true the the query also includes a where
		 *  clause that inlcudes a %s which can be used to substitute
		 *  in a wildcard value which can be used to filter the
		 *  results.
		 *
		 *  If "currentschemaonly" is true then the query only returns
		 *  schemas on the current database. */
		const char	*getTableTypeListQuery(bool wild,
							bool currentschemaonly);

		/** Returns a query that can be used to fetch the list of
		 *  global temporary table names.
		 *
		 *  If "currentschemaonly" is true then the query only returns
		 *  schemas on the current database. */
		const char	*getGlobalTempTableListQuery(
							bool currentschemaonly);

		/** Returns a query that can be used to fetch the list of
		 *  column names from "table".
		 *
		 *  If "wild" is true the the query also includes a where
		 *  clause that inlcudes a %s which can be used to substitute
		 *  in a wildcard value which can be used to filter the
		 *  results. */
		const char	*getColumnListQuery(const char *table,
								bool wild);

		/** Returns a query that can be used to fetch the list of
		 *  columns that compose the primary key of "table".
		 *
		 *  If "wild" is true the the query also includes a where
		 *  clause that inlcudes a %s which can be used to substitute
		 *  in a wildcard value which can be used to filter the
		 *  results. */
		const char	*getPrimaryKeyListQuery(const char *table,
								bool wild);

		/** Returns a query that can be used to fetch the indices and
		 *  indexed columns  of "table", where "table" is in the
		 *  current database and schema.
		 *
		 *  If "wild" is true the the query also includes a where
		 *  clause that inlcudes a %s which can be used to substitute
		 *  in a wildcard value which can be used to filter the
		 *  results. */
		const char	*getKeyAndIndexListQuery(const char *table,
								bool wild);

		/** Returns a query that can be used to fetch the parameter
		 *  names of "proc", where "proc" is in the current database
		 *  and schema, and information about them, such as whether
		 *  they are input, output, or input-output variables.
		 *
		 *  If "wild" is true then the query also includes a where
		 *  clause that inlcudes a %s which can be used to substitute
		 *  in a wildcard value which can be used to filter the
		 *  results. */
		const char	*getProcedureParameterListQuery(
							const char *proc,
							bool wild);

		/** Returns a query that can be used to fetch info about
		 *  datatype "type", where "type" is in the current database
		 *  and schema.
		 *
		 *  If "wild" is true the the query also includes a where
		 *  clause that inlcudes a %s which can be used to substitute
		 *  in a wildcard value which can be used to filter the
		 *  results.
		 *
		 *  If "currentschemaonly" is true then the query only returns
		 *  schemas on the current database. */
		const char	*getTypeInfoListQuery(const char *type,
							bool wild,
							bool currentschemaonly);

		/** Returns a query that can be used to fetch the list of
		 *  stored procedures in the current database and schema, and
		 *  information about them, such as the number of input and
		 *  output parameters, the numer of result sets that the
		 *  procdure may retrun, a description of the procedure, and
		 *  the procedure type (procedure or function).
		 *
		 *  If "wild" is true the the query also includes a where/
		 *  clause that inlcudes a %s which can be used to substitute
		 *  in a wildcard value which can be used to filter the
		 *  results.
		 *
		 *  If "currentschemaonly" is true then the query only returns
		 *  schemas on the current database. */
		const char	*getProcedureListQuery(bool wild,
							bool currentschemaonly);



		// column info...

		/** Returns true if, for "cursor", column info such as name,
		 *  size, type, precision, scale, etc. is valid after a query
		 *  has been prepared and false if column info is only valid
		 *  after a query has been executed. */
		bool	columnInfoIsValidAfterPrepare(sqlrservercursor *cursor);

		/** Sets whether to send column info to the client, or not.
		 *
		 *  If "sendcolumninfo" is true then column info will be sent to
		 *  the client as part of the result set.  If "sendcolumninfo"
		 *  is false then no column info will be sent to the client. */
		void	setSendColumnInfo(bool sendcolumninfo);

		/** Returns true if column info will be sent to the client and
		 *  false if column info will not be sent to the client. */
		bool	getSendColumnInfo();

		/** Sets the format to map columns to when fetching the list
		 *  of database names to "listformat". */
		void	setDatabaseListFormat(
					sqlrserverlistformat_t listformat);

		/** Sets the format to map columns to when fetching the list
		 *  of schema names to "listformat". */
		void	setSchemaListFormat(
					sqlrserverlistformat_t listformat);

		/** Sets the format to map columns to when fetching the list
		 *  of table names to "listformat". */
		void	setTableListFormat(
					sqlrserverlistformat_t listformat);

		/** Sets the format to map columns to when fetching the list
		 *  of table type names to "listformat". */
		void	setTableTypeListFormat(
					sqlrserverlistformat_t listformat);

		/** Sets the format to map columns to when fetching the list
		 *  of column names to "listformat". */
		void	setColumnListFormat(
					sqlrserverlistformat_t listformat);

		/** Sets the format to map columns to when fetching the list
		 *  of primary key names to "listformat". */
		void	setPrimaryKeyListFormat(
					sqlrserverlistformat_t listformat);

		/** Sets the format to map columns to when fetching the list
		 *  of key and index names to "listformat". */
		void	setKeyAndIndexListFormat(
					sqlrserverlistformat_t listformat);

		/** Sets the format to map columns to when fetching the list
		 *  of procedure parameter names to "listformat". */
		void	setProcedureParameterListFormat(
					sqlrserverlistformat_t listformat);

		/** Sets the format to map columns to when fetching the list
		 *  of type info names to "listformat". */
		void	setTypeInfoListFormat(
					sqlrserverlistformat_t listformat);

		/** Sets the format to map columns to when fetching the list
		 *  of procedure names to "listformat". */
		void	setProcedureListFormat(
					sqlrserverlistformat_t listformat);

		/** Returns the number of columns in the current result set of
		 *  "cursor".
		 *
		 *  Returns 0 if column info is not yet valid or if the query
		 *  has no result set (eg. if it was a DML or DDL query). */
		uint32_t	colCount(sqlrservercursor *cursor);

		/** Some database backends have predictable column types that
		 *  can be mapped to numeric ids.  If the client is aware of
		 *  these ids and if the protocol supports sending column types
		 *  as ids (eg. the sqlrclient protocol), then these numeric
		 *  ids can be sent to the client instead of sending column
		 *  type name strings.
		 *
		 *  Other database backends (eg. postgresql, router) don't have
		 *  predictable column types and so column type names must be
		 *  sent as strings.
		 *
		 *  This method returns COLUMN_TYPE_IDS in the first case and
		 *  COLUMN_TYPE_NAMES in the second case. */
		uint16_t	columnTypeFormat(sqlrservercursor *cursor);

		/** Returns the name of the column at position "col" in the
		 *  current result set of "cursor".
		 *
		 *  Returns NULL if column info is not yet valid or if the
		 *  query has no result set (eg. if it was a DML or DDL
		 *  query). */
		const char	*getColumnName(sqlrservercursor *cursor,
							uint32_t col);

		/** Returns the size (number of bytes) of the column name at
		 *  position "col" in the current result set of "cursor".
		 *
		 *  Returns 0 if column info is not yet valid or if the query
		 *  has no result set (eg. if it was a DML or DDL query). */
		uint16_t	getColumnNameSize(sqlrservercursor *cursor,
							uint32_t col);

		/** Returns the numeric type id of the column at position "col"
		 *  in the current result set of "cursor".
		 *
		 *  Returns UNKNOWN_DATATYPE if column info is not yet valid,
		 *  if the query has no result set (eg. if it was a DML or DDL
		 *  query, or if columnTypeFormat() returns
		 *  COLUMN_TYPE_NAMES. */
		uint16_t	getColumnType(sqlrservercursor *cursor,
							uint32_t col);

		/** Returns the type name string of the column at position "col"
		 *  in the current result set of "cursor".
		 *
		 *  Returns NULL if column info is not yet valid, if the query
		 *  has no result set (eg. if it was a DML or DDL query, or if
		 *  columnTypeFormat() returns COLUMN_TYPE_IDS. */
		const char	*getColumnTypeName(sqlrservercursor *cursor,
							uint32_t col);

		/** Returns the size (number of bytes) of the type name string
		 *  of the column at position "col" in the current result set
		 *  of "cursor".
		 *
		 *  Returns 0 if column info is not yet valid, if the query has
		 *  no result set (eg. if it was a DML or DDL query, or if
		 *  columnTypeFormat() returns COLUMN_TYPE_IDS. */
		uint16_t	getColumnTypeNameSize(
						sqlrservercursor *cursor,
							uint32_t col);

		/** Returns the size of the column at position "col" in the
		 *  current result set of "cursor".
		 *
		 *  Returns 0 if column info is not yet valid or if the query
		 *  has no result set (eg. if it was a DML or DDL query). */
		uint32_t	getColumnSize(sqlrservercursor *cursor,
							uint32_t col);

		/** Returns the precision of the column at position "col" in
		 *  the current result set of "cursor".
		 *
		 *  Returns 0 if column info is not yet valid or if the query
		 *  has no result set (eg. if it was a DML or DDL query). */
		uint32_t	getColumnPrecision(sqlrservercursor *cursor,
							uint32_t col);

		/** Returns the scale of the column at position "col" in
		 *  the current result set of "cursor".
		 *
		 *  Returns 0 if column info is not yet valid or if the query
		 *  has no result set (eg. if it was a DML or DDL query). */
		uint32_t	getColumnScale(sqlrservercursor *cursor,
							uint32_t col);

		/** Returns non-zero if the column at position "col" in the
		 *  current result set of "cursor" is nullable and 0if it is
		 *  not nullable.
		 *
		 *  Returns 0 if column info is not yet valid or if the query
		 *  has no result set (eg. if it was a DML or DDL query). */
		uint16_t	getColumnIsNullable(sqlrservercursor *cursor,
							uint32_t col);

		/** Returns non-zero if the column at position "col" in the
		 *  current result set of "cursor" is the primary key and 0 if
		 *  it is not the primary key.
		 *
		 *  Returns 0 if column info is not yet valid or if the query
		 *  has no result set (eg. if it was a DML or DDL query). */
		uint16_t	getColumnIsPrimaryKey(sqlrservercursor *cursor,
							uint32_t col);

		/** Returns non-zero if the column at position "col" in the
		 *  current result set of "cursor" is a unique column and 0 if
		 *  it is not a unique column.
		 *
		 *  Returns 0 if column info is not yet valid or if the query
		 *  has no result set (eg. if it was a DML or DDL query). */
		uint16_t	getColumnIsUnique(sqlrservercursor *cursor,
							uint32_t col);

		/** Returns non-zero if the column at position "col" in the
		 *  current result set of "cursor" is part of a key and 0 if it
		 *  is not part of a key.
		 *
		 *  Returns 0 if column info is not yet valid or if the query
		 *  has no result set (eg. if it was a DML or DDL query). */
		uint16_t	getColumnIsPartOfKey(sqlrservercursor *cursor,
							uint32_t col);

		/** Returns non-zero if the column at position "col" in the
		 *  current result set of "cursor" is unsigned and 0 if it is
		 *  signed.
		 *
		 *  Returns 0 if column info is not yet valid or if the query
		 *  has no result set (eg. if it was a DML or DDL query). */
		uint16_t	getColumnIsUnsigned(sqlrservercursor *cursor,
							uint32_t col);

		/** Returns non-zero if the column at position "col" in the
		 *  current result set of "cursor" is zero-filled and 0 if it
		 *  is not zero-filled.
		 *
		 *  Returns 0 if column info is not yet valid or if the query
		 *  has no result set (eg. if it was a DML or DDL query). */
		uint16_t	getColumnIsZeroFilled(sqlrservercursor *cursor,
							uint32_t col);

		/** Returns non-zero if the column at position "col" in the
		 *  current result set of "cursor" is binary and 0 if it is not
		 *  binary.
		 *
		 *  Returns 0 if column info is not yet valid or if the query
		 *  has no result set (eg. if it was a DML or DDL query). */
		uint16_t	getColumnIsBinary(sqlrservercursor *cursor,
							uint32_t col);

		/** Returns non-zero if the column at position "col" in the
		 *  current result set of "cursor" is an auto-increment column
		 *  and 0 if it is not an auto-increment column.
		 *
		 *  Returns 0 if column info is not yet valid or if the query
		 *  has no result set (eg. if it was a DML or DDL query). */
		uint16_t	getColumnIsAutoIncrement(
						sqlrservercursor *cursor,
							uint32_t col);

		/** Returns the name of the table that the column at position
		 *  "col" in the current result set of "cursor" is from.
		 *
		 *  Returns NULL if column info is not yet valid, if the
		 *  query has no result set (eg. if it was a DML or DDL
		 *  query), or if the database backend doesn't know what
		 *  table the column came from. */
		const char	*getColumnTable(sqlrservercursor *cursor,
							uint32_t col);

		/** Returns the size (number of bytes) of the name of the table
		 *  that the column at position "col" in the current result set
		 *  of "cursor" is from.
		 *
		 *  Returns NULL if column info is not yet valid, if the query
		 *  has no result set (eg. if it was a DML or DDL query), or if
		 *  the database backend doesn't know what table the column
		 *  came from. */
		uint16_t	getColumnTableSize(sqlrservercursor *cursor,
							uint32_t col);

		/** Prepares and executes "query", but doesn't fetch any
		 *  results, and appends a comma-separated list of the names of
		 *  the columns in the result set to "output".
		 *
		 *  Returns false if "query" was null or an empty string and
		 *  true otherwise.  If the query fails then nothing is
		 *  appended to "output". */
		bool	getColumnNames(const char *query, stringbuffer *output);

		/** Appends a comma-separated list of the names of the columns
		 *  in the current result of "cursor" to "output".  Nothing is
		 *  appended to "output" if no query has been executed yet, or
		 *  if the query has no result set. */
		void	getColumnNames(sqlrservercursor *cursor,
							stringbuffer *output);



		// result set navigation...

		/** Some databases know the row count of a result set prior to
		 *  fetching any results.  Other databases do not, or do not
		 *  always know the row count.  For the current result set of
		 *  "cursor", this method returns true in the first case and
		 *  false in the second. */
		bool	knowsRowCount(sqlrservercursor *cursor);

		/** If the database knows the row count of a result set prior to
		 *  fetching any results (see knowsRowCount()) then this method
		 *  returns the number of rows in the result set.  For the
		 *  current result set of "cursor", if the database does not
		 *  know the row count, then this method returns the number of
		 *  rows that have currently been fetched. */
		uint64_t	rowCount(sqlrservercursor *cursor);

		/** Most databases know the affected row count, but some
		 *  databases (eg. firebird, sqlite, some versions of freetds)
		 *  do not.  For the current result set of "cursor", this
		 *  method returns true in the first case and false in the
		 *  second. */
		bool	knowsAffectedRows(sqlrservercursor *cursor);

		/** Sets the number of affected rows for the current result set
		 *  of "cursor" to "affectedrows". */
		void	setAffectedRows(sqlrservercursor *cursor,
						uint64_t affectedrows);

		/** Returns the number of affected rows for the current result
		 *  set of "cursor", as set by the most recent call to
		 *  setAffectedRows(). */
		uint64_t	getAffectedRows(sqlrservercursor *cursor);

		/** Returns true if the current result set of "cursor" has no
		 *  rows to return and false if there are rows to return. */
		bool	noRowsToReturn(sqlrservercursor *cursor);

		/** Fetches the next row of the current result set of "cursor".
		 *  Sets "error" to true if an error occurred and to false if
		 *  no error occurred.
		 *
		 *  Returns true if a row was fetched and false if no row was
		 *  fetched, either because an error occurred or because all
		 *  rows have already been fetched. */
		bool	fetchRow(sqlrservercursor *cursor, bool *error);

		/** Skips the next row of the current result set of "cursor".
		 *  If the database supports skipping without fetching, then
		 *  this is more efficient than skipping a row by just fetching
		 *  it.  Sets "error" to true if an error occurred and to false
		 *  if no error occurred.
		 *
		 *  Returns true if a row was fetched and false if no row was
		 *  fetched, either because an error occurred or because all
		 *  rows have already been fetched. */
		bool	skipRow(sqlrservercursor *cursor, bool *error);

		/** Skips the next "rows" rows of the current result set of
		 *  "cursor".  If the database supports skipping without
		 *  fetching, then this is more efficient than skipping rows
		 *  by just fetching them.  If the database supports skipping
		 *  multiple rows at a time, without fetching them, then this
		 *  is more efficient than calling skipRow() "rows" times.
		 *  Sets "error" to true if an error occurred and to false if
		 *  no error occurred.
		 *
		 *  Returns true if a row was fetched and false if no row was
		 *  fetched, either because an error occurred or because all
		 *  rows have already been fetched. */
		bool	skipRows(sqlrservercursor *cursor, uint64_t rows,
								bool *error);

		/** Sets internal flags and/or counters related to moving to
		 *  the next row of the current result set of "cursor".  Must
		 *  be called after fetchRow().  Note that this method is
		 *  kludgy and may be removed in the future. */
		void	nextRow(sqlrservercursor *cursor);

		/** Returns the total number of rows that have been skipped or
		 *  fetched in the current result set of "cursor" since the
		 *  query was executed.
		 *
		 *  Note that this differs from rowCount() in that it always
		 *  returns only the number of rows that have been skipped or
		 *  fetched so far, independent of knowsRowCount(), whereas the
		 *  behavior of rowCount() depends on knowsRowCount(). */
		uint64_t	getTotalRowsFetched(sqlrservercursor *cursor);

		/** Suspends the current result set of "cursor". */
		void	suspendResultSet(sqlrservercursor *cursor);

		/** Closes the current result set of "cursor". */
		void	closeResultSet(sqlrservercursor *cursor);

		/** Closes the current result set of all cursors open in the
		 *  current connection. */
		void	closeAllResultSets();



		// fields...

		/** Fetches information about the field in column "col" of the
		 *  current row, of the current result set of "cursor".  Sets
		 *  "field" to the value of the field, "fieldsize" to the
		 *  number of bytes in the value, "lob" true if the field is
		 *  a LOB or false otherwise, and "null" true if the field is
		 *  null or false otherwise.
		 *
		 *  Returns true on success and false if an error occurred. */
		bool	getField(sqlrservercursor *cursor,
						uint32_t col,
						const char **field,
						uint64_t *fieldsize,
						bool *lob,
						bool *null);

		/** If the field in column "col" of the current row, of the
		 *  current result set of "cursor" is a LOB, then this method
		 *  sets "length" to the number of characters in the field.
		 *
		 *  Returns true on success and false if an error occurred.
		 *
		 *  Note that "length" is set to the number of characters
		 *  rather than bytes.  In the case of a binary LOB, the number
		 *  of characters and bytes will be the same, but in the case
		 *  of a text LOB, they will be different if the LOB uses a
		 *  variable width encoding such as UTF-8 or a fixed width
		 *  encoding of more than 8-bits, such as UCS2. */
		bool	getLobFieldLength(sqlrservercursor *cursor,
						uint32_t col,
						uint64_t *length);

		/** If the field in column "col" of the current row, of the
		 *  current result set of "cursor" is a LOB, then this method
		 *  attempts to fetch "charstoread" characters from character
		 *  position "offset" into "buffer" of size "buffer".  The
		 *  actual number of characters that were read is returned
		 *  in "charsread".  "charsread" may be less than "charstoread"
		 *  if "charsread" characters won't fit in "buffer" or if
		 *  the end of the LOB was reached prior to fetching
		 *  "charstoread".  0 will be returned in "charsread" if an
		 *  attempt is made to read past the end of the LOB.
		 *
		 *  Note that "offset", "charstoread", and "charsread" all
		 *  refer to numbers of characters, while "buffersize" refers
		 *  to numbers of bytes.
		 *
		 *  Returns true on success and false if an error occurred.
		 *
		 *  Note that attempting to read past the end of the LOB is not
		 *  considered to be an error.*/
		bool	getLobFieldSegment(sqlrservercursor *cursor,
						uint32_t col,
						char *buffer,
						uint64_t buffersize,
						uint64_t offset,
						uint64_t charstoread,
						uint64_t *charsread);

		/** If the field in column "col" of the current row, of the
		 *  current result set of "cursor" is a LOB then this method
		 *  closes the LOB. */
		void	closeLobField(sqlrservercursor *cursor,
						uint32_t col);



		// errors...

		/** Fetches the current cursor-level error into the
		 *  cursor's error buffer, unless there is already an
		 *  error saved in the buffer. */
		void	saveError(sqlrservercursor *cursor);

		/** Returns the error message and code by:
		 *
		 *  Setting "errorbuffer" to the cursor-level error buffer.
		 *  Populating "errorsize" with the number of bytes in the
		 *  error buffer.
		 *  Populating "errorcode" with the cursor-level numeric
		 *  error code.
		 *  Populating "liveconnection" with true if the connection to
		 *  the database is still up.
		 *  Populating "liveconnection" with true if the connection to
		 *  the database is down. */
		void	getError(sqlrservercursor *cursor,
						const char **errorbuffer,
						uint32_t *errorsize,
						int64_t *errorcode,
						bool *liveconnection);

		/** Returns the error message and code by:
		 *
		 *  Copying at most "errorbuffersize" bytes from the
		 *  cursor-level error buffer into "errorbuffer".
		 *  Populating "errorsize" with the number of bytes in the
		 *  error buffer.
		 *  Populating "errorcode" with the cursor-level numeric
		 *  error code.
		 *  Populating "liveconnection" with true if the connection to
		 *  the database is still up.
		 *  Populating "liveconnection" with true if the connection to
		 *  the database is down. */
		void	getError(sqlrservercursor *cursor,
						char *errorbuffer,
						uint32_t errorbuffersize,
						uint32_t *errorsize,
						int64_t *errorcode,
						bool *liveconnection);

		/** Empties the cursor-level error buffer and sets a flag
		 *  indicating that the connection to the database is up. */
		void	clearError(sqlrservercursor *cursor);

		/** Copies "errsize" bytes of "err" into the cursor-level
		 *  error buffer, sets the cursor-level numeric error code
		 *  to "errn" and sets a flag indicating that the connection to
		 *  the database is up. */
		void	setError(sqlrservercursor *cursor,
						const char *err,
						uint32_t errsize,
						int64_t errn,
						bool liveconn);

		/** Copies "err" into the cursor-level error buffer,
		 *  sets the cursor-level numeric error code to "errn" and
		 *  sets a flag indicating that the connection to the database
		 *  is up. */
		void	setError(sqlrservercursor *cursor,
						const char *err,
						int64_t errn,
						bool liveconn);

		/** Returns a pointer to the cursor-level error buffer. */
		char	*getErrorBuffer(sqlrservercursor *cursor);

		/** Returns the size, in bytes, of the cursor-level error
		 *  buffer. */
		uint32_t	getErrorBufferSize(sqlrservercursor *cursor);

		/** Sets the number of bytes currently stored in the
		 *  cursor-level error buffer to "errorsize". */
		void	setErrorSize(sqlrservercursor *cursor,
							uint32_t errorsize);

		/** Returns the number of bytes currently stored in the
		 *  cursor-level error buffer, as set by setErrorSize(). */
		uint32_t	getErrorSize(sqlrservercursor *cursor);

		/** Sets the cursor-level numeric error code to "errnum". */
		void	setErrorNumber(sqlrservercursor *cursor,
							uint32_t errnum);

		/** Returns the cursor-level numeric error code as set by
		 *  setErrorNumber(). */
		uint32_t	getErrorNumber(sqlrservercursor *cursor);

		/** Sets a flag indicating whether the connection to the
		 *  database is up to "liveconnection". */
		void	setLiveConnection(sqlrservercursor *cursor,
							bool liveconnection);

		/** Returns the flag indicating whether the connection to the
		 *  database is up, as set by setLiveConnection(). */
		bool	getLiveConnection(sqlrservercursor *cursor);



		// cursor state...

		/** Sets the state of "cursor" to "state". */
		void	setState(sqlrservercursor *cursor,
						sqlrcursorstate_t state);

		/** Returns the state of "cursor" as set by the most recent
		 *  call to setState(). */
		sqlrcursorstate_t	getState(sqlrservercursor *cursor);



		// memory pools...

		/** Returns the memory pool that persists for the duration of a
		 *  client session and is cleared at the end of each
		 *  session. */
		memorypool	*getPerSessionMemoryPool();

		/** Returns the memory pool that persists for the duration of a
		 *  transaction, and is cleared at the end of each
		 *  transaction. */
		memorypool	*getPerTransactionMemoryPool();



		// query parser...

		/** Returns the query parser that is configured to be used by
		 *  this instance or the default parser if none was
		 *  configured. */
		sqlrparser	*getParser();



		// gss...

		/** Returns the GSS context in use by the current client
		 *  connection, or NULL if no GSS context is in use. */
		gsscontext	*getGssContext();



		// tls...

		/** Returns the TLS context in use by the current client
		 *  connection, or NULL if no TLS context is in use. */
		tlscontext	*getTlsContext();



		// configuration...

		/** Returns the configuration for the this instance. */
		sqlrconfig	*getConfig();

		/** Returns the paths for this instance. */
		sqlrpaths	*getPaths();



		// shared memory...

		/** Returns the shared memory segment that this instance is
		 *  using to communicate with the sqlr-listener, sqlr-scaler,
		 *  sqlr-status, and other processes. */
		sqlrshm	*getShm();



		// module data...

		/** Returns the instance of module data assigned to "id" or
		 *  NULL if no module data was assigned to "id". */
		sqlrmoduledata	*getModuleData(const char *id);



		// utilities...

		/** Advances "ptr" past any SQL comments, without advancing
		 *  it past "endptr". */
		bool	skipComment(const char **ptr, const char *endptr);

		/** Advances "ptr" past any whitespace, without advancing it
		 *  past "endptr". */
		bool	skipWhitespace(const char **ptr, const char *endptr);

		/** Skips any whitespace at the beginning of "query" and
		 *  returns a pointer to the first character after the
		 *  whitespace. */
		const char	*skipWhitespace(const char *query);

		/** Skips any SQL comments at the beginning of "query" and
		 *  returns a pointer to the first character after the
		 *  whitespace. */
		const char	*skipComments(const char *query);

		/** Skips any whitespace and SQL comments at the beginning of
		 *  "query" and returns a pointer to the first character after
		 *  the whitespace. */
		const char	*skipWhitespaceAndComments(const char *query);

		/** Returns a 2-character hex representation of "ch". */
		const char	*asciiToHex(byte_t ch);

		/** Returns a 2-digit octal representation of "ch". */
		const char	*asciiToOctal(byte_t ch);

		/** Returns true if the first "querysize" bytes of "query"
		 *  contain any bind variables. */
		bool	hasBindVariables(const char *query, uint32_t querysize);

		/** Returns the number of bind variables found in the first
		 *  "querysize" bytes of "query". */
		uint16_t	countBindVariables(const char *query,
							uint32_t querysize);

		/** Splits "combinedobject" into "db", "schema", and "object".
		 *
		 *  If "combinedobject" consisted of 3 parts, then "currentdb"
		 *  and "currentschema" are ignored, "db" is set to the first
		 *  part, "schema" is set to the second part, and "object" is
		 *  set to the third part.
		 *
		 *  If "combineeobject" consisted of 2 parts, then if the first
		 *  part is the same as "currentdb" then "db" is set to
		 *  "currentdb" ,"schema" is set to "currentschema" and
		 *  "object" is set to the second part.  If the first part is
		 *  not the same as "currentdb" then "db" is set to "currentdb",
		 *  "schema" is set to the first part, and "object" is set to
		 *  the second part.
		 *
		 *  If "combinedobject" only consisted of 1 part, then "db" is
		 *  set to "currentdb", "schema" is set to "currentschema", and
		 *  "object" is set to "combinedobject".
		 *
		 *  Note that "db", "schema", and "object" point to internal
		 *  buffers that are overwritten by each call to
		 *  splitObjectName(). */
		void	splitObjectName(const char *currentdb,
						const char *currentschema,
						const char *combinedobject,
						const char **db,
						const char **schema,
						const char **object);

		/** Parses the first "querysize" bytes of "query", which is
		 *  presumed to be some type of normalized insert query.
		 *
		 *  "querytype" is set to one of SQLRQUERYTYPE_INSERT,
		 *  SQLRQUERYTYPE_MULTIINSERT, SQLRQUERYTYPE_INSERTSELECT, or
		 *  SQLRQUERYTYPE_SELECT, depending on what type of insert
		 *  statement was found.  If the query type could not be
		 *  determined then "querytype" is set to SQLRQUERYTYPE_ETC.
		 *
		 *  If the query was an insert, multi-insert, or insert-select,
		 *  then...
		 *
		 *  "table" is set to the name of the table that the insert
		 *  is targeting.
		 *
		 *  If the query specified a set of columns, then "columns"
		 *  is set to a list of columns.  If the query does
		 *  not specify a set of columns, then "columns" is populated
		 *  with the full set of columns in the table.
		 *
		 *  "allcolumns" is populated with the full set of columns in
		 *  the table.
		 *
		 *  "autoinccolumn" is populated with the name of the table's
		 *  auto-increment column, or NULL if the table does not contain
		 *  an auto-increment column.
		 *
		 *  "columnsincludeautoinccolumn" is set to true if "columns"
		 *  contains the auto-increment column, and false if it does
		 *  not.
		 *
		 *  "primarykeycolumn" is populated with the name of the table's
		 *  primary key column, or NULL if the table does not contain
		 *  a primary key column.
		 *
		 *  "columnsincludeprimarykeycolumn" is set to true if "columns"
		 *  contains the primary key column, and false if it does
		 *  not.
		 *
		 *  "values" is populated with the first set of values
		 *  provided by the query.  If the query is a multi-insert
		 *  then subsequent sets of values are not returned.  If the
		 *  query is an insert-select then values is set to NULL.
		 *
		 *  "rawvalues" is set to the opening parentheses in "query"
		 *  that begins the set of values provided by the query.
		 *  If the query is an insert-select then rawvalues is set to
		 *  NULL.
		 *
		 *  If the query is a select-into then "table", "columns",
		 *  "allcolumns", "autoinccolumn", "primarykeycolumn",
		 *  "values", and "rawvalues" are set to NULL, and
		 *  "columnsincludeautoinccolumn" and
		 *  "columnsincludeprimarykeycolumn" are set to false.
		 *
		 *  Note that "table", "columns", "allcolumns",
		 *  "autoinccolumn", "primarykeycolumn", "values", and
		 *  "rawvalues" are allocated internally and returned, and must
		 *  be deallocated by the calling program.  Note also that the
		 *  linkedlists "columns", "allcolumns", and "values" manage
		 *  the values stored in them.  Thus deallocating or clearing
		 *  the linkedlists deallocates the values stored in them.
		 *
		 *  Returns true if parsing succeeded and false if the query
		 *  was somehow malformed. */
		bool	parseInsert(const char *query,
					uint32_t querysize,
					sqlrquerytype_t *querytype,
					char **table,
					linkedlist<char *> **columns,
					linkedlist<char *> **allcolumns,
					const char **autoinccolumn,
					bool *columnsincludeautoinccolumn,
					const char **primarykeycolumn,
					bool *columnsincludeprimarykeycolumn,
					linkedlist<char *> **values,
					const char **rawvalues);

		/** Attempts to parse the first "valuesize" bytes of "value",
		 *  which is presumed to be a date, time, or date/time and
		 *  reformat it as specified by the other parameters.
		 *
		 *  Handles a wide variety of date/time formats.
		 *
		 *  If "ddmm" is set true then the date format is assumed to
		 *  be dd/mm/yyyy rather than mm/dd/yyyy when a date with a
		 *  trailing year is encountered.
		 *
		 *  If "yyyyddmm" is set true then the date format is assumed
		 *  to be yyyy/dd/mm rather than yyyy/mm/dd when a date with
		 *  a leading year is encountered.
		 *
		 *  "datedelimiters" may be set to a set of valid date
		 *  delimiters and may contain any combination of '/', '-', '.',
		 *  and ':'.  Eg. "/-" would mean that only '/' and '-' are
		 *  valid date delimiters.  If left NULL then it defaults to
		 *  "/-.:"
		 *
		 *  If "value" is determined to be a date/time then it is
		 *  converted to the format specified by "datetimeformat".
		 *
		 *  If "value" is determined to be a date then it is converted
		 *  to the format specified by "dateformat".
		 *
		 *  If "value" is determined to be a time then it is converted
		 *  to the format specified by "timeformat".
		 *
		 *  The reformatted "value" is written to "newvalue" and
		 *  "newvaluesize" is set to the number of bytes that were
		 *  written to "newvalue", not including the NULL terminator.
		 *  Note that "newvalue" points to an internal buffer that will
		 *  be overwritten by the next call to reformatDateTimes().
		 *
		 *  Returns true if the date/time was successfully parsed and
		 *  false if it failed to parse the date/time. */
		bool	reformatDateTime(const char *value,
						uint64_t valuesize,
						const char **newvalue,
						uint64_t *newvaluesize,
						bool ddmm, bool yyyyddmm,
						const char *datedelimiters,
						const char *datetimeformat,
						const char *dateformat,
						const char *timeformat);

		/** Compares (object names) str1 and str2, ignoring any
		 *  quoting by single-quotes, double-quotes, back-quotes, or
		 *  square brackets.
		 *
		 *  Eg. the following strings would be considered equivalent:
		 *
		 *  * object
		 *  * 'object'
		 *  * "object"
		 *  * `object`
		 *  * [object]
		 *
		 *  Returns 0 if the strings are equivalent, a value less than
		 *  1 if str1 is less than str2, and a value greater than 1 if
		 *  str2 is greater than str1.
		 *
		 *  Also returns 0 if both strings are NULL, 1 if str1 is NULL
		 *  but str2 is not, and -1 if str2 is NULl but str1 is not. */
		int32_t	compareQuoted(const char *str1, const char *str2);

		/** Returns true if datatype string "type" is a bit type and
		 *  false otherwise. */
		bool	isBitType(const char *type);

		/** Returns true if datatype id "type" is a bit type and false
		 *  otherwise. */
		bool	isBitType(int16_t type);

		/** Returns true if datatype string "type" is a bool type and
		 *  false otherwise. */
		bool	isBoolType(const char *type);

		/** Returns true if datatype id "type" is a bool type and false
		 *  otherwise. */
		bool	isBoolType(int16_t type);

		/** Returns true if datatype string "type" is a float type and
		 *  false otherwise. */
		bool	isFloatType(const char *type);

		/** Returns true if datatype id "type" is a float type and false
		 *  otherwise. */
		bool	isFloatType(int16_t type);

		/** Returns true if datatype string "type" is a number type and
		 *  false otherwise. */
		bool	isNumberType(const char *type);

		/** Returns true if datatype id "type" is a number type and
		 *  false otherwise. */
		bool	isNumberType(int16_t type);

		/** Returns true if datatype string "type" is a blob type and
		 *  false otherwise. */
		bool	isBlobType(const char *type);

		/** Returns true if datatype id "type" is a blob type and false
		 *  otherwise. */
		bool	isBlobType(int16_t type);

		/** Returns true if datatype string "type" is a clob type and
		 *  false otherwise. */
		bool	isClobType(const char *type);

		/** Returns true if datatype id "type" is a clob type and false
		 *  otherwise. */
		bool	isClobType(int16_t type);

		/** Returns true if datatype string "type" is an unsigned type
		 *  and false otherwise. */
		bool	isUnsignedType(const char *type);

		/** Returns true if datatype id "type" is an unsigned type and
		 *  false otherwise. */
		bool	isUnsignedType(int16_t type);

		/** Returns true if datatype string "type" is a binary type and
		 *  false otherwise. */
		bool	isBinaryType(const char *type);

		/** Returns true if datatype id "type" is a binary type and
		 *  false otherwise. */
		bool	isBinaryType(int16_t type);

		/** Returns true if datatype string "type" is a date/time type
		 *  and false otherwise. */
		bool	isDateTimeType(const char *type);

		/** Returns true if datatype id "type" is a date/tim type and
		 *  false otherwise. */
		bool	isDateTimeType(int16_t type);

		/** Returns the full set of known datatype strings. */
		const char * const	*dataTypeStrings();

		/** Returns a string representation of log level "level". */
		const char	*getLogLevel(sqlrloglevel_t level);

		/** Returns the sqlrloglevel_t corresponding to string
 		 *  "level". */
		sqlrloglevel_t	getLogLevel(const char *level);

		/** Returns a string representation of event type "event". */
		const char	*getEventType(sqlrevent_t event);

		/** Returns the sqlrevent_t corresponding to string "event". */
		sqlrevent_t	getEventType(const char *event);

	#include <sqlrelay/private/sqlrservercontroller.h>
};

class SQLRSERVER_DLLSPEC sqlrserverconnection : public sqlrserverbase {
	public:

		/** Creates an instance of sqlrserverconnection. */
		sqlrserverconnection(sqlrservercontroller *cont);

		/** Deletes this instance of sqlrserverconnection. */
		virtual	~sqlrserverconnection();

		/** Returns true if the process must detach from the
		 *  controlling tty prior to logging in to the database, and
		 *  false if the process may log in prior to detaching from the
		 *  controlling tty.
		 *
		 *  Returns false by default but may be overridden by a child
		 *  class. */
		virtual bool	mustDetachBeforeLogIn();

		/** Parses the database connect string.
		 *
		 *  This implementation parses out the following connect string
		 *  parameters: 
		 *
		 *   * user
		 *   * password
		 *   * autocommit
		 *   * faketransactionblocks
		 *   * fakebinds
		 *   * fetchatonce
		 *   * maxcolumncount
		 *   * maxfieldsize
		 *   * connecttimeout
		 *   * querytimeout
		 *   * executedirect
		 *   * detachbeforelogin
		 *
		 *  A child class may override this method to parse additional
		 *  parameters. */
		virtual	void	handleConnectString();

		/** Sends "size" bytes of "data" to the database. */
		virtual	bool	send(byte_t *data, size_t size);

		/** Receives "size" bytes from the database into buffer
		 *  "data". */
		virtual	bool	recv(byte_t **data, size_t *size);

		/** Logs in to the database.
		 * 
		 *  Returns true on success, even if a warning occurred, and
		 *  false if an error occurred.
		 *
		 *  If an error occurred, "error" is set to a string
		 *  containing the error.
		 *
		 *  If a warning occurred, "warning" is set to a string
		 *  containing the warning.
		 *
		 *  Note that both "error" and "warning" are set to internal
		 *  buffers which will be overwritten the next time logIn()
		 *  is called. */
		virtual	bool	logIn(const char **error,
					const char **warning)=0;

		/** Logs out of the database. */
		virtual	void	logOut()=0;

		/** By default, logs out of the database and back in as
		 *  "newuser", authenticated using password "newpassword".
		 *
		 *  However if the database supports a more efficient strategy
		 *  for changing users then this method may be overridden by a
		 *  child class to use that strategy.
		 *
		 *  Returns true if login was successful and false otherwise. */
		virtual	bool	changeUser(const char *newuser,
						const char *newpassword);

		/** If the database supports proxied users, then this method
		 *  switches from the current user to "newuser" using password
		 *  "newpassword".
		 *
		 *  This implementation just returns false, but may be
		 *  overridden by a child class to change proxied users if
		 *  the database supports a way of doing that.
		 *
		 *  Returns true if the user-change was successful and false
		 *  otherwise. */
		virtual	bool	changeProxiedUser(const char *newuser,
						const char *newpassword);

		/** Set auto-commit on.
		 *  Returns true on success and false on failure. */
		virtual bool	setAutoCommitOn();

		/** Set auto-commit off.
		 *
		 *  Returns true on success and false on failure. */
		virtual bool	setAutoCommitOff();

		/** Returns true if the database is transactional and false
		 *  otherwise.
		 *
		 *  Returns true by default but may be overridden by a child
		 *  class. */
		virtual bool	isTransactional();

		/** Returns true if the database supports begin-commit/rollback
		 *  transaction blocks (eg. the behavior of most databases) and
		 *  false it a commit/rollback just begins another transaction
		 *  (eg. the behavior of oracle databases).
		 *
		 *  Returns true by default, but may be overridden by a child
		 *  class. */
		virtual bool	supportsTransactionBlocks();

		/** Begins a new transaction.
		 * 
		 *  Returns true on success and false on failure. */
		virtual bool	begin();

		/** Returns the query that the database uses to begin a
		 *  transcation. */
		virtual const char	*beginTransactionQuery();

		/** Commits the current transaction.
		 *  
		 *  Returns true on success and false on failure. */
		virtual bool	commit();

		/** Rolls the current transaction back.
		 *  
		 *  Returns true on success and false on failure. */
		virtual bool	rollback();

		/** Returns the error message and code by:
		 *
		 *  Copying at most "errorbuffersize" bytes from the
		 *  connection-level error buffer into "errorbuffer".
		 *  Populating "errorsize" with the number of bytes in the
		 *  error buffer.
		 *  Populating "errorcode" with the connection-level numeric
		 *  error code.
		 *  Populating "liveconnection" with true if the connection to
		 *  the database is still up.
		 *  Populating "liveconnection" with true if the connection to
		 *  the database is down. */
		virtual	void	getError(char *errorbuffer,
						uint32_t errorbuffersize,
						uint32_t *errorsize,
						int64_t *errorcode,
						bool *liveconnection)=0;

		/** Selects database "db".
		 *  
		 *  Returns true if selection succeeded and false otherwise. */
		virtual bool		selectDatabase(const char *database);

		/** Returns the query that the database uses to select a
		 *  database, which includes a %s which can be used to
		 *  substitute in the name of the database to select.
		 *
		 *  This implementation just returns NULL, but it may be
		 *  overridden by child class to return the query. */
		virtual const char	*selectDatabaseQuery();

		/** Returns the current database.
		 *
		 *  Note that this method allocates a buffer for the return
		 *  value internally and returns it.  The calling method must
		 *  deallocate this buffer. */
		virtual char		*getCurrentDatabase();

		/** Returns the query that the database uses to return the
		 *  database that is currently in use.
		 *
		 *  This implementation just returns getNoopQuery(), but it may
		 *  be overridden by a child class to return the query. */
		virtual const char	*getCurrentDatabaseQuery();

		/** Returns the current database.
		 *
		 *  Note that this method allocates a buffer for the return
		 *  value internally and returns it.  The calling method must
		 *  deallocate this buffer. */
		virtual char		*getCurrentSchema();

		/** Returns the query that the database uses to return the
		 *  schema that is currently in use.
		 *
		 *  This implementation just returns getNoopQuery(), but it may
		 *  be overridden by a child class to return the query. */
		virtual const char	*getCurrentSchemaQuery();

		/** Returns the last-insert-id.  That is, if an insert was
		 *  performed into a table with an auto-increment field, then
		 *  this method returns the auto-increment-id that was
		 *  generated in "id".
		 *
		 *  Returns true on succcess and false if an error occurred. */
		virtual bool	getLastInsertId(uint64_t *id);

		/** Returns the query that the database uses to fetch the
		 *  last-insert-id.  That is, if an insert was performed into a
		 *  table with an auto-increment field, then this method
		 *  returns the query that can be used to fetch that
		 *  auto-increment-id.
		 *
		 *  This implementation just returns getNoopQuery(), but it may
		 *  be overridden by a child class to return the query. */
		virtual const char	*getLastInsertIdQuery();

		/** Returns the no-op query for this the database.  That is, a
		 *  query that safely doesn't do anything - doesn't modify any
		 *  tables, return any result set, or set any parameters.
		 *
		 *  This implementation just returns "", but it may be
		 *  overridden by a child class to return a more appropriate
		 *  query for this database. */
		virtual const char	*getNoopQuery();

		/** Sets the transaction isolation level to "isolevel".
		 *
		 *  Returns true on success and false if an error occurred. */
		virtual bool	setIsolationLevel(const char *isolevel);

		/** Returns the query that the database uses to set the
		 *  isolation level, which includes a %s that can be used to
		 *  substitute in the name of the isolation level.
		 *
		 *  This implementation returns "set transaction isolation
		 *  level %s", but it may be overridden by a child class if the
		 *  database requires a different query. */
		virtual const char	*setIsolationLevelQuery();

		/** Pings the database using the "ping query" defined by the
		 *  database connection module.
		 *
		 *  Returns true if the ping succeeded and false if it
		 *  failed. */
		virtual bool	ping();

		/** Returns an inexpensive query that may be used to determine
		 *  if the database is alive or not.
		 *
		 *  This implementation returns "select 1", but it may be
		 *  overridden by a child class to return a more approprite
		 *  query for this database. */
		virtual const char	*pingQuery();

		/** Returns the type of database: oracle, mysql, postgresql,
		 *  odbc, etc. */
		virtual const char	*getDbType()=0;

		/** Returns the database version. */
		virtual	const char	*getDbVersion()=0;

		/** Returns the host name of the server hosting the
		 *  database. */
		virtual const char	*getDbHostName();

		/** Returns the query that the database uses to return its host
		 *  name in a result set consisting of a single row and column.
		 *
		 *  This method just returns NULL, but it may be overridden by
		 *  a child class to return the query. */
		virtual const char	*getDbHostNameQuery();

		/** Returns the IP address of the server hosting the
		 *  database. */
		virtual const char	*getDbIpAddress();

		/** Returns the query that the database uses to return its ip
		 *  address in a result set consisting of a single row and
		 *  column.
		 *
		 *  This implementation just returns NULL, but it may be
		 *  overridden by a child class to return the query. */
		virtual const char	*getDbIpAddressQuery();

		virtual bool	cacheDbHostInfo();

		/** Returns true if the currently loaded database connection
		 *  module fetches lists (database lists, table lists, column
		 *  lists, etc.) via database API call and false if it fetches
		 *  lists via query. */
		virtual bool	getListsByApiCalls();

		/** Returns the format that the result set that database lists
		 *  are returned in.
		 *
		 *  This implementation returns SQLRSERVERLISTFORMAT_MYSQL, but
		 *  may be overridden by a child class to return a different
		 *  result. */
		virtual	sqlrserverlistformat_t	getNativeDatabaseListFormat();

		/** Returns the format that the result set that schema lists
		 *  are returned in.
		 *
		 *  This implementation returns SQLRSERVERLISTFORMAT_MYSQL, but
		 *  may be overridden by a child class to return a different
		 *  result. */
		virtual	sqlrserverlistformat_t	getNativeSchemaListFormat();

		/** Returns the format that the result set that table lists
		 *  are returned in.
		 *
		 *  This implementation returns SQLRSERVERLISTFORMAT_MYSQL, but
		 *  may be overridden by a child class to return a different
		 *  result. */
		virtual	sqlrserverlistformat_t	getNativeTableListFormat();

		/** Returns the format that the result set that table type lists
		 *  are returned in.
		 *
		 *  This implementation returns SQLRSERVERLISTFORMAT_MYSQL, but
		 *  may be overridden by a child class to return a different
		 *  result. */
		virtual	sqlrserverlistformat_t	getNativeTableTypeListFormat();

		/** Returns the format that the result set that column lists
		 *  are returned in.
		 *
		 *  This implementation returns SQLRSERVERLISTFORMAT_MYSQL, but
		 *  may be overridden by a child class to return a different
		 *  result. */
		virtual	sqlrserverlistformat_t	getNativeColumnListFormat();

		/** Returns the format that the result set that primary key
		 *  lists are returned in.
		 *
		 *  This implementation returns SQLRSERVERLISTFORMAT_MYSQL, but
		 *  may be overridden by a child class to return a different
		 *  result. */
		virtual	sqlrserverlistformat_t	getNativePrimaryKeyListFormat();

		/** Returns the format that the result set that key and index
		 *  lists are returned in.
		 *
		 *  This implementation returns SQLRSERVERLISTFORMAT_MYSQL, but
		 *  may be overridden by a child class to return a different
		 *  result. */
		virtual	sqlrserverlistformat_t
					getNativeKeyAndIndexListFormat();

		/** Returns the format that the result set that procedure
		 *  parameter lists are returned in.
		 *
		 *  This implementation returns SQLRSERVERLISTFORMAT_MYSQL, but
		 *  may be overridden by a child class to return a different
		 *  result. */
		virtual	sqlrserverlistformat_t
					getNativeProcedureParameterListFormat();

		/** Returns the format that the result set that type info lists
		 *  are returned in.
		 *
		 *  This implementation returns SQLRSERVERLISTFORMAT_MYSQL, but
		 *  may be overridden by a child class to return a different
		 *  result. */
		virtual	sqlrserverlistformat_t	getNativeTypeInfoListFormat();

		/** Returns the format that the result set that procedure lists
		 *  are returned in.
		 *
		 *  This implementation returns SQLRSERVERLISTFORMAT_MYSQL, but
		 *  may be overridden by a child class to return a different
		 *  result. */
		virtual	sqlrserverlistformat_t	getNativeProcedureListFormat();

		/** Makes the database API call to fetch the list of databases
		 *  that are visible to the user that SQL Relay is logged in
		 *  as.  Only returns database names that match wildcard "wild"
		 *  if "wild" is non-NULL.
		 *
		 *  Returns true on success and false on failure. */
		virtual bool	getDatabaseList(sqlrservercursor *cursor,
							const char *wild);

		/** Makes the database API call to fetch the list of schemas,
		 *  in the current database, that are visible to the user that
		 *  SQL Relay is logged in as.  Only returns schema names that
		 *  match wildcard "wild" if "wild" is non-NULL.
		 *
		 *  Returns true on success and false on failure. */
		virtual bool	getSchemaList(sqlrservercursor *cursor,
							const char *wild);

		/** Makes the databaase API call to fetch the list of tables
		 *  (and table-like objects), in the current database and
		 *  schema, that are visible to the user that SQL Relay is
		 *  logged in as.  "objecttypes" should be an or-ed set of one
		 *  or more of the following object types:
		 *
		 *  DB_OBJECT_TABLE
		 *  DB_OBJECT_VIEW
		 *  DB_OBJECT_ALIAS
		 *  DB_OBJECT_SYNONYM
		 *
		 *  Only returns table names that match wildcard "wild" if
		 *  "wild" is non-NULL.
		 *
		 *  Returns true on success and false on failure. */
		virtual bool	getTableList(sqlrservercursor *cursor,
							const char *wild,
							uint16_t objecttypes);

		/** Makes the database API call to fetch the list of table type
		 *  names in the current database and schema, that are visible
		 *  to the user that SQL Relay is logged in as.  Only returns
		 *  table type names that match wildcard "wild" if "wild" is
		 *  non-NULL.
		 *
		 *  Returns true on success and false on failure. */
		virtual bool	getTableTypeList(sqlrservercursor *cursor,
							const char *wild);

		/** Makes the database API call to fetch the list of column
		 *  names in "table", where "table" is in the current database
		 *  and schema.  Only returns column names that match wildcard
		 *  "wild" if "wild" is non-NULL.
		 *
		 *  Returns true on success and false on failure. */
		virtual bool	getColumnList(sqlrservercursor *cursor,
							const char *table,
							const char *wild);

		/** Makes the database API call to fetch the list of columns
		 *  that compose the primary key of "table".  Only returns
		 *  primary key column names that match wildcard "wild" if
		 *  "wild" is non-NULL.
		 *
		 *  Returns true on success and false on failure. */
		virtual bool	getPrimaryKeyList(sqlrservercursor *cursor,
							const char *table,
							const char *wild);

		/** Makes the database API call to fetch the indices and
		 *  indexed columns of "table", where "table" is in the current
		 *  database and schema.  Only returns primary key column names
		 *  that match wildcard "wild" if "wild" is non-NULL.
		 *
		 *  Returns true on success and false on failure. */
		virtual bool	getKeyAndIndexList(sqlrservercursor *cursor,
							const char *table,
							const char *wild);

		/** Makes the database API call to fetch the parameter names of
		 *  "proc", where "proc" is in the current database and schema,
		 *  and information about them, such as whether they are input,
		 *  output, or input-output variables.  Only returns parameter
		 *  names that match wildcard "wild" if "wild" is non-NULL.
		 *
		 *  Returns true on success and false on failure. */
		virtual bool	getProcedureParameterList(
						sqlrservercursor *cursor,
							const char *procedure,
							const char *wild);

		/** Makes the database API call to fetch the info about
		 *  datatype "type", where "type" is in the current database
		 *  and schema.  Only returns info for types that match
		 *  wildcard "wild" if "wild" is non-NULL.
		 *
		 *  Returns true on success and false on failure. */
		virtual bool	getTypeInfoList(sqlrservercursor *cursor,
							const char *type,
							const char *wild);

		/** Makes the database API call to fetch the list of stored
		 *  procedures in the current database and schema, and
		 *  information about them, such as the number of input and
		 *  output parameters, the numer of result sets that the
		 *  procdure may retrun, a description of the procedure, and
		 *  the procedure type (procedure or function).  Only returns
		 *  info for procedures that match wildcard "wild" if "wild" is
		 *  non-NULL.
		 *
		 *  Returns true on success and false on failure. */
		virtual bool	getProcedureList(sqlrservercursor *cursor,
							const char *wild);

		/** Returns a query that can be used to fetch the list of
		 *  database names.
		 *
		 *  If "wild" is true the the query also includes a where
		 *  clause that inlcudes a %s which can be used to substitute
		 *  in a wildcard value which can be used to filter the
		 *  results.
		 *
		 *  This implementation just returns getNoopQuery(), but it may
		 *  be overridden by a child class to return a database-specific
		 *  query. */
		virtual const char	*getDatabaseListQuery(bool wild);

		/** Returns a query that can be used to fetch the list of
		 *  schema names.
		 *
		 *  If "wild" is true the the query also includes a where
		 *  clause that inlcudes a %s which can be used to substitute
		 *  in a wildcard value which can be use  to filter the
		 *  results.
		 *
		 *  If "currentdbonly" is true then the query only returns
		 *  schemas on the current database.

		 *  This implementation just returns getNoopQuery(), but it may
		 *  be overridden by a child class to return a database-specific
		 *  query. */
		virtual const char	*getSchemaListQuery(
							bool wild,
							bool currentdbonly);

		/** Returns a query that can be used to fetch the list of
		 *  table names.
		 *
		 *  If "wild" is true the the query also includes a where
		 *  clause that inlcudes a %s which can be used to substitute
		 *  in a wildcard value which can be used  to filter the
		 *  results.
		 *
		 *  If "currentschemaonly" is true then the query only returns
		 *  schemas on the current database.
		 *
		 *  This implementation returns a query against the
		 *  information_schema, but it may be overridden by a child
		 *  class to return a database-specific query. */
		virtual const char	*getTableListQuery(
							bool wild,
							uint16_t objecttypes,
							bool currentschemaonly);

		/** Returns a query that can be used to fetch the list of
		 *  table types.
		 *
		 *  If "wild" is true the the query also includes a where
		 *  clause that inlcudes a %s which can be used to substitute
		 *  in a wildcard value which can be used to filter the
		 *  results.
		 *
		 *  If "currentschemaonly" is true then the query only returns
		 *  schemas on the current database.
		 *
		 *  This implementation just returns getNoopQuery(), but it may
		 *  be overridden by a child class to return a database-specific
		 *  query. */
		virtual const char	*getTableTypeListQuery(
							bool wild,
							bool currentschemaonly);

		/** Returns a query that can be used to fetch the list of
		 *  global temporary table names.
		 *
		 *  If "currentschemaonly" is true then the query only returns
		 *  schemas on the current database.
		 *
		 *  This implementation just returns getNoopQuery(), but it may
		 *  be overridden by a child class to return a database-specific
		 *  query. */
		virtual const char	*getGlobalTempTableListQuery(
							bool currentschemaonly);

		/** Returns a query that can be used to fetch the list of
		 *  column names from "table".
		 *
		 *  If "wild" is true the the query also includes a where
		 *  clause that inlcudes a %s which can be used to substitute
		 *  in a wildcard value which can be used to filter the
		 *  results.
		 *
		 *  This implementation just returns getNoopQuery(), but it may
		 *  be overridden by a child class to return a database-specific
		 *  query. */
		virtual const char	*getColumnListQuery(
							const char *table,
							bool wild);

		/** Returns a query that can be used to fetch the list of
		 *  columns that compose the primary key of "table".
		 *
		 *  If "wild" is true the the query also includes a where
		 *  clause that inlcudes a %s which can be used to substitute
		 *  in a wildcard value which can be used to filter the
		 *  results.
		 *
		 *  This implementation just returns getNoopQuery(), but it may
		 *  be overridden by a child class to return a database-specific
		 *  query. */
		virtual const char	*getPrimaryKeyListQuery(
							const char *table,
							bool wild);

		/** Returns a query that can be used to fetch the indices and
		 *  indexed columns  of "table", where "table" is in the
		 *  current database and schema.
		 *
		 *  If "wild" is true the the query also includes a where
		 *  clause that inlcudes a %s which can be used to substitute
		 *  in a wildcard value which can be used to filter the
		 *  results.
		 *
		 *  This implementation just returns getNoopQuery(), but it may
		 *  be overridden by a child class to return a database-specific
		 *  query. */
		virtual const char	*getKeyAndIndexListQuery(
							const char *table,
							bool wild);

		/** Returns a query that can be used to fetch the parameter
		 *  names of "proc", where "proc" is in the current database
		 *  and schema, and information about them, such as whether
		 *  they are input, output, or input-output variables.
		 *
		 *  If "wild" is true then the query also includes a where
		 *  clause that inlcudes a %s which can be used to substitute
		 *  in a wildcard value which can be used to filter the
		 *  results.
		 *
		 *  This implementation just returns getNoopQuery(), but it may
		 *  be overridden by a child class to return a database-specific
		 *  query. */
		virtual const char	*getProcedureParameterListQuery(
							const char *procedure,
							bool wild);

		/** Returns a query that can be used to fetch info about
		 *  datatype "type", where "type" is in the current database
		 *  and schema.
		 *
		 *  If "wild" is true the the query also includes a where
		 *  clause that inlcudes a %s which can be used to substitute
		 *  in a wildcard value which can be used to filter the
		 *  results.
		 *
		 *  If "currentschemaonly" is true then the query only returns
		 *  schemas on the current database.
		 *
		 *  This implementation just returns getNoopQuery(), but it may
		 *  be overridden by a child class to return a database-specific
		 *  query. */
		virtual const char	*getTypeInfoListQuery(
							const char *type,
							bool wild,
							bool currentschemaonly);

		/** Returns a query that can be used to fetch the list of
		 *  stored procedures in the current database and schema, and
		 *  information about them, such as the number of input and
		 *  output parameters, the numer of result sets that the
		 *  procdure may retrun, a description of the procedure, and
		 *  the procedure type (procedure or function).
		 *
		 *  If "wild" is true the the query also includes a where/
		 *  clause that inlcudes a %s which can be used to substitute
		 *  in a wildcard value which can be used to filter the
		 *  results.
		 *
		 *  If "currentschemaonly" is true then the query only returns
		 *  schemas on the current database.
		 *
		 *  This implementation just returns getNoopQuery(), but it may
		 *  be overridden by a child class to return a database-specific
		 *  query. */
		virtual const char	*getProcedureListQuery(
							bool wild,
							bool currentschemaonly);

		/** Returns true if "table" is a synonym, rather than an actual
		 *  table and false otherwise. */
		virtual bool	isSynonym(const char *table);

		/** Returns the query that can be used to determine if a table
		 *  name is a synonym rather than an actual table, that
		 *  includes a %s which can be used to substitute in a table
		 *  name.
		 *
		 *  This implementation just returns getNoopQuery(), but it may
		 *  be overridden by a child class to return a database-specific
		 *  query. */
		virtual const char	*isSynonymQuery();

		/** Allocates a cursor and assigns it an id of "id".
		 *
		 *  This cursor must be opened by calling open(cursor) before
		 *  it can be used and should be closed by calling close(cursor)
		 *  and deleted by calling deleteCursor(cursor) when you are
		 *  done using it.
		 *
		 *  Returns NULL if a new cursor couldn't be allocated. */
		virtual sqlrservercursor	*newCursor(uint16_t id)=0;

		/** Deletes "cursor" which was previously allocated by
		 *  newCursor(). */
		virtual void deleteCursor(sqlrservercursor *curs)=0;

		/** Returns a string representing the bind variable format used
		 *  by the database.  For example:
		 *
		 *  ?  - database uses a ? to represent a bind variable
		 *  @* - database uses a @ followed by any characters to
		 *       represent a bind variable
		 *  $1 - database uses a $ followed by a number to represent a
		 *       bind variable
		 *  :* - database uses a : followed by any characters to
		 *       represent a bind variable
		 *
		 *  Returns :* by default but may be overriden by a child
		 *  class. */
		virtual	const char	*getBindFormat();

		/** Returns the value that the database expects or returns in
		 *  the "null indicator" for a non-null bind value.
		 *
		 *  Returns 0 by default but may be overriden by a child
		 *  class. */
		virtual	int16_t		getNonNullBindValue();

		/** Returns the value that the database expects or returns in
		 *  the "null indicator" for a null bind value.
		 *
		 *  Retruns -1 by default but may be overriden by a child
		 *  class. */
		virtual	int16_t		getNullBindValue();

		/** Returns true if "isnull" matches the value that the database
		 *  expects or returns in the "null indicator" for a null bind
		 *  value. */
		virtual bool		getBindValueIsNull(int16_t isnull);

		/** Returns a string representing the format of the sequence
		 *  nextval command used in the database.  The format will
		 *  contain a %s in place of the sequence name.  For example:
		 *
		 *  (nextval for %s)
		 *  next value for %s
		 *  nextval('%s')
		 *  %s.nextval
		 *
		 *  Returns an empty string if the database does not support
		 *  sequences.
		 *
		 *  Returns %s.nextval by default but may be overriden by a
		 *  child class. */
		virtual const char	*getNextvalFormat();

		/** Returns the prefix that the database requires when
		 *  referencing a temporary table.  For example, SAP/Sybase
		 *  temp tables are prefixed with #.  This method just returns
		 *  "", but it may be overridden by a child class to return a
		 *  prefix. */
		virtual const char	*tempTablePrefix();

		/** Returns true if a temporary table must be truncated before
		 *  it may be dropped, and false otherwise.  This method
		 *  returns false, but it may be overridden by a child class to
		 *  return true. */
		virtual bool	tempTableTruncateBeforeDrop();

		/** Performs various clean-up tasks when a client session
		 *  ends. */
		virtual void	endSession();

		/** Returns a pointer to the connection-level error buffer. */
		char		*getErrorBuffer();

		/** Returns the size, in bytes, of the connection-level error
		 *  buffer. */
		uint32_t	getErrorBufferSize();

		/** Sets the number of bytes currently stored in the
		 *  connection-level error buffer to "errorsize". */
		void	setErrorSize(uint32_t errorsize);

		/** Returns the number of bytes currently stored in the
		 *  connection-level error buffer, as set by setErrorSize(). */
		uint32_t	getErrorSize();

		/** Sets the connection-level numeric error code to "errnum". */
		void	setErrorNumber(uint32_t errnum);

		/** Returns the connection-level numeric error code as set by
		 *  setErrorNumber(). */
		uint32_t	getErrorNumber();

		/** Sets a flag indicating whether the connection to the
		 *  database is up to "liveconnection". */
		void	setLiveConnection(bool liveconnection);

		/** Returns the flag indicating whether the connection to the
		 *  database is up, as set by setLiveConnection(). */
		bool	getLiveConnection();

		sqlrservercontroller	*cont;

	#include <sqlrelay/private/sqlrserverconnection.h>
};

class SQLRSERVER_DLLSPEC sqlrservercursor : public sqlrserverbase {
	public:

		/** Creates an instance of sqlrserverccursor and assigns it an
		 *  id of "id". */
		sqlrservercursor(sqlrserverconnection *conn, uint16_t id);

		/** Deletes this instance of sqlrservercursor. */
		virtual	~sqlrservercursor();

		/** Opens the cursor.
		 *  
		 *  Returns true on success and false on failure.
		 *
		 *  This method just returns true, but may be overridden by
		 *  a child class to perform module-specific tasks. */
		virtual	bool	open();

		/** Closes the cursor.
		 *  
		 *  Returns true on success and false on failure.
		 *
		 *  This method just returns true, but may be overridden by
		 *  a child class to perform module-specific tasks. */
		virtual	bool	close();

		/** Returns the type "query", of "size" bytes.  This method
		 *  does this by skipping any whitespace and comments, then
		 *  comparing the query to various patterns, however it may be
		 *  overridden by a child class to use a database-API-provided
		 *  method, or something else. */
		virtual sqlrquerytype_t	determineQueryType(const char *query,
								uint32_t size);

		/** Returns false. */
		virtual	bool	isCustomQuery();

		/** Prepares "query", of "size" bytes.
		 *
		 *  Returns true on success and false otherwise.
		 *
		 *  This method just returns true, but may be overridden by
		 *  a child class to actually prepare the query and return
		 *  true or false, as appropriate. */
		virtual	bool	prepareQuery(const char *query, uint32_t size);

		/** Returns true if "query", of "size" bytes, supports native
		 *  binds, and false if native binds are not supported by this
		 *  query.
		 *
		 *  This method just returns true, but may be overriden by
		 *  a child class to return true or false, as appropriate. */
		virtual	bool	supportsNativeBinds(const char *query,
							uint32_t size);

		/** Binds character value "value", of "valuesize" bytes, with
		 *  null indicator "isnull" to input bind variable "variable"
		 *  of "variablesize" bytes.
		 *
		 *  Returns true if the bind succeeded and false otherwise.
		 *
		 *  This method just returns true, but may be overridden by
		 *  a child class to actually bind the value and return
		 *  true or false, as appropriate. */
		virtual	bool	inputBind(const char *variable, 
						uint16_t variablesize,
						const char *value, 
						uint32_t valuesize,
						int16_t *isnull);

		/** Binds integer value "value" to input bind variable
		 *  "variable" of "variablesize" bytes.
		 *
		 *  Returns true if the bind succeeded and false otherwise.
		 *
		 *  This method just returns true, but may be overridden by
		 *  a child class to actually bind the value and return
		 *  true or false, as appropriate. */
		virtual	bool	inputBind(const char *variable, 
						uint16_t variablesize,
						int64_t *value);

		/** Binds double-precision floating point value "value", with
		 *  precision "precision" and scale "scale" to input bind
		 *  variable "variable" of "variablesize" bytes.
		 *
		 *  Returns true if the bind succeeded and false otherwise.
		 *
		 *  This method just returns true, but may be overridden by
		 *  a child class to actually bind the value and return
		 *  true or false, as appropriate. */
		virtual	bool	inputBind(const char *variable, 
						uint16_t variablesize,
						double *value,
						uint32_t precision,
						uint32_t scale);

		/** Converts the broken-down date/time specified by "year",
		 *  "month", "day", "hour", "minute", "second", "microsecond",
		 *  "tz", and "isnegative" to a date/time string and writes the
		 *  string to "buffer" of "buffersize" bytes.
		 *
		 *  The date/time is convered using the format specfied in the
		 *  fakeinputbindvariablesdateformat attribute of the instance
		 *  tag in the sqlrelay config file, or "YYYY-MM-DD HH24:MI:SS"
		 *  if no format was specified.
		 *
		 *  This method is used by inputBind() to convert a broken-down
		 *  date/time into a date/time string, if the database requires
		 *  this.  If the inputBind() method for date/times is
		 *  implemented such that it uses the broken-down date/time
		 *  directly and doesn't require it to be converted to a string,
		 *  then this method is not used.  May be overridden by a child
		 *  class if the inputBind() method for date/times requires a
		 *  different format if fakeinputbindvariablesdateformat isn't
		 *  specified. */
		virtual void	dateToString(char *buffer,
						uint16_t buffersize,
						int16_t year,
						int16_t month,
						int16_t day,
						int16_t hour,
						int16_t minute,
						int16_t second,
						int32_t microsecond,
						const char *tz,
						bool isnegative);

		/** Binds the broken-down date/time specified by "year",
		 *  "month", "day", "hour", "minute", "second", "microsecond",
		 *  "tz", and "isnegative" to input bind variable "variable" of
		 *  "variablesize" bytes.
		 *  
		 *  Returns true if the bind succeeded and false otherwise.
		 *
		 *  This method uses converts the broken-down date/time to a
		 *  string, stored in "buffer", then calls the inputBind()
		 *  method for binding strings, but may be overridden by a
		 *  child class to work differently, for example, to bind the
		 *  broken-down date/time components directly, if the database
		 *  API supports that. */
		virtual bool	inputBind(const char *variable,
						uint16_t variablesize,
						int64_t year,
						int16_t month,
						int16_t day,
						int16_t hour,
						int16_t minute,
						int16_t second,
						int32_t microsecond,
						const char *tz,
						bool isnegative,
						int16_t *isnull);

		/** Binds blob value "value", of "valuesize" bytes, with
		 *  null indicator "isnull" to input bind variable "variable"
		 *  of "variablesize" bytes.
		 *
		 *  Returns true if the bind succeeded and false otherwise.
		 *
		 *  This method just calls the inputBind() method for strings,
		 *  but may be overridden by a child class to work differently,
		 *  for example, to operate on "value" in chunks. */
		virtual	bool	inputBindBlob(const char *variable, 
						uint16_t variablesize,
						const char *value, 
						uint32_t valuesize,
						int16_t *isnull);

		/** Binds clob value "value", of "valuesize" bytes, with
		 *  null indicator "isnull" to input bind variable "variable"
		 *  of "variablesize" bytes.
		 *
		 *  Returns true if the bind succeeded and false otherwise.
		 *
		 *  This method just calls the inputBind() method for strings,
		 *  but may be overridden by a child class to work differently,
		 *  for example, to operate on "value" in chunks. */
		virtual	bool	inputBindClob(const char *variable, 
						uint16_t variablesize,
						const char *value, 
						uint32_t valuesize,
						int16_t *isnull);

		/** Binds character buffer "value", of "valuesize" bytes, and
		 *  null indicator "isnull" to output bind variable "variable"
		 *  of "variablesize" bytes.
		 *
		 *  Returns true if the bind succeeded and false otherwise.
		 *
		 *  This method just returns true, but may be overridden by
		 *  a child class to actually bind the value and return
		 *  true or false, as appropriate. */
		virtual	bool	outputBind(const char *variable, 
						uint16_t variablesize,
						char *value,
						uint32_t valuesize,
						int16_t *isnull);

		/** Binds integer buffer "value", and null indicator "isnull"
		 *  to output bind variable "variable" of "variablesize" bytes.
		 *
		 *  Returns true if the bind succeeded and false otherwise.
		 *
		 *  This method just returns true, but may be overridden by
		 *  a child class to actually bind the value and return
		 *  true or false, as appropriate. */
		virtual	bool	outputBind(const char *variable, 
						uint16_t variablesize,
						int64_t *value,
						int16_t *isnull);

		/** Binds double-precision floating point buffer "value",
		 *  integer buffers "precision" and "scale", and null indicator
		 *  "isnull" to output bind variable "variable" of
		 *  "variablesize" bytes.
		 *
		 *  Returns true if the bind succeeded and false otherwise.
		 *
		 *  This method just returns true, but may be overridden by
		 *  a child class to actually bind the value and return
		 *  true or false, as appropriate. */
		virtual	bool	outputBind(const char *variable, 
						uint16_t variablesize,
						double *value,
						uint32_t *precision,
						uint32_t *scale,
						int16_t *isnull);

		/** Binds the broken-down date/time buffers "year", "month",
		 *  "day", "hour", "minute", "second", "microsecond", "tz",
		 *  and "isnegative", and null indicator "isnull" to output
		 *  bind variable "variable" of "variablesize" bytes.
		 *  
		 *  A "buffer" of "buffersize" bytes is also provided in case
		 *  the date/time must be converted from a string.
		 *
		 *  Returns true if the bind succeeded and false otherwise.
		 *
		 *  This method just returns true, but may be overridden by
		 *  a child class to actually bind the value and return
		 *  true or false, as appropriate. */
		virtual bool	outputBind(const char *variable,
						uint16_t variablesize,
						int16_t *year,
						int16_t *month,
						int16_t *day,
						int16_t *hour,
						int16_t *minute,
						int16_t *second,
						int32_t *microsecond,
						const char **tz,
						bool *isnegative,
						int16_t *isnull);

		/** Binds a blob buffer and null indicator "isnull" to output
		 *  bind variable "variable" of "variablesize" bytes such that,
		 *  later, data can be fetched from the blob buffer using
		 *  getLobOutputBindLength(), getLobOutputBindSegment(), and
		 *  closeLobOutputBind().
		 *
		 *  Returns true if the bind succeeded and false otherwise.
		 *
		 *  This method just returns true, but may be overridden by
		 *  a child class to actually bind the value and return
		 *  true or false, as appropriate. */
		virtual	bool	outputBindBlob(const char *variable, 
						uint16_t variablesize,
						uint16_t index,
						int16_t *isnull);

		/** Binds a clob buffer and null indicator "isnull" to output
		 *  bind variable "variable" of "variablesize" bytes such that,
		 *  later, data can be fetched from the blob buffer using
		 *  getLobOutputBindLength(), getLobOutputBindSegment(), and
		 *  closeLobOutputBind().
		 *
		 *  Returns true if the bind succeeded and false otherwise.
		 *
		 *  This method just returns true, but may be overridden by
		 *  a child class to actually bind the value and return
		 *  true or false, as appropriate. */
		virtual	bool	outputBindClob(const char *variable, 
						uint16_t variablesize,
						uint16_t index,
						int16_t *isnull);

		/** Opens LOB output bind at position "index" (unless it is
		 *  already open) and sets "length" equal to its length, in
		 *  characters.
		 *
		 *  Returns true on success and false otherwise.  Will return
		 *  false if the output bind at position "index" is not a LOB.
		 *
		 *  For example...
		 *
		 *  if
		 *  getOutputBinds()[index].type==SQLRSERVERBINDVARTYPE_BLOB
		 *  or
		 *  getOutputBinds()[index].type==SQLRSERVERBINDVARTYPE_CLOB
		 *  then this method will return true. */
		virtual bool	getLobOutputBindLength(uint16_t index,
							uint64_t *length);

		/** Opens LOB output bind at position "index" (unless it is
		 *  already open) and attempts to fetch "charstoread"
		 *  characters from position "offset" into "buffer" of
		 *  "buffersize" bytes.  Populates "charsread" with the number
		 *  of characters that were actually read.
		 *
		 *  Returns true on success and false otherwise.  Will return
		 *  false if the output bind at position "index" is not a LOB.
		 *
		 *  For example...
		 *
		 *  if
		 *  getOutputBinds()[index].type==SQLRSERVERBINDVARTYPE_BLOB
		 *  or
		 *  getOutputBinds()[index].type==SQLRSERVERBINDVARTYPE_CLOB
		 *  then this method will return true. */
		virtual bool	getLobOutputBindSegment(uint16_t index,
							char *buffer,
							uint64_t buffersize,
							uint64_t offset,
							uint64_t charstoread,
							uint64_t *charsread);

		/** Closes LOB output bind at position "index".
		 *
		 *  For example...
		 *
		 *  if
		 *  getOutputBinds()[index].type==SQLRSERVERBINDVARTYPE_BLOB
		 *  or
		 *  getOutputBinds()[index].type==SQLRSERVERBINDVARTYPE_CLOB
		 *  and the LOB was opened by a call to
		 *  getLobOutputBindLength() or getLobOutputBindSegment()
		 *  then this method will close it. */
		virtual void	closeLobOutputBind(uint16_t index);

		/** Binds cursor "cursor" to output bind variable "variable"
		 *  of "variablesize" bytes, such that, later, a result set
		 *  can be fetched from it using fetchFromBindCursor().
		 *
		 *  Returns true if the bind succeeded and false otherwise.
		 *
		 *  This method just returns true, but may be overridden by
		 *  a child class to actually bind the value and return
		 *  true or false, as appropriate. */
		virtual	bool	outputBindCursor(const char *variable,
						uint16_t variablesize,
						sqlrservercursor *cursor);

		/** Binds character buffer "value", of "valuesize" bytes, and
		 *  null indicator "isnull" to input/output bind variable
		 *  "variable" of "variablesize" bytes.
		 *
		 *  Returns true if the bind succeeded and false otherwise.
		 *
		 *  This method just returns true, but may be overridden by
		 *  a child class to actually bind the value and return
		 *  true or false, as appropriate. */
		virtual	bool	inputOutputBind(const char *variable, 
						uint16_t variablesize,
						char *value,
						uint32_t valuesize,
						int16_t *isnull);

		/** Binds integer buffer "value" and null indicator "isnull"
		 *  to input/output bind variable "variable" of "variablesize"
		 *  bytes.
		 *
		 *  Returns true if the bind succeeded and false otherwise.
		 *
		 *  This method just returns true, but may be overridden by
		 *  a child class to actually bind the value and return
		 *  true or false, as appropriate. */
		virtual	bool	inputOutputBind(const char *variable, 
						uint16_t variablesize,
						int64_t *value,
						int16_t *isnull);

		/** Binds double-precision floating point buffer "value",
		 *  integer buffers "precision" and "scale", and null indicator
		 *  "isnull" to input/output bind variable "variable" of
		 *  "variablesize" bytes.
		 *
		 *  Returns true if the bind succeeded and false otherwise.
		 *
		 *  This method just returns true, but may be overridden by
		 *  a child class to actually bind the value and return
		 *  true or false, as appropriate. */
		virtual	bool	inputOutputBind(const char *variable, 
						uint16_t variablesize,
						double *value,
						uint32_t *precision,
						uint32_t *scale,
						int16_t *isnull);

		/** Binds the broken-down date/time buffers "year", "month",
		 *  "day", "hour", "minute", "second", "microsecond", "tz",
		 *  and "isnegative", and null indicator "isnull" to
		 *  input/output bind variable "variable" of "variablesize"
		 *  bytes.
		 *  
		 *  A "buffer" of "buffersize" bytes is also provided in case
		 *  the date/time must be converted from a string.
		 *
		 *  Returns true if the bind succeeded and false otherwise.
		 *
		 *  This method just returns true, but may be overridden by
		 *  a child class to actually bind the value and return
		 *  true or false, as appropriate. */
		virtual bool	inputOutputBind(const char *variable,
						uint16_t variablesize,
						int16_t *year,
						int16_t *month,
						int16_t *day,
						int16_t *hour,
						int16_t *minute,
						int16_t *second,
						int32_t *microsecond,
						const char **tz,
						bool *isnegative,
						int16_t *isnull);

		/** Binds a blob buffer and null indicator "isnull" to
		 *  input/output bind variable "variable" of "variablesize"
		 *  bytes such that, later, data can be fetched from the blob
		 *  buffer using getLobInputOutputBindLength(),
		 *  getLobInputOutputBindSegment(), and
		 *  closeLobInputOutputBind().
		 *
		 *  Returns true if the bind succeeded and false otherwise.
		 *
		 *  This method just returns true, but may be overridden by
		 *  a child class to actually bind the value and return
		 *  true or false, as appropriate. */
		virtual	bool	inputOutputBindBlob(const char *variable, 
						uint16_t variablesize,
						uint16_t index,
						int16_t *isnull);

		/** Binds a clob buffer and null indicator "isnull" to
		 *  input/output bind variable "variable" of "variablesize"
		 *  bytes such that, later, data can be fetched from the blob
		 *  buffer using getLobInputOutputBindLength(),
		 *  getLobInputOutputBindSegment(), and
		 *  closeLobInputOutputBind().
		 *
		 *  Returns true if the bind succeeded and false otherwise.
		 *
		 *  This method just returns true, but may be overridden by
		 *  a child class to actually bind the value and return
		 *  true or false, as appropriate. */
		virtual	bool	inputOutputBindClob(const char *variable, 
						uint16_t variablesize,
						uint16_t index,
						int16_t *isnull);

		/** Opens LOB input/output bind at position "index" (unless it
		 *  is already open) and sets "length" equal to its length, in
		 *  characters.
		 *
		 *  Returns true on success and false otherwise.  Will return
		 *  false if the output bind at position "index" is not a LOB.
		 *
		 *  For example...
		 *
		 *  if
		 *  getOutputBinds()[index].type==SQLRSERVERBINDVARTYPE_BLOB
		 *  or
		 *  getOutputBinds()[index].type==SQLRSERVERBINDVARTYPE_CLOB
		 *  then this method will return true. */
		virtual bool	getLobInputOutputBindLength(uint16_t index,
							uint64_t *length);

		/** Opens LOB input/output bind at position "index" (unless it
		 *  is already open) and attempts to fetch "charstoread"
		 *  characters from position "offset" into "buffer" of
		 *  "buffersize" bytes.  Populates "charsread" with the number
		 *  of characters that were actually read.
		 *
		 *  Returns true on success and false otherwise.  Will return
		 *  false if the output bind at position "index" is not a LOB.
		 *
		 *  For example...
		 *
		 *  if
		 *  getOutputBinds()[index].type==SQLRSERVERBINDVARTYPE_BLOB
		 *  or
		 *  getOutputBinds()[index].type==SQLRSERVERBINDVARTYPE_CLOB
		 *  then this method will return true. */
		virtual bool	getLobInputOutputBindSegment(uint16_t index,
							char *buffer,
							uint64_t buffersize,
							uint64_t offset,
							uint64_t charstoread,
							uint64_t *charsread);

		/** Closes LOB input/output bind at position "index".
		 *
		 *  For example...
		 *
		 *  if
		 *  getOutputBinds()[index].type==SQLRSERVERBINDVARTYPE_BLOB
		 *  or
		 *  getOutputBinds()[index].type==SQLRSERVERBINDVARTYPE_CLOB
		 *  and the LOB was opened by a call to
		 *  getLobOutputBindLength() or getLobOutputBindSegment()
		 *  then this method will close it. */
		virtual void	closeLobInputOutputBind(uint16_t index);

		/** Checks "query" of "size" bytes, to see if it is a create
		 *  temporary table query.  If it is, then the temporary table
		 *  referenced in the query is appended to the list of
		 *  temporary tables to drop when the client session ends.
		 *
		 *  May be overrdden by a child class, for example to
		 *  differentiate between temp tables and global temp tables,
		 *  if the database supports both. */
		virtual void	checkForTempTable(const char *query,
							uint32_t size);

		/** Returns the beginning of the query that the database uses
		 *  to truncate a table.  The table name to truncate must be
		 *  appended to this query.
		 *
		 *  This method just returns "delete from", but it may be
		 *  overridden by a child class to return a more appropriate
		 *  query for this database. */
		virtual	const char	*truncateTableQuery();

		/** Executes "query", of "size" bytes.
		 *
		 *  Returns true on success and false otherwise.
		 *
		 *  This method just returns true, but may be overridden by
		 *  a child class to actually prepare the query and return
		 *  true or false, as appropriate. */
		virtual	bool	executeQuery(const char *query, uint32_t size);

		/** Assumes that the current cursor is a bind cursor and
		 *  fetches from it.
		 *
		 *  Returns true on success and false otherwise. */
		virtual bool	fetchFromBindCursor();

		/** Advances to the next result set.
		 *
		 *  Returns true and sets "nextresultsetavailable" true if
		 *  another result set was available.
		 *
		 *  Returns true and sets "nextresultsetavailable" false if
		 *  another result set was not available.
		 *
		 *  Returns false if an error occured while checking for
		 *  another resulet set. */
		virtual	bool	nextResultSet(bool *nextresultsetavailable);

		/** Returns true if the query currently stored in the query
		 *  buffer is not a select and false if it is a select.
		 *
		 *  This method does this by skipping any whitespace and
		 *  comments, then comparing the query to various patterns,
		 *  however it may be overridden by a child class to use a
		 *  database-API-provided method, or something else. */
		virtual	bool	queryIsNotSelect();

		/** Returns true if the query currently stored in the query
		 *  buffer is a commit or rollback.
		 *
		 *  This method does this by skipping any whitespace and
		 *  comments, then comparing the query to various patterns,
		 *  however it may be overridden by a child class to use a
		 *  database-API-provided method, or something else. */
		virtual	bool	queryIsCommitOrRollback();

		/** Returns the error message and code by:
		 *
		 *  Copying at most "errorbuffersize" bytes from the
		 *  cursor-level error buffer into "errorbuffer".
		 *  Populating "errorsize" with the number of bytes in the
		 *  error buffer.
		 *  Populating "errorcode" with the cursor-level numeric
		 *  error code.
		 *  Populating "liveconnection" with true if the connection to
		 *  the database is still up.
		 *  Populating "liveconnection" with true if the connection to
		 *  the database is down. */
		virtual	void	getError(char *errorbuffer,
						uint32_t errorbuffersize,
						uint32_t *errorsize,
						int64_t *errorcode,
						bool *liveconnection);

		/** Some databases know the row count of a result set prior to
		 *  fetching any results.  Other databases do not, or do not
		 *  always know the row count.  For the current result set,
		 *  this method returns true in the first case and false in the
		 *  second.
		 *
		 *  Returns false by default but may be overridden by a child
		 *  class. */
		virtual bool	knowsRowCount();

		/** If the database knows the row count of a result set prior to
		 *  fetching any results (see knowsRowCount()) then this method
		 *  returns the number of rows in the result set.  For the
		 *  current result set, if the database does not know the row
		 *  count, then this method returns the number of rows that
		 *  have currently been fetched.
		 *
		 *  Returns 0 by default but may be overridden by a child
		 *  class. */
		virtual uint64_t	rowCount();

		/** Most databases know the affected row count, but some
		 *  databases (eg. firebird, sqlite, some versions of freetds)
		 *  do not.  For the current result set, this method returns
		 *  true in the first case and false in the second.
		 *
		 *  Returns true by default but may be overridden by a child
		 *  class. */
		virtual bool	knowsAffectedRows();

		/** Returns the number of affected rows for the current result
		 *  set, as set by the most recent call to setAffectedRows().
		 *
		 *  Returns 0 by default but may be overridden by a child
		 *  class. */
		virtual uint64_t	getAffectedRows();

		/** Returns the number of columns in the current result set.
		 *
		 *  Returns 0 if column info is not yet valid or if the query
		 *  has no result set (eg. if it was a DML or DDL query). */
		virtual uint32_t	colCount();

		/** Some database backends have predictable column types that
		 *  can be mapped to numeric ids.  If the client is aware of
		 *  these ids and if the protocol supports sending column types
		 *  as ids (eg. the sqlrclient protocol), then these numeric
		 *  ids can be sent to the client instead of sending column
		 *  type name strings.
		 *
		 *  Other database backends (eg. postgresql, router) don't have
		 *  predictable column types and so column type names must be
		 *  sent as strings.
		 *
		 *  This method returns COLUMN_TYPE_IDS in the first case and
		 *  COLUMN_TYPE_NAMES in the second case. */
		virtual uint16_t	columnTypeFormat();

		/** Returns the name of the column at position "col" in the
		 *  current result set.
		 *
		 *  Returns NULL if column info is not yet valid or if the
		 *  query has no result set (eg. if it was a DML or DDL
		 *  query). */
		virtual const char	*getColumnName(uint32_t col);

		/** Returns the size (number of bytes) of the column name at
		 *  position "col" in the current result set.
		 *
		 *  Returns 0 if column info is not yet valid or if the query
		 *  has no result set (eg. if it was a DML or DDL query). */
		virtual uint16_t	getColumnNameSize(uint32_t col);

		/** Returns the numeric type id of the column at position "col"
		 *  in the current result set.
		 *
		 *  Returns UNKNOWN_DATATYPE if column info is not yet valid,
		 *  if the query has no result set (eg. if it was a DML or DDL
		 *  query, or if columnTypeFormat() returns
		 *  COLUMN_TYPE_NAMES. */
		virtual uint16_t	getColumnType(uint32_t col);

		/** Returns the type name string of the column at position "col"
		 *  in the current result set.
		 *
		 *  Returns NULL if column info is not yet valid, if the query
		 *  has no result set (eg. if it was a DML or DDL query, or if
		 *  columnTypeFormat() return  COLUMN_TYPE_IDS. */
		virtual const char	*getColumnTypeName(uint32_t col);

		/** Returns the size (number of bytes) of the type name string
		 *  of the column at position "col" in the current result set.
		 *
		 *  Returns 0 if column info is not yet valid, if the query has
		 *  no result set (eg. if it was a DML or DDL query, or if
		 *  columnTypeFormat() returns COLUMN_TYPE_IDS. */
		virtual uint16_t	getColumnTypeNameSize(uint32_t col);

		/** Returns the size of the column at position "col" in the
		 *  current result set.
		 *
		 *  Returns 0 if column info is not yet valid or if the query
		 *  has no result set (eg. if it was a DML or DDL query). */
		virtual uint32_t	getColumnSize(uint32_t col);

		/** Returns the precision of the column at position "col" in
		 *  the current result set.
		 *
		 *  Returns 0 if column info is not yet valid or if the query
		 *  has no result set (eg. if it was a DML or DDL query). */
		virtual uint32_t	getColumnPrecision(uint32_t col);

		/** Returns the scale of the column at position "col" in
		 *  the current result set.
		 *
		 *  Returns 0 if column info is not yet valid or if the query
		 *  has no result set (eg. if it was a DML or DDL query). */
		virtual uint32_t	getColumnScale(uint32_t col);

		/** Returns non-zero if the column at position "col" in the
		 *  current result set is nullable and 0if it is not nullable.
		 *
		 *  Returns 0 if column info is not yet valid or if the query
		 *  has no result set (eg. if it was a DML or DDL query). */
		virtual uint16_t	getColumnIsNullable(uint32_t col);

		/** Returns non-zero if the column at position "col" in the
		 *  current result set is the primary key and 0 if it is not
		 *  the primary key.
		 *
		 *  Returns 0 if column info is not yet valid or if the query
		 *  has no result set (eg. if it was a DML or DDL query). */
		virtual uint16_t	getColumnIsPrimaryKey(uint32_t col);

		/** Returns non-zero if the column at position "col" in the
		 *  current result set is a unique column and 0 if it is not a
		 *  unique column.
		 *
		 *  Returns 0 if column info is not yet valid or if the query
		 *  has no result set (eg. if it was a DML or DDL query). */
		virtual uint16_t	getColumnIsUnique(uint32_t col);

		/** Returns non-zero if the column at position "col" in the
		 *  current result set is part of a key and 0 if it is not part
		 *  of a key.
		 *
		 *  Returns 0 if column info is not yet valid or if the query
		 *  has no result set (eg. if it was a DML or DDL query). */
		virtual uint16_t	getColumnIsPartOfKey(uint32_t col);

		/** Returns non-zero if the column at position "col" in the
		 *  current result set is unsigned and 0 if it is signed.
		 *
		 *  Returns 0 if column info is not yet valid or if the query
		 *  has no result set (eg. if it was a DML or DDL query). */
		virtual uint16_t	getColumnIsUnsigned(uint32_t col);

		/** Returns non-zero if the column at position "col" in the
		 *  current result set is zero-filled and 0 if it is not
		 *  zero-filled.
		 *
		 *  Returns 0 if column info is not yet valid or if the query
		 *  has no result set (eg. if it was a DML or DDL query). */
		virtual uint16_t	getColumnIsZeroFilled(uint32_t col);

		/** Returns non-zero if the column at position "col" in the
		 *  current result set is binary and 0 if it is not binary.
		 *
		 *  Returns 0 if column info is not yet valid or if the query
		 *  has no result set (eg. if it was a DML or DDL query). */
		virtual uint16_t	getColumnIsBinary(uint32_t col);

		/** Returns non-zero if the column at position "col" in the
		 *  current result set is an auto-increment column and 0 if it
		 *  is not an auto-increment column.
		 *
		 *  Returns 0 if column info is not yet valid or if the query
		 *  has no result set (eg. if it was a DML or DDL query). */
		virtual uint16_t	getColumnIsAutoIncrement(uint32_t col);

		/** Returns the name of the table that the column at position
		 *  "col" in the current result set is from.
		 *
		 *  Returns NULL if column info is not yet valid, if the query
		 *  has no result set (eg. if it was a DML or DDL query), or if
		 *  the database backend doesn't know what table the column
		 *  came from. */
		virtual const char	*getColumnTable(uint32_t col);

		/** Returns the size (number of bytes) of the name of the table
		 *  that the column at position "col" in the current result set
		 *  is from.
		 *
		 *  Returns NULL if column info is not yet valid, if the query
		 *  has no result set (eg. if it was a DML or DDL query), or if
		 *  the database backend doesn't know what table the column
		 *  came from. */
		virtual uint16_t	getColumnTableSize(uint32_t col);

		/** Returns true if the dateddmm attribute of the instance tag
		 *  in the sqlrelay config file should be ignored for "data" of
		 *  "size" bytes.
		 *  
		 *  This method returns false, but may be overridden by a child
		 *  class to return true under very specific circumstances. */
		virtual bool	ignoreDateDdMmParameter(const char *data,
							uint32_t size);

		/** Returns true if the current result set has no rows to
		 *  return and false if there are rows to return. */
		virtual	bool	noRowsToReturn();

		/** Skips the next row of the current result set.  If the
		 *  database supports skipping without fetching, then this is
		 *  more efficient than skipping a row by just fetching it.
		 *  Sets "error" to true if an error occurred and to false if
		 *  no error occurred.
		 *
		 *  Returns true if a row was fetched and false if no row was
		 *  fetched, either because an error occurred or because all
		 *  rows have already been fetched. */
		virtual	bool	skipRow(bool *error);

		/** Fetches the next row of the current result set.  Sets
		 *  "error" to true if an error occurred and to false if no
		 *  error occurred.
		 *
		 *  Returns true if a row was fetched and false if no row was
		 *  fetched, either because an error occurred or because all
		 *  rows have already been fetched. */
		virtual	bool	fetchRow(bool *error);

		/** Sets internal flags and/or counters related to moving to
		 *  the next row of the current result set.  Must be called
		 *  after fetchRow().  Note that this method is kludgy and may
		 *  be removed in the future. */
		virtual	void	nextRow();

		/** Fetches information about the field in column "col" of the
		 *  current row, of the current result set.  Sets "field" to
		 *  the value of the field, "fieldsize" to the number of bytes
		 *  in the value, "lob" true if the field is a LOB or false
		 *  otherwise, and "null" true if the field is null or false
		 *  otherwise.
		 *
		 *  Returns true on success and false if an error occurred. */
		virtual void	getField(uint32_t col,
						const char **field,
						uint64_t *fieldsize,
						bool *lob,
						bool *null);

		/** If the field in column "col" of the current row, of the
		 *  current result set is a LOB, then this method sets "length"
		 *  to the number of characters in the field.
		 *
		 *  Returns true on success and false if an error occurred.
		 *
		 *  Note that "length" is set to the number of characters
		 *  rather than bytes.  In the case of a binary LOB, the number
		 *  of characters and bytes will be the same, but in the case
		 *  of a text LOB, they will be different if the LOB uses a
		 *  variable width encoding such as UTF-8 or a fixed width
		 *  encoding of more than 8-bits, such as UCS2. */
		virtual bool	getLobFieldLength(uint32_t col,
						uint64_t *length);

		/** If the field in column "col" of the current row, of the
		 *  current result set is a LOB, then this method attempts to
		 *  fetch "charstoread" characters from character position
		 *  "offset" into "buffer" of size "buffer".  The actual number
		 *  of characters that were read is returned in "charsread".
		 *  "charsread" may be less than "charstoread" if "charsread"
		 *  characters won't fit in "buffer" or if the end of the LOB
		 *  was reached prior to fetching "charstoread".  0 will be
		 *  returned in "charsread" if an attempt is made to read past
		 *  the end of the LOB.
		 *
		 *  Note that "offset", "charstoread", and "charsread" all
		 *  refer to numbers of characters, while "buffersize" refers
		 *  to numbers of bytes.
		 *
		 *  Returns true on success and false if an error occurred.
		 *
		 *  Note that attempting to read past the end of the LOB is not
		 *  considered to be an error.*/
		virtual bool	getLobFieldSegment(uint32_t col,
						char *buffer,
						uint64_t buffersize,
						uint64_t offset,
						uint64_t charstoread,
						uint64_t *charsread);

		/** If the field in column "col" of the current row, of the
		 *  current result is a LOB then this method closes the LOB. */
		virtual void	closeLobField(uint32_t col);

		/** Closes the current result set. */
		virtual	void	closeResultSet();

		/** Encodes "datasize" bytes of blob data "data" as characters
		 *  such that they may included in a text query.
		 *
		 *  This method encodes the blob following the SQL standard:
		 *  X'...' where ... is the lob data and each byte of lob data
		 *  is converted to two hex characters.
		 *  eg: hello -> X'68656C6C6F'
		 *
		 *  However, it may be overridden by a child class to encode
		 *  blobs differently, as required by the database. */
		virtual void	encodeBlob(stringbuffer *buffer,
						const char *data,
						uint32_t datasize);

		/** Returns true if column info such as name, size, type,
		 *  precision, scale, etc. is valid after a query has been
		 *  prepared and false if column info is only valid after a
		 *  query has been executed. */
		virtual bool	columnInfoIsValidAfterPrepare();

		/** Returns the id of this cursor. */
		uint16_t	getId();

		/** Returns the memory pool used to store bind variable names
		 *  and values. */
		memorypool	*getBindPool();

		/** Returns the memory pool used to map bind variable names
		 *  when the value of the attribute translatebindvariables of
		 *  the instance tag in the config file is set to "yes". */
		memorypool	*getBindMappingsPool();

		/** Returns the dictionary of bind variable name mappings.  The
		 *  keys are the old (original) bind variable names and the
		 *  values are the new (translated) bind variable names.
		 *
		 *  The dictionary is populated by
		 *  sqlrservercontroller::prepareQuery() and remains populated
		 *  with the same key-value pairs until the next call to
		 *  prepareQuery().  If it is empty then either bind variable
		 *  translation is disabled or the most recently prepared query
		 *  contained no bind variables. */
		dictionary<char *, char *>	*getBindMappings();

		/** Sets the number of input binds in "cursor" to
		 *  "inbindcount". */
		void	setInputBindCount(uint16_t inbindcount);

		/** Returns the number of input binds in "cursor", as set by
		 *  setInputBindCount(). */
		uint16_t	getInputBindCount();

		/** Returns the array of input binds in "cursor".  The total
		 *  number of bind variables in the array is equal to the value
		 *  of the maxbindcount attribute of the instance tag in the
		 *  config file.  However, only the first getInputBindCount()
		 *  bind variables are currently valid.  The state of the rest
		 *  are undefined. */
		sqlrserverbindvar	*getInputBinds();

		/** Sets the number of valid output binds to "outbindcount". */
		void	setOutputBindCount(uint16_t outbindcount);

		/** Returns the number of valid output binds, as set by
		 *  setOutputBindCount(). */
		uint16_t	getOutputBindCount();

		/** Returns the array of output binds in "cursor".  The total
		 *  number of bind variables in the array is equal to the value
		 *  of the maxbindcount attribute of the instance tag in the
		 *  config file.  However, only the first getOutputBindCount()
		 *  bind variables are currently valid.  The state of the rest
		 *  are undefined. */
		sqlrserverbindvar	*getOutputBinds();

		/** Sets the number of valid input-output binds to
		 *  "inoutbindcount". */
		void	setInputOutputBindCount(uint16_t inoutbindcount);

		/** Returns the number of valid input-output binds as set by
		 *  setInputOutputBindCount(). */
		uint16_t	getInputOutputBindCount();

		/** Returns the array of input-output binds.  The total number
		 *  of bind variables in the array is equal to the value of the
		 *  maxbindcount attribute of the instance tag in the config
		 *  file.  However, only the first getInputOutputBindCount()
		 *  bind variables are currently valid.  The state of the rest
		 *  are undefined. */
		sqlrserverbindvar	*getInputOutputBinds();

		/** Immediately closes the result set. */
		void	abort();

		/** Returns a pointer to the query buffer. */
		char	*getQueryBuffer();

		/** Sets the size, in bytes, of the query that is currently
		 *  present in the query buffer to "querysize". */
		void	setQuerySize(uint32_t querysize);

		/** Returns the size, in bytes, of the query that is currently
		 *  present in the query buffer, as set by setQuerySize(). */
		uint32_t	getQuerySize();

		/** Sets the status of the current query to "status". */
		void	setQueryStatus(sqlrquerystatus_t status);

		/** Returns the status of the current query as set by
		 *  setQueryStatus(). */
		sqlrquerystatus_t	getQueryStatus();

		/** Sets the tree representing the current query to "tree". */
		void	setQueryTree(xmldom *tree);

		/** Returns the tree representing the current query as set by
		 *  setQueryTree(), or NULL if no tree has been set since
		 *  initialization or since the most recent call to
		 *  clearQueryTree(). */
		xmldom	*getQueryTree();

		/** Sets the tree representing the current query to NULL. */
		void	clearQueryTree();

		/** Returns the translated query buffer. */
		stringbuffer	*getTranslatedQueryBuffer();

		/** Returns the query currently stored in the translated quer
		 *  buffer. */
		const char	*getTranslatedQuery();

		/** Sets the command-start time.  That is, the time that the
		 *  SQL command (eg. begin a transaction, run a query, commit,
		 *  etc.) was received by SQL Relay, to "sec" and "usec". */
		void	setCommandStart(uint64_t sec, uint64_t usec);

		/** Returns the seconds component of the command-start time as
		 *  set by setCommandStart(). */
		uint64_t	getCommandStartSec();

		/** Returns the milliseconds component of the command start-time
		 *   as set by setCommandStart(). */
		uint64_t	getCommandStartUSec();

		/** Sets the command-end time.  That is, the time that the SQL
		 *  command (eg. begin a transaction, run a query, commit,
		 *  etc.) was completed, to "sec" and "usec". */
		void	setCommandEnd(uint64_t sec, uint64_t usec);

		/** Returns the seconds component of the command-end time as
		 *  set by setCommandEnd(). */
		uint64_t	getCommandEndSec();

		/** Returns the milliseconds component of the command-end time
		 *  as set by setCommandEnd(). */
		uint64_t	getCommandEndUSec();

		/** Sets the query-start time.  That is, the time that the
		 *  query was executed, to "sec" and "usec". */
		void	setQueryStart(uint64_t sec, uint64_t usec);

		/** Returns the seconds component of the query-start time
		 *  as set by setQueryStart(). */
		uint64_t	getQueryStartSec();

		/** Returns the milliseconds component of the query-start time
		 *  as set by setQueryStart(). */
		uint64_t	getQueryStartUSec();

		/** Sets the query-end time.  That is, the time that query
		 *  execution was complated, to "sec" and "usec". */
		void	setQueryEnd(uint64_t sec, uint64_t usec);

		/** Returns the seconds component of the query-end time
		 *  as set by setQueryEnd(). */
		uint64_t	getQueryEndSec();

		/** Returns the milliseconds component of the query-end time
		 *  as set by setQueryEnd(). */
		uint64_t	getQueryEndUSec();

		/** Sets the fetch-start time.  That is, the time that fetching
		 *  of the result set started, to "sec" and "usec". */
		void	setFetchStart(uint64_t sec, uint64_t usec);

		/** Returns the seconds component of the fetch-start time
		 *  as set by setFetchStart(). */
		uint64_t	getFetchStartSec();

		/** Returns the milliseconds component of the fetch-start time
		 *  as set by setFetchStart(). */
		uint64_t	getFetchStartUSec();

		/** Sets the fetch-end time.  That is, the time that fetching
		 *  of the result set was completed, to "sec" and "usec". */
		void	setFetchEnd(uint64_t sec, uint64_t usec);

		/** Returns the seconds component of the fetch-end time
		 *  as set by setFetchEnd(). */
		uint64_t	getFetchEndSec();

		/** Returns the milliseconds component of the fetch-end time
		 *  as set by setFetchEnd(). */
		uint64_t	getFetchEndUSec();

		/** Resets the fetch-time.  That is the number of milliseconds
		 *  that elapsed during the fetch of the result set, to 0. */
		void	resetFetchTime();

		/** Adds the number of milliseconds between the fetch-start
		 *  time and fetch-end time to the fetch-time.  Should be
		 *  called after setFetchEnd() or unexpected results may
		 *  occur. */
		void	tallyFetchTime();

		/** Returns the fetch-time, in milliseconds. */
		uint64_t	getFetchUSec();

		/** Sets the state of the cursor to "state". */
		void	setState(sqlrcursorstate_t state);

		/** Returns the state of the as set by the most recent
		 *  call to setState(). */
		sqlrcursorstate_t	getState();

		/** Sets the custom query cursor to "cur".
		 *  
		 *  If this query should be handled by a custom sqlrquerycursor
		 *  rather than the standard sqlrservercursor implemented by
		 *  this database module, then set that cursor using this
		 *  method. */
		void	setCustomQueryCursor(sqlrquerycursor *cur);

		/** Returns the custom query cursor as set by a previous call
		 *  to setCustomQueryCursor() or NULL of none has been set or
		 *  if clearCustomQueryCursor() has been called. */
		sqlrquerycursor	*getCustomQueryCursor();

		/** Clears the custom query cursor. */
		void	clearCustomQueryCursor();

		/** Increments the total number of rows fetched. */
		void	incrementTotalRowsFetched();

		/** Returns the total number of rows fetched. */
		uint64_t	getTotalRowsFetched();

		/** Sets the total number of rows fetched to 0. */
		void	clearTotalRowsFetched();

		/** Sets whether or not the current row was reformatted. */
		void	setCurrentRowReformatted(bool crr);

		/** Returns whether or not the current row was reformatted,
		 *  as set by the previous call to
		 *  setCurrentRowReformatted(). */
		bool	getCurrentRowReformatted();

		/** Returns a pointer to the cursor-level error buffer. */
		char	*getErrorBuffer();

		/** Returns the size, in bytes, of the cursor-level error
		 *  buffer. */
		uint32_t	getErrorBufferSize();

		/** Sets the number of bytes currently stored in the
		 *  cursor-level error buffer to "errorsize". */
		void	setErrorSize(uint32_t errorsize);

		/** Returns the number of bytes currently stored in the
		 *  cursor-level error buffer, as set by setErrorSize(). */
		uint32_t	getErrorSize();

		/** Sets the cursor-level numeric error code to "errnum". */
		void	setErrorNumber(uint32_t errnum);

		/** Returns the cursor-level numeric error code as set by
		 *  setErrorNumber(). */
		uint32_t	getErrorNumber();

		/** Sets a flag indicating whether the connection to the
		 *  database is up to "liveconnection". */
		void	setLiveConnection(bool liveconnection);

		/** Returns the flag indicating whether the connection to the
		 *  database is up, as set by setLiveConnection(). */
		bool	getLiveConnection();

		/** Sets the regular expression pattern used to identify a
		 *  create-temp-table clause to "createtemp".
		 *
		 *  Defaults to "(create|CREATE|declare|DECLARE)[ 	\\r\\n]+((global|GLOBAL|local|LOCAL)?[ 	\\r\\n]+)?(temp|TEMP|temporary|TEMPORARY)?[ 	\\r\\n]+(table|TABLE)[ 	\\r\\n]+" */
		void	setCreateTempTablePattern(const char *createtemp);

		/** Skips past any leading whitespace and comments and then
		 *  past any create-temp-table clause as set by
		 *  setCreateTempTablePattern() in "query" and returns a
		 *  pointer to the first character past the create-temp-table
		 *  clause, or NULL if no create-temp-table pattern was
		 *  found. */
		const char	*skipCreateTempTableClause(const char *query);

		/** Sets whether column info is currently valid to "valid". */
		void	setColumnInfoIsValid(bool valid);

		/** Returns whether column info is currently valid, as set by
		 *  the most recent call to setColumnInfoIsValid(). */
		bool	getColumnInfoIsValid();

		/** Sets whether the query has been pre-processed to
		 *  "preprocessed". */
		void	setQueryHasBeenPreProcessed(bool preprocessed);

		/** Returns whether the query has been pre-processed, as set by
		 *  the most recent call to setQueryHasBeenPreProcessed(). */
		bool	getQueryHasBeenPreProcessed();

		/** Sets whether the query has been prepared to "prepared". */
		void	setQueryHasBeenPrepared(bool prepared);

		/** Returns whether the query has been prepared, as set by
		 *  the most recent call to setQueryHasBeenPrepared(). */
		bool	getQueryHasBeenPrepared();

		/** Sets whether the query has been executed to "executed". */
		void	setQueryHasBeenExecuted(bool executed);

		/** Returns whether the query has been executed, as set by
		 *  the most recent call to setQueryHasBeenExecuted(). */
		bool	getQueryHasBeenExecuted();

		/** Sets whether the query needs to be intercepted to
		 *  "intercept". */
		void	setQueryNeedsIntercept(bool intercept);

		/** Returns whether the query needs to be intercepted, as set
		 *  by the most recent call to setQueryNeedsIntercept(). */
		bool	getQueryNeedsIntercept();

		/** Sets whether the query was intercepted to "intercepted". */
		void	setQueryWasIntercepted(bool intercepted);

		/** Returns whether the query was intercepted, as set by the
		 *  most recent call to setQueryWasIntercepted(). */
		bool	getQueryWasIntercepted();

		/** Sets whether binds were faked to "faked". */
		void	setBindsWereFaked(bool faked);

		/** Returns whether binds were faked, as set by the most recent
		 *  call to setBindsWereFaked(). */
		bool	getBindsWereFaked();

		/** Sets whether to fake input binds for the current query of
		 *  "cursor", by rewriting the query, to "fake".
		 *
		 *  Note that the behavior of prepareQuery() is as follows:
		 *
		 *    * It initially configures whether input binds should be
		 *      faked or not, based on the value of the
		 *      "fakeinputbindvariables" attribute of the instance tag,
		 *      in the config file.
		 *    * It then processes filters, directives, translations,
		 *      and before-triggers, which can call this method to
		 *      override that.
		 *    * It then checks to see if the query supports native
		 *      binds and sets input binds to be faked if native binds
		 *      are not supported with this query.
		 *
		 *  So, it is possible for this method to hae no effect if:
		 *
		 *    * It is called from a module other than a filter,
		 *      directive, translation, or before-trigger.
		 *    * It sets binds to not be faked, but the query itself
		 *      doesn't support native binds.
		 */
		void	setFakeInputBindsForThisQuery(bool fake);

		/** Returns whether or not input binds will be faked for the
		 *  current query.  See setFakeInputBindsForThisQuery(). */
		bool	getFakeInputBindsForThisQuery();

		/** Sets the type of the current query to "querytype". */
		void	setQueryType(sqlrquerytype_t querytype);

		/** Returns the type of the current query as set by the most
		 *  recent call to setQueryType(). */
		sqlrquerytype_t	getQueryType();

		/** Returns the buffer that the query was written to if input
		 *  binds were faked. */
		stringbuffer	*getQueryWithFakeInputBindsBuffer();

		/** Returns the column name, currently stored in the column
		 *  name buffer for column "col". */
		const char	*getColumnNameFromBuffer(uint32_t col);

		/** Returns the size (in bytes) of the column name,  currently
		 *  stored in the column name buffer for column "col". */
		uint16_t	getColumnNameSizeFromBuffer(uint32_t col);

		/** Returns the column type, currently stored in the column
		 *  type buffer for column "col". */
		uint16_t	getColumnTypeFromBuffer(uint32_t col);

		/** Returns the column type name, currently stored in the
		 *  column type name buffer for column "col". */
		const char	*getColumnTypeNameFromBuffer(uint32_t col);

		/** Returns the size (in bytes) of the column type name,
		 *  currently stored in the column type name buffer for column
		 *  "col". */
		uint16_t	getColumnTypeNameSizeFromBuffer(uint32_t col);

		/** Returns the size (in bytes) of the column, currently stored
		 *  in the column size buffer for column "col". */
		uint32_t	getColumnSizeFromBuffer(uint32_t col);

		/** Returns the precision of the column, currently stored
		 *  in the column precision buffer for column "col". */
		uint32_t	getColumnPrecisionFromBuffer(uint32_t col);

		/** Returns the scale of the column, currently stored
		 *  in the column scale buffer for column "col". */
		uint32_t	getColumnScaleFromBuffer(uint32_t col);

		/** Returns the is-nullable flag of the column, currently
		 *  stored in the is-nullable buffer for column "col". */
		uint16_t	getColumnIsNullableFromBuffer(uint32_t col);

		/** Returns the primary-key flag of the column, currently
		 *  stored in the primary-key buffer for column "col". */
		uint16_t	getColumnIsPrimaryKeyFromBuffer(uint32_t col);

		/** Returns the is-unique flag of the column, currently
		 *  stored in the is-unique buffer for column "col". */
		uint16_t	getColumnIsUniqueFromBuffer(uint32_t col);

		/** Returns the is-part-of-key flag of the column, currently
		 *  stored in the is-part-of-key buffer for column "col". */
		uint16_t	getColumnIsPartOfKeyFromBuffer(uint32_t col);

		/** Returns the is-unsigned flag of the column, currently
		 *  stored in the is-unsigned buffer for column "col". */
		uint16_t	getColumnIsUnsignedFromBuffer(uint32_t col);

		/** Returns the is-zero-filled flag of the column, currently
		 *  stored in the is-zero-filled buffer for column "col". */
		uint16_t	getColumnIsZeroFilledFromBuffer(uint32_t col);

		/** Returns the is-binary flag of the column, currently
		 *  stored in the is-binary buffer for column "col". */
		uint16_t	getColumnIsBinaryFromBuffer(uint32_t col);

		/** Returns the is-auto-increment flag of the column, currently
		 *  stored in the is-auto-increment buffer for column "col". */
		uint16_t	getColumnIsAutoIncrementFromBuffer(
							uint32_t col);

		/** Returns the column table, currently stored in the
		 *  column table buffer for column "col". */
		const char	*getColumnTableFromBuffer(uint32_t col);

		/** Returns the size (in bytes) of the column table, currently
		 *  stored in the column table buffer for column "col". */
		uint16_t	getColumnTableSizeFromBuffer(uint32_t col);

		/** Allocates pointers to the preallocated field buffers for
		 *  "colcount" columns. */
		void	allocateFieldPointers(uint32_t colcount);

		/** Returns the field buffer pointers allocated by the most
		 *  recent call to allocateFieldPointers(), or NULLs if
		 *  allocateFieldPointers() was never called, or if
		 *  deallocateFieldPointers() was called. */
		void	getFieldPointers(const char ***fieldnames,
					const char ***fields,
					uint64_t **fieldsizes,
					bool **lob,
					bool **null);

		/** Deallocates any field buffer pointers previously allocated
		 *  by a call to allocateFieldPointers(). */
		void	deallocateFieldPointers();

		/** Sets the query timeout to "querytimeout". */
		void	setQueryTimeout(uint64_t querytimeout);

		/** Returns the query timeout as set by the most recent call
		 *  to setQueryTimeout(). */
		uint64_t	getQueryTimeout();

		/** Sets the execute-direct flag to "executedirect". */
		void	setExecuteDirect(bool executedirect);

		/** Returns the execute-direct flag as set by the most recent
		 *  call to setExecuteDirect(). */
		bool	getExecuteDirect();

		/** Sets the execute-rpc flag to "executerpc". */
		void	setExecuteRpc(bool executerpc);

		/** Returns the execute-rpc flag as set by the most recent
		 *  call to setExecuteRpc(). */
		bool	getExecuteRpc();

		/** Sets the number of rows to fetch at once to
		 *  "fetchatonce". */
		void	setFetchAtOnce(uint32_t fetchatonce);

		/** Returns the number of rows that will be fetched at once. */
		uint32_t	getFetchAtOnce();

		/** Sets whether or not the result set header has been handled
		 *  to "resultsetheaderhasbeenhandled". */
		void	setResultSetHeaderHasBeenHandled(
					bool resultsetheaderhasbeenhandled);

		/** Returns whether or not the result set header has been
		 *  handled as set by the most recent call to
		 *  setResultSetHeaderHasBeenHandled(). */
		bool	getResultSetHeaderHasBeenHandled();

		/** Sets whether the current query is "suppressed" or not.
		 *  Currently may be called by a before-trigger to suppresses
		 *  execution of the query by
		 *  sqlrservercontroller::executeQuery(). */
		void	setQuerySuppressed(bool querysuppressed);

		/** Returns whether the current query is "suppressed" or not
		 *  as set by a call to setQuerySuppressed(). */
		bool	getQuerySuppressed();

		sqlrserverconnection	*conn;

	#include <sqlrelay/private/sqlrservercursor.h>
};

enum clientsessionexitstatus_t {
	CLIENTSESSIONEXITSTATUS_ERROR=0,
	CLIENTSESSIONEXITSTATUS_CLOSED_CONNECTION,
	CLIENTSESSIONEXITSTATUS_ENDED_SESSION,
	CLIENTSESSIONEXITSTATUS_SUSPENDED_SESSION
};

#include <sqlrelay/private/sqlrservermodules.h>

class SQLRSERVER_DLLSPEC sqlrservermodule : public sqlrserverbase {
	public:
		/** Creates an instance of sqlrservermodule, configured with
		 *  parameters "parameters".
		 *
		 *  This implementation handles the following parameters
		 *  which are generic to all modules:
		 *
		 *  * debug - yes/no, whether or not to enabled debug
		 *
		 *  However, it may be overridden by a child class to perform
		 *  additional initialization tasks and handle additional
		 *  parameters. */
		sqlrservermodule(sqlrservercontroller *cont,
					domnode *parameters);

		/** Deletes this instance of sqlrservermodule. */
		virtual	~sqlrservermodule();

		/** Returns true or false depending on whether the enabled
		 *  parameter was set to yes or no. */
		bool	getEnabled();

		/** Called by the sqlrservercontroller at the end of a
		 *  transaction.
		 *
		 *  This implementation just returns, but may be overridden by
		 *  a child class to do additional things at transaction-end. */
		virtual void	endTransaction(bool commit);

		/** Called by the sqlrservercontroller at the end of a client
		 *  session.
		 *
		 *  This implementation just returns, but may be overridden by
		 *  a child class to do additional things at transaction-end. */
		virtual void	endSession();

	protected:
		/** Returns the top-level domnode of the parameters passed in
		 *  as "parameters" to the constructor. */
		domnode	*getParameters();

		sqlrservercontroller	*cont;

	#include <sqlrelay/private/sqlrservermodule.h>
};

class SQLRSERVER_DLLSPEC sqlrprotocol : public sqlrservermodule {
	public:
		/** Creates an instance of sqlrprotocol, configured with
		 *  parameters "parameters".
		 *
		 *  This implementation handles the following parameters
		 *  which are generic to all protocols:
		 *
		 *  * tls - yes/no, whether or not to support TLS
		 *  * tlsversion - the TLS protocol version to use
		 *  * tlscert - the TLS certificate to use
		 *  * tlskey - the TLS key to use
		 *  * tlspassword - the password to use when reading the key
		 *  * tlsvalidate - yes/no - whether or not to validate peer
		 *  * tlsca - the TLS CA certificate to use
		 *  * tlsciphers - the TLS cipher list to use
		 *  * tlsdepth - the TLS validation depth
		 *
		 *  * krb - yes/no, whether or not to support Kerberos
		 *  * krbkeytab - the Kerberos keytab file to use
		 *  * krbservice - the Kerberos service to use
		 *  * krbmech - the Kerberos mech to use
		 *  * krbflags - the Kerboeros flags to use
		 *
		 *  However, it may be overridden by a child class to perform
		 *  additional initialization tasks and handle additional
		 *  parameters. */
		sqlrprotocol(sqlrservercontroller *cont,
					domnode *parameters);

		/** Deletes this instance of sqlrprotocol. */
		virtual	~sqlrprotocol();

		/** Has a session with the client, should:
		 *
		 *  * configure any socket options on "clientsock"
		 *  * accept the security context, if necessary
		 *  * get commands from the client
		 *  * perform those commands
		 *  * close the client connection
		 *  * end the session
		 *  * return the exit status
		 */
		virtual clientsessionexitstatus_t
				clientSession(filedescriptor *clientsock)=0;

		/** Returns true or false depending on whether the krb
		 *  parameter was set to yes or no. */
		virtual	bool	useKrb();

		/** Returns the GSS context if the krb parameter was "yes",
		 *  or NULL otherwise. */
		virtual gsscontext	*getGssContext();

		/** Returns true or false depending on whether the tls
		 *  parameter was set to yes or no. */
		virtual	bool	useTls();

		/** Returns the TLS context if the tls parameter was "yes",
		 *  or NULL otherwise. */
		virtual tlscontext	*getTlsContext();

	protected:

		/** Surprisingly, not all client-server protocols expect
		 *  integers to be passed in network byte order (big-endian).
		 *  Many, which presumably originated on intel hardware, expect
		 *  integers to be passed in little-endian byte order.
		 * 
		 *  This method sets the byte order that integers will be
		 *  passed across the network in.
		 *
		 *  If "bigendian" is true, then integers are expected to be
		 *  passed in to it in big-endian byte order.  If "bigendian"
		 *  is false, then integers are expected to be passed in to it
		 *  in little-endian byte order.
		 *
		 *  The read(), toHost(), write(), and hostTo() methods use
		 *  this setting to determine what conversion to use when
		 *  converting integers between protocol and host byte order. */
		void	setProtocolIsBigEndian(bool bigendian);

		/** Returns true or false as set by setProtocolIsBigEndian().
		 *
		 *  The read(), toHost(), write(), and hostTo() methods use
		 *  this setting to determine what conversion to use when
		 *  converting integers between protocol and host byte order. */
		bool	getProtocolIsBigEndian();

		/** Reads a (signed) character from byte string "rp" into
		 *  buffer "value" and sets "rpout" to the byte following the
		 *  character read. */
		void	read(const byte_t *rp,
					char *value,
					const byte_t **rpout);

		/** Reads a (signed) character from byte string "rp" into
		 *  buffer "value" and sets "rpout" to the byte following the
		 *  character read.
		 *
		 *  If the (signed) character read does not match "expected"
		 *  then "rpout" is set back to "rp" and an error message
		 *  including "name" is written out if debug is enabled.
		 *
		 *  Returns true if the value read matches expected and false
		 *  otherwise. */
		bool	read(const byte_t *rp,
					char *value,
					const char *name,
					char expected,
					const byte_t **rpout);

		/** Reads a byte from byte string "rp" into buffer "value"
		 *  and sets "rpout" to the byte following the byte read. */
		void	read(const byte_t *rp,
					byte_t *value,
					const byte_t **rpout);

		/** Reads a byte from byte string "rp" into buffer "value" and
		 *  sets "rpout" to the byte following the byte read.
		 *
		 *  If the byte read does not match "expected" then "rpout" is
		 *  set back to "rp" and an error message including "name" is
		 *  written out if debug is enabled.
		 *
		 *  Returns true if the value read matches expected and false
		 *  otherwise. */
		bool	read(const byte_t *rp,
					byte_t *value,
					const char *name,
					byte_t expected,
					const byte_t **rpout);

		/** Reads "size" (signed) characters from byte string "rp" into
		 *  buffer "value" and sets "rpout" to the byte following the
		 *  characters read. */
		void	read(const byte_t *rp,
					char *value,
					size_t size,
					const byte_t **rpout);

		/** Reads "size" bytes from byte string "rp" into buffer
		 *  "value" and sets "rpout" to the byte following the byte
		 *  read. */
		void	read(const byte_t *rp,
					byte_t *value,
					size_t size,
					const byte_t **rpout);

		/** Reads "size" ucs2_t characters from byte string "rp" into
		 *  buffer "value" and sets "rpout" to the byte following the
		 *  characters read. */
		void	read(const byte_t *rp,
					ucs2_t *value,
					size_t size,
					const byte_t **rpout);

		/** Reads a float from byte string "rp" into buffer "value" and
		 *  sets "rpout" to the byte following the float read. */
		void	read(const byte_t *rp,
					float *value,
					const byte_t **rpout);

		/** Reads a double from byte string "rp" into buffer "value" and
		 *  sets "rpout" to the byte following the double read. */
		void	read(const byte_t *rp,
					double *value,
					const byte_t **rpout);

		/** Reads a 16 bit unsigned integer from byte string "rp" into
		 *  buffer "value", converts "value" to host byte order
		 *  (consulting getProtocolIsBigEndian() to determine how to
		 *  convert it), and sets "rpout" to the byte following the
		 *  integer read. */
		void	read(const byte_t *rp,
					uint16_t *value,
					const byte_t **rpout);

		/** Reads a 16 bit unsigned integer from byte string "rp" into
		 *  buffer "value", converts "value" from little-endian to host
		 *  byte order, and sets "rpout" to the byte following the
		 *  integer read. */
		void	readLE(const byte_t *rp,
					uint16_t *value,
					const byte_t **rpout);

		/** Reads a 16 bit unsigned integer from byte string "rp" into
		 *  buffer "value", converts "value" from little-endian to host
		 *  byte order, and sets "rpout" to the byte following the
		 *  integer read.
		 *
		 *  If the integer read  does not match "expected" then "rpout"
		 *  is set back to "rp" and an error message including "name" is
		 *  written out if debug is enabled.
		 *
		 *  Returns true if the value read matches expected and false
		 *  otherwise. */
		bool	readLE(const byte_t *rp,
					uint16_t *value,
					const char *name,
					uint16_t expected,
					const byte_t **rpout);

		/** Reads a 16 bit unsigned integer from byte string "rp" into
		 *  buffer "value", converts "value" from big-endian to host
		 *  byte order, and sets "rpout" to the byte following the
		 *  integer read. */
		void	readBE(const byte_t *rp,
					uint16_t *value,
					const byte_t **rpout);

		/** Reads a 16 bit unsigned integer from byte string "rp" into
		 *  buffer "value", converts "value" from big-endian to host
		 *  byte order, and sets "rpout" to the byte following the
		 *  integer read.
		 *
		 *  If the integer read  does not match "expected" then "rpout"
		 *  is set back to "rp" and an error message including "name" is
		 *  written out if debug is enabled.
		 *
		 *  Returns true if the value read matches expected and false
		 *  otherwise. */
		bool	readBE(const byte_t *rp,
					uint16_t *value,
					const char *name,
					uint16_t expected,
					const byte_t **rpout);

		/** Reads a 32 bit unsigned integer from byte string "rp" into
		 *  buffer "value", converts "value" to host byte order
		 *  (consulting getProtocolIsBigEndian() to determine how to
		 *  convert it), and sets "rpout" to the byte following the
		 *  integer read. */
		void	read(const byte_t *rp,
					uint32_t *value,
					const byte_t **rpout);

		/** Reads a 32 bit unsigned integer from byte string "rp" into
		 *  buffer "value", converts "value" from little-endian to host
		 *  byte order, and sets "rpout" to the byte following the
		 *  integer read. */
		void	readLE(const byte_t *rp,
					uint32_t *value,
					const byte_t **rpout);

		/** Reads a 32 bit unsigned integer from byte string "rp" into
		 *  buffer "value", converts "value" from little-endian to host
		 *  byte order, and sets "rpout" to the byte following the
		 *  integer read.
		 *
		 *  If the integer read  does not match "expected" then "rpout"
		 *  is set back to "rp" and an error message including "name" is
		 *  written out if debug is enabled.
		 *
		 *  Returns true if the value read matches expected and false
		 *  otherwise. */
		bool	readLE(const byte_t *rp,
					uint32_t *value,
					const char *name,
					uint32_t expected,
					const byte_t **rpout);

		/** Reads a 32 bit unsigned integer from byte string "rp" into
		 *  buffer "value", converts "value" from big-endian to host
		 *  byte order, and sets "rpout" to the byte following the
		 *  integer read. */
		void	readBE(const byte_t *rp,
					uint32_t *value,
					const byte_t **rpout);

		/** Reads a 32 bit unsigned integer from byte string "rp" into
		 *  buffer "value", converts "value" from big-endian to host
		 *  byte order, and sets "rpout" to the byte following the
		 *  integer read.
		 *
		 *  If the integer read  does not match "expected" then "rpout"
		 *  is set back to "rp" and an error message including "name" is
		 *  written out if debug is enabled.
		 *
		 *  Returns true if the value read matches expected and false
		 *  otherwise. */
		bool	readBE(const byte_t *rp,
					uint32_t *value,
					const char *name,
					uint32_t expected,
					const byte_t **rpout);

		/** Reads a 64 bit unsigned integer from byte string "rp" into
		 *  buffer "value", converts "value" to host byte order
		 *  (consulting getProtocolIsBigEndian() to determine how to
		 *  convert it), and sets "rpout" to the byte following the
		 *  integer read. */
		void	read(const byte_t *rp,
					uint64_t *value,
					const byte_t **rpout);

		/** Reads a 64 bit unsigned integer from byte string "rp" into
		 *  buffer "value", converts "value" from little-endian to host
		 *  byte order, and sets "rpout" to the byte following the
		 *  integer read. */
		void	readLE(const byte_t *rp,
					uint64_t *value,
					const byte_t **rpout);

		/** Reads a 64 bit unsigned integer from byte string "rp" into
		 *  buffer "value", converts "value" from little-endian to host
		 *  byte order, and sets "rpout" to the byte following the
		 *  integer read.
		 *
		 *  If the integer read  does not match "expected" then "rpout"
		 *  is set back to "rp" and an error message including "name" is
		 *  written out if debug is enabled.
		 *
		 *  Returns true if the value read matches expected and false
		 *  otherwise. */
		bool	readLE(const byte_t *rp,
					uint64_t *value,
					const char *name,
					uint64_t expected,
					const byte_t **rpout);

		/** Reads a 64 bit unsigned integer from byte string "rp" into
		 *  buffer "value", converts "value" from big-endian to host
		 *  byte order, and sets "rpout" to the byte following the
		 *  integer read. */
		void	readBE(const byte_t *rp,
					uint64_t *value,
					const byte_t **rpout);

		/** Reads a 64 bit unsigned integer from byte string "rp" into
		 *  buffer "value", converts "value" from big-endian to host
		 *  byte order, and sets "rpout" to the byte following the
		 *  integer read.
		 *
		 *  If the integer read  does not match "expected" then "rpout"
		 *  is set back to "rp" and an error message including "name" is
		 *  written out if debug is enabled.
		 *
		 *  Returns true if the value read matches expected and false
		 *  otherwise. */
		bool	readBE(const byte_t *rp,
					uint64_t *value,
					const char *name,
					uint64_t expected,
					const byte_t **rpout);

		/** Reads length-encoded integer from byte string "in",
		 *  converts it from big-endian to host byte order, returns it,
		 *  and sets "out" to the byte following the end of the
		 *  length-encoded integer. */
		uint64_t	readLenEncInt(const byte_t *in,
						const byte_t **out);

		/** Writes "value" to byte buffer "buffer". */
		void	write(bytebuffer *buffer, char value);

		/** Writes "value" to byte buffer "buffer". */
		void	write(bytebuffer *buffer, byte_t value);

		/** Writes "value" to byte buffer "buffer". */
		void	write(bytebuffer *buffer, const char *value);

		/** Writes "size" characters of "value" to byte buffer
		 *  "buffer". */
		void	write(bytebuffer *buffer, const char *value,
								size_t size);

		/** Writes "size" bytes of "value" to byte buffer "buffer". */
		void	write(bytebuffer *buffer, const byte_t *value,
								size_t size);

		/** Writes "size" characters of "value" to byte buffer
		 *  "buffer". */
		void	write(bytebuffer *buffer, const ucs2_t *str,
								size_t size);

		/** Writes "value" to byte buffer "buffer". */
		void	write(bytebuffer *buffer, float value);

		/** Writes "value" to byte buffer "buffer". */
		void	write(bytebuffer *buffer, double value);

		/** Converts "value" to protocol byte order (consulting
		 *  getProtocolIsBigEndian() to determine how to convert it)
		 *  then writes "value" to byte buffer "buffer". */
		void	write(bytebuffer *buffer, uint16_t value);

		/** Converts "value" from host byte order to little-endian,
		 *  then writes "value" to byte buffer "buffer". */
		void	writeLE(bytebuffer *buffer, uint16_t value);

		/** Converts "value" from host byte order to big-endian,
		 *  then writes "value" to byte buffer "buffer". */
		void	writeBE(bytebuffer *buffer, uint16_t value);

		/** Converts "value" to protocol byte order (consulting
		 *  getProtocolIsBigEndian() to determine how to convert it)
		 *  then writes "value" to byte buffer "buffer". */
		void	write(bytebuffer *buffer, uint32_t value);

		/** Converts "value" from host byte order to little-endian,
		 *  then writes "value" to byte buffer "buffer". */
		void	writeLE(bytebuffer *buffer, uint32_t value);

		/** Converts "value" from host byte order to big-endian,
		 *  then writes "value" to byte buffer "buffer". */
		void	writeBE(bytebuffer *buffer, uint32_t value);

		/** Converts "value" to protocol byte order (consulting
		 *  getProtocolIsBigEndian() to determine how to convert it)
		 *  then writes "value" to byte buffer "buffer". */
		void	write(bytebuffer *buffer, uint64_t value);

		/** Converts "value" from host byte order to little-endian,
		 *  then writes "value" to byte buffer "buffer". */
		void	writeLE(bytebuffer *buffer, uint64_t value);

		/** Converts "value" from host byte order to big-endian,
		 *  then writes "value" to byte buffer "buffer". */
		void	writeBE(bytebuffer *buffer, uint64_t value);

		/** Writes length-encoded-integer "value" to byte buffer
		 *  "buffer". */
		void	writeLenEncInt(bytebuffer *buffer,
						uint64_t value);

		/** Writes length-encoded-string "string" to byte buffer
		 *  "buffer". */
		void	writeLenEncStr(bytebuffer *buffer,
						const char *string);

		/** Writes "size" bytes of length-encoded-string "string" to
		 *  byte buffer "buffer". */
		void	writeLenEncStr(bytebuffer *buffer,
						const char *string,
						uint64_t size);

		/** Converts "value" from host byte order to big-endian, then
		 *  writes the first 3 bytes of it to byte buffer "buffer". */
		void	writeTriplet(bytebuffer *buffer, uint32_t value);

		/** Converts "value" from protocol byte order to host byte
		 *  order (consulting getProtocolIsBigEndian() to determine how
		 *  to convert it) and returns it. */
		uint16_t	toHost(uint16_t value);

		/** Converts "value" from protocol byte order to host byte
		 *  order (consulting getProtocolIsBigEndian() to determine how
		 *  to convert it) and returns it. */
		uint32_t	toHost(uint32_t value);

		/** Converts "value" from protocol byte order to host byte
		 *  order (consulting getProtocolIsBigEndian() to determine how
		 *  to convert it) and returns it. */
		uint64_t	toHost(uint64_t value);

		/** Converts "value" from little-endian to host byteorder and
		 *  returns it. */
		uint16_t	leToHost(uint16_t value);

		/** Converts "value" from little-endian to host byteorder and
		 *  returns it. */
		uint32_t	leToHost(uint32_t value);

		/** Converts "value" from little-endian to host byteorder and
		 *  returns it. */
		uint64_t	leToHost(uint64_t value);

		/** Converts "value" from big-endian to host byteorder and
		 *  returns it. */
		uint16_t	beToHost(uint16_t value);

		/** Converts "value" from big-endian to host byteorder and
		 *  returns it. */
		uint32_t	beToHost(uint32_t value);

		/** Converts "value" from big-endian to host byteorder and
		 *  returns it. */
		uint64_t	beToHost(uint64_t value);

		/** Converts "value" from host byte order to protocol byte order
		 *  (consulting getProtocolIsBigEndian() to determine how to
		 *  convert it) and returns it. */
		uint16_t	hostTo(uint16_t value);

		/** Converts "value" from host byte order to protocol byte order
		 *  (consulting getProtocolIsBigEndian() to determine how to
		 *  convert it) and returns it. */
		uint32_t	hostTo(uint32_t value);

		/** Converts "value" from host byte order to protocol byte order
		 *  (consulting getProtocolIsBigEndian() to determine how to
		 *  convert it) and returns it. */
		uint64_t	hostTo(uint64_t value);

		/** Converts "value" from host byte order to little-endian and
		 *  returns it. */
		uint16_t	hostToLE(uint16_t value);

		/** Converts "value" from host byte order to little-endian and
		 *  returns it. */
		uint32_t	hostToLE(uint32_t value);

		/** Converts "value" from host byte order to little-endian and
		 *  returns it. */
		uint64_t	hostToLE(uint64_t value);

		/** Converts "value" from host byte order to big-endian and
		 *  returns it. */
		uint16_t	hostToBE(uint16_t value);

		/** Converts "value" from host byte order to big-endian and
		 *  returns it. */
		uint32_t	hostToBE(uint32_t value);

		/** Converts "value" from host byte order to big-endian and
		 *  returns it. */
		uint64_t	hostToBE(uint64_t value);

	#include <sqlrelay/private/sqlrprotocol.h>
};

class SQLRSERVER_DLLSPEC sqlrcredentials {
	public:
		/** Creates an instance of sqlrcredentials. */
		sqlrcredentials();

		/** Deletes this instance of sqlrcredentials. */
		virtual	~sqlrcredentials();

		/** Returns a string describing the type of credentials. */
		virtual const char	*getType()=0;
};

class SQLRSERVER_DLLSPEC sqlruserpasswordcredentials : public sqlrcredentials {
	public:
		/** Creates an instance of sqlruserpasswordcredentials. */
		sqlruserpasswordcredentials();

		/** Deletes this instance of sqlruserpasswordcredentials. */
		~sqlruserpasswordcredentials();

		/** Returns "userpassword". */
		const char	*getType();

		/** Sets the user to "user". */
		void	setUser(const char *user);

		/** Sets the password to "password". */
		void	setPassword(const char *password);

		/** Returns the user. */
		const char	*getUser();

		/** Returns the password. */
		const char	*getPassword();

	#include <sqlrelay/private/sqlruserpasswordcredentials.h>
};

class SQLRSERVER_DLLSPEC sqlrgsscredentials : public sqlrcredentials {
	public:
		/** Creates an instance of sqlrgsscredentials. */
		sqlrgsscredentials();

		/** Deletes this instance of sqlrgsscredentials. */
		~sqlrgsscredentials();

		/** Returns "gss". */
		const char	*getType();

		/** Sets the initiator to "initiator". */
		void		setInitiator(const char *initiator);

		/** Returns the initiator. */
		const char	*getInitiator();

	#include <sqlrelay/private/sqlrgsscredentials.h>
};

class SQLRSERVER_DLLSPEC sqlrtlscredentials : public sqlrcredentials {
	public:
		/** Creates an instance of sqlrtlscredentials. */
		sqlrtlscredentials();

		/** Deletes this instance of sqlrtlscredentials. */
		~sqlrtlscredentials();

		/** Returns "tls". */
		const char	*getType();

		/** Sets the common name to "commonname". */
		void	setCommonName(const char *commonname);

		/** Sets the set of subject alternative names to
		 *  "subjectalternativenames". */
		void	setSubjectAlternateNames(
				linkedlist < char * > *subjectalternatenames);

		/** Returns the common name. */
		const char		*getCommonName();

		/** Returns the set of subject alternative names. */
		linkedlist< char * >	*getSubjectAlternateNames();

	#include <sqlrelay/private/sqlrtlscredentials.h>
};

class SQLRSERVER_DLLSPEC sqlrmysqlcredentials : public sqlrcredentials {
	public:
		/** Creates an instance of sqlrmysqlcredentials. */
		sqlrmysqlcredentials();

		/** Deletes this instance of sqlrmysqlcredentials. */
		~sqlrmysqlcredentials();

		/** Returns "mysql". */
		const char	*getType();

		/** Sets the user to "user". */
		void	setUser(const char *user);

		/** Sets the password to "password". */
		void	setPassword(const char *password);

		/** Sets the number of bytes in the password to
		 *  "passwordsize". */
		void	setPasswordSize(uint64_t passwordsize);

		/** Sets the authentication method (eg. mysql_native_password,
		 *  sha256_password, cached_sha2_password, etc.) to "method". */
		void	setMethod(const char *method);

		/** Sets the extra info required for the selected
		 *  authentication method (eg. challenge) to "extra". */
		void	setExtra(const char *extra);

		/** Returns the user. */
		const char	*getUser();

		/** Returns the password. */
		const char	*getPassword();

		/** Returns the number of bytes in the password. */
		uint64_t	getPasswordSize();

		/** Returns the authentication method. */
		const char	*getMethod();

		/** Returns the extra info. */
		const char	*getExtra();

	#include <sqlrelay/private/sqlrmysqlcredentials.h>
};

class SQLRSERVER_DLLSPEC sqlrpostgresqlcredentials : public sqlrcredentials {
	public:
		/** Creates an instance of sqlrpostgresqlcredentials. */
		sqlrpostgresqlcredentials();

		/** Deletes this instance of sqlrpostgresqlcredentials. */
		~sqlrpostgresqlcredentials();

		/** Returns "postgresql". */
		const char	*getType();

		/** Sets the user to "user". */
		void	setUser(const char *user);

		/** Sets the password to "password". */
		void	setPassword(const char *password);

		/** Sets the number of bytes in the password to
		 *  "passwordsize". */
		void	setPasswordSize(uint64_t passwordsize);

		/** Sets the authentication method (eg. psotgresql_md5,
		 *  postgresql_cleartext, etc.) to "method". */
		void	setMethod(const char *method);

		/** Sets the salt required by some authentication methods to
		 *  "salt". */
		void	setSalt(uint32_t salt);

		/** Returns the user. */
		const char	*getUser();

		/** Returns the password. */
		const char	*getPassword();

		/** Returns the number of bytes in the password. */
		uint64_t	getPasswordSize();

		/** Returns the authentication method. */
		const char	*getMethod();

		/** Returns the salt. */
		uint32_t	getSalt();

	#include <sqlrelay/private/sqlrpostgresqlcredentials.h>
};

class SQLRSERVER_DLLSPEC sqlroraclecredentials : public sqlrcredentials {
	public:
		/** Creates an instance of sqlroraclecredentials. */
		sqlroraclecredentials();

		/** Deletes this instance of sqlroraclecredentials. */
		~sqlroraclecredentials();

		/** Returns "oracle". */
		const char	*getType();

		/** Sets the user to "user". */
		void	setUser(const char *user);

		/** Sets the password to "password". */
		void	setPassword(const char *password);

		/** Sets the number of bytes in the password to
		 *  "passwordsize". */
		void	setPasswordSize(uint64_t passwordsize);

		/** Sets the authentication method to "method". */
		void	setMethod(const char *method);

		/** Sets the extra info required for the selected
		 *  authentication method to "extra". */
		void	setExtra(const char *extra);

		/** Returns the user. */
		const char	*getUser();

		/** Returns the password. */
		const char	*getPassword();

		/** Returns the number of bytes in the password. */
		uint64_t	getPasswordSize();

		/** Returns the authentication method. */
		const char	*getMethod();

		/** Returns the extra info. */
		const char	*getExtra();

	#include <sqlrelay/private/sqlroraclecredentials.h>
};

class SQLRSERVER_DLLSPEC sqlrteradatacredentials : public sqlrcredentials {
	public:
		/** Creates an instance of sqlrteradatacredentials. */
		sqlrteradatacredentials();

		/** Deletes this instance of sqlroraclecredentials. */
		~sqlrteradatacredentials();

		/** Returns "teradata". */
		const char	*getType();

		/** Sets the client file descriptor to "fd". */
		void	setClientFileDescriptor(filedescriptor *fd);

		/** Returns the client file desctiptor. */
		filedescriptor	*getClientFileDescriptor();

	#include <sqlrelay/private/sqlrteradatacredentials.h>
};

class SQLRSERVER_DLLSPEC sqlrauth : public sqlrservermodule {
	public:
		/** Creates an instance of sqlrauth, configured with
		 *  parameters "parameters".
		 *
		 *  This implementation doesn't handle any parameters,
		 *  however, it may be overridden by a child class to
		 *  perform additional initialization tasks and handle
		 *  some set of parameters. */
		sqlrauth(sqlrservercontroller *cont,
					domnode *parameters);

		/** Deletes this instance of sqlrauth. */
		virtual	~sqlrauth();

		/** Authenticates "cred" against the authentication backend.
		 *  Returns a representation of the entity that was successfully
		 *  authenticated (eg. user, GSS initiator, TLS common name or
		 *  subject alternate name, etc.) if authentication succeeded,
		 *  or NULL if authentication failed.
		 *
		 *  This implementation just returns NULL, but may be
		 *  overridden by a child class to perform authentication
		 *  of instances one or more children of sqlrcredentials
		 *  against against a particular backend, such as a list of
		 *  users, the database itself, or some other authentication
		 *  system. */
		virtual	const char	*auth(sqlrcredentials *cred);

	#include <sqlrelay/private/sqlrauth.h>
};

class SQLRSERVER_DLLSPEC sqlrlogger : public sqlrservermodule {
	public:
		/** Creates an instance of sqlrlogger, configured with
		 *  parameters "parameters".
		 *
		 *  This implementation doesn't handle any parameters,
		 *  however, it may be overridden by a child class to
		 *  perform additional initialization tasks and handle
		 *  some set of parameters. */
		sqlrlogger(domnode *parameters);

		/** Deletes this instance of sqlrlogger. */
		virtual	~sqlrlogger();

		/** Initializes this instance of sqlrlogger.
		 *  
		 *  Returns true on success and false if an error occurred. */
		virtual bool	init(sqlrlistener *sqlrl,
					sqlrserverconnection *sqlrcon);

		/** Logs "event" of loglevel "level" with "info".
		 *
		 *  Returns true on success and false if an error occurred.
		 *
		 *  This implementation just returns true, but may be
		 *  overridden by a child class to perform logging of
		 *  specific events/info at specific loglevels to specific
		 *  backends. */
		virtual bool	run(sqlrlistener *sqlrl,
					sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					sqlrloglevel_t level,
					sqlrevent_t event,
					const char *info);

	#include <sqlrelay/private/sqlrlogger.h>
};

class SQLRSERVER_DLLSPEC sqlrnotification : public sqlrservermodule {
	public:
		/** Creates an instance of sqlrnotification, configured with
		 *  parameters "parameters".
		 *
		 *  This implementation doesn't handle any parameters,
		 *  however, it may be overridden by a child class to
		 *  perform additional initialization tasks and handle
		 *  some set of parameters. */
		sqlrnotification(domnode *parameters);
		virtual	~sqlrnotification();

		/** Performs notification of "event" with "info".
		 *
		 *  Returns true on success and false if an error occurred.
		 *
		 *  This implementation just returns true, but may be
		 *  overridden by a child class to perform notification of
		 *  specific events/info to specific backends. */
		virtual bool	run(sqlrlistener *sqlrl,
					sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					sqlrevent_t event,
					const char *info);

		/** Composes a notification regarding "event" with additional
		 *  information "info" using template "templatefile" and sends
		 *  it to "address" with subject "subject" using the transport
		 *  defined by "transportid".
		 *
		 *  Currently, the only supported transport is mail, so
		 *  effectively the transportid parameter is ignored, but may
		 *  be implemented in the future.
		 *
		 *  If "templatefile" is null or empty then the default
		 *  template is used:
		 *
		 *  SQL Relay Notification
		 *
		 *  Event          : @event@
		 *  Event Info     : @eventinfo@
		 *  Date           : @datetime@
		 *  Host Name      : @hostname@
		 *  Instance       : @instance@
		 *  Process Id     : @pid@
		 *  Client Address : @clientaddr@
		 *  Client Info    : @clientinfo@
		 *  User           : @user@
		 *  Query          : 
		 *  @query@
		 *
		 *  * @event@ is substituted with "event"
		 *  * @eventinfo@ is substituted with "info"
		 *  * @datetime@ is substituted with the current date/time
		 *  * @hostname@ is substituted with the hostname
		 *  * @instance@ is substituted with the id of the
		 *               sqlr-listner or sqlr-connection process
		 *  * @pid@ is substituted with process id of the
		 *               sqlr-listner or sqlr-connection process
		 *  * @clientaddr@ is substituted with the IP address of the
		 *                 client that generated the event
		 *  * @clientinfo@ is substituted with any client info provided
		 *                 by the client that generated the event
		 *  * @user@ is substituted with the current user
		 *  * @query@ is substituted with the query that generated the
		 *            event, if there was one
		 *
		 *  Returns true on success and false if an error occurred. */
		bool	sendNotification(sqlrlistener *sqlrl,
						sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						const char *address,
						const char *transportid,
						const char *subject,
						const char *templatefile,
						sqlrevent_t event,
						const char *info);

	#include <sqlrelay/private/sqlrnotification.h>
};

class SQLRSERVER_DLLSPEC sqlrscheduleperiod {
	public:
		uint16_t	start;
		uint16_t	end;
};

class SQLRSERVER_DLLSPEC sqlrscheduledaypart {
	public:
		uint16_t	starthour;
		uint16_t	startminute;
		uint16_t	endhour;
		uint16_t	endminute;
};

class SQLRSERVER_DLLSPEC sqlrschedulerule {
	public:
		sqlrschedulerule(bool allow, const char *when);
		sqlrschedulerule(bool allow,
			const char *years,
			const char *months,
			const char *daysofmonth,
			const char *daysofweek,
			const char *dayparts);
		~sqlrschedulerule();

		bool	allowed(datetime *dt, bool currentlyallowed);
		
	#include <sqlrelay/private/sqlrschedulerule.h>
};

class SQLRSERVER_DLLSPEC sqlrschedule : public sqlrservermodule {
	public:
		/** Creates an instance of sqlrschedule, configured with
		 *  parameters "parameters".
		 *
		 *  This implementation handles the following parameters
		 *  which are generic to all schedules.
		 *
		 *  * default - allow/deny, whether access is initially allowed
		 *              or denied, prior to the application of any
		 *              rules.  Defaults to "allow" if not set.
		 *
		 *  This implementation also parses and applies rules of the
		 *  following format:
		 *
		 *  <rules>
		 *    <!-- deny access at all times -->
		 *    <deny when="* * * * *"/>
		 *
		 *    <!-- allow access during business hours -->
		 *    <allow when="* * * 2-5 8:00-17:00"/>
		 *
		 *    <!-- deny access during the lunch hour -->
		 *    <deny when="* * * 2-5 12:00-12:59"/>
		 *  </rules>
		 *
		 *  Access is allowed by default, unless default="deny" is set.
		 *  Each rule is then applied, in order, and each rule may
		 *  reverse the outcome of the previous rule.
		 *
		 *  It may be overridden by a child class to perform additional
		 *  initialization tasks and handle additional parameters. */
		sqlrschedule(sqlrservercontroller *cont,
					domnode *parameters);

		/** Deletes this instance of sqlrschedule. */
		virtual	~sqlrschedule();

		/** Returns true if the default is to allow access, prior to
		 *  the application of any rules, or false if the default is to
		 *  deny access, prior to the application of any rules, as set
		 *  by the "default" parameter. */
		bool getDefaultAllow();

		/** Adds a schedule rule for time period "when".
		 *
		 *  If "allow" is true then the user is allowed access.  If
		 *  "allow" is false  then the user is denied access.
		 *
		 *  This implementation interprets "when" as a cron-style
		 *  string defining which years, months, days of month, days of
		 *  week, and day parts the rule applies to.
		 *
		 *  May be overridden by a child class to interpret "when"
		 *  differently. */
		virtual	void	addRule(bool allow, const char *when);

		/** Adds a schedule rule for broken-down time period composed
		 *  of "years", "months", "daysofmonth", "daysofweek", and
		 *  "dayparts".
		 *  
		 *  If "allow" is true then the user is allowed access.  If
		 *  "allow" is false then the user is denied access.
		 *
		 *  This implementation interprets each of the broken-down
		 *  time period components as cron-style string components,
		 *  which collectively define the time period that the rule
		 *  applies to.
		 *
		 *  May be overridden by a child class to interpret the time
		 *  period components differently. */
		virtual	void	addRule(bool allow,
					const char *years,
					const char *months,
					const char *daysofmonth,
					const char *daysofweek,
					const char *dayparts);

		/** Clears all rules. */
		virtual void	clearRules();

		/** Determines if the user is allowed access or not, at
		 *  date/time "dt".  If the user is currently allowed access,
		 *  then "currentlyallowed" should be set to true.  If the user
		 *  is not currently allowed access, then "currentlyallowed"
		 *  should be set to false.
		 *
		 *  Returns "currentlyallowed" if none of the rules affect
		 *  the date/time "dt".  Returns true if the rules do affect
		 *  the date/time "dt" and allow access.  Returns false if the
		 *  rules do affect the date/time "dt" and deny access. */
		virtual	bool	rulesAllow(datetime *dt, bool currentlyallowed);

		/** Returns true of "user" is allowed access at the current
		 *  time, and false if "user" is not allowed access.
		 *
		 *  This implementation ignores "user" and just returns true if
		 *  access woud be allowed by comparing the current date/time
		 *  to the rules, taking the "default" parameter into account,
		 *  or false otherwise.  It may be overridden by child class to
		 *  evaluate rules to determine if the user is allowed
		 *  access. */
		virtual bool	allowed(sqlrserverconnection *sqlrcon,
							const char *user);

	#include <sqlrelay/private/sqlrschedule.h>
};

class SQLRSERVER_DLLSPEC sqlrrouters : public sqlrservermodules {
	public:
		/** Creates an instance of sqlrrouters, where:
		 *
		 *  * "connections" is an array of sqlrconnections, as defined
		 *    in the config file
		 *  * "connectionids" is an array of corresponding ids, as
		 *    specified in the config file
		 *  * "connectioncount" is the number of elements in those
		 *    arrays. */
		sqlrrouters(sqlrservercontroller *cont,
				sqlrconnection **connections,
				const char **connectionids,
				uint16_t connectioncount);

		/** Deletes this instance of sqlrrouters. */
		~sqlrrouters();

		/** Returns the array of sqlrconnections passed in to the
		 *  constructor. */
		sqlrconnection	**getConnections();

		/** Returns the array of corresponding ids passed in to the
		 *  constructor. */
		const char	**getConnectionIds();

		/** Returns the number of elements in the arrays of
		 *  sqlrconnections and ids that were passed into the
		 *  constructor. */
		uint16_t	getConnectionCount();

		/** Runs through the router modules, running each one's route()
		 *  method, and returning the connectionid of the first one
		 *  that doesn't return NULL.  Returns NULL if all modules
		 *  returned NULL. */
		const char	*route(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						const char **err,
						int64_t *errn);

		/** Runs through the router modules, running each one's
		 *  routeEntireSession() method, and returning true if all
		 *  return true, or false if one returns false. */
		bool	routeEntireSession();

		/** Sets the currently selected connection id to "connid". */
		void	setCurrentConnectionId(const char *connid);

		/** Gets the current connection id as set by a previous call to
		 *  setCurrentConnectionId(), or NULL if
		 *  setCurrentConnectionId() has never been set.  Typically
		 *  called after route(), with the result of that method. */
		const char	*getCurrentConnectionId();

	#include <sqlrelay/private/sqlrrouters.h>
};

class SQLRSERVER_DLLSPEC sqlrrouter : public sqlrservermodule {
	public:
		/** Creates an instance of sqlrrouter, configured with
		 *  parameters "parameters".
		 *
		 *  This implementation doesn't handle any parameters,
		 *  however, it may be overridden by a child class to
		 *  perform additional initialization tasks and handle
		 *  some set of parameters. */
		sqlrrouter(sqlrservercontroller *cont,
					sqlrrouters *rs,
					domnode *parameters);

		/** Deletes this instance of sqlrrouter. */
		virtual	~sqlrrouter();

		/** Returns true if this implementation routes entire sessions,
		 *  and false if this module routes individual SQL commands or
		 *  queries. */
		virtual	bool	routeEntireSession();

		/** Examines the current operation that "sqlrcon" and/or
		 *  "sqlrcur" is engaged in and returns the connectionid that
		 *  the operation needs to be routed to, or NULL if none of
		 *  this instance's rules apply to the current operation.
		 *
		 *  This implementation just returns NULL, but it may be
		 *  overridden by child class to evaluate rules to determine
		 *  which connectionid to route an operation to.
		 *
		 *  If an error occurred, then "err" is set to the error
		 *  message and "errn" is set to the error number. */
		virtual const char *route(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						const char **err,
						int64_t *errn);

	protected:

		/** Returns the instance of sqlrrouters passed in as "rs" to
		 *  the constructor. */
		sqlrrouters	*getRouters();

	#include <sqlrelay/private/sqlrrouter.h>
};

class SQLRSERVER_DLLSPEC sqlrparser : public sqlrservermodule {
	public:
		/** Creates an instance of sqlrparser, configured with
		 *  parameters "parameters".
		 *
		 *  This implementation doesn't handle any parameters,
		 *  however, it may be overridden by a child class to
		 *  perform additional initialization tasks and handle
		 *  some set of parameters. */
		sqlrparser(sqlrservercontroller *cont,
				domnode *parameters);

		/** Deletes this instance of sqlrparser. */
		virtual	~sqlrparser();

		/** By default, when parse() is called, it allocates a query
		 *  tree internally and appends to the root node of that tree.
		 *  This method configures this instance to append to the root
		 *  node of "querytree" rather than to the root node of the
		 *  internally-allocated query tree when parse() is called.
		 *
		 *  setQueryTree(NULL) can be called to revert to the default
		 *  behavior.
		 *
		 *  Should be called prior to calling parse(), otherwise the
		 *  behavior is undefined. */
		virtual	void	setQueryTree(xmldom *querytree);

		/** Returns the query tree root node set by a previous call to
		 *  setQueryTree(), or the root node of the internal query tree
		 *  if no call to setQueryTree() was previously made or if
		 *  setQueryTree(NULL) was previously called.
		 *
		 *  May return NULL if neither setQueryTree() nor parse() have
		 *  ever been called, or if setQueryTree(NULL) was previously
		 *  called. */
		virtual	xmldom	*getQueryTree();

		/** Detaches the query tree that this instance is configured to
		 *  use and returns it - either the internally-allocated query
		 *  tree or the query tree set by a previous call to
		 *  setQueryTree().
		 *
		 *  Subsequent calls to detachTree() will return NULL.
		 *
		 *  The returned query tree must ultimately be deallocated by
		 *  the calling program.
		 *
		 *  May return NULL if neither setQueryTree() nor parse() have
		 *  ever been called, or if setQueryTree(NULL) was previously
		 *  called. */
		virtual	xmldom	*detachQueryTree();

		/** By default, when parse() is called, it allocates a metadata
		 *  tree internally and appends to the root node of that tree.
		 *  This method configures this instance to append to the root
		 *  node of "metadatatree" rather than to the root node of the
		 *  internally-allocated metadata tree when parse() is called.
		 *
		 *  setMetaDataTree(NULL) can be called to revert to the default
		 *  behavior.
		 *
		 *  Should be called prior to calling parse(), otherwise the
		 *  behavior is undefined. */
		virtual	void	setMetaDataTree(xmldom *metadatatree);

		/** Returns the metadata tree root node set by a previous call
		 *  to setMetaDataTree(), or the root node of the internal
		 *  metadata tree if no call to setMetaDataTree() was
		 *  previously made or if setMetaDataTree(NULL) was previously
		 *  called.
		 *
		 *  May return NULL if neither setMetaDataTree() nor parse()
		 *  have ever been called, or if setMetaDataTree(NULL) was
		 *  previously called. */
		virtual	xmldom	*getMetaDataTree();

		/** Detaches the metadata tree that this instance is configured
		 *  to use and returns it - either the internally-allocated
		 *  metadata tree or the metadata tree set by a previous call
		 *  to setMetaDataTree().
		 *
		 *  Subsequent calls to detachTree() will return NULL.
		 *
		 *  The returned metadata tree must ultimately be deallocated by
		 *  the calling program.
		 *
		 *  May return NULL if neither setMetaDataTree() nor parse()
		 *  have ever been called, or if setMetaDataTree(NULL) was
		 *  previously called. */
		virtual	xmldom	*detachMetaDataTree();

		/** Parses "query" and populates an xmldom tree that represents
		 *  the query.  Also, optionally, populatss an xmldom tree
		 *  representing metadata about the query.
		 *
		 *  Returns true on success and false if an error occurred.
		 *
		 *  This implementation just empties any internally-allocated
		 *  query and metadata trees and returns false, but may be
		 *  overridden by a child class to parse the query and generate
		 *  an implementation-specific tree representing the query,
		 *  and, optionally, an implementation-specific tree
		 *  representing metadata about the query. */
		virtual	bool	parse(const char *query);

		/** Walks the query tree and writes the query represented by
		 *  the tree to "output".
		 *
		 *  Returns true on success and false if an error occurred.
		 *
		 *  This implementation just returns false, but may be
		 *  overridden by a child class to write out the
		 *  implementation-specific query tree. */
		virtual	bool	write(stringbuffer *output);

		/** Walks the query tree, starting at "node" and writes the
		 *  query represented by that node, its children, its siblings,
		 *  and their children to "output".
		 *
		 *  Returns true on success and false if an error occurred.
		 *
		 *  This implementation just returns false, but may be
		 *  overridden by a child class to write out the
		 *  implementation-specific query tree. */
		virtual	bool	write(domnode *node, 
					stringbuffer *output);

		/** Walks the query tree, starting at "node" and writes the
		 *  query represented to "output".
		 *
		 *  If "omitsiblings" is true then only the parts of the query
		 *  represented by that node and its children will be written.
		 *
		 *  If "omitsiblings" is false then the parts of the query
		 *  represented by that node, its children, its siblings,
		 *  and their children will be written.
		 *
		 *  Returns true on success and false if an error occurred.
		 *
		 *  This implementation just returns false, but may be
		 *  overridden by a child class to write out the
		 *  implementation-specific query tree. */
		virtual	bool	write(domnode *node,
					stringbuffer *output,
					bool omitsiblings);

	#include <sqlrelay/private/sqlrparser.h>
};

class SQLRSERVER_DLLSPEC sqlrdirective : public sqlrservermodule {
	public:
		/** Creates an instance of sqlrdirective, configured with
		 *  parameters "parameters".
		 *
		 *  This implementation doesn't handle any parameters,
		 *  however, it may be overridden by a child class to
		 *  perform additional initialization tasks and handle
		 *  some set of parameters. */
		sqlrdirective(sqlrservercontroller *cont,
					domnode *parameters);

		/** Deletes this instance of sqlrparser. */
		virtual	~sqlrdirective();

		/** Applies this directive to "query".
		 *
		 *  Returns true on success and false if an error occurred.
		 *
		 *  This implementation just returns true, but may be
		 *  overridden by a child class to perform specific tasks. */
		virtual bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					const char *query);
	protected:

		/** Searches "line" for a directive.  Directives must start
		 *  with the SQL comment -- and end with a new line.
		 *  For example:
		 * 
		 *  -- directive
		 *
		 *  If a directive is found on "line" then "directivestart" is
		 *  set to the first byte of the directive and "directivesize"
		 *  is set to the number of bytes in the directive.  "newline"
		 *  is set to the \r following the directive.
		 *  
		 *  If no directive was found then "directivestart" is set to
		 *  NULL, "directivesize" is set to 0, and newline is set to
		 *  the start of the line.
		 *
		 *  Returns true if a directive was found and false if no
		 *  directive was found. */
		bool	getDirective(const char *line,
					const char **directivestart,
					uint32_t *directivesize,
					const char **newline);

	#include <sqlrelay/private/sqlrdirective.h>
};

class SQLRSERVER_DLLSPEC sqlrquerytranslation : public sqlrservermodule {
	public:
		/** Creates an instance of sqlrquerytranslation, configured with
		 *  parameters "parameters".
		 *
		 *  This implementation doesn't handle any parameters,
		 *  however, it may be overridden by a child class to
		 *  perform additional initialization tasks and handle
		 *  some set of parameters. */
		sqlrquerytranslation(sqlrservercontroller *cont,
					domnode *parameters);

		/** Deletes this instance of sqlrtranslation. */
		virtual	~sqlrquerytranslation();

		/** Returns true if this implementation requires a query tree
		 *  and false if it does not.
		 *
		 *  This implementation returns false, but may be overridden by
		 *  a child class to return true. */
		virtual bool	requiresTree();

		/** Translates to "query" of "querysize" bytes and writes the
		 *  translated query to "translatedquery".
		 *
		 *  Returns true on success and false if an error occurred.
		 *
		 *  If an error occurred, then getError() may be used to get
		 *  the specific error.
		 *
		 *  This implementation returns true, but may be overridden by
		 *  a child class to perform specific translations. */
		virtual bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					const char *query,
					uint32_t querysize,
					stringbuffer *translatedquery);

		/** Translates "querytree" in-place.
		 *
		 *  Returns true on success and false if an error occurred.
		 *
		 *  If an error occurred, then getError() may be used to get
		 *  the specific error.
		 *
		 *  This implementation just returns true, but may be
		 *  overridden by a child class to perform specific
		 *  translations. */
		virtual bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					xmldom *querytree);

		/** Returns an error if the previous call to run() returned
 		 *  false, or NULL if the previous call to run() succeeded or
 		 *  if run() was never called. */
		virtual const char	*getError();

	#include <sqlrelay/private/sqlrquerytranslation.h>
};

class SQLRSERVER_DLLSPEC sqlrfilter : public sqlrservermodule {
	public:
		/** Creates an instance of sqlrqueryfilter, configured with
		 *  parameters "parameters".
		 *
		 *  This implementation doesn't handle any parameters,
		 *  however, it may be overridden by a child class to
		 *  perform additional initialization tasks and handle
		 *  some set of parameters. */
		sqlrfilter(sqlrservercontroller *cont,
					domnode *parameters);

		/** Deletes this instance of sqlrtranslation. */
		virtual	~sqlrfilter();

		/** Returns true if this implementation requires a query tree
		 *  and false if it does not.
		 *
		 *  This implementation returns false, but may be overridden by
		 *  a child class to return true. */
		virtual bool	requiresTree();

		/** Filters "query".
		 *
		 *  Returns true if the query should be filtered out and false
		 *  if the query should not be filtered out.  May optionally
		 *  set an error indicating why the query was filtered out.
		 *
		 *  If an error occurred, then getError() may be used to get
		 *  the specific error.
		 *
		 *  This implementation just returns true, but may be
		 *  overridden by a child class to perform specific
		 *  filtering tasks. */
		virtual bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					const char *query);


		/** Filters the query represented by "querytree".
		 *
		 *  Returns true if the query should be filtered out and false
		 *  if the query should not be filtered out.  May optionally
		 *  set an error indicating why the query was filtered out.
		 *
		 *  If an error occurred, then getError() may be used to get
		 *  the specific error.
		 *
		 *  This implementation just returns true, but may be
		 *  overridden by a child class to perform specific
		 *  filtering tasks. */
		virtual bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					xmldom *querytree);

		/** If run() filtered out the query and set an error indicating
 		 *  why, then "err" returns the error string and "errn" returns
 		 *  the error number. */
		virtual void	getError(const char **err, int64_t *errn);

	#include <sqlrelay/private/sqlrfilter.h>
};

class SQLRSERVER_DLLSPEC sqlrbindvariabletranslation : public sqlrservermodule {
	public:
		/** Creates an instance of sqlrbindvariabletranslation,
 		 *  configured with parameters "parameters".
		 *
		 *  This implementation doesn't handle any parameters,
		 *  however, it may be overridden by a child class to
		 *  perform additional initialization tasks and handle
		 *  some set of parameters. */
		sqlrbindvariabletranslation(sqlrservercontroller *cont,
					domnode *parameters);

		/** Deletes this instance of sqlrbindvariabletranslation. */
		virtual	~sqlrbindvariabletranslation();

		/** Translates the current bind variables of "sqlrcur".
		 *
		 *  Returns true on success and false if an error occurred.
		 *
		 *  This implementation just returns true, but may be
		 *  overridden by a child class to perform specific
		 *  translations. */
		virtual bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur);

		/** Returns an error if the previous call to run() returned
 		 *  false, or NULL if the previous call to run() succeeded or
 		 *  if run() was never called. */
		virtual const char	*getError();

	#include <sqlrelay/private/sqlrbindvariabletranslation.h>
};

class SQLRSERVER_DLLSPEC sqlrresultsettranslation : public sqlrservermodule {
	public:
		/** Creates an instance of sqlrresultsettranslation,
 		 *  configured with parameters "parameters".
		 *
		 *  This implementation doesn't handle any parameters,
		 *  however, it may be overridden by a child class to
		 *  perform additional initialization tasks and handle
		 *  some set of parameters. */
		sqlrresultsettranslation(sqlrservercontroller *cont,
						domnode *parameters);

		/** Deletes this instance of sqlrresultsettranslation. */
		virtual	~sqlrresultsettranslation();

		/** Translates the field of the column at index "fieldindex"
		 *  of the current row of the current result set of "sqlrcur".
		 *  "fieldname" will also be set to the column name of that
		 *  field.  Returns the translated field in "field", and
		 *  returns the translated field size in "fieldsize".
		 *
		 *  Returns true on success and false if an error occurred.
		 *
		 *  This implementation just returns true, but may be
		 *  overridden by a child class to perform specific
		 *  translations. */
		virtual bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					const char *fieldname,
					uint32_t fieldindex,
					const char **field,
					uint64_t *fieldsize);

		/** Returns an error if the previous call to run() returned
 		 *  false, or NULL if the previous call to run() succeeded or
 		 *  if run() was never called. */
		virtual const char	*getError();

	#include <sqlrelay/private/sqlrresultsettranslation.h>
};

class SQLRSERVER_DLLSPEC sqlrresultsetrowtranslation : public sqlrservermodule {
	public:
		/** Creates an instance of sqlrresultsetrowtranslation,
 		 *  configured with parameters "parameters".
		 *
		 *  This implementation doesn't handle any parameters,
		 *  however, it may be overridden by a child class to
		 *  perform additional initialization tasks and handle
		 *  some set of parameters. */
		sqlrresultsetrowtranslation(
					sqlrservercontroller *cont,
					domnode *parameters);

		/** Deletes this instance of sqlrresultsetrowtranslation. */
		virtual	~sqlrresultsetrowtranslation();

		/** Translates all fields of the current row of the current
		 *  result set of "sqlrcur".  "colcount" will be set to the
		 *  number of columns in the result set, and "fieldnames" will
		 *  be set to the column names.  Returns the translated fields
		 *  in "fields", and returns the translated field sizes in
		 *  "fieldsizes".
		 *
		 *  Returns true on success and false if an error occurred.
		 *
		 *  This implementation just returns true, but may be
		 *  overridden by a child class to perform specific
		 *  translations. */
		virtual bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					uint32_t colcount,
					const char * const *fieldnames,
					const char ***fields,
					uint64_t **fieldsizes);

		/** Returns an error if the previous call to run() returned
 		 *  false, or NULL if the previous call to run() succeeded or
 		 *  if run() was never called. */
		virtual const char	*getError();

	#include <sqlrelay/private/sqlrresultsetrowtranslation.h>
};

class SQLRSERVER_DLLSPEC sqlrresultsetrowblocktranslation :
						public sqlrservermodule {
	public:
		/** Creates an instance of sqlrresultsetrowblocktranslation,
 		 *  configured with parameters "parameters".
		 *
		 *  This implementation handles the following parameter
		 *  which is generic to all result set row block translation
		 *  implementations:
		 *
		 *  * rowblockcount - the number of rows that each block
		 *                    consists of, defaults to 10 if not
		 *                    specified
		 *
		 *  Note that rowblockcont is an attribute of the parent
		 *  resultestrowblocktranslations tag, not of individual
		 *  resultsetrowblocktranslation tags.
		 *
		 *  However, it may be overridden by a child class to perform
		 *  additional initialization tasks and handle additional
		 *  parameters. */
		sqlrresultsetrowblocktranslation(
					sqlrservercontroller *cont,
					domnode *parameters);

		/** Deletes this instance of
		 *  sqlrresultsetrowblocktranslation. */
		virtual	~sqlrresultsetrowblocktranslation();

		/** Returns rowblockcount as set in the parameters. */
		uint64_t	getRowBlockCount();

		/** Sets a row to be translated.  The row will be the current
		 *  row of the current result set of "sqlrcur".
		 *
		 *  setRow() will be called several times to define a block of
		 *  rows to be translated, then run() will be called to
		 *  translate the rows, then getRow() will be called several
		 *  times to fetch the translated rows.
		 *
		 *  "colcount" will be set to the number of columns in the
		 *  result set, and "fieldnames" will be set to the column
		 *  names.  "fields" will contain the field values,
		 *  "fieldsizes" will contain the number of bytes in each
		 *  field, "lobs" will contain true or false for each field,
		 *  indicating whether it is a lob or not, and "nulls" will
		 *  contain true or false for each field, indicating whether it
		 *  is null or not.
		 *
		 *  Returns true on success and false if an error occurred.
		 *
		 *  This implementation just returns true, but may be
		 *  overridden by a child class to perform specific tasks to
		 *  keep track of the row. */
		virtual bool	setRow(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					uint32_t colcount,
					const char * const *fieldnames,
					const char * const *fields,
					uint64_t *fieldsizes,
					bool *lobs,
					bool *nulls);

		/** Translates the rows set by previous calls to setRow().
		 *
		 *  "colcount" will be set to the number of columns in the
		 *  result set, and "fieldnames" will be set to the column
		 *  names.
		 *
		 *  setRow() will be called several times to define a block of
		 *  rows to be translated, then run() will be called to
		 *  translate the rows, then getRow() will be called several
		 *  times to fetch the translated rows.
		 *
		 *  Returns true on success and false if an error occurred.
		 *
		 *  This implementation just returns true, but may be
		 *  overridden by a child class to perform specific
		 *  translations. */
		virtual bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					uint32_t colcount,
					const char * const *fieldnames);

		/** Returns the next translated row in the block of rows that
		 *  were translated by the most recent call to run().
		 *
		 *  "colcount" will be set to the number of columns in the
		 *  result set, and "fieldnames" will be set to the column
		 *  names.
		 *
		 *  Returns the translated fields in "fields", the translated
		 *  field sizes in "fieldsizes", whether each column is a lob
		 *  or not in "lobs", and whether each column is null or not in
		 *  "nulls".
		 *
		 *  Returns true on success and false if an error occurred.
		 *
		 *  This implementation just returns true, but may be
		 *  overridden by a child class to perform specific tasks to
		 *  return the translated row. */
		virtual bool	getRow(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					uint32_t colcount,
					const char ***fields,
					uint64_t **fieldsizes,
					bool **lobs,
					bool **nulls);

		/** Returns an error if the previous call to run() or getRow()
		 *  returned false, or NULL if the previous call to run()
		 *  succeeded or if run() was never called. */
		virtual const char	*getError();

	#include <sqlrelay/private/sqlrresultsetrowblocktranslation.h>
};

class SQLRSERVER_DLLSPEC sqlrresultsetheadertranslation :
						public sqlrservermodule {
	public:
		/** Creates an instance of sqlrresultsetheadertranslation,
 		 *  configured with parameters "parameters".
		 *
		 *  This implementation doesn't handle any parameters,
		 *  however, it may be overridden by a child class to
		 *  perform additional initialization tasks and handle
		 *  some set of parameters. */
		sqlrresultsetheadertranslation(
					sqlrservercontroller *cont,
					domnode *parameters);

		/** Deletes this instance of sqlrresultsetheadertranslation. */
		virtual	~sqlrresultsetheadertranslation();

		/** Translates the header of the current result set of
		 *  "sqlrcur".
		 *  
		 *  "colcount" will be set to the number of columns in the
		 *  result set.
		 *
		 *  "columnnames" will be set to the current column names and
		 *  will return the set of translated column names.
		 *
		 *  "columnnamesizes" will be set to the number of bytes in each
		 *  of the current column names and will return the number of
		 *  bytes in each of the translated column name.
		 *
		 *  "columntypes" will be set to the current column types and
		 *  will return the set of translated column types.
		 *
		 *  "columntypenames" will be set to the current column type
		 *  names and will return the set of translated column type
		 *  names.
		 *
		 *  "columntypenamesizes" will be set to the number of bytes in
		 *  each of the current column type names and will return the
		 *  number of bytes in each of the translated column type name.
		 *
		 *  "columnsizes" will be set to the current column sizes and
		 *  will return the set of translated column sizes.
		 *
		 *  "columnprecisions" will be set to the current column
		 *  precisions and will return the set of translated column
		 *  precisions.
		 *
		 *  "columnscales" will be set to the current column scales and
		 *  will return the set of translated column scales.
		 *
		 *  "columnisnullables" will be set to the current is-nullables
		 *  flags and will return the set of translated is-nullable
		 *  flags.
		 *
		 *  "columnisprimarykeys" will be set to the current
		 *  is-primary-key flags and will return the set of translated
		 *  is-primary-key flags.
		 *
		 *  "columnisuniques" will be set to the current is-unique-key
		 *  flags and will return the set of translated is-unique-key
		 *  flags.
		 *
		 *  "columnispartofkeys" will be set to the current
		 *  is-part-of-key flags and will return the set of translated
		 *  is-part-of-key flags.
		 *
		 *  "columnisunsigneds" will be set to the current
		 *  is-unsigned flags and will return the set of translated
		 *  is-unsigned flags.
		 *
		 *  "columniszerofilled" will be set to the current
		 *  is-zero-filled flags and will return the set of translated
		 *  is-zero-filled flags.
		 *
		 *  "columnisbinary" will be set to the current
		 *  is-binary flags and will return the set of translated
		 *  is-binary flags.
		 *
		 *  "columnisautoincrements" will be set to the current
		 *  is-auto-increment flags and will return the set of
		 *  translated is-auto-increment flags.
		 *
		 *  "columntablenames" will be set to the current column table
		 *  names and  will return the set of translated column table
		 *  names.
		 *
		 *  "columntablenamesizes" will be set to the number of bytes
		 *  in each of the current column table names and will return
		 *  the number of bytes in each of the translated column table
		 *  name.
		 *
		 *  Returns true on success and false if an error occurred.
		 *
		 *  This implementation just returns true, but may be
		 *  overridden by a child class to perform specific
		 *  translation tasks. */
		virtual bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					uint32_t colcount,
					const char ***columnnames,
					uint16_t **columnnamesizes,
					uint16_t **columntypes,
					const char ***columntypenames,
					uint16_t **columntypenamesizes,
					uint32_t **columnsizes,
					uint32_t **columnprecisions,
					uint32_t **columnscales,
					uint16_t **columnisnullables,
					uint16_t **columnisprimarykeys,
					uint16_t **columnisuniques,
					uint16_t **columnispartofkeys,
					uint16_t **columnisunsigneds,
					uint16_t **columniszerofilleds,
					uint16_t **columnisbinarys,
					uint16_t **columnisautoincrements,
					const char ***columntables,
					uint16_t **columntablesizes);

		/** Returns an error if the previous call to run() returned
		 *  false, or NULL if the previous call to run() succeeded or
		 *  if run() was never called. */
		virtual const char	*getError();

	#include <sqlrelay/private/sqlrresultsetheadertranslation.h>
};

class SQLRSERVER_DLLSPEC sqlrerrortranslation : public sqlrservermodule {
	public:
		/** Creates an instance of sqlrerrortranslation,
 		 *  configured with parameters "parameters".
		 *
		 *  This implementation doesn't handle any parameters,
		 *  however, it may be overridden by a child class to
		 *  perform additional initialization tasks and handle
		 *  some set of parameters. */
		sqlrerrortranslation(sqlrservercontroller *cont,
					domnode *parameters);

		/** Deletes this instance of sqlrerrortranslation. */
		virtual	~sqlrerrortranslation();

		/** Translates "error" of "errorsize" bytes and "errornumber".
		 *  Returns the translated error in "translatederror" and the
		 *  translated error number in "translatederrornumber".
		 *
		 *  Returns true on success and false if an error occurred.
		 *
		 *  This implementation just returns true, but may be
		 *  overridden by a child class to perform specific
		 *  translation tasks. */
		virtual bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					int64_t errornumber,
					const char *error,
					uint32_t errorsize,
					int64_t *translatederrornumber,
					stringbuffer *translatederror);

		/** Returns an error if the previous call to run() returned
		 *  false, or NULL if the previous call to run() succeeded or
		 *  if run() was never called. */
		virtual const char	*getError();

	#include <sqlrelay/private/sqlrerrortranslation.h>
};

class SQLRSERVER_DLLSPEC sqlrtrigger : public sqlrservermodule {
	public:
		/** Creates an instance of sqlrtrigger, configured with
		 *  parameters "parameters".
		 *
		 *  This implementation doesn't handle any parameters,
		 *  however, it may be overridden by a child class to
		 *  perform additional initialization tasks and handle
		 *  some set of parameters. */
		sqlrtrigger(sqlrservercontroller *cont,
					domnode *parameters);

		/** Deletes this instance of sqlrtrigger. */
		virtual	~sqlrtrigger();

		/** Runs before execution of the query.
		 *
		 *  Returns true on success and false if an error occurred.
		 *
		 *  This implementation just returns true, but may be
		 *  overridden by a child class to perform specific tasks. */
		virtual bool	runBefore(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur);

		/** Runs after execution of the query.
		 *
		 *  Returns true on success and false if an error occurred.
		 *
		 *  This implementation just returns true, but may be
		 *  overridden by a child class to perform specific tasks. */
		virtual bool	runAfter(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur);

	#include <sqlrelay/private/sqlrtrigger.h>
};

class SQLRSERVER_DLLSPEC sqlrquery : public sqlrservermodule {
	public:
		/** Creates an instance of sqlrquery, configured with
		 *  parameters "parameters".
		 *
		 *  This implementation doesn't handle any parameters,
		 *  however, it may be overridden by a child class to
		 *  perform additional initialization tasks and handle
		 *  some set of parameters. */
		sqlrquery(sqlrservercontroller *cont, domnode *parameters);

		/** Deletes this instance of sqlrquery. */
		virtual	~sqlrquery();

		/** Returns true if "querysize" bytes of "querystring" match
		 *  critera for handling this query and false otherwise.
		 *
		 *  This implementation just returns false, but may be
		 *  overridden by a child class to do specfic query
		 *  matching. */
		virtual bool	match(const char *querystring,
						uint32_t querysize);

		/** Returns a new sqlrquerycursor to handle this query, and
		 *  assigns it an id of "id".
		 *
		 *  This implementation just returns NULL, but may be
		 *  overridden by a child class to allocate and return a
		 *  specific child of sqlrquerycursor. */
		virtual sqlrquerycursor	*newCursor(	
						sqlrserverconnection *sqlrcon,
						uint16_t id);

	#include <sqlrelay/private/sqlrquery.h>
};

class SQLRSERVER_DLLSPEC sqlrquerycursor : public sqlrservercursor {
	public:
		/** Creates an instance of sqlrquerycursor, configured with
		 *  parameters "parameters" and an id of "id". */
		sqlrquerycursor(sqlrserverconnection *conn,
					sqlrquery *q,
					domnode *parameters,
					uint16_t id);

		/** Deletes this instance of sqlrquerycursor. */
		virtual	~sqlrquerycursor();

		/** Returns the query type of the first "size" bytes of "query".
		 * 
		 *  This implementation just returns SQLRQUERYTYPE_CUSTOM, but
		 *  may be overridden by a child class to do specfic query type
		 *  analysis. */
		virtual sqlrquerytype_t	determineQueryType(
						const char *query,
						uint32_t size);

		/** Returns true. */
		bool	isCustomQuery();

	protected:

		/** Returns the instance of sqlrquery passed in as "q" to the
		 *  constructor. */
		sqlrquery	*getQuery();

		/** Returns the top-level domnode of the parameters passed in
		 *  as "parameters" to the constructor. */
		domnode	*getParameters();

	#include <sqlrelay/private/sqlrquerycursor.h>
};

class SQLRSERVER_DLLSPEC sqlrmoduledata : public sqlrservermodule {
	public:
		/** Creates an instance of sqlrmoduledata, configured with
		 *  parameters "parameters".
		 *
		 *  This implementation handles the following parameters
		 *  which are generic to all module data implementations:
		 *
		 *  * module - the name of the module to load
		 *  * id - the id assigned to this instance of the module
		 *
		 *  However, it may be overridden by a child class to perform
		 *  additional initialization tasks and handle additional
		 *  parameters. */
		sqlrmoduledata(domnode *parameters);

		/** Deletes this instance of sqlrmoduledata. */
		virtual	~sqlrmoduledata();

		/** Returns the value of the module parameter. */
		const char	*getModuleType();

		/** Returns the value of the id parameter. */
		const char	*getId();

		/** Called by the sqlrservercontroller when the current result
		 *  set of "sqlrcur" is closed.
		 *
		 *  This implementation just returns, but may be overridden by
		 *  a child class to do additional things at transaction-end. */
		virtual void	closeResultSet(sqlrservercursor *sqlrcur);

	#include <sqlrelay/private/sqlrmoduledata.h>
};

class SQLRSERVER_DLLSPEC sqlrmoduledata_tag : public sqlrmoduledata {
	public:
		/** Creates an instance of sqlrmoduledata_tag, configured with
		 *  parameters "parameters". */
		sqlrmoduledata_tag(domnode *parameters);

		/** Deletes this instance of sqlrmoduledata_tag. */
		~sqlrmoduledata_tag();
		
		/** Tags the current query of "cursorid" (as returned by
		 *  sqlrservercursor::getId()) with "tag".
		 *
		 *  May be called multiple times to tag the query with multiple
		 *  tags. */
		void	addTag(uint16_t cursorid, const char *tag);
		
		/** Tags the current query of "cursorid" (as returned by
		 *  sqlrservercursor::getId()) with the first "size" bytes of
		 *  "tag".
		 *
		 *  May be called multiple times to tag the query with multiple
		 *  tags. */
		void	addTag(uint16_t cursorid, const char *tag, size_t size);

		/** Returns the set of tags for "cursorid" (as returned by
		 *  sqlrservercursor::getId()) as an avltree. */
		avltree<char *>	*getTags(uint16_t cursorid);

		/** Returns true if the current query of "cursorid" (as
		 *  returned by sqlrservercursor::getId()) has been tagged with
		 *  tag "tag" and false otherwise. */
		bool	tagExists(uint16_t cursorid, const char *tag);

		/** Clears any tags that were assigned to the current query of
		 *  "sqlrcur".
		 *
		 *  Called by the sqlrservercontroller when the current result
		 *  set of "sqlrcur" is closed. */
		void	closeResultSet(sqlrservercursor *sqlrcur);

	#include <sqlrelay/private/sqlrmoduledata_tag.h>
};

#endif
