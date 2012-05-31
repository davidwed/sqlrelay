using System;
using System.ComponentModel;
using System.Runtime.InteropServices;
using SQLRClient;

namespace SQLRClient
{

public class SQLRCursor : IDisposable
{

    /** Creates a cursor to run queries and fetch
     *  result sets using connection "conn" */
    public SQLRCursor(SQLRConnection conn)
    {
        sqlrcurref = sqlrcur_alloc_copyrefs(conn.getInternalConnectionStructure(), 1);
    }

    /** Dispose framework */
    private Boolean  disposed = false;
    public void Dispose()
    {
        Dispose(true);
        GC.SuppressFinalize(this);
    }
    protected virtual void Dispose(Boolean  disposing)
    {
        if (!disposed)
        {
            sqlrcur_free(sqlrcurref);
            disposed = true;
        }
    }

    /** Destroys the cursor and cleans up all associated result set data. */
    ~SQLRCursor()
    {
        Dispose(false);
    }



    /** Sets the number of rows of the result set to buffer at a time.
     *  0 (the default) means buffer the entire result set. */
    public void setResultSetBufferSize(UInt64 rows)
    {
        sqlrcur_setResultSetBufferSize(sqlrcurref, rows);
    }

    /** Returns the number of result set rows that will be buffered at a time or
     *  0 for the entire result set. */
    public UInt64 getResultSetBufferSize()
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
    public void cacheToFile(String filename)
    {
        sqlrcur_cacheToFile(sqlrcurref, filename);
    }

    /** Sets the time-to-live for cached result sets. The sqlr-cachemanger will
     *  remove each cached result set "ttl" seconds after it's created, provided
     *  it's scanning the directory containing the cache files. */
    public void setCacheTtl(UInt32 ttl)
    {
        sqlrcur_setCacheTtl(sqlrcurref, ttl);
    }

    /** Returns the name of the file containing
     *  the most recently cached result set. */
    public String getCacheFileName()
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
    public Boolean  getDatabaseList(String wild)
    {
        return sqlrcur_getDatabaseList(sqlrcurref, wild) != 0;
    }

    /** Sends a query that returns a list of tables matching "wild".  If wild is
     *  empty or NULL then a list of all tables will be returned. */
    public Boolean  getTableList(String wild)
    {
        return sqlrcur_getTableList(sqlrcurref, wild) != 0;
    }

    /** Sends a query that returns a list of columns in the table specified by
     *  the "table" parameter matching "wild".  If wild is empty or NULL then a
     *  list of all columns will be returned. */
    public Boolean  getColumnList(String table, String wild)
    {
        return sqlrcur_getColumnList(sqlrcurref, table, wild) != 0;
    }



    /** Sends "query" directly and gets a result set. */
    public Boolean  sendQuery(String query)
    {
        return sqlrcur_sendQuery(sqlrcurref, query) != 0;
    }

    /** Sends "query" with length "length" directly and gets a result set. This
     *  function must be used if the query contains binary data. */
    public Boolean  sendQuery(String query, UInt32 length)
    {
        return sqlrcur_sendQueryWithLength(sqlrcurref, query, length) != 0;
    }

    /** Sends the query in file "path"/"filename" and gets a result set. */
    public Boolean  sendFileQuery(String path, String filename)
    {
        return sqlrcur_sendFileQuery(sqlrcurref, path, filename) != 0;
    }



    /** Prepare to execute "query". */
    public void prepareQuery(String query)
    {
        sqlrcur_prepareQuery(sqlrcurref, query);
    }

    /** Prepare to execute "query" with length "length".  This function must be
     *  used if the query contains binary data. */
    public void prepareQuery(String query, UInt32 length)
    {
        sqlrcur_prepareQueryWithLength(sqlrcurref, query, length);
    }

    /** Prepare to execute the contents of "path"/"filename". */
    public void prepareFileQuery(String path, String filename)
    {
        sqlrcur_prepareFileQuery(sqlrcurref, path, filename);
    }



    /** Defines a String substitution variable. */
    public void substitution(String variable, String val)
    {
        sqlrcur_subString(sqlrcurref, variable, val);
    }

    /** Defines a integer substitution variable. */
    public void substitution(String variable, Int64 val)
    {
        sqlrcur_subLong(sqlrcurref, variable, val);
    }

