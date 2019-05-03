/* Copyright (c) 1999-2018 David Muse
 See the file COPYING for more information */

#ifndef SQLRCLIENTWRAPPER_H
#define SQLRCLIENTWRAPPER_H

#include <sqlrelay/private/sqlrclientwrapperincludes.h>

/** @file
 *  @defgroup sqlrclientwrapper sqlrclientwrapper */

typedef	struct sqlrconnection *sqlrcon;
typedef	struct sqlrcursor *sqlrcur;

/** @ingroup sqlrclientwrapper
 *  Initiates a connection to "server" on "port" or to the unix "socket" on
 *  the local machine and auths with "user" and "password".  Failed
 *  connections will be retried for "tries" times, waiting "retrytime" seconds
 *  between each try.  If "tries" is 0 then retries will continue forever.  If
 *  "retrytime" is 0 then retries will be attempted on a default interval.  If
 *  the "socket" parameter is nether NULL nor "" then an attempt will be made
 *  to connect through it before attempting to connect to "server" on "port".
 *  If it is NULL or "" then no attempt will be made to connect through the
 *  socket.*/
SQLRCLIENT_DLLSPEC
sqlrcon	sqlrcon_alloc(const char *server, uint16_t port, const char *socket,
					const char *user, const char *password, 
					int32_t retrytime, int32_t tries);

/** @ingroup sqlrclientwrapper
 *  Disconnects and ends the session if it hasn't been terminated already. */
SQLRCLIENT_DLLSPEC
void	sqlrcon_free(sqlrcon sqlrconref);



/** @ingroup sqlrclientwrapper
 *  Sets the server connect timeout in seconds and
 *  milliseconds.  Setting either parameter to -1 disables the
 *  timeout.  You can also set this timeout using the
 *  SQLR_CLIENT_CONNECT_TIMEOUT environment variable. */
SQLRCLIENT_DLLSPEC
void	sqlrcon_setConnectTimeout(sqlrcon sqlrconref,
				int32_t timeoutsec, int32_t timeoutusec);

/** @ingroup sqlrclientwrapper
 *  Sets the auth timeout in seconds and
 *  milliseconds.  Setting either parameter to -1 disables the
 *  timeout.   You can also set this timeout using the
 *  SQLR_CLIENT_AUTHENTICATION_TIMEOUT environment variable. */
SQLRCLIENT_DLLSPEC
void	sqlrcon_setAuthenticationTimeout(sqlrcon sqlrconref,
				int32_t timeoutsec, int32_t timeoutusec);

/** @ingroup sqlrclientwrapper
 *  Sets the response timeout (for queries, commits, rollbacks,
 *  pings, etc.) in seconds and milliseconds.  Setting either
 *  parameter to -1 disables the timeout.  You can also set
 *  this timeout using the SQLR_CLIENT_RESPONSE_TIMEOUT
 *  environment variable. */
SQLRCLIENT_DLLSPEC
void	sqlrcon_setResponseTimeout(sqlrcon sqlrconref,
				int32_t timeoutsec, int32_t timeoutusec);


/** @ingroup sqlrclientwrapper
 *  Gets the server connect timeout in seconds and microseconds. */
SQLRCLIENT_DLLSPEC
void	sqlrcon_getConnectTimeout(sqlrcon sqlrconref,
				int32_t *timeoutsec, int32_t *timeoutusec);

/** @ingroup sqlrclientwrapper
 *  Gets the authentication timeout in seconds and microseconds. */
SQLRCLIENT_DLLSPEC
void	sqlrcon_getAuthenticationTimeout(sqlrcon sqlrconref,
				int32_t *timeoutsec, int32_t *timeoutusec);

/** @ingroup sqlrclientwrapper
 *  Gets the response timeout in seconds and microseconds. */
SQLRCLIENT_DLLSPEC
void	sqlrcon_getResponseTimeout(sqlrcon sqlrconref,
				int32_t *timeoutsec, int32_t *timeoutusec);



/** @ingroup sqlrclientwrapper
 *  Sets which delimiters are used to identify bind variables
 *  in countBindVariables() and validateBinds().  Valid
 *  delimiters include ?,:,@, and $.  Defaults to "?:@$" */
void	sqlrcon_setBindVariableDelimiters(sqlrcon sqlrconref,
						const char *delimiters);

/** @ingroup sqlrclientwrapper
 *  Returns true if question marks (?) are considered to be
 *  valid bind variable delimiters. */
int	sqlrcon_getBindVariableDelimiterQuestionMarkSupported(
						sqlrcon sqlrconref);

/** @ingroup sqlrclientwrapper
 *  Returns true if colons (:) are considered to be
 *  valid bind variable delimiters. */
int	sqlrcon_getBindVariableDelimiterColonSupported(
						sqlrcon sqlrconref);

/** @ingroup sqlrclientwrapper
 *  Returns true if at-signs (@) are considered to be
 *  valid bind variable delimiters. */
int	sqlrcon_getBindVariableDelimiterAtSignSupported(
						sqlrcon sqlrconref);

/** @ingroup sqlrclientwrapper
 *  Returns true if dollar signs ($) are considered to be
 *  valid bind variable delimiters. */
int	sqlrcon_getBindVariableDelimiterDollarSignSupported(
						sqlrcon sqlrconref);



/** @ingroup sqlrclientwrapper
 *  Enables Kerberos authentication and encryption.
 *
 *  "service" indicates the Kerberos service name of the
 *  SQL Relay server.  If left empty or NULL then the service
 *  name "sqlrelay" will be used. "sqlrelay" is the default
 *  service name of the SQL Relay server.  Note that on Windows
 *  platforms the service name must be fully qualified,
 *  including the host and realm name.  For example:
 *  "sqlrelay/sqlrserver.firstworks.com@AD.FIRSTWORKS.COM".
 *
 *  "mech" indicates the specific Kerberos mechanism to use.
 *  On Linux/Unix platforms, this should be a string
 *  representation of the mechnaism's OID, such as:
 *      { 1 2 840 113554 1 2 2 }
 *  On Windows platforms, this should be a string like:
 *      Kerberos
 *  If left empty or NULL then the default mechanism will be
 *  used.  Only set this if you know that you have a good
 *  reason to.
 *
 *  "flags" indicates what Kerberos flags to use.  Multiple
 *  flags may be specified, separated by commas.  If left
 *  empty or NULL then a defalt set of flags will be used.
 *  Only set this if you know that you have a good reason to.
 *
 *  Valid flags include:
 *   * GSS_C_MUTUAL_FLAG
 *   * GSS_C_REPLAY_FLAG
 *   * GSS_C_SEQUENCE_FLAG
 *   * GSS_C_CONF_FLAG
 *   * GSS_C_INTEG_FLAG
 *
 *  For a full list of flags, consult the GSSAPI documentation,
 *  though note that only the flags listed above are supported
 *  on Windows. */
