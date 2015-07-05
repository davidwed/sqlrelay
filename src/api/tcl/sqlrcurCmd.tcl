# Copyright (c) 2003 Takeshi Taguchi
# See the file COPYING for more information

# Creates a cursor to run queries and fetch result
# sets using connecton "sqlrc".
proc sqlrcurCmd {sqlrc} 

# Destroys the cursor and cleans up all associated
# result set data.
proc sqlrcurDelete {} 



# Sets the number of rows of the result set
# to buffer at a time.  0  {the default}
# means buffer the entire result set.
proc setResultSetBufferSize {rows} 

# Returns the number of result set rows that 
# will be buffered at a time or 0 for the
# entire result set.
proc getResultSetBufferSize {} 



# Tells the server not to send any column
# info  {names types sizes}.  If you don't
# need that info you should call this
# method to improve performance.
proc dontGetColumnInfo {} 

# Tells the server to send column info.
proc getColumnInfo {} 


# Columns names are returned in the same
# case as they are defined in the database.
# This is the default.
proc mixedCaseColumnNames {} 

# Columns names are converted to upper case.
proc upperCaseColumnNames {} 

# Columns names are converted to lower case.
proc lowerCaseColumnNames {} 



# Sets query caching on.  Future queries
# will be cached to the file "filename".
# 
# A default time-to-live of 10 minutes is
# also set.
# 
# Note that once cacheToFile is called
# the result sets of all future queries will
# be cached to that file until another call 
# to cacheToFile changes which file to
# cache to or a call to cacheOff turns off
# caching.
proc cacheToFile {filename} 

# Sets the time-to-live for cached result
# sets. The sqlr-cachemanger will remove each 
# cached result set "ttl" seconds after it's 
# created provided it's scanning the directory
# containing the cache files.
proc setCacheTtl {ttl} 

# Returns the name of the file containing the
# cached result set.
proc getCacheFileName {} 

# Sets query caching off.
proc cacheOff {} 



# Sends a query that returns a list of
# databases/schemas matching "wild".  If wild is empty
# or NULL then a list of all databases/schemas will be
# returned.
proc getDatabaseList {wild} 

# Sends a query that returns a list of tables
# matching "wild".  If wild is empty or NULL then
# a list of all tables will be returned.
proc getTableList {wild} 

# Sends a query that returns a list of columns
# in the table specified by the "table" parameter
# matching "wild".  If wild is empty or NULL then
# a list of all columns will be returned.
proc getColumnList {table wild} 



# Sends "query" directly and gets a result set.
proc sendQuery {query} 

# Sends "query" with length "length" directly
# and gets a result set. This method must be used
# if the query contains binary data.
proc sendQueryWithLength {query length} 

# Sends the query in file "path"/"filename" directly
# and gets a result set.
proc sendFileQuery {path filename}  



# Prepare to execute "query".
proc prepareQuery {query} 

# Prepare to execute "query" with length 
# "length".  This method must be used if the
# query contains binary data.
proc prepareQuery {query length} 

# Prepare to execute the contents 
# of "path"/"filename".  Returns false if the
# // file couldn't be opened.
proc prepareFileQuery {path filename} 



# Defines a substitution variable.
proc substitution {variable value precision scale} 

# Defines an array of substitution variables.
proc substitutions {variables values precisions scales} 



# Defines a input bind variable.
# The value may be a string, integer or decimal.  If the value is a decimal,
# then precision and scale may also be specified.  If you don't have the
# precision and scale then set them both to 0.  However in that case you may
# get unexpected rounding behavior if the server is faking binds.
proc inputBind {variable value precision scale} 

# Defines a binary lob input bind variable.
proc inputBindBlob {variable value size} 

# Defines a character lob input bind variable.
proc inputBindClob {variable value size} 

# Defines an array of input bind variables.
proc inputBinds {variables values precisions scales} 



# Defines an output bind variable.
# "bufferlength" bytes will be reserved
# to store the value.
proc defineOutputBindString {variable bufferlength} 

# Defines an integer output bind variable.
proc defineOutputBindInteger {variable} 

# Defines a decimal output bind variable.
proc defineOutputBindDouble {variable} 

