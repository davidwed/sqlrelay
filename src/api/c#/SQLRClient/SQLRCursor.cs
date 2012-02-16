using System;
using System.Runtime.InteropServices;

public class SQLRCursor
{

    /** 
     *  Creates a cursor to run queries and fetch
     *  result sets using connection "sqlrconref" */
    [DllImport("libsqlrclientwrapper.dll")]
    IntPtr sqlrcur_alloc(IntPtr sqlrconref);

    /** 
     *  Destroys the cursor and cleans up all associated result set data. */
    [DllImport("libsqlrclientwrapper.dll")]
    void sqlrcur_free(IntPtr sqlrcurref);



    /** 
     *  Sets the number of rows of the result set to buffer at a time.
     *  0 (the default) means buffer the entire result set. */
    [DllImport("libsqlrclientwrapper.dll")]
    void sqlrcur_setResultSetBufferSize(IntPtr sqlrcurref, ulong rows);

    /** 
     *  Returns the number of result set rows that will be buffered at a time or
     *  0 for the entire result set. */
    [DllImport("libsqlrclientwrapper.dll")]
    ulong sqlrcur_getResultSetBufferSize(IntPtr sqlrcurref);



    /** 
     *  Tells the server not to send any column info (names, types, sizes).  If
     *  you don't need that info, you should call this function to improve
     *  performance. */
    [DllImport("libsqlrclientwrapper.dll")]
    void sqlrcur_dontGetColumnInfo(IntPtr sqlrcurref);

    /** 
     *  Tells the server to send column info. */
    [DllImport("libsqlrclientwrapper.dll")]
    void sqlrcur_getColumnInfo(IntPtr sqlrcurref);



    /** 
     *  Columns names are returned in the same case as they are defined in the
     *  database.  This is the default. */
    [DllImport("libsqlrclientwrapper.dll")]
    void sqlrcur_mixedCaseColumnNames(IntPtr sqlrcurref);

    /** 
     *  Columns names are converted to upper case. */
    [DllImport("libsqlrclientwrapper.dll")]
    void sqlrcur_upperCaseColumnNames(IntPtr sqlrcurref);

    /** 
     *  Columns names are converted to lower case. */
    [DllImport("libsqlrclientwrapper.dll")]
    void sqlrcur_lowerCaseColumnNames(IntPtr sqlrcurref);



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
    [DllImport("libsqlrclientwrapper.dll")]
    void sqlrcur_cacheToFile(IntPtr sqlrcurref, string filename);

    /** 
     *  Sets the time-to-live for cached result sets. The sqlr-cachemanger will
     *  remove each cached result set "ttl" seconds after it's created, provided
     *  it's scanning the directory containing the cache files. */
    [DllImport("libsqlrclientwrapper.dll")]
    void sqlrcur_setCacheTtl(IntPtr sqlrcurref, uint ttl);

    /** 
     *  Returns the name of the file containing
     *  the most recently cached result set. */
    [DllImport("libsqlrclientwrapper.dll")]
    string sqlrcur_getCacheFileName(IntPtr sqlrcurref);

    /** 
     *  Sets query caching off. */
    [DllImport("libsqlrclientwrapper.dll")]
    void sqlrcur_cacheOff(IntPtr sqlrcurref);



    /** 
     *  Sends a query that returns a list of databases/schemas matching "wild".
     *  If wild is empty or NULL then a list of all databases/schemas will be
     *  returned. */
    [DllImport("libsqlrclientwrapper.dll")]
    int sqlrcur_getDatabaseList(IntPtr sqlrcurref, string wild);

    /** 
     *  Sends a query that returns a list of tables matching "wild".  If wild is
     *  empty or NULL then a list of all tables will be returned. */
    [DllImport("libsqlrclientwrapper.dll")]
    int sqlrcur_getTableList(IntPtr sqlrcurref, string wild);

    /** 
     *  Sends a query that returns a list of columns in the table specified by
     *  the "table" parameter matching "wild".  If wild is empty or NULL then a
     *  list of all columns will be returned. */
    [DllImport("libsqlrclientwrapper.dll")]
    int sqlrcur_getColumnList(IntPtr sqlrcurref, string table, string wild);



