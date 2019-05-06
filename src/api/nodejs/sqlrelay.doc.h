// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information.

class SQLRConnection {
	public:
			/** Initiates a connection to "server" on "port"
			 *  or to the unix "socket" on the local machine
			 *  and auths with "user" and "password".
			 *  Failed connections will be retried for 
			 *  "tries" times, waiting "retrytime" seconds
			 *  between each try.  If "tries" is 0 then retries
			 *  will continue forever.  If "retrytime" is 0 then
			 *  retries will be attempted on a default interval.
			 *
			 *  If the "socket" parameter is neither 
			 *  NULL nor "" then an attempt will be made to 
			 *  connect through it before attempting to 
			 *  connect to "server" on "port".  If it is 
			 *  NULL or "" then no attempt will be made to 
			 *  connect through the socket. */
			SQLRConnection(var server, var port,
					var socket,
					var user, var password,
					var retrytime, var tries);



		/** Sets the server connect timeout in seconds and
		 *  milliseconds.  Setting either parameter to -1 disables the
		 *  timeout.  You can also set this timeout using the
		 *  SQLR_CLIENT_CONNECT_TIMEOUT environment variable. */
		function setConnectTimeout(var timeoutsec, var timeoutusec);

		/** Sets the authentication timeout in seconds and
		 *  milliseconds.  Setting either parameter to -1 disables the
		 *  timeout.   You can also set this timeout using the
		 *  SQLR_CLIENT_AUTHENTICATION_TIMEOUT environment variable. */
		function setAuthenticationTimeout(var timeoutsec,
							var timeoutusec);

		/** Sets the response timeout (for queries, commits, rollbacks,
		 *  pings, etc.) in seconds and milliseconds.  Setting either
		 *  parameter to -1 disables the timeout.  You can also set
		 *  this timeout using the SQLR_CLIENT_RESPONSE_TIMEOUT
		 *  environment variable. */
		function setResponseTimeout(var timeoutsec, var timeoutusec);



		/** Sets which delimiters are used to identify bind variables
		 *  in countBindVariables() and validateBinds().  Valid
		 *  delimiters include ?,:,@, and $.  Defaults to "?:@$" */
		function setBindVariableDelimiters(var delimiters);

		/** Returns true if question marks (?) are considered to be
		 *  valid bind variable delimiters. */
		function getBindVariableDelimiterQuestionMarkSupported();

		/** Returns true if colons (:) are considered to be
		 *  valid bind variable delimiters. */
		function getBindVariableDelimiterColonSupported();

		/** Returns true if at-signs (@) are considered to be
		 *  valid bind variable delimiters. */
		function getBindVariableDelimiterAtSignSupported();

		/** Returns true if dollar signs ($) are considered to be
		 *  valid bind variable delimiters. */
		function getBindVariableDelimiterDollarSignSupported();



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
		function enableKerberos(var service, var mech, var flags);

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
		function enableTls(var version,
					var cert, var password, var ciphers,
					var validate, var ca, var depth);

		/** Disables encryption. */
		function disableEncryption();



		/** Ends the session. */
		function endSession();

		/** Disconnects this connection from the current
		 *  session but leaves the session open so 
		 *  that another connection can connect to it 
		 *  using resumeSession(). */
		function suspendSession();

		/** Returns the inet port that the connection is 
		 *  communicating over. This parameter may be 
		 *  passed to another connection for use in
		 *  the resumeSession() method.
		 *  Note: The value this method returns is only
		 *  valid after a call to suspendSession(). */
		function getConnectionPort();

		/** Returns the unix socket that the connection 
		 *  is communicating over. This parameter may be 
		 *  passed to another connection for use in
		 *  the resumeSession() method.
		 *  Note: The value this method returns is only
		 *  valid after a call to suspendSession(). */
		function getConnectionSocket();

		/** Resumes a session previously left open 
		 *  using suspendSession().
		 *  Returns true on success and false on failure. */
		function resumeSession(var port, var socket);



		/** Returns true if the database is up and false
		 *  if it's down. */
		function ping();

		/** Returns the type of database: 
		 *  oracle, postgresql, mysql, etc. */
		function identify();

		/** Returns the version of the database */
		function dbVersion();

		/** Returns the host name of the database */
		function dbHostName();

		/** Returns the ip address of the database */
		function dbIpAddress();

		/** Returns the version of the sqlrelay server software. */
		function serverVersion();

		/** Returns the version of the sqlrelay client software. */
		function clientVersion();

