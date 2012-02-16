using System;
using System.Runtime.InteropServices;

public class SQLRCursor
{

    /** Creates a cursor to run queries and fetch
     *  result sets using connection "sqlrconref" */
    public SQLRCursor(IntPtr sqlrconref)
    {
        sqlrcurref = sqlrcur_alloc_copyrefs(sqlrconref, 1);
    }

    /** Destroys the cursor and cleans up all associated result set data. */
    /* public void sqlrcur_free(IntPtr sqlrcurref); */



    /** Sets the number of rows of the result set to buffer at a time.
     *  0 (the default) means buffer the entire result set. */
    public void setResultSetBufferSize(ulong rows)
    {
        sqlrcur_setResultSetBufferSize(sqlrcurref, rows);
    }

    /** Returns the number of result set rows that will be buffered at a time or
     *  0 for the entire result set. */
    public ulong getResultSetBufferSize()
    {
        return sqlrcur_getResultSetBufferSize(sqlrcurref);
    }



    /** Tells the server not to send any column info (names, types, sizes).  If
     *  you don't need that info, you should call this function to improve
     *  performance. */
    public void dontGetColumnInfo()
    {
        sqlrcur_dontGetColumnInfo(sqlrcurref);
    }

    /** Tells the server to send column info. */
    public void getColumnInfo()
    {
        sqlrcur_getColumnInfo(sqlrcurref);
    }



    /** Columns names are returned in the same case as they are defined in the
     *  database.  This is the default. */
    public void mixedCaseColumnNames()
    {
        sqlrcur_mixedCaseColumnNames(sqlrcurref);
    }

    /** Columns names are converted to upper case. */
    public void upperCaseColumnNames()
    {
        sqlrcur_upperCaseColumnNames(sqlrcurref);
    }

    /** Columns names are converted to lower case. */
    public void lowerCaseColumnNames()
    {
        sqlrcur_lowerCaseColumnNames(sqlrcurref);
    }




    /** Sets query caching on.  Future queries will be cached to the
     *  file "filename".
     * 
     *  A default time-to-live of 10 minutes is also set.
     * 
     *  Note that once sqlrcur_cacheToFile() is called, the result sets of all
     *  future queries will be cached to that file until another call to
     *  sqlrcur_cacheToFile() changes which file to cache to or a call to
     *  sqlrcur_cacheOff() turns off caching. */
    public void cacheToFile(string filename)
    {
        sqlrcur_cacheToFile(sqlrcurref, filename);
    }

    /** Sets the time-to-live for cached result sets. The sqlr-cachemanger will
     *  remove each cached result set "ttl" seconds after it's created, provided
     *  it's scanning the directory containing the cache files. */
    public void setCacheTtl(uint ttl)
    {
        sqlrcur_setCacheTtl(sqlrcurref, ttl);
    }

    /** Returns the name of the file containing
     *  the most recently cached result set. */
    public string getCacheFileName()
    {
        return sqlrcur_getCacheFileName(sqlrcurref);
    }

    /** Sets query caching off. */
    public void cacheOff()
    {
        sqlrcur_cacheOff(sqlrcurref);
    }



    /** Sends a query that returns a list of databases/schemas matching "wild".
     *  If wild is empty or NULL then a list of all databases/schemas will be
     *  returned. */
    public int getDatabaseList(string wild)
    {
        return sqlrcur_getDatabaseList(sqlrcurref, wild);
    }

    /** Sends a query that returns a list of tables matching "wild".  If wild is
     *  empty or NULL then a list of all tables will be returned. */
    public int getTableList(string wild)
    {
        return sqlrcur_getTableList(sqlrcurref, wild);
    }

    /** Sends a query that returns a list of columns in the table specified by
     *  the "table" parameter matching "wild".  If wild is empty or NULL then a
     *  list of all columns will be returned. */
    public int getColumnList(string table, string wild)
    {
        return sqlrcur_getColumnList(sqlrcurref, table, wild);
    }



    /** Sends "query" directly and gets a result set. */
    public int sendQuery(string query)
    {
        return sqlrcur_sendQuery(sqlrcurref, query);
    }