    /** Defines a decimal substitution variable. */
    public void substitution(String variable, Double val, UInt32 precision, UInt32 scale)
    {
        sqlrcur_subDouble(sqlrcurref, variable, val, precision, scale);
    }


    /** Defines a String input bind variable. */
    public void inputBind(String variable, String val)
    {
        sqlrcur_inputBindString(sqlrcurref, variable, val);
    }

    /** Defines a String input bind variable. */
    public void inputBind(String variable, String val, UInt32 vallength)
    {
        sqlrcur_inputBindStringWithLength(sqlrcurref, variable, val, vallength);
    }

    /** Defines a integer input bind variable. */
    public void inputBind(String variable, Int64 val)
    {
        sqlrcur_inputBindLong(sqlrcurref, variable, val);
    }

    /** Defines a decimal input bind variable.
     * (If you don't have the precision and scale then set
     * them both to 0.  However in that case you may get
     * unexpected rounding behavior if the server is faking
     * binds.) */
    public void inputBind(String variable, Double val, UInt32 precision, UInt32 scale)
    {
        sqlrcur_inputBindDouble(sqlrcurref, variable, val, precision, scale);
    }

    /**  Defines a date input bind variable.  "day" should be
     *  1-31 and "month" should be 1-12.  Any date components
     *  that you don't want used should be set to -1.  "tz" may
     *  be left NULL.  Most databases ignore "tz".  */
    public void inputBind(String variable, Int16 year, Int16 month, Int16 day, Int16 hour, Int16 minute, Int16 second, String tz)
    {
        sqlrcur_inputBindDate(sqlrcurref, variable, year, month, day, hour, minute, second, tz);
    }

    /** Defines a binary lob input bind variable. */
    public void inputBindBlob(String variable, Byte[] val, UInt32 size)
    {
        sqlrcur_inputBindBlob(sqlrcurref, variable, val, size);
    }

    /** Defines a character lob input bind variable. */
    public void inputBindClob(String variable, String val, UInt32 size)
    {
        sqlrcur_inputBindClob(sqlrcurref, variable, val, size);
    }



    /** Defines a String output bind variable.
     *  "length" Bytes will be reserved to store the value. */
    public void defineOutputBindString(String variable, UInt32 length)
    {
        sqlrcur_defineOutputBindString(sqlrcurref, variable, length);
    }

    /** Defines an integer output bind variable. */
    public void defineOutputBindInteger(String variable)
    {
        sqlrcur_defineOutputBindInteger(sqlrcurref, variable);
    }

    /** Defines an decimal output bind variable. */
    public void defineOutputBindDouble(String variable)
    {
        sqlrcur_defineOutputBindDouble(sqlrcurref, variable);
    }

    /** Defines an date output bind variable. */
    public void defineOutputBindDate(String variable)
    {
        sqlrcur_defineOutputBindDate(sqlrcurref, variable);
    }

    /** Defines a binary lob output bind variable */
    public void defineOutputBindBlob(String variable)
    {
        sqlrcur_defineOutputBindBlob(sqlrcurref, variable);
    }

    /** Defines a character lob output bind variable */
    public void defineOutputBindClob(String variable)
    {
        sqlrcur_defineOutputBindClob(sqlrcurref, variable);
    }

    /** Defines a cursor output bind variable */
    public void defineOutputBindCursor(String variable)
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
    public UInt16 countBindVariables()
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
    public Boolean  validBind(String variable)
    {
        return sqlrcur_validBind(sqlrcurref, variable) != 0;
    }



    /** Execute the query that was previously prepared and bound. */
    public Boolean  executeQuery()
    {
        return sqlrcur_executeQuery(sqlrcurref) != 0;
    }

    /** Fetch from a cursor that was returned as an output bind variable. */
    public Boolean  fetchFromBindCursor()
    {
        return sqlrcur_fetchFromBindCursor(sqlrcurref) != 0;
    }



    /** Get the value stored in a previously defined
     *  String output bind variable. */
    public String getOutputBindString(String variable)
    {
        return sqlrcur_getOutputBindString(sqlrcurref, variable);
    }

    /** Get the value stored in a previously defined
     *  integer output bind variable. */
    public Int64 getOutputBindInteger(String variable)
    {
        return sqlrcur_getOutputBindInteger(sqlrcurref, variable);
    }

