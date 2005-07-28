#! /usr/bin/tclsh

# Copyright] c 2001] David Muse
# See the file COPYING for more information.

load /usr/lib/sqlrelay/sqlrelay.so sqlrelay

proc checkUndef {value} {

	switch $value "" {
		puts "success "
	} default {
		puts "failure "
		exit
	}
}

proc checkSuccess {value success} {

	if {$value==$success} {
		puts "success "
	} else {
		puts "failure "
		exit
	}
}

# usage...
if {$argc<5} {
	puts "usage: msql.tcl host port socket user password"
	exit
}

# instantiation
set con [sqlrcon -server [lindex $argv 0] -port [lindex $argv 1] -socket [lindex $argv 2] -user [lindex $argv 3] -password [lindex $argv 4] -retrytime 0 -tries 1]
set cur [$con sqlrcur]

# get database type
puts "IDENTIFY: "
checkSuccess [$con identify] "msql"
puts ""

# ping
puts "PING: "
checkSuccess [$con ping] 1
puts ""

# drop existing table
catch {$cur sendQuery "drop table testtable"}

puts "CREATE TEMPTABLE: "
checkSuccess [$cur sendQuery "create table testtable (testchar char(40), testdate date, testint int, testmoney money, testreal real, testtext text(40), testtime time, testuint uint)"] 1
puts ""

puts "INSERT: "
checkSuccess [$cur sendQuery "insert into testtable values ('char1','01-Jan-2001',1,1.00,1.1,'text1','01:00:00',1)"] 1
checkSuccess [$cur sendQuery "insert into testtable values ('char2','01-Jan-2002',2,2.00,2.1,'text2','02:00:00',2)"] 1
checkSuccess [$cur sendQuery "insert into testtable values ('char3','01-Jan-2003',3,3.00,3.1,'text3','03:00:00',3)"] 1
checkSuccess [$cur sendQuery "insert into testtable values ('char4','01-Jan-2004',4,4.00,4.1,'text4','04:00:00',4)"] 1
puts ""

puts "AFFECTED ROWS: "
checkSuccess [$cur affectedRows] 0
puts ""

puts "BIND BY NAME: "
$cur prepareQuery "insert into testtable values (:var1,:var2,:var3,:var4,:var5,:var6,:var7,:var8)"
checkSuccess [$cur countBindVariables] 8
$cur inputBind "var1" "char5"
$cur inputBind "var2" "01-Jan-2005"
$cur inputBind "var3" 5
$cur inputBind "var4" 5.00 3 2
$cur inputBind "var5" 5.1 2 1
$cur inputBind "var6" "text5"
$cur inputBind "var7" "05:00:00"
$cur inputBind "var8" 5
checkSuccess [$cur executeQuery] 1
$cur clearBinds 
$cur inputBind "var1" "char6"
$cur inputBind "var2" "01-Jan-2006"
$cur inputBind "var3" 6
$cur inputBind "var4" 6.00 3 2
$cur inputBind "var5" 6.1 2 1
$cur inputBind "var6" "text6"
$cur inputBind "var7" "06:00:00"
$cur inputBind "var8" 6
checkSuccess [$cur executeQuery] 1
puts ""

puts "ARRAY OF BINDS BY NAME: "
$cur clearBinds 
$cur inputBinds {{"var1" "char7"} {"var2" "01-Jan-2007"} {"var3" 7} {"var4" 7.00 3 2} {"var5" 7.1 2 1} {"var6" "text7"} {"var7" "07:00:00"} {"var8" 7}}
checkSuccess [$cur executeQuery] 1
puts ""

puts "BIND BY NAME WITH VALIDATION: "
$cur clearBinds 
$cur inputBind "var1" "char8"
$cur inputBind "var2" "01-Jan-2008"
$cur inputBind "var3" 8
$cur inputBind "var4" 8.00 3 2
$cur inputBind "var5" 8.1 2 1
$cur inputBind "var6" "text8"
$cur inputBind "var7" "08:00:00"
$cur inputBind "var8" 8
$cur inputBind "var9" "junkvalue"
$cur validateBinds 
checkSuccess [$cur executeQuery] 1
puts ""

puts "SELECT: "
checkSuccess [$cur sendQuery "select * from testtable order by testint"] 1
puts ""

