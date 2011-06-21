#! /usr/bin/tclsh

# Copyright] c] 2001] David Muse
# See the file COPYING for more information.


load /usr/lib/sqlrelay/sqlrelay.so sqlrelay

proc checkUndef {value} {

	switch $value "" {
		puts -nonewline "success "
	} default {
		puts "$value != $success "
		puts "failure "
		exit
	}
}

proc checkSuccess {value success} {

	if {$value==$success} {
		puts -nonewline "success "
	} else {
		puts "$value != $success "
		puts "failure "
		exit
	}
}

# instantiation
set con [sqlrcon -server "localhost" -port 9000 -socket "/tmp/test.socket" -user "test" -password "test" -retrytime 0 -tries 1]
set cur [$con sqlrcur]

puts "IDENTIFY: "
checkSuccess [$con identify] "postgresql"
puts ""

# ping
puts "PING: "
checkSuccess [$con ping] 1
puts ""

# drop existing table
catch {$cur sendQuery "drop table testtable"}

puts "CREATE TEMPTABLE: "
checkSuccess [$cur sendQuery "create table testtable (testint int, testfloat float, testreal real, testsmallint smallint, testchar char(40), testvarchar varchar(40), testdate date, testtime time, testtimestamp timestamp)"] 1
puts ""

puts "BEGIN TRANSCTION: "
checkSuccess [$cur sendQuery "begin"] 1
puts ""

puts "INSERT: "
checkSuccess [$cur sendQuery "insert into testtable values (1,1.1,1.1,1,'testchar1','testvarchar1','01/01/2001','01:00:00',NULL)"] 1
checkSuccess [$cur sendQuery "insert into testtable values (2,2.2,2.2,2,'testchar2','testvarchar2','01/01/2002','02:00:00',NULL)"] 1
checkSuccess [$cur sendQuery "insert into testtable values (3,3.3,3.3,3,'testchar3','testvarchar3','01/01/2003','03:00:00',NULL)"] 1
checkSuccess [$cur sendQuery "insert into testtable values (4,4.4,4.4,4,'testchar4','testvarchar4','01/01/2004','04:00:00',NULL)"] 1
puts ""

puts "AFFECTED ROWS: "
checkSuccess [$cur affectedRows] 1
puts ""

puts "BIND BY NAME: "
$cur prepareQuery "insert into testtable values (:var1,:var2,:var3,:var4,:var5,:var6,:var7,:var8)"
checkSuccess [$cur countBindVariables] 8
$cur inputBind "var1" 5
$cur inputBind "var2" 5.5 4 2
$cur inputBind "var3" 5.5 4 2
$cur inputBind "var4" 5
$cur inputBind "var5" "testchar5"
$cur inputBind "var6" "testvarchar5"
$cur inputBind "var7" "01/01/2005"
$cur inputBind "var8" "05:00:00"
checkSuccess [$cur executeQuery] 1
$cur clearBinds 
$cur inputBind "var1" 6
$cur inputBind "var2" 6.6 4 2
$cur inputBind "var3" 6.6 4 2
$cur inputBind "var4" 6
$cur inputBind "var5" "testchar6"
$cur inputBind "var6" "testvarchar6"
$cur inputBind "var7" "01/01/2006"
$cur inputBind "var8" "06:00:00"
checkSuccess [$cur executeQuery] 1
puts ""

puts "ARRAY OF BINDS BY NAME: "
$cur clearBinds
$cur inputBinds {{"var1" 7} {"var2" 7.7 4 2} {"var3" 7.7 4 2} {"var4" 7} {"var5" "testchar7"} {"var6" "testvarchar7"} {"var7" "01/01/2007"} {"var8" "07:00:00"}}
checkSuccess [$cur executeQuery] 1
puts ""

puts "BIND BY NAME WITH VALIDATION: "
$cur clearBinds 
$cur inputBind "var1" 8
$cur inputBind "var2" 8.8 4 2
$cur inputBind "var3" 8.8 4 2
$cur inputBind "var4" 8
$cur inputBind "var5" "testchar8"
$cur inputBind "var6" "testvarchar8"
$cur inputBind "var7" "01/01/2008"
$cur inputBind "var8" "08:00:00"
$cur inputBind "var9" "junkvalue"
$cur validateBinds 
checkSuccess [$cur executeQuery] 1
puts ""

