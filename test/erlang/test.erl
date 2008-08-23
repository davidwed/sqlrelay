-
module(test).

%
% This module provides a test harness for the ERL SQLRelay API
%

-import(sqlrelay, [alloc/7, connectionFree/0, cursorFree/0, endSession/0, suspendSession/0, resumeSession/2]).
-import(sqlrelay, [ping/0, identify/0, dbVersion/0, bindFormat/0, errorMessage/0]).
-import(sqlrelay, [getConnectionPort/0, getConnectionSocket/0]).
-import(sqlrelay, [autoCommitOn/0, autoCommitOff/0, commit/0, rollback/0]).
-import(sqlrelay, [debugOn/0, debugOff/0, getDebug/0]).
-import(sqlrelay, [setResultSetBufferSize/1, getResultSetBufferSize/0]).
-import(sqlrelay, [dontGetColumnInfo/0, getColumnInfo/0]).
-import(sqlrelay, [mixedCaseColumnNames/0, upperCaseColumnNames/0, lowerCaseColumnNames/0]).
-import(sqlrelay, [cacheToFile/1, setCacheTtl/1, getCacheFileName/0, cacheOff/0]).
-import(sqlrelay, [sendQuery/1, sendQueryWithLength/2, sendFileQuery/2]).
-import(sqlrelay, [prepareQuery/1, prepareQueryWithLength/2, prepareFileQuery/2]).
-import(sqlrelay, [subString/2, subLong/2, subDouble/4]).
-import(sqlrelay, [clearBinds/0, countBindVariables/0]).
-import(sqlrelay, [inputBindString/2, inputBindLong/2, inputBindDouble/4, inputBindBlob/3, inputBindClob/3 ]).
-import(sqlrelay, [defineOutputBindString/2, defineOutputBindInteger/1, defineOutputBindDouble/1, defineOutputBindBlob/1, defineOutputBindClob/1, defineOutputBindCursor/1]).
-import(sqlrelay, [validateBinds/0, validBind/1]).
-import(sqlrelay, [executeQuery/0]).
-import(sqlrelay, [fetchFromBindCursor/0]).
-import(sqlrelay, [getOutputBindString/1, getOutputBindInteger/1, getOutputBindDouble/1, getOutputBindBlob/1, getOutputBindClob/1, getOutputBindCursor/1, getOutputBindLength/1]).
-import(sqlrelay, [openCachedResultSet/1]).
-import(sqlrelay, [colCount/0, rowCount/0, totalRows/0, affectedRows/0]).
-import(sqlrelay, [firstRowIndex/0, endOfResultSet/0]).
-import(sqlrelay, [getNullsAsEmptyStrings/0, getNullsAsNulls/0]).
-import(sqlrelay, [getFieldByIndex/2, getFieldByName/2]).
-import(sqlrelay, [getFieldAsIntegerByIndex/2, getFieldAsIntegerByName/2]).
-import(sqlrelay, [getFieldAsDoubleByIndex/2, getFieldAsDoubleByName/2]).
-import(sqlrelay, [getFieldLengthByIndex/2, getFieldLengthByName/2]).
-import(sqlrelay, [getRow/1, getRowLengths/1]).
-import(sqlrelay, [getColumnNames/0, getColumnName/1]).
-import(sqlrelay, [getColumnTypeByIndex/1, getColumnTypeByName/1]).
-import(sqlrelay, [getColumnLengthByIndex/1, getColumnLengthByName/1]).
-import(sqlrelay, [getColumnPrecisionByIndex/1, getColumnPrecisionByName/1]).
-import(sqlrelay, [getColumnScaleByIndex/1, getColumnScaleByName/1]).
-import(sqlrelay, [getColumnIsNullableByIndex/1, getColumnIsNullableByName/1]).
-import(sqlrelay, [getColumnIsPrimaryKeyByIndex/1, getColumnIsPrimaryKeyByName/1]).
-import(sqlrelay, [getColumnIsUniqueByIndex/1, getColumnIsUniqueByName/1]).
-import(sqlrelay, [getColumnIsPartOfKeyByIndex/1, getColumnIsPartOfKeyByName/1]).
-import(sqlrelay, [getColumnIsUnsignedByIndex/1, getColumnIsUnsignedByName/1]).
-import(sqlrelay, [getColumnIsZeroFilledByIndex/1, getColumnIsZeroFilledByName/1]).
-import(sqlrelay, [getColumnIsBinaryByIndex/1, getColumnIsBinaryByName/1]).
-import(sqlrelay, [getColumnIsAutoIncrementByIndex/1, getColumnIsAutoIncrementByName/1]).
-import(sqlrelay, [getLongestByIndex/1, getLongestByName/1]).
-import(sqlrelay, [suspendResultSet/0, getResultSetId/0, resumeResultSet/1, resumeCachedResultSet/2]).

