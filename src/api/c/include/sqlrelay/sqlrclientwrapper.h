/* Copyright (c) 2000-2001  David Muse
 See the file COPYING for more information */

#ifndef SQLRCLIENTWRAPPER_H
#define SQLRCLIENTWRAPPER_H

#include <rudiments/private/inttypes.h>

/** @file
 *  @defgroup sqlrclientwrapper sqlrclientwrapper */

typedef	struct sqlrconnection *sqlrcon;
typedef	struct sqlrcursor *sqlrcur;

/** @ingroup sqlrclientwrapper
 *  Initiates a connection to "server" on "port" or to the unix "socket" on
 *  the local machine and authenticates with "user" and "password".  Failed
 *  connections will be retried for "tries" times on interval "retrytime".
 *  If "tries" is 0 then retries will continue forever.  If "retrytime" is 0
 *  then retries will be attempted on a default interval.
 *  If the "socket" parameter is nether NULL nor "" then an attempt will be
 *  made to connect through it before attempting to connect to "server" on
 *  "port".  If it is NULL or "" then no attempt will be made to connect
 *  through the socket.*/
sqlrcon	sqlrcon_alloc(const char *server, uint16_t port, const char *socket,
					const char *user, const char *password, 
					int32_t retrytime, int32_t tries);

/** @ingroup sqlrclientwrapper
 *  Disconnects and ends the session if it hasn't been terminated already. */
void	sqlrcon_free(sqlrcon sqlrconref);



/** @ingroup sqlrclientwrapper
 *  Sets the server connect timeout in seconds and milliseconds.
 *  Setting either parameter to -1 disables the timeout. */
void	sqlrcon_setTimeout(sqlrcon sqlrconref,
			int32_t timeoutsec, int32_t timeoutusec);

/** @ingroup sqlrclientwrapper
 *  Ends the session. */
void	sqlrcon_endSession(sqlrcon sqlrconref);

/** @ingroup sqlrclientwrapper
 *  Disconnects this connection from the current session but leaves the session
 *  open so that another connection can connect to it using
 *  sqlrcon_resumeSession(). */
int	sqlrcon_suspendSession(sqlrcon sqlrconref);

/** @ingroup sqlrclientwrapper
 *  Returns the inet port that the connection is communicating over.  This
 *  parameter may be passed to another connection for use in the
 *  sqlrcon_resumeSession() command.  Note: The result this function returns
 *  is only valid after a call to suspendSession().*/
uint16_t	sqlrcon_getConnectionPort(sqlrcon sqlrconref);

/** @ingroup sqlrclientwrapper
 *  Returns the unix socket that the connection is communicating over.  This
 *  parameter may be passed to another connection for use in the
 *  sqlrcon_resumeSession() command.  Note: The result this function returns
 *  is only valid after a call to suspendSession().*/
const char	*sqlrcon_getConnectionSocket(sqlrcon sqlrconref);

/** @ingroup sqlrclientwrapper
 *  Resumes a session previously left open using sqlrcon_suspendSession().
 *  Returns 1 on success and 0 on failure. */
int	sqlrcon_resumeSession(sqlrcon sqlrconref, uint16_t port,
							const char *socket);



/** @ingroup sqlrclientwrapper
 *  Returns 1 if the database is up and 0 if it's down. */
int	sqlrcon_ping(sqlrcon sqlrconref);

/** @ingroup sqlrclientwrapper
 *  Returns the type of database: oracle8, postgresql, mysql, etc. */
const char	*sqlrcon_identify(sqlrcon sqlrconref);

/** @ingroup sqlrclientwrapper
 *  Returns the version of the database */
const char	*sqlrcon_dbVersion(sqlrcon sqlrconref);

/** @ingroup sqlrclientwrapper
 *  Returns the version of the sqlrelay server software. */
const char	*sqlrcon_serverVersion(sqlrcon sqlrconref);

/** @ingroup sqlrclientwrapper
 *  Returns the version of the sqlrelay client software. */
const char	*sqlrcon_clientVersion(sqlrcon sqlrconref);