puts "SELECT: "
checkSuccess [$cur sendQuery "select * from testtable order by testint"] 1
puts ""

puts "COLUMN COUNT: "
checkSuccess [$cur colCount] 9
puts ""

puts "COLUMN NAMES: "
checkSuccess [$cur getColumnName 0] "testint"
checkSuccess [$cur getColumnName 1] "testfloat"
checkSuccess [$cur getColumnName 2] "testreal"
checkSuccess [$cur getColumnName 3] "testsmallint"
checkSuccess [$cur getColumnName 4] "testchar"
checkSuccess [$cur getColumnName 5] "testvarchar"
checkSuccess [$cur getColumnName 6] "testdate"
checkSuccess [$cur getColumnName 7] "testtime"
checkSuccess [$cur getColumnName 8] "testtimestamp"
set cols [$cur getColumnNames]
checkSuccess [lindex $cols 0] "testint"
checkSuccess [lindex $cols 1] "testfloat"
checkSuccess [lindex $cols 2] "testreal"
checkSuccess [lindex $cols 3] "testsmallint"
checkSuccess [lindex $cols 4] "testchar"
checkSuccess [lindex $cols 5] "testvarchar"
checkSuccess [lindex $cols 6] "testdate"
checkSuccess [lindex $cols 7] "testtime"
checkSuccess [lindex $cols 8] "testtimestamp"
puts ""

puts "COLUMN TYPES: "
checkSuccess [$cur getColumnTypeByIndex 0] "int4"
checkSuccess [$cur getColumnTypeByName "testint"] "int4"
checkSuccess [$cur getColumnTypeByIndex 1] "float8"
checkSuccess [$cur getColumnTypeByName "testfloat"] "float8"
checkSuccess [$cur getColumnTypeByIndex 2] "float4"
checkSuccess [$cur getColumnTypeByName "testreal"] "float4"
checkSuccess [$cur getColumnTypeByIndex 3] "int2"
checkSuccess [$cur getColumnTypeByName "testsmallint"] "int2"
checkSuccess [$cur getColumnTypeByIndex 4] "bpchar"
checkSuccess [$cur getColumnTypeByName "testchar"] "bpchar"
checkSuccess [$cur getColumnTypeByIndex 5] "varchar"
checkSuccess [$cur getColumnTypeByName "testvarchar"] "varchar"
checkSuccess [$cur getColumnTypeByIndex 6] "date"
checkSuccess [$cur getColumnTypeByName "testdate"] "date"
checkSuccess [$cur getColumnTypeByIndex 7] "time"
checkSuccess [$cur getColumnTypeByName "testtime"] "time"
checkSuccess [$cur getColumnTypeByIndex 8] "timestamp"
checkSuccess [$cur getColumnTypeByName "testtimestamp"] "timestamp"
puts ""

puts "COLUMN LENGTH: "
checkSuccess [$cur getColumnLengthByIndex 0] 4
checkSuccess [$cur getColumnLengthByName "testint"] 4
checkSuccess [$cur getColumnLengthByIndex 1] 8
checkSuccess [$cur getColumnLengthByName "testfloat"] 8
checkSuccess [$cur getColumnLengthByIndex 2] 4
checkSuccess [$cur getColumnLengthByName "testreal"] 4
checkSuccess [$cur getColumnLengthByIndex 3] 2
checkSuccess [$cur getColumnLengthByName "testsmallint"] 2
checkSuccess [$cur getColumnLengthByIndex 4] 44
checkSuccess [$cur getColumnLengthByName "testchar"] 44
checkSuccess [$cur getColumnLengthByIndex 5] 44
checkSuccess [$cur getColumnLengthByName "testvarchar"] 44
checkSuccess [$cur getColumnLengthByIndex 6] 4
checkSuccess [$cur getColumnLengthByName "testdate"] 4
checkSuccess [$cur getColumnLengthByIndex 7] 8
checkSuccess [$cur getColumnLengthByName "testtime"] 8
checkSuccess [$cur getColumnLengthByIndex 8] 8
checkSuccess [$cur getColumnLengthByName "testtimestamp"] 8
puts ""

