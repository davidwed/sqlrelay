// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information.
package com.firstworks.sqlrelay;

public class SQLRCursor {

	static {
		System.loadLibrary("SQLRCursor");
	}

	public SQLRCursor(SQLRConnection con) {
		connection=con;
		cursor=alloc(con.connection);
	}
	public native void	delete();


	/** Sets the number of rows of the result set
	 *  to buffer at a time.  0 (the default)
	 *  means buffer the entire result set.  */
	public native void	setResultSetBufferSize(long rows);
	/** Returns the number of result set rows that 
	 *  will be buffered at a time or 0 for the
	 *  entire result set.  */
	public native long	getResultSetBufferSize();


	/** Tells the server not to send any column
	 *  info (names, types, sizes).  If you don't
	 *  need that info, you should call this
	 *  method to improve performance.  */
	public native void	dontGetColumnInfo();
	/** Tells the server to send column info.  */
	public native void	getColumnInfo();


	/** Columns names are returned in the same
	 *  case as they are defined in the database.
	 *  This is the default. */
	public native void	mixedCaseColumnNames();
	/** Columns names are converted to upper case. */
	public native void	upperCaseColumnNames();
	/** Columns names are converted to lower case. */
	public native void	lowerCaseColumnNames();

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
	 *  caching.  */
	public native void	cacheToFile(String filename);
	/** Sets the time-to-live for cached result
	 *  sets. The sqlr-cachemanger will remove each 
	 *  cached result set "ttl" seconds after it's 
	 *  created, provided it's scanning the directory
	 *  containing the cache files.  */
	public native void	setCacheTtl(int ttl);
	/** Returns the name of the file containing the
	 *  cached result set.  */
	public native String	getCacheFileName();
	/** Sets query caching off.  */
	public native void	cacheOff();


	/** Sends a query that returns a list of
	 *  databases/schemas matching "wild".  If wild is empty
	 *  or NULL then a list of all databases/schemas will be
	 *  returned. */
	public native boolean	getDatabaseList(String wild);
	/** Sends a query that returns a list of tables
	 *  matching "wild".  If wild is empty or NULL then
	 *  a list of all tables will be returned. */
	public native boolean	getTableList(String wild);
	/** Sends a query that returns a list of columns
	 *  in the table specified by the "table" parameter
	 *  matching "wild".  If wild is empty or NULL then
	 *  a list of all columns will be returned. */
	public native boolean	getColumnList(String table, String wild);


	/** Sends "query" and gets a result set.  */
	public native boolean	sendQuery(String query);
	/** Sends the query in file "path"/"filename" 
	 *  and gets a result set.  */
	public native boolean	sendQuery(String query, int length);
	/** Sends "query" with length "length" and gets
	*  a result set. This method must be used if
	*  the query contains binary data. */
	public native boolean	sendFileQuery(String path, String filename); 


	/** Prepare to execute "query".  */
	public native void	prepareQuery(String query);
	/** Prepare to execute the contents 
	 *  of "path"/"filename".  Returns 0 if the
	 *  file couldn't be opened.  */
	public native void	prepareQuery(String query, int length);
	/** Prepare to execute "query" with length 
	 * "length".  This method must be used if the
	 * query contains binary data. */
	public native boolean	prepareFileQuery(String path, String filename);

	/** Clear all bind variables.  */
	public native void	clearBinds();

	/** Define a substitution variable.  */
	public native void	substitution(String variable, String value);
	/** Define a substitution variable.  */
	public native void	substitution(String variable, long value);
	/** Define a substitution variable.  */
	public native void	substitution(String variable, double value, 
					int precision, int scale);

	/** Parses the previously prepared query,
	 *  counts the number of bind variables defined
	 *  in it and returns that number. */
	public native short	countBindVariables();