/** @ingroup sqlrclientwrapper
 *  Returns a string representing the format
 *  of the bind variables used in the db. */
const char	*sqlrcon_bindFormat(sqlrcon sqlrconref);



/** @ingroup sqlrclientwrapper
 *  Sets the current database/schema to "database" */
int	sqlrcon_selectDatabase(sqlrcon sqlrconref, const char *database);

/** @ingroup sqlrclientwrapper
 *  Returns the database/schema that is currently in use. */
const char	*sqlrcon_getCurrentDatabase(sqlrcon sqlrconref);



/** @ingroup sqlrclientwrapper
 *  Returns the value of the autoincrement column for the last insert */
uint64_t	sqlrcon_getLastInsertId(sqlrcon sqlrconref);



/** @ingroup sqlrclientwrapper
 *  Instructs the database to perform a commit after every successful query. */
int	sqlrcon_autoCommitOn(sqlrcon sqlrconref);

/** @ingroup sqlrclientwrapper
 *  Instructs the database to wait for the client to tell it when to commit. */
int	sqlrcon_autoCommitOff(sqlrcon sqlrconref);



/** @ingroup sqlrclientwrapper
 *  Issues a commit.  Returns 1 if the commit succeeded, 0 if it failed and -1
 *  if an error occurred. */
int	sqlrcon_commit(sqlrcon sqlrconref);

/** @ingroup sqlrclientwrapper
 *  Issues a rollback.  Returns 1 if the rollback succeeded, 0 if it failed
 *  and -1 if an error occurred. */
int	sqlrcon_rollback(sqlrcon sqlrconref);



/** @ingroup sqlrclientwrapper
 *  If an operation failed and generated an error, the error message is
 *  available here.  If there is no error then this method returns NULL */
const char	*sqlrcon_errorMessage(sqlrcon sqlrconref);



/** @ingroup sqlrclientwrapper
 *  Causes verbose debugging information to be sent to standard output.
 *  Another way to do this is to start a query with "-- debug\n". */
void	sqlrcon_debugOn(sqlrcon sqlrconref);

/** @ingroup sqlrclientwrapper
 *  Turns debugging off. */
void	sqlrcon_debugOff(sqlrcon sqlrconref);

/** @ingroup sqlrclientwrapper
 *  Returns 0 if debugging is off and 1 if debugging is on. */
int	sqlrcon_getDebug(sqlrcon sqlrconref);



/** @ingroup sqlrclientwrapper
 *  Allows you to replace the function used to print debug messages with your
 *  own function.  The function is expected to take arguments like printf. */
void	sqlrcon_debugPrintFunction(sqlrcon sqlrconref, 
					int (*printfunction)(const char *,...));



/** @ingroup sqlrclientwrapper
 *  Creates a cursor to run queries and fetch
 *  result sets using connection "sqlrconref" */
sqlrcur	sqlrcur_alloc(sqlrcon sqlrconref);

/** @ingroup sqlrclientwrapper
 *  Destroys the cursor and cleans up all associated result set data. *//
void	sqlrcur_free(sqlrcur sqlrcurref);



/** @ingroup sqlrclientwrapper
 *  Sets the number of rows of the result set to buffer at a time.
 *  0 (the default) means buffer the entire result set. */
void	sqlrcur_setResultSetBufferSize(sqlrcur sqlrcurref, uint64_t rows);

/** @ingroup sqlrclientwrapper
 *  Returns the number of result set rows that will be buffered at a time or
 *  0 for the entire result set. */
uint64_t	sqlrcur_getResultSetBufferSize(sqlrcur sqlrcurref);



/** @ingroup sqlrclientwrapper
 *  Tells the server not to send any column info (names, types, sizes).  If
 *  you don't need that info, you should call this function to improve
 *  performance. */
void	sqlrcur_dontGetColumnInfo(sqlrcur sqlrcurref);

/** @ingroup sqlrclientwrapper
 *  Tells the server to send column info. */
void	sqlrcur_getColumnInfo(sqlrcur sqlrcurref);



