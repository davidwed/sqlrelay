# Copyright (c) 2000 Roman Milner
# See the file COPYING for more information

from SQLRelay import CSQLRelay

class sqlrconnection:

    """
    A wrapper for the sqlrelay connection API.  Closely follows the C++ API.
    """

    def __init__(self, host, port, socket, user, password, retrytime=0, tries=1):
        """ 
        Opens a connection to the sqlrelay server and authenticates with
        user and password.  Failed connections are retried for tries times
        at retrytime interval.  If tries is 0 then retries will continue
        forever.  If retrytime is 0 then retries will be attempted on a
        default interval.
        """
        self.connection = CSQLRelay.sqlrcon_alloc(host, port, socket, user, password, retrytime, tries)


    def __del__(self):
        CSQLRelay.sqlrcon_free(self.connection)

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
        """
        return CSQLRelay.getConnectionPort(self.connection)

    def getConnectionSocket(self):
        """
        Returns the unix socket that the connection is 
        communicating over. This parameter may be 
        passed to another connection for use in
        the resumeSession() method.
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
          oracle7, oracle8, postgresql, mysql, etc.
        """
        return CSQLRelay.identify(self.connection)

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

    def commit(self):
        """
        Issues a commit, returns 1 if the commit
        succeeded, 0 if it failed and -1 if an
        error occurred.
        """
        return CSQLRelay.commit(self.connection)

    def rollback(self):
        """
        Issues a rollback, returns 1 if the rollback
        succeeded, 0 if it failed and -1 if an
        error occurred.
        """
        return CSQLRelay.rollback(self.connection)

    def debugOn(self):
        """
        Turn verbose debugging on.
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
        """
        return CSQLRelay.substitution(self.cursor,variable,value,precision,scale)

    def clearBinds(self):
        """
        Clear all binds variables.
        """
        return CSQLRelay.clearBinds(self.cursor)

    def inputBind(self, variable, value, precision=0, scale=0):
        """
        Define an input bind varaible.
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

    def defineOutputBind(self, variable, length):
        """
        Define an output bind varaible.
        """
        return CSQLRelay.defineOutputBind(self.cursor, variable, length)

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
        """
        return CSQLRelay.substitutions(self.cursor,variables,values,precisions,scales)

    def inputBinds(self, variables, values, precisions=None, scales=None):
        """
        Define input bind variables.
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

    def getOutputBind(self, variable):
        """
        Get the value stored in a previously
        defined output bind variable.
        """
        return CSQLRelay.getOutputBind(self.cursor, variable)

    def getOutputBindAsLong(self, variable):
        """
        Get the value stored in a previously
        defined output bind variable as a long
        integer.
        """
        return CSQLRelay.getOutputBindAsLong(self.cursor, variable)

    def getOutputBindAsDouble(self, variable):
        """
        Get the value stored in a previously
        defined output bind variable as a double
        precision floating point number.
        """
        return CSQLRelay.getOutputBindAsDouble(self.cursor, variable)

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
        return CSQLRelay.errorMessage(self.cursor)

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

    def getFieldAsLong(self, row, col):
        """
        Returns the specified field as a long integer.
        """
        return CSQLRelay.getFieldAsLong(self.cursor, row, col)

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

    def getResultSetId(self):
        """
        Returns the internal ID of this result set.
        This parameter may be passed to another 
        cursor for use in the resumeResultSet() 
        method.
        """
        return CSQLRelay.getResultSetId(self.cursor)

    def suspendResultSet(self):
        """
        Tells the server to leave this result
        set open when the cursor calls 
        suspendSession() so that another cursor can 
        connect to it using resumeResultSet() after 
        it calls resumeSession().
        """
        return CSQLRelay.suspendResultSet(self.cursor)

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