		/** Returns a string representing the format
		 *  of the bind variables used in the db. */
		function bindFormat();



		/** Sets the current database/schema to "database" */
		function selectDatabase(var database);

		/** Returns the database/schema that is currently in use. */
		function getCurrentDatabase();



		/** Returns the value of the autoincrement
		 *  column for the last insert */
		function getLastInsertId();



		/** Instructs the database to perform a commit
		 *  after every successful query. */
		function autoCommitOn();

		/** Instructs the database to wait for the 
		 *  client to tell it when to commit. */
		function autoCommitOff();


		/** Begins a transaction.  Returns true if the begin
		 *  succeeded, false if it failed.  If the database
		 *  automatically begins a new transaction when a
		 *  commit or rollback is issued then this doesn't
		 *  do anything unless SQL Relay is faking transaction
		 *  blocks. */
		function begin();

		/** Commits a transaction.  Returns true if the commit
		 *  succeeded, false if it failed. */
		function commit();

		/** Rolls back a transaction.  Returns true if the rollback
		 *  succeeded, false if it failed. */
		function rollback();



		/** If an operation failed and generated an
		 *  error, the error message is available here.
		 *  If there is no error then this method 
		 *  returns NULL. */
		function errorMessage();

		/** If an operation failed and generated an
		 *  error, the error number is available here.
		 *  If there is no error then this method 
		 *  returns 0. */
		function errorNumber();



		/** Causes verbose debugging information to be 
		 *  sent to standard output.  Another way to do
		 *  this is to start a query with "-- debug\n".
		 *  Yet another way is to set the environment
		 *  variable SQLR_CLIENT_DEBUG to "ON" */
		function debugOn();

		/** Turns debugging off. */
		function debugOff();

		/** Returns false if debugging is off and true
		 *  if debugging is on. */
		function getDebug();



		/** Allows you to specify a file to write debug to.
		 *  Setting "filename" to NULL or an empty string causes debug
		 *  to be written to standard output (the default). */
		function setDebugFile(var filename);


		/** Allows you to set a string that will be passed to the
		 *  server and ultimately included in server-side logging
		 *  along with queries that were run by this instance of
		 *  the client. */
		function setClientInfo(var clientinfo);

		/** Returns the string that was set by setClientInfo(). */
		function getClientInfo();
};


class SQLRCursor {
	public:
			/** Creates a cursor to run queries and fetch result
			 *  sets using connecton "sqlrc". */
			SQLRCursor(var sqlrc);


		/** Sets the number of rows of the result set
		 *  to buffer at a time.  0 (the default)
		 *  means buffer the entire result set. */
		function setResultSetBufferSize(var rows);

		/** Returns the number of result set rows that 
		 *  will be buffered at a time or 0 for the
		 *  entire result set. */
		function getResultSetBufferSize();



		/** Tells the server not to send any column
		 *  info (names, types, sizes).  If you don't
		 *  need that info, you should call this
		 *  method to improve performance. */
		function dontGetColumnInfo();

		/** Tells the server to send column info. */
		function getColumnInfo();


		/** Columns names are returned in the same
		 *  case as they are defined in the database.
		 *  This is the default. */
		function mixedCaseColumnNames();

		/** Columns names are converted to upper case. */
		function upperCaseColumnNames();

		/** Columns names are converted to lower case. */
		function lowerCaseColumnNames();



		/** Sets query caching on.  Future queries
		 *  will be cached to the file "filename".
		 *  
		 *  A default time-to-live of 10 minutes is
		 *  also set.
		 *  
		 *  Note that once cacheToFile() is called,
		 *  the result sets of all future queries will
		 *  be cached to that file until another call 
		 *  to cacheToFile() changes which file to
		 *  cache to or a call to cacheOff() turns off
		 *  caching. */
		function cacheToFile(var filename);

		/** Sets the time-to-live for cached result
		 *  sets. The sqlr-cachemanger will remove each 
		 *  cached result set "ttl" seconds after it's 
		 *  created, provided it's scanning the directory
		 *  containing the cache files. */
		function setCacheTtl(var ttl);

		/** Returns the name of the file containing the
		 *  cached result set. */
		function getCacheFileName();

		/** Sets query caching off. */
		function cacheOff();



		/** Sends a query that returns a list of
		 *  databases/schemas matching "wild".  If wild is empty
		 *  or NULL then a list of all databases/schemas will be
		 *  returned. */
		function getDatabaseList(var wild);

