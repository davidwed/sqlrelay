// Copyright (c) 2001  David Muse
// See the file COPYING for more information.
package com.firstworks.sqlrelay;

public class SQLRCursor {

	static {
		System.loadLibrary("SQLRCursor");
	}

	public SQLRCursor(SQLRConnection con) {
		cursor=sqlrcur_alloc(con.connection);
	}
	public native void	delete();


	/** Sets the number of rows of the result set
	 *  to buffer at a time.  0 (the default)
	 *  means buffer the entire result set.  */
	public native void	setResultSetBufferSize(int rows);
	/** Returns the number of result set rows that 
	 *  will be buffered at a time or 0 for the
	 *  entire result set.  */
	public native int	getResultSetBufferSize();


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


	/** Sends "query" and gets a result set.  */
	public native int	sendQuery(String query);
	/** Sends the query in file "path"/"filename" 
	 *  and gets a result set.  */
	public native int	sendQuery(String query, int length);
	/** Sends "query" with length "length" and gets
	*  a result set. This method must be used if
	*  the query contains binary data. */
	public native int	sendFileQuery(String path, String filename); 


	/** Prepare to execute "query".  */
	public native void	prepareQuery(String query);
	/** Prepare to execute the contents 
	 *  of "path"/"filename".  Returns 0 if the
	 *  file couldn't be opened.  */
	public native void	prepareQuery(String query, int length);
	/** Prepare to execute "query" with length 
	 * "length".  This method must be used if the
	 * query contains binary data. */
	public native int	prepareFileQuery(String path, String filename);

	/** Clear all bind variables.  */
	public native void	clearBinds();

	/** Define a substitution variable.  */
	public native void	substitution(String variable, String value);
	/** Define a substitution variable.  */
	public native void	substitution(String variable, long value);
	/** Define a substitution variable.  */
	public native void	substitution(String variable, double value, 
					int precision, int scale);
	/** Define an input bind variable.  */
	public native void	inputBind(String variable, String value);
	/** Define an input bind variable.  */
	public native void	inputBind(String variable, long value);
	/** Define an input bind variable.  */
	public native void	inputBind(String variable, double value, 
					int precision, int scale);
	/** Define an output bind variable.  */
	public native void	defineOutputBind(String variable, 
							int bufferlength);

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

	/** Execute the query that was previously 
	 *  prepared and bound.  */
	public native int	executeQuery();

	/** Get the value stored in a previously
	 *  defined output bind variable.  */
	public native String	getOutputBind(String variable);


	/** Opens a cached result set.
	 *  Returns 1 on success and 0 on failure.  */
	public native int	openCachedResultSet(String filename);

	/** Returns the number of columns in the current
	 *  result set.  */
	public native int	colCount();
	/** Returns the number of rows in the current 
	 *  result set (if the result set is being
	 *  stepped through, this returns the number
	 *  of rows processed so far).  */
	public native int	rowCount();
	/** Returns the total number of rows that will 
	 *  be returned in the result set.  Not all 
	 *  databases support this call.  Don't use it 
	 *  for applications which are designed to be 
	 *  portable across databases.  -1 is returned
	 *  by databases which don't support this option.  */
	public native int	totalRows();
	/** Returns the number of rows that were 
	 *  updated, inserted or deleted by the query.
	 *  Not all databases support this call.  Don't 
	 *  use it for applications which are designed 
	 *  to be portable across databases.  -1 is 
	 *  returned by databases which don't support 
	 *  this option.  */
	public native int	affectedRows();
	/** Returns the index of the first buffered row.
	 *  This is useful when buffering only part of
	 *  the result set at a time.  */
	public native int	firstRowIndex();
	/** Returns 0 if part of the result set is still
	 *  pending on the server and 1 if not.  This
	 *  method can only return 0 if 
	 *  setResultSetBufferSize() has been called
	 *  with a parameter other than 0.  */
	public native int	endOfResultSet();


	/** If a query failed and generated an error, 
	 *  the error message is available here.  If 
	 *  the query succeeded then this method 
	 *  returns a NULL.  */
	public native String	errorMessage();


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
	public native String	getField(int row, int col);
	/** Returns a pointer to the value of the 
	 *  specified row and column.  */
	public native String	getField(int row, String col);
	/** Returns the length of the 
	 *  specified row and column.  */
	public native long	getFieldLength(int row, int col);
	/** Returns the length of the 
	 *  specified row and column.  */
	public native long	getFieldLength(int row, String col);
	/** Returns a null terminated array of the 
	 *  values of the fields in the specified row.  */
	public native String[]	getRow(int row);
	/** Returns a null terminated array of the 
	 *  lengths of the fields in the specified row.  */
	public native long[]	getRowLengths(int row);
	/** Returns a null terminated array of the 
	 *  column names of the current result set.  */
	public native String[]	getColumnNames();
	/** Returns the name of the specified column.  */
	public native String	getColumnName(int col);
	/** Returns the type of the specified column.  */
	public native String	getColumnType(int col);
	/** Returns the type of the specified column.  */
	public native String	getColumnType(String col);
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


	/** Returns the internal ID of this result set.
	 *  This parameter may be passed to another 
	 *  cursor for use in the resumeResultSet() 
	 *  method.  */
	public native int	getResultSetId();
	/** Tells the server to leave this result
	 *  set open when the connection calls 
	 *  suspendSession() so that another connection 
	 *  can connect to it using resumeResultSet() 
	 *  after it calls resumeSession().  */
	public native void	suspendResultSet();
	/** Resumes a result set previously left open 
	 *  using suspendSession().
	 *  Returns 1 on success and 0 on failure.  */
	public native int	resumeResultSet(int id);
	/** Resumes a result set previously left open
	 *  using suspendSession() and continues caching
	 *  the result set to "filename".
	 *  Returns 1 on success and 0 on failure.  */
	public native int	resumeCachedResultSet(int id, String filename);


	/** cursor is used internally, it's just
	 *  public to make the JNI wrapper work faster.  */
	public int	cursor;
	private native int	sqlrcur_alloc(int con);
}