SQLRCLIENT_DLLSPEC
void	sqlrcon_enableKerberos(sqlrcon sqlrconref,
					const char *service,
					const char *mech,
					const char *flags);

/** @ingroup sqlrclientwrapper
 *  Enables TLS/SSL encryption, and optionally authentication.
 *
 *  "version" specifies the TLS/SSL protocol version that the
 *  client will attempt to use.  Valid values include SSL2,
 *  SSL3, TLS1, TLS1.1, TLS1.2 or any more recent version of
 *  TLS, as supported by and enabled in the underlying TLS/SSL
 *  library.  If left blank or empty then the highest supported
 *  version will be negotiated.
 *
 *  "cert" is the file name of the certificate chain file to
 *  send to the SQL Relay server.  This is only necessary if
 *  the SQL Relay server is configured to authenticate and
 *  authorize clients by certificate.
 *
 *  If "cert" contains a password-protected private key, then
 *  "password" may be supplied to access it.  If the private
 *  key is not password-protected, then this argument is
 *  ignored, and may be left empty or NULL.
 *
 *  "ciphers" is a list of ciphers to allow.  Ciphers may be
 *  separated by spaces, commas, or colons.  If "ciphers" is
 *  empty or NULL then a default set is used.  Only set this if
 *  you know that you have a good reason to.
 *
 *  For a list of valid ciphers on Linux/Unix platforms, see:
 *      man ciphers
 *
 *  For a list of valid ciphers on Windows platforms, see:
 *      https://msdn.microsoft.com/en-us/library/windows/desktop/aa375549%28v=vs.85%29.aspx
 *  On Windows platforms, the ciphers (alg_id's) should omit
 *  CALG_ and may be given with underscores or dashes.
 *  For example: 3DES_112
 *
 *  "validate" indicates whether to validate the SQL Relay's
 *  server certificate, and may be set to one of the following:
 *      "no" - Don't validate the server's certificate.
 *      "ca" - Validate that the server's certificate was
 *             signed by a trusted certificate authority.
 *      "ca+host" - Perform "ca" validation and also validate
 *             that one of the subject altenate names (or the
 *             common name if no SANs are present) in the
 *             certificate matches the host parameter.
 *             (Falls back to "ca" validation when a unix
 *             socket is used.)
 *      "ca+domain" - Perform "ca" validation and also validate
 *             that the domain name of one of the subject
 *             alternate names (or the common name if no SANs
 *             are present) in the certificate matches the
 *             domain name of the host parameter.  (Falls back
 *             to "ca" validation when a unix socket is used.)
 *
 *  "ca" is the location of a certificate authority file to
 *  use, in addition to the system's root certificates, when
 *  validating the SQL Relay server's certificate.  This is
 *  useful if the SQL Relay server's certificate is self-signed.
 *
 *  On Windows, "ca" must be a file name.
 *
 *  On non-Windows systems, "ca" can be either a file or
 *  directory name.  If it is a directory name, then all
 *  certificate authority files found in that directory will be
 *  used.  If it a file name, then only that file will be used.
 *
 *
 *  Note that the supported "cert" and "ca" file formats may
 *  vary between platforms.  A variety of file formats are
 *  generally supported on Linux/Unix platfoms (.pem, .pfx,
 *  etc.) but only the .pfx format is currently supported on
 *  Windows. */
SQLRCLIENT_DLLSPEC
void	sqlrcon_enableTls(sqlrcon sqlrconref,
				const char *version,
				const char *cert,
				const char *password,
				const char *ciphers,
				const char *validate,
				const char *ca,
				uint16_t depth);

/** @ingroup sqlrclientwrapper
  * Disables encryption. */
SQLRCLIENT_DLLSPEC
void	sqlrcon_disableEncryption(sqlrcon sqlrconref);



/** @ingroup sqlrclientwrapper
 *  Ends the session. */
SQLRCLIENT_DLLSPEC
void	sqlrcon_endSession(sqlrcon sqlrconref);

/** @ingroup sqlrclientwrapper
 *  Disconnects this connection from the current session but leaves the session
 *  open so that another connection can connect to it using
 *  sqlrcon_resumeSession(). */
SQLRCLIENT_DLLSPEC
int	sqlrcon_suspendSession(sqlrcon sqlrconref);

/** @ingroup sqlrclientwrapper
 *  Returns the inet port that the connection is communicating over.  This
 *  parameter may be passed to another connection for use in the
 *  sqlrcon_resumeSession() command.  Note: The result this function returns
 *  is only valid after a call to suspendSession(). */
SQLRCLIENT_DLLSPEC
uint16_t	sqlrcon_getConnectionPort(sqlrcon sqlrconref);

/** @ingroup sqlrclientwrapper
 *  Returns the unix socket that the connection is communicating over.  This
 *  parameter may be passed to another connection for use in the
 *  sqlrcon_resumeSession() command.  Note: The result this function returns
 *  is only valid after a call to suspendSession(). */
SQLRCLIENT_DLLSPEC
const char	*sqlrcon_getConnectionSocket(sqlrcon sqlrconref);

/** @ingroup sqlrclientwrapper
 *  Resumes a session previously left open using sqlrcon_suspendSession().
 *  Returns 1 on success and 0 on failure. */
SQLRCLIENT_DLLSPEC
int	sqlrcon_resumeSession(sqlrcon sqlrconref, uint16_t port,
							const char *socket);



/** @ingroup sqlrclientwrapper
 *  Returns 1 if the database is up and 0 if it's down. */
SQLRCLIENT_DLLSPEC
int	sqlrcon_ping(sqlrcon sqlrconref);

/** @ingroup sqlrclientwrapper
 *  Returns the type of database: oracle, postgresql, mysql, etc. */
SQLRCLIENT_DLLSPEC
const char	*sqlrcon_identify(sqlrcon sqlrconref);

/** @ingroup sqlrclientwrapper
 *  Returns the version of the database */