puts "COLUMN COUNT: "
checkSuccess [$cur colCount] 8
puts ""

puts "COLUMN NAMES: "
checkSuccess [$cur getColumnName 0] "testchar"
checkSuccess [$cur getColumnName 1] "testdate"
checkSuccess [$cur getColumnName 2] "testint"
checkSuccess [$cur getColumnName 3] "testmoney"
checkSuccess [$cur getColumnName 4] "testreal"
checkSuccess [$cur getColumnName 5] "testtext"
checkSuccess [$cur getColumnName 6] "testtime"
checkSuccess [$cur getColumnName 7] "testuint"
set cols [$cur getColumnNames]
checkSuccess [lindex $cols 0] "testchar"
checkSuccess [lindex $cols 1] "testdate"
checkSuccess [lindex $cols 2] "testint"
checkSuccess [lindex $cols 3] "testmoney"
checkSuccess [lindex $cols 4] "testreal"
checkSuccess [lindex $cols 5] "testtext"
checkSuccess [lindex $cols 6] "testtime"
checkSuccess [lindex $cols 7] "testuint"
puts ""

puts "COLUMN TYPES: "
checkSuccess [$cur getColumnTypeByIndex 0] "CHAR"
checkSuccess [$cur getColumnTypeByName "testchar"] "CHAR"
checkSuccess [$cur getColumnTypeByIndex 1] "DATE"
checkSuccess [$cur getColumnTypeByName "testdate"] "DATE"
checkSuccess [$cur getColumnTypeByIndex 2] "INT"
checkSuccess [$cur getColumnTypeByName "testint"] "INT"
checkSuccess [$cur getColumnTypeByIndex 3] "MONEY"
checkSuccess [$cur getColumnTypeByName "testmoney"] "MONEY"
checkSuccess [$cur getColumnTypeByIndex 4] "REAL"
checkSuccess [$cur getColumnTypeByName "testreal"] "REAL"
checkSuccess [$cur getColumnTypeByIndex 5] "TEXT"
checkSuccess [$cur getColumnTypeByName "testtext"] "TEXT"
checkSuccess [$cur getColumnTypeByIndex 6] "TIME"
checkSuccess [$cur getColumnTypeByName "testtime"] "TIME"
checkSuccess [$cur getColumnTypeByIndex 7] "UINT"
checkSuccess [$cur getColumnTypeByName "testuint"] "UINT"
puts ""

puts "COLUMN LENGTH: "
checkSuccess [$cur getColumnLengthByIndex 0] 40
checkSuccess [$cur getColumnLengthByName "testchar"] 40
checkSuccess [$cur getColumnLengthByIndex 1] 4
checkSuccess [$cur getColumnLengthByName "testdate"] 4
checkSuccess [$cur getColumnLengthByIndex 2] 4
checkSuccess [$cur getColumnLengthByName "testint"] 4
checkSuccess [$cur getColumnLengthByIndex 3] 4
checkSuccess [$cur getColumnLengthByName "testmoney"] 4
checkSuccess [$cur getColumnLengthByIndex 4] 8
checkSuccess [$cur getColumnLengthByName "testreal"] 8
checkSuccess [$cur getColumnLengthByIndex 5] 40
checkSuccess [$cur getColumnLengthByName "testtext"] 40
checkSuccess [$cur getColumnLengthByIndex 6] 4
checkSuccess [$cur getColumnLengthByName "testtime"] 4
checkSuccess [$cur getColumnLengthByIndex 7] 4
checkSuccess [$cur getColumnLengthByName "testuint"] 4
puts ""

