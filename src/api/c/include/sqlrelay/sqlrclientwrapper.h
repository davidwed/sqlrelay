/* Copyright (c) 2000-2001  David Muse
 See the file COPYING for more information */

#ifndef SQLRCLIENTWRAPPER_H
#define SQLRCLIENTWRAPPER_H

typedef	struct sqlrconnection *sqlrcon;
typedef	struct sqlrcursor *sqlrcur;

sqlrcon	sqlrcon_alloc(const char *server, uint16_t port, const char *socket,
					const char *user, const char *password, 
					int32_t retrytime, int32_t tries);
			/* Initiates a connection to "server" on "port"
			   or to the unix "socket" on the local machine
			   and authenticates with "user" and "password".
			   Failed connections will be retried for 
			   "tries" times on interval "retrytime".
			   If "tries" is 0 then retries will continue
			   forever.  If "retrytime" is 0 then retries
			   will be attempted on a default interval.

			   If the "socket" parameter is nether 
			   NULL nor "" then an attempt will be made to 
			   connect through it before attempting to 
			   connect to "server" on "port".  If it is 
			   NULL or "" then no attempt will be made to 
			   connect through the socket.*/
void	sqlrcon_free(sqlrcon sqlrconref); 
			/* Disconnects and ends the session if
			   it hasn't been terminated already. */

void	sqlrcon_endSession(sqlrcon sqlrconref); 
			/* Ends the session. */
int	sqlrcon_suspendSession(sqlrcon sqlrconref);
			/* Disconnects this connection from the current
			   session but leaves the session open so 
			   that another connection can connect to it 
			   using sqlrcon_resumeSession(). */
uint16_t	sqlrcon_getConnectionPort(sqlrcon sqlrconref);
			/* Returns the inet port that the connection is 
			   communicating over. This parameter may be 
			   passed to another connection for use in
			   the sqlrcon_resumeSession() command. */
const char	*sqlrcon_getConnectionSocket(sqlrcon sqlrconref);
			/* Returns the unix socket that the connection is 
			   communicating over. This parameter may be 
			   passed to another connection for use in
			   the sqlrcon_resumeSession() command. */
int	sqlrcon_resumeSession(sqlrcon sqlrconref, uint16_t port,
							const char *socket);
			/* Resumes a session previously left open 
			   using sqlrcon_suspendSession().
			   Returns 1 on success and 0 on failure. */

int	sqlrcon_ping(sqlrcon sqlrconref); 
			/* Returns 1 if the database is up and 0
			   if it's down. */
const char	*sqlrcon_identify(sqlrcon sqlrconref); 
			/* Returns the type of database: 
			     oracle7, oracle8, postgresql, mysql, etc. */

int	sqlrcon_autoCommitOn(sqlrcon sqlrconref);
			/* Instructs the database to perform a commit
			   after every successful query. */
int	sqlrcon_autoCommitOff(sqlrcon sqlrconref);
			/* Instructs the database to wait for the 
			   client to tell it when to commit. */
int	sqlrcon_commit(sqlrcon sqlrconref);
			/* Issues a commit.  Returns 1 if the commit
			   succeeded, 0 if it failed and -1 if an
			   error occurred. */
int	sqlrcon_rollback(sqlrcon sqlrconref);
			/* Issues a rollback.  Returns 1 if the rollback
			   succeeded, 0 if it failed and -1 if an
			   error occurred. */

void	sqlrcon_debugOn(sqlrcon sqlrconref); 
			/* Causes verbose debugging information to be 
			   sent to standard output.  Another way to do 
			   this is to start a query with "-- debug\n". */
void	sqlrcon_debugOff(sqlrcon sqlrconref);
			/* Turns debugging off. */
int	sqlrcon_getDebug(sqlrcon sqlrconref);
			/* Returns 0 if debugging is off and 1 if 
			   debugging is on. */

void	sqlrcon_debugPrintFunction(sqlrcon sqlrconref, 
					int (*printfunction)(const char *,...));
			/* Allows you to replace the function used
			   to print debug messages with your own
			   function.  The function is expected to take
			   arguments like printf. */


sqlrcur	sqlrcur_alloc(sqlrcon sqlrconref);
void	sqlrcur_free(sqlrcur sqlrcurref);

void	sqlrcur_setResultSetBufferSize(sqlrcur sqlrcurref, uint32_t rows);
			/* Sets the number of rows of the result set
			   to buffer at a time.  0 (the default)
			   means buffer the entire result set. */