SQLRCLIENT_DLLSPEC
const char	*sqlrcon_dbVersion(sqlrcon sqlrconref);

/** @ingroup sqlrclientwrapper
 *  Returns the host name of the database */
SQLRCLIENT_DLLSPEC
const char	*sqlrcon_dbHostName(sqlrcon sqlrconref);

/** @ingroup sqlrclientwrapper
 *  Returns the ip address of the database */
SQLRCLIENT_DLLSPEC
const char	*sqlrcon_dbIpAddress(sqlrcon sqlrconref);

/** @ingroup sqlrclientwrapper
 *  Returns the version of the sqlrelay server software. */
SQLRCLIENT_DLLSPEC
const char	*sqlrcon_serverVersion(sqlrcon sqlrconref);

/** @ingroup sqlrclientwrapper
 *  Returns the version of the sqlrelay client software. */
SQLRCLIENT_DLLSPEC
const char	*sqlrcon_clientVersion(sqlrcon sqlrconref);

/** @ingroup sqlrclientwrapper
 *  Returns a string representing the format
 *  of the bind variables used in the db. */
SQLRCLIENT_DLLSPEC
const char	*sqlrcon_bindFormat(sqlrcon sqlrconref);



/** @ingroup sqlrclientwrapper
 *  Sets the current database/schema to "database" */
SQLRCLIENT_DLLSPEC
int	sqlrcon_selectDatabase(sqlrcon sqlrconref, const char *database);

/** @ingroup sqlrclientwrapper
 *  Returns the database/schema that is currently in use. */
SQLRCLIENT_DLLSPEC
const char	*sqlrcon_getCurrentDatabase(sqlrcon sqlrconref);



/** @ingroup sqlrclientwrapper
 *  Returns the value of the autoincrement column for the last insert */
SQLRCLIENT_DLLSPEC
uint64_t	sqlrcon_getLastInsertId(sqlrcon sqlrconref);



/** @ingroup sqlrclientwrapper
 *  Instructs the database to perform a commit after every successful query. */
SQLRCLIENT_DLLSPEC
int	sqlrcon_autoCommitOn(sqlrcon sqlrconref);

/** @ingroup sqlrclientwrapper
 *  Instructs the database to wait for the client to tell it when to commit. */
SQLRCLIENT_DLLSPEC
int	sqlrcon_autoCommitOff(sqlrcon sqlrconref);



/** @ingroup sqlrclientwrapper
 *  Begins a transaction.  Returns 1 if the begin
 *  succeeded, 0 if it failed.  If the database
 *  automatically begins a new transaction when a
 *  commit or rollback is issued then this doesn't
 *  do anything unless SQL Relay is faking transaction
 *  blocks. */
SQLRCLIENT_DLLSPEC
int	sqlrcon_begin(sqlrcon sqlrconref);

/** @ingroup sqlrclientwrapper
 *  Issues a commit.  Returns 1 if the commit succeeded, 0 if it failed. */
SQLRCLIENT_DLLSPEC
int	sqlrcon_commit(sqlrcon sqlrconref);

/** @ingroup sqlrclientwrapper
 *  Issues a rollback.  Returns 1 if the rollback succeeded, 0 if it failed. */
SQLRCLIENT_DLLSPEC
int	sqlrcon_rollback(sqlrcon sqlrconref);



/** @ingroup sqlrclientwrapper
 *  If an operation failed and generated an error, the error message is
 *  available here.  If there is no error then this method returns NULL */
SQLRCLIENT_DLLSPEC
const char	*sqlrcon_errorMessage(sqlrcon sqlrconref);

/** @ingroup sqlrclientwrapper
 *  If an operation failed and generated an error, the error number is
 *  available here.  If there is no error then this method returns 0. */
SQLRCLIENT_DLLSPEC
int64_t		sqlrcon_errorNumber(sqlrcon sqlrconref);


/** @ingroup sqlrclientwrapper
 *  Causes verbose debugging information to be sent to standard output.
 *  Another way to do this is to start a query with "-- debug\n".
 *  Yet another way is to set the environment variable SQLR_CLIENT_DEBUG
 *  to "ON" */
SQLRCLIENT_DLLSPEC
void	sqlrcon_debugOn(sqlrcon sqlrconref);

/** @ingroup sqlrclientwrapper
 *  Turns debugging off. */
SQLRCLIENT_DLLSPEC
void	sqlrcon_debugOff(sqlrcon sqlrconref);

/** @ingroup sqlrclientwrapper
 *  Returns 0 if debugging is off and 1 if debugging is on. */
SQLRCLIENT_DLLSPEC
int	sqlrcon_getDebug(sqlrcon sqlrconref);



/** @ingroup sqlrclientwrapper
 *  Allows you to replace the function used to print debug messages with your
 *  own function.  The function is expected to take arguments like printf. */
SQLRCLIENT_DLLSPEC
void	sqlrcon_debugPrintFunction(sqlrcon sqlrconref, 
					int (*printfunction)(const char *,...));

/** @ingroup sqlrclientwrapper
 *  Allows you to specify a file to write debug to.
 *  Setting "filename" to NULL or an empty string causes debug
 *  to be written to standard output (the default). */
SQLRCLIENT_DLLSPEC
void	sqlrcon_setDebugFile(sqlrcon sqlrconref, const char *filename);

/** @ingroup sqlrclientwrapper
 *  Allows you to set a string that will be passed to the server and ultimately
 *  included in server-side logging along with queries that were run by this
 *  instance of the client. */
SQLRCLIENT_DLLSPEC
void	sqlrcon_setClientInfo(sqlrcon sqlrconref, const char *clientinfo);

/** @ingroup sqlrclientwrapper
 *  Returns the string that was set by sqlrcon_setClientInfo(). */
SQLRCLIENT_DLLSPEC
const char	*sqlrcon_getClientInfo(sqlrcon sqlrconref);



/** @ingroup sqlrclientwrapper
 *  Creates a cursor to run queries and fetch
 *  result sets using connection "sqlrconref" */
SQLRCLIENT_DLLSPEC
sqlrcur	sqlrcur_alloc(sqlrcon sqlrconref);

/** @ingroup sqlrclientwrapper
 *  Destroys the cursor and cleans up all associated result set data. */
SQLRCLIENT_DLLSPEC
void	sqlrcur_free(sqlrcur sqlrcurref);



/** @ingroup sqlrclientwrapper
 *  Sets the number of rows of the result set to buffer at a time.
 *  0 (the default) means buffer the entire result set. */