/** @ingroup sqlrclientwrapper
 *  Columns names are returned in the same case as they are defined in the
 *  database.  This is the default. */
void	sqlrcur_mixedCaseColumnNames(sqlrcur sqlrcurref);

/** @ingroup sqlrclientwrapper
 *  Columns names are converted to upper case. */
void	sqlrcur_upperCaseColumnNames(sqlrcur sqlrcurref);

/** @ingroup sqlrclientwrapper
 *  Columns names are converted to lower case. */
void	sqlrcur_lowerCaseColumnNames(sqlrcur sqlrcurref);



/** @ingroup sqlrclientwrapper
 *  Sets query caching on.  Future queries will be cached to the
 *  file "filename".
 * 
 *  A default time-to-live of 10 minutes is also set.
 * 
 *  Note that once sqlrcur_cacheToFile() is called, the result sets of all
 *  future queries will be cached to that file until another call to
 *  sqlrcur_cacheToFile() changes which file to cache to or a call to
 *  sqlrcur_cacheOff() turns off caching. */
void	sqlrcur_cacheToFile(sqlrcur sqlrcurref, const char *filename);

/** @ingroup sqlrclientwrapper
 *  Sets the time-to-live for cached result sets. The sqlr-cachemanger will
 *  remove each cached result set "ttl" seconds after it's created, provided
 *  it's scanning the directory containing the cache files. */
void	sqlrcur_setCacheTtl(sqlrcur sqlrcurref, uint32_t ttl);

/** @ingroup sqlrclientwrapper
 *  Returns the name of the file containing
 *  the most recently cached result set. */
const char	*sqlrcur_getCacheFileName(sqlrcur sqlrcurref);

/** @ingroup sqlrclientwrapper
 *  Sets query caching off. */
void	sqlrcur_cacheOff(sqlrcur sqlrcurref);



/** @ingroup sqlrclientwrapper
 *  Sends a query that returns a list of databases/schemas matching "wild".
 *  If wild is empty or NULL then a list of all databases/schemas will be
 *  returned. */
int	sqlrcur_getDatabaseList(sqlrcur sqlrcurref, const char *wild);

/** @ingroup sqlrclientwrapper
 *  Sends a query that returns a list of tables matching "wild".  If wild is
 *  empty or NULL then a list of all tables will be returned. */
int	sqlrcur_getTableList(sqlrcur sqlrcurref, const char *wild);

/** @ingroup sqlrclientwrapper
 *  Sends a query that returns a list of columns in the table specified by the
 *  "table" parameter matching "wild".  If wild is empty or NULL then a list of
 *  all columns will be returned. */
int	sqlrcur_getColumnList(sqlrcur sqlrcurref,
				const char *table, const char *wild);



/** @ingroup sqlrclientwrapper
 *  Sends "query" directly and gets a result set. */
int	sqlrcur_sendQuery(sqlrcur sqlrcurref, const char *query);

/** @ingroup sqlrclientwrapper
 *  Sends "query" with length "length" directly and gets a result set. This
 *  function must be used if the query contains binary data. */
int	sqlrcur_sendQueryWithLength(sqlrcur sqlrcurref, const char *query,
							uint32_t length);

/** @ingroup sqlrclientwrapper
 *  Sends the query in file "path"/"filename" and gets a result set. */
int	sqlrcur_sendFileQuery(sqlrcur sqlrcurref,
				const char *path, const char *filename);



/** @ingroup sqlrclientwrapper
 *  Prepare to execute "query". */
void	sqlrcur_prepareQuery(sqlrcur sqlrcurref, const char *query);

/** @ingroup sqlrclientwrapper
 *  Prepare to execute "query" with length "length".  This function must be
 *  used if the query contains binary data. */
void	sqlrcur_prepareQueryWithLength(sqlrcur sqlrcurref,
						const char *query,
						uint32_t length);

/** @ingroup sqlrclientwrapper
 *  Prepare to execute the contents of "path"/"filename". */
void	sqlrcur_prepareFileQuery(sqlrcur sqlrcurref, 
					const char *path, const char *filename);



/** @ingroup sqlrclientwrapper
 *  Defines a string substitution variable. */