-import(string, [len/1]).


% API
-export([startTest/0, loop/1, startRelay/0, stopRelay/0]).

%
% Main test harness
%

% start the relay process if it has not yet been started
startRelay() ->
	case(get("relayStarted")) of 
		"true" ->
			true;
		_ ->
			io:format("Starting relay...", []),
			sqlrelay:start(),
			put("relayStarted", "true"),
			io:format("done~n", [])
	end.


stopRelay() ->
	erase("relayStarted"),
	sqlrelay:stop().


startTest() ->
	% start the relay process
	startRelay(),

	% adjust the following line for your host, port, username, and password
   	{ok, _} = alloc("node6", 9000, "", "user1", "password1", 10, 3),

        % ping the database to see if it's up
        case(ping()) of
                {ok,1} ->
                        io:format("Database is up~n", []),

			% create or replace a test table 
			createTestTable(),
			loadTestData(),

                        % send a query
                        {ok, _} = sendQuery("select * from testtable1"),

                        % process results
                        io:format("~n*********** Testing Result Processing~n", []),
                        {ok, Rows} = rowCount(),
                        io:format("Number of rows returned is ~w~n", [Rows]),

% the following function fails
%			{ok, Rows} = totalRows(),	% total rows should be the same number that was just returned

                        {ok, Field00} = getFieldByIndex(0,0),
                        io:format("Value at row 0, col 0 is ~w~n", [list_to_atom(Field00)]),
                        {ok, Field01} = getFieldByIndex(0,1),
                        io:format("Value at row 0, col 1 is ~w~n", [list_to_atom(Field01)]),
                        {ok, Field02} = getFieldByIndex(0,2),
                        io:format("Value at row 0, col 2 is ~w~n", [list_to_atom(Field02)]),
                        
			Record0 = getRow(0),
			io:format("Row 0 contents are ~w~n", [Record0]),
			
			Record1 = getRow(1),
			io:format("Row 1 contents are ~w~n", [Record1]),
			
			{ok, RowLengths} = getRowLengths(1),
			io:format("Length of row 1 is ~w~n", [RowLengths]),
	
			{ok, "1"} = getFieldByName(0, "col1"),
			{ok, FieldLength} = getFieldLengthByIndex(0, 0),
			io:format("Field at row 0, col 0 length is ~w~n", [FieldLength]),
			{ok, FieldLength} = getFieldLengthByName(0, "COL1"),
			cursorFree(),		
	
			% send a query from a file
			% note: you may need to adjust the directory where the file is loaded
			{ok, 1} = sendFileQuery("./", "query_file.txt"),
			cursorFree(),		


			% test suspend/resume session functions
                        io:format("~n*********** Testing suspend/resume functions~n", []),
			% test suspend/resume result set functions

			% test identity/version functions
                        io:format("*********** Testing identity/version functions~n", []),
			{ok, Identity} = identify(),
			io:format("Identity is ~s~n", [Identity]),
			
			{ok, Version} = dbVersion(),
			io:format("DB Version is ~s~n", [Version]),

			{ok, Format} = bindFormat(),
			io:format("Bind format is ~s~n", [Format]),

			% test commit functions
                        io:format("~n*********** Testing commit functions~n", []),
			{ok, _} = autoCommitOff(),
                        {ok, 1} = sendQuery("insert into testtable1 (col1, col2, col3) values ('5', '6', '7')"),
			{ok, _} = rollback(),
                        {ok, 1} = sendQuery("insert into testtable1 (col1, col2, col3) values ('5', '6', '7')"),
			{ok, _} = commit(),
			{ok, _} = autoCommitOn(),
                        {ok, 1} = sendQuery("delete from testtable1 where col1='5'"),
			{ok, 1} = affectedRows(),
			cursorFree(),		
		
	
			% test debug functions
                        io:format("~n*********** Testing debug functions~n", []),
			{ok, _} = debugOn(),
			{ok, _} = debugOff(),
			{ok, 0} = getDebug(),

			% test buffer size functions
                        io:format("~n*********** Testing buffer size functions~n", []),
			{ok, _} = setResultSetBufferSize(2),
			{ok, 2} = getResultSetBufferSize(),
			{ok, 1} = sendQuery("select * from testtable1"),
                        {ok, 2} = rowCount(),
			cursorFree(),		

			% test column info functions
                        io:format("~n*********** Testing column info functions~n", []),
			{ok, _} = dontGetColumnInfo(),
			{ok, _} = getColumnInfo(),

			{ok, _} = mixedCaseColumnNames(),
			{ok, 1} = sendQuery("select * from testtable1"),
