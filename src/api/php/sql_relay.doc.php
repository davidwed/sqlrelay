<?php

/** 
 *  Initiates a connection to "server" on "port" or to the unix "socket" on
 *  the local machine and auths with "user" and "password".  Failed
 *  connections will be retried for "tries" times, waiting "retrytime" seconds
 *  between each try.  If "tries" is 0 then retries will continue forever.  If
 *  "retrytime" is 0 then retries will be attempted on a default interval.  If
 *  the "socket" parameter is nether NULL nor "" then an attempt will be made
 *  to connect through it before attempting to connect to "server" on "port".
 *  If it is NULL or "" then no attempt will be made to connect through the
 *  socket.*/
function sqlrcon_alloc($server, $port, $socket, $user, $password, $retrytime, $tries){}

/** 
 *  Disconnects and ends the session if it hasn't been terminated already. */
function sqlrcon_free($sqlrconref){}



/** 
 *  Sets the server connect timeout in seconds and
 *  milliseconds.  Setting either parameter to -1 disables the
 *  timeout.  You can also set this timeout using the
 *  SQLR_CLIENT_CONNECT_TIMEOUT environment variable. */
function sqlrcon_setConnectTimeout($sqlrconref, $timeoutsec, $timeoutusec){}

/**
 *  Sets the authentication timeout in seconds and
 *  milliseconds.  Setting either parameter to -1 disables the
 *  timeout.   You can also set this timeout using the
 *  SQLR_CLIENT_AUTHENTICATION_TIMEOUT environment variable. */
function sqlrcon_setAuthenticationTimeout($sqlrconref, $timeoutsec, $timeoutusec){}

/**
 *  Sets the response timeout (for queries, commits, rollbacks,
 *  pings, etc.) in seconds and milliseconds.  Setting either
 *  parameter to -1 disables the timeout.  You can also set
 *  this timeout using the SQLR_CLIENT_RESPONSE_TIMEOUT
 *  environment variable. */
function sqlrcon_setResponseTimeout($sqlrconref, $timeoutsec, $timeoutusec){}

/**
 *  Sets which delimiters are used to identify bind variables
 *  in countBindVariables() and validateBinds().  Valid
 *  delimiters include ?,:,@, and $.  Defaults to "?:@$" */
function setBindVariableDelimiters($delimiters){}

/**
 *  Returns true if question marks (?) are considered to be
 *  valid bind variable delimiters. */
function getBindVariableDelimiterQuestionMarkSupported(){}

/**
 *  Returns true if colons (:) are considered to be
 *  valid bind variable delimiters. */
function getBindVariableDelimiterColonSupported(){}

/**
 *  Returns true if at-signs (@) are considered to be
 *  valid bind variable delimiters. */
function getBindVariableDelimiterAtSignSupported(){}

/**
 *  Returns true if dollar signs ($) are considered to be
 *  valid bind variable delimiters. */
function getBindVariableDelimiterDollarSignSupported(){}

/** Enables Kerberos authentication and encryption.
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
function sqlrcon_enableKerberos($sqlrconref, $service, $mech, $flags){}

/** Enables TLS/SSL encryption, and optionally authentication.
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
function sqlrcon_enableTls($sqlrconref, $version, $cert, $password, $ciphers, $validate, $ca, $depth){}

/** Disables encryption. */
function sqlrcon_disableEncryption($sqlrconref){}

/** 
 *  Ends the session. */
function sqlrcon_endSession($sqlrconref){}

/** 
 *  Disconnects this connection from the current session but leaves the session
 *  open so that another connection can connect to it using
 *  sqlrcon_resumeSession(). */
function sqlrcon_suspendSession($sqlrconref){}

/** 
 *  Returns the inet port that the connection is communicating over.  This
 *  parameter may be passed to another connection for use in the
 *  sqlrcon_resumeSession() command.  Note: The result this function returns
 *  is only valid after a call to suspendSession(). */
function sqlrcon_getConnectionPort($sqlrconref){}