SQLRCLIENT_DLLSPEC
void	sqlrcur_setResultSetBufferSize(sqlrcur sqlrcurref, uint64_t rows);

/** @ingroup sqlrclientwrapper
 *  Returns the number of result set rows that will be buffered at a time or
 *  0 for the entire result set. */
SQLRCLIENT_DLLSPEC
uint64_t	sqlrcur_getResultSetBufferSize(sqlrcur sqlrcurref);



/** @ingroup sqlrclientwrapper
 *  Tells the server not to send any column info (names, types, sizes).  If
 *  you don't need that info, you should call this function to improve
 *  performance. */
SQLRCLIENT_DLLSPEC
void	sqlrcur_dontGetColumnInfo(sqlrcur sqlrcurref);

/** @ingroup sqlrclientwrapper
 *  Tells the server to send column info. */
SQLRCLIENT_DLLSPEC
void	sqlrcur_getColumnInfo(sqlrcur sqlrcurref);



/** @ingroup sqlrclientwrapper
 *  Columns names are returned in the same case as they are defined in the
 *  database.  This is the default. */
SQLRCLIENT_DLLSPEC
void	sqlrcur_mixedCaseColumnNames(sqlrcur sqlrcurref);

/** @ingroup sqlrclientwrapper
 *  Columns names are converted to upper case. */
SQLRCLIENT_DLLSPEC
void	sqlrcur_upperCaseColumnNames(sqlrcur sqlrcurref);

/** @ingroup sqlrclientwrapper
 *  Columns names are converted to lower case. */
SQLRCLIENT_DLLSPEC
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
SQLRCLIENT_DLLSPEC
void	sqlrcur_cacheToFile(sqlrcur sqlrcurref, const char *filename);

/** @ingroup sqlrclientwrapper
 *  Sets the time-to-live for cached result sets. The sqlr-cachemanger will
 *  remove each cached result set "ttl" seconds after it's created, provided
 *  it's scanning the directory containing the cache files. */
SQLRCLIENT_DLLSPEC
void	sqlrcur_setCacheTtl(sqlrcur sqlrcurref, uint32_t ttl);

/** @ingroup sqlrclientwrapper
 *  Returns the name of the file containing
 *  the most recently cached result set. */
SQLRCLIENT_DLLSPEC
const char	*sqlrcur_getCacheFileName(sqlrcur sqlrcurref);

/** @ingroup sqlrclientwrapper
 *  Sets query caching off. */
SQLRCLIENT_DLLSPEC
void	sqlrcur_cacheOff(sqlrcur sqlrcurref);



/** @ingroup sqlrclientwrapper
 *  Sends a query that returns a list of databases/schemas matching "wild".
 *  If wild is empty or NULL then a list of all databases/schemas will be
 *  returned. */
SQLRCLIENT_DLLSPEC
int	sqlrcur_getDatabaseList(sqlrcur sqlrcurref, const char *wild);

/** @ingroup sqlrclientwrapper
 *  Sends a query that returns a list of tables matching "wild".  If wild is
 *  empty or NULL then a list of all tables will be returned. */
SQLRCLIENT_DLLSPEC
int	sqlrcur_getTableList(sqlrcur sqlrcurref, const char *wild);

/** @ingroup sqlrclientwrapper
 *  Sends a query that returns a list of columns in the table specified by the
 *  "table" parameter matching "wild".  If wild is empty or NULL then a list of
 *  all columns will be returned. */
SQLRCLIENT_DLLSPEC
int	sqlrcur_getColumnList(sqlrcur sqlrcurref,
				const char *table, const char *wild);



/** @ingroup sqlrclientwrapper
 *  Sends "query" directly and gets a result set. */
SQLRCLIENT_DLLSPEC
int	sqlrcur_sendQuery(sqlrcur sqlrcurref, const char *query);

/** @ingroup sqlrclientwrapper
 *  Sends "query" with length "length" directly and gets a result set. This
 *  function must be used if the query contains binary data. */
SQLRCLIENT_DLLSPEC
int	sqlrcur_sendQueryWithLength(sqlrcur sqlrcurref, const char *query,
							uint32_t length);

/** @ingroup sqlrclientwrapper
 *  Sends the query in file "path"/"filename" and gets a result set. */
SQLRCLIENT_DLLSPEC
int	sqlrcur_sendFileQuery(sqlrcur sqlrcurref,
				const char *path, const char *filename);



/** @ingroup sqlrclientwrapper
 *  Prepare to execute "query". */
SQLRCLIENT_DLLSPEC
void	sqlrcur_prepareQuery(sqlrcur sqlrcurref, const char *query);

/** @ingroup sqlrclientwrapper
 *  Prepare to execute "query" with length "length".  This function must be
 *  used if the query contains binary data. */
SQLRCLIENT_DLLSPEC
void	sqlrcur_prepareQueryWithLength(sqlrcur sqlrcurref,
						const char *query,
						uint32_t length);

/** @ingroup sqlrclientwrapper
 *  Prepare to execute the contents of "path"/"filename". */
SQLRCLIENT_DLLSPEC
void	sqlrcur_prepareFileQuery(sqlrcur sqlrcurref, 
					const char *path, const char *filename);



/** @ingroup sqlrclientwrapper
 *  Defines a string substitution variable. */
SQLRCLIENT_DLLSPEC
void	sqlrcur_subString(sqlrcur sqlrcurref,
				const char *variable, const char *value);

/** @ingroup sqlrclientwrapper
 *  Defines a integer substitution variable. */
SQLRCLIENT_DLLSPEC
void	sqlrcur_subLong(sqlrcur sqlrcurref,
				const char *variable, int64_t value);

/** @ingroup sqlrclientwrapper
 *  Defines a decimal substitution variable. */
SQLRCLIENT_DLLSPEC
void	sqlrcur_subDouble(sqlrcur sqlrcurref,
				const char *variable, double value,
				uint32_t precision, uint32_t scale);

/** @ingroup sqlrclientwrapper
 *  Defines an array of string substitution variables. */
SQLRCLIENT_DLLSPEC
void	sqlrcur_subStrings(sqlrcur sqlrcurref,
				const char **variables, const char **values);

/** @ingroup sqlrclientwrapper
 *  Defines an array of integer substitution variables. */