puts "LONGEST COLUMN: "
checkSuccess [$cur getLongestByIndex 0] 1
checkSuccess [$cur getLongestByName "testint"] 1
checkSuccess [$cur getLongestByIndex 1] 3
checkSuccess [$cur getLongestByName "testfloat"] 3
checkSuccess [$cur getLongestByIndex 2] 3
checkSuccess [$cur getLongestByName "testreal"] 3
checkSuccess [$cur getLongestByIndex 3] 1
checkSuccess [$cur getLongestByName "testsmallint"] 1
checkSuccess [$cur getLongestByIndex 4] 40
checkSuccess [$cur getLongestByName "testchar"] 40
checkSuccess [$cur getLongestByIndex 5] 12
checkSuccess [$cur getLongestByName "testvarchar"] 12
checkSuccess [$cur getLongestByIndex 6] 10
checkSuccess [$cur getLongestByName "testdate"] 10
checkSuccess [$cur getLongestByIndex 7] 8
checkSuccess [$cur getLongestByName "testtime"] 8
puts ""

puts "ROW COUNT: "
checkSuccess [$cur rowCount] 8
puts ""

puts "TOTAL ROWS: "
checkSuccess [$cur totalRows] 8
puts ""

puts "FIRST ROW INDEX: "
checkSuccess [$cur firstRowIndex] 0
puts ""

puts "END OF RESULT SET: "
checkSuccess [$cur endOfResultSet] 1
puts ""

puts "FIELDS BY INDEX: "
checkSuccess [$cur getFieldByIndex 0 0] "1"
checkSuccess [$cur getFieldByIndex 0 1] "1.1"
checkSuccess [$cur getFieldByIndex 0 2] "1.1"
checkSuccess [$cur getFieldByIndex 0 3] "1"
checkSuccess [$cur getFieldByIndex 0 4] "testchar1                               "
checkSuccess [$cur getFieldByIndex 0 5] "testvarchar1"
checkSuccess [$cur getFieldByIndex 0 6] "2001-01-01"
checkSuccess [$cur getFieldByIndex 0 7] "01:00:00"
puts ""
checkSuccess [$cur getFieldByIndex 7 0] "8"
checkSuccess [$cur getFieldByIndex 7 1] "8.8"
checkSuccess [$cur getFieldByIndex 7 2] "8.8"
checkSuccess [$cur getFieldByIndex 7 3] "8"
checkSuccess [$cur getFieldByIndex 7 4] "testchar8                               "
checkSuccess [$cur getFieldByIndex 7 5] "testvarchar8"
checkSuccess [$cur getFieldByIndex 7 6] "2008-01-01"
checkSuccess [$cur getFieldByIndex 7 7] "08:00:00"
puts ""

puts "FIELD LENGTHS BY INDEX: "
checkSuccess [$cur getFieldLengthByIndex 0 0] 1
checkSuccess [$cur getFieldLengthByIndex 0 1] 3
checkSuccess [$cur getFieldLengthByIndex 0 2] 3
checkSuccess [$cur getFieldLengthByIndex 0 3] 1
checkSuccess [$cur getFieldLengthByIndex 0 4] 40
checkSuccess [$cur getFieldLengthByIndex 0 5] 12
checkSuccess [$cur getFieldLengthByIndex 0 6] 10
checkSuccess [$cur getFieldLengthByIndex 0 7] 8
puts ""
checkSuccess [$cur getFieldLengthByIndex 7 0] 1
checkSuccess [$cur getFieldLengthByIndex 7 1] 3
checkSuccess [$cur getFieldLengthByIndex 7 2] 3
checkSuccess [$cur getFieldLengthByIndex 7 3] 1
checkSuccess [$cur getFieldLengthByIndex 7 4] 40
checkSuccess [$cur getFieldLengthByIndex 7 5] 12
checkSuccess [$cur getFieldLengthByIndex 7 6] 10
checkSuccess [$cur getFieldLengthByIndex 7 7] 8
puts ""

