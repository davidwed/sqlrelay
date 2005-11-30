// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information.

#ifndef SQLRCLIENT_H
#define SQLRCLIENT_H

#include <sqlrelay/private/sqlrincludes.h>

#ifdef RUDIMENTS_NAMESPACE
using namespace rudiments;
#endif

class sqlrconnection {
	public:
			sqlrconnection(const char *server, uint16_t port,
					const char *socket,
					const char *user, const char *password,
					int32_t retrytime, int32_t tries);
				// Initiates a connection to "server" on "port"
				// or to the unix "socket" on the local machine
				// and authenticates with "user" and "password".
				// Failed connections will be retried for 
				// "tries" times on interval "retrytime".
				// If "tries" is 0 then retries will continue
				// forever.  If "retrytime" is 0 then retries
				// will be attempted on a default interval.

				// If the "socket" parameter is neither 
				// NULL nor "" then an attempt will be made to 
				// connect through it before attempting to 
				// connect to "server" on "port".  If it is 
				// NULL or "" then no attempt will be made to 
				// connect through the socket.
			~sqlrconnection();
				// Disconnects and ends the session if
				// it hasn't been ended already.

		void	endSession();
				// Ends the session.
		bool	suspendSession();
				// Disconnects this connection from the current
				// session but leaves the session open so 
				// that another connection can connect to it 
				// using resumeSession().
		uint16_t	getConnectionPort();
				// Returns the inet port that the connection is 
				// communicating over. This parameter may be 
				// passed to another connection for use in
				// the resumeSession() method.
				// Note: The value this method returns is only
				// valid after a call to suspendSession().
		const char	*getConnectionSocket();
				// Returns the unix socket that the connection 
				// is communicating over. This parameter may be 
				// passed to another connection for use in
				// the resumeSession() method.
				// Note: The value this method returns is only
				// valid after a call to suspendSession().
		bool	resumeSession(uint16_t port, const char *socket);
				// Resumes a session previously left open 
				// using suspendSession().
				// Returns true on success and false on failure.


		bool	ping();
				// Returns true if the database is up and false
				// if it's down.
		const char	*identify();
				// Returns the type of database: 
				//   oracle7, oracle8, postgresql, mysql, etc.


		bool	autoCommitOn();
				// Instructs the database to perform a commit
				// after every successful query.
		bool	autoCommitOff();
				// Instructs the database to wait for the 
				// client to tell it when to commit.
		bool	commit();
				// Issues a commit.  Returns 1 if the commit
				// succeeded, 0 if it failed.
		bool	rollback();
				// Issues a rollback.  Returns 1 if the rollback
				// succeeded, 0 if it failed.


		void	debugOn();
				// Causes verbose debugging information to be 
				// sent to standard output.  Another way to do
				// this is to start a query with "-- debug\n".
		void	debugOff();
				// Turns debugging off.
		bool	getDebug();
				// Returns false if debugging is off and true
				// if debugging is on.


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

		void	setResultSetBufferSize(uint64_t rows);
				// Sets the number of rows of the result set
				// to buffer at a time.  0 (the default)
				// means buffer the entire result set.
		uint64_t	getResultSetBufferSize();
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
		void	setCacheTtl(uint32_t ttl);
				// Sets the time-to-live for cached result
				// sets. The sqlr-cachemanger will remove each 
				// cached result set "ttl" seconds after it's 
				// created, provided it's scanning the directory
				// containing the cache files.
		const char	*getCacheFileName();
				// Returns the name of the file containing the
				// cached result set.
		void	cacheOff();
				// Sets query caching off.


		// If you don't need to use substitution or bind variables
		// in your queries, use these two methods.
		bool	sendQuery(const char *query);
				// Sends "query" and gets a result set.
		bool	sendQuery(const char *query, uint32_t length);
				// Sends "query" with length "length" and gets
				// a result set. This method must be used if
				// the query contains binary data.
		bool	sendFileQuery(const char *path, const char *filename); 
				// Sends the query in file "path"/"filename" 
				// and gets a result set.


		// If you need to use substitution or bind variables, in your
		// queries use the following methods.  See the API 
		// documentation for more information about substitution and 
		// bind variables.
		void	prepareQuery(const char *query);
				// Prepare to execute "query".
		void	prepareQuery(const char *query, uint32_t length);
				// Prepare to execute "query" with length 
				// "length".  This method must be used if the
				// query contains binary data.
		bool	prepareFileQuery(const char *path,
						const char *filename);
				// Prepare to execute the contents 
				// of "path"/"filename".  Returns false if the
				// file couldn't be opened.