SQLRCLIENT_DLLSPEC
void	sqlrcur_subLongs(sqlrcur sqlrcurref,
				const char **variables, const int64_t *values);

/** @ingroup sqlrclientwrapper
 *  Defines an array of decmial substitution variables. */
SQLRCLIENT_DLLSPEC
void	sqlrcur_subDoubles(sqlrcur sqlrcurref,
				const char **variables, const double *values,
				const uint32_t *precisions,
				const uint32_t *scales);



/** @ingroup sqlrclientwrapper
 *  Defines a string input bind variable. */
SQLRCLIENT_DLLSPEC
void	sqlrcur_inputBindString(sqlrcur sqlrcurref, 
				const char *variable, const char *value);

/** @ingroup sqlrclientwrapper
 *  Defines a string input bind variable. */
SQLRCLIENT_DLLSPEC
void	sqlrcur_inputBindStringWithLength(sqlrcur sqlrcurref, 
				const char *variable,
				const char *value, uint32_t valuelength);

/** @ingroup sqlrclientwrapper
 *  Defines a integer input bind variable. */
SQLRCLIENT_DLLSPEC
void	sqlrcur_inputBindLong(sqlrcur sqlrcurref, const char *variable, 
							int64_t value);

/** @ingroup sqlrclientwrapper
 *  Defines a decimal input bind variable.
 * (If you don't have the precision and scale then set
 * them both to 0.  However in that case you may get
 * unexpected rounding behavior if the server is faking
 * binds.) */
SQLRCLIENT_DLLSPEC
void	sqlrcur_inputBindDouble(sqlrcur sqlrcurref, 
					const char *variable, double value,
					uint32_t precision, 
					uint32_t scale);

/** @ingroup sqlrclientwraper
 *  Defines a date input bind variable.  "day" and "month"
 *  are 1-based.
 *
 *  Some databases distinguish between date, time, and
 *  datetime types.  For those databases...
 *
 *  * The input bind variable will be interpreted as a time type
 *  if year and/or month are negative.
 *
 *  * The input bind variable will be interpreted as a date type
 *  if hour, minute, second, and/or microsecond are negative.
 *
 *  * The input bind variable will be interpreted as a datetime
 *  type if all parts are positive.
 *
 *  "tz" is the timezone abbreviation, and may be left NULL.
 *  Most databases ignore "tz".
 *
 *  Set "isnegative" may be set to true to represent a negative
 *  time interval.  However, few databases support negative
 *  time intervals and ignore "isnegative". */
SQLRCLIENT_DLLSPEC
void	sqlrcur_inputBindDate(sqlrcur sqlrcurref,
				const char *variable,
				int16_t year, int16_t month, int16_t day,
				int16_t hour, int16_t minute, int16_t second,
				int32_t microsecond, const char *tz,
				int isnegative);

/** @ingroup sqlrclientwrapper
 *  Defines a binary lob input bind variable. */
SQLRCLIENT_DLLSPEC
void	sqlrcur_inputBindBlob(sqlrcur sqlrcurref, 
					const char *variable, const char *value,
					uint32_t size);

/** @ingroup sqlrclientwrapper
 *  Defines a character lob input bind variable. */
SQLRCLIENT_DLLSPEC
void	sqlrcur_inputBindClob(sqlrcur sqlrcurref, 
					const char *variable, const char *value,
					uint32_t size);

/** @ingroup sqlrclientwrapper
 *  Defines an array of string input bind variables. */
SQLRCLIENT_DLLSPEC
void	sqlrcur_inputBindStrings(sqlrcur sqlrcurref, 
					const char **variables,
					const char **values);

/** @ingroup sqlrclientwrapper
 *  Defines an array of integer input bind variables. */
SQLRCLIENT_DLLSPEC
void	sqlrcur_inputBindLongs(sqlrcur sqlrcurref, 
					const char **variables, 
					const int64_t *values);

/** @ingroup sqlrclientwrapper
 *  Defines an array of decimal input bind variables. */
SQLRCLIENT_DLLSPEC
void	sqlrcur_inputBindDoubles(sqlrcur sqlrcurref, 
					const char **variables,
					const double *values,
					const uint32_t *precisions, 
					const uint32_t *scales);



/** @ingroup sqlrclientwrapper
 *  Defines a string output bind variable.
 *  "length" bytes will be reserved to store the value. */
SQLRCLIENT_DLLSPEC
void	sqlrcur_defineOutputBindString(sqlrcur sqlrcurref,
					const char *variable, uint32_t length);

/** @ingroup sqlrclientwrapper
 *  Defines an integer output bind variable. */
SQLRCLIENT_DLLSPEC
void	sqlrcur_defineOutputBindInteger(sqlrcur sqlrcurref,
					const char *variable);

/** @ingroup sqlrclientwrapper
 *  Defines an decimal output bind variable. */
SQLRCLIENT_DLLSPEC
void	sqlrcur_defineOutputBindDouble(sqlrcur sqlrcurref,
					const char *variable);

/** @ingroup sqlrclientwrapper
 *   Defines a date output bind variable. */
SQLRCLIENT_DLLSPEC
void	sqlrcur_defineOutputBindDate(sqlrcur sqlrcurref,
					const char *variable);

/** @ingroup sqlrclientwrapper
 *  Defines a binary lob output bind variable */
SQLRCLIENT_DLLSPEC
void	sqlrcur_defineOutputBindBlob(sqlrcur sqlrcurref,
					const char *variable);

/** @ingroup sqlrclientwrapper
 *  Defines a character lob output bind variable */
SQLRCLIENT_DLLSPEC
void	sqlrcur_defineOutputBindClob(sqlrcur sqlrcurref,
					const char *variable);

/** @ingroup sqlrclientwrapper
 *  Defines a cursor output bind variable */
SQLRCLIENT_DLLSPEC
void	sqlrcur_defineOutputBindCursor(sqlrcur sqlrcurref,
					const char *variable);



/** @ingroup sqlrclientwrapper
 *  Clears all bind variables. */
SQLRCLIENT_DLLSPEC
void	sqlrcur_clearBinds(sqlrcur sqlrcurref);

/** @ingroup sqlrclientwrapper
 *  Parses the previously prepared query, counts the number of bind variables
 *  defined in it and returns that number. */
SQLRCLIENT_DLLSPEC
uint16_t	sqlrcur_countBindVariables(sqlrcur sqlrcurref);