puts "FIELDS BY NAME: "
checkSuccess [$cur getFieldByName 0 "testint"] "1"
checkSuccess [$cur getFieldByName 0 "testfloat"] "1.1"
checkSuccess [$cur getFieldByName 0 "testreal"] "1.1"
checkSuccess [$cur getFieldByName 0 "testsmallint"] "1"
checkSuccess [$cur getFieldByName 0 "testchar"] "testchar1                               "
checkSuccess [$cur getFieldByName 0 "testvarchar"] "testvarchar1"
checkSuccess [$cur getFieldByName 0 "testdate"] "2001-01-01"
checkSuccess [$cur getFieldByName 0 "testtime"] "01:00:00"
puts ""
checkSuccess [$cur getFieldByName 7 "testint"] "8"
checkSuccess [$cur getFieldByName 7 "testfloat"] "8.8"
checkSuccess [$cur getFieldByName 7 "testreal"] "8.8"
checkSuccess [$cur getFieldByName 7 "testsmallint"] "8"
checkSuccess [$cur getFieldByName 7 "testchar"] "testchar8                               "
checkSuccess [$cur getFieldByName 7 "testvarchar"] "testvarchar8"
checkSuccess [$cur getFieldByName 7 "testdate"] "2008-01-01"
checkSuccess [$cur getFieldByName 7 "testtime"] "08:00:00"
puts ""

puts "FIELD LENGTHS BY NAME: "
checkSuccess [$cur getFieldLengthByName 0 "testint"] 1
checkSuccess [$cur getFieldLengthByName 0 "testfloat"] 3
checkSuccess [$cur getFieldLengthByName 0 "testreal"] 3
checkSuccess [$cur getFieldLengthByName 0 "testsmallint"] 1
checkSuccess [$cur getFieldLengthByName 0 "testchar"] 40
checkSuccess [$cur getFieldLengthByName 0 "testvarchar"] 12
checkSuccess [$cur getFieldLengthByName 0 "testdate"] 10
checkSuccess [$cur getFieldLengthByName 0 "testtime"] 8
puts ""
checkSuccess [$cur getFieldLengthByName 7 "testint"] 1
checkSuccess [$cur getFieldLengthByName 7 "testfloat"] 3
checkSuccess [$cur getFieldLengthByName 7 "testreal"] 3
checkSuccess [$cur getFieldLengthByName 7 "testsmallint"] 1
checkSuccess [$cur getFieldLengthByName 7 "testchar"] 40
checkSuccess [$cur getFieldLengthByName 7 "testvarchar"] 12
checkSuccess [$cur getFieldLengthByName 7 "testdate"] 10
checkSuccess [$cur getFieldLengthByName 7 "testtime"] 8
puts ""

puts "FIELDS BY ARRAY: "
set fields [$cur getRow 0]
checkSuccess [lindex $fields 0] 1
checkSuccess [lindex $fields 1] 1.1
checkSuccess [lindex $fields 2] 1.1
checkSuccess [lindex $fields 3] 1
checkSuccess [lindex $fields 4] "testchar1                               "
checkSuccess [lindex $fields 5] "testvarchar1"
checkSuccess [lindex $fields 6] "2001-01-01"
checkSuccess [lindex $fields 7] "01:00:00"
puts ""

puts "FIELD LENGTHS BY ARRAY: "
set fieldlens [$cur getRowLengths 0]
checkSuccess [lindex $fieldlens 0] 1
checkSuccess [lindex $fieldlens 1] 3
checkSuccess [lindex $fieldlens 2] 3
checkSuccess [lindex $fieldlens 3] 1
checkSuccess [lindex $fieldlens 4] 40
checkSuccess [lindex $fieldlens 5] 12
checkSuccess [lindex $fieldlens 6] 10
checkSuccess [lindex $fieldlens 7] 8
puts ""