    /** Sends "query" with length "length" directly and gets a result set. This
     *  function must be used if the query contains binary data. */
    public int sendQueryWithLength(string query, uint length)
    {
        return sqlrcur_sendQueryWithLength(sqlrcurref, query, length);
    }

    /** Sends the query in file "path"/"filename" and gets a result set. */
    public int sendFileQuery(IntPtr sqlrcurref, string path, string filename)
    {
        return sqlrcur_sendFileQuery(sqlrcurref, path, filename);
    }



    /** Prepare to execute "query". */
    public void prepareQuery(string query)
    {
        sqlrcur_prepareQuery(sqlrcurref, query);
    }

    /** Prepare to execute "query" with length "length".  This function must be
     *  used if the query contains binary data. */
    public void prepareQueryWithLength(string query, uint length)
    {
        sqlrcur_prepareQueryWithLength(sqlrcurref, query, length);
    }

    /** Prepare to execute the contents of "path"/"filename". */
    public void prepareFileQuery(string path, string filename)
    {
        sqlrcur_prepareFileQuery(sqlrcurref, path, filename);
    }



    /** Defines a string substitution variable. */
    public void subString(string variable, string val)
    {
        sqlrcur_subString(sqlrcurref, variable, val);
    }

    /** Defines a integer substitution variable. */
    public void subLong(string variable, long val)
    {
        sqlrcur_subLong(sqlrcurref, variable, val);
    }

    /** Defines a decimal substitution variable. */
    public void subDouble(string variable, double val, uint precision, uint scale)
    {
        sqlrcur_subDouble(sqlrcurref, variable, val, precision, scale);
    }


    /** Defines a string input bind variable. */
    public void inputBindString(string variable, string val)
    {
        sqlrcur_inputBindString(sqlrcurref, variable, val);
    }

    /** Defines a string input bind variable. */
    public void inputBindStringWithLength(string variable, string val, uint vallength)
    {
        sqlrcur_inputBindStringWithLength(sqlrcurref, variable, val, vallength);
    }

    /** Defines a integer input bind variable. */
    public void inputBindLong(string variable, long val)
    {
        sqlrcur_inputBindLong(sqlrcurref, variable, val);
    }

    /** Defines a decimal input bind variable.
     * (If you don't have the precision and scale then set
     * them both to 0.  However in that case you may get
     * unexpected rounding behavior if the server is faking
     * binds.) */
    public void inputBindDouble(string variable, double val, uint precision, uint scale)
    {
        sqlrcur_inputBindDouble(sqlrcurref, variable, val, precision, scale);
    }

    /** Defines a binary lob input bind variable. */
    public void inputBindBlob(string variable, string val, uint size)
    {
        sqlrcur_inputBindBlob(sqlrcurref, variable, val, size);
    }

    /** Defines a character lob input bind variable. */
    public void inputBindClob(string variable, string val, uint size)
    {
        sqlrcur_inputBindClob(sqlrcurref, variable, val, size);
    }



    /** Defines a string output bind variable.
     *  "length" bytes will be reserved to store the value. */
    public void defineOutputBindString(string variable, uint length)
    {
        sqlrcur_defineOutputBindString(sqlrcurref, variable, length);
    }

    /** Defines an integer output bind variable. */
    public void defineOutputBindInteger(string variable)
    {
        sqlrcur_defineOutputBindInteger(sqlrcurref, variable);
    }

    /** Defines an decimal output bind variable. */
    public void defineOutputBindDouble(string variable)
    {
        sqlrcur_defineOutputBindDouble(sqlrcurref, variable);
    }

    /** Defines a binary lob output bind variable */
    public void defineOutputBindBlob(string variable)
    {
        sqlrcur_defineOutputBindBlob(sqlrcurref, variable);
    }

    /** Defines a character lob output bind variable */
    public void defineOutputBindClob(string variable)
    {
        sqlrcur_defineOutputBindClob(sqlrcurref, variable);
    }