/** 
 *  Returns the unix socket that the connection is communicating over.  This
 *  parameter may be passed to another connection for use in the
 *  sqlrcon_resumeSession() command.  Note: The result this function returns
 *  is only valid after a call to suspendSession(). */
function sqlrcon_getConnectionSocket($sqlrconref){}

/** 
 *  Resumes a session previously left open using sqlrcon_suspendSession().
 *  Returns 1 on success and 0 on failure. */
function sqlrcon_resumeSession($sqlrconref, $port, $socket){}



/** 
 *  Returns 1 if the database is up and 0 if it's down. */
function sqlrcon_ping($sqlrconref){}

/** 
 *  Returns the type of database: oracle, postgresql, mysql, etc. */
function sqlrcon_identify($sqlrconref){}

/** 
 *  Returns the version of the database */
function sqlrcon_dbVersion($sqlrconref){}

/** 
 *  Returns the host name of the database */
function sqlrcon_dbHostName($sqlrconref){}

/** 
 *  Returns the ip address of the database */
function sqlrcon_dbIpAddress($sqlrconref){}

/** 
 *  Returns the version of the sqlrelay server software. */
function sqlrcon_serverVersion($sqlrconref){}

/** 
 *  Returns the version of the sqlrelay client software. */
function sqlrcon_clientVersion($sqlrconref){}

/** 
 *  Returns a string representing the format
 *  of the bind variables used in the db. */
function sqlrcon_bindFormat($sqlrconref){}



/** 
 *  Sets the current database/schema to "database" */
function sqlrcon_selectDatabase($sqlrconref, $database){}

/** 
 *  Returns the database/schema that is currently in use. */
function sqlrcon_getCurrentDatabase($sqlrconref){}



/** 
 *  Returns the value of the autoincrement column for the last insert */
function sqlrcon_getLastInsertId($sqlrconref){}



/** 
 *  Instructs the database to perform a commit after every successful query. */
function sqlrcon_autoCommitOn($sqlrconref){}

/** 
 *  Instructs the database to wait for the client to tell it when to commit. */
function sqlrcon_autoCommitOff($sqlrconref){}



/** 
 *  Issues a commit.  Returns 1 if the commit succeeded, 0 if it failed and -1
 *  if an error occurred. */
function sqlrcon_commit($sqlrconref){}

/** 
 *  Issues a rollback.  Returns 1 if the rollback succeeded, 0 if it failed
 *  and -1 if an error occurred. */
function sqlrcon_rollback($sqlrconref){}



/** 
 *  If an operation failed and generated an error, the error message is
 *  available here.  If there is no error then this method returns NULL */
function sqlrcon_errorMessage($sqlrconref){}


/**
 *  If an operation failed and generated an error, the error number is
 *  available here.  If there is no error then this method returns 0. */
function sqlrcon_errorNumber($sqlrconref){}



/** 
 *  Causes verbose debugging information to be sent to standard output.
 *  Another way to do this is to start a query with "-- debug\n".
 *  Yet another way is to set the environment variable SQLR_CLIENT_DEBUG
 *  to "ON" */
function sqlrcon_debugOn($sqlrconref){}

/** 
 *  Turns debugging off. */
function sqlrcon_debugOff($sqlrconref){}

/** 
 *  Returns 0 if debugging is off and 1 if debugging is on. */
function sqlrcon_getDebug($sqlrconref){}

/** Allows you to specify a file to write debug to.
 *  Setting "filename" to NULL or an empty string causes debug
 *  to be written to standard output (the default). */
function sqlrcon_setDebugFile($sqlrconref, $filename){}

/** Allows you to set a string that will be passed to the
 *  server and ultimately included in server-side logging
 *  along with queries that were run by this instance of
 *  the client. */
function sqlrcon_setClientInfo($sqlrconref, $clientinfo){}

/** Returns the string that was set by setClientInfo(). */
function sqlrcon_getClientInfo($sqlrconref){}





/** 
 *  Creates a cursor to run queries and fetch
 *  result sets using connection "sqlrconref" */
function sqlrcur_alloc($sqlrconref){}