	/** Define an input bind variable.  */
	public native void	inputBind(String variable, String value);
	/** Define an input bind variable.  */
	public native void	inputBind(String variable,
						String value, int length);
	/** Define an input bind variable.  */
	public native void	inputBind(String variable, long value);
	/** Define an input bind variable.
	  * (If you don't have the precision and scale then they may
	  * both be set to 0.  However in that case you may get
	  * unexpected rounding behavior if the server is faking
	  * binds.) */
	public native void	inputBind(String variable, double value, 
					int precision, int scale);
	/** Define an input bind variable.  */
	public native void	inputBindBlob(String variable, byte[] value, 
								long size);
	/** Define an input bind variable.  */
	public native void	inputBindClob(String variable, String value, 
								long size);
	/** Define a string output bind variable.  */
	public native void	defineOutputBindString(String variable, 
							int bufferlength);
	/** Define an integer output bind variable.  */
	public native void	defineOutputBindInteger(String variable);
	/** Define a double precision floating point output bind variable.  */
	public native void	defineOutputBindDouble(String variable);
	/** Define an output bind variable.  */
	public native void	defineOutputBindBlob(String variable);
	/** Define an output bind variable.  */
	public native void	defineOutputBindClob(String variable);
	/** Define an output bind variable.  */
	public native void	defineOutputBindCursor(String variable);

	/** Define an array of substitution variables.  */
	public native void	substitutions(String[] variables, 
							String[] values);

	/** Define an array of substitution variables.  */
	public native void	substitutions(String[] variables, 
							long[] values);

	/** Define an array of substitution variables.  */
	public native void	substitutions(String[] variables, 
					double[] values,
					int[] precisions, int[] scales);

	/** Define an array of input bind variables.  */
	public native void	inputBinds(String[] variables, String[] values);

	/** Define an array of input bind variables.  */
	public native void	inputBinds(String[] variables, long[] values);

	/** Define an array of input bind variables.  */
	public native void	inputBinds(String[] variables, 
					double[] values, 
					int[] precisions, int[] scales);

	/** If you are binding to any variables that 
	 *  might not actually be in your query, call 
	 *  this to ensure that the database won't try 
	 *  to bind them unless they really are in the 
	 *  query.  There is a performance penalty for
	 *  calling this method.  */
	public native void	validateBinds();

	/** Returns true if "variable" was a valid
	 *  bind variable of the query  */
	public native boolean	validBind(String variable);

	/** Execute the query that was previously 
	 *  prepared and bound.  */
	public native boolean	executeQuery();

	/** Fetch from a cursor that was returned as
	 *  an output bind variable.  */
	public native boolean	fetchFromBindCursor();

	/** Get the value stored in a previously
	 *  defined output bind variable.  */
	public native String	getOutputBindString(String variable);
	/** Get the value stored in a previously
	 *  defined output bind variable.  */
	public native byte[]	getOutputBindBlob(String variable);
	/** Get the value stored in a previously
	 *  defined output bind variable.  */
	public native String	getOutputBindClob(String variable);
	/** Get the length of the value stored in a
	 *  previously defined output bind variable.  */
	public native byte[]	getOutputBindAsByteArray(String variable);
	/** Get the value stored in a previously
	 *  defined output bind variable as a long
	 *  integer. */
	public native long	getOutputBindInteger(String variable);
	/** Get the value stored in a previously
	 *  defined output bind variable as a double
	 *  precision floating point number. */
	public native double	getOutputBindDouble(String variable);
	/** Get the length of the value stored in a
	 *  previously defined output bind variable.  */
	public native long	getOutputBindLength(String variable);
	/** Get the cursor associated with a
	 *  previously defined output bind variable.  */
	public SQLRCursor	getOutputBindCursor(String variable) {
		SQLRCursor	bindcur=new SQLRCursor(connection);
		bindcur.cursor=getOutputBindCursorInternal(variable);
		return bindcur;
	}


	/** Opens a cached result set.
	 *  Returns 1 on success and 0 on failure.  */
	public native boolean	openCachedResultSet(String filename);