    /** Defines a cursor output bind variable */
    public void defineOutputBindCursor(string variable)
    {
    	sqlrcur_defineOutputBindCursor(sqlrcurref, variable);
    }



    /** Clears all bind variables. */
    public void clearBinds()
    {
        sqlrcur_clearBinds(sqlrcurref);
    }

    /** Parses the previously prepared query, counts the number of bind
     *  variables defined in it and returns that number. */
    public ushort countBindVariables()
    {
        return sqlrcur_countBindVariables(sqlrcurref);
    }

    /** If you are binding to any variables that might not actually be in your
     *  query, call this to ensure that the database won't try to bind them
     *  unless they really are in the query.  There is a performance penalty
     *  for calling this function */
    public void validateBinds()
    {
        sqlrcur_validateBinds(sqlrcurref);
    }

    /** Returns true if "variable" was a valid bind variable of the query. */
    public int validBind(string variable)
    {
        return sqlrcur_validBind(sqlrcurref, variable);
    }



    /** Execute the query that was previously prepared and bound. */
    public int executeQuery()
    {
        return sqlrcur_executeQuery(sqlrcurref);
    }

    /** Fetch from a cursor that was returned as an output bind variable. */
    public int fetchFromBindCursor()
    {
        return sqlrcur_fetchFromBindCursor(sqlrcurref);
    }



    /** Get the value stored in a previously defined
     *  string output bind variable. */
    public string getOutputBindString(string variable)
    {
        return sqlrcur_getOutputBindString(sqlrcurref, variable);
    }

    /** Get the value stored in a previously defined
     *  integer output bind variable. */
    public long getOutputBindInteger(string variable)
    {
        return sqlrcur_getOutputBindInteger(sqlrcurref, variable);
    }

    /** Get the value stored in a previously defined
     *  decimal output bind variable. */
    public double getOutputBindDouble(string variable)
    {
        return sqlrcur_getOutputBindDouble(sqlrcurref, variable);
    }

    /** Get the value stored in a previously defined
     *  binary lob output bind variable. */
    public string getOutputBindBlob(string variable)
    {
        return sqlrcur_getOutputBindBlob(sqlrcurref, variable);
    }

    /** Get the value stored in a previously defined
     *  character lob output bind variable. */
    public string getOutputBindClob(string variable)
    {
        return sqlrcur_getOutputBindClob(sqlrcurref, variable);
    }

    /** Get the length of the value stored in a previously
     *  defined output bind variable. */
    public uint getOutputBindLength(string variable)
    {
        return sqlrcur_getOutputBindLength(sqlrcurref, variable);
    }

    /** Get the cursor associated with a previously defined output bind
     *  variable. */
    public IntPtr getOutputBindCursor(string variable)
    {
        return sqlrcur_getOutputBindCursor(sqlrcurref, variable);
    }



    /** Opens a cached result set.  Returns true on success and
     * false on failure. */
    public bool openCachedResultSet(string filename)
    {
        return sqlrcur_openCachedResultSet(sqlrcurref, filename)!=0;
    }



    /** Returns the number of columns in the current result set. */
    public uint colCount()
    {
        return sqlrcur_colCount(sqlrcurref);
    }

    /** Returns the number of rows in the current result set. */
    public ulong rowCount()
    {
        return sqlrcur_rowCount(sqlrcurref);
    }

    /** Returns the total number of rows that will be returned in the result
     *  set.  Not all databases support this call.  Don't use it for
     *  applications which are designed to be portable across databases.  -1
     *  is returned by databases which don't support this option. */
    public ulong totalRows()
    {
        return sqlrcur_totalRows(sqlrcurref);
    }

    /** Returns the number of rows that were updated, inserted or deleted by
     *  the query.  Not all databases support this call.  Don't use it for
     *  applications which are designed to be portable across databases.  -1
     *  is returned by databases which don't support this option. */
    public ulong affectedRows()
    {
        return sqlrcur_affectedRows(sqlrcurref);
    }

    /** Returns the index of the first buffered row.  This is useful when
     *  buffering only part of the result set at a time. */
    public ulong firstRowIndex()
    {
        return sqlrcur_firstRowIndex(sqlrcurref);
    }