puts "LONGEST COLUMN: "
checkSuccess [$cur getLongestByIndex 0] 5
checkSuccess [$cur getLongestByName "testchar"] 5
checkSuccess [$cur getLongestByIndex 1] 11
checkSuccess [$cur getLongestByName "testdate"] 11
checkSuccess [$cur getLongestByIndex 2] 1
checkSuccess [$cur getLongestByName "testint"] 1
checkSuccess [$cur getLongestByIndex 3] 4
checkSuccess [$cur getLongestByName "testmoney"] 4
checkSuccess [$cur getLongestByIndex 4] 3
checkSuccess [$cur getLongestByName "testreal"] 3
checkSuccess [$cur getLongestByIndex 5] 5
checkSuccess [$cur getLongestByName "testtext"] 5
checkSuccess [$cur getLongestByIndex 6] 8
checkSuccess [$cur getLongestByName "testtime"] 8
checkSuccess [$cur getLongestByIndex 7] 1
checkSuccess [$cur getLongestByName "testuint"] 1
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
checkSuccess [$cur getFieldByIndex 0 0] "char1"
checkSuccess [$cur getFieldByIndex 0 1] "01-Jan-2001"
checkSuccess [$cur getFieldByIndex 0 2] "1"
checkSuccess [$cur getFieldByIndex 0 3] "1.00"
checkSuccess [$cur getFieldByIndex 0 4] "1.1"
checkSuccess [$cur getFieldByIndex 0 5] "text1"
checkSuccess [$cur getFieldByIndex 0 6] "01:00:00"
checkSuccess [$cur getFieldByIndex 0 7] "1"
puts ""
checkSuccess [$cur getFieldByIndex 7 0] "char8"
checkSuccess [$cur getFieldByIndex 7 1] "01-Jan-2008"
checkSuccess [$cur getFieldByIndex 7 2] "8"
checkSuccess [$cur getFieldByIndex 7 3] "8.00"
checkSuccess [$cur getFieldByIndex 7 4] "8.1"
checkSuccess [$cur getFieldByIndex 7 5] "text8"
checkSuccess [$cur getFieldByIndex 7 6] "08:00:00"
checkSuccess [$cur getFieldByIndex 7 7] "8"
puts ""

puts "FIELD LENGTHS BY INDEX: "
checkSuccess [$cur getFieldLengthByIndex 0 0] 5
checkSuccess [$cur getFieldLengthByIndex 0 1] 11
checkSuccess [$cur getFieldLengthByIndex 0 2] 1
checkSuccess [$cur getFieldLengthByIndex 0 3] 4
checkSuccess [$cur getFieldLengthByIndex 0 4] 3
checkSuccess [$cur getFieldLengthByIndex 0 5] 5
checkSuccess [$cur getFieldLengthByIndex 0 6] 8
checkSuccess [$cur getFieldLengthByIndex 0 7] 1
puts ""
checkSuccess [$cur getFieldLengthByIndex 7 0] 5
checkSuccess [$cur getFieldLengthByIndex 7 1] 11
checkSuccess [$cur getFieldLengthByIndex 7 2] 1
checkSuccess [$cur getFieldLengthByIndex 7 3] 4
checkSuccess [$cur getFieldLengthByIndex 7 4] 3
checkSuccess [$cur getFieldLengthByIndex 7 5] 5
checkSuccess [$cur getFieldLengthByIndex 7 6] 8
checkSuccess [$cur getFieldLengthByIndex 7 7] 1
puts ""

puts "FIELDS BY NAME: "
checkSuccess [$cur getFieldByName 0 "testchar"] "char1"
checkSuccess [$cur getFieldByName 0 "testdate"] "01-Jan-2001"
checkSuccess [$cur getFieldByName 0 "testint"] "1"
checkSuccess [$cur getFieldByName 0 "testmoney"] "1.00"
checkSuccess [$cur getFieldByName 0 "testreal"] "1.1"
checkSuccess [$cur getFieldByName 0 "testtext"] "text1"
checkSuccess [$cur getFieldByName 0 "testtime"] "01:00:00"
checkSuccess [$cur getFieldByName 0 "testuint"] "1"
puts ""
checkSuccess [$cur getFieldByName 7 "testchar"] "char8"
checkSuccess [$cur getFieldByName 7 "testdate"] "01-Jan-2008"
checkSuccess [$cur getFieldByName 7 "testint"] "8"
checkSuccess [$cur getFieldByName 7 "testmoney"] "8.00"
checkSuccess [$cur getFieldByName 7 "testreal"] "8.1"
checkSuccess [$cur getFieldByName 7 "testtext"] "text8"
checkSuccess [$cur getFieldByName 7 "testtime"] "08:00:00"
checkSuccess [$cur getFieldByName 7 "testuint"] "8"
puts ""