	/** Returns the number of columns in the current
	 *  result set.  */
	public native int	colCount();
	/** Returns the number of rows in the current 
	 *  result set (if the result set is being
	 *  stepped through, this returns the number
	 *  of rows processed so far).  */
	public native long	rowCount();
	/** Returns the total number of rows that will 
	 *  be returned in the result set.  Not all 
	 *  databases support this call.  Don't use it 
	 *  for applications which are designed to be 
	 *  portable across databases.  -1 is returned
	 *  by databases which don't support this option.  */
	public native long	totalRows();
	/** Returns the number of rows that were 
	 *  updated, inserted or deleted by the query.
	 *  Not all databases support this call.  Don't 
	 *  use it for applications which are designed 
	 *  to be portable across databases.  -1 is 
	 *  returned by databases which don't support 
	 *  this option.  */
	public native long	affectedRows();
	/** Returns the index of the first buffered row.
	 *  This is useful when buffering only part of
	 *  the result set at a time.  */
	public native long	firstRowIndex();
	/** Returns 0 if part of the result set is still
	 *  pending on the server and 1 if not.  This
	 *  method can only return 0 if 
	 *  setResultSetBufferSize() has been called
	 *  with a parameter other than 0.  */
	public native boolean	endOfResultSet();


	/** If a query failed and generated an error, 
	 *  the error message is available here.  If 
	 *  the query succeeded then this method 
	 *  returns NULL.  */
	public native String	errorMessage();

	/** If a query failed and generated an
	 *  error, the error number is available here.
	 *  If there is no error then this method 
	 *  returns 0. */
	public native long	errorNumber();


	/** Tells the connection to return NULL fields
	 *  and output bind variables as empty strings. 
	 *  This is the default.  */
	public native void	getNullsAsEmptyStrings();
	/** Tells the connection to return NULL fields
	 *  and output bind variables as NULL's rather
	 *  than as empty strings.  */
	public native void	getNullsAsNulls();