/** @ingroup sqlrclientwrapper
 *  If you are binding to any variables that might not actually be in your
 *  query, call this to ensure that the database won't try to bind them unless
 *  they really are in the query.  There is a performance penalty for calling
 *  this function */
SQLRCLIENT_DLLSPEC
void	sqlrcur_validateBinds(sqlrcur sqlrcurref);

/** @ingroup sqlrclientwrapper
 *  Returns true if "variable" was a valid bind variable of the query. */
SQLRCLIENT_DLLSPEC
int	sqlrcur_validBind(sqlrcur sqlrcurref, const char *variable);



/** @ingroup sqlrclientwrapper
 *  Execute the query that was previously prepared and bound. */
SQLRCLIENT_DLLSPEC
int	sqlrcur_executeQuery(sqlrcur sqlrcurref);

/** @ingroup sqlrclientwrapper
 *  Fetch from a cursor that was returned as an output bind variable. */
SQLRCLIENT_DLLSPEC
int	sqlrcur_fetchFromBindCursor(sqlrcur sqlrcurref);



/** @ingroup sqlrclientwrapper
 *  Get the value stored in a previously defined
 *  string output bind variable. */
SQLRCLIENT_DLLSPEC
const char	*sqlrcur_getOutputBindString(sqlrcur sqlrcurref,
						const char *variable);

/** @ingroup sqlrclientwrapper
 *  Get the value stored in a previously defined
 *  integer output bind variable. */
SQLRCLIENT_DLLSPEC
int64_t	sqlrcur_getOutputBindInteger(sqlrcur sqlrcurref,
						const char *variable);

/** @ingroup sqlrclientwrapper
 *  Get the value stored in a previously defined
 *  decimal output bind variable. */
SQLRCLIENT_DLLSPEC
double	sqlrcur_getOutputBindDouble(sqlrcur sqlrcurref,
						const char *variable);

/** @ingroup sqlrclientwrapper
 *  Get the value stored in a previously
 *  defined date output bind variable. */
SQLRCLIENT_DLLSPEC
int	sqlrcur_getOutputBindDate(sqlrcur sqlrcurref,
				const char *variable,
				int16_t *year, int16_t *month, int16_t *day,
				int16_t *hour, int16_t *minute, int16_t *second,
				int32_t *microsecond, const char **tz,
				int *isnegative);

/** @ingroup sqlrclientwrapper
 *  Get the value stored in a previously defined
 *  binary lob output bind variable. */
SQLRCLIENT_DLLSPEC
const char	*sqlrcur_getOutputBindBlob(sqlrcur sqlrcurref,
						const char *variable);

/** @ingroup sqlrclientwrapper
 *  Get the value stored in a previously defined
 *  character lob output bind variable. */
SQLRCLIENT_DLLSPEC
const char	*sqlrcur_getOutputBindClob(sqlrcur sqlrcurref,
						const char *variable);

/** @ingroup sqlrclientwrapper
 *  Get the length of the value stored in a previously
 *  defined output bind variable. */
SQLRCLIENT_DLLSPEC
uint32_t	sqlrcur_getOutputBindLength(sqlrcur sqlrcurref,
						const char *variable);

/** @ingroup sqlrclientwrapper
 *  Get the cursor associated with a previously defined output bind variable. */
SQLRCLIENT_DLLSPEC
sqlrcur	sqlrcur_getOutputBindCursor(sqlrcur sqlrcurref, const char *variable);



/** @ingroup sqlrclientwrapper
 *  Opens a cached result set.  Returns 1 on success and 0 on failure. */
SQLRCLIENT_DLLSPEC
int	sqlrcur_openCachedResultSet(sqlrcur sqlrcurref, const char *filename);



/** @ingroup sqlrclientwrapper
 *  Returns the number of columns in the current result set. */
SQLRCLIENT_DLLSPEC
uint32_t	sqlrcur_colCount(sqlrcur sqlrcurref);

/** @ingroup sqlrclientwrapper
 *  Returns the number of rows in the current result set. */
SQLRCLIENT_DLLSPEC
uint64_t	sqlrcur_rowCount(sqlrcur sqlrcurref);

/** @ingroup sqlrclientwrapper
 *  Returns the total number of rows that will be returned in the result set.
 *  Not all databases support this call.  Don't use it for applications which
 *  are designed to be portable across databases.  -1 is returned by databases
 *  which don't support this option. */
SQLRCLIENT_DLLSPEC
uint64_t	sqlrcur_totalRows(sqlrcur sqlrcurref);

/** @ingroup sqlrclientwrapper
 *  Returns the number of rows that were updated, inserted or deleted by the
 *  query.  Not all databases support this call.  Don't use it for applications
 *  which are designed to be portable across databases.  -1 is returned by
 *  databases which don't support this option. */
SQLRCLIENT_DLLSPEC
uint64_t	sqlrcur_affectedRows(sqlrcur sqlrcurref);

/** @ingroup sqlrclientwrapper
 *  Returns the index of the first buffered row.  This is useful when buffering
 *  only part of the result set at a time. */
SQLRCLIENT_DLLSPEC
uint64_t	sqlrcur_firstRowIndex(sqlrcur sqlrcurref);

/** @ingroup sqlrclientwrapper
 *  Returns 0 if part of the result set is still pending on the server and 1 if
 *  not.  This function can only return 0 if setResultSetBufferSize() has been
 *  called with a parameter other than 0. */
SQLRCLIENT_DLLSPEC
int	sqlrcur_endOfResultSet(sqlrcur sqlrcurref);



/** @ingroup sqlrclientwrapper
 *  If a query failed and generated an error, the error message is available
 *  here.  If the query succeeded then this function returns a NULL. */
SQLRCLIENT_DLLSPEC
const char	*sqlrcur_errorMessage(sqlrcur sqlrcurref);

/** @ingroup sqlrclientwrapper
 *  If a query failed and generated an error, the error number is available
 *  here.  If there is no error then this method returns 0. */
SQLRCLIENT_DLLSPEC
int64_t		sqlrcur_errorNumber(sqlrcur sqlrcurref);


/** @ingroup sqlrclientwrapper
 *  Tells the connection to return NULL fields and output bind variables as
 *  empty strings.  This is the default. */
SQLRCLIENT_DLLSPEC
void	sqlrcur_getNullsAsEmptyStrings(sqlrcur sqlrcurref);

/** @ingroup sqlrclientwrapper
 *  Tells the connection to return NULL fields
 *  and output bind variables as NULL's. */