puts "FIELD LENGTHS BY NAME: "
checkSuccess [$cur getFieldLengthByName 0 "testchar"] 5
checkSuccess [$cur getFieldLengthByName 0 "testdate"] 11
checkSuccess [$cur getFieldLengthByName 0 "testint"] 1
checkSuccess [$cur getFieldLengthByName 0 "testmoney"] 4
checkSuccess [$cur getFieldLengthByName 0 "testreal"] 3
checkSuccess [$cur getFieldLengthByName 0 "testtext"] 5
checkSuccess [$cur getFieldLengthByName 0 "testtime"] 8
checkSuccess [$cur getFieldLengthByName 0 "testuint"] 1
puts ""
checkSuccess [$cur getFieldLengthByName 7 "testchar"] 5
checkSuccess [$cur getFieldLengthByName 7 "testdate"] 11
checkSuccess [$cur getFieldLengthByName 7 "testint"] 1
checkSuccess [$cur getFieldLengthByName 7 "testmoney"] 4
checkSuccess [$cur getFieldLengthByName 7 "testreal"] 3
checkSuccess [$cur getFieldLengthByName 7 "testtext"] 5
checkSuccess [$cur getFieldLengthByName 7 "testtime"] 8
checkSuccess [$cur getFieldLengthByName 7 "testuint"] 1
puts ""

puts "FIELDS BY ARRAY: "
set fields [$cur getRow 0]
checkSuccess [lindex $fields 0] "char1"
checkSuccess [lindex $fields 1] "01-Jan-2001"
checkSuccess [lindex $fields 2] 1
checkSuccess [lindex $fields 3] 1.00
checkSuccess [lindex $fields 4] 1.1
checkSuccess [lindex $fields 5] "text1"
checkSuccess [lindex $fields 6] "01:00:00"
checkSuccess [lindex $fields 7] 1
puts ""

puts "FIELD LENGTHS BY ARRAY: "
set fieldlens [$cur getRowLengths 0]
checkSuccess [lindex $fieldlens 0] 5
checkSuccess [lindex $fieldlens 1] 11
checkSuccess [lindex $fieldlens 2] 1
checkSuccess [lindex $fieldlens 3] 4
checkSuccess [lindex $fieldlens 4] 3
checkSuccess [lindex $fieldlens 5] 5
checkSuccess [lindex $fieldlens 6] 8
checkSuccess [lindex $fieldlens 7] 1
puts ""

puts "INDIVIDUAL SUBSTITUTIONS: "
catch {$cur sendQuery "drop table testtable1"}
checkSuccess [$cur sendQuery "create table testtable1 (col1 int, col2 char(40), col3 real)"] 1
$cur prepareQuery "insert into testtable1 values (\$(var1),'\$(var2)',\$(var3))"
$cur substitution "var1" 1
$cur substitution "var2" "hello"
$cur substitution "var3" 10.5556 6 4
checkSuccess [$cur executeQuery] 1
puts ""

puts "FIELDS: "
checkSuccess [$cur sendQuery "select * from testtable1"] 1
checkSuccess [$cur getFieldByIndex 0 0] "1"
checkSuccess [$cur getFieldByIndex 0 1] "hello"
checkSuccess [$cur getFieldByIndex 0 2] "10.5556"
checkSuccess [$cur sendQuery "delete from testtable1"] 1
puts ""

puts "ARRAY SUBSTITUTIONS: "
$cur prepareQuery "insert into testtable1 values (\$(var1),'\$(var2)',\$(var3))"
$cur substitutions {{"var1" 1} {"var2" "hello"} {"var3" 10.5556 6 4}}
checkSuccess [$cur executeQuery] 1
puts ""

puts "FIELDS: "
checkSuccess [$cur sendQuery "select * from testtable1"] 1
checkSuccess [$cur getFieldByIndex 0 0] "1"
checkSuccess [$cur getFieldByIndex 0 1] "hello"
checkSuccess [$cur getFieldByIndex 0 2] "10.5556"
checkSuccess [$cur sendQuery "delete from testtable1"] 1
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
checkSuccess [$cur getFieldByIndex 0 0] "char1"
checkSuccess [$cur getFieldByIndex 1 0] "char2"
checkSuccess [$cur getFieldByIndex 2 0] "char3"
puts ""
checkSuccess [$cur firstRowIndex] 2
checkSuccess [$cur endOfResultSet] 0
checkSuccess [$cur rowCount] 4
checkSuccess [$cur getFieldByIndex 6 0] "char7"
checkSuccess [$cur getFieldByIndex 7 0] "char8"
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
checkSuccess [$cur getColumnName 0] "testchar"
checkSuccess [$cur getColumnLengthByIndex 0] 40
checkSuccess [$cur getColumnTypeByIndex 0] "CHAR"
puts ""