	/** Returns a pointer to the value of the 
	 *  specified row and column.  */
	public native String	getField(long row, int col);
	/** Returns a pointer to the value of the 
	 *  specified row and column.  */
	public native String	getField(long row, String col);
	/** Returns the specified field as a long integer */
	public native long	getFieldAsInteger(long row, int col);
	/** Returns the specified field as a long integer */
	public native long	getFieldAsInteger(long row, String col);
	/** Returns the specified field as a double floating point number */
	public native double	getFieldAsDouble(long row, int col);
	/** Returns the specified field as a double floating point number */
	public native double	getFieldAsDouble(long row, String col);
	/** Returns a pointer to the value of the 
	 *  specified row and column.  */
	public native byte[]	getFieldAsByteArray(long row, int col);
	/** Returns the length of the 
	 *  specified row and column.  */
	public native byte[]	getFieldAsByteArray(long row, String col);
	/** Returns the length of the 
	 *  specified row and column.  */
	public native long	getFieldLength(long row, int col);
	/** Returns the length of the 
	 *  specified row and column.  */
	public native long	getFieldLength(long row, String col);
	/** Returns a null terminated array of the 
	 *  values of the fields in the specified row.  */
	public native String[]	getRow(long row);
	/** Returns a null terminated array of the 
	 *  lengths of the fields in the specified row.  */
	public native long[]	getRowLengths(long row);
	/** Returns a null terminated array of the 
	 *  column names of the current result set.  */
	public native String[]	getColumnNames();
	/** Returns the name of the specified column.  */
	public native String	getColumnName(int col);
	/** Returns the type of the specified column.  */
	public native String	getColumnType(int col);
	/** Returns the type of the specified column.  */
	public native String	getColumnType(String col);
	/** Returns the precision of the specified
	 *  column.
	 *  Precision is the total number of digits in
	 *  a number.  eg: 123.45 has a precision of 5.
	 *  For non-numeric types, it's the number of
	 *  characters in the string. */
	public native long	getColumnPrecision(int col);
	/** Returns the precision of the specified
	 *  column.
	 *  Precision is the total number of digits in
	 *  a number.  eg: 123.45 has a precision of 5.
	 *  For non-numeric types, it's the number of
	 *  characters in the string. */
	public native long	getColumnPrecision(String col);
	/** Returns the scale of the specified column.
	 *  Scale is the total number of digits to the
	 *  right of the decimal point in a number.
	 *  eg: 123.45 has a scale of 2. */
	public native long	getColumnScale(int col);
	/** Returns the scale of the specified column.
	 *  Scale is the total number of digits to the
	 *  right of the decimal point in a number.
	 *  eg: 123.45 has a scale of 2. */
	public native long	getColumnScale(String col);
	/** Returns true if the specified column can
	 *  contain nulls and false otherwise. */
	public native boolean	getColumnIsNullable(int col);
	/** Returns true if the specified column can
	 *  contain nulls and false otherwise. */
	public native boolean	getColumnIsNullable(String col);
	/** Returns true if the specified column is a
	 * primary key and false otherwise. */
	public native boolean	getColumnIsPrimaryKey(int col);
	/** Returns true if the specified column is a
	 * primary key and false otherwise. */
	public native boolean	getColumnIsPrimaryKey(String col);
	/** Returns true if the specified column is
	 * unique and false otherwise. */
	public native boolean	getColumnIsUnique(int col);
	/** Returns true if the specified column is
	 * unique and false otherwise. */
	public native boolean	getColumnIsUnique(String col);
	/** Returns true if the specified column is
	 * part of a composite key and false otherwise. */
	public native boolean	getColumnIsPartOfKey(int col);
	/** Returns true if the specified column is
	 * part of a composite key and false otherwise. */
	public native boolean	getColumnIsPartOfKey(String col);
	/** Returns true if the specified column is
	 * an unsigned number and false otherwise. */
	public native boolean	getColumnIsUnsigned(int col);
	/** Returns true if the specified column is
	 * an unsigned number and false otherwise. */
	public native boolean	getColumnIsUnsigned(String col);
	/** Returns true if the specified column was
	 * created with the zero-fill flag and false
	 * otherwise. */
	public native boolean	getColumnIsZeroFilled(int col);
	/** Returns true if the specified column was
	 * created with the zero-fill flag and false
	 * otherwise. */
	public native boolean	getColumnIsZeroFilled(String col);
	/** Returns true if the specified column
	 * contains binary data and false
	 * otherwise. */
	public native boolean	getColumnIsBinary(int col);
	/** Returns true if the specified column
	 * contains binary data and false
	 * otherwise. */
	public native boolean	getColumnIsBinary(String col);
	/** Returns true if the specified column
	 * auto-increments and false otherwise. */
	public native boolean	getColumnIsAutoIncrement(int col);
	/** Returns true if the specified column
	 * auto-increments and false otherwise. */
	public native boolean	getColumnIsAutoIncrement(String col);
	/** Returns the length of the specified column.  */
	public native int	getColumnLength(int col);
	/** Returns the length of the specified column.  */
	public native int	getColumnLength(String col);
	/** Returns the length of the longest field
	 *  in the specified column.  */
	public native int	getLongest(int col);
	/** Returns the length of the longest field
	 *  in the specified column.  */
	public native int	getLongest(String col);


	/** Tells the server to leave this result
	 *  set open when the connection calls 
	 *  suspendSession() so that another connection 
	 *  can connect to it using resumeResultSet() 
	 *  after it calls resumeSession(). */
	public native void	suspendResultSet();
	/** Returns the internal ID of this result set.
	 *  This parameter may be passed to another 
	 *  cursor for use in the resumeResultSet() 
	 *  method.
	 *  Note: the value returned by this method is only
	 *  valid after a call to suspendResultSet(). */
	public native short	getResultSetId();
	/** Resumes a result set previously left open 
	 *  using suspendSession().
	 *  Returns 1 on success and 0 on failure.  */
	public native boolean	resumeResultSet(short id);
	/** Resumes a result set previously left open
	 *  using suspendSession() and continues caching
	 *  the result set to "filename".
	 *  Returns 1 on success and 0 on failure.  */
	public native boolean	resumeCachedResultSet(short id,
							String filename);
	/** Closes the current result set, if one is open.  Data
 	 *  that has been fetched already is still available but
 	 *  no more data may be fetched.  Server side resources
 	 *  for the result set are freed as well. */
	public native void	closeResultSet();


	/** cursor and connection are used internally, they're just
	 *  public to make the JNI wrapper work faster.  */
	public long		cursor;
	public SQLRConnection	connection;
	private native long	alloc(long con);
	private native long	getOutputBindCursorInternal(String variable);
}