    /** Returns false if part of the result set is still pending on the server
     *  and true if not.  This function can only return false if
     *  setResultSetBufferSize() has been called with a parameter other than
     *  0. */
    public bool endOfResultSet()
    {
        return sqlrcur_endOfResultSet(sqlrcurref)!=0;
    }



    /** If a query failed and generated an error, the error message is available
     *  here.  If the query succeeded then this function returns a NULL. */
    public string errorMessage()
    {
        return sqlrcur_errorMessage(sqlrcurref);
    }

    /** If a query failed and generated an error, the error number is available
     *  here.  If there is no error then this method returns 0. */
    public long errorNumber()
    {
        return sqlrcur_errorNumber(sqlrcurref);
    }


    /** Tells the connection to return NULL fields and output bind variables as
     *  empty strings.  This is the default. */
    public void getNullsAsEmptyStrings()
    {
        sqlrcur_getNullsAsEmptyStrings(sqlrcurref);
    }

    /** Tells the connection to return NULL fields
     *  and output bind variables as NULL's. */
    public void getNullsAsNulls()
    {
        sqlrcur_getNullsAsNulls(sqlrcurref);
    }



    /** Returns the specified field as a string. */
    public string getFieldByIndex(ulong row, uint col)
    {
        return sqlrcur_getFieldByIndex(sqlrcurref, row, col);
    }

    /** Returns the specified field as a string. */
    public string getFieldByName(ulong row, string col)
    {
        return sqlrcur_getFieldByName(sqlrcurref, row, col);
    }

    /** Returns the specified field as an integer. */
    public long getFieldAsIntegerByIndex(ulong row, uint col)
    {
        return sqlrcur_getFieldAsIntegerByIndex(sqlrcurref, row, col);
    }

    /** Returns the specified field as an integer. */
    public long getFieldAsIntegerByName(ulong row, string col)
    {
        return sqlrcur_getFieldAsIntegerByName(sqlrcurref, row, col);
    }

    /** Returns the specified field as an decimal. */
    public double getFieldAsDoubleByIndex(ulong row, uint col)
    {
        return sqlrcur_getFieldAsDoubleByIndex(sqlrcurref, row, col);
    }

    /** Returns the specified field as an decimal. */
    public double getFieldAsDoubleByName(ulong row, string col)
    {
        return sqlrcur_getFieldAsDoubleByName(sqlrcurref, row, col);
    }



    /** Returns the length of the specified row and column. */
    public uint getFieldLengthByIndex(ulong row, uint col)
    {
        return sqlrcur_getFieldLengthByIndex(sqlrcurref, row, col);
    }

    /** Returns the length of the specified row and column. */
    public uint getFieldLengthByName(ulong row, string col)
    {
        return sqlrcur_getFieldLengthByName(sqlrcurref, row, col);
    }



    /** Returns the name of the specified column. */
    public string getColumnName(uint col)
    {
        return sqlrcur_getColumnName(sqlrcurref, col);
    }

    /** Returns the type of the specified column. */
    public string getColumnTypeByIndex(uint col)
    {
        return sqlrcur_getColumnTypeByIndex(sqlrcurref, col);
    }

    /** Returns the type of the specified column. */
    public string getColumnTypeByName(string col)
    {
        return sqlrcur_getColumnTypeByName(sqlrcurref, col);
    }

    /** Returns the length of the specified column. */
    public uint getColumnLengthByIndex(uint col)
    {
        return sqlrcur_getColumnLengthByIndex(sqlrcurref, col);
    }

    /** Returns the length of the specified column. */
    public uint getColumnLengthByName(string col)
    {
        return sqlrcur_getColumnLengthByName(sqlrcurref, col);
    }

    /** Returns the precision of the specified column.  Precision is the total
     *  number of digits in a number.  eg: 123.45 has a precision of 5.  For
     *  non-numeric types, it's the number of characters in the string. */
    public uint getColumnPrecisionByIndex(uint col)
    {
        return sqlrcur_getColumnPrecisionByIndex(sqlrcurref, col);
    }