puts "INDIVIDUAL SUBSTITUTIONS: "
$cur prepareQuery "select \$(var1),'\$(var2)',\$(var3)"
$cur substitution "var1" 1
$cur substitution "var2" "hello"
$cur substitution "var3" 10.5556 6 4
checkSuccess [$cur executeQuery] 1
puts ""

puts "FIELDS: "
checkSuccess [$cur getFieldByIndex 0 0] "1"
checkSuccess [$cur getFieldByIndex 0 1] "hello"
checkSuccess [$cur getFieldByIndex 0 2] "10.5556"
puts ""

puts "ARRAY SUBSTITUTIONS: "
$cur prepareQuery "select \$(var1),'\$(var2)',\$(var3)"
$cur substitutions {{"var1" 1} {"var2" "hello"} {"var3" 10.5556 6 4}}
checkSuccess [$cur executeQuery] 1
puts ""

puts "FIELDS: "
checkSuccess [$cur getFieldByIndex 0 0] "1"
checkSuccess [$cur getFieldByIndex 0 1] "hello"
checkSuccess [$cur getFieldByIndex 0 2] "10.5556"
puts ""

puts "RESULT SET BUFFER SIZE: "
checkSuccess [$cur getResultSetBufferSize] 0
$cur setResultSetBufferSize 2
checkSuccess [$cur sendQuery "select * from testtable order by testint"] 1
checkSuccess [$cur getResultSetBufferSize] 2
puts ""
checkSuccess [$cur firstRowIndex] 0
checkSuccess [$cur endOfResultSet] 0
checkSuccess [$cur rowCount] 2
checkSuccess [$cur getFieldByIndex 0 0] "1"
checkSuccess [$cur getFieldByIndex 1 0] "2"
checkSuccess [$cur getFieldByIndex 2 0] "3"
puts ""
checkSuccess [$cur firstRowIndex] 2
checkSuccess [$cur endOfResultSet] 0
checkSuccess [$cur rowCount] 4
checkSuccess [$cur getFieldByIndex 6 0] "7"
checkSuccess [$cur getFieldByIndex 7 0] "8"
puts ""
checkSuccess [$cur firstRowIndex] 6
checkSuccess [$cur endOfResultSet] 0
checkSuccess [$cur rowCount] 8
checkUndef [$cur getFieldByIndex 8 0]
puts ""
checkSuccess [$cur firstRowIndex] 8
checkSuccess [$cur endOfResultSet] 1
checkSuccess [$cur rowCount] 8
puts ""

puts "DONT GET COLUMN INFO: "
$cur dontGetColumnInfo 
checkSuccess [$cur sendQuery "select * from testtable order by testint"] 1
checkUndef [$cur getColumnName 0]
checkSuccess [$cur getColumnLengthByIndex 0] 0
checkUndef [$cur getColumnTypeByIndex 0]
$cur getColumnInfo 
checkSuccess [$cur sendQuery "select * from testtable order by testint"] 1
checkSuccess [$cur getColumnName 0] "testint"
checkSuccess [$cur getColumnLengthByIndex 0] 4
checkSuccess [$cur getColumnTypeByIndex 0] "int4"
puts ""

