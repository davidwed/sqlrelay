# Copyright (c) 1999-2018 David Muse
# See the file COPYING for more information

package SQLRelay::Cursor;

require DynaLoader;
@ISA = 'DynaLoader';

bootstrap SQLRelay::Cursor;

sub getRowLengths {
	($this,$row)=@_;
	if (validRow($this,$row)==0) {
		return ();
	}
	my @lengths;
	for (my $col=0; $col<colCount($this); $col++) {
		$lengths[$col]=getFieldLength($this,$row,$col);
	}
	return @lengths;
}

sub getRowLengthsHash {
	($this,$row)=@_;
	if (validRow($this,$row)==0) {
		return ();
	}
	my %hash;
	for (my $col=0; $col<colCount($this); $col++) {
		$hash{getColumnName($this,$col)}=
			getFieldLength($this,$row,$col);
	}
	return %hash;
}

sub getRow {
	($this,$row)=@_;
	if (validRow($this,$row)==0) {
		return ();
	}
	my @fields;
	for (my $col=0; $col<colCount($this); $col++) {
		$fields[$col]=getField($this,$row,$col);
	}
	return @fields;
}

sub getRowHash {
        ($this,$row)=@_;
	if (validRow($this,$row)==0) {
		return ();
	}
        my %hash;
        for (my $col=0; $col<colCount($this); $col++) {
		$hash{getColumnName($this,$col)}=getField($this,$row,$col);
        }
        return %hash;
}

sub substitutions {
        ($this,$variables,$values,$precisions,$scales)=@_;
	@vars=@$variables;
	@vals=@$values;
	@precs=@$precisions;
	@scls=@$scales;
	for (my $i=0; $i<=$#vars; $i++) {
		if (defined($precs[$i]) and defined($scls[$i])) {
			substitution($this,$vars[$i],$vals[$i],
						$precs[$i],$scls[$i]);
		} else {
			substitution($this,$vars[$i],$vals[$i]);
		}
	}
}

sub inputBinds {
        ($this,$variables,$values,$precisions,$scales)=@_;
	@vars=@$variables;
	@vals=@$values;
	@precs=@$precisions;
	@scls=@$scales;
	for (my $i=0; $i<=$#vars; $i++) {
		if (defined($precs[$i]) and defined($scls[$i])) {
			inputBind($this,$vars[$i],$vals[$i],
						$precs[$i],$scls[$i]);
		} else {
			inputBind($this,$vars[$i],$vals[$i]);
		}
	}
}

1;
__END__

=head1 NAME

    SQLRelay::Cursor - Perl API for SQL Relay

=head1 SYNOPSIS

        use SQLRelay::Connection;
        use SQLRelay::Cursor;

        my $sc=SQLRelay::Connection->new("testhost",9000,"",
                                          "testuser","testpassword",0,1);
        my $ss=SQLRelay::Cursor->new($sc);

        $ss->sendQuery("select table_name from user_tables");
        $sc->endSession();

        for (my $i=0; $i<$ss->rowCount(); $i++) {
                print $ss->getField($i,"table_name"), "\n";
        }

