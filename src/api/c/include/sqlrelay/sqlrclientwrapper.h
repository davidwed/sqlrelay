/* Copyright (c) 2000-2001  David Muse
 See the file COPYING for more information */

#ifndef SQLRCLIENTWRAPPER_H
#define SQLRCLIENTWRAPPER_H

typedef	struct sqlrconnection *sqlrcon;
typedef	struct sqlrcursor *sqlrcur;

sqlrcon	sqlrcon_alloc(const char *server, int port, const char *socket,
					const char *user, const char *password, 
					int retrytime, int tries);
			/* Initiates a connection to "server" on "port"
			   or to the unix "socket" on the local machine
			   and authenticates with "user" and "password".
			   Failed connections will be retried for 
			   "tries" times on interval "retrytime"
			   or on for a default number of times on
			   a default interval if left unspecified.

			   If the "socket" parameter is nether 
			   NULL nor "" then an attempt will be made to 
			   connect through it before attempting to 
			   connect to "server" on "port".  If it is 
			   NULL or "" then no attempt will be made to 
			   connect through the socket.*/
void	sqlrcon_free(sqlrcon sqlrconref); 
			/* Disconnects and ends the session if
			   it hasn't been terminated already. */

int	sqlrcon_endSession(sqlrcon sqlrconref); 
			/* Ends the session. */
int	sqlrcon_suspendSession(sqlrcon sqlrconref);
			/* Disconnects this connection from the current
			   session but leaves the session open so 
			   that another connection can connect to it 
			   using sqlrcon_resumeSession(). */
int	sqlrcon_getConnectionPort(sqlrcon sqlrconref);
			/* Returns the inet port that the connection is 
			   communicating over. This parameter may be 
			   passed to another connection for use in
			   the sqlrcon_resumeSession() command. */
char	*sqlrcon_getConnectionSocket(sqlrcon sqlrconref);
			/* Returns the unix socket that the connection is 
			   communicating over. This parameter may be 
			   passed to another connection for use in
			   the sqlrcon_resumeSession() command. */
int	sqlrcon_resumeSession(sqlrcon sqlrconref, int port, const char *socket);
			/* Resumes a session previously left open 
			   using sqlrcon_suspendSession().
			   Returns 1 on success and 0 on failure. */

int	sqlrcon_ping(sqlrcon sqlrconref); 
			/* Returns 1 if the database is up and 0
			   if it's down. */
char	*sqlrcon_identify(sqlrcon sqlrconref); 
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

void	sqlrcur_setResultSetBufferSize(sqlrcur sqlrcurref, int rows);
			/* Sets the number of rows of the result set
			   to buffer at a time.  0 (the default)
			   means buffer the entire result set. */
int	sqlrcur_getResultSetBufferSize(sqlrcur sqlrcurref);
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
void	sqlrcur_setCacheTtl(sqlrcur sqlrcurref, int ttl);
			/* Sets the time-to-live for cached result
			   sets. The sqlr-cachemanger will remove each 
			   cached result set "ttl" seconds after it's 
			   created, provided it's scanning the directory
			   containing the cache files. */
char	*sqlrcur_getCacheFileName(sqlrcur sqlrcurref);
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
								int length); 
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
								int length);
			/* Prepare to execute "query" with length 
			   "length".  This function must be used if the
			   query contains binary data. */
void	sqlrcur_prepareFileQuery(sqlrcur sqlrcurref, 
					const char *path, const char *filename);
			/* Prepare to execute the contents 
			   of "path"/"filename". */

void	sqlrcur_subString(sqlrcur sqlrcurref,
				const char *variable, const char *value);
void	sqlrcur_subLong(sqlrcur sqlrcurref, const char *variable, long value);
void	sqlrcur_subDouble(sqlrcur sqlrcurref,
				const char *variable, double value,
				unsigned short precision, unsigned short scale);
			/* Define a substitution variable. */

void	sqlrcur_clearBinds(sqlrcur sqlrcurref);
			/* Clear all bind variables. */
void	sqlrcur_inputBindString(sqlrcur sqlrcurref, 
				const char *variable, const char *value);
void	sqlrcur_inputBindLong(sqlrcur sqlrcurref, const char *variable, 
						unsigned long value);
void	sqlrcur_inputBindDouble(sqlrcur sqlrcurref, 
					const char *variable, double value,
					unsigned short precision, 
					unsigned short scale);
void	sqlrcur_inputBindBlob(sqlrcur sqlrcurref, 
					const char *variable, const char *value,
					unsigned long size);
void	sqlrcur_inputBindClob(sqlrcur sqlrcurref, 
					const char *variable, const char *value,
					unsigned long size);
			/* Define an input bind variable. */
void	sqlrcur_defineOutputBind(sqlrcur sqlrcurref, const char *variable, 
							unsigned long length);
void	sqlrcur_defineOutputBindBlob(sqlrcur sqlrcurref,
					const char *variable);
void	sqlrcur_defineOutputBindClob(sqlrcur sqlrcurref,
					const char *variable);
void	sqlrcur_defineOutputBindCursor(sqlrcur sqlrcurref,
					const char *variable);
			/* Define an output bind variable. */
void	sqlrcur_subStrings(sqlrcur sqlrcurref,
				const char **variables, const char **values);