SQLRCLIENT_DLLSPEC
void	sqlrcur_getNullsAsNulls(sqlrcur sqlrcurref);



/** @ingroup sqlrclientwrapper
 *  Returns the specified field as a string. */
SQLRCLIENT_DLLSPEC
const char	*sqlrcur_getFieldByIndex(sqlrcur sqlrcurref,
						uint64_t row, uint32_t col);

/** @ingroup sqlrclientwrapper
 *  Returns the specified field as a string. */
SQLRCLIENT_DLLSPEC
const char	*sqlrcur_getFieldByName(sqlrcur sqlrcurref,
						uint64_t row, const char *col);

/** @ingroup sqlrclientwrapper
 *  Returns the specified field as an integer. */
SQLRCLIENT_DLLSPEC
int64_t	sqlrcur_getFieldAsIntegerByIndex(sqlrcur sqlrcurref,
						uint64_t row, uint32_t col);

/** @ingroup sqlrclientwrapper
 *  Returns the specified field as an integer. */
SQLRCLIENT_DLLSPEC
int64_t	sqlrcur_getFieldAsIntegerByName(sqlrcur sqlrcurref,
						uint64_t row, const char *col);

/** @ingroup sqlrclientwrapper
 *  Returns the specified field as an decimal. */
SQLRCLIENT_DLLSPEC
double	sqlrcur_getFieldAsDoubleByIndex(sqlrcur sqlrcurref,
						uint64_t row, uint32_t col);

/** @ingroup sqlrclientwrapper
 *  Returns the specified field as an decimal. */
SQLRCLIENT_DLLSPEC
double	sqlrcur_getFieldAsDoubleByName(sqlrcur sqlrcurref,
						uint64_t row, const char *col);



/** @ingroup sqlrclientwrapper
 *  Returns the length of the specified row and column. */
SQLRCLIENT_DLLSPEC
uint32_t	sqlrcur_getFieldLengthByIndex(sqlrcur sqlrcurref,
						uint64_t row, uint32_t col);

/** @ingroup sqlrclientwrapper
 *  Returns the length of the specified row and column. */
SQLRCLIENT_DLLSPEC
uint32_t	sqlrcur_getFieldLengthByName(sqlrcur sqlrcurref,
						uint64_t row, const char *col);



/** @ingroup sqlrclientwrapper
 *  Returns a null terminated array of the values
 *  of the fields in the specified row. */
SQLRCLIENT_DLLSPEC
const char * const *sqlrcur_getRow(sqlrcur sqlrcurref, uint64_t row);

/** @ingroup sqlrclientwrapper
 *  Returns a null terminated array of the lengths
 *  of the fields in the specified row. */
SQLRCLIENT_DLLSPEC
uint32_t	*sqlrcur_getRowLengths(sqlrcur sqlrcurref, uint64_t row);

/** @ingroup sqlrclientwrapper
 *  Returns a null terminated array of the
 *  column names of the current result set. */
SQLRCLIENT_DLLSPEC
const char * const *sqlrcur_getColumnNames(sqlrcur sqlrcurref);

/** @ingroup sqlrclientwrapper
 *  Returns the name of the specified column. */
SQLRCLIENT_DLLSPEC
const char	*sqlrcur_getColumnName(sqlrcur sqlrcurref, uint32_t col);

/** @ingroup sqlrclientwrapper
 *  Returns the type of the specified column. */
SQLRCLIENT_DLLSPEC
const char	*sqlrcur_getColumnTypeByIndex(sqlrcur sqlrcurref, uint32_t col);

/** @ingroup sqlrclientwrapper
 *  Returns the type of the specified column. */
SQLRCLIENT_DLLSPEC
const char	*sqlrcur_getColumnTypeByName(sqlrcur sqlrcurref,
							const char *col);

/** @ingroup sqlrclientwrapper
 *  Returns the length of the specified column. */
SQLRCLIENT_DLLSPEC
uint32_t	sqlrcur_getColumnLengthByIndex(sqlrcur sqlrcurref,
							uint32_t col);

/** @ingroup sqlrclientwrapper
 *  Returns the length of the specified column. */
SQLRCLIENT_DLLSPEC
uint32_t	sqlrcur_getColumnLengthByName(sqlrcur sqlrcurref,
							const char *col);

/** @ingroup sqlrclientwrapper
 *  Returns the precision of the specified column.  Precision is the total
 *  number of digits in a number.  eg: 123.45 has a precision of 5.  For
 *  non-numeric types, it's the number of characters in the string. */
SQLRCLIENT_DLLSPEC
uint32_t	sqlrcur_getColumnPrecisionByIndex(sqlrcur sqlrcurref,
							uint32_t col);

/** @ingroup sqlrclientwrapper
 *  Returns the precision of the specified column.  Precision is the total
 *  number of digits in a number.  eg: 123.45 has a precision of 5.  For
 *  non-numeric types, it's the number of characters in the string. */
SQLRCLIENT_DLLSPEC
uint32_t	sqlrcur_getColumnPrecisionByName(sqlrcur sqlrcurref,
							const char *col);

/** @ingroup sqlrclientwrapper
 *  Returns the scale of the specified column.  Scale is the total number of
 *  digits to the right of the decimal point in a number.  eg: 123.45 has a
 *  scale of 2. */
SQLRCLIENT_DLLSPEC
uint32_t	sqlrcur_getColumnScaleByIndex(sqlrcur sqlrcurref,
							uint32_t col);

/** @ingroup sqlrclientwrapper
 *  Returns the scale of the specified column.  Scale is the total number of
 *  digits to the right of the decimal point in a number.  eg: 123.45 has a 
 *  scale of 2. */
SQLRCLIENT_DLLSPEC
uint32_t	sqlrcur_getColumnScaleByName(sqlrcur sqlrcurref,
							const char *col);

/** @ingroup sqlrclientwrapper
 *  Returns the scale of the specified column.  Scale is the total number of
 *  digits to the right of the decimal point in a number.  eg: 123.45 has a
 *  scale of 2. */
SQLRCLIENT_DLLSPEC
int	sqlrcur_getColumnIsNullableByIndex(sqlrcur sqlrcurref,
							uint32_t col);

/** @ingroup sqlrclientwrapper
 *  Returns 1 if the specified column can contain nulls and 0 otherwise. */
SQLRCLIENT_DLLSPEC
int	sqlrcur_getColumnIsNullableByName(sqlrcur sqlrcurref,
							const char *col);