    /** Get the value stored in a previously defined
     *  decimal output bind variable. */
    public double getOutputBindDouble(String variable)
    {
        return sqlrcur_getOutputBindDouble(sqlrcurref, variable);
    }

    /** Get the value stored in a previously defined
     *  decimal output bind variable. */
    public Boolean getOutputBindDate(String variable, out Int16 year, out Int16 month, out Int16 day, out Int16 hour, out Int16 minute, out Int16 second, out String tz)
    {
        year = -1;
        month = -1;
        day = -1;
        hour = -1;
        minute = -1;
        second = -1;
        tz = "";
        sqlrcur_getOutputBindDate(sqlrcurref, variable, ref year, ref month, ref day, ref hour, ref minute, ref second, ref tz);
        return false;
    }

    /** Get the value stored in a previously defined
     *  binary lob output bind variable. */
    public Byte[] getOutputBindBlob(String variable)
    {
        Int32 size = (Int32)sqlrcur_getOutputBindLength(sqlrcurref, variable);
        if (size == 0)
        {
            return null;
        }
        Byte[] retval = new Byte[size];
        Marshal.Copy(sqlrcur_getOutputBindBlob(sqlrcurref, variable), retval, 0, size);
        return retval;
    }

    /** Get the value stored in a previously defined
     *  character lob output bind variable. */
    public String getOutputBindClob(String variable)
    {
        return sqlrcur_getOutputBindClob(sqlrcurref, variable);
    }

    /** Get the length of the value stored in a previously
     *  defined output bind variable. */
    public UInt32 getOutputBindLength(String variable)
    {
        return sqlrcur_getOutputBindLength(sqlrcurref, variable);
    }

    /** Get the cursor associated with a previously defined output bind
     *  variable. */
    public IntPtr getOutputBindCursor(String variable)
    {
        return sqlrcur_getOutputBindCursor_copyrefs(sqlrcurref, variable, 1);
    }



    /** Opens a cached result set.  Returns true on success and
     * false on failure. */
    public Boolean  openCachedResultSet(String filename)
    {
        return sqlrcur_openCachedResultSet(sqlrcurref, filename)!=0;
    }



    /** Returns the number of columns in the current result set. */
    public UInt32 colCount()
    {
        return sqlrcur_colCount(sqlrcurref);
    }

    /** Returns the number of rows in the current result set. */
    public UInt64 rowCount()
    {
        return sqlrcur_rowCount(sqlrcurref);
    }

    /** Returns the total number of rows that will be returned in the result
     *  set.  Not all databases support this call.  Don't use it for
     *  applications which are designed to be portable across databases.  -1
     *  is returned by databases which don't support this option. */
    public UInt64 totalRows()
    {
        return sqlrcur_totalRows(sqlrcurref);
    }

    /** Returns the number of rows that were updated, inserted or deleted by
     *  the query.  Not all databases support this call.  Don't use it for
     *  applications which are designed to be portable across databases.  -1
     *  is returned by databases which don't support this option. */
    public UInt64 affectedRows()
    {
        return sqlrcur_affectedRows(sqlrcurref);
    }

    /** Returns the index of the first buffered row.  This is useful when
     *  buffering only part of the result set at a time. */
    public UInt64 firstRowIndex()
    {
        return sqlrcur_firstRowIndex(sqlrcurref);
    }

    /** Returns false if part of the result set is still pending on the server
     *  and true if not.  This function can only return false if
     *  setResultSetBufferSize() has been called with a parameter other than
     *  0. */
    public Boolean  endOfResultSet()
    {
        return sqlrcur_endOfResultSet(sqlrcurref)!=0;
    }



    /** If a query failed and generated an error, the error message is available
     *  here.  If the query succeeded then this function returns a NULL. */
    public String errorMessage()
    {
        return sqlrcur_errorMessage(sqlrcurref);
    }

    /** If a query failed and generated an error, the error number is available
     *  here.  If there is no error then this method returns 0. */
    public Int64 errorNumber()
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
    public String getField(UInt64 row, UInt32 col)
    {
        // if we're getting nulls as nulls or we've run off the end of the result set,
        // return a null for a null field
        if (sqlrcur_getFieldByIndex(sqlrcurref, row, col) == IntPtr.Zero)
        {
            return null;
        }

        // if we're getting nulls as empty strings, return an empty string for a null field
        Byte[] field = getFieldAsByteArray(row,col);
        if (field == null)
        {
            return "";
        }

        // if we didn't get a null field, return an actual string
        return System.Text.Encoding.Default.GetString(field);
    }