/** 
 *  Destroys the cursor and cleans up all associated result set data. *//
function sqlrcur_free($sqlrcurref){}



/** 
 *  Sets the number of rows of the result set to buffer at a time.
 *  0 (the default) means buffer the entire result set. */
function sqlrcur_setResultSetBufferSize($sqlrcurref, $rows){}

/** 
 *  Returns the number of result set rows that will be buffered at a time or
 *  0 for the entire result set. */
function sqlrcur_getResultSetBufferSize($sqlrcurref){}



/** 
 *  Tells the server not to send any column info (names, types, sizes).  If
 *  you don't need that info, you should call this function to improve
 *  performance. */
function sqlrcur_dontGetColumnInfo($sqlrcurref){}

/** 
 *  Tells the server to send column info. */
function sqlrcur_getColumnInfo($sqlrcurref){}



/** 
 *  Columns names are returned in the same case as they are defined in the
 *  database.  This is the default. */
function sqlrcur_mixedCaseColumnNames($sqlrcurref){}

/** 
 *  Columns names are converted to upper case. */
function sqlrcur_upperCaseColumnNames($sqlrcurref){}

/** 
 *  Columns names are converted to lower case. */
function sqlrcur_lowerCaseColumnNames($sqlrcurref){}



/** 
 *  Sets query caching on.  Future queries will be cached to the
 *  file "filename".
 * 
 *  A default time-to-live of 10 minutes is also set.
 * 
 *  Note that once sqlrcur_cacheToFile() is called, the result sets of all
 *  future queries will be cached to that file until another call to
 *  sqlrcur_cacheToFile() changes which file to cache to or a call to
 *  sqlrcur_cacheOff() turns off caching. */
function sqlrcur_cacheToFile($sqlrcurref, $filename){}

/** 
 *  Sets the time-to-live for cached result sets. The sqlr-cachemanger will
 *  remove each cached result set "ttl" seconds after it's created, provided
 *  it's scanning the directory containing the cache files. */
function sqlrcur_setCacheTtl($sqlrcurref, $ttl){}

/** 
 *  Returns the name of the file containing
 *  the most recently cached result set. */
function sqlrcur_getCacheFileName($sqlrcurref){}

/** 
 *  Sets query caching off. */
function sqlrcur_cacheOff($sqlrcurref){}



/** 
 *  Sends a query that returns a list of databases/schemas matching "wild".
 *  If wild is empty or NULL then a list of all databases/schemas will be
 *  returned. */
function sqlrcur_getDatabaseList($sqlrcurref, $wild){}

/** 
 *  Sends a query that returns a list of tables matching "wild".  If wild is
 *  empty or NULL then a list of all tables will be returned. */
function sqlrcur_getTableList($sqlrcurref, $wild){}

/** 
 *  Sends a query that returns a list of columns in the table specified by the
 *  "table" parameter matching "wild".  If wild is empty or NULL then a list of
 *  all columns will be returned. */
function sqlrcur_getColumnList($sqlrcurref, $table, $wild){}



/** 
 *  Sends "query" directly and gets a result set. */
function sqlrcur_sendQuery($sqlrcurref, $query){}

/** 
 *  Sends "query" with length "length" directly and gets a result set. This
 *  function must be used if the query contains binary data. */
function sqlrcur_sendQueryWithLength($sqlrcurref, $query, $length){}

/** 
 *  Sends the query in file "path"/"filename" and gets a result set. */
function sqlrcur_sendFileQuery($sqlrcurref, $path, $filename){}



/** 
 *  Prepare to execute "query". */
function sqlrcur_prepareQuery($sqlrcurref, $query){}

/** 
 *  Prepare to execute "query" with length "length".  This function must be
 *  used if the query contains binary data. */
function sqlrcur_prepareQueryWithLength($sqlrcurref, $query, $length){}

/** 
 *  Prepare to execute the contents of "path"/"filename". */
function sqlrcur_prepareFileQuery($sqlrcurref, $path, $filename){}

/** 
 *  Defines a substitution variable.  The value may be a string,
 *  integer or decimal.  If it is a decimal, then precision and scale may
 *  also be specified */