puts "SUSPENDED SESSION: "
checkSuccess [$cur sendQuery "select * from testtable order by testint"] 1
$cur suspendResultSet 
checkSuccess [$con suspendSession] 1
set port [$con getConnectionPort]
set socket [$con getConnectionSocket]
checkSuccess [$con resumeSession $port $socket] 1
puts ""
checkSuccess [$cur getFieldByIndex 0 0] "char1"
checkSuccess [$cur getFieldByIndex 1 0] "char2"
checkSuccess [$cur getFieldByIndex 2 0] "char3"
checkSuccess [$cur getFieldByIndex 3 0] "char4"
checkSuccess [$cur getFieldByIndex 4 0] "char5"
checkSuccess [$cur getFieldByIndex 5 0] "char6"
checkSuccess [$cur getFieldByIndex 6 0] "char7"
checkSuccess [$cur getFieldByIndex 7 0] "char8"
puts ""
checkSuccess [$cur sendQuery "select * from testtable order by testint"] 1
$cur suspendResultSet 
checkSuccess [$con suspendSession] 1
set port [$con getConnectionPort]
set socket [$con getConnectionSocket]
checkSuccess [$con resumeSession $port $socket] 1
puts ""
checkSuccess [$cur getFieldByIndex 0 0] "char1"
checkSuccess [$cur getFieldByIndex 1 0] "char2"
checkSuccess [$cur getFieldByIndex 2 0] "char3"
checkSuccess [$cur getFieldByIndex 3 0] "char4"
checkSuccess [$cur getFieldByIndex 4 0] "char5"
checkSuccess [$cur getFieldByIndex 5 0] "char6"
checkSuccess [$cur getFieldByIndex 6 0] "char7"
checkSuccess [$cur getFieldByIndex 7 0] "char8"
puts ""
checkSuccess [$cur sendQuery "select * from testtable order by testint"] 1
$cur suspendResultSet 
checkSuccess [$con suspendSession] 1
set port [$con getConnectionPort]
set socket [$con getConnectionSocket]
checkSuccess [$con resumeSession $port $socket] 1
puts ""
checkSuccess [$cur getFieldByIndex 0 0] "char1"
checkSuccess [$cur getFieldByIndex 1 0] "char2"
checkSuccess [$cur getFieldByIndex 2 0] "char3"
checkSuccess [$cur getFieldByIndex 3 0] "char4"
checkSuccess [$cur getFieldByIndex 4 0] "char5"
checkSuccess [$cur getFieldByIndex 5 0] "char6"
checkSuccess [$cur getFieldByIndex 6 0] "char7"
checkSuccess [$cur getFieldByIndex 7 0] "char8"
puts ""

puts "SUSPENDED RESULT SET: "
$cur setResultSetBufferSize 2
checkSuccess [$cur sendQuery "select * from testtable order by testint"] 1
checkSuccess [$cur getFieldByIndex 2 0] "char3"
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
checkSuccess [$cur getFieldByIndex 7 0] "char8"
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
checkSuccess [$cur getFieldByIndex 7 0] "char8"
puts ""

puts "COLUMN COUNT FOR CACHED RESULT SET: "
checkSuccess [$cur colCount] 8
puts ""

puts "COLUMN NAMES FOR CACHED RESULT SET: "
checkSuccess [$cur getColumnName 0] "testchar"
checkSuccess [$cur getColumnName 1] "testdate"
checkSuccess [$cur getColumnName 2] "testint"
checkSuccess [$cur getColumnName 3] "testmoney"
checkSuccess [$cur getColumnName 4] "testreal"
checkSuccess [$cur getColumnName 5] "testtext"
checkSuccess [$cur getColumnName 6] "testtime"
checkSuccess [$cur getColumnName 7] "testuint"
set cols [$cur getColumnNames]
checkSuccess [lindex $cols 0] "testchar"
checkSuccess [lindex $cols 1] "testdate"
checkSuccess [lindex $cols 2] "testint"
checkSuccess [lindex $cols 3] "testmoney"
checkSuccess [lindex $cols 4] "testreal"
checkSuccess [lindex $cols 5] "testtext"
checkSuccess [lindex $cols 6] "testtime"
checkSuccess [lindex $cols 7] "testuint"
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
checkSuccess [$cur getFieldByIndex 7 0] "char8"
checkUndef [$cur getFieldByIndex 8 0]
$cur setResultSetBufferSize 0
puts ""