		/** Sends a query that returns a list of tables
		 *  matching "wild".  If wild is empty or NULL then
		 *  a list of all tables will be returned. */
		function getTableList(var wild);

		/** Sends a query that returns a list of columns
		 *  in the table specified by the "table" parameter
		 *  matching "wild".  If wild is empty or NULL then
		 *  a list of all columns will be returned. */
		function getColumnList(var table, var wild);



		/** Sends "query" directly and gets a result set. */
		function sendQuery(var query);

		/** Sends "query" with length "length" directly
		 *  and gets a result set. This method must be used
		 *  if the query contains binary data. */
		function sendQuery(var query, var length);

		/** Sends the query in file "path"/"filename" directly
		 *  and gets a result set. */
		function sendFileQuery(var path, var filename); 



		/** Prepare to execute "query". */
		function prepareQuery(var query);

		/** Prepare to execute "query" with length 
		 *  "length".  This method must be used if the
		 *  query contains binary data. */
		function prepareQuery(var query, var length);

		/** Prepare to execute the contents 
		 *  of "path"/"filename".  Returns false if the
		 * // file couldn't be opened. */
		function prepareFileQuery(var path,
						var filename);



		/** Defines a string substitution variable. */
		function substitution(var variable, var value);

		/** Defines an integer substitution variable. */
		function substitution(var variable, var value);

		/** Defines a decimal substitution variable. */
		function substitution(var variable, var value, 
							var precision, 
							var scale);

		/** Defines an array of string substitution variables. */
		function substitutions(var variables,
						var values);

		/** Defines an array of integer substitution variables. */
		function substitutions(var variables,
						var values);

		/** Defines an array of decimal substitution variables. */
		function substitutions(var variables,
						var values,
						var precisions, 
						var scales);



		/** Defines a string input bind variable. */
		function inputBind(var variable, var value);

		/** Defines a string input bind variable. */
		function inputBind(var variable, var value, var valuelength);

		/** Defines a integer input bind variable. */
		function inputBind(var variable, var value);

		/** Defines a decimal input bind variable.
		  * (If you don't have the precision and scale then set
		  * them both 0.  However in that case you may get
		  * unexpected rounding behavior if the server is faking
		  * binds.) */
		function inputBind(var variable, var value, 
							var precision, 
							var scale);

		/** Defines a date input bind variable.  "day" should be
		 *  1-31 and "month" should be 1-12.  "tz" may be left NULL.
		 *  Most databases ignore "tz".  */
		function inputBind(var variable,
				var year, var month, var day,
				var hour, var minute, var second,
				var microsecond, var tz);

		/** Defines a binary lob input bind variable. */
		function inputBindBlob(var variable,
						var value,
						var size);

		/** Defines a character lob input bind variable. */
		function inputBindClob(var variable,
						var value,
						var size);

		/** Defines an array of string input bind variables. */
		function inputBinds(var variables, var values);

		/** Defines an array of integer input bind variables. */
		function inputBinds(var variables,
					const var values);

		/** Defines an array of decimal input bind variables. */
		function inputBinds(var variables,
					const var values, 
					const var precisions, 
					const var scales);



		/** Defines an output bind variable.
		 *  "bufferlength" bytes will be reserved
		 *  to store the value. */
		function defineOutputBindString(var variable,
						var bufferlength);

		/** Defines an integer output bind variable. */
		function defineOutputBindInteger(var variable);

		/** Defines a decimal output bind variable. */
		function defineOutputBindDouble(var variable);

		/** Defines a date output bind variable. */
		function defineOutputBindDate(var variable);

		/** Defines a binary lob output bind variable. */
		function defineOutputBindBlob(var variable);

		/** Defines a character lob output bind variable. */
		function defineOutputBindClob(var variable);

		/** Defines a cursor output bind variable. */
		function defineOutputBindCursor(var variable);



		/** Clears all bind variables. */
		function clearBinds();

		/** Parses the previously prepared query,
		 *  counts the number of bind variables defined
		 *  in it and returns that number. */
		function countBindVariables() const;

		/** If you are binding to any variables that 
		 *  might not actually be in your query, call 
		 *  this to ensure that the database won't try 
		 *  to bind them unless they really are in the 
		 *  query.  There is a performance penalty for
		 *  calling this method. */
		function validateBinds();

		/** Returns true if "variable" was a valid
		 *  bind variable of the query. */
		function validBind(var variable);



		/** Execute the query that was previously 
		 *  prepared and bound. */
		function executeQuery();