    /** Returns the precision of the specified column.  Precision is the total
     *  number of digits in a number.  eg: 123.45 has a precision of 5.  For
     *  non-numeric types, it's the number of characters in the string. */
    public uint getColumnPrecisionByName(string col)
    {
        return sqlrcur_getColumnPrecisionByName(sqlrcurref, col);
    }

    /** Returns the scale of the specified column.  Scale is the total number of
     *  digits to the right of the decimal point in a number.  eg: 123.45 has a
     *  scale of 2. */
    public uint getColumnScaleByIndex(uint col)
    {
        return sqlrcur_getColumnScaleByIndex(sqlrcurref, col);
    }

    /** Returns the scale of the specified column.  Scale is the total number of
     *  digits to the right of the decimal point in a number.  eg: 123.45 has a 
     *  scale of 2. */
    public uint getColumnScaleByName(string col)
    {
        return sqlrcur_getColumnScaleByName(sqlrcurref, col);
    }

    /** Returns true if the specified column can contain
     *  nulls and false otherwise. */
    public bool getColumnIsNullableByIndex(uint col)
    {
        return sqlrcur_getColumnIsNullableByIndex(sqlrcurref, col)!=0;
    }

    /** Returns true if the specified column can contain
     *  nulls and false otherwise. */
    public bool getColumnIsNullableByName(string col)
    {
        return sqlrcur_getColumnIsNullableByName(sqlrcurref, col)!=0;
    }

    /** Returns true if the specified column is a
     *  primary key and false otherwise. */
    public bool getColumnIsPrimaryKeyByIndex(uint col)
    {
        return sqlrcur_getColumnIsPrimaryKeyByIndex(sqlrcurref, col)!=0;
    }

    /** Returns true if the specified column is a
     *  primary key and false otherwise. */
    public bool getColumnIsPrimaryKeyByName(string col)
    {
        return sqlrcur_getColumnIsPrimaryKeyByName(sqlrcurref, col)!=0;
    }

    /** Returns true if the specified column is unique and false otherwise. */
    public bool getColumnIsUniqueByIndex(uint col)
    {
        return sqlrcur_getColumnIsUniqueByIndex(sqlrcurref, col)!=0;
    }

    /** Returns true if the specified column is unique and false otherwise. */
    public bool getColumnIsUniqueByName(string col)
    {
        return sqlrcur_getColumnIsUniqueByName(sqlrcurref, col)!=0;
    }

    /** Returns true if the specified column is part of a composite key and
     *  false otherwise. */
    public bool getColumnIsPartOfKeyByIndex(uint col)
    {
        return sqlrcur_getColumnIsPartOfKeyByIndex(sqlrcurref, col)!=0;
    }

    /** Returns true if the specified column is part of a composite key and
     *  false otherwise. */
    public bool getColumnIsPartOfKeyByName(string col)
    {
        return sqlrcur_getColumnIsPartOfKeyByName(sqlrcurref, col)!=0;
    }

    /** Returns true if the specified column is an unsigned number and false
     *  otherwise. */
    public bool getColumnIsUnsignedByIndex(uint col)
    {
        return sqlrcur_getColumnIsUnsignedByIndex(sqlrcurref, col)!=0;
    }

    /** Returns true if the specified column is an unsigned number and false
     *  otherwise. */
    public bool getColumnIsUnsignedByName(string col)
    {
        return sqlrcur_getColumnIsUnsignedByName(sqlrcurref, col)!=0;
    }

    /** Returns true if the specified column was created
     *  with the zero-fill flag and false otherwise. */
    public bool getColumnIsZeroFilledByIndex(uint col)
    {
        return sqlrcur_getColumnIsZeroFilledByIndex(sqlrcurref, col)!=0;
    }

    /** Returns true if the specified column was created
     *  with the zero-fill flag and false otherwise. */
    public bool getColumnIsZeroFilledByName(string col)
    {
        return sqlrcur_getColumnIsZeroFilledByName(sqlrcurref, col)!=0;
    }