puts "FROM ONE CACHE FILE TO ANOTHER: "
$cur cacheToFile "cachefile2"
checkSuccess [$cur openCachedResultSet "cachefile1"] 1
$cur cacheOff 
checkSuccess [$cur openCachedResultSet "cachefile2"] 1
checkSuccess [$cur getFieldByIndex 7 0] "char8"
checkUndef [$cur getFieldByIndex 8 0]
puts ""

puts "FROM ONE CACHE FILE TO ANOTHER WITH RESULT SET BUFFER SIZE: "
$cur setResultSetBufferSize 2
$cur cacheToFile "cachefile2"
checkSuccess [$cur openCachedResultSet "cachefile1"] 1
$cur cacheOff 
checkSuccess [$cur openCachedResultSet "cachefile2"] 1
checkSuccess [$cur getFieldByIndex 7 0] "char8"
checkUndef [$cur getFieldByIndex 8 0]
$cur setResultSetBufferSize 0
puts ""

puts "CACHED RESULT SET WITH SUSPEND AND RESULT SET BUFFER SIZE: "
$cur setResultSetBufferSize 2
$cur cacheToFile "cachefile1"
$cur setCacheTtl 200
checkSuccess [$cur sendQuery "select * from testtable order by testint"] 1
checkSuccess [$cur getFieldByIndex 2 0] "char3"
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
checkSuccess [$cur getFieldByIndex 7 0] "char8"
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
checkSuccess [$cur getFieldByIndex 7 0] "char8"
checkUndef [$cur getFieldByIndex 8 0]
$cur setResultSetBufferSize 0
puts ""

puts "COMMIT AND ROLLBACK: "
set secondcon [sqlrcon -server [lindex $argv 0] -port [lindex $argv 1] -socket [lindex $argv 2] -user [lindex $argv 3] -password [lindex $argv 4] -retrytime 0 -tries 1]
set secondcur [$secondcon sqlrcur]
checkSuccess [$secondcur sendQuery "select * from testtable order by testint"] 1
checkSuccess [$secondcur getFieldByIndex 0 0] "char1"
checkSuccess [$con commit] 1
checkSuccess [$secondcur sendQuery "select * from testtable order by testint"] 1
checkSuccess [$secondcur getFieldByIndex 0 0] "char1"
checkSuccess [$con autoCommit 1] 1
checkSuccess [$cur sendQuery "insert into testtable values ('char10','01-Jan-2010',10,10.00,10.1,'text10','10:00:00',10)"] 1
checkSuccess [$secondcur sendQuery "select * from testtable order by testint"] 1
checkSuccess [$secondcur getFieldByIndex 8 0] "char10"
checkSuccess [$con autoCommit 0] 1
puts ""

puts "FINISHED SUSPENDED SESSION: "
checkSuccess [$cur sendQuery "select * from testtable order by testint"] 1
checkSuccess [$cur getFieldByIndex 4 2] "5"
checkSuccess [$cur getFieldByIndex 5 2] "6"
checkSuccess [$cur getFieldByIndex 6 2] "7"
checkSuccess [$cur getFieldByIndex 7 2] "8"
set id [$cur getResultSetId]
$cur suspendResultSet 
checkSuccess [$con suspendSession] 1
set port [$con getConnectionPort]
set socket [$con getConnectionSocket]
checkSuccess [$con resumeSession $port $socket] 1
checkSuccess [$cur resumeResultSet $id] 1
checkUndef [$cur getFieldByIndex 4 2]
checkUndef [$cur getFieldByIndex 5 2]
checkUndef [$cur getFieldByIndex 6 2]
checkUndef [$cur getFieldByIndex 7 2]
puts ""

# drop existing table
catch {$cur sendQuery "drop table testtable"}

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