		/** Fetch from a cursor that was returned as
		 *  an output bind variable. */
		function fetchFromBindCursor();



		/** Get the value stored in a previously
		 *  defined string output bind variable. */
		function getOutputBindString(var variable);

		/** Get the value stored in a previously
		 *  defined integer output bind variable. */
		function getOutputBindInteger(var variable);

		/** Get the value stored in a previously
		 *  defined decimal output bind variable. */
		function getOutputBindDouble(var variable);

		/** Get the value stored in a previously
		 *  defined date output bind variable. */
		function getOutputBindDate(var variable,
							var year,
							var month,
							var day,
							var hour,
							var minute,
							var second,
							var microsecond,
							var tz);

		/** Get the value stored in a previously
		 *  defined binary lob output bind variable. */
		function getOutputBindBlob(var variable);

		/** Get the value stored in a previously
		 *  defined character lob output bind variable. */
		function getOutputBindClob(var variable);

		/** Get the length of the value stored in a
		 *  previously defined output bind variable. */
		function getOutputBindLength(var variable);

		/** Get the cursor associated with a previously
		 *  defined output bind variable. */
		function getOutputBindCursor(var variable);


		
		/** Opens a cached result set.
		 *  Returns true on success and false on failure. */
		function openCachedResultSet(var filename);



		/** Returns the number of columns in the current
		 *  result set. */
		function colCount();

		/** Returns the number of rows in the current 
		 *  result set (if the result set is being
		 *  stepped through, this returns the number
		 *  of rows processed so far). */
		function rowCount();

		/** Returns the total number of rows that will 
		 *  be returned in the result set.  Not all 
		 *  databases support this call.  Don't use it 
		 *  for applications which are designed to be 
		 *  portable across databases.  0 is returned
		 *  by databases which don't support this option. */
		function totalRows();

		/** Returns the number of rows that were 
		 *  updated, inserted or deleted by the query.
		 *  Not all databases support this call.  Don't 
		 *  use it for applications which are designed 
		 *  to be portable across databases.  0 is 
		 *  returned by databases which don't support 
		 *  this option. */
		function affectedRows();

		/** Returns the index of the first buffered row.
		 *  This is useful when buffering only part of
		 *  the result set at a time. */
		function firstRowIndex();

		/** Returns false if part of the result set is
		 *  still pending on the server and true if not.
		 *  This method can only return false if 
		 *  setResultSetBufferSize() has been called
		 *  with a parameter other than 0. */
		function endOfResultSet();
		
		

		/** If a query failed and generated an error, 
		 *  the error message is available here.  If 
		 *  the query succeeded then this method 
		 *  returns NULL. */
		function errorMessage();

		/** If a query failed and generated an
		 *  error, the error number is available here.
		 *  If there is no error then this method 
		 *  returns 0. */
		function errorNumber();



		/** Tells the connection to return NULL fields
		 *  and output bind variables as empty strings. 
		 *  This is the default. */
		function getNullsAsEmptyStrings();

		/** Tells the connection to return NULL fields
		 *  and output bind variables as NULL's rather
		 *  than as empty strings. */
		function getNullsAsNulls();



		/** Returns the specified field as a string. */
		function getField(var row, var col);

		/** Returns the specified field as a string */
		function getField(var row, var col);

		/** Returns the specified field as an integer. */
		function getFieldAsInteger(var row, var col);

		/** Returns the specified field as an integer. */
		function getFieldAsInteger(var row, var col);

		/** Returns the specified field as a decimal. */
		function getFieldAsDouble(var row, var col);

		/** Returns the specified field as a decimal. */
		function getFieldAsDouble(var row, var col);



		/** Returns the length of the specified field. */
		function getFieldLength(var row, var col);

		/** Returns the length of the specified field. */
		function getFieldLength(var row, var col);



		/** Returns a null terminated array of the 
		 *  values of the fields in the specified row. */
		function  getRow(var row);

		/** Returns a null terminated array of the 
		 *  lengths of the fields in the specified row. */
		function getRowLengths(var row);

		/** Returns a null terminated array of the 
		 *  column names of the current result set. */
		function  getColumnNames();

		/** Returns the name of the specified column. */
		function getColumnName(var col);

		/** Returns the type of the specified column. */
		function getColumnType(var col);

		/** Returns the type of the specified column. */
		function getColumnType(var col);

		/** Returns the number of bytes required on
		 *  the server to store the data for the specified column */
		function getColumnLength(var col);