uint32_t	sqlrcur_getResultSetBufferSize(sqlrcur sqlrcurref);
			/* Returns the number of result set rows that 
			   will be buffered at a time or 0 for the
			   entire result set. */

void	sqlrcur_dontGetColumnInfo(sqlrcur sqlrcurref);
			/* Tells the server not to send any column
			   info (names, types, sizes).  If you don't
			   need that info, you should call this
			   function to improve performance. */
void	sqlrcur_getColumnInfo(sqlrcur sqlrcurref);
			/* Tells the server to send column info. */


void	sqlrcur_mixedCaseColumnNames(sqlrcur sqlrcurref);
			/* Columns names are returned in the same
			   case as they are defined in the database.
			   This is the default. */
void	sqlrcur_upperCaseColumnNames(sqlrcur sqlrcurref);
			/* Columns names are converted to upper case. */
void	sqlrcur_lowerCaseColumnNames(sqlrcur sqlrcurref);
			/* Columns names are converted to lower case. */


void	sqlrcur_cacheToFile(sqlrcur sqlrcurref, const char *filename);
			/* Sets query caching on.  Future queries
			   will be cached to the file "filename".
			  
			   A default time-to-live of 10 minutes is
			   also set.
			  
			   Note that once sqlrcur_cacheToFile() is called,
			   the result sets of all future queries will
			   be cached to that file until another call 
			   to sqlrcur_cacheToFile() changes which file to
			   cache to or a call to sqlrcur_cacheOff() turns off
			   caching. */
void	sqlrcur_setCacheTtl(sqlrcur sqlrcurref, uint32_t ttl);
			/* Sets the time-to-live for cached result
			   sets. The sqlr-cachemanger will remove each 
			   cached result set "ttl" seconds after it's 
			   created, provided it's scanning the directory
			   containing the cache files. */
const char	*sqlrcur_getCacheFileName(sqlrcur sqlrcurref);
			/* Returns the name of the file containing the
			   most recently cached result set. */
void	sqlrcur_cacheOff(sqlrcur sqlrcurref);
			/* Sets query caching off. */



/* If you need to use substitution or bind variables, in your
   queries use the following functions.  See the API documentation
   for more information about substitution and bind variables. */
int	sqlrcur_sendQuery(sqlrcur sqlrcurref, const char *query); 
			/* Sends "query" and gets a result set. */
int	sqlrcur_sendQueryWithLength(sqlrcur sqlrcurref, const char *query,
							uint32_t length); 
			/* Sends "query" with length "length" and gets
			   a result set. This function must be used if
			   the query contains binary data. */
int	sqlrcur_sendFileQuery(sqlrcur sqlrcurref,
				const char *path, const char *filename);
			/* Sends the query in file "path"/"filename" 
			   and gets a result set. */


/* If you need to use substitution or bind variables, in your
   queries use the following functions.  See the footnote for 
   information about substitution and bind variables. */
void	sqlrcur_prepareQuery(sqlrcur sqlrcurref, const char *query);
			/* Prepare to execute "query". */
void	sqlrcur_prepareQueryWithLength(sqlrcur sqlrcurref, const char *query,
							uint32_t length);
			/* Prepare to execute "query" with length 
			   "length".  This function must be used if the
			   query contains binary data. */
void	sqlrcur_prepareFileQuery(sqlrcur sqlrcurref, 
					const char *path, const char *filename);
			/* Prepare to execute the contents 
			   of "path"/"filename". */

void	sqlrcur_subString(sqlrcur sqlrcurref,
				const char *variable, const char *value);
void	sqlrcur_subLong(sqlrcur sqlrcurref,
				const char *variable, int32_t value);
void	sqlrcur_subDouble(sqlrcur sqlrcurref,
				const char *variable, double value,
				uint32_t precision, uint32_t scale);
			/* Define a substitution variable. */

void	sqlrcur_clearBinds(sqlrcur sqlrcurref);
			/* Clear all bind variables. */

uint16_t	sqlrcur_countBindVariables(sqlrcur sqlrcurref);
			/* Parses the previously prepared query,
			   counts the number of bind variables defined
			   in it and returns that number. */

void	sqlrcur_inputBindString(sqlrcur sqlrcurref, 
				const char *variable, const char *value);
void	sqlrcur_inputBindLong(sqlrcur sqlrcurref, const char *variable, 
							int32_t value);