    /** Returns true if the specified column contains binary data and false
     *  otherwise. */
    public bool getColumnIsBinaryByIndex(uint col)
    {
        return sqlrcur_getColumnIsBinaryByIndex(sqlrcurref, col)!=0;
    }

    /** Returns true if the specified column contains binary data and false
     *  otherwise. */
    public bool getColumnIsBinaryByName(string col)
    {
        return sqlrcur_getColumnIsBinaryByName(sqlrcurref, col)!=0;
    }

    /** Returns true if the specified column auto-increments
     *  and false otherwise. */
    public bool getColumnIsAutoIncrementByIndex(uint col)
    {
        return sqlrcur_getColumnIsAutoIncrementByIndex(sqlrcurref, col)!=0;
    }

    /** Returns true if the specified column auto-increments
     *  and false otherwise. */
    public bool getColumnIsAutoIncrementByName(string col)
    {
        return sqlrcur_getColumnIsAutoIncrementByName(sqlrcurref, col)!=0;
    }

    /** Returns the length of the longest field in the specified column. */
    public uint getLongestByIndex(uint col)
    {
        return sqlrcur_getLongestByIndex(sqlrcurref, col);
    }

    /** Returns the length of the longest field in the specified column. */
    public uint getLongestByName(string col)
    {
        return sqlrcur_getLongestByName(sqlrcurref, col);
    }



    /** Tells the server to leave this result set open when the connection calls
     *  suspendSession() so that another connection can connect to it using
     *  resumeResultSet() after it calls resumeSession(). */
    public void suspendResultSet()
    {
        sqlrcur_suspendResultSet(sqlrcurref);
    }

    /** Returns the internal ID of this result set.  This parameter may be
     *  passed to another statement for use in the resumeResultSet() function.
     *  Note: The value this function returns is only valid after a call to
     *  suspendResultSet().*/
    public ushort getResultSetId()
    {
        return sqlrcur_getResultSetId(sqlrcurref);
    }

    /** Resumes a result set previously left open using suspendSession().
     *  Returns true on success and false on failure. */
    public bool resumeResultSet(ushort id)
    {
        return sqlrcur_resumeResultSet(sqlrcurref, id)!=0;
    }

    /** Resumes a result set previously left open using suspendSession() and
     *  continues caching the result set to "filename".  Returns true on success
     *  and false on failure. */
    public bool resumeCachedResultSet(ushort id, string filename)
    {
        return sqlrcur_resumeCachedResultSet(sqlrcurref, id, filename)!=0;
    }

    private IntPtr sqlrcurref;

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern IntPtr sqlrcur_alloc_copyrefs(IntPtr sqlrconref, int copyrefs);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern void sqlrcur_free(IntPtr sqlrcurref);