void	sqlrcur_subLongs(sqlrcur sqlrcurref,
				const char **variables, const long *values);
void	sqlrcur_subDoubles(sqlrcur sqlrcurref,
				const char **variables, const double *values,
				const unsigned short *precisions,
				const unsigned short *scales);
			/* Define an array of substitution variables. */
void	sqlrcur_inputBindStrings(sqlrcur sqlrcurref, 
					const char **variables,
					const char **values);
void	sqlrcur_inputBindLongs(sqlrcur sqlrcurref, 
					const char **variables, 
					const unsigned long *values);
void	sqlrcur_inputBindDoubles(sqlrcur sqlrcurref, 
					const char **variables,
					const double *values,
					const unsigned short *precisions, 
					const unsigned short *scales);
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

char	*sqlrcur_getOutputBind(sqlrcur sqlrcurref, const char *variable);
			/* Get the value stored in a previously
			   defined output bind variable. */
long	sqlrcur_getOutputBindLength(sqlrcur sqlrcurref, const char *variable);
			/* Get the length of the value stored in a previously
			   defined output bind variable. */
sqlrcur	sqlrcur_getOuputBindCursor(sqlrcur sqlrcurref, const char *variable);
			/* Get the cursor associated with a previously
			   defined output bind variable. */



int	sqlrcur_openCachedResultSet(sqlrcur sqlrcurref, const char *filename); 
			/* Opens a cached result set.
			   Returns 1 on success and 0 on failure. */

int	sqlrcur_colCount(sqlrcur sqlrcurref); 
			/* Returns the number of columns in the current
			   result set. */
int	sqlrcur_rowCount(sqlrcur sqlrcurref); 
			/* Returns the number of rows in the current 
			   result set. */
int	sqlrcur_totalRows(sqlrcur sqlrcurref);
			/* Returns the total number of rows that will 
			   be returned in the result set.  Not all 
			   databases support this call.  Don't use it 
			   for applications which are designed to be 
			   portable across databases.  -1 is returned
			   by databases which don't support this option. */
int	sqlrcur_affectedRows(sqlrcur sqlrcurref);
			/* Returns the number of rows that were 
			   updated, inserted or deleted by the query.
			   Not all databases support this call.  Don't 
			   use it for applications which are designed 
			   to be portable across databases.  -1 is 
			   returned by databases which don't support 
			   this option. */
int	sqlrcur_firstRowIndex(sqlrcur sqlrcurref);
			/* Returns the index of the first buffered row.
			   This is useful when buffering only part of
			   the result set at a time. */
int	sqlrcur_endOfResultSet(sqlrcur sqlrcurref);
			/* Returns 0 if part of the result set is still
			   pending on the server and 1 if not.  This
			   function can only return 0 if 
			   setResultSetBufferSize() has been called
			   with a parameter other than 0. */
  

char	*sqlrcur_errorMessage(sqlrcur sqlrcurref); 
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

char	*sqlrcur_getFieldByIndex(sqlrcur sqlrcurref, int row, int col); 
char	*sqlrcur_getFieldByName(sqlrcur sqlrcurref, int row, const char *col); 
			/* Returns a pointer to the value of the 
			   specified row and column. */
long	sqlrcur_getFieldLengthByIndex(sqlrcur sqlrcurref, int row, int col); 
long	sqlrcur_getFieldLengthByName(sqlrcur sqlrcurref, int row,
							const char *col); 
			/* Returns the length of the 
			   specified row and column. */
char	**sqlrcur_getRow(sqlrcur sqlrcurref, int row); 
			/* Returns a null terminated array of the 
			   values of the fields in the specified row. */
long	*sqlrcur_getRowLengths(sqlrcur sqlrcurref, int row); 
			/* Returns a null terminated array of the 
			   lengths of the fields in the specified row. */
char	**sqlrcur_getColumnNames(sqlrcur sqlrcurref); 
			/* Returns a null terminated array of the 
			   column names of the current result set. */
char	*sqlrcur_getColumnName(sqlrcur sqlrcurref, int col); 
			/* Returns the name of the specified column. */
char	*sqlrcur_getColumnTypeByIndex(sqlrcur sqlrcurref, int col); 
char	*sqlrcur_getColumnTypeByName(sqlrcur sqlrcurref, const char *col); 
			/* Returns the type of the specified column. */
int	sqlrcur_getColumnLengthByIndex(sqlrcur sqlrcurref, int col); 
int	sqlrcur_getColumnLengthByName(sqlrcur sqlrcurref, const char *col); 
			/* Returns the length of the specified column. */
int	sqlrcur_getLongestByIndex(sqlrcur sqlrcurref, int col);
int	sqlrcur_getLongestByName(sqlrcur sqlrcurref, const char *col);
			/* Returns the length of the longest field in the
			   specified column. */


int	sqlrcur_getResultSetId(sqlrcur sqlrcurref);
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
int	sqlrcur_resumeResultSet(sqlrcur sqlrcurref, int id);
			/* Resumes a result set previously left open 
			   using suspendSession().
			   Returns 1 on success and 0 on failure. */
int	sqlrcur_resumeCachedResultSet(sqlrcur sqlrcurref, 
						int id, const char *filename);
			/* Resumes a result set previously left open
			   using suspendSession() and continues caching
			   the result set to "filename".
			   Returns 1 on success and 0 on failure. */
#endif