/** @ingroup sqlrclientwrapper
 *  Returns 1 if the specified column is a primary key and 0 otherwise. */
SQLRCLIENT_DLLSPEC
int	sqlrcur_getColumnIsPrimaryKeyByIndex(sqlrcur sqlrcurref,
							uint32_t col);

/** @ingroup sqlrclientwrapper
 *  Returns 1 if the specified column is a primary key and 0 otherwise. */
SQLRCLIENT_DLLSPEC
int	sqlrcur_getColumnIsPrimaryKeyByName(sqlrcur sqlrcurref,
							const char *col);

/** @ingroup sqlrclientwrapper
 *  Returns 1 if the specified column is unique and 0 otherwise. */
SQLRCLIENT_DLLSPEC
int		sqlrcur_getColumnIsUniqueByIndex(sqlrcur sqlrcurref,
							uint32_t col);

/** @ingroup sqlrclientwrapper
 *  Returns 1 if the specified column is unique and 0 otherwise. */
SQLRCLIENT_DLLSPEC
int	sqlrcur_getColumnIsUniqueByName(sqlrcur sqlrcurref,
							const char *col);

/** @ingroup sqlrclientwrapper
 *  Returns 1 if the specified column is part of a composite key and 0
 *  otherwise. */
SQLRCLIENT_DLLSPEC
int	sqlrcur_getColumnIsPartOfKeyByIndex(sqlrcur sqlrcurref,
							uint32_t col);

/** @ingroup sqlrclientwrapper
 *  Returns 1 if the specified column is part of a composite key and 0
 *  otherwise. */
SQLRCLIENT_DLLSPEC
int	sqlrcur_getColumnIsPartOfKeyByName(sqlrcur sqlrcurref,
							const char *col);

/** @ingroup sqlrclientwrapper
 *  Returns 1 if the specified column is an unsigned number and 0 otherwise. */
SQLRCLIENT_DLLSPEC
int	sqlrcur_getColumnIsUnsignedByIndex(sqlrcur sqlrcurref,
							uint32_t col);

/** @ingroup sqlrclientwrapper
 *  Returns 1 if the specified column is an unsigned number and 0 otherwise. */
SQLRCLIENT_DLLSPEC
int	sqlrcur_getColumnIsUnsignedByName(sqlrcur sqlrcurref,
							const char *col);

/** @ingroup sqlrclientwrapper
 *  Returns 1 if the specified column was created
 *  with the zero-fill flag and 0 otherwise. */
SQLRCLIENT_DLLSPEC
int	sqlrcur_getColumnIsZeroFilledByIndex(sqlrcur sqlrcurref,
							uint32_t col);

/** @ingroup sqlrclientwrapper
 *  Returns 1 if the specified column was created
 *  with the zero-fill flag and 0 otherwise. */
SQLRCLIENT_DLLSPEC
int	sqlrcur_getColumnIsZeroFilledByName(sqlrcur sqlrcurref,
							const char *col);

/** @ingroup sqlrclientwrapper
 *  Returns 1 if the specified column contains binary data and 0 otherwise. */
SQLRCLIENT_DLLSPEC
int	sqlrcur_getColumnIsBinaryByIndex(sqlrcur sqlrcurref,
							uint32_t col);

/** @ingroup sqlrclientwrapper
 *  Returns 1 if the specified column contains binary data and 0 otherwise. */
SQLRCLIENT_DLLSPEC
int	sqlrcur_getColumnIsBinaryByName(sqlrcur sqlrcurref,
							const char *col);

/** @ingroup sqlrclientwrapper
 *  Returns 1 if the specified column auto-increments and 0 otherwise. */
SQLRCLIENT_DLLSPEC
int	sqlrcur_getColumnIsAutoIncrementByIndex(sqlrcur sqlrcurref,
							uint32_t col);

/** @ingroup sqlrclientwrapper
 *  Returns 1 if the specified column auto-increments and 0 otherwise. */
SQLRCLIENT_DLLSPEC
int	sqlrcur_getColumnIsAutoIncrementByName(sqlrcur sqlrcurref,
							const char *col);

/** @ingroup sqlrclientwrapper
 *  Returns the length of the longest field in the specified column. */
SQLRCLIENT_DLLSPEC
uint32_t	sqlrcur_getLongestByIndex(sqlrcur sqlrcurref, uint32_t col);

/** @ingroup sqlrclientwrapper
 *  Returns the length of the longest field in the specified column. */
SQLRCLIENT_DLLSPEC
uint32_t	sqlrcur_getLongestByName(sqlrcur sqlrcurref, const char *col);



/** @ingroup sqlrclientwrapper
 *  Tells the server to leave this result set open when the connection calls
 *  suspendSession() so that another connection can connect to it using
 *  resumeResultSet() after it calls resumeSession(). */
SQLRCLIENT_DLLSPEC
void	sqlrcur_suspendResultSet(sqlrcur sqlrcurref);

/** @ingroup sqlrclientwrapper
 *  Returns the internal ID of this result set.  This parameter may be passed
 *  to another statement for use in the resumeResultSet() function.  Note: The
 *  value this function returns is only valid after a call to
 *  suspendResultSet().*/
SQLRCLIENT_DLLSPEC
uint16_t	sqlrcur_getResultSetId(sqlrcur sqlrcurref);

/** @ingroup sqlrclientwrapper
 *  Resumes a result set previously left open using suspendSession().
 *  Returns 1 on success and 0 on failure. */
SQLRCLIENT_DLLSPEC
int	sqlrcur_resumeResultSet(sqlrcur sqlrcurref, uint16_t id);

/** @ingroup sqlrclientwrapper
 *  Resumes a result set previously left open using suspendSession() and
 *  continues caching the result set to "filename".  Returns 1 on success and 0
 *  on failure. */
SQLRCLIENT_DLLSPEC
int	sqlrcur_resumeCachedResultSet(sqlrcur sqlrcurref, 
					uint16_t id, const char *filename);

/** @ingroup sqlrclientwrapper
 *  Closes the current result set, if one is open.  Data
 *  that has been fetched already is still available but
 *  no more data may be fetched.  Server side resources
 *  for the result set are freed as well. */
SQLRCLIENT_DLLSPEC
void	sqlrcur_closeResultSet(sqlrcur sqlrcurref);

#include <sqlrelay/private/sqlrclientwrapper.h>

#endif