void	sqlrcur_inputBindDouble(sqlrcur sqlrcurref, 
					const char *variable, double value,
					uint32_t precision, 
					uint32_t scale);
void	sqlrcur_inputBindBlob(sqlrcur sqlrcurref, 
					const char *variable, const char *value,
					uint32_t size);
void	sqlrcur_inputBindClob(sqlrcur sqlrcurref, 
					const char *variable, const char *value,
					uint32_t size);
			/* Define an input bind variable. */

void	sqlrcur_defineOutputBind(sqlrcur sqlrcurref, const char *variable, 
							uint32_t length);
			/* Define an output bind variable.
			  "length" bytes will be reserved to store the value. */
void	sqlrcur_defineOutputBindBlob(sqlrcur sqlrcurref,
					const char *variable);
			/* Define a BLOB output bind variable */
void	sqlrcur_defineOutputBindClob(sqlrcur sqlrcurref,
					const char *variable);
			/* Define a CLOB output bind variable */
void	sqlrcur_defineOutputBindCursor(sqlrcur sqlrcurref,
					const char *variable);
			/* Define a cursor output bind variable */

void	sqlrcur_subStrings(sqlrcur sqlrcurref,
				const char **variables, const char **values);
void	sqlrcur_subLongs(sqlrcur sqlrcurref,
				const char **variables, const int32_t *values);
void	sqlrcur_subDoubles(sqlrcur sqlrcurref,
				const char **variables, const double *values,
				const uint32_t *precisions,
				const uint32_t *scales);
			/* Define an array of substitution variables. */
void	sqlrcur_inputBindStrings(sqlrcur sqlrcurref, 
					const char **variables,
					const char **values);
void	sqlrcur_inputBindLongs(sqlrcur sqlrcurref, 
					const char **variables, 
					const int32_t *values);
void	sqlrcur_inputBindDoubles(sqlrcur sqlrcurref, 
					const char **variables,
					const double *values,
					const uint32_t *precisions, 
					const uint32_t *scales);
			/* Define an array of input bind variables. */

void	sqlrcur_validateBinds(sqlrcur sqlrcurref);
			/* If you are binding to any variables that 
			   might not actually be in your query, call 
			   this to ensure that the database won't try 
			   to bind them unless they really are in the 
			   query.  There is a performance penalty for
			   calling this function */

int	sqlrcur_executeQuery(sqlrcur sqlrcurref);
			/* Execute the query that was previously prepared 
			   and bound. */

int	sqlrcur_fetchFromBindCursor(sqlrcur sqlrcurref);
			/* Fetch from a cursor that was returned as
			   an output bind variable. */

const char	*sqlrcur_getOutputBind(sqlrcur sqlrcurref,
					const char *variable);
			/* Get the value stored in a previously
			   defined output bind variable. */
int32_t	sqlrcur_getOutputBindAsLong(sqlrcur sqlrcurref,
						const char *variable);
			/* Get the value stored in a previously
			   defined output bind variable as a 
			   long integer. */
double	sqlrcur_getOutputBindAsDouble(sqlrcur sqlrcurref,
						const char *variable);
			/* Get the value stored in a previously
			   defined output bind variable as a 
			   double precision floating point
			   number. */
uint32_t	sqlrcur_getOutputBindLength(sqlrcur sqlrcurref,
						const char *variable);
			/* Get the length of the value stored in a previously
			   defined output bind variable. */
sqlrcur	sqlrcur_getOutputBindCursor(sqlrcur sqlrcurref, const char *variable);
			/* Get the cursor associated with a previously
			   defined output bind variable. */



int	sqlrcur_openCachedResultSet(sqlrcur sqlrcurref, const char *filename); 
			/* Opens a cached result set.
			   Returns 1 on success and 0 on failure. */

uint32_t	sqlrcur_colCount(sqlrcur sqlrcurref); 
			/* Returns the number of columns in the current
			   result set. */
uint32_t	sqlrcur_rowCount(sqlrcur sqlrcurref); 
			/* Returns the number of rows in the current 
			   result set. */
uint32_t	sqlrcur_totalRows(sqlrcur sqlrcurref);
			/* Returns the total number of rows that will 
			   be returned in the result set.  Not all 
			   databases support this call.  Don't use it 
			   for applications which are designed to be 
			   portable across databases.  -1 is returned
			   by databases which don't support this option. */