puts "SUSPENDED SESSION: "
checkSuccess [$cur sendQuery "select * from testtable order by testint"] 1
$cur suspendResultSet 
checkSuccess [$con suspendSession] 1
set port [$con getConnectionPort]
set socket [$con getConnectionSocket]
checkSuccess [$con resumeSession $port $socket] 1
puts ""
checkSuccess [$cur getFieldByIndex 0 0] "1"
checkSuccess [$cur getFieldByIndex 1 0] "2"
checkSuccess [$cur getFieldByIndex 2 0] "3"
checkSuccess [$cur getFieldByIndex 3 0] "4"
checkSuccess [$cur getFieldByIndex 4 0] "5"
checkSuccess [$cur getFieldByIndex 5 0] "6"
checkSuccess [$cur getFieldByIndex 6 0] "7"
checkSuccess [$cur getFieldByIndex 7 0] "8"
puts ""
checkSuccess [$cur sendQuery "select * from testtable order by testint"] 1
$cur suspendResultSet 
checkSuccess [$con suspendSession] 1
set port [$con getConnectionPort]
set socket [$con getConnectionSocket]
checkSuccess [$con resumeSession $port $socket] 1
puts ""
checkSuccess [$cur getFieldByIndex 0 0] "1"
checkSuccess [$cur getFieldByIndex 1 0] "2"
checkSuccess [$cur getFieldByIndex 2 0] "3"
checkSuccess [$cur getFieldByIndex 3 0] "4"
checkSuccess [$cur getFieldByIndex 4 0] "5"
checkSuccess [$cur getFieldByIndex 5 0] "6"
checkSuccess [$cur getFieldByIndex 6 0] "7"
checkSuccess [$cur getFieldByIndex 7 0] "8"
puts ""
checkSuccess [$cur sendQuery "select * from testtable order by testint"] 1
$cur suspendResultSet 
checkSuccess [$con suspendSession] 1
set port [$con getConnectionPort]
set socket [$con getConnectionSocket]
checkSuccess [$con resumeSession $port $socket] 1
puts ""
checkSuccess [$cur getFieldByIndex 0 0] "1"
checkSuccess [$cur getFieldByIndex 1 0] "2"
checkSuccess [$cur getFieldByIndex 2 0] "3"
checkSuccess [$cur getFieldByIndex 3 0] "4"
checkSuccess [$cur getFieldByIndex 4 0] "5"
checkSuccess [$cur getFieldByIndex 5 0] "6"
checkSuccess [$cur getFieldByIndex 6 0] "7"
checkSuccess [$cur getFieldByIndex 7 0] "8"
puts ""

puts "SUSPENDED RESULT SET: "
$cur setResultSetBufferSize 2
checkSuccess [$cur sendQuery "select * from testtable order by testint"] 1
checkSuccess [$cur getFieldByIndex 2 0] "3"
set id [$cur getResultSetId]
$cur suspendResultSet 
checkSuccess [$con suspendSession] 1
set port [$con getConnectionPort]
set socket [$con getConnectionSocket]
checkSuccess [$con resumeSession $port $socket] 1
checkSuccess [$cur resumeResultSet $id] 1
puts ""
checkSuccess [$cur firstRowIndex] 4
checkSuccess [$cur endOfResultSet] 0
checkSuccess [$cur rowCount] 6
checkSuccess [$cur getFieldByIndex 7 0] "8"
puts ""
checkSuccess [$cur firstRowIndex] 6
checkSuccess [$cur endOfResultSet] 0
checkSuccess [$cur rowCount] 8
checkUndef [$cur getFieldByIndex 8 0]
puts ""
checkSuccess [$cur firstRowIndex] 8
checkSuccess [$cur endOfResultSet] 1
checkSuccess [$cur rowCount] 8
$cur setResultSetBufferSize 0
puts ""

puts "CACHED RESULT SET: "
$cur cacheToFile "cachefile1"
$cur setCacheTtl 200
checkSuccess [$cur sendQuery "select * from testtable order by testint"] 1
set filename [$cur getCacheFileName]
checkSuccess $filename "cachefile1"
$cur cacheOff 
checkSuccess [$cur openCachedResultSet $filename] 1
checkSuccess [$cur getFieldByIndex 7 0] "8"
puts ""

puts "COLUMN COUNT FOR CACHED RESULT SET: "
checkSuccess [$cur colCount] 9
puts ""

puts "COLUMN NAMES FOR CACHED RESULT SET: "
checkSuccess [$cur getColumnName 0] "testint"
checkSuccess [$cur getColumnName 1] "testfloat"
checkSuccess [$cur getColumnName 2] "testreal"
checkSuccess [$cur getColumnName 3] "testsmallint"
checkSuccess [$cur getColumnName 4] "testchar"
checkSuccess [$cur getColumnName 5] "testvarchar"
checkSuccess [$cur getColumnName 6] "testdate"
checkSuccess [$cur getColumnName 7] "testtime"
checkSuccess [$cur getColumnName 8] "testtimestamp"
set cols [$cur getColumnNames]
checkSuccess [lindex $cols 0] "testint"
checkSuccess [lindex $cols 1] "testfloat"
checkSuccess [lindex $cols 2] "testreal"
checkSuccess [lindex $cols 3] "testsmallint"
checkSuccess [lindex $cols 4] "testchar"
checkSuccess [lindex $cols 5] "testvarchar"
checkSuccess [lindex $cols 6] "testdate"
checkSuccess [lindex $cols 7] "testtime"
checkSuccess [lindex $cols 8] "testtimestamp"
puts ""