    /** 
     *  Sends "query" directly and gets a result set. */
    [DllImport("libsqlrclientwrapper.dll")]
    int sqlrcur_sendQuery(IntPtr sqlrcurref, string query);

    /** 
     *  Sends "query" with length "length" directly and gets a result set. This
     *  function must be used if the query contains binary data. */
    [DllImport("libsqlrclientwrapper.dll")]
    int sqlrcur_sendQueryWithLength(IntPtr sqlrcurref, string query, uint length);

    /** 
     *  Sends the query in file "path"/"filename" and gets a result set. */
    [DllImport("libsqlrclientwrapper.dll")]
    int sqlrcur_sendFileQuery(IntPtr sqlrcurref, string path, string filename);



    /** 
     *  Prepare to execute "query". */
    [DllImport("libsqlrclientwrapper.dll")]
    void sqlrcur_prepareQuery(IntPtr sqlrcurref, string query);

    /** 
     *  Prepare to execute "query" with length "length".  This function must be
     *  used if the query contains binary data. */
    [DllImport("libsqlrclientwrapper.dll")]
    void sqlrcur_prepareQueryWithLength(IntPtr sqlrcurref, string query, uint length);

    /** 
     *  Prepare to execute the contents of "path"/"filename". */
    [DllImport("libsqlrclientwrapper.dll")]
    void sqlrcur_prepareFileQuery(IntPtr sqlrcurref, string path, string filename);



    /** 
     *  Defines a string substitution variable. */
    [DllImport("libsqlrclientwrapper.dll")]
    void sqlrcur_subString(IntPtr sqlrcurref, string variable, string val);

    /** 
     *  Defines a integer substitution variable. */
    [DllImport("libsqlrclientwrapper.dll")]
    void sqlrcur_subLong(IntPtr sqlrcurref, string variable, long val);

    /** 
     *  Defines a decimal substitution variable. */
    [DllImport("libsqlrclientwrapper.dll")]
    void sqlrcur_subDouble(IntPtr sqlrcurref, string variable, double val, uint precision, uint scale);


    /** 
     *  Defines a string input bind variable. */
    [DllImport("libsqlrclientwrapper.dll")]
    void sqlrcur_inputBindString(IntPtr sqlrcurref, string variable, string val);

    /** 
     *  Defines a string input bind variable. */
    [DllImport("libsqlrclientwrapper.dll")]
    void sqlrcur_inputBindStringWithLength(IntPtr sqlrcurref, string variable, string val, uint vallength);

    /** 
     *  Defines a integer input bind variable. */
    [DllImport("libsqlrclientwrapper.dll")]
    void sqlrcur_inputBindLong(IntPtr sqlrcurref, string variable, long val);

    /** 
     *  Defines a decimal input bind variable.
     * (If you don't have the precision and scale then set
     * them both to 0.  However in that case you may get
     * unexpected rounding behavior if the server is faking
     * binds.) */
    [DllImport("libsqlrclientwrapper.dll")]
    void sqlrcur_inputBindDouble(IntPtr sqlrcurref, string variable, double val, uint precision, uint scale);

    /** 
     *  Defines a binary lob input bind variable. */
    [DllImport("libsqlrclientwrapper.dll")]
    void sqlrcur_inputBindBlob(IntPtr sqlrcurref, string variable, string val, uint size);

    /** 
     *  Defines a character lob input bind variable. */
    [DllImport("libsqlrclientwrapper.dll")]
    void sqlrcur_inputBindClob(IntPtr sqlrcurref, string variable, string val, uint size);



    /** 
     *  Defines a string output bind variable.
     *  "length" bytes will be reserved to store the value. */
    [DllImport("libsqlrclientwrapper.dll")]
    void sqlrcur_defineOutputBindString(IntPtr sqlrcurref, string variable, uint length);

    /** 
     *  Defines an integer output bind variable. */
    [DllImport("libsqlrclientwrapper.dll")]
    void sqlrcur_defineOutputBindInteger(IntPtr sqlrcurref, string variable);

    /** 
     *  Defines an decimal output bind variable. */
    [DllImport("libsqlrclientwrapper.dll")]
    void sqlrcur_defineOutputBindDouble(IntPtr sqlrcurref, string variable);