void	sqlrcur_subString(sqlrcur sqlrcurref,
				const char *variable, const char *value);

/** @ingroup sqlrclientwrapper
 *  Defines a integer substitution variable. */
void	sqlrcur_subLong(sqlrcur sqlrcurref,
				const char *variable, int64_t value);

/** @ingroup sqlrclientwrapper
 *  Defines a decimal substitution variable. */
void	sqlrcur_subDouble(sqlrcur sqlrcurref,
				const char *variable, double value,
				uint32_t precision, uint32_t scale);

/** @ingroup sqlrclientwrapper
 *  Defines an array of string substitution variables. */
void	sqlrcur_subStrings(sqlrcur sqlrcurref,
				const char **variables, const char **values);

/** @ingroup sqlrclientwrapper
 *  Defines an array of integer substitution variables. */
void	sqlrcur_subLongs(sqlrcur sqlrcurref,
				const char **variables, const int64_t *values);

/** @ingroup sqlrclientwrapper
 *  Defines an array of decmial substitution variables. */
void	sqlrcur_subDoubles(sqlrcur sqlrcurref,
				const char **variables, const double *values,
				const uint32_t *precisions,
				const uint32_t *scales);



/** @ingroup sqlrclientwrapper
 *  Defines a string input bind variable. */
void	sqlrcur_inputBindString(sqlrcur sqlrcurref, 
				const char *variable, const char *value);

/** @ingroup sqlrclientwrapper
 *  Defines a string input bind variable. */
void	sqlrcur_inputBindStringWithLength(sqlrcur sqlrcurref, 
				const char *variable,
				const char *value, uint32_t valuelength);

/** @ingroup sqlrclientwrapper
 *  Defines a integer input bind variable. */
void	sqlrcur_inputBindLong(sqlrcur sqlrcurref, const char *variable, 
							int64_t value);

/** @ingroup sqlrclientwrapper
 *  Defines a decimal input bind variable. */
void	sqlrcur_inputBindDouble(sqlrcur sqlrcurref, 
					const char *variable, double value,
					uint32_t precision, 
					uint32_t scale);

/** @ingroup sqlrclientwrapper
 *  Defines a binary lob input bind variable. */
void	sqlrcur_inputBindBlob(sqlrcur sqlrcurref, 
					const char *variable, const char *value,
					uint32_t size);

/** @ingroup sqlrclientwrapper
 *  Defines a character lob input bind variable. */
void	sqlrcur_inputBindClob(sqlrcur sqlrcurref, 
					const char *variable, const char *value,
					uint32_t size);

/** @ingroup sqlrclientwrapper
 *  Defines an array of string input bind variables. */
void	sqlrcur_inputBindStrings(sqlrcur sqlrcurref, 
					const char **variables,
					const char **values);

/** @ingroup sqlrclientwrapper
 *  Defines an array of integer input bind variables. */
void	sqlrcur_inputBindLongs(sqlrcur sqlrcurref, 
					const char **variables, 
					const int64_t *values);

/** @ingroup sqlrclientwrapper
 *  Defines an array of decimal input bind variables. */
void	sqlrcur_inputBindDoubles(sqlrcur sqlrcurref, 
					const char **variables,
					const double *values,
					const uint32_t *precisions, 
					const uint32_t *scales);



/** @ingroup sqlrclientwrapper
 *  Defines a string output bind variable.
 *  "length" bytes will be reserved to store the value. */
void	sqlrcur_defineOutputBindString(sqlrcur sqlrcurref,
					const char *variable, uint32_t length);

/** @ingroup sqlrclientwrapper
 *  Defines an integer output bind variable. */
void	sqlrcur_defineOutputBindInteger(sqlrcur sqlrcurref,
					const char *variable);

/** @ingroup sqlrclientwrapper
 *  Defines an decimal output bind variable. */
void	sqlrcur_defineOutputBindDouble(sqlrcur sqlrcurref,
					const char *variable);

/** @ingroup sqlrclientwrapper
 *  Defines a binary lob output bind variable */