puts "CACHED RESULT SET WITH RESULT SET BUFFER SIZE: "
$cur setResultSetBufferSize 2
$cur cacheToFile "cachefile1"
$cur setCacheTtl 200
checkSuccess [$cur sendQuery "select * from testtable order by testint"] 1
set filename [$cur getCacheFileName]
checkSuccess $filename "cachefile1"
$cur cacheOff 
checkSuccess [$cur openCachedResultSet $filename] 1
checkSuccess [$cur getFieldByIndex 7 0] "8"
checkUndef [$cur getFieldByIndex 8 0]
$cur setResultSetBufferSize 0
puts ""

puts "FROM ONE CACHE FILE TO ANOTHER: "
$cur cacheToFile "cachefile2"
checkSuccess [$cur openCachedResultSet "cachefile1"] 1
$cur cacheOff 
checkSuccess [$cur openCachedResultSet "cachefile2"] 1
checkSuccess [$cur getFieldByIndex 7 0] "8"
checkUndef [$cur getFieldByIndex 8 0]
puts ""

puts "FROM ONE CACHE FILE TO ANOTHER WITH RESULT SET BUFFER SIZE: "
$cur setResultSetBufferSize 2
$cur cacheToFile "cachefile2"
checkSuccess [$cur openCachedResultSet "cachefile1"] 1
$cur cacheOff 
checkSuccess [$cur openCachedResultSet "cachefile2"] 1
checkSuccess [$cur getFieldByIndex 7 0] "8"
checkUndef [$cur getFieldByIndex 8 0]
$cur setResultSetBufferSize 0
puts ""

puts "CACHED RESULT SET WITH SUSPEND AND RESULT SET BUFFER SIZE: "
$cur setResultSetBufferSize 2
$cur cacheToFile "cachefile1"
$cur setCacheTtl 200
checkSuccess [$cur sendQuery "select * from testtable order by testint"] 1
checkSuccess [$cur getFieldByIndex 2 0] "3"
set filename [$cur getCacheFileName]
checkSuccess $filename "cachefile1"
set id [$cur getResultSetId]
$cur suspendResultSet 
checkSuccess [$con suspendSession] 1
set port [$con getConnectionPort]
set socket [$con getConnectionSocket]
puts ""
checkSuccess [$con resumeSession $port $socket] 1
checkSuccess [$cur resumeCachedResultSet $id $filename] 1
puts ""
checkSuccess [$cur firstRowIndex] 4
checkSuccess [$cur endOfResultSet] 0
checkSuccess [$cur rowCount] 6
checkSuccess [$cur getFieldByIndex 7 0] "8"
puts ""
checkSuccess [$cur firstRowIndex] 6
checkSuccess [$cur endOfResultSet] 0
checkSuccess [$cur rowCount] 8
checkUndef [$cur getFieldByIndex 8 0]
puts ""
checkSuccess [$cur firstRowIndex] 8
checkSuccess [$cur endOfResultSet] 1
checkSuccess [$cur rowCount] 8
$cur cacheOff 
puts ""
checkSuccess [$cur openCachedResultSet $filename] 1
checkSuccess [$cur getFieldByIndex 7 0] "8"
checkUndef [$cur getFieldByIndex 8 0]
$cur setResultSetBufferSize 0
puts ""

puts "COMMIT AND ROLLBACK: "
set secondcon [sqlrcon -server "localhost" -port 9000 -socket "/tmp/test.socket" -user "test" -password "test" -retrytime 0 -tries 1]
set secondcur [$secondcon sqlrcur]

