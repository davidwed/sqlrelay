# Copyright (c) 2000 Roman Milner
# See the file COPYING for more information

from SQLRelay import CSQLRelay

def getNumericFieldsAsStrings():
    """
    Instructs the API to return numeric fields as strings.  This is the
    default setting.  Truncation cannot occur if this setting is used.
    """
    CSQLRelay.getNumericFieldsAsStrings()

def getNumericFieldsAsNumbers():
    """
    Instructs the API to return numeric fields as numbers.  Integer fields will
    be returned as integers and floating point fields will be returned as
    Decimals if that class is avialable or floats otherwise.  The drawback to
    using numerics is that under some circumstances truncation can occur.
    """
    CSQLRelay.getNumericFieldsAsNumbers()


class sqlrconnection:

    """
    A wrapper for the sqlrelay connection API.  Closely follows the C++ API.
    """

    def __init__(self, host, port, socket, user, password, retrytime=0, tries=1):
        """ 
        Opens a connection to the sqlrelay server and auths with
        user and password.  Failed connections are retried for tries times,
        waiting retrytime seconds between each try.  If tries is 0 then retries
        will continue forever.  If retrytime is 0 then retries will be attempted
        on a default interval.
        """
        self.connection = CSQLRelay.sqlrcon_alloc(host, port, socket, user, password, retrytime, tries)


    def __del__(self):
        CSQLRelay.sqlrcon_free(self.connection)

    def setConnectTimeout(self, timeoutsec, timeoutusec):
        """
        Sets the server connect timeout in seconds and
        milliseconds.  Setting either parameter to -1 disables the
        timeout.  You can also set this timeout using the
        SQLR_CLIENT_CONNECT_TIMEOUT environment variable.
        """
        return CSQLRelay.setConnectTimeout(self.connection, timeoutsec, timeoutusec)

    def setAuthenticationTimeout(self, timeoutsec, timeoutusec):
        """
        Sets the authentication timeout in seconds and
        milliseconds.  Setting either parameter to -1 disables the
        timeout.   You can also set this timeout using the
        SQLR_CLIENT_AUTHENTICATION_TIMEOUT environment variable.
        """
        return CSQLRelay.setAuthenticationTimeout(self.connection, timeoutsec, timeoutusec)

    def setResponseTimeout(self, timeoutsec, timeoutusec):
        """
        Sets the response timeout (for queries, commits, rollbacks,
        pings, etc.) in seconds and milliseconds.  Setting either
        parameter to -1 disables the timeout.  You can also set
        this timeout using the SQLR_CLIENT_RESPONSE_TIMEOUT
        environment variable.
        """
        return CSQLRelay.setResponseTimeout(self.connection, timeoutsec, timeoutusec)

    def setBindVariableDelimiters(self, delimiters):
        """
        Sets which delimiters are used to identify bind variables
        in countBindVariables() and validateBinds().  Valid
        delimiters include ?,:,@, and $.  Defaults to "?:@$"
        """
        return CSQLRelay.setBindVariableDelimiters(self.connection, delimiters)

    def getBindVariableDelimiterQuestionMarkSupported(self):
        """
        Returns true if question marks (?) are considered to be
        valid bind variable delimiters.
        """
        return CSQLRelay.getBindVariableDelimiterQuestionMarkSupported(self.connection)

    def getBindVariableDelimiterColonSupported(self):
        """
        Returns true if colons (:) are considered to be
        valid bind variable delimiters.
        """
        return CSQLRelay.getBindVariableDelimiterColonSupported(self.connection)

    def getBindVariableDelimiterAtSignSupported(self):
        """
        Returns true if at-signs (@) are considered to be
        valid bind variable delimiters.
        """
        return CSQLRelay.getBindVariableDelimiterAtSignSupported(self.connection)

    def getBindVariableDelimiterDollarSignSupported(self):
        """
        Returns true if dollar signs ($) are considered to be
        valid bind variable delimiters.
        """
        return CSQLRelay.getBindVariableDelimiterDollarSignSupported(self.connection)

    def enableKerberos(self, service, mech, flags):
        """
        Enables Kerberos authentication and encryption.

        "service" indicates the Kerberos service name of the
        SQL Relay server.  If left empty or NULL then the service
        name "sqlrelay" will be used. "sqlrelay" is the default
        service name of the SQL Relay server.  Note that on Windows
        platforms the service name must be fully qualified,
        including the host and realm name.  For example:
        "sqlrelay/sqlrserver.firstworks.com@AD.FIRSTWORKS.COM".
      
        "mech" indicates the specific Kerberos mechanism to use.
        On Linux/Unix platforms, this should be a string
        representation of the mechnaism's OID, such as:
            { 1 2 840 113554 1 2 2 }
        On Windows platforms, this should be a string like:
            Kerberos
        If left empty or NULL then the default mechanism will be
        used.  Only set this if you know that you have a good
        reason to.
      
        "flags" indicates what Kerberos flags to use.  Multiple
        flags may be specified, separated by commas.  If left
        empty or NULL then a defalt set of flags will be used.
        Only set this if you know that you have a good reason to.
      
        Valid flags include:
         * GSS_C_MUTUAL_FLAG
         * GSS_C_REPLAY_FLAG
         * GSS_C_SEQUENCE_FLAG
         * GSS_C_CONF_FLAG
         * GSS_C_INTEG_FLAG
      
        For a full list of flags, consult the GSSAPI documentation,
        though note that only the flags listed above are supported
        on Windows.
        """
        return CSQLRelay.enableKerberos(self.connection, service, mech, flags)

    def enableTls(self, version, cert, password, ciphers, validate, ca, depth):
        """
        Enables TLS/SSL encryption, and optionally authentication.

        "version" specifies the TLS/SSL protocol version that the
        client will attempt to use.  Valid values include SSL2,
        SSL3, TLS1, TLS1.1, TLS1.2 or any more recent version of
        TLS, as supported by and enabled in the underlying TLS/SSL
        library.  If left blank or empty then the highest supported
        version will be negotiated.
        
        "cert" is the file name of the certificate chain file to
        send to the SQL Relay server.  This is only necessary if
        the SQL Relay server is configured to authenticate and
        authorize clients by certificate.
        
        If "cert" contains a password-protected private key, then
        "password" may be supplied to access it.  If the private
        key is not password-protected, then this argument is
        ignored, and may be left empty or NULL.
        
        "ciphers" is a list of ciphers to allow.  Ciphers may be
        separated by spaces, commas, or colons.  If "ciphers" is
        empty or NULL then a default set is used.  Only set this if
        you know that you have a good reason to.
        
        For a list of valid ciphers on Linux/Unix platforms, see:
            man ciphers
        
        For a list of valid ciphers on Windows platforms, see:
            https://msdn.microsoft.com/en-us/library/windows/desktop/aa375549%28v=vs.85%29.aspx
        On Windows platforms, the ciphers (alg_id's) should omit
        CALG_ and may be given with underscores or dashes.
        For example: 3DES_112
        
        "validate" indicates whether to validate the SQL Relay's
        server certificate, and may be set to one of the following:
            "no" - Don't validate the server's certificate.
            "ca" - Validate that the server's certificate was
                   signed by a trusted certificate authority.
            "ca+host" - Perform "ca" validation and also validate
                   that one of the subject altenate names (or the
                   common name if no SANs are present) in the
                   certificate matches the host parameter.
                   (Falls back to "ca" validation when a unix
                   socket is used.)
            "ca+domain" - Perform "ca" validation and also validate
                   that the domain name of one of the subject
                   alternate names (or the common name if no SANs
                   are present) in the certificate matches the
                   domain name of the host parameter.  (Falls back
                   to "ca" validation when a unix socket is used.)
        
        "ca" is the location of a certificate authority file to
        use, in addition to the system's root certificates, when
        validating the SQL Relay server's certificate.  This is
        useful if the SQL Relay server's certificate is self-signed.
        
        On Windows, "ca" must be a file name.
        
        On non-Windows systems, "ca" can be either a file or
        directory name.  If it is a directory name, then all
        certificate authority files found in that directory will be
        used.  If it a file name, then only that file will be used.
        
        
        Note that the supported "cert" and "ca" file formats may
        vary between platforms.  A variety of file formats are
        generally supported on Linux/Unix platfoms (.pem, .pfx,
        etc.) but only the .pfx format is currently supported on
        Windows. */
        """
        return CSQLRelay.enableTls(self.connection, version, cert, password, ciphers, validate, ca, depth)

    def disableEncryption(self):
        """
        Disables encryption.
        """
        return CSQLRelay.disableEncryption(self.connection)

    def endSession(self):
        """
        Ends the current session.
        """
        return CSQLRelay.endSession(self.connection)

    def suspendSession(self):
        """
        Disconnects this connection from the current
        session but leaves the session open so 
        that another connection can connect to it 
        using resumeSession().
        """
        return CSQLRelay.suspendSession(self.connection)

    def getConnectionPort(self):
        """
        Returns the inet port that the connection is 
        communicating over. This parameter may be 
        passed to another connection for use in
        the resumeSession() method.
        Note: the value returned by this method is
        only valid after a call to suspendSession().
        """
        return CSQLRelay.getConnectionPort(self.connection)

    def getConnectionSocket(self):
        """
        Returns the unix socket that the connection is 
        communicating over. This parameter may be 
        passed to another connection for use in
        the resumeSession() method.
        Note: the value returned by this method is
        only valid after a call to suspendSession().
        """
        return CSQLRelay.getConnectionSocket(self.connection)

    def resumeSession(self, port, socket):
        """
        Resumes a session previously left open 
        using suspendSession().
        Returns 1 on success and 0 on failure.
        """
        return CSQLRelay.resumeSession(self.connection, port, socket)

    def ping(self):
        """
        Returns 1 if the database is up and 0
        if it's down.
        """
        return CSQLRelay.ping(self.connection)

    def identify(self):
        """
        Returns the type of database: 
          oracle, postgresql, mysql, etc.
        """
        return CSQLRelay.identify(self.connection)

    def dbVersion(self):
        """
        Returns the version of the database
        """
        return CSQLRelay.dbVersion(self.connection)

    def dbHostName(self):
        """
        Returns the host name of the database
        """
        return CSQLRelay.dbHostName(self.connection)

    def dbIpAddress(self):
        """
        Returns the ip address of the database
        """
        return CSQLRelay.dbIpAddress(self.connection)


    def serverVersion(self):
        """
        Returns the version of the SQL Relay server version
        """
        return CSQLRelay.serverVersion(self.connection)


    def clientVersion(self):
        """
        Returns the version of the SQL Relay client version
        """
        return CSQLRelay.clientVersion(self.connection)

    def bindFormat(self):
        """
        Returns a string representing the format
        of the bind variables used in the db.
        """
        return CSQLRelay.bindFormat(self.connection)

    def selectDatabase(self,database):
        """
        Sets the current database/schema to "database"
        """
        return CSQLRelay.selectDatabase(self.connection,database)

    def getCurrentDatabase(self):
        """
        Returns the database/schema that is currently in use.
        """
        return CSQLRelay.getCurrentDatabase(self.connection)

    def getLastInsertId(self):
        """
        Returns the value of the autoincrement column for the last insert
        """
        return CSQLRelay.getLastInsertId(self.connection)

    def autoCommitOn(self):
        """
        Instructs the database to perform a commit
        after every successful query.
        """
        return CSQLRelay.autoCommitOn(self.connection)

    def autoCommitOff(self):
        """
        Instructs the database to wait for the 
        client to tell it when to commit.
        """
        return CSQLRelay.autoCommitOff(self.connection)

    def begin(self):
        """
        Begins a transaction.  Returns true if the begin
        succeeded, false if it failed.  If the database
        automatically begins a new transaction when a
        commit or rollback is issued then this doesn't
        do anything unless SQL Relay is faking transaction
        blocks.
        """
        return CSQLRelay.begin(self.connection)

    def commit(self):
        """
        Issues a commit, returns true if the commit
        succeeded, false if it failed.
        """
        return CSQLRelay.commit(self.connection)

    def rollback(self):
        """
        Issues a rollback, returns true if the rollback
        succeeded, false if it failed.
        """
        return CSQLRelay.rollback(self.connection)

    def errorMessage(self):
        """
        If the operation failed, the error message will be returned
        from this method.  Otherwise, it returns None.
        """
        return CSQLRelay.connectionErrorMessage(self.connection)

    def errorNumber(self):
        """
        If an operation failed and generated an
        error, the error number is available here.
        If there is no error then this method 
        returns 0.
        """
        return CSQLRelay.connectionErrorNumber(self.connection)

    def debugOn(self):
        """
        Turn verbose debugging on.
        Another way to do this is to start a query with "-- debug\n".
        Yet another way is to set the environment variable SQLR_CLIENT_DEBUG
        to "ON"
        """
        return CSQLRelay.debugOn(self.connection)

    def debugOff(self):
        """
        Turn verbose debugging off.
        """
        return CSQLRelay.debugOff(self.connection)

    def getDebug(self):
        """
        Returns 1 if debugging is turned on and 0 if debugging is turned off.
        """
        return CSQLRelay.getDebug(self.connection)

    def setDebugFile(self,filename):
        """
        Allows you to specify a file to write debug to.
        Setting "filename" to NULL or an empty string causes debug
        to be written to standard output (the default).
        """
        return CSQLRelay.setDebugFile(self.connection,filename)

    def setClientInfo(self,clientinfo):
        """
        Allows you to set a string that will be passed to the
        server and ultimately included in server-side logging
        along with queries that were run by this instance of
        the client.
        """
        return CSQLRelay.setClientInfo(self.connection,clientinfo)

    def getClientInfo(self):
        """
        Returns the string that was set by setClientInfo().
        """
        return CSQLRelay.getClientInfo(self.connection)