		void	clearBinds();
				// Clear all bind variables.

		uint16_t	countBindVariables() const;
				// Parses the previously prepared query,
				// counts the number of bind variables defined
				// in it and returns that number.

		void	substitution(const char *variable, const char *value);
		void	substitution(const char *variable, int64_t value);
		void	substitution(const char *variable, double value, 
							uint32_t precision, 
							uint32_t scale);
				// Define a substitution variable.

		void	inputBind(const char *variable, const char *value);
		void	inputBind(const char *variable, int64_t value);
		void	inputBind(const char *variable, double value, 
							uint32_t precision, 
							uint32_t scale);
		void	inputBindBlob(const char *variable,
						const char *value,
						uint32_t size);
		void	inputBindClob(const char *variable,
						const char *value,
						uint32_t size);
				// Define an input bind variable.

		void	defineOutputBindString(const char *variable,
						uint32_t bufferlength);
				// Define an output bind variable.
				// "bufferlength" bytes will be reserved
				// to store the value.
		void	defineOutputBindInteger(const char *variable);
				// Define an integer output bind variable.
		void	defineOutputBindDouble(const char *variable);
				// Define a double precision floating
				// point output bind variable.
		void	defineOutputBindBlob(const char *variable);
				// Define a BLOB output bind variable.
		void	defineOutputBindClob(const char *variable);
				// Define a CLOB output bind variable.
		void	defineOutputBindCursor(const char *variable);
				// Define a cursor output bind variable.

		void	substitutions(const char **variables,
						const char **values);
		void	substitutions(const char **variables,
						const int64_t *values);
		void	substitutions(const char **variables,
					const double *values,
					const uint32_t *precisions, 
					const uint32_t *scales);
				// Define an array of substitution variables.
		void	inputBinds(const char **variables, const char **values);
		void	inputBinds(const char **variables,
					const int64_t *values);
		void	inputBinds(const char **variables,
					const double *values, 
					const uint32_t *precisions, 
					const uint32_t *scales);
				// Define an array of input bind variables.

		void	validateBinds();
				// If you are binding to any variables that 
				// might not actually be in your query, call 
				// this to ensure that the database won't try 
				// to bind them unless they really are in the 
				// query.  There is a performance penalty for
				// calling this method.

		bool	executeQuery();
				// Execute the query that was previously 
				// prepared and bound.

		bool	fetchFromBindCursor();
				// Fetch from a cursor that was returned as
				// an output bind variable.

		const char	*getOutputBindString(const char *variable);
				// Get the value stored in a previously
				// defined output bind variable.
		int64_t		getOutputBindInteger(const char *variable);
				// Get the value stored in a previously
				// defined output bind variable as a long
				// integer.
		double		getOutputBindDouble(const char *variable);
				// Get the value stored in a previously
				// defined output bind variable as a double
				// precision floating point number.
		uint32_t	getOutputBindLength(const char *variable);
				// Get the length of the value stored in a
				// previously defined output bind variable.
		sqlrcursor	*getOutputBindCursor(const char *variable);
				// Get the cursor associated with a previously
				// defined output bind variable.

		
		bool	openCachedResultSet(const char *filename);
				// Opens a cached result set.
				// Returns true on success and false on failure.

		uint32_t	colCount();
				// Returns the number of columns in the current
				// result set.
		uint64_t	rowCount();
				// Returns the number of rows in the current 
				// result set (if the result set is being
				// stepped through, this returns the number
				// of rows processed so far).
		uint64_t	totalRows();
				// Returns the total number of rows that will 
				// be returned in the result set.  Not all 
				// databases support this call.  Don't use it 
				// for applications which are designed to be 
				// portable across databases.  0 is returned
				// by databases which don't support this option.
		uint64_t	affectedRows();
				// Returns the number of rows that were 
				// updated, inserted or deleted by the query.
				// Not all databases support this call.  Don't 
				// use it for applications which are designed 
				// to be portable across databases.  0 is 
				// returned by databases which don't support 
				// this option.
		uint64_t	firstRowIndex();
				// Returns the index of the first buffered row.
				// This is useful when buffering only part of
				// the result set at a time.
		bool	endOfResultSet();
				// Returns false if part of the result set is
				// still pending on the server and true if not.
				// This method can only return false if 
				// setResultSetBufferSize() has been called
				// with a parameter other than 0.
		
		
		const char	*errorMessage();
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


