// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information.

#ifndef SQLRCLIENT_H
#define SQLRCLIENT_H

#include <sqlrelay/private/sqlrincludes.h>

class sqlrconnection : public inetclientsocket, public unixclientsocket {
	public:
			sqlrconnection(const char *server, int port,
					const char *socket,
					const char *user, const char *password,
					int retrytime, int tries);
				// Initiates a connection to "server" on "port"
				// or to the unix "socket" on the local machine
				// and authenticates with "user" and "password".
				// Failed connections will be retried for 
				// "tries" times on interval "retrytime"
				// or on for a default number of times on
				// a default interval if left unspecified.

				// If the "socket" parameter is neither 
				// NULL nor "" then an attempt will be made to 
				// connect through it before attempting to 
				// connect to "server" on "port".  If it is 
				// NULL or "" then no attempt will be made to 
				// connect through the socket.
			~sqlrconnection();
				// Disconnects and ends the session if
				// it hasn't been ended already.

		int	endSession();
				// Ends the session.
		int	suspendSession();
				// Disconnects this connection from the current
				// session but leaves the session open so 
				// that another connection can connect to it 
				// using resumeSession().
		int	getConnectionPort();
				// Returns the inet port that the connection is 
				// communicating over. This parameter may be 
				// passed to another connection for use in
				// the resumeSession() method.
		char	*getConnectionSocket();
				// Returns the unix socket that the connection 
				// is communicating over. This parameter may be 
				// passed to another connection for use in
				// the resumeSession() method.
		int	resumeSession(int port, const char *socket);
				// Resumes a session previously left open 
				// using suspendSession().
				// Returns 1 on success and 0 on failure.


		int	ping();
				// Returns 1 if the database is up and 0
				// if it's down.
		char	*identify();
				// Returns the type of database: 
				//   oracle7, oracle8, postgresql, mysql, etc.


		int	autoCommitOn();
				// Instructs the database to perform a commit
				// after every successful query.
		int	autoCommitOff();
				// Instructs the database to wait for the 
				// client to tell it when to commit.
		int	commit();
				// Issues a commit.  Returns 1 if the commit
				// succeeded, 0 if it failed and -1 if an
				// error occurred.
		int	rollback();
				// Issues a rollback.  Returns 1 if the rollback
				// succeeded, 0 if it failed and -1 if an
				// error occurred.


		void	debugOn();
				// Causes verbose debugging information to be 
				// sent to standard output.  Another way to do
				// this is to start a query with "-- debug\n".
		void	debugOff();
				// Turns debugging off.
		int	getDebug();
				// Returns 0 if debugging is off and 1 if 
				// debugging is on.


		void	debugPrintFunction(int (*printfunction)
							(const char *,...));
				// Allows you to replace the function used
				// to print debug messages with your own
				// function.  The function is expected to take
				// arguments like printf().

	#include <sqlrelay/private/sqlrconnection.h>
};


class sqlrcursor {
	public:
			sqlrcursor(sqlrconnection *sqlrc);
			~sqlrcursor();

		void	setResultSetBufferSize(int rows);
				// Sets the number of rows of the result set
				// to buffer at a time.  0 (the default)
				// means buffer the entire result set.
		int	getResultSetBufferSize();
				// Returns the number of result set rows that 
				// will be buffered at a time or 0 for the
				// entire result set.


		void	dontGetColumnInfo();
				// Tells the server not to send any column
				// info (names, types, sizes).  If you don't
				// need that info, you should call this
				// method to improve performance.
		void	getColumnInfo();
				// Tells the server to send column info.

		void	mixedCaseColumnNames();
				// Columns names are returned in the same
				// case as they are defined in the database.
				// This is the default.
		void	upperCaseColumnNames();
				// Columns names are converted to upper case.
		void	lowerCaseColumnNames();
				// Columns names are converted to lower case.


		void	cacheToFile(const char *filename);
				// Sets query caching on.  Future queries
				// will be cached to the file "filename".
				//
				// A default time-to-live of 10 minutes is
				// also set.
				//
				// Note that once cacheToFile() is called,
				// the result sets of all future queries will
				// be cached to that file until another call 
				// to cacheToFile() changes which file to
				// cache to or a call to cacheOff() turns off
				// caching.
		void	setCacheTtl(int ttl);
				// Sets the time-to-live for cached result
				// sets. The sqlr-cachemanger will remove each 
				// cached result set "ttl" seconds after it's 
				// created, provided it's scanning the directory
				// containing the cache files.
		char	*getCacheFileName();
				// Returns the name of the file containing the
				// cached result set.
		void	cacheOff();
				// Sets query caching off.


		// If you don't need to use substitution or bind variables
		// in your queries, use these two methods.
		int	sendQuery(const char *query);
				// Sends "query" and gets a result set.
		int	sendQuery(const char *query, int length);
				// Sends "query" with length "length" and gets
				// a result set. This method must be used if
				// the query contains binary data.
		int	sendFileQuery(const char *path, const char *filename); 
				// Sends the query in file "path"/"filename" 
				// and gets a result set.


		// If you need to use substitution or bind variables, in your
		// queries use the following methods.  See the API 
		// documentation for more information about substitution and 
		// bind variables.
		void	prepareQuery(const char *query);
				// Prepare to execute "query".
		void	prepareQuery(const char *query, int length);
				// Prepare to execute "query" with length 
				// "length".  This method must be used if the
				// query contains binary data.
		int	prepareFileQuery(const char *path,
						const char *filename);
				// Prepare to execute the contents 
				// of "path"/"filename".  Returns 0 if the
				// file couldn't be opened.

		void	clearBinds();
				// Clear all bind variables.