    /** 
     *  Defines a binary lob output bind variable */
    [DllImport("libsqlrclientwrapper.dll")]
    void sqlrcur_defineOutputBindBlob(IntPtr sqlrcurref, string variable);

    /** 
     *  Defines a character lob output bind variable */
    [DllImport("libsqlrclientwrapper.dll")]
    void sqlrcur_defineOutputBindClob(IntPtr sqlrcurref, string variable);

    /** 
     *  Defines a cursor output bind variable */
    [DllImport("libsqlrclientwrapper.dll")]
    void sqlrcur_defineOutputBindCursor(IntPtr sqlrcurref, string variable);



    /** 
     *  Clears all bind variables. */
    [DllImport("libsqlrclientwrapper.dll")]
    void sqlrcur_clearBinds(IntPtr sqlrcurref);

    /** 
     *  Parses the previously prepared query, counts the number of bind
     *  variables defined in it and returns that number. */
    [DllImport("libsqlrclientwrapper.dll")]
    ushort sqlrcur_countBindVariables(IntPtr sqlrcurref);

    /** 
     *  If you are binding to any variables that might not actually be in your
     *  query, call this to ensure that the database won't try to bind them
     *  unless they really are in the query.  There is a performance penalty
     *  for calling this function */
    [DllImport("libsqlrclientwrapper.dll")]
    void sqlrcur_validateBinds(IntPtr sqlrcurref);

    /** 
     *  Returns true if "variable" was a valid bind variable of the query. */
    [DllImport("libsqlrclientwrapper.dll")]
    int sqlrcur_validBind(IntPtr sqlrcurref, string variable);



    /** 
     *  Execute the query that was previously prepared and bound. */
    [DllImport("libsqlrclientwrapper.dll")]
    int sqlrcur_executeQuery(IntPtr sqlrcurref);

    /** 
     *  Fetch from a cursor that was returned as an output bind variable. */
    [DllImport("libsqlrclientwrapper.dll")]
    int sqlrcur_fetchFromBindCursor(IntPtr sqlrcurref);



    /** 
     *  Get the value stored in a previously defined
     *  string output bind variable. */
    [DllImport("libsqlrclientwrapper.dll")]
    string sqlrcur_getOutputBindString(IntPtr sqlrcurref, string variable);

    /** 
     *  Get the value stored in a previously defined
     *  integer output bind variable. */
    [DllImport("libsqlrclientwrapper.dll")]
    long sqlrcur_getOutputBindInteger(IntPtr sqlrcurref, string variable);

    /** 
     *  Get the value stored in a previously defined
     *  decimal output bind variable. */
    [DllImport("libsqlrclientwrapper.dll")]
    double sqlrcur_getOutputBindDouble(IntPtr sqlrcurref, string variable);

    /** 
     *  Get the value stored in a previously defined
     *  binary lob output bind variable. */
    [DllImport("libsqlrclientwrapper.dll")]
    string sqlrcur_getOutputBindBlob(IntPtr sqlrcurref, string variable);

    /** 
     *  Get the value stored in a previously defined
     *  character lob output bind variable. */
    [DllImport("libsqlrclientwrapper.dll")]
    string sqlrcur_getOutputBindClob(IntPtr sqlrcurref, string variable);

    /** 
     *  Get the length of the value stored in a previously
     *  defined output bind variable. */
    [DllImport("libsqlrclientwrapper.dll")]
    uint sqlrcur_getOutputBindLength(IntPtr sqlrcurref, string variable);

    /** 
     *  Get the cursor associated with a previously defined output bind
     *  variable. */
    [DllImport("libsqlrclientwrapper.dll")]
    IntPtr sqlrcur_getOutputBindCursor(IntPtr sqlrcurref, string variable);



    /** 
     *  Opens a cached result set.  Returns 1 on success and 0 on failure. */
    [DllImport("libsqlrclientwrapper.dll")]
    int sqlrcur_openCachedResultSet(IntPtr sqlrcurref, string filename);



    /** 
     *  Returns the number of columns in the current result set. */
    [DllImport("libsqlrclientwrapper.dll")]
    uint sqlrcur_colCount(IntPtr sqlrcurref);