    /** Returns the specified field as a string. */
    public String getField(UInt64 row, String col)
    {
        // if we're getting nulls as nulls or we've run off the end of the result set,
        // return a null for a null field
        if (sqlrcur_getFieldByName(sqlrcurref, row, col) == IntPtr.Zero)
        {
            return null;
        }

        // if we're getting nulls as empty strings, return an empty string for a null field
        Byte[] field = getFieldAsByteArray(row, col);
        if (field == null)
        {
            return "";
        }

        // if we didn't get a null field, return an actual string
        return System.Text.Encoding.Default.GetString(field);
    }

    /** Returns the specified field as an integer. */
    public Int64 getFieldAsInteger(UInt64 row, UInt32 col)
    {
        return sqlrcur_getFieldAsIntegerByIndex(sqlrcurref, row, col);
    }

    /** Returns the specified field as an integer. */
    public Int64 getFieldAsInteger(UInt64 row, String col)
    {
        return sqlrcur_getFieldAsIntegerByName(sqlrcurref, row, col);
    }

    /** Returns the specified field as an decimal. */
    public Double getFieldAsDouble(UInt64 row, UInt32 col)
    {
        return sqlrcur_getFieldAsDoubleByIndex(sqlrcurref, row, col);
    }

    /** Returns the specified field as an decimal. */
    public Double getFieldAsDouble(UInt64 row, String col)
    {
        return sqlrcur_getFieldAsDoubleByName(sqlrcurref, row, col);
    }

    /** Returns the specified field as a string. */
    public Byte[] getFieldAsByteArray(UInt64 row, UInt32 col)
    {
        Int32 size = (Int32)sqlrcur_getFieldLengthByIndex(sqlrcurref, row, col);
        if (size == 0)
        {
            return null;
        }
        Byte[] retval = new Byte[size];
        Marshal.Copy(sqlrcur_getFieldByIndex(sqlrcurref, row, col), retval, 0, size);
        return retval;
    }

    /** Returns the specified field as a string. */
    public Byte[] getFieldAsByteArray(UInt64 row, String col)
    {
        Int32 size = (Int32)sqlrcur_getFieldLengthByName(sqlrcurref, row, col);
        if (size == 0)
        {
            return null;
        }
        Byte[] retval = new Byte[size];
        Marshal.Copy(sqlrcur_getFieldByName(sqlrcurref, row, col), retval, 0, size);
        return retval;
    }



    /** Returns the length of the specified row and column. */
    public UInt32 getFieldLength(UInt64 row, UInt32 col)
    {
        return sqlrcur_getFieldLengthByIndex(sqlrcurref, row, col);
    }

    /** Returns the length of the specified row and column. */
    public UInt32 getFieldLength(UInt64 row, String col)
    {
        return sqlrcur_getFieldLengthByName(sqlrcurref, row, col);
    }



    /** Returns the name of the specified column. */
    public String getColumnName(UInt32 col)
    {
        return sqlrcur_getColumnName(sqlrcurref, col);
    }

    /** Returns the type of the specified column. */
    public String getColumnType(UInt32 col)
    {
        return sqlrcur_getColumnTypeByIndex(sqlrcurref, col);
    }

    /** Returns the type of the specified column. */
    public String getColumnType(String col)
    {
        return sqlrcur_getColumnTypeByName(sqlrcurref, col);
    }

    /** Returns the length of the specified column. */
    public UInt32 getColumnLength(UInt32 col)
    {
        return sqlrcur_getColumnLengthByIndex(sqlrcurref, col);
    }

    /** Returns the length of the specified column. */
    public UInt32 getColumnLength(String col)
    {
        return sqlrcur_getColumnLengthByName(sqlrcurref, col);
    }

    /** Returns the precision of the specified column.  Precision is the total
     *  number of digits in a number.  eg: 123.45 has a precision of 5.  For
     *  non-numeric types, it's the number of characters in the string. */
    public UInt32 getColumnPrecision(UInt32 col)
    {
        return sqlrcur_getColumnPrecisionByIndex(sqlrcurref, col);
    }