checkSuccess [$secondcur sendQuery "select count(*) from testtable"] 1
checkSuccess [$secondcur getFieldByIndex 0 0] "0"
checkSuccess [$con commit] 1
checkSuccess [$secondcur sendQuery "select count(*) from testtable"] 1
checkSuccess [$secondcur getFieldByIndex 0 0] "8"
#checkSuccess [$con autoCommitOn] 1
checkSuccess [$cur sendQuery "insert into testtable values (10,10.1,10.1,10,'testchar10','testvarchar10','01/01/2010','10:00:00',NULL)"] 1
checkSuccess [$secondcur sendQuery "select count(*) from testtable"] 1
checkSuccess [$secondcur getFieldByIndex 0 0] "9"
#checkSuccess [$con autoCommitOff] 1
puts ""

puts "FINISHED SUSPENDED SESSION: "
checkSuccess [$cur sendQuery "select * from testtable order by testint"] 1
checkSuccess [$cur getFieldByIndex 4 0] "5"
checkSuccess [$cur getFieldByIndex 5 0] "6"
checkSuccess [$cur getFieldByIndex 6 0] "7"
checkSuccess [$cur getFieldByIndex 7 0] "8"
set id [$cur getResultSetId]
$cur suspendResultSet 
checkSuccess [$con suspendSession] 1
set port [$con getConnectionPort]
set socket [$con getConnectionSocket]
checkSuccess [$con resumeSession $port $socket] 1
checkSuccess [$cur resumeResultSet $id] 1
checkUndef [$cur getFieldByIndex 4 0]
checkUndef [$cur getFieldByIndex 5 0]
checkUndef [$cur getFieldByIndex 6 0]
checkUndef [$cur getFieldByIndex 7 0]
puts ""

# drop existing table
catch {$cur sendQuery "drop table testtable"}

puts "STORED PROCEDURES: "
catch {$cur sendQuery "drop function testfunc(int)"}
checkSuccess [$cur sendQuery "create function testfunc(int) returns int as ' begin return \$1; end;' language plpgsql"] 1
$cur prepareQuery "select * from testfunc(:int)"
$cur inputBind "int" 5
checkSuccess [$cur executeQuery] 1
checkSuccess [$cur getFieldByIndex 0 0] "5"
catch {$cur sendQuery "drop function testfunc(int)"}

catch {$cur sendQuery "drop function testfunc(int,char(20))"}
checkSuccess [$cur sendQuery "create function testfunc(int, char(20)) returns record as ' declare output record; begin select \$1,\$2 into output; return output; end;' language plpgsql"] 1
$cur prepareQuery "select * from testfunc(:int,:char) as (col1 int, col2 bpchar)"
$cur inputBind "int" 5
$cur inputBind "char" "hello"
checkSuccess [$cur executeQuery] 1
checkSuccess [$cur getFieldByIndex 0 0] "5"
checkSuccess [$cur getFieldByIndex 0 1] "hello"
catch {$cur sendQuery "drop function testfunc(int,char(20))"}
puts ""

# invalid queries...
puts "INVALID QUERIES: "
catch {checkSuccess [$cur sendQuery "select * from testtable order by testint"] 0}
catch {checkSuccess [$cur sendQuery "select * from testtable order by testint"] 0}
catch {checkSuccess [$cur sendQuery "select * from testtable order by testint"] 0}
catch {checkSuccess [$cur sendQuery "select * from testtable order by testint"] 0}
puts ""
catch {checkSuccess [$cur sendQuery "insert into testtable values (1,2,3,4)"] 0}
catch {checkSuccess [$cur sendQuery "insert into testtable values (1,2,3,4)"] 0}
catch {checkSuccess [$cur sendQuery "insert into testtable values (1,2,3,4)"] 0}
catch {checkSuccess [$cur sendQuery "insert into testtable values (1,2,3,4)"] 0}
puts ""
catch {checkSuccess [$cur sendQuery "create table testtable"] 0}
catch {checkSuccess [$cur sendQuery "create table testtable"] 0}
catch {checkSuccess [$cur sendQuery "create table testtable"] 0}
catch {checkSuccess [$cur sendQuery "create table testtable"] 0}
puts ""