void	sqlrcur_defineOutputBindBlob(sqlrcur sqlrcurref,
					const char *variable);

/** @ingroup sqlrclientwrapper
 *  Defines a character lob output bind variable */
void	sqlrcur_defineOutputBindClob(sqlrcur sqlrcurref,
					const char *variable);

/** @ingroup sqlrclientwrapper
 *  Defines a cursor output bind variable */
void	sqlrcur_defineOutputBindCursor(sqlrcur sqlrcurref,
					const char *variable);



/** @ingroup sqlrclientwrapper
 *  Clears all bind variables. */
void	sqlrcur_clearBinds(sqlrcur sqlrcurref);

/** @ingroup sqlrclientwrapper
 *  Parses the previously prepared query, counts the number of bind variables
 *  defined in it and returns that number. */
uint16_t	sqlrcur_countBindVariables(sqlrcur sqlrcurref);

/** @ingroup sqlrclientwrapper
 *  If you are binding to any variables that might not actually be in your
 *  query, call this to ensure that the database won't try to bind them unless
 *  they really are in the query.  There is a performance penalty for calling
 *  this function */
void	sqlrcur_validateBinds(sqlrcur sqlrcurref);

/** @ingroup sqlrclientwrapper
 *  Returns true if "variable" was a valid bind variable of the query. */
int	sqlrcur_validBind(sqlrcur sqlrcurref, const char *variable);



/** @ingroup sqlrclientwrapper
 *  Execute the query that was previously prepared and bound. */
int	sqlrcur_executeQuery(sqlrcur sqlrcurref);

/** @ingroup sqlrclientwrapper
 *  Fetch from a cursor that was returned as an output bind variable. */
int	sqlrcur_fetchFromBindCursor(sqlrcur sqlrcurref);



/** @ingroup sqlrclientwrapper
 *  Get the value stored in a previously defined
 *  string output bind variable. */
const char	*sqlrcur_getOutputBindString(sqlrcur sqlrcurref,
						const char *variable);

/** @ingroup sqlrclientwrapper
 *  Get the value stored in a previously defined
 *  integer output bind variable. */
int64_t	sqlrcur_getOutputBindInteger(sqlrcur sqlrcurref,
						const char *variable);

/** @ingroup sqlrclientwrapper
 *  Get the value stored in a previously defined
 *  decimal output bind variable. */
double	sqlrcur_getOutputBindDouble(sqlrcur sqlrcurref,
						const char *variable);

/** @ingroup sqlrclientwrapper
 *  Get the value stored in a previously defined
 *  binary lob output bind variable. */
const char	*sqlrcur_getOutputBindBlob(sqlrcur sqlrcurref,
						const char *variable);

/** @ingroup sqlrclientwrapper
 *  Get the value stored in a previously defined
 *  character lob output bind variable. */
const char	*sqlrcur_getOutputBindClob(sqlrcur sqlrcurref,
						const char *variable);

/** @ingroup sqlrclientwrapper
 *  Get the length of the value stored in a previously
 *  defined output bind variable. */
uint32_t	sqlrcur_getOutputBindLength(sqlrcur sqlrcurref,
						const char *variable);

/** @ingroup sqlrclientwrapper
 *  Get the cursor associated with a previously defined output bind variable. */
sqlrcur	sqlrcur_getOutputBindCursor(sqlrcur sqlrcurref, const char *variable);



/** @ingroup sqlrclientwrapper
 *  Opens a cached result set.  Returns 1 on success and 0 on failure. */
int	sqlrcur_openCachedResultSet(sqlrcur sqlrcurref, const char *filename);



/** @ingroup sqlrclientwrapper
 *  Returns the number of columns in the current result set. */
uint32_t	sqlrcur_colCount(sqlrcur sqlrcurref);

/** @ingroup sqlrclientwrapper
 *  Returns the number of rows in the current result set. */
uint64_t	sqlrcur_rowCount(sqlrcur sqlrcurref);