    /** 
     *  Returns the number of rows in the current result set. */
    [DllImport("libsqlrclientwrapper.dll")]
    ulong sqlrcur_rowCount(IntPtr sqlrcurref);

    /** 
     *  Returns the total number of rows that will be returned in the result
     *  set.  Not all databases support this call.  Don't use it for
     *  applications which are designed to be portable across databases.  -1
     *  is returned by databases which don't support this option. */
    [DllImport("libsqlrclientwrapper.dll")]
    ulong sqlrcur_totalRows(IntPtr sqlrcurref);

    /** 
     *  Returns the number of rows that were updated, inserted or deleted by
     *  the query.  Not all databases support this call.  Don't use it for
     *  applications which are designed to be portable across databases.  -1
     *  is returned by databases which don't support this option. */
    [DllImport("libsqlrclientwrapper.dll")]
    ulong sqlrcur_affectedRows(IntPtr sqlrcurref);

    /** 
     *  Returns the index of the first buffered row.  This is useful when
     *  buffering only part of the result set at a time. */
    [DllImport("libsqlrclientwrapper.dll")]
    ulong sqlrcur_firstRowIndex(IntPtr sqlrcurref);

    /** 
     *  Returns 0 if part of the result set is still pending on the server and
     *  1 if not.  This function can only return 0 if setResultSetBufferSize()
     *  has been called with a parameter other than 0. */
    [DllImport("libsqlrclientwrapper.dll")]
    int sqlrcur_endOfResultSet(IntPtr sqlrcurref);



    /** 
     *  If a query failed and generated an error, the error message is available
     *  here.  If the query succeeded then this function returns a NULL. */
    [DllImport("libsqlrclientwrapper.dll")]
    string sqlrcur_errorMessage(IntPtr sqlrcurref);

    /** 
     *  If a query failed and generated an error, the error number is available
     *  here.  If there is no error then this method returns 0. */
    [DllImport("libsqlrclientwrapper.dll")]
    long sqlrcur_errorNumber(IntPtr sqlrcurref);


    /** 
     *  Tells the connection to return NULL fields and output bind variables as
     *  empty strings.  This is the default. */
    [DllImport("libsqlrclientwrapper.dll")]
    void sqlrcur_getNullsAsEmptyStrings(IntPtr sqlrcurref);

    /** 
     *  Tells the connection to return NULL fields
     *  and output bind variables as NULL's. */
    [DllImport("libsqlrclientwrapper.dll")]
    void sqlrcur_getNullsAsNulls(IntPtr sqlrcurref);



    /** 
     *  Returns the specified field as a string. */
    [DllImport("libsqlrclientwrapper.dll")]
    string sqlrcur_getFieldByIndex(IntPtr sqlrcurref, ulong row, uint col);

    /** 
     *  Returns the specified field as a string. */
    [DllImport("libsqlrclientwrapper.dll")]
    string sqlrcur_getFieldByName(IntPtr sqlrcurref, ulong row, string col);

    /** 
     *  Returns the specified field as an integer. */
    [DllImport("libsqlrclientwrapper.dll")]
    long sqlrcur_getFieldAsIntegerByIndex(IntPtr sqlrcurref, ulong row, uint col);

    /** 
     *  Returns the specified field as an integer. */
    [DllImport("libsqlrclientwrapper.dll")]
    long sqlrcur_getFieldAsIntegerByName(IntPtr sqlrcurref, ulong row, string col);

    /** 
     *  Returns the specified field as an decimal. */
    [DllImport("libsqlrclientwrapper.dll")]
    double sqlrcur_getFieldAsDoubleByIndex(IntPtr sqlrcurref, ulong row, uint col);

    /** 
     *  Returns the specified field as an decimal. */
    [DllImport("libsqlrclientwrapper.dll")]
    double sqlrcur_getFieldAsDoubleByName(IntPtr sqlrcurref, ulong row, string col);



    /** 
     *  Returns the length of the specified row and column. */
    [DllImport("libsqlrclientwrapper.dll")]
    uint sqlrcur_getFieldLengthByIndex(IntPtr sqlrcurref, ulong row, uint col);

    /** 
     *  Returns the length of the specified row and column. */
    [DllImport("libsqlrclientwrapper.dll")]
    uint sqlrcur_getFieldLengthByName(IntPtr sqlrcurref, ulong row, string col);