uint32_t	sqlrcur_affectedRows(sqlrcur sqlrcurref);
			/* Returns the number of rows that were 
			   updated, inserted or deleted by the query.
			   Not all databases support this call.  Don't 
			   use it for applications which are designed 
			   to be portable across databases.  -1 is 
			   returned by databases which don't support 
			   this option. */
uint32_t	sqlrcur_firstRowIndex(sqlrcur sqlrcurref);
			/* Returns the index of the first buffered row.
			   This is useful when buffering only part of
			   the result set at a time. */
int	sqlrcur_endOfResultSet(sqlrcur sqlrcurref);
			/* Returns 0 if part of the result set is still
			   pending on the server and 1 if not.  This
			   function can only return 0 if 
			   setResultSetBufferSize() has been called
			   with a parameter other than 0. */
  

const char	*sqlrcur_errorMessage(sqlrcur sqlrcurref); 
			/* If a query failed and generated an error, the
			   error message is available here.  If the 
			   query succeeded then this function returns a 
			   NULL. */


void	sqlrcur_getNullsAsEmptyStrings(sqlrcur sqlrcurref);
			/* Tells the connection to return NULL fields and
			   output bind variables as empty strings.
			   This is the default. */
void	sqlrcur_getNullsAsNulls(sqlrcur sqlrcurref);
			/* Tells the connection to return NULL fields and
			   output bind variables as NULL's. */

const char	*sqlrcur_getFieldByIndex(sqlrcur sqlrcurref,
						uint32_t row, uint32_t col); 
const char	*sqlrcur_getFieldByName(sqlrcur sqlrcurref, uint32_t row,
							const char *col); 
			/* Returns a pointer to the value of the 
			   specified row and column. */
int32_t	sqlrcur_getFieldAsLongByIndex(sqlrcur sqlrcurref,
						uint32_t row, uint32_t col); 
int32_t	sqlrcur_getFieldAsLongByName(sqlrcur sqlrcurref, uint32_t row,
							const char *col); 
			/* Returns the specified field as a long integer. */
double	sqlrcur_getFieldAsDoubleByIndex(sqlrcur sqlrcurref,
						uint32_t row, uint32_t col); 
double	sqlrcur_getFieldAsDoubleByName(sqlrcur sqlrcurref, uint32_t row,
							const char *col); 
			/* Returns the specified field as a double precision
			   floating point number. */
const char	*sqlrcur_getFieldByIndex(sqlrcur sqlrcurref,
						uint32_t row, uint32_t col); 
const char	*sqlrcur_getFieldByName(sqlrcur sqlrcurref, uint32_t row,
							const char *col); 
			/* Returns a pointer to the value of the 
			   specified row and column. */
uint32_t	sqlrcur_getFieldLengthByIndex(sqlrcur sqlrcurref,
						uint32_t row, uint32_t col); 
uint32_t	sqlrcur_getFieldLengthByName(sqlrcur sqlrcurref, uint32_t row,
							const char *col); 
			/* Returns the length of the 
			   specified row and column. */
const char * const *sqlrcur_getRow(sqlrcur sqlrcurref, uint32_t row); 
			/* Returns a null terminated array of the 
			   values of the fields in the specified row. */
uint32_t	*sqlrcur_getRowLengths(sqlrcur sqlrcurref, uint32_t row); 
			/* Returns a null terminated array of the 
			   lengths of the fields in the specified row. */
const char * const *sqlrcur_getColumnNames(sqlrcur sqlrcurref); 
			/* Returns a null terminated array of the 
			   column names of the current result set. */
const char	*sqlrcur_getColumnName(sqlrcur sqlrcurref, uint32_t col); 
			/* Returns the name of the specified column. */
const char	*sqlrcur_getColumnTypeByIndex(sqlrcur sqlrcurref, uint32_t col); 
const char	*sqlrcur_getColumnTypeByName(sqlrcur sqlrcurref,
							const char *col); 
			/* Returns the type of the specified column. */
uint32_t	sqlrcur_getColumnLengthByIndex(sqlrcur sqlrcurref,
							uint32_t col); 
uint32_t	sqlrcur_getColumnLengthByName(sqlrcur sqlrcurref,
							const char *col); 
			/* Returns the length of the specified column. */
uint32_t	sqlrcur_getColumnPrecisionByIndex(sqlrcur sqlrcurref,
							uint32_t col);
uint32_t	sqlrcur_getColumnPrecisionByName(sqlrcur sqlrcurref,
							const char *col);
			/* Returns the precision of the specified
			   column.
			   Precision is the total number of digits in
			   a number.  eg: 123.45 has a precision of 5.
			   For non-numeric types, it's the number of
			   characters in the string. */