/** @ingroup sqlrclientwrapper
 *  Returns the total number of rows that will be returned in the result set.
 *  Not all databases support this call.  Don't use it for applications which
 *  are designed to be portable across databases.  -1 is returned by databases
 *  which don't support this option. */
uint64_t	sqlrcur_totalRows(sqlrcur sqlrcurref);

/** @ingroup sqlrclientwrapper
 *  Returns the number of rows that were updated, inserted or deleted by the
 *  query.  Not all databases support this call.  Don't use it for applications
 *  which are designed to be portable across databases.  -1 is returned by
 *  databases which don't support this option. */
uint64_t	sqlrcur_affectedRows(sqlrcur sqlrcurref);

/** @ingroup sqlrclientwrapper
 *  Returns the index of the first buffered row.  This is useful when buffering
 *  only part of the result set at a time. */
uint64_t	sqlrcur_firstRowIndex(sqlrcur sqlrcurref);

/** @ingroup sqlrclientwrapper
 *  Returns 0 if part of the result set is still pending on the server and 1 if
 *  not.  This function can only return 0 if setResultSetBufferSize() has been
 *  called with a parameter other than 0. */
int	sqlrcur_endOfResultSet(sqlrcur sqlrcurref);



/** @ingroup sqlrclientwrapper
 *  If a query failed and generated an error, the error message is available
 *  here.  If the query succeeded then this function returns a NULL. */
const char	*sqlrcur_errorMessage(sqlrcur sqlrcurref);



/** @ingroup sqlrclientwrapper
 *  Tells the connection to return NULL fields and output bind variables as
 *  empty strings.  This is the default. */
void	sqlrcur_getNullsAsEmptyStrings(sqlrcur sqlrcurref);

/** @ingroup sqlrclientwrapper
 *  Tells the connection to return NULL fields
 *  and output bind variables as NULL's. */
void	sqlrcur_getNullsAsNulls(sqlrcur sqlrcurref);



/** @ingroup sqlrclientwrapper
 *  Returns the specified field as a string. */
const char	*sqlrcur_getFieldByIndex(sqlrcur sqlrcurref,
						uint64_t row, uint32_t col);

/** @ingroup sqlrclientwrapper
 *  Returns the specified field as a string. */
const char	*sqlrcur_getFieldByName(sqlrcur sqlrcurref,
						uint64_t row, const char *col);

/** @ingroup sqlrclientwrapper
 *  Returns the specified field as an integer. */
int64_t	sqlrcur_getFieldAsIntegerByIndex(sqlrcur sqlrcurref,
						uint64_t row, uint32_t col);

/** @ingroup sqlrclientwrapper
 *  Returns the specified field as an integer. */
int64_t	sqlrcur_getFieldAsIntegerByName(sqlrcur sqlrcurref,
						uint64_t row, const char *col);

/** @ingroup sqlrclientwrapper
 *  Returns the specified field as an decimal. */
double	sqlrcur_getFieldAsDoubleByIndex(sqlrcur sqlrcurref,
						uint64_t row, uint32_t col);

/** @ingroup sqlrclientwrapper
 *  Returns the specified field as an decimal. */
double	sqlrcur_getFieldAsDoubleByName(sqlrcur sqlrcurref,
						uint64_t row, const char *col);



/** @ingroup sqlrclientwrapper
 *  Returns the length of the specified row and column. */
uint32_t	sqlrcur_getFieldLengthByIndex(sqlrcur sqlrcurref,
						uint64_t row, uint32_t col);

/** @ingroup sqlrclientwrapper
 *  Returns the length of the specified row and column. */
uint32_t	sqlrcur_getFieldLengthByName(sqlrcur sqlrcurref,
						uint64_t row, const char *col);



/** @ingroup sqlrclientwrapper
 *  Returns a null terminated array of the values
 *  of the fields in the specified row. */
const char * const *sqlrcur_getRow(sqlrcur sqlrcurref, uint64_t row);

/** @ingroup sqlrclientwrapper
 *  Returns a null terminated array of the lengths
 *  of the fields in the specified row. */
uint32_t	*sqlrcur_getRowLengths(sqlrcur sqlrcurref, uint64_t row);