class sqlrcursor:


    """
    A wrapper for the sqlrelay cursor API.  Closely follows the C++ API.
    """

    def __init__(self, sqlrcon):
        self.connection = sqlrcon
        self.cursor = CSQLRelay.sqlrcur_alloc(sqlrcon.connection)

    def __del__(self):
        CSQLRelay.sqlrcur_free(self.cursor)

    def setResultSetBufferSize(self, rows):
        """
        Sets the number of rows of the result set
        to buffer at a time.  0 (the default)
        means buffer the entire result set.
        """
        return CSQLRelay.setResultSetBufferSize(self.cursor, rows)

    def getResultSetBufferSize(self):
        """
        Returns the number of result set rows that 
        will be buffered at a time or 0 for the
        entire result set.
        """
        return CSQLRelay.getResultSetBufferSize(self.cursor)

    def dontGetColumnInfo(self):
        """
        Tells the server not to send any column
        info (names, types, sizes).  If you don't
        need that info, you should call this
        method to improve performance.
        """
        return CSQLRelay.dontGetColumnInfo(self.cursor)

    def mixedCaseColumnNames(self):
        """
        Columns names are returned in the same
        case as they are defined in the database.
        This is the default.
        """
        return CSQLRelay.mixedCaseColumnNames(self.cursor)

    def upperCaseColumnNames(self):
        """
        Columns names are converted to upper case.
        """
        return CSQLRelay.upperCaseColumnNames(self.cursor)

    def lowerCaseColumnNames(self):
        """
        Columns names are converted to lower case.
        """
        return CSQLRelay.lowerCaseColumnNames(self.cursor)

    def getColumnInfo(self):
        """
        Tells the server to send column info.
        """
        return CSQLRelay.getColumnInfo(self.cursor)

    def cacheToFile(self, filename):
        """
        Sets query caching on.  Future queries
        will be cached to the file "filename".
        The full pathname of the file can be
        retrieved using getCacheFileName().
        
        A default time-to-live of 10 minutes is
        also set.
        
        Note that once cacheToFile() is called,
        the result sets of all future queries will
        be cached to that file until another call 
        to cacheToFile() changes which file to
        cache to or a call to cacheOff() turns off
        caching.
        """
        return CSQLRelay.cacheToFile(self.cursor,filename)

    def setCacheTtl(self, ttl):
        """
        Sets the time-to-live for cached result
        sets. The sqlr-cachemanger will remove each 
        cached result set "ttl" seconds after it's 
        created.
        """
        return CSQLRelay.setCacheTtl(self.cursor,ttl)

    def getCacheFileName(self):
        """
        Returns the name of the file containing the most
        recently cached result set.
        """
        return CSQLRelay.getCacheFileName(self.cursor)

    def cacheOff(self):
        """
        Sets query caching off.
        """
        return CSQLRelay.cacheOff(self.cursor)

    def getDatabaseList(self,wild):
        """
        Sends a query that returns a list of
        databases/schemas matching "wild".  If wild is empty
        or NULL then a list of all databases/schemas will be
        returned.
        """
        return CSQLRelay.setDatabaseList(self.cursor,wild)

    def getTableList(self,wild):
        """
        Sends a query that returns a list of tables
        matching "wild".  If wild is empty or NULL then
        a list of all tables will be returned.
        """
        return CSQLRelay.setTableList(self.cursor,wild)

    def getColumnList(sefl,table,wild):
        """
        Sends a query that returns a list of columns
        in the table specified by the "table" parameter
        matching "wild".  If wild is empty or NULL then
        a list of all columns will be returned.
        """
        return CSQLRelay.setColumnList(self.cursor,table,wild)

    """
    If you don't need to use substitution or bind variables
    in your queries, use these two methods.
    """
    def sendQuery(self, query):
        """
        Send a SQL query to the server and
        gets a result set.
        """
        return CSQLRelay.sendQuery(self.cursor, query)

    def sendQueryWithLength(self, query, length):
        """
        Sends "query" with length "length" and gets
        a result set. This method must be used if
        the query contains binary data.
        """
        return CSQLRelay.sendQueryWithLength(self.cursor, query, length)

    def sendFileQuery(self, path, file):
        """
        Send the SQL query in path/file to the server and
        gets a result set.
        """
        return CSQLRelay.sendFileQuery(self.cursor, path, file)

    """
    If you need to use substitution or bind variables, in your
    queries use the following methods.  See the API documentation
    for more information about substitution and bind variables.
    """
    def prepareQuery(self, query):
        """
        Prepare to execute query.
        """
        return CSQLRelay.prepareQuery(self.cursor, query)

    def prepareQueryWithLength(self, query, length):
        """
        Prepare to execute "query" with length 
        "length".  This method must be used if the
        query contains binary data.
        """
        return CSQLRelay.prepareQueryWithLength(self.cursor, query, length)

    def prepareFileQuery(self, path, file):
        """
        Prepare to execute the contents of path/filename.
        """
        return CSQLRelay.prepareFileQuery(self.cursor, path, file)

    def substitution(self, variable, value, precision=0, scale=0):
        """
        Define a substitution variable.
        Returns true if the variable was successfully substituted or false if
        the variable isn't a string, integer or floating point number, or if
        precision and scale aren't provided for a floating point number.
        """
        return CSQLRelay.substitution(self.cursor,variable,value,precision,scale)

    def clearBinds(self):
        """
        Clear all binds variables.
        """
        return CSQLRelay.clearBinds(self.cursor)

    def countBindVariables(self):
        """
        Parses the previously prepared query,
        counts the number of bind variables defined
        in it and returns that number.
        """
        return CSQLRelay.countBindVariables(self.cursor)

    def inputBind(self, variable, value, precision=0, scale=0):
        """
        Define an input bind varaible.
        Returns true if the variable was successfully bound or false if the
        variable isn't a string, integer or decimal.  If the value is a decimal
        then precision and scale may also be specified.  If you don't have the
        precision and scale then set them both to 0.  However in that case you
        may get unexpected rounding behavior if the server is faking binds.
        """
        return CSQLRelay.inputBind(self.cursor, variable, value, precision, scale)

    def inputBindBlob(self, variable, value, length):
        """
        Define an input bind varaible.
        """
        return CSQLRelay.inputBindBlob(self.cursor, variable, value, length)

    def inputBindClob(self, variable, value, length):
        """
        Define an input bind varaible.
        """
        return CSQLRelay.inputBindClob(self.cursor, variable, value, length)

    def defineOutputBindString(self, variable, length):
        """
        Define a string output bind varaible.
        """
        return CSQLRelay.defineOutputBindString(self.cursor, variable, length)

    def defineOutputBindInteger(self, variable):
        """
        Define an integer output bind varaible.
        """
        return CSQLRelay.defineOutputBindInteger(self.cursor, variable)

    def defineOutputBindDouble(self, variable):
        """
        Define a double precision floating point output bind varaible.
        """
        return CSQLRelay.defineOutputBindDouble(self.cursor, variable)

    def defineOutputBindBlob(self, variable):
        """
        Define an output bind varaible.
        """
        return CSQLRelay.defineOutputBindBlob(self.cursor, variable)

    def defineOutputBindClob(self, variable):
        """
        Define an output bind varaible.
        """
        return CSQLRelay.defineOutputBindClob(self.cursor, variable)

    def defineOutputBindCursor(self, variable):
        """
        Define an output bind varaible.
        """
        return CSQLRelay.defineOutputBindCursor(self.cursor, variable)

    def substitutions(self, variables, values, precisions=None, scales=None):
        """
        Define substitution variables.
        Returns true if the variables were successfully substituted or false if
        one of the variables wasn't a string, integer or floating point number,
        or if precision and scale weren't provided for a floating point number.
        """
        return CSQLRelay.substitutions(self.cursor,variables,values,precisions,scales)

    def inputBinds(self, variables, values, precisions=None, scales=None):
        """
        Define input bind variables.
        Returns true if the variables were successfully bound or false if one
        of the variables wasn't a string, integer or floating point number,
        or if precision and scale weren't provided for a floating point number.
        """
        return CSQLRelay.inputBinds(self.cursor,variables,values,precisions,scales)
        

    def validateBinds(self):
        """
        If you are binding to any variables that 
        might not actually be in your query, call 
        this to ensure that the database won't try 
        to bind them unless they really are in the 
        query.
        """
        return CSQLRelay.validateBinds(self.cursor)
        

    def validBind(self,variable):
        """
        Returns true if "variable" was a valid
        input bind variable of the query.
        """
        return CSQLRelay.validBind(self.cursor,variable)
        

    def executeQuery(self):
        """
        Execute the query that was previously
        prepared and bound.
        """
        return CSQLRelay.executeQuery(self.cursor)

    def fetchFromBindCursor(self):
        """
        Fetch from a cursor that was returned as
        an output bind variable.
        """
        return CSQLRelay.fetchFromBindCursor(self.cursor)

    def getOutputBindString(self, variable):
        """
        Get the value stored in a previously
        defined output bind variable.
        """
        return CSQLRelay.getOutputBindString(self.cursor, variable)

    def getOutputBindBlob(self, variable):
        """
        Get the value stored in a previously
        defined output bind variable.
        """
        return CSQLRelay.getOutputBindBlob(self.cursor, variable)

    def getOutputBindClob(self, variable):
        """
        Get the value stored in a previously
        defined output bind variable.
        """
        return CSQLRelay.getOutputBindClob(self.cursor, variable)

    def getOutputBindInteger(self, variable):
        """
        Get the value stored in a previously
        defined output bind variable as a long
        integer.
        """
        return CSQLRelay.getOutputBindInteger(self.cursor, variable)

    def getOutputBindDouble(self, variable):
        """
        Get the value stored in a previously
        defined output bind variable as a double
        precision floating point number.
        """
        return CSQLRelay.getOutputBindDouble(self.cursor, variable)

    def getOutputBindLength(self, variable):
        """
        Retrieve the length of an output bind variable.
        """
        return CSQLRelay.getOutputBindLength(self.cursor, variable)

    def getOutputBindCursor(self, variable):
        """
        Get the cursor associated with a previously
        defined output bind variable.
        """
        bindcursorid=CSQLRelay.getOutputBindCursorId(self.cursor, variable)
        if bindcursorid==-1:
                return None
        bindcursor=sqlrcursor(self.connection)
        CSQLRelay.attachToBindCursor(bindcursor.cursor, bindcursorid)
        return bindcursor

    def openCachedResultSet(self, filename):
        """
        Open a result set after a sendCachedQeury
        """
        return CSQLRelay.openCachedResultSet(self.cursor, filename)

    def colCount(self):
        """
        Returns the number of columns in the current result set.
        """
        return CSQLRelay.colCount(self.cursor)

    def rowCount(self):
        """
        Returns the number of rows in the current result set.
        """
        return CSQLRelay.rowCount(self.cursor)

    def totalRows(self):
        """
        Returns the total number of rows that will 
        be returned in the result set.  Not all 
        databases support this call.  Don't use it 
        for applications which are designed to be 
        portable across databases.  -1 is returned
        by databases which don't support this option.
        """
        return CSQLRelay.totalRows(self.cursor)

    def affectedRows(self):
        """
        Returns the number of rows that were 
        updated, inserted or deleted by the query.
        Not all databases support this call.  Don't 
        use it for applications which are designed 
        to be portable across databases.  -1 is 
        returned by databases which don't support 
        this option.
        """
        return CSQLRelay.affectedRows(self.cursor)

    def firstRowIndex(self):
        """
        Returns the index of the first buffered row.
        This is useful when buffering only part of
        the result set at a time.
        """
        return CSQLRelay.firstRowIndex(self.cursor)

    def endOfResultSet(self):
        """
        Returns 0 if part of the result set is still
        pending on the server and 1 if not.  This
        method can only return 0 if 
        setResultSetBufferSize() has been called
        with a parameter other than 0.
        """
        return CSQLRelay.endOfResultSet(self.cursor)

    def errorMessage(self):
        """
        If the query failed, the error message will be returned
        from this method.  Otherwise, it returns None.
        """
        return CSQLRelay.cursorErrorMessage(self.cursor)

    def errorNumber(self):
        """
        If the query failed and generated an
        error, the error number is available here.
        If there is no error then this method 
        returns 0.
        """
        return CSQLRelay.cursorErrorNumber(self.cursor)

    def getNullsAsEmptyStrings(self):
        """
        Tells the cursor to return NULL fields and output
        bind variables as empty strings.
        This is the default.
        """
        return CSQLRelay.getNullsAsEmptyStrings(self.cursor)

    def getNullsAsNone(self):
        """
        Tells the cursor to return NULL fields and output
        bind variables as NULL's.
        """
        return CSQLRelay.getNullsAsNone(self.cursor)

    def getField(self, row, col):
        """
        Returns the value of the specified row and
        column.  col may be a column name or number.
        """
        return CSQLRelay.getField(self.cursor, row, col)

    def getFieldAsInteger(self, row, col):
        """
        Returns the specified field as a long integer.
        """
        return CSQLRelay.getFieldAsInteger(self.cursor, row, col)

    def getFieldAsDouble(self, row, col):
        """
        Returns the specified field as a double precision
        floating point number.
        """
        return CSQLRelay.getFieldAsDouble(self.cursor, row, col)

    def getFieldLength(self, row, col):
        """
        Returns the length of the specified row and
        column.  col may be a column name or number.
        """
        return CSQLRelay.getFieldLength(self.cursor, row, col)

    def getRow(self, row):
        """
        Returns a list of values in the given row.
        """
        return CSQLRelay.getRow(self.cursor, row)

    def getRowDictionary(self, row):
        """
        Returns the requested row as values in a dictionary
        with column names for keys.
        """
        return CSQLRelay.getRowDictionary(self.cursor, row)

    def getRowRange(self, beg, end):
        """
        Returns a list of lists of the rows between beg and end.
        Note: this function has no equivalent in other SQL Relay API's.
        """
        return CSQLRelay.getRowRange(self.cursor, beg, end)

    def getRowLengths(self, row):
        """
        Returns a list of lengths in the given row.
        """
        return CSQLRelay.getRowLengths(self.cursor, row)

    def getRowLengthsDictionary(self, row):
        """
        Returns the requested row lengths as values in a dictionary
        with column names for keys.
        """
        return CSQLRelay.getRowLengthsDictionary(self.cursor, row)

    def getRowLengthsRange(self, beg, end):
        """
        Returns a list of lists of the lengths of rows between beg and end.
        Note: this function has no equivalent in other SQL Relay API's.
        """
        return CSQLRelay.getRowLengthsRange(self.cursor, beg, end)

    def getColumnName(self, col):
        """
        Returns the name of column number col.
        """
        return CSQLRelay.getColumnName(self.cursor, col)

    def getColumnNames(self):
        """
        Returns a list of column names in the current result set.
        """
        return CSQLRelay.getColumnNames(self.cursor)

    def getColumnType(self, col):
        """
        Returns the type of the specified column.  col may
        be a name or number.
        """
        return CSQLRelay.getColumnType(self.cursor, col)

    def getColumnLength(self, col):
        """
        Returns the length of the specified column.  col may
        be a name or number.
        """
        return CSQLRelay.getColumnLength(self.cursor, col)

    def getColumnPrecision(self, col):
        """
        Returns the precision of the specified column.
        Precision is the total number of digits in a number.
        eg: 123.45 has a precision of 5.  For non-numeric
        types, it's the number of characters in the string.
        """
        return CSQLRelay.getColumnPrecision(self.cursor, col)

    def getColumnScale(self, col):
        """
        Returns the scale of the specified column.  Scale is
        the total number of digits to the right of the decimal
        point in a number.  eg: 123.45 has a scale of 2.
        """
        return CSQLRelay.getColumnScale(self.cursor, col)

    def getColumnIsNullable(self, col):
        """
        Returns 1 if the specified column can contain nulls and
        0 otherwise.
        """
        return CSQLRelay.getColumnIsNullable(self.cursor, col)

    def getColumnIsPrimaryKey(self, col):
        """
        Returns 1 if the specified column is a primary key and
        0 otherwise.
        """
        return CSQLRelay.getColumnIsPrimaryKey(self.cursor, col)

    def getColumnIsUnique(self, col):
        """
        Returns 1 if the specified column is unique and
        0 otherwise.
        """
        return CSQLRelay.getColumnIsUnique(self.cursor, col)

    def getColumnIsPartOfKey(self, col):
        """
        Returns 1 if the specified column is part of a composite
        key and 0 otherwise.
        """
        return CSQLRelay.getColumnIsPartOfKey(self.cursor, col)

    def getColumnIsUnsigned(self, col):
        """
        Returns 1 if the specified column is an unsigned number
        and 0 otherwise.
        """
        return CSQLRelay.getColumnIsUnsigned(self.cursor, col)

    def getColumnIsZeroFilled(self, col):
        """
        Returns 1 if the specified column was created with the
        zero-fill flag and 0 otherwise.
        """
        return CSQLRelay.getColumnIsZeroFilled(self.cursor, col)

    def getColumnIsBinary(self, col):
        """
        Returns 1 if the specified column contains binary data
        and 0 otherwise.
        """
        return CSQLRelay.getColumnIsBinary(self.cursor, col)

    def getColumnIsAutoIncrement(self, col):
        """
        Returns 1 if the specified column auto-increments and
        0 otherwise.
        """
        return CSQLRelay.getColumnIsAutoIncrement(self.cursor, col)

    def getLongest(self, col):
        """
        Returns the length of the specified column.  col may
        be a name or number.
        """
        return CSQLRelay.getLongest(self.cursor, col)

    def suspendResultSet(self):
        """
        Tells the server to leave this result
        set open when the cursor calls 
        suspendSession() so that another cursor can 
        connect to it using resumeResultSet() after 
        it calls resumeSession().
        """
        return CSQLRelay.suspendResultSet(self.cursor)

    def getResultSetId(self):
        """
        Returns the internal ID of this result set.
        This parameter may be passed to another 
        cursor for use in the resumeResultSet() 
        method.
        Note: the value returned by this method is
        only valid after a call to suspendResultSet().
        """
        return CSQLRelay.getResultSetId(self.cursor)

    def resumeResultSet(self, id):
        """
        Resumes a result set previously left open 
        using suspendSession().
        Returns 1 on success and 0 on failure.
        """
        return CSQLRelay.resumeResultSet(self.cursor, id)

    def resumeCachedResultSet(self, id, filename):
        """
        Resumes a result set previously left open
        using suspendSession() and continues caching
        the result set to "filename".
        Returns 1 on success and 0 on failure.
        """
        return CSQLRelay.resumeCachedResultSet(self.cursor, id, filename)

    def closeResultSet():
        """
        Closes the current result set, if one is open.  Data
        that has been fetched already is still available but
        no more data may be fetched.  Server side resources
        for the result set are freed as well.
        """
        return CSQLRelay.closeResultSet(self.cursor)