function sqlrcur_substitution($sqlrcurref, $variable, $value, $precision, $scale){}

/** 
 *  Defines an array of substitution variables.  The values may be
 *  strings, integers or decimals.  If they are decimals, then precisions and
 *  scales may also be specified */
function sqlrcur_substitution($sqlrcurref, $variable, $value){}

/** 
 *  Defines an input bind variable.  The value may be a string,
 *  integer or decimal.  If the value is a decimal, then precision and scale may
 *  also be specified.  If you don't have the precision and scale then set them
 *  both to 0.  However in that case you may get unexpected rounding behavior
 *  if the server is faking binds. */
function sqlrcur_inputBind($sqlrcurref, $variable, $value, $precision, $scale){}

/** 
 *  Defines an input bind variables.  The values may be a strings,
 *  integers or decimals.  If they are a decimals, then precisions and
 *  scales may also be specified */
function sqlrcur_inputBind($sqlrcurref, $variable, $value, $precision, $scale){}

/** 
 *  Defines a binary lob input bind variable. */
function sqlrcur_inputBindBlob($sqlrcurref, $variable, $value, $size){}

/** 
 *  Defines a character lob input bind variable. */
function sqlrcur_inputBindClob($sqlrcurref, $variable, $value, $size){}



/** 
 *  Defines a string output bind variable.
 *  "length" bytes will be reserved to store the value. */
function sqlrcur_defineOutputBindString($sqlrcurref, $variable, $length){}

/** 
 *  Defines an integer output bind variable. */
function sqlrcur_defineOutputBindInteger($sqlrcurref, $variable){}

/** 
 *  Defines an decimal output bind variable. */
function sqlrcur_defineOutputBindDouble($sqlrcurref, $variable){}

/** 
 *  Defines a binary lob output bind variable */
function sqlrcur_defineOutputBindBlob($sqlrcurref, $variable){}

/** 
 *  Defines a character lob output bind variable */
function sqlrcur_defineOutputBindClob($sqlrcurref, $variable){}

/** 
 *  Defines a cursor output bind variable */
function sqlrcur_defineOutputBindCursor($sqlrcurref, $variable){}



/** 
 *  Clears all bind variables. */
function sqlrcur_clearBinds($sqlrcurref){}

/** 
 *  Parses the previously prepared query, counts the number of bind variables
 *  defined in it and returns that number. */
function sqlrcur_countBindVariables($sqlrcurref){}

/** 
 *  If you are binding to any variables that might not actually be in your
 *  query, call this to ensure that the database won't try to bind them unless
 *  they really are in the query.  There is a performance penalty for calling
 *  this function */
function sqlrcur_validateBinds($sqlrcurref){}

/** 
 *  Returns true if "variable" was a valid bind variable of the query. */
function sqlrcur_validBind($sqlrcurref, $variable){}



/** 
 *  Execute the query that was previously prepared and bound. */
function sqlrcur_executeQuery($sqlrcurref){}

/** 
 *  Fetch from a cursor that was returned as an output bind variable. */
function sqlrcur_fetchFromBindCursor($sqlrcurref){}



/** 
 *  Get the value stored in a previously defined
 *  string output bind variable. */
function sqlrcur_getOutputBindString($sqlrcurref, $variable){}

/** 
 *  Get the value stored in a previously defined
 *  integer output bind variable. */
function sqlrcur_getOutputBindInteger($sqlrcurref, $variable){}

/** 
 *  Get the value stored in a previously defined
 *  decimal output bind variable. */
function sqlrcur_getOutputBindDouble($sqlrcurref, $variable){}

/** 
 *  Get the value stored in a previously defined
 *  binary lob output bind variable. */
function sqlrcur_getOutputBindBlob($sqlrcurref, $variable){}

/** 
 *  Get the value stored in a previously defined
 *  character lob output bind variable. */
function sqlrcur_getOutputBindClob($sqlrcurref, $variable){}

/** 
 *  Get the length of the value stored in a previously
 *  defined output bind variable. */