/** @ingroup sqlrclientwrapper
 *  Returns a null terminated array of the
 *  column names of the current result set. */
const char * const *sqlrcur_getColumnNames(sqlrcur sqlrcurref);

/** @ingroup sqlrclientwrapper
 *  Returns the name of the specified column. */
const char	*sqlrcur_getColumnName(sqlrcur sqlrcurref, uint32_t col);

/** @ingroup sqlrclientwrapper
 *  Returns the type of the specified column. */
const char	*sqlrcur_getColumnTypeByIndex(sqlrcur sqlrcurref, uint32_t col);

/** @ingroup sqlrclientwrapper
 *  Returns the type of the specified column. */
const char	*sqlrcur_getColumnTypeByName(sqlrcur sqlrcurref,
							const char *col);

/** @ingroup sqlrclientwrapper
 *  Returns the length of the specified column. */
uint32_t	sqlrcur_getColumnLengthByIndex(sqlrcur sqlrcurref,
							uint32_t col);

/** @ingroup sqlrclientwrapper
 *  Returns the length of the specified column. */
uint32_t	sqlrcur_getColumnLengthByName(sqlrcur sqlrcurref,
							const char *col);

/** @ingroup sqlrclientwrapper
 *  Returns the precision of the specified column.  Precision is the total
 *  number of digits in a number.  eg: 123.45 has a precision of 5.  For
 *  non-numeric types, it's the number of characters in the string. */
uint32_t	sqlrcur_getColumnPrecisionByIndex(sqlrcur sqlrcurref,
							uint32_t col);

/** @ingroup sqlrclientwrapper
 *  Returns the precision of the specified column.  Precision is the total
 *  number of digits in a number.  eg: 123.45 has a precision of 5.  For
 *  non-numeric types, it's the number of characters in the string. */
uint32_t	sqlrcur_getColumnPrecisionByName(sqlrcur sqlrcurref,
							const char *col);

/** @ingroup sqlrclientwrapper
 *  Returns the scale of the specified column.  Scale is the total number of
 *  digits to the right of the decimal point in a number.  eg: 123.45 has a
 *  scale of 2. */
uint32_t	sqlrcur_getColumnScaleByIndex(sqlrcur sqlrcurref,
							uint32_t col);

/** @ingroup sqlrclientwrapper
 *  Returns the scale of the specified column.  Scale is the total number of
 *  digits to the right of the decimal point in a number.  eg: 123.45 has a 
 *  scale of 2. */
uint32_t	sqlrcur_getColumnScaleByName(sqlrcur sqlrcurref,
							const char *col);

/** @ingroup sqlrclientwrapper
 *  Returns 1 if the specified column can contain nulls and 0 otherwise. */
int		sqlrcur_getColumnIsNullableByIndex(sqlrcur sqlrcurref,
							uint32_t col);

/** @ingroup sqlrclientwrapper
 *  Returns 1 if the specified column can contain nulls and 0 otherwise. */
int		sqlrcur_getColumnIsNullableByName(sqlrcur sqlrcurref,
							const char *col);

/** @ingroup sqlrclientwrapper
 *  Returns 1 if the specified column is a primary key and 0 otherwise. */
int		sqlrcur_getColumnIsPrimaryKeyByIndex(sqlrcur sqlrcurref,
							uint32_t col);

/** @ingroup sqlrclientwrapper
 *  Returns 1 if the specified column is a primary key and 0 otherwise. */
int		sqlrcur_getColumnIsPrimaryKeyByName(sqlrcur sqlrcurref,
							const char *col);

/** @ingroup sqlrclientwrapper
 *  Returns 1 if the specified column is unique and 0 otherwise. */
int		sqlrcur_getColumnIsUniqueByIndex(sqlrcur sqlrcurref,
							uint32_t col);

/** @ingroup sqlrclientwrapper
 *  Returns 1 if the specified column is unique and 0 otherwise. */
int		sqlrcur_getColumnIsUniqueByName(sqlrcur sqlrcurref,
							const char *col);

/** @ingroup sqlrclientwrapper
 *  Returns 1 if the specified column is part of a composite key and 0
 *  otherwise. */