		const char	*getField(uint64_t row, uint32_t col);
		const char	*getField(uint64_t row, const char *col);
				// Returns a pointer to the value of the 
				// specified row and column.
		int64_t	getFieldAsInteger(uint64_t row, uint32_t col);
		int64_t	getFieldAsInteger(uint64_t row, const char *col);
				// Returns the specified field as a long
				// integer.
		double	getFieldAsDouble(uint64_t row, uint32_t col);
		double	getFieldAsDouble(uint64_t row, const char *col);
				// Returns the specified field as a double
				// precision floating point number.
		uint32_t	getFieldLength(uint64_t row, uint32_t col);
		uint32_t	getFieldLength(uint64_t row, const char *col);
				// Returns the length of the 
				// specified row and column.
		const char * const *getRow(uint64_t row);
				// Returns a null terminated array of the 
				// values of the fields in the specified row.
		uint32_t	*getRowLengths(uint64_t row);
				// Returns a null terminated array of the 
				// lengths of the fields in the specified row.
		const char * const *getColumnNames();
				// Returns a null terminated array of the 
				// column names of the current result set.
		const char	*getColumnName(uint32_t col);
				// Returns the name of the specified column.
		const char	*getColumnType(uint32_t col);
		const char	*getColumnType(const char *col);
				// Returns the type of the specified column.
		uint32_t	getColumnLength(uint32_t col);
		uint32_t	getColumnLength(const char *col);
				// Returns the number of bytes required on
				// the server to store the data.
		uint32_t	getColumnPrecision(uint32_t col);
		uint32_t	getColumnPrecision(const char *col);
				// Returns the precision of the specified
				// column.
				// Precision is the total number of digits in
				// a number.  eg: 123.45 has a precision of 5.
				// For non-numeric types, it's the number of
				// characters in the string.
		uint32_t	getColumnScale(uint32_t col);
		uint32_t	getColumnScale(const char *col);
				// Returns the scale of the specified column.
				// Scale is the total number of digits to the
				// right of the decimal point in a number.
				// eg: 123.45 has a scale of 2.
		bool		getColumnIsNullable(uint32_t col);
		bool		getColumnIsNullable(const char *col);
				// Returns 1 if the specified column can
				// contain nulls and 0 otherwise.
		bool		getColumnIsPrimaryKey(uint32_t col);
		bool		getColumnIsPrimaryKey(const char *col);
				// Returns 1 if the specified column is a
				// primary key and 0 otherwise.
		bool		getColumnIsUnique(uint32_t col);
		bool		getColumnIsUnique(const char *col);
				// Returns 1 if the specified column is
				// unique and 0 otherwise.
		bool		getColumnIsPartOfKey(uint32_t col);
		bool		getColumnIsPartOfKey(const char *col);
				// Returns 1 if the specified column is
				// part of a composite key and 0 otherwise.
		bool		getColumnIsUnsigned(uint32_t col);
		bool		getColumnIsUnsigned(const char *col);
				// Returns 1 if the specified column is
				// an unsigned number and 0 otherwise.
		bool		getColumnIsZeroFilled(uint32_t col);
		bool		getColumnIsZeroFilled(const char *col);
				// Returns 1 if the specified column was
				// created with the zero-fill flag and 0
				// otherwise.
		bool		getColumnIsBinary(uint32_t col);
		bool		getColumnIsBinary(const char *col);
				// Returns 1 if the specified column
				// contains binary data and 0
				// otherwise.
		bool		getColumnIsAutoIncrement(uint32_t col);
		bool		getColumnIsAutoIncrement(const char *col);
				// Returns 1 if the specified column
				// auto-increments and 0 otherwise.
		uint32_t	getLongest(uint32_t col);
		uint32_t	getLongest(const char *col);
				// Returns the length of the longest field
				// in the specified column.


		void	suspendResultSet();
				// Tells the server to leave this result
				// set open when the connection calls 
				// suspendSession() so that another connection 
				// can connect to it using resumeResultSet() 
				// after it calls resumeSession().
		uint16_t	getResultSetId();
				// Returns the internal ID of this result set.
				// This parameter may be passed to another 
				// cursor for use in the resumeResultSet() 
				// method.
				// Note: The value this method returns is only
				// valid after a call to suspendResultSet().
		bool	resumeResultSet(uint16_t id);
				// Resumes a result set previously left open 
				// using suspendSession().
				// Returns true on success and false on failure.
		bool	resumeCachedResultSet(uint16_t id,
						const char *filename);
				// Resumes a result set previously left open
				// using suspendSession() and continues caching
				// the result set to "filename".
				// Returns true on success and false on failure.

	#include <sqlrelay/private/sqlrcursor.h>
};

#endif