    /** Returns the precision of the specified column.  Precision is the total
     *  number of digits in a number.  eg: 123.45 has a precision of 5.  For
     *  non-numeric types, it's the number of characters in the string. */
    public UInt32 getColumnPrecision(String col)
    {
        return sqlrcur_getColumnPrecisionByName(sqlrcurref, col);
    }

    /** Returns the scale of the specified column.  Scale is the total number of
     *  digits to the right of the decimal poInt32 in a number.  eg: 123.45 has a
     *  scale of 2. */
    public UInt32 getColumnScale(UInt32 col)
    {
        return sqlrcur_getColumnScaleByIndex(sqlrcurref, col);
    }

    /** Returns the scale of the specified column.  Scale is the total number of
     *  digits to the right of the decimal poInt32 in a number.  eg: 123.45 has a 
     *  scale of 2. */
    public UInt32 getColumnScale(String col)
    {
        return sqlrcur_getColumnScaleByName(sqlrcurref, col);
    }

    /** Returns true if the specified column can contain
     *  nulls and false otherwise. */
    public Boolean  getColumnIsNullable(UInt32 col)
    {
        return sqlrcur_getColumnIsNullableByIndex(sqlrcurref, col)!=0;
    }

    /** Returns true if the specified column can contain
     *  nulls and false otherwise. */
    public Boolean  getColumnIsNullable(String col)
    {
        return sqlrcur_getColumnIsNullableByName(sqlrcurref, col)!=0;
    }

    /** Returns true if the specified column is a
     *  primary key and false otherwise. */
    public Boolean  getColumnIsPrimaryKey(UInt32 col)
    {
        return sqlrcur_getColumnIsPrimaryKeyByIndex(sqlrcurref, col)!=0;
    }

    /** Returns true if the specified column is a
     *  primary key and false otherwise. */
    public Boolean  getColumnIsPrimaryKey(String col)
    {
        return sqlrcur_getColumnIsPrimaryKeyByName(sqlrcurref, col)!=0;
    }

    /** Returns true if the specified column is unique and false otherwise. */
    public Boolean  getColumnIsUnique(UInt32 col)
    {
        return sqlrcur_getColumnIsUniqueByIndex(sqlrcurref, col)!=0;
    }

    /** Returns true if the specified column is unique and false otherwise. */
    public Boolean  getColumnIsUnique(String col)
    {
        return sqlrcur_getColumnIsUniqueByName(sqlrcurref, col)!=0;
    }

    /** Returns true if the specified column is part of a composite key and
     *  false otherwise. */
    public Boolean  getColumnIsPartOfKey(UInt32 col)
    {
        return sqlrcur_getColumnIsPartOfKeyByIndex(sqlrcurref, col)!=0;
    }

    /** Returns true if the specified column is part of a composite key and
     *  false otherwise. */
    public Boolean  getColumnIsPartOfKey(String col)
    {
        return sqlrcur_getColumnIsPartOfKeyByName(sqlrcurref, col)!=0;
    }

    /** Returns true if the specified column is an unsigned number and false
     *  otherwise. */
    public Boolean  getColumnIsUnsigned(UInt32 col)
    {
        return sqlrcur_getColumnIsUnsignedByIndex(sqlrcurref, col)!=0;
    }

    /** Returns true if the specified column is an unsigned number and false
     *  otherwise. */
    public Boolean  getColumnIsUnsigned(String col)
    {
        return sqlrcur_getColumnIsUnsignedByName(sqlrcurref, col)!=0;
    }

    /** Returns true if the specified column was created
     *  with the zero-fill flag and false otherwise. */
    public Boolean  getColumnIsZeroFilled(UInt32 col)
    {
        return sqlrcur_getColumnIsZeroFilledByIndex(sqlrcurref, col)!=0;
    }

    /** Returns true if the specified column was created
     *  with the zero-fill flag and false otherwise. */
    public Boolean  getColumnIsZeroFilled(String col)
    {
        return sqlrcur_getColumnIsZeroFilledByName(sqlrcurref, col)!=0;
    }

    /** Returns true if the specified column contains binary data and false
     *  otherwise. */
    public Boolean  getColumnIsBinary(UInt32 col)
    {
        return sqlrcur_getColumnIsBinaryByIndex(sqlrcurref, col)!=0;
    }