    [DllImport("libsqlrclientwrapper.dll")]
    private static extern void sqlrcur_setResultSetBufferSize(IntPtr sqlrcurref, ulong rows);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern ulong sqlrcur_getResultSetBufferSize(IntPtr sqlrcurref);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern void sqlrcur_dontGetColumnInfo(IntPtr sqlrcurref);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern void sqlrcur_getColumnInfo(IntPtr sqlrcurref);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern void sqlrcur_mixedCaseColumnNames(IntPtr sqlrcurref);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern void sqlrcur_upperCaseColumnNames(IntPtr sqlrcurref);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern void sqlrcur_lowerCaseColumnNames(IntPtr sqlrcurref);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern void sqlrcur_cacheToFile(IntPtr sqlrcurref, string filename);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern void sqlrcur_setCacheTtl(IntPtr sqlrcurref, uint ttl);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern string sqlrcur_getCacheFileName(IntPtr sqlrcurref);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern void sqlrcur_cacheOff(IntPtr sqlrcurref);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern int sqlrcur_getDatabaseList(IntPtr sqlrcurref, string wild);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern int sqlrcur_getTableList(IntPtr sqlrcurref, string wild);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern int sqlrcur_getColumnList(IntPtr sqlrcurref, string table, string wild);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern int sqlrcur_sendQuery(IntPtr sqlrcurref, string query);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern int sqlrcur_sendQueryWithLength(IntPtr sqlrcurref, string query, uint length);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern int sqlrcur_sendFileQuery(IntPtr sqlrcurref, string path, string filename);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern void sqlrcur_prepareQuery(IntPtr sqlrcurref, string query);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern void sqlrcur_prepareQueryWithLength(IntPtr sqlrcurref, string query, uint length);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern void sqlrcur_prepareFileQuery(IntPtr sqlrcurref, string path, string filename);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern void sqlrcur_subString(IntPtr sqlrcurref, string variable, string val);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern void sqlrcur_subLong(IntPtr sqlrcurref, string variable, long val);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern void sqlrcur_subDouble(IntPtr sqlrcurref, string variable, double val, uint precision, uint scale);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern void sqlrcur_inputBindString(IntPtr sqlrcurref, string variable, string val);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern void sqlrcur_inputBindStringWithLength(IntPtr sqlrcurref, string variable, string val, uint vallength);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern void sqlrcur_inputBindLong(IntPtr sqlrcurref, string variable, long val);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern void sqlrcur_inputBindDouble(IntPtr sqlrcurref, string variable, double val, uint precision, uint scale);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern void sqlrcur_inputBindBlob(IntPtr sqlrcurref, string variable, string val, uint size);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern void sqlrcur_inputBindClob(IntPtr sqlrcurref, string variable, string val, uint size);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern void sqlrcur_defineOutputBindString(IntPtr sqlrcurref, string variable, uint length);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern void sqlrcur_defineOutputBindInteger(IntPtr sqlrcurref, string variable);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern void sqlrcur_defineOutputBindDouble(IntPtr sqlrcurref, string variable);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern void sqlrcur_defineOutputBindBlob(IntPtr sqlrcurref, string variable);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern void sqlrcur_defineOutputBindClob(IntPtr sqlrcurref, string variable);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern void sqlrcur_defineOutputBindCursor(IntPtr sqlrcurref, string variable);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern void sqlrcur_clearBinds(IntPtr sqlrcurref);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern ushort sqlrcur_countBindVariables(IntPtr sqlrcurref);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern void sqlrcur_validateBinds(IntPtr sqlrcurref);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern int sqlrcur_validBind(IntPtr sqlrcurref, string variable);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern int sqlrcur_executeQuery(IntPtr sqlrcurref);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern int sqlrcur_fetchFromBindCursor(IntPtr sqlrcurref);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern string sqlrcur_getOutputBindString(IntPtr sqlrcurref, string variable);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern long sqlrcur_getOutputBindInteger(IntPtr sqlrcurref, string variable);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern double sqlrcur_getOutputBindDouble(IntPtr sqlrcurref, string variable);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern string sqlrcur_getOutputBindBlob(IntPtr sqlrcurref, string variable);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern string sqlrcur_getOutputBindClob(IntPtr sqlrcurref, string variable);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern uint sqlrcur_getOutputBindLength(IntPtr sqlrcurref, string variable);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern IntPtr sqlrcur_getOutputBindCursor(IntPtr sqlrcurref, string variable);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern int sqlrcur_openCachedResultSet(IntPtr sqlrcurref, string filename);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern uint sqlrcur_colCount(IntPtr sqlrcurref);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern ulong sqlrcur_rowCount(IntPtr sqlrcurref);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern ulong sqlrcur_totalRows(IntPtr sqlrcurref);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern ulong sqlrcur_affectedRows(IntPtr sqlrcurref);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern ulong sqlrcur_firstRowIndex(IntPtr sqlrcurref);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern int sqlrcur_endOfResultSet(IntPtr sqlrcurref);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern string sqlrcur_errorMessage(IntPtr sqlrcurref);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern long sqlrcur_errorNumber(IntPtr sqlrcurref);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern void sqlrcur_getNullsAsEmptyStrings(IntPtr sqlrcurref);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern void sqlrcur_getNullsAsNulls(IntPtr sqlrcurref);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern string sqlrcur_getFieldByIndex(IntPtr sqlrcurref, ulong row, uint col);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern string sqlrcur_getFieldByName(IntPtr sqlrcurref, ulong row, string col);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern long sqlrcur_getFieldAsIntegerByIndex(IntPtr sqlrcurref, ulong row, uint col);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern long sqlrcur_getFieldAsIntegerByName(IntPtr sqlrcurref, ulong row, string col);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern double sqlrcur_getFieldAsDoubleByIndex(IntPtr sqlrcurref, ulong row, uint col);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern double sqlrcur_getFieldAsDoubleByName(IntPtr sqlrcurref, ulong row, string col);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern uint sqlrcur_getFieldLengthByIndex(IntPtr sqlrcurref, ulong row, uint col);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern uint sqlrcur_getFieldLengthByName(IntPtr sqlrcurref, ulong row, string col);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern string sqlrcur_getColumnName(IntPtr sqlrcurref, uint col);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern string sqlrcur_getColumnTypeByIndex(IntPtr sqlrcurref, uint col);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern string sqlrcur_getColumnTypeByName(IntPtr sqlrcurref, string col);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern uint sqlrcur_getColumnLengthByIndex(IntPtr sqlrcurref, uint col);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern uint sqlrcur_getColumnLengthByName(IntPtr sqlrcurref, string col);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern uint sqlrcur_getColumnPrecisionByIndex(IntPtr sqlrcurref, uint col);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern uint sqlrcur_getColumnPrecisionByName(IntPtr sqlrcurref, string col);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern uint sqlrcur_getColumnScaleByIndex(IntPtr sqlrcurref, uint col);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern uint sqlrcur_getColumnScaleByName(IntPtr sqlrcurref, string col);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern int sqlrcur_getColumnIsNullableByIndex(IntPtr sqlrcurref, uint col);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern int sqlrcur_getColumnIsNullableByName(IntPtr sqlrcurref, string col);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern int sqlrcur_getColumnIsPrimaryKeyByIndex(IntPtr sqlrcurref, uint col);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern int sqlrcur_getColumnIsPrimaryKeyByName(IntPtr sqlrcurref, string col);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern int sqlrcur_getColumnIsUniqueByIndex(IntPtr sqlrcurref, uint col);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern int sqlrcur_getColumnIsUniqueByName(IntPtr sqlrcurref, string col);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern int sqlrcur_getColumnIsPartOfKeyByIndex(IntPtr sqlrcurref, uint col);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern int sqlrcur_getColumnIsPartOfKeyByName(IntPtr sqlrcurref, string col);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern int sqlrcur_getColumnIsUnsignedByIndex(IntPtr sqlrcurref, uint col);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern int sqlrcur_getColumnIsUnsignedByName(IntPtr sqlrcurref, string col);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern int sqlrcur_getColumnIsZeroFilledByIndex(IntPtr sqlrcurref, uint col);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern int sqlrcur_getColumnIsZeroFilledByName(IntPtr sqlrcurref, string col);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern int sqlrcur_getColumnIsBinaryByIndex(IntPtr sqlrcurref, uint col);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern int sqlrcur_getColumnIsBinaryByName(IntPtr sqlrcurref, string col);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern int sqlrcur_getColumnIsAutoIncrementByIndex(IntPtr sqlrcurref, uint col);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern int sqlrcur_getColumnIsAutoIncrementByName(IntPtr sqlrcurref, string col);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern uint sqlrcur_getLongestByIndex(IntPtr sqlrcurref, uint col);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern uint sqlrcur_getLongestByName(IntPtr sqlrcurref, string col);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern void sqlrcur_suspendResultSet(IntPtr sqlrcurref);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern ushort sqlrcur_getResultSetId(IntPtr sqlrcurref);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern int sqlrcur_resumeResultSet(IntPtr sqlrcurref, ushort id);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern int sqlrcur_resumeCachedResultSet(IntPtr sqlrcurref, ushort id, string filename);
}