=head1 DESCRIPTION

    SQLRelay::Cursor

        new(sqlrclient);

        DESTROY();

        setResultSetBufferSize(rows);
            # Sets the number of rows of the result set
            # to buffer at a time.  0 (the default)
            # means buffer the entire result set.

        getResultSetBufferSize();
            # Returns the number of result set rows that 
            # will be buffered at a time or 0 for the
            # entire result set.

        dontGetColumnInfo();
            # Tells the server not to send any column
            # info (names, types, sizes).  If you don't
            # need that info, you should call this
            # method to improve performance.

        getColumnInfo();
            # Tells the server to send column info.

        mixedCaseColumnNames();
            # Columns names are returned in the same
            # case as they are defined in the database.
            # This is the default.

        upperCaseColumnNames();
            # Columns names are converted to upper case.

        lowerCaseColumnNames();
            # Columns names are converted to lower case.


        cacheToFile(char *filename);
            # Sets query caching on.  Future queries
            # will be cached to the file "filename".
            #
            # A default time-to-live of 10 minutes is
            # also set.
            #
            # Note that once cacheToFile() is called,
            # the result sets of all future queries will
            # be cached to that file until another call 
            # to cacheToFile() changes which file to
            # cache to or a call to cacheOff() turns off
            # caching.

        setCacheTtl(int ttl);
            # Sets the time-to-live for cached result
            # sets. The sqlr-cachemanger will remove each 
            # cached result set "ttl" seconds after it's 
            # created, provided it's scanning the directory
            # containing the cache files.

        getCacheFileName();
            # Returns the name of the file containing the
            # cached result set.

        cacheOff();
            # Sets query caching off.

        getDatabaseList(wild);
            # Sends a query that returns a list of
            # databases/schemas matching "wild".  If wild is empty
            # or NULL then a list of all databases/schemas will be
            # returned.
        getTableList(wild);
            # Sends a query that returns a list of tables
            # matching "wild".  If wild is empty or NULL then
            # a list of all tables will be returned.
        getColumnList(table,wild);
            # Sends a query that returns a list of columns
            # in the table specified by the "table" parameter
            # matching "wild".  If wild is empty or NULL then
            # a list of all columns will be returned.


        # If you don't need to use substitution or bind variables
        # in your queries, use these two methods.
        sendQuery(query);
            # Sends "query" and gets a result set.

        sendQueryWithLength(query,length);
            # Sends "query" with length "length" and gets
            # a result set. This method must be used if
            # the query contains binary data.

        sendFileQuery(path,filename);
            # Sends the query in file "path"/"filename" 
            # and gets a result set.




        # If you need to use substitution or bind variables, in your
        # queries use the following methods.  See the API documentation
        # for more information about substitution and bind variables.
        prepareQuery(query);
            # Prepare to execute "query".

        prepareQueryWithLength(query,length);
            # Prepare to execute "query" with length 
            # "length".  This method must be used if the
            # query contains binary data.

        prepareFileQuery(path,filename);
            # Prepare to execute the contents 
            # of "path"/"filename".

        substitution(variable,value);
            # Define a substitution variable.

        clearBinds();
            # Clear all bind variables.

        countBindVariables();
            # Parses the previously prepared query,
            # counts the number of bind variables defined
            # in it and returns that number.

        inputBind(variable,value);
        inputBind(variable,value,length);
        inputBind(variable,value,precision,scale);
        inputBindBlob(variable,value,size);
        inputBindClob(variable,value,size);
            # Define an input bind variable.
            # (For floating point values, if you don't have the precision and
            # scale then they may both be set to 0.  However in that case you
            # may get unexpected rounding behavior if the server is faking
            # binds.)

        defineOutputBindString(variable,bufferlength);
            # Define an output bind variable.
            # "bufferlength" bytes will be reserved to store the value.
        defineOutputBindBlob(variable);
            # Define a BLOB output bind variable.
        defineOutputBindClob(variable);
            # Define a CLOB output bind variable.
        defineOutputBindCursor(variable);
            # Define a cursor output bind variable.

        substitutions(variables,values);
            # Define an array of substitution variables.

        inputBinds(variables,values);
            # Define an array of input bind variables.

        validateBinds();
            # If you are binding to any variables that 
            # might not actually be in your query, call 
            # this to ensure that the database won't try 
            # to bind them unless they really are in the 
            # query.

        validBind(variable);
            # Returns true if "variable" was a valid
            # bind variable of the query.

        executeQuery();
            # Execute the query that was previously 
            # prepared and bound.

        fetchFromBindCursor();
            # Fetch from a cursor that was returned as
            # an output bind variable.


        getOutputBindString(variable);
            # Get the value stored in a previously
            # defined output bind variable.

	getOutputBindBlob(variable);
            # Get the value stored in a previously
            # defined output bind variable.

	getOutputBindClob(variable);
            # Get the value stored in a previously
            # defined output bind variable.

	getOutputBindLength(variable);
            # Get the length of the value stored in a
            # previously defined output bind variable.

	getOutputBindCursor(variable);
            # Get the cursor associated with a previously
            # defined output bind variable.



        openCachedResultSet(filename);
            # Opens a cached result set as if a query that
            # would have generated it had been executed.
            # Returns true on success and false on failure.



        colCount();
            # Returns the number of columns in the current
            # result set.

        rowCount();
            # Returns the number of rows in the current 
            # result set.

        totalRows();
            # Returns the total number of rows that will 
            # be returned in the result set.  Not all 
            # databases support this call.  Don't use it 
            # for applications which are designed to be 
            # portable across databases.  -1 is returned
            # by databases which don't support this option.

        affectedRows();
            # Returns the number of rows that were 
            # updated, inserted or deleted by the query.
            # Not all databases support this call.  Don't 
            # use it for applications which are designed 
            # to be portable across databases.  -1 is 
            # returned by databases which don't support 
            # this option.

        firstRowIndex();
            # Returns the index of the first buffered row.
            # This is useful when bufferning only part of the
            # result set at a time.

        endOfResultSet();
            # Returns false if part of the result set is still
            # pending on the server and true if not.  This
            # method can only return false if 
            # setResultSetBufferSize() has been called
            # with a parameter other than 0.

        errorMessage();
            # If a query failed and generated an error, the
            # error message is available here.  If the 
            # query succeeded then this method returns NULL.

        errorNumber();
            # If a query failed and generated an
            # error, the error number is available here.
            # If there is no error then this method 
            # returns 0.

        getNullsAsEmptyStrings();
            # Tells the client to return NULL fields and 
            # output bind variables as empty strings.
            # This is the default.

        getNullsAsUndefined();
            # Tells the client to return NULL fields and 
            # output bind variables as undefined.

        getField(row, col);
            # Returns a pointer to the value of the 
            # specified row and column.

        getFieldLength(row, col);
            # Returns a the length of the 
            # specified row and column.

        getRow(row);
            # Returns an array of the values of the
            # specified row or an empty list if the
            # requested row is past the end of the
            # result set.

        getRowHash(row);
            # Returns the requested row as values in a
            # hash with column names for keys or an
            # empty list if the requested row is past
            # the end of the result set.

        getRowLengths(row);
            # Returns a null terminated array of the 
            # lengths of the specified row or an empty
            # list if the requested row is past the end
            # of the result set.

        getRowLengthsHash(row);
            # Returns the requested row lengths as values 
            # in a hash with column names for keys or an
            # empty list if the requested row is past
            # the end of the result set.

        getColumnNames();
            # Returns a null terminated array of the 
            # column names of the current result set.

        getColumnName(col);
            # Returns the name of the specified column.

        getColumnType(col);
            # Returns the type of the specified column.

        getColumnLength(col);
            # Returns the length of the specified column.

        getColumnPrecision(col);
            # Returns the precision of the specified
            # column.
            # Precision is the total number of digits in
            # a number.  eg: 123.45 has a precision of 5.
            # For non-numeric types, it's the number of
            # characters in the string.

        getColumnScale(col);
            # Returns the scale of the specified column.
            # Scale is the total number of digits to the
            # right of the decimal point in a number.
            # eg: 123.45 has a scale of 2.

        getColumnIsNullable(col);
            # Returns 1 if the specified column can
            # contain nulls and 0 otherwise.

        getColumnIsPrimaryKey(col);
            # Returns 1 if the specified column is a
            # primary key and 0 otherwise.

        getColumnIsUnique(col);
            # Returns 1 if the specified column is
            # unique and 0 otherwise.

        getColumnIsPartOfKey(col);
            # Returns 1 if the specified column is
            # part of a composite key and 0 otherwise.

        getColumnIsUnsigned(col);
            # Returns 1 if the specified column is
            # an unsigned number and 0 otherwise.

        getColumnIsZeroFilled(col);
            # Returns 1 if the specified column was
            # created with the zero-fill flag and 0
            # otherwise.

        getColumnIsBinary(col);
            # Returns 1 if the specified column
            # contains binary data and 0
            # otherwise.

        getColumnIsAutoIncrement(col);
            # Returns 1 if the specified column
            # auto-increments and 0 otherwise.

        getLongest(col);
            # Returns the length of the longest field
            # in the specified column.

        suspendResultSet();
            # Tells the server to leave this result
            # set open when the client calls 
            # suspendSession() so that another client can 
            # connect to it using resumeResultSet() after 
            # it calls resumeSession().

        getResultSetId();
            # Returns the internal ID of this result set.
            # This parameter may be passed to another 
            # statement for use in the resumeResultSet() 
            # method.
            # Note: the value returned by this method is
            # only valid after a call to suspendResultSet().

        resumeResultSet(int id);
            # Resumes a result set previously left open 
            # using suspendSession().
            # Returns true on success and false on failure.

        resumeCachedResultSet(int id, char *filename);
            # Resumes a result set previously left open
            # using suspendSession() and continues caching
            # the result set to "filename".
            # Returns true on success and false on failure.

        closeResultSet();
            # Closes the current result set, if one is open.  Data
            # that has been fetched already is still available but
            # no more data may be fetched.  Server side resources
            # for the result set are freed as well.

=head1 AUTHOR

    David Muse
    david.muse@firstworks.com

=cut