		void	substitution(const char *variable, const char *value);
		void	substitution(const char *variable, long value);
		void	substitution(const char *variable, double value, 
					unsigned short precision, 
					unsigned short scale);
				// Define a substitution variable.

		void	inputBind(const char *variable, const char *value);
		void	inputBind(const char *variable, long value);
		void	inputBind(const char *variable, double value, 
					unsigned short precision, 
					unsigned short scale);
		void	inputBindBlob(const char *variable, const char *value,
						unsigned long size);
		void	inputBindClob(const char *variable, const char *value,
						unsigned long size);
				// Define an input bind variable.

		void	defineOutputBind(const char *variable,
					unsigned long bufferlength);
				// Define an output bind variable.
				// "bufferlength" bytes will be reserved
				// to store the value.
		void	defineOutputBindBlob(const char *variable);
				// Define a BLOB output bind variable.
		void	defineOutputBindClob(const char *variable);
				// Define a CLOB output bind variable.
		void	defineOutputBindCursor(const char *variable);
				// Define a cursor output bind variable.

		void	substitutions(const char **variables,
						const char **values);
		void	substitutions(const char **variables,
						const long *values);
		void	substitutions(const char **variables,
					const double *values,
					const unsigned short *precisions, 
					const unsigned short *scales);
				// Define an array of substitution variables.
		void	inputBinds(const char **variables, const char **values);
		void	inputBinds(const char **variables,
					const unsigned long *values);
		void	inputBinds(const char **variables,
					const double *values, 
					const unsigned short *precisions, 
					const unsigned short *scales);
				// Define an array of input bind variables.

		void	validateBinds();
				// If you are binding to any variables that 
				// might not actually be in your query, call 
				// this to ensure that the database won't try 
				// to bind them unless they really are in the 
				// query.  There is a performance penalty for
				// calling this method.

		int	executeQuery();
				// Execute the query that was previously 
				// prepared and bound.

		int	fetchFromBindCursor();
				// Fetch from a cursor that was returned as
				// an output bind variable.

		char	*getOutputBind(const char *variable);
				// Get the value stored in a previously
				// defined output bind variable.
		long	getOutputBindLength(const char *variable);
				// Get the length of the value stored in a
				// previously defined output bind variable.
		sqlrcursor	*getOutputBindCursor(const char *variable);
				// Get the cursor associated with a previously
				// defined output bind variable.

		
		int	openCachedResultSet(const char *filename);
				// Opens a cached result set.
				// Returns 1 on success and 0 on failure.

		int	colCount();
				// Returns the number of columns in the current
				// result set.
		int	rowCount();
				// Returns the number of rows in the current 
				// result set (if the result set is being
				// stepped through, this returns the number
				// of rows processed so far).
		int	totalRows();
				// Returns the total number of rows that will 
				// be returned in the result set.  Not all 
				// databases support this call.  Don't use it 
				// for applications which are designed to be 
				// portable across databases.  -1 is returned
				// by databases which don't support this option.
		int	affectedRows();
				// Returns the number of rows that were 
				// updated, inserted or deleted by the query.
				// Not all databases support this call.  Don't 
				// use it for applications which are designed 
				// to be portable across databases.  -1 is 
				// returned by databases which don't support 
				// this option.
		int	firstRowIndex();
				// Returns the index of the first buffered row.
				// This is useful when buffering only part of
				// the result set at a time.
		int	endOfResultSet();
				// Returns 0 if part of the result set is still
				// pending on the server and 1 if not.  This
				// method can only return 0 if 
				// setResultSetBufferSize() has been called
				// with a parameter other than 0.
		
		
		char	*errorMessage();
				// If a query failed and generated an error, 
				// the error message is available here.  If 
				// the query succeeded then this method 
				// returns a NULL.


		void	getNullsAsEmptyStrings();
				// Tells the connection to return NULL fields
				// and output bind variables as empty strings. 
				// This is the default.
		void	getNullsAsNulls();
				// Tells the connection to return NULL fields
				// and output bind variables as NULL's rather
				// than as empty strings.


		char	*getField(int row, int col);
		char	*getField(int row, const char *col);
				// Returns a pointer to the value of the 
				// specified row and column.
		long	getFieldLength(int row, int col);
		long	getFieldLength(int row, const char *col);
				// Returns the length of the 
				// specified row and column.
		char	**getRow(int row);
				// Returns a null terminated array of the 
				// values of the fields in the specified row.
		long	*getRowLengths(int row);
				// Returns a null terminated array of the 
				// lengths of the fields in the specified row.
		char	**getColumnNames();
				// Returns a null terminated array of the 
				// column names of the current result set.
		char	*getColumnName(int col);
				// Returns the name of the specified column.
		char	*getColumnType(int col);
		char	*getColumnType(const char *col);
				// Returns the type of the specified column.
		int	getColumnLength(int col);
		int	getColumnLength(const char *col);
				// Returns the length of the specified column.
		int	getLongest(int col);
		int	getLongest(const char *col);
				// Returns the length of the longest field
				// in the specified column.


		int	getResultSetId();
				// Returns the internal ID of this result set.
				// This parameter may be passed to another 
				// cursor for use in the resumeResultSet() 
				// method.
		void	suspendResultSet();
				// Tells the server to leave this result
				// set open when the connection calls 
				// suspendSession() so that another connection 
				// can connect to it using resumeResultSet() 
				// after it calls resumeSession().
		int	resumeResultSet(int id);
				// Resumes a result set previously left open 
				// using suspendSession().
				// Returns 1 on success and 0 on failure.
		int	resumeCachedResultSet(int id, const char *filename);
				// Resumes a result set previously left open
				// using suspendSession() and continues caching
				// the result set to "filename".
				// Returns 1 on success and 0 on failure.

	#include <sqlrelay/private/sqlrcursor.h>
};

#endif