    /** 
     *  Returns the name of the specified column. */
    [DllImport("libsqlrclientwrapper.dll")]
    string sqlrcur_getColumnName(IntPtr sqlrcurref, uint col);

    /** 
     *  Returns the type of the specified column. */
    [DllImport("libsqlrclientwrapper.dll")]
    string sqlrcur_getColumnTypeByIndex(IntPtr sqlrcurref, uint col);

    /** 
     *  Returns the type of the specified column. */
    [DllImport("libsqlrclientwrapper.dll")]
    string sqlrcur_getColumnTypeByName(IntPtr sqlrcurref, string col);

    /** 
     *  Returns the length of the specified column. */
    [DllImport("libsqlrclientwrapper.dll")]
    uint sqlrcur_getColumnLengthByIndex(IntPtr sqlrcurref, uint col);

    /** 
     *  Returns the length of the specified column. */
    [DllImport("libsqlrclientwrapper.dll")]
    uint sqlrcur_getColumnLengthByName(IntPtr sqlrcurref, string col);

    /** 
     *  Returns the precision of the specified column.  Precision is the total
     *  number of digits in a number.  eg: 123.45 has a precision of 5.  For
     *  non-numeric types, it's the number of characters in the string. */
    [DllImport("libsqlrclientwrapper.dll")]
    uint sqlrcur_getColumnPrecisionByIndex(IntPtr sqlrcurref, uint col);

    /** 
     *  Returns the precision of the specified column.  Precision is the total
     *  number of digits in a number.  eg: 123.45 has a precision of 5.  For
     *  non-numeric types, it's the number of characters in the string. */
    [DllImport("libsqlrclientwrapper.dll")]
    uint sqlrcur_getColumnPrecisionByName(IntPtr sqlrcurref, string col);

    /** 
     *  Returns the scale of the specified column.  Scale is the total number of
     *  digits to the right of the decimal point in a number.  eg: 123.45 has a
     *  scale of 2. */
    [DllImport("libsqlrclientwrapper.dll")]
    uint sqlrcur_getColumnScaleByIndex(IntPtr sqlrcurref, uint col);

    /** 
     *  Returns the scale of the specified column.  Scale is the total number of
     *  digits to the right of the decimal point in a number.  eg: 123.45 has a 
     *  scale of 2. */
    [DllImport("libsqlrclientwrapper.dll")]
    uint sqlrcur_getColumnScaleByName(IntPtr sqlrcurref, string col);

    /** 
     *  Returns the scale of the specified column.  Scale is the total number of
     *  digits to the right of the decimal point in a number.  eg: 123.45 has a
     *  scale of 2. */
    [DllImport("libsqlrclientwrapper.dll")]
    int sqlrcur_getColumnIsNullableByIndex(IntPtr sqlrcurref, uint col);

    /** 
     *  Returns 1 if the specified column can contain nulls and 0 otherwise. */
    [DllImport("libsqlrclientwrapper.dll")]
    int sqlrcur_getColumnIsNullableByName(IntPtr sqlrcurref, string col);

    /** 
     *  Returns 1 if the specified column is a primary key and 0 otherwise. */
    [DllImport("libsqlrclientwrapper.dll")]
    int sqlrcur_getColumnIsPrimaryKeyByIndex(IntPtr sqlrcurref, uint col);

    /** 
     *  Returns 1 if the specified column is a primary key and 0 otherwise. */
    [DllImport("libsqlrclientwrapper.dll")]
    int sqlrcur_getColumnIsPrimaryKeyByName(IntPtr sqlrcurref, string col);

    /** 
     *  Returns 1 if the specified column is unique and 0 otherwise. */
    int sqlrcur_getColumnIsUniqueByIndex(IntPtr sqlrcurref, uint col);

    /** 
     *  Returns 1 if the specified column is unique and 0 otherwise. */
    [DllImport("libsqlrclientwrapper.dll")]
    int sqlrcur_getColumnIsUniqueByName(IntPtr sqlrcurref, string col);

    /** 
     *  Returns 1 if the specified column is part of a composite key and 0
     *  otherwise. */
    [DllImport("libsqlrclientwrapper.dll")]
    int sqlrcur_getColumnIsPartOfKeyByIndex(IntPtr sqlrcurref, uint col);