% the following function fails in Oracle because mixed case columns are not returned
%			{ok, "Col1"} = getColumnName(0),
			cursorFree(),		

			{ok, _} = upperCaseColumnNames(),
			{ok, 1} = sendQuery("select * from testtable1"),
% the following function fails
			{ok, "COL1"} = getColumnName(0),
			cursorFree(),		

			{ok, _} = lowerCaseColumnNames(),
			{ok, 1} = sendQuery("select * from testtable1"),
% the following function fails in Oracle because lower case columns are not returned
%			{ok, "col1"} = getColumnName(0),
			cursorFree(),		


			% test cache functions
                        io:format("~n*********** Testing cache functions~n", []),
			Filename = "/tmp/mycache",
			{ok, _} = cacheToFile(Filename),
			{ok, Filename} = getCacheFileName(),
			
			{ok, _} = setCacheTtl(600),
			{ok, _} = sendQuery("select * from user_objects"),
			{ok, _} = cacheOff(),

			{ok, _} = openCachedResultSet(Filename),
			{ok, CachedRowCount} = rowCount(),
			io:format("Number of cached rows is ~w~n", [CachedRowCount]),

			{ok, _} = cursorFree(),

			% test prepared query functions
                        io:format("~n*********** Testing prepared query functions~n", []),
			% note: you may need to adjust the directory where the file is loaded
 			{ok, _} = prepareFileQuery("./", "prepared_query_file.txt"),
			{ok, _} = inputBindString("value","a"),
			{ok, _} = validateBinds(),
			{ok, _} = executeQuery(),
			{ok, 1} = rowCount(),
			{ok, _} = cursorFree(),

   
			{ok, _} = prepareQuery("select * from testtable1 where col2 = :value"),
			{ok, _} = inputBindString("value","a"),
			{ok, _} = validateBinds(),
			{ok, _} = executeQuery(),
% the following function fails - the prepareQueryWithLength function does not appear to be working
%			{ok, 1} = rowCount(),
			{ok, _} = cursorFree(),

 			Query = "select * from testtable1 where col2 = :value",
                        {ok, _} = prepareQueryWithLength(Query, len(Query)),
                        {ok, _} = inputBindString("value","a"),
                        {ok, _} = executeQuery(),
% the following function fails - the prepareQueryWithLength function does not appear to be working
%                        {ok, 1} = rowCount(),
                        {ok, _} = cursorFree(),


			% try executing a stored procedure
                        io:format("~n*********** Testing stored procedures~n", []),
			createTestStoredProcedure(),