    /** Returns true if the specified column contains binary data and false
     *  otherwise. */
    public Boolean  getColumnIsBinary(String col)
    {
        return sqlrcur_getColumnIsBinaryByName(sqlrcurref, col)!=0;
    }

    /** Returns true if the specified column auto-increments
     *  and false otherwise. */
    public Boolean  getColumnIsAutoIncrement(UInt32 col)
    {
        return sqlrcur_getColumnIsAutoIncrementByIndex(sqlrcurref, col)!=0;
    }

    /** Returns true if the specified column auto-increments
     *  and false otherwise. */
    public Boolean  getColumnIsAutoIncrement(String col)
    {
        return sqlrcur_getColumnIsAutoIncrementByName(sqlrcurref, col)!=0;
    }

    /** Returns the length of the longest field in the specified column. */
    public UInt32 getLongest(UInt32 col)
    {
        return sqlrcur_getLongestByIndex(sqlrcurref, col);
    }

    /** Returns the length of the longest field in the specified column. */
    public UInt32 getLongest(String col)
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
    public UInt16 getResultSetId()
    {
        return sqlrcur_getResultSetId(sqlrcurref);
    }

    /** Resumes a result set previously left open using suspendSession().
     *  Returns true on success and false on failure. */
    public Boolean  resumeResultSet(UInt16 id)
    {
        return sqlrcur_resumeResultSet(sqlrcurref, id)!=0;
    }

    /** Resumes a result set previously left open using suspendSession() and
     *  continues caching the result set to "filename".  Returns true on success
     *  and false on failure. */
    public Boolean  resumeCachedResultSet(UInt16 id, String filename)
    {
        return sqlrcur_resumeCachedResultSet(sqlrcurref, id, filename)!=0;
    }