		/** Returns the number of bytes required on
		 *  the server to store the data for the specified column */
		function getColumnLength(var col);

		/** Returns the precision of the specified
		 *  column.
		 *  Precision is the total number of digits in
		 *  a number.  eg: 123.45 has a precision of 5.
		 *  For non-numeric types, it's the number of
		 *  characters in the string. */
		function getColumnPrecision(var col);

		/** Returns the precision of the specified
		 *  column.
		 *  Precision is the total number of digits in
		 *  a number.  eg: 123.45 has a precision of 5.
		 *  For non-numeric types, it's the number of
		 *  characters in the string. */
		function getColumnPrecision(var col);

		/** Returns the scale of the specified column.
		 *  Scale is the total number of digits to the
		 *  right of the decimal point in a number.
		 *  eg: 123.45 has a scale of 2. */
		function getColumnScale(var col);

		/** Returns the scale of the specified column.
		 *  Scale is the total number of digits to the
		 *  right of the decimal point in a number.
		 *  eg: 123.45 has a scale of 2. */
		function getColumnScale(var col);

		/** Returns true if the specified column can
		 *  contain nulls and false otherwise. */
		function getColumnIsNullable(var col);

		/** Returns true if the specified column can
		 *  contain nulls and false otherwise. */
		function getColumnIsNullable(var col);

		/** Returns true if the specified column is a
		 *  primary key and false otherwise. */
		function getColumnIsPrimaryKey(var col);

		/** Returns true if the specified column is a
		 *  primary key and false otherwise. */
		function getColumnIsPrimaryKey(var col);

		/** Returns true if the specified column is
		 *  unique and false otherwise. */
		function getColumnIsUnique(var col);

		/** Returns true if the specified column is
		 *  unique and false otherwise. */
		function getColumnIsUnique(var col);

		/** Returns true if the specified column is
		 *  part of a composite key and false otherwise. */
		function getColumnIsPartOfKey(var col);

		/** Returns true if the specified column is
		 *  part of a composite key and false otherwise. */
		function getColumnIsPartOfKey(var col);

		/** Returns true if the specified column is
		 *  an unsigned number and false otherwise. */
		function getColumnIsUnsigned(var col);

		/** Returns true if the specified column is
		 *  an unsigned number and false otherwise. */
		function getColumnIsUnsigned(var col);

		/** Returns true if the specified column was
		 *  created with the zero-fill flag and false
		 *  otherwise. */
		function getColumnIsZeroFilled(var col);

		/** Returns true if the specified column was
		 *  created with the zero-fill flag and false
		 *  otherwise. */
		function getColumnIsZeroFilled(var col);

		/** Returns true if the specified column
		 *  contains binary data and false
		 *  otherwise. */
		function getColumnIsBinary(var col);

		/** Returns true if the specified column
		 *  contains binary data and false
		 *  otherwise. */
		function getColumnIsBinary(var col);

		/** Returns true if the specified column
		 *  auto-increments and false otherwise. */
		function getColumnIsAutoIncrement(var col);

		/** Returns true if the specified column
		 *  auto-increments and false otherwise. */
		function getColumnIsAutoIncrement(var col);

		/** Returns the length of the longest field
		 *  in the specified column. */
		function getLongest(var col);

		/** Returns the length of the longest field
		 *  in the specified column. */
		function getLongest(var col);



		/** Tells the server to leave this result
		 *  set open when the connection calls 
		 *  suspendSession() so that another connection 
		 *  can connect to it using resumeResultSet() 
		 *  after it calls resumeSession(). */
		function suspendResultSet();

		/** Returns the internal ID of this result set.
		 *  This parameter may be passed to another 
		 *  cursor for use in the resumeResultSet() 
		 *  method.
		 *  Note: The value this method returns is only
		 *  valid after a call to suspendResultSet(). */
		function getResultSetId();

		/** Resumes a result set previously left open 
		 *  using suspendSession().
		 *  Returns true on success and false on failure. */
		function resumeResultSet(var id);

		/** Resumes a result set previously left open
		 *  using suspendSession() and continues caching
		 *  the result set to "filename".
		 *  Returns true on success and false on failure. */
		function resumeCachedResultSet(var id,
						var filename);

		/** Closes the current result set, if one is open.  Data
		 *  that has been fetched already is still available but
		 *  no more data may be fetched.  Server side resources
		 *  for the result set are freed as well. */
		function closeResultSet();
};