function sqlrcur_getOutputBindLength($sqlrcurref, $variable){}

/** 
 *  Get the cursor associated with a previously defined output bind variable. */
sqlrcur	sqlrcur_getOutputBindCursor($sqlrcurref, $variable){}



/** 
 *  Opens a cached result set.  Returns 1 on success and 0 on failure. */
function sqlrcur_openCachedResultSet($sqlrcurref, $filename){}



/** 
 *  Returns the number of columns in the current result set. */
function sqlrcur_colCount($sqlrcurref){}

/** 
 *  Returns the number of rows in the current result set. */
function sqlrcur_rowCount($sqlrcurref){}

/** 
 *  Returns the total number of rows that will be returned in the result set.
 *  Not all databases support this call.  Don't use it for applications which
 *  are designed to be portable across databases.  -1 is returned by databases
 *  which don't support this option. */
function sqlrcur_totalRows($sqlrcurref){}

/** 
 *  Returns the number of rows that were updated, inserted or deleted by the
 *  query.  Not all databases support this call.  Don't use it for applications
 *  which are designed to be portable across databases.  -1 is returned by
 *  databases which don't support this option. */
function sqlrcur_affectedRows($sqlrcurref){}

/** 
 *  Returns the index of the first buffered row.  This is useful when buffering
 *  only part of the result set at a time. */
function sqlrcur_firstRowIndex($sqlrcurref){}

/** 
 *  Returns 0 if part of the result set is still pending on the server and 1 if
 *  not.  This function can only return 0 if setResultSetBufferSize() has been
 *  called with a parameter other than 0. */
function sqlrcur_endOfResultSet($sqlrcurref){}



/** 
 *  If a query failed and generated an error, the error message is available
 *  here.  If the query succeeded then this function returns a NULL. */
function sqlrcur_errorMessage($sqlrcurref){}


/**
 *  If a query failed and generated an error, the error number is available
 *  here.  If there is no error then this method returns 0. */
function sqlrcur_errorNumber($sqlrcurref){}



/** 
 *  Tells the connection to return NULL fields and output bind variables as
 *  empty strings.  This is the default. */
function sqlrcur_getNullsAsEmptyStrings($sqlrcurref){}

/** 
 *  Tells the connection to return NULL fields
 *  and output bind variables as NULL's. */
function sqlrcur_getNullsAsNulls($sqlrcurref){}



/** 
 *  Returns the specified field as a string. "col" may be specified as the
 *  column name or number. */
function sqlrcur_getField($sqlrcurref, $row, $col){}

/** 
 *  Returns the specified field as an integer. "col" may be specified as the
 *  column name or number. */
function sqlrcur_getFieldAsInteger($sqlrcurref, $row, $col){}

/** 
 *  Returns the specified field as an decimal. "col" may be specified as the
 *  column name or number. */
function sqlrcur_getFieldAsDouble($sqlrcurref, $row, $col){}



/** 
 *  Returns the length of the specified row and column. "col" may be
 *  specified as the column name or number. */
function sqlrcur_getFieldLength($sqlrcurref, $row, $col){}



/** 
 *  Returns an array of the values of the fields in the specified row. */
function sqlrcur_getRow($sqlrcurref, $row){}

/** 
 *  Returns an associative array of the
 *  values of the fields in the specified row. */
function sqlrcur_getRowAssoc($sqlrcurref, $row){}

/** 
 *  Returns an array of the lengths of the fields in the specified row. */
function *sqlrcur_getRowLengths($sqlrcurref, $row){}

/** 
 *  Returns an associative array of the
 *  lengths of the fields in the specified row. */
function sqlrcur_getRowLenghtsAssoc($sqlrcurref, $row){}

/** 
 *  Returns an array of the column names of the current result set. */
function sqlrcur_getColumnNames($sqlrcurref){}

/** 
 *  Returns the name of the specified column. */
function sqlrcur_getColumnName($sqlrcurref, $col){}

/** 
 *  Returns the type of the specified column.  "col" may be specified as the
 *  column name or number. */