int		sqlrcur_getColumnIsPartOfKeyByIndex(sqlrcur sqlrcurref,
							uint32_t col);

/** @ingroup sqlrclientwrapper
 *  Returns 1 if the specified column is part of a composite key and 0
 *  otherwise. */
int		sqlrcur_getColumnIsPartOfKeyByName(sqlrcur sqlrcurref,
							const char *col);

/** @ingroup sqlrclientwrapper
 *  Returns 1 if the specified column is an unsigned number and 0 otherwise. */
int		sqlrcur_getColumnIsUnsignedByIndex(sqlrcur sqlrcurref,
							uint32_t col);

/** @ingroup sqlrclientwrapper
 *  Returns 1 if the specified column is an unsigned number and 0 otherwise. */
int		sqlrcur_getColumnIsUnsignedByName(sqlrcur sqlrcurref,
							const char *col);

/** @ingroup sqlrclientwrapper
 *  Returns 1 if the specified column was created
 *  with the zero-fill flag and 0 otherwise. */
int		sqlrcur_getColumnIsZeroFilledByIndex(sqlrcur sqlrcurref,
							uint32_t col);

/** @ingroup sqlrclientwrapper
 *  Returns 1 if the specified column was created
 *  with the zero-fill flag and 0 otherwise. */
int		sqlrcur_getColumnIsZeroFilledByName(sqlrcur sqlrcurref,
							const char *col);

/** @ingroup sqlrclientwrapper
 *  Returns 1 if the specified column contains binary data and 0 otherwise. */
int		sqlrcur_getColumnIsBinaryByIndex(sqlrcur sqlrcurref,
							uint32_t col);

/** @ingroup sqlrclientwrapper
 *  Returns 1 if the specified column contains binary data and 0 otherwise. */
int		sqlrcur_getColumnIsBinaryByName(sqlrcur sqlrcurref,
							const char *col);

/** @ingroup sqlrclientwrapper
 *  Returns 1 if the specified column auto-increments and 0 otherwise. */
int		sqlrcur_getColumnIsAutoIncrementByIndex(sqlrcur sqlrcurref,
							uint32_t col);

/** @ingroup sqlrclientwrapper
 *  Returns 1 if the specified column auto-increments and 0 otherwise. */
int		sqlrcur_getColumnIsAutoIncrementByName(sqlrcur sqlrcurref,
							const char *col);

/** @ingroup sqlrclientwrapper
 *  Returns the length of the longest field in the specified column. */
uint32_t	sqlrcur_getLongestByIndex(sqlrcur sqlrcurref, uint32_t col);

/** @ingroup sqlrclientwrapper
 *  Returns the length of the longest field in the specified column. */
uint32_t	sqlrcur_getLongestByName(sqlrcur sqlrcurref, const char *col);



/** @ingroup sqlrclientwrapper
 *  Tells the server to leave this result set open when the connection calls
 *  suspendSession() so that another connection can connect to it using
 *  resumeResultSet() after it calls resumeSession(). */
void	sqlrcur_suspendResultSet(sqlrcur sqlrcurref);

/** @ingroup sqlrclientwrapper
 *  Returns the internal ID of this result set.  This parameter may be passed
 *  to another statement for use in the resumeResultSet() function.  Note: The
 *  value this function returns is only valid after a call to
 *  suspendResultSet().*/
uint16_t	sqlrcur_getResultSetId(sqlrcur sqlrcurref);

/** @ingroup sqlrclientwrapper
 *  Resumes a result set previously left open using suspendSession().
 *  Returns 1 on success and 0 on failure. */
int	sqlrcur_resumeResultSet(sqlrcur sqlrcurref, uint16_t id);

/** @ingroup sqlrclientwrapper
 *  Resumes a result set previously left open using suspendSession() and
 *  continues caching the result set to "filename".  Returns 1 on success and 0
 *  on failure. */
int	sqlrcur_resumeCachedResultSet(sqlrcur sqlrcurref, 
					uint16_t id, const char *filename);

#endif