    private IntPtr sqlrcurref;

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern IntPtr sqlrcur_alloc_copyrefs(IntPtr sqlrconref, Int32 copyrefs);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern void sqlrcur_free(IntPtr sqlrcurref);


    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern void sqlrcur_setResultSetBufferSize(IntPtr sqlrcurref, UInt64 rows);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern UInt64 sqlrcur_getResultSetBufferSize(IntPtr sqlrcurref);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern void sqlrcur_dontGetColumnInfo(IntPtr sqlrcurref);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern void sqlrcur_getColumnInfo(IntPtr sqlrcurref);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern void sqlrcur_mixedCaseColumnNames(IntPtr sqlrcurref);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern void sqlrcur_upperCaseColumnNames(IntPtr sqlrcurref);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern void sqlrcur_lowerCaseColumnNames(IntPtr sqlrcurref);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern void sqlrcur_cacheToFile(IntPtr sqlrcurref, String filename);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern void sqlrcur_setCacheTtl(IntPtr sqlrcurref, UInt32 ttl);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern String sqlrcur_getCacheFileName(IntPtr sqlrcurref);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern void sqlrcur_cacheOff(IntPtr sqlrcurref);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern Int32 sqlrcur_getDatabaseList(IntPtr sqlrcurref, String wild);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern Int32 sqlrcur_getTableList(IntPtr sqlrcurref, String wild);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern Int32 sqlrcur_getColumnList(IntPtr sqlrcurref, String table, String wild);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern Int32 sqlrcur_sendQuery(IntPtr sqlrcurref, String query);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern Int32 sqlrcur_sendQueryWithLength(IntPtr sqlrcurref, String query, UInt32 length);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern Int32 sqlrcur_sendFileQuery(IntPtr sqlrcurref, String path, String filename);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern void sqlrcur_prepareQuery(IntPtr sqlrcurref, String query);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern void sqlrcur_prepareQueryWithLength(IntPtr sqlrcurref, String query, UInt32 length);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern void sqlrcur_prepareFileQuery(IntPtr sqlrcurref, String path, String filename);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern void sqlrcur_subString(IntPtr sqlrcurref, String variable, String val);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern void sqlrcur_subLong(IntPtr sqlrcurref, String variable, Int64 val);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern void sqlrcur_subDouble(IntPtr sqlrcurref, String variable, Double val, UInt32 precision, UInt32 scale);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern void sqlrcur_inputBindString(IntPtr sqlrcurref, String variable, String val);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern void sqlrcur_inputBindStringWithLength(IntPtr sqlrcurref, String variable, String val, UInt32 vallength);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern void sqlrcur_inputBindLong(IntPtr sqlrcurref, String variable, Int64 val);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern void sqlrcur_inputBindDouble(IntPtr sqlrcurref, String variable, Double val, UInt32 precision, UInt32 scale);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern void sqlrcur_inputBindDate(IntPtr sqlrcurref, String variable, Int16 year, Int16 month, Int16 day, Int16 hour, Int16 minute, Int16 second, String tz);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern void sqlrcur_inputBindBlob(IntPtr sqlrcurref, String variable, Byte[] val, UInt32 size);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern void sqlrcur_inputBindClob(IntPtr sqlrcurref, String variable, String val, UInt32 size);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern void sqlrcur_defineOutputBindString(IntPtr sqlrcurref, String variable, UInt32 length);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern void sqlrcur_defineOutputBindInteger(IntPtr sqlrcurref, String variable);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern void sqlrcur_defineOutputBindDouble(IntPtr sqlrcurref, String variable);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern void sqlrcur_defineOutputBindDate(IntPtr sqlrcurref, String variable);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern void sqlrcur_defineOutputBindBlob(IntPtr sqlrcurref, String variable);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern void sqlrcur_defineOutputBindClob(IntPtr sqlrcurref, String variable);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern void sqlrcur_defineOutputBindCursor(IntPtr sqlrcurref, String variable);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern void sqlrcur_clearBinds(IntPtr sqlrcurref);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern UInt16 sqlrcur_countBindVariables(IntPtr sqlrcurref);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern void sqlrcur_validateBinds(IntPtr sqlrcurref);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern Int32 sqlrcur_validBind(IntPtr sqlrcurref, String variable);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern Int32 sqlrcur_executeQuery(IntPtr sqlrcurref);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern Int32 sqlrcur_fetchFromBindCursor(IntPtr sqlrcurref);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern String sqlrcur_getOutputBindString(IntPtr sqlrcurref, String variable);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern Int64 sqlrcur_getOutputBindInteger(IntPtr sqlrcurref, String variable);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern Double sqlrcur_getOutputBindDouble(IntPtr sqlrcurref, String variable);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern Int32 sqlrcur_getOutputBindDate(IntPtr sqlrcurref, String variable, ref Int16 year, ref Int16 month, ref Int16 day, ref Int16 hour, ref Int16 minute, ref Int16 second, ref String tz);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern IntPtr sqlrcur_getOutputBindBlob(IntPtr sqlrcurref, String variable);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern String sqlrcur_getOutputBindClob(IntPtr sqlrcurref, String variable);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern UInt32 sqlrcur_getOutputBindLength(IntPtr sqlrcurref, String variable);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern IntPtr sqlrcur_getOutputBindCursor_copyrefs(IntPtr sqlrcurref, String variable, Int32 copyrefs);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern Int32 sqlrcur_openCachedResultSet(IntPtr sqlrcurref, String filename);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern UInt32 sqlrcur_colCount(IntPtr sqlrcurref);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern UInt64 sqlrcur_rowCount(IntPtr sqlrcurref);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern UInt64 sqlrcur_totalRows(IntPtr sqlrcurref);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern UInt64 sqlrcur_affectedRows(IntPtr sqlrcurref);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern UInt64 sqlrcur_firstRowIndex(IntPtr sqlrcurref);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern Int32 sqlrcur_endOfResultSet(IntPtr sqlrcurref);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern String sqlrcur_errorMessage(IntPtr sqlrcurref);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern Int64 sqlrcur_errorNumber(IntPtr sqlrcurref);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern void sqlrcur_getNullsAsEmptyStrings(IntPtr sqlrcurref);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern void sqlrcur_getNullsAsNulls(IntPtr sqlrcurref);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern IntPtr sqlrcur_getFieldByIndex(IntPtr sqlrcurref, UInt64 row, UInt32 col);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern IntPtr sqlrcur_getFieldByName(IntPtr sqlrcurref, UInt64 row, String col);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern Int64 sqlrcur_getFieldAsIntegerByIndex(IntPtr sqlrcurref, UInt64 row, UInt32 col);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern Int64 sqlrcur_getFieldAsIntegerByName(IntPtr sqlrcurref, UInt64 row, String col);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern Double sqlrcur_getFieldAsDoubleByIndex(IntPtr sqlrcurref, UInt64 row, UInt32 col);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern Double sqlrcur_getFieldAsDoubleByName(IntPtr sqlrcurref, UInt64 row, String col);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern UInt32 sqlrcur_getFieldLengthByIndex(IntPtr sqlrcurref, UInt64 row, UInt32 col);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern UInt32 sqlrcur_getFieldLengthByName(IntPtr sqlrcurref, UInt64 row, String col);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern String sqlrcur_getColumnName(IntPtr sqlrcurref, UInt32 col);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern String sqlrcur_getColumnTypeByIndex(IntPtr sqlrcurref, UInt32 col);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern String sqlrcur_getColumnTypeByName(IntPtr sqlrcurref, String col);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern UInt32 sqlrcur_getColumnLengthByIndex(IntPtr sqlrcurref, UInt32 col);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern UInt32 sqlrcur_getColumnLengthByName(IntPtr sqlrcurref, String col);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern UInt32 sqlrcur_getColumnPrecisionByIndex(IntPtr sqlrcurref, UInt32 col);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern UInt32 sqlrcur_getColumnPrecisionByName(IntPtr sqlrcurref, String col);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern UInt32 sqlrcur_getColumnScaleByIndex(IntPtr sqlrcurref, UInt32 col);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern UInt32 sqlrcur_getColumnScaleByName(IntPtr sqlrcurref, String col);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern Int32 sqlrcur_getColumnIsNullableByIndex(IntPtr sqlrcurref, UInt32 col);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern Int32 sqlrcur_getColumnIsNullableByName(IntPtr sqlrcurref, String col);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern Int32 sqlrcur_getColumnIsPrimaryKeyByIndex(IntPtr sqlrcurref, UInt32 col);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern Int32 sqlrcur_getColumnIsPrimaryKeyByName(IntPtr sqlrcurref, String col);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern Int32 sqlrcur_getColumnIsUniqueByIndex(IntPtr sqlrcurref, UInt32 col);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern Int32 sqlrcur_getColumnIsUniqueByName(IntPtr sqlrcurref, String col);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern Int32 sqlrcur_getColumnIsPartOfKeyByIndex(IntPtr sqlrcurref, UInt32 col);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern Int32 sqlrcur_getColumnIsPartOfKeyByName(IntPtr sqlrcurref, String col);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern Int32 sqlrcur_getColumnIsUnsignedByIndex(IntPtr sqlrcurref, UInt32 col);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern Int32 sqlrcur_getColumnIsUnsignedByName(IntPtr sqlrcurref, String col);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern Int32 sqlrcur_getColumnIsZeroFilledByIndex(IntPtr sqlrcurref, UInt32 col);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern Int32 sqlrcur_getColumnIsZeroFilledByName(IntPtr sqlrcurref, String col);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern Int32 sqlrcur_getColumnIsBinaryByIndex(IntPtr sqlrcurref, UInt32 col);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern Int32 sqlrcur_getColumnIsBinaryByName(IntPtr sqlrcurref, String col);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern Int32 sqlrcur_getColumnIsAutoIncrementByIndex(IntPtr sqlrcurref, UInt32 col);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern Int32 sqlrcur_getColumnIsAutoIncrementByName(IntPtr sqlrcurref, String col);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern UInt32 sqlrcur_getLongestByIndex(IntPtr sqlrcurref, UInt32 col);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern UInt32 sqlrcur_getLongestByName(IntPtr sqlrcurref, String col);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern void sqlrcur_suspendResultSet(IntPtr sqlrcurref);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern UInt16 sqlrcur_getResultSetId(IntPtr sqlrcurref);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern Int32 sqlrcur_resumeResultSet(IntPtr sqlrcurref, UInt16 id);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern Int32 sqlrcur_resumeCachedResultSet(IntPtr sqlrcurref, UInt16 id, String filename);
}

}