function sqlrcur_getColumnType($sqlrcurref, $col){}

/** 
 *  Returns the length of the specified column.  "col" may be specified as the
 *  column name or number. */
function sqlrcur_getColumnLength($sqlrcurref, $col){}

/** 
 *  Returns the precision of the specified column.  Precision is the total
 *  number of digits in a number.  eg: 123.45 has a precision of 5.  For
 *  non-numeric types, it's the number of characters in the string.  "col"
 * may be specified as the column name or number. */
function sqlrcur_getColumnPrecision($sqlrcurref, $col){}

/** 
 *  Returns the scale of the specified column.  Scale is the total number of
 *  digits to the right of the decimal point in a number.  eg: 123.45 has a
 *  scale of 2.  "col" may be specified as the column name or number. */
function sqlrcur_getColumnScale($sqlrcurref, $col){}

/** 
 *  Returns the scale of the specified column.  Scale is the total number of
 *  digits to the right of the decimal point in a number.  eg: 123.45 has a
 *  scale of 2.  "col" may be specified as the column name or number. */
function sqlrcur_getColumnIsNullable($sqlrcurref, $col){}

/** 
 *  Returns 1 if the specified column is a primary key and 0 otherwise.
 *  "col" may be specified as the column name or number. */
function sqlrcur_getColumnIsPrimaryKey($sqlrcurref, $col){}

/** 
 *  Returns 1 if the specified column is unique and 0 otherwise.  "col"
 *  may be specified as the column name or number. */
function sqlrcur_getColumnIsUnique($sqlrcurref, $col){}

/** 
 *  Returns 1 if the specified column is part of a composite key and 0
 *  otherwise.  "col" may be specified as the column name or number. */
function sqlrcur_getColumnIsPartOfKey($sqlrcurref, $col){}

/** 
 *  Returns 1 if the specified column is an unsigned number and 0 otherwise.
 *  "col" may be specified as the column name or number. */
function sqlrcur_getColumnIsUnsigned($sqlrcurref, $col){}

/** 
 *  Returns 1 if the specified column is zero-filled and 0 otherwise.
 *  "col" may be specified as the with the zero-fill flag and 0 otherwise. */
function sqlrcur_getColumnIsZeroFilled($sqlrcurref, $col){}

/** 
 *  Returns 1 if the specified column contains binary data and 0 otherwise.
 *  "col" may be specified as the column name or number. */
function sqlrcur_getColumnIsBinary($sqlrcurref, $col){}

/** 
 *  Returns 1 if the specified column auto-increments and 0 otherwise.
 *  "col" may be specified as the column name or number. */
function sqlrcur_getColumnIsAutoIncrement($sqlrcurref, $col){}

/** 
 *  Returns the length of the longest field in the specified column.
 *  "col" may be specified as the column name or number. */
function sqlrcur_getLongest($sqlrcurref, $col){}



/** 
 *  Tells the server to leave this result set open when the connection calls
 *  suspendSession() so that another connection can connect to it using
 *  resumeResultSet() after it calls resumeSession(). */
function sqlrcur_suspendResultSet($sqlrcurref){}

/** 
 *  Returns the internal ID of this result set.  This parameter may be passed
 *  to another statement for use in the resumeResultSet() function.  Note: The
 *  value this function returns is only valid after a call to
 *  suspendResultSet().*/
function sqlrcur_getResultSetId($sqlrcurref){}

/** 
 *  Resumes a result set previously left open using suspendSession().
 *  Returns 1 on success and 0 on failure. */
function sqlrcur_resumeResultSet($sqlrcurref, $id){}

/** 
 *  Resumes a result set previously left open using suspendSession() and
 *  continues caching the result set to "filename".  Returns 1 on success and 0
 *  on failure. */
function sqlrcur_resumeCachedResultSet($sqlrcurref, $id, $filename){}

/**
 *  Closes the current result set, if one is open.  Data
 *  that has been fetched already is still available but
 *  no more data may be fetched.  Server side resources
 *  for the result set are freed as well. */
function sqlrcur_closeResultSet($sqlrcurref){}

?>