# Defines a binary lob output bind variable.
proc defineOutputBindBlob {variable} 

# Defines a character lob output bind variable.
proc defineOutputBindClob {variable} 

# Defines a cursor output bind variable.
proc defineOutputBindCursor {variable} 



# Clears all bind variables.
proc clearBinds {} 

# Parses the previously prepared query
# counts the number of bind variables defined
# in it and returns that number.
proc countBindVariables {} 

# If you are binding to any variables that 
# might not actually be in your query call 
# this to ensure that the database won't try 
# to bind them unless they really are in the 
# query.  There is a performance penalty for
# calling this method.
proc validateBinds {} 

# Returns true if "variable" was a valid
# bind variable of the query.
proc validBind {variable} 



# Execute the query that was previously 
# prepared and bound.
proc executeQuery {} 

# Fetch from a cursor that was returned as
# an output bind variable.
proc fetchFromBindCursor {} 



# Get the value stored in a previously
# defined string output bind variable.
proc getOutputBindString {variable} 

# Get the value stored in a previously
# defined integer output bind variable.
proc getOutputBindInteger {variable} 

# Get the value stored in a previously
# defined decimal output bind variable.
proc getOutputBindDouble {variable} 

# Get the value stored in a previously
# defined binary lob output bind variable.
proc getOutputBindBlob {variable} 

# Get the value stored in a previously
# defined character lob output bind variable.
proc getOutputBindClob {variable} 

# Get the length of the value stored in a
# previously defined output bind variable.
proc getOutputBindLength {variable} 

# Get the cursor associated with a previously
# defined output bind variable.
proc getOutputBindCursor {variable} 



# Opens a cached result set.
# Returns true on success and false on failure.
proc openCachedResultSet {filename} 



# Returns the number of columns in the current
# result set.
proc colCount {} 

# Returns the number of rows in the current 
# result set  {if the result set is being
# stepped through this returns the number
# of rows processed so far}.
proc rowCount {} 

# Returns the total number of rows that will 
# be returned in the result set.  Not all 
# databases support this call.  Don't use it 
# for applications which are designed to be 
# portable across databases.  0 is returned
# by databases which don't support this option.
proc totalRows {} 

# Returns the number of rows that were 
# updated inserted or deleted by the query.
# Not all databases support this call.  Don't 
# use it for applications which are designed 
# to be portable across databases.  0 is 
# returned by databases which don't support 
# this option.
proc affectedRows {} 

# Returns the index of the first buffered row.
# This is useful when buffering only part of
# the result set at a time.
proc firstRowIndex {} 

# Returns false if part of the result set is
# still pending on the server and true if not.
# This method can only return false if 
# setResultSetBufferSize {} has been called
# with a parameter other than 0.
proc endOfResultSet {} 



# If a query failed and generated an error 
# the error message is available here.  If 
# the query succeeded then this method 
# returns NULL.
proc errorMessage {} 



# If a query failed and generated an
# error, the error number is available here.
# If there is no error then this method 
# returns 0.
proc errorNumber {}



# Returns the specified field as a string.
proc getFieldByIndex {row  col} 

# Returns the specified field as a string
proc getFieldByName {row col} 

# Returns the specified field as an integer.
proc getFieldAsIntegerByIndex {row  col} 

# Returns the specified field as an integer.
proc getFieldAsIntegerByName {row col} 

# Returns the specified field as a decimal.
proc getFieldAsDoubleByIndex {row  col} 

# Returns the specified field as a decimal.
proc getFieldAsDoubleByName {row col} 



# Returns the length of the specified field.
proc getFieldLengthByIndex {row  col} 

# Returns the length of the specified field.
proc getFieldLengthByName {row col} 



# Returns a null terminated array of the 
# values of the fields in the specified row.
proc getRow {row} 

# Returns a null terminated array of the 
# lengths of the fields in the specified row.
proc getRowLengths {row} 

# Returns a null terminated array of the 
# column names of the current result set.
proc getColumnNames {} 

# Returns the name of the specified column.
proc getColumnName {col} 

# Returns the name of the specified column.
proc getColumnNameByIndex {col} 

# Returns the type of the specified column.
proc getColumnTypeByName {col} 

# Returns the type of the specified column.
proc getColumnTypeByIndex {col} 