uint32_t	sqlrcur_getColumnScaleByIndex(sqlrcur sqlrcurref,
							uint32_t col);
uint32_t	sqlrcur_getColumnScaleByName(sqlrcur sqlrcurref,
							const char *col);
			/* Returns the scale of the specified column.
			   Scale is the total number of digits to the
			   right of the decimal point in a number.
			   eg: 123.45 has a scale of 2. */
int		sqlrcur_getColumnIsNullableByIndex(sqlrcur sqlrcurref,
							uint32_t col);
int		sqlrcur_getColumnIsNullableByName(sqlrcur sqlrcurref,
							const char *col);
			/* Returns 1 if the specified column can
			   contain nulls and 0 otherwise. */
int		sqlrcur_getColumnIsPrimaryKeyByIndex(sqlrcur sqlrcurref,
							uint32_t col);
int		sqlrcur_getColumnIsPrimaryKeyByName(sqlrcur sqlrcurref,
							const char *col);
			/* Returns 1 if the specified column is a
			   primary key and 0 otherwise. */
int		sqlrcur_getColumnIsUniqueByIndex(sqlrcur sqlrcurref,
							uint32_t col);
int		sqlrcur_getColumnIsUniqueByName(sqlrcur sqlrcurref,
							const char *col);
			/* Returns 1 if the specified column is
			   unique and 0 otherwise. */
int		sqlrcur_getColumnIsPartOfKeyByIndex(sqlrcur sqlrcurref,
							uint32_t col);
int		sqlrcur_getColumnIsPartOfKeyByName(sqlrcur sqlrcurref,
							const char *col);
			/* Returns 1 if the specified column is
			   part of a composite key and 0 otherwise. */
int		sqlrcur_getColumnIsUnsignedByIndex(sqlrcur sqlrcurref,
							uint32_t col);
int		sqlrcur_getColumnIsUnsignedByName(sqlrcur sqlrcurref,
							const char *col);
			/* Returns 1 if the specified column is
			   an unsigned number and 0 otherwise. */
int		sqlrcur_getColumnIsZeroFilledByIndex(sqlrcur sqlrcurref,
							uint32_t col);
int		sqlrcur_getColumnIsZeroFilledByName(sqlrcur sqlrcurref,
							const char *col);
			/* Returns 1 if the specified column was
			   created with the zero-fill flag and 0
			   otherwise. */
int		sqlrcur_getColumnIsBinaryByIndex(sqlrcur sqlrcurref,
							uint32_t col);
int		sqlrcur_getColumnIsBinaryByName(sqlrcur sqlrcurref,
							const char *col);
			/* Returns 1 if the specified column
			   contains binary data and 0
			   otherwise. */
int		sqlrcur_getColumnIsAutoIncrementByIndex(sqlrcur sqlrcurref,
							uint32_t col);
int		sqlrcur_getColumnIsAutoIncrementByName(sqlrcur sqlrcurref,
							const char *col);
			/* Returns 1 if the specified column
			   auto-increments and 0 otherwise. */

uint32_t	sqlrcur_getLongestByIndex(sqlrcur sqlrcurref, uint32_t col);
uint32_t	sqlrcur_getLongestByName(sqlrcur sqlrcurref, const char *col);
			/* Returns the length of the longest field in the
			   specified column. */


uint16_t	sqlrcur_getResultSetId(sqlrcur sqlrcurref);
			/* Returns the internal ID of this result set.
			   This parameter may be passed to another 
			   statement for use in the resumeResultSet() 
			   function. */
void	sqlrcur_suspendResultSet(sqlrcur sqlrcurref);
			/* Tells the server to leave this result
			   set open when the connection calls 
			   suspendSession() so that another connection can 
			   connect to it using resumeResultSet() after 
			   it calls resumeSession(). */
int	sqlrcur_resumeResultSet(sqlrcur sqlrcurref, uint16_t id);
			/* Resumes a result set previously left open 
			   using suspendSession().
			   Returns 1 on success and 0 on failure. */
int	sqlrcur_resumeCachedResultSet(sqlrcur sqlrcurref, 
					uint16_t id, const char *filename);
			/* Resumes a result set previously left open
			   using suspendSession() and continues caching
			   the result set to "filename".
			   Returns 1 on success and 0 on failure. */
#endif