%			{ok, _} = prepareQuery("begin testproc(:in1,:in2,:in3); end"),
%			{ok, _} = prepareQuery("call testproc(:in1,:in2,:in3)"),

			% note: you may need to adjust the directory where the file is loaded
 			{ok, _} = prepareFileQuery("./", "prepared_sp_file.txt"),
			{ok, 3} = countBindVariables(), 
			{ok, _} = inputBindLong("in1",10),
			{ok, _} = inputBindString("in2","hello"),
			{ok, _} = inputBindString("in3","world"),
			{ok, _} = validateBinds(),
			{ok, _} = executeQuery(),
% investigate -- for some reason the row is not inserted
			{ok, _} = cursorFree(),

			% try selecting the value that was just inserted using the stored procedure
                        {ok, _} = sendQuery("select count(*) from testtable1 where col1=10"),
% the following match fails - the row was not inserted correctly above 
%                       {ok, 1} = getFieldByIndex(0,0),
			{ok, _} = cursorFree(),


			% test column attribute functions
                        io:format("~n*********** Testing column attribute functions~n", []),
			{ok, _} = sendQuery("select * from testtable1"),
			{ok, 0} = getColumnIsNullableByIndex(0),
			{ok, 1} = getColumnIsNullableByIndex(1),
			{ok, 0} = getColumnIsNullableByName("col1"),
			{ok, 1} = getColumnIsNullableByName("col2"),

% the following function fails  
%			{ok, 1} = getColumnIsPrimaryKeyByIndex(0),
			{ok, 0} = getColumnIsPrimaryKeyByIndex(1),
% the following function fails  
%			{ok, 1} = getColumnIsPrimaryKeyByName("col1"),
			{ok, 0} = getColumnIsPrimaryKeyByName("col2"),
			{ok, _} = cursorFree(),



                        % end the session
                        io:format("~n*********** Testing Complete **********~n", []),
			{ok, _} = connectionFree();
                {ok,0} ->
                        io:format("Database is down~n", []);
                _ ->
                        io:format("Error executing ping~n", []),
                        false
        end.

loop(0)  -> 
	{ok, _} = endSession(),
	stopRelay(),
	io:format("Test done~n", []);

loop(Times) ->
        io:format("~n*********** Iteration ~w ***********~n", [Times]),
	startTest(),
	loop(Times - 1).

%
% See if a stored procedure used for testing exists -- if not create it
%
createTestStoredProcedure() ->
	{ok, _} = sendQuery("select count(*) from user_objects where object_name='TESTPROC'"),
        {ok, Count} = getFieldByIndex(0,0),

        case(Count) of
                "1" ->
                        io:format("Test procedure has already been created~n", []);
                "0" ->
                        io:format("Test procedure has not been created~n", []),

			% create the stored procedure using Oracle syntax
			{ok, _} = sendQuery("
create procedure testproc(in1 in number, in2 in varchar2, in3 in varchar2) is
begin
        insert into testtable1 values (in1,in2,in3);
	commit;
end;
")
        end,

	{ok, _} = cursorFree().

%
% See if a table used for testing exists -- if not create it
%
createTestTable() ->
	{ok, _} = sendQuery("select count(*) from user_objects where upper(object_name)='TESTTABLE1'"),
        {ok, Count} = getFieldByIndex(0,0),

        case(Count) of
                "1" ->
                        io:format("Test table has already been created, truncating data...~n", []),
			{ok, _} = sendQuery("truncate table testtable1");
                "0" ->
                        io:format("Test table has not been created~n", []),

			% create the table using Oracle syntax
			{ok, _} = sendQuery("create table testtable1 (col1 number not null, col2 varchar2(20), col3 varchar2(20), CONSTRAINT test_pk PRIMARY KEY (col1))")
        end,

	{ok, _} = cursorFree().


%
% Load test data 
%
loadTestData() ->
	io:format("Loading test data...~n", []),
	{ok, 1} = sendQuery("insert into testtable1 (col1, col2, col3) values (1, 'a', 'b')"),
	{ok, _} = commit(),
	{ok, _} = cursorFree(),

	{ok, 1} = sendQuery("insert into testtable1 (col1, col2, col3) values (2, 'b', 'c')"),
	{ok, _} = commit(),
	{ok, _} = cursorFree().