# Returns the type of the specified column.
proc getColumnTypeByName {col} 

# Returns the number of bytes required on
# the server to store the data for the specified column
proc getColumnLengthByIndex {col} 

# Returns the number of bytes required on
# the server to store the data for the specified column
proc getColumnLengthByName {col} 

# Returns the precision of the specified
# column.
# Precision is the total number of digits in
# a number.  eg: 123.45 has a precision of 5.
# For non-numeric types it's the number of
# characters in the string.
proc getColumnPrecisionByIndex {col} 

# Returns the precision of the specified
# column.
# Precision is the total number of digits in
# a number.  eg: 123.45 has a precision of 5.
# For non-numeric types it's the number of
# characters in the string.
proc getColumnPrecisionByName {col} 

# Returns the scale of the specified column.
# Scale is the total number of digits to the
# right of the decimal point in a number.
# eg: 123.45 has a scale of 2.
proc getColumnScaleByIndex {col} 

# Returns the scale of the specified column.
# Scale is the total number of digits to the
# right of the decimal point in a number.
# eg: 123.45 has a scale of 2.
proc getColumnScaleByName {col} 

# Returns true if the specified column can
# contain nulls and false otherwise.
proc getColumnIsNullableByIndex {col} 

# Returns true if the specified column can
# contain nulls and false otherwise.
proc getColumnIsNullableByName {col} 

# Returns true if the specified column is a
# primary key and false otherwise.
proc getColumnIsPrimaryKeyByIndex {col} 

# Returns true if the specified column is a
# primary key and false otherwise.
proc getColumnIsPrimaryKeyByName {col} 

# Returns true if the specified column is
 	# unique and false otherwise.
proc getColumnIsUniqueByIndex {col} 

# Returns true if the specified column is
 	# unique and false otherwise.
proc getColumnIsUniqueByName {col} 

# Returns true if the specified column is
# part of a composite key and false otherwise.
proc getColumnIsPartOfKeyByIndex {col} 

# Returns true if the specified column is
# part of a composite key and false otherwise.
proc getColumnIsPartOfKeyByName {col} 

# Returns true if the specified column is
# an unsigned number and false otherwise.
proc getColumnIsUnsignedByIndex {col} 

# Returns true if the specified column is
# an unsigned number and false otherwise.
proc getColumnIsUnsignedByName {col} 

# Returns true if the specified column was
# created with the zero-fill flag and false
# otherwise.
proc getColumnIsZeroFilledByIndex {col} 

# Returns true if the specified column was
# created with the zero-fill flag and false
# otherwise.
proc getColumnIsZeroFilledByName {col} 

# Returns true if the specified column
# contains binary data and false
# otherwise.
proc getColumnIsBinaryByIndex {col} 

# Returns true if the specified column
# contains binary data and false
# otherwise.
proc getColumnIsBinaryByName {col} 

# Returns true if the specified column
# auto-increments and false otherwise.
proc getColumnIsAutoIncrementByIndex {col} 

# Returns true if the specified column
# auto-increments and false otherwise.
proc getColumnIsAutoIncrementByName {col} 

# Returns the length of the longest field
# in the specified column.
proc getLongestByIndex {col} 

# Returns the length of the longest field
# in the specified column.
proc getLongestByName {col} 



# Tells the server to leave this result
# set open when the connection calls 
# suspendSession so that another connection 
# can connect to it using resumeResultSet 
# after it calls resumeSession.
proc suspendResultSet {} 

# Returns the internal ID of this result set.
# This parameter may be passed to another 
# cursor for use in the resumeResultSet 
# method.
# Note: The value this method returns is only
# valid after a call to suspendResultSet.
proc getResultSetId {} 

# Resumes a result set previously left open 
# using suspendSession.
# Returns true on success and false on failure.
proc resumeResultSet {id} 

# Resumes a result set previously left open
# using suspendSession and continues caching
# the result set to "filename".
# Returns true on success and false on failure.
proc resumeCachedResultSet {id filename} 

# Closes the current result set, if one is open.  Data
# that has been fetched already is still available but
# no more data may be fetched.  Server side resources
# for the result set are freed as well.
proc closeResultSet {}