    /** 
     *  Returns 1 if the specified column is part of a composite key and 0
     *  otherwise. */
    [DllImport("libsqlrclientwrapper.dll")]
    int sqlrcur_getColumnIsPartOfKeyByName(IntPtr sqlrcurref, string col);

    /** 
     *  Returns 1 if the specified column is an unsigned number and 0
     *  otherwise. */
    [DllImport("libsqlrclientwrapper.dll")]
    int sqlrcur_getColumnIsUnsignedByIndex(IntPtr sqlrcurref, uint col);

    /** 
     *  Returns 1 if the specified column is an unsigned number and 0
     *  otherwise. */
    [DllImport("libsqlrclientwrapper.dll")]
    int sqlrcur_getColumnIsUnsignedByName(IntPtr sqlrcurref, string col);

    /** 
     *  Returns 1 if the specified column was created
     *  with the zero-fill flag and 0 otherwise. */
    [DllImport("libsqlrclientwrapper.dll")]
    int sqlrcur_getColumnIsZeroFilledByIndex(IntPtr sqlrcurref, uint col);

    /** 
     *  Returns 1 if the specified column was created
     *  with the zero-fill flag and 0 otherwise. */
    [DllImport("libsqlrclientwrapper.dll")]
    int sqlrcur_getColumnIsZeroFilledByName(IntPtr sqlrcurref, string col);

    /** 
     *  Returns 1 if the specified column contains binary data and 0
     *  otherwise. */
    [DllImport("libsqlrclientwrapper.dll")]
    int sqlrcur_getColumnIsBinaryByIndex(IntPtr sqlrcurref, uint col);

    /** 
     *  Returns 1 if the specified column contains binary data and 0
     *  otherwise. */
    [DllImport("libsqlrclientwrapper.dll")]
    int sqlrcur_getColumnIsBinaryByName(IntPtr sqlrcurref, string col);

    /** 
     *  Returns 1 if the specified column auto-increments and 0 otherwise. */
    [DllImport("libsqlrclientwrapper.dll")]
    int sqlrcur_getColumnIsAutoIncrementByIndex(IntPtr sqlrcurref, uint col);

    /** 
     *  Returns 1 if the specified column auto-increments and 0 otherwise. */
    [DllImport("libsqlrclientwrapper.dll")]
    int sqlrcur_getColumnIsAutoIncrementByName(IntPtr sqlrcurref, string col);

    /** 
     *  Returns the length of the longest field in the specified column. */
    [DllImport("libsqlrclientwrapper.dll")]
    uint sqlrcur_getLongestByIndex(IntPtr sqlrcurref, uint col);

    /** 
     *  Returns the length of the longest field in the specified column. */
    [DllImport("libsqlrclientwrapper.dll")]
    uint sqlrcur_getLongestByName(IntPtr sqlrcurref, string col);



    /** 
     *  Tells the server to leave this result set open when the connection calls
     *  suspendSession() so that another connection can connect to it using
     *  resumeResultSet() after it calls resumeSession(). */
    [DllImport("libsqlrclientwrapper.dll")]
    void sqlrcur_suspendResultSet(IntPtr sqlrcurref);

    /** 
     *  Returns the internal ID of this result set.  This parameter may be
     *  passed to another statement for use in the resumeResultSet() function.
     *  Note: The value this function returns is only valid after a call to
     *  suspendResultSet().*/
    [DllImport("libsqlrclientwrapper.dll")]
    ushort sqlrcur_getResultSetId(IntPtr sqlrcurref);

    /** 
     *  Resumes a result set previously left open using suspendSession().
     *  Returns 1 on success and 0 on failure. */
    [DllImport("libsqlrclientwrapper.dll")]
    int sqlrcur_resumeResultSet(IntPtr sqlrcurref, ushort id);

    /** 
     *  Resumes a result set previously left open using suspendSession() and
     *  continues caching the result set to "filename".  Returns 1 on success
     *  and 0 on failure. */
    [DllImport("libsqlrclientwrapper.dll")]
    int sqlrcur_resumeCachedResultSet(IntPtr sqlrcurref, ushort id, string filename);

}
