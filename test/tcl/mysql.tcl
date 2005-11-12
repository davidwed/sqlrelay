#! /usr/bin/tclsh

# Copyright] c] 2001] David Muse
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
	puts "usage: mysql.tcl host port socket user password"
	exit
}

# instantiation
set con [sqlrcon -server [lindex $argv 0] -port [lindex $argv 1] -socket [lindex $argv 2] -user [lindex $argv 3] -password [lindex $argv 4] -retrytime 0 -tries 1]
set cur [$con sqlrcur]

# get database type
puts "IDENTIFY: "
checkSuccess [$con identify] "mysql"

# ping
puts "PING: "
checkSuccess [$con ping] 1
puts ""

# drop existing table
catch {$cur sendQuery "drop table testtable"}

# create a new table
puts "CREATE TEMPTABLE: "
checkSuccess [$cur sendQuery "create table testdb.testtable (testtinyint tinyint, testsmallint smallint, testmediumint mediumint, testint int, testbigint bigint, testfloat float, testreal real, testdecimal decimal(1,1), testdate date, testtime time, testdatetime datetime, testyear year, testchar char(40), testtext text, testvarchar varchar(40), testtinytext tinytext, testmediumtext mediumtext, testlongtext longtext, testtimestamp timestamp)"] 1
puts ""

puts "INSERT: "
checkSuccess [$cur sendQuery "insert into testdb.testtable values (1,1,1,1,1,1.1,1.1,1.1,'2001-01-01','01:00:00','2001-01-01,01:00:00','2001','char1','text1','varchar1','tinytext1','mediumtext1','longtext1',NULL)"] 1
checkSuccess [$cur sendQuery "insert into testdb.testtable values (2,2,2,2,2,2.1,2.1,2.1,'2002-01-01','02:00:00','2002-01-01,02:00:00','2002','char2','text2','varchar2','tinytext2','mediumtext2','longtext2',NULL)"] 1
checkSuccess [$cur sendQuery "insert into testdb.testtable values (3,3,3,3,3,3.1,3.1,3.1,'2003-01-01','03:00:00','2003-01-01,03:00:00','2003','char3','text3','varchar3','tinytext3','mediumtext3','longtext3',NULL)"] 1
checkSuccess [$cur sendQuery "insert into testdb.testtable values (4,4,4,4,4,4.1,4.1,4.1,'2004-01-01','04:00:00','2004-01-01,04:00:00','2004','char4','text4','varchar4','tinytext4','mediumtext4','longtext4',NULL)"] 1
puts ""

puts "AFFECTED ROWS: "
checkSuccess [$cur affectedRows] 1
puts ""

puts "BIND BY NAME: "
$cur prepareQuery "insert into testdb.testtable values (:var1,:var2,:var3,:var4,:var5,:var6,:var7,:var8,:var9,:var10,:var11,:var12,:var13,:var14,:var15,:var16,:var17,:var18,NULL)"
checkSuccess [$cur countBindVariables] 18
$cur inputBind "var1" 5
$cur inputBind "var2" 5
$cur inputBind "var3" 5
$cur inputBind "var4" 5
$cur inputBind "var5" 5
$cur inputBind "var6" 5.1 2 1
$cur inputBind "var7" 5.1 2 1
$cur inputBind "var8" 5.1 2 1
$cur inputBind "var9" "2005-01-01"
$cur inputBind "var10" "05:00:00"
$cur inputBind "var11" "2005-01-01 05:00:00"
$cur inputBind "var12" "2005"
$cur inputBind "var13" "char5"
$cur inputBind "var14" "text5"
$cur inputBind "var15" "varchar5"
$cur inputBind "var16" "tinytext5"
$cur inputBind "var17" "mediumtext5"
$cur inputBind "var18" "longtext5"
checkSuccess [$cur executeQuery] 1
$cur clearBinds 
$cur inputBind "var1" 6
$cur inputBind "var2" 6
$cur inputBind "var3" 6
$cur inputBind "var4" 6
$cur inputBind "var5" 6
$cur inputBind "var6" 6.1 2 1
$cur inputBind "var7" 6.1 2 1
$cur inputBind "var8" 6.1 2 1
$cur inputBind "var9" "2006-01-01"
$cur inputBind "var10" "06:00:00"
$cur inputBind "var11" "2006-01-01 06:00:00"
$cur inputBind "var12" "2006"
$cur inputBind "var13" "char6"
$cur inputBind "var14" "text6"
$cur inputBind "var15" "varchar6"
$cur inputBind "var16" "tinytext6"
$cur inputBind "var17" "mediumtext6"
$cur inputBind "var18" "longtext6"
checkSuccess [$cur executeQuery] 1
puts ""

puts "ARRAY OF BINDS BY NAME: "
$cur clearBinds 
$cur inputBinds {{"var1" 7} {"var2" 7} {"var3" 7} {"var4" 7} {"var5" 7} {"var6" 7.1 2 1} {"var7" 7.1 2 1} {"var8" 7.1 2 1} {"var9" "2007-01-01"} {"var10" "07:00:00"} {"var11" "2007-01-01 07:00:00"} {"var12" "2007"} {"var13" "char7"} {"var14" "text7"} {"var15" "varchar7"} {"var16" "tinytext7"} {"var17" "mediumtext7"} {"var18" "longtext7"}}
checkSuccess [$cur executeQuery] 1
puts ""

puts "BIND BY NAME WITH VALIDATION: "
$cur clearBinds 
$cur inputBind "var1" 8
$cur inputBind "var2" 8
$cur inputBind "var3" 8
$cur inputBind "var4" 8
$cur inputBind "var5" 8
$cur inputBind "var6" 8.1 2 1
$cur inputBind "var7" 8.1 2 1
$cur inputBind "var8" 8.1 2 1
$cur inputBind "var9" "2008-01-01"
$cur inputBind "var10" "08:00:00"
$cur inputBind "var11" "2008-01-01 08:00:00"
$cur inputBind "var12" "2008"
$cur inputBind "var13" "char8"
$cur inputBind "var14" "text8"
$cur inputBind "var15" "varchar8"
$cur inputBind "var16" "tinytext8"
$cur inputBind "var17" "mediumtext8"
$cur inputBind "var18" "longtext8"
$cur validateBinds 
checkSuccess [$cur executeQuery] 1
puts ""

puts "SELECT: "
checkSuccess [$cur sendQuery "select * from testtable order by testtinyint"] 1
puts ""

puts "COLUMN COUNT: "
checkSuccess [$cur colCount] 19
puts ""

puts "COLUMN NAMES: "
checkSuccess [$cur getColumnName 0] "testtinyint"
checkSuccess [$cur getColumnName 1] "testsmallint"
checkSuccess [$cur getColumnName 2] "testmediumint"
checkSuccess [$cur getColumnName 3] "testint"
checkSuccess [$cur getColumnName 4] "testbigint"
checkSuccess [$cur getColumnName 5] "testfloat"
checkSuccess [$cur getColumnName 6] "testreal"
checkSuccess [$cur getColumnName 7] "testdecimal"
checkSuccess [$cur getColumnName 8] "testdate"
checkSuccess [$cur getColumnName 9] "testtime"
checkSuccess [$cur getColumnName 10] "testdatetime"
checkSuccess [$cur getColumnName 11] "testyear"
checkSuccess [$cur getColumnName 12] "testchar"
checkSuccess [$cur getColumnName 13] "testtext"
checkSuccess [$cur getColumnName 14] "testvarchar"
checkSuccess [$cur getColumnName 15] "testtinytext"
checkSuccess [$cur getColumnName 16] "testmediumtext"
checkSuccess [$cur getColumnName 17] "testlongtext"
checkSuccess [$cur getColumnName 18] "testtimestamp"
set cols [$cur getColumnNames]
checkSuccess [lindex $cols 0] "testtinyint"
checkSuccess [lindex $cols 1] "testsmallint"
checkSuccess [lindex $cols 2] "testmediumint"
checkSuccess [lindex $cols 3] "testint"
checkSuccess [lindex $cols 4] "testbigint"
checkSuccess [lindex $cols 5] "testfloat"
checkSuccess [lindex $cols 6] "testreal"
checkSuccess [lindex $cols 7] "testdecimal"
checkSuccess [lindex $cols 8] "testdate"
checkSuccess [lindex $cols 9] "testtime"
checkSuccess [lindex $cols 10] "testdatetime"
checkSuccess [lindex $cols 11] "testyear"
checkSuccess [lindex $cols 12] "testchar"
checkSuccess [lindex $cols 13] "testtext"
checkSuccess [lindex $cols 14] "testvarchar"
checkSuccess [lindex $cols 15] "testtinytext"
checkSuccess [lindex $cols 16] "testmediumtext"
checkSuccess [lindex $cols 17] "testlongtext"
checkSuccess [lindex $cols 18] "testtimestamp"
puts ""

puts "COLUMN TYPES: "
checkSuccess [$cur getColumnTypeByIndex 0] "TINYINT"
checkSuccess [$cur getColumnTypeByIndex 1] "SMALLINT"
checkSuccess [$cur getColumnTypeByIndex 2] "MEDIUMINT"
checkSuccess [$cur getColumnTypeByIndex 3] "INT"
checkSuccess [$cur getColumnTypeByIndex 4] "BIGINT"
checkSuccess [$cur getColumnTypeByIndex 5] "FLOAT"
checkSuccess [$cur getColumnTypeByIndex 6] "REAL"
checkSuccess [$cur getColumnTypeByIndex 7] "DECIMAL"
checkSuccess [$cur getColumnTypeByIndex 8] "DATE"
checkSuccess [$cur getColumnTypeByIndex 9] "TIME"
checkSuccess [$cur getColumnTypeByIndex 10] "DATETIME"
checkSuccess [$cur getColumnTypeByIndex 11] "YEAR"
checkSuccess [$cur getColumnTypeByIndex 12] "CHAR"
checkSuccess [$cur getColumnTypeByIndex 13] "BLOB"
checkSuccess [$cur getColumnTypeByIndex 14] "CHAR"
checkSuccess [$cur getColumnTypeByIndex 15] "TINYBLOB"
checkSuccess [$cur getColumnTypeByIndex 16] "MEDIUMBLOB"
checkSuccess [$cur getColumnTypeByIndex 17] "LONGBLOB"
checkSuccess [$cur getColumnTypeByIndex 18] "TIMESTAMP"
checkSuccess [$cur getColumnTypeByName "testtinyint"] "TINYINT"
checkSuccess [$cur getColumnTypeByName "testsmallint"] "SMALLINT"
checkSuccess [$cur getColumnTypeByName "testmediumint"] "MEDIUMINT"
checkSuccess [$cur getColumnTypeByName "testint"] "INT"
checkSuccess [$cur getColumnTypeByName "testbigint"] "BIGINT"
checkSuccess [$cur getColumnTypeByName "testfloat"] "FLOAT"
checkSuccess [$cur getColumnTypeByName "testreal"] "REAL"
checkSuccess [$cur getColumnTypeByName "testdecimal"] "DECIMAL"
checkSuccess [$cur getColumnTypeByName "testdate"] "DATE"
checkSuccess [$cur getColumnTypeByName "testtime"] "TIME"
checkSuccess [$cur getColumnTypeByName "testdatetime"] "DATETIME"
checkSuccess [$cur getColumnTypeByName "testyear"] "YEAR"
checkSuccess [$cur getColumnTypeByName "testchar"] "CHAR"
checkSuccess [$cur getColumnTypeByName "testtext"] "BLOB"
checkSuccess [$cur getColumnTypeByName "testvarchar"] "CHAR"
checkSuccess [$cur getColumnTypeByName "testtinytext"] "TINYBLOB"
checkSuccess [$cur getColumnTypeByName "testmediumtext"] "MEDIUMBLOB"
checkSuccess [$cur getColumnTypeByName "testlongtext"] "LONGBLOB"
checkSuccess [$cur getColumnTypeByName "testtimestamp"] "TIMESTAMP"
puts ""

puts "COLUMN LENGTH: "
checkSuccess [$cur getColumnLengthByIndex 0] 1
checkSuccess [$cur getColumnLengthByIndex 1] 2
checkSuccess [$cur getColumnLengthByIndex 2] 3
checkSuccess [$cur getColumnLengthByIndex 3] 4
checkSuccess [$cur getColumnLengthByIndex 4] 8
checkSuccess [$cur getColumnLengthByIndex 5] 4
checkSuccess [$cur getColumnLengthByIndex 6] 8
checkSuccess [$cur getColumnLengthByIndex 7] 6
checkSuccess [$cur getColumnLengthByIndex 8] 3
checkSuccess [$cur getColumnLengthByIndex 9] 3
checkSuccess [$cur getColumnLengthByIndex 10] 8
checkSuccess [$cur getColumnLengthByIndex 11] 1
checkSuccess [$cur getColumnLengthByIndex 12] 41
checkSuccess [$cur getColumnLengthByIndex 13] 65535
checkSuccess [$cur getColumnLengthByIndex 14] 41
checkSuccess [$cur getColumnLengthByIndex 15] 255
checkSuccess [$cur getColumnLengthByIndex 16] 16777215
checkSuccess [$cur getColumnLengthByIndex 17] 2147483647
checkSuccess [$cur getColumnLengthByIndex 18] 4
checkSuccess [$cur getColumnLengthByName "testtinyint"] 1
checkSuccess [$cur getColumnLengthByName "testsmallint"] 2
checkSuccess [$cur getColumnLengthByName "testmediumint"] 3
checkSuccess [$cur getColumnLengthByName "testint"] 4
checkSuccess [$cur getColumnLengthByName "testbigint"] 8
checkSuccess [$cur getColumnLengthByName "testfloat"] 4
checkSuccess [$cur getColumnLengthByName "testreal"] 8
checkSuccess [$cur getColumnLengthByName "testdecimal"] 6
checkSuccess [$cur getColumnLengthByName "testdate"] 3
checkSuccess [$cur getColumnLengthByName "testtime"] 3
checkSuccess [$cur getColumnLengthByName "testdatetime"] 8
checkSuccess [$cur getColumnLengthByName "testyear"] 1
checkSuccess [$cur getColumnLengthByName "testchar"] 41
checkSuccess [$cur getColumnLengthByName "testtext"] 65535
checkSuccess [$cur getColumnLengthByName "testvarchar"] 41
checkSuccess [$cur getColumnLengthByName "testtinytext"] 255
checkSuccess [$cur getColumnLengthByName "testmediumtext"] 16777215
checkSuccess [$cur getColumnLengthByName "testlongtext"] 2147483647
checkSuccess [$cur getColumnLengthByName "testtimestamp"] 4
puts ""

puts "LONGEST COLUMN: "
checkSuccess [$cur getLongestByIndex 0] 1
checkSuccess [$cur getLongestByIndex 1] 1
checkSuccess [$cur getLongestByIndex 2] 1
checkSuccess [$cur getLongestByIndex 3] 1
checkSuccess [$cur getLongestByIndex 4] 1
checkSuccess [$cur getLongestByIndex 5] 3
checkSuccess [$cur getLongestByIndex 6] 3
checkSuccess [$cur getLongestByIndex 7] 3
checkSuccess [$cur getLongestByIndex 8] 10
checkSuccess [$cur getLongestByIndex 9] 8
checkSuccess [$cur getLongestByIndex 10] 19
checkSuccess [$cur getLongestByIndex 11] 4
checkSuccess [$cur getLongestByIndex 12] 5
checkSuccess [$cur getLongestByIndex 13] 5
checkSuccess [$cur getLongestByIndex 14] 8
checkSuccess [$cur getLongestByIndex 15] 9
checkSuccess [$cur getLongestByIndex 16] 11
checkSuccess [$cur getLongestByIndex 17] 9
checkSuccess [$cur getLongestByIndex 18] 19
checkSuccess [$cur getLongestByName "testtinyint"] 1
checkSuccess [$cur getLongestByName "testsmallint"] 1
checkSuccess [$cur getLongestByName "testmediumint"] 1
checkSuccess [$cur getLongestByName "testint"] 1
checkSuccess [$cur getLongestByName "testbigint"] 1
checkSuccess [$cur getLongestByName "testfloat"] 3
checkSuccess [$cur getLongestByName "testreal"] 3
checkSuccess [$cur getLongestByName "testdecimal"] 3
checkSuccess [$cur getLongestByName "testdate"] 10
checkSuccess [$cur getLongestByName "testtime"] 8
checkSuccess [$cur getLongestByName "testdatetime"] 19
checkSuccess [$cur getLongestByName "testyear"] 4
checkSuccess [$cur getLongestByName "testchar"] 5
checkSuccess [$cur getLongestByName "testtext"] 5
checkSuccess [$cur getLongestByName "testvarchar"] 8
checkSuccess [$cur getLongestByName "testtinytext"] 9
checkSuccess [$cur getLongestByName "testmediumtext"] 11
checkSuccess [$cur getLongestByName "testlongtext"] 9
checkSuccess [$cur getLongestByName "testtimestamp"] 19
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
checkSuccess [$cur getFieldByIndex 0 1] "1"
checkSuccess [$cur getFieldByIndex 0 2] "1"
checkSuccess [$cur getFieldByIndex 0 3] "1"
checkSuccess [$cur getFieldByIndex 0 4] "1"
checkSuccess [$cur getFieldByIndex 0 5] "1.1"
checkSuccess [$cur getFieldByIndex 0 6] "1.1"
checkSuccess [$cur getFieldByIndex 0 7] "1.1"
checkSuccess [$cur getFieldByIndex 0 8] "2001-01-01"
checkSuccess [$cur getFieldByIndex 0 9] "01:00:00"
checkSuccess [$cur getFieldByIndex 0 10] "2001-01-01 01:00:00"
checkSuccess [$cur getFieldByIndex 0 11] "2001"
checkSuccess [$cur getFieldByIndex 0 12] "char1"
checkSuccess [$cur getFieldByIndex 0 13] "text1"
checkSuccess [$cur getFieldByIndex 0 14] "varchar1"
checkSuccess [$cur getFieldByIndex 0 15] "tinytext1"
checkSuccess [$cur getFieldByIndex 0 16] "mediumtext1"
checkSuccess [$cur getFieldByIndex 0 17] "longtext1"
puts ""
checkSuccess [$cur getFieldByIndex 7 0] "8"
checkSuccess [$cur getFieldByIndex 7 1] "8"
checkSuccess [$cur getFieldByIndex 7 2] "8"
checkSuccess [$cur getFieldByIndex 7 3] "8"
checkSuccess [$cur getFieldByIndex 7 4] "8"
checkSuccess [$cur getFieldByIndex 7 5] "8.1"
checkSuccess [$cur getFieldByIndex 7 6] "8.1"
checkSuccess [$cur getFieldByIndex 7 7] "8.1"
checkSuccess [$cur getFieldByIndex 7 8] "2008-01-01"
checkSuccess [$cur getFieldByIndex 7 9] "08:00:00"
checkSuccess [$cur getFieldByIndex 7 10] "2008-01-01 08:00:00"
checkSuccess [$cur getFieldByIndex 7 11] "2008"
checkSuccess [$cur getFieldByIndex 7 12] "char8"
checkSuccess [$cur getFieldByIndex 7 13] "text8"
checkSuccess [$cur getFieldByIndex 7 14] "varchar8"
checkSuccess [$cur getFieldByIndex 7 15] "tinytext8"
checkSuccess [$cur getFieldByIndex 7 16] "mediumtext8"
checkSuccess [$cur getFieldByIndex 7 17] "longtext8"
puts ""

puts "FIELD LENGTHS BY INDEX: "
checkSuccess [$cur getFieldLengthByIndex 0 0] 1
checkSuccess [$cur getFieldLengthByIndex 0 1] 1
checkSuccess [$cur getFieldLengthByIndex 0 2] 1
checkSuccess [$cur getFieldLengthByIndex 0 3] 1
checkSuccess [$cur getFieldLengthByIndex 0 4] 1
checkSuccess [$cur getFieldLengthByIndex 0 5] 3
checkSuccess [$cur getFieldLengthByIndex 0 6] 3
checkSuccess [$cur getFieldLengthByIndex 0 7] 3
checkSuccess [$cur getFieldLengthByIndex 0 8] 10
checkSuccess [$cur getFieldLengthByIndex 0 9] 8
checkSuccess [$cur getFieldLengthByIndex 0 10] 19
checkSuccess [$cur getFieldLengthByIndex 0 11] 4
checkSuccess [$cur getFieldLengthByIndex 0 12] 5
checkSuccess [$cur getFieldLengthByIndex 0 13] 5
checkSuccess [$cur getFieldLengthByIndex 0 14] 8
checkSuccess [$cur getFieldLengthByIndex 0 15] 9
checkSuccess [$cur getFieldLengthByIndex 0 16] 11
checkSuccess [$cur getFieldLengthByIndex 0 17] 9
puts ""
checkSuccess [$cur getFieldLengthByIndex 7 0] 1
checkSuccess [$cur getFieldLengthByIndex 7 1] 1
checkSuccess [$cur getFieldLengthByIndex 7 2] 1
checkSuccess [$cur getFieldLengthByIndex 7 3] 1
checkSuccess [$cur getFieldLengthByIndex 7 4] 1
checkSuccess [$cur getFieldLengthByIndex 7 5] 3
checkSuccess [$cur getFieldLengthByIndex 7 6] 3
checkSuccess [$cur getFieldLengthByIndex 7 7] 3
checkSuccess [$cur getFieldLengthByIndex 7 8] 10
checkSuccess [$cur getFieldLengthByIndex 7 9] 8
checkSuccess [$cur getFieldLengthByIndex 7 10] 19
checkSuccess [$cur getFieldLengthByIndex 7 11] 4
checkSuccess [$cur getFieldLengthByIndex 7 12] 5
checkSuccess [$cur getFieldLengthByIndex 7 13] 5
checkSuccess [$cur getFieldLengthByIndex 7 14] 8
checkSuccess [$cur getFieldLengthByIndex 7 15] 9
checkSuccess [$cur getFieldLengthByIndex 7 16] 11
checkSuccess [$cur getFieldLengthByIndex 7 17] 9
puts ""

puts "FIELDS BY NAME: "
checkSuccess [$cur getFieldByName 0 "testtinyint"] "1"
checkSuccess [$cur getFieldByName 0 "testsmallint"] "1"
checkSuccess [$cur getFieldByName 0 "testmediumint"] "1"
checkSuccess [$cur getFieldByName 0 "testint"] "1"
checkSuccess [$cur getFieldByName 0 "testbigint"] "1"
checkSuccess [$cur getFieldByName 0 "testfloat"] "1.1"
checkSuccess [$cur getFieldByName 0 "testreal"] "1.1"
checkSuccess [$cur getFieldByName 0 "testdecimal"] "1.1"
checkSuccess [$cur getFieldByName 0 "testdate"] "2001-01-01"
checkSuccess [$cur getFieldByName 0 "testtime"] "01:00:00"
checkSuccess [$cur getFieldByName 0 "testdatetime"] "2001-01-01 01:00:00"
checkSuccess [$cur getFieldByName 0 "testyear"] "2001"
checkSuccess [$cur getFieldByName 0 "testchar"] "char1"
checkSuccess [$cur getFieldByName 0 "testtext"] "text1"
checkSuccess [$cur getFieldByName 0 "testvarchar"] "varchar1"
checkSuccess [$cur getFieldByName 0 "testtinytext"] "tinytext1"
checkSuccess [$cur getFieldByName 0 "testmediumtext"] "mediumtext1"
checkSuccess [$cur getFieldByName 0 "testlongtext"] "longtext1"
puts ""
checkSuccess [$cur getFieldByName 7 "testtinyint"] "8"
checkSuccess [$cur getFieldByName 7 "testsmallint"] "8"
checkSuccess [$cur getFieldByName 7 "testmediumint"] "8"
checkSuccess [$cur getFieldByName 7 "testint"] "8"
checkSuccess [$cur getFieldByName 7 "testbigint"] "8"
checkSuccess [$cur getFieldByName 7 "testfloat"] "8.1"
checkSuccess [$cur getFieldByName 7 "testreal"] "8.1"
checkSuccess [$cur getFieldByName 7 "testdecimal"] "8.1"
checkSuccess [$cur getFieldByName 7 "testdate"] "2008-01-01"
checkSuccess [$cur getFieldByName 7 "testtime"] "08:00:00"
checkSuccess [$cur getFieldByName 7 "testdatetime"] "2008-01-01 08:00:00"
checkSuccess [$cur getFieldByName 7 "testyear"] "2008"
checkSuccess [$cur getFieldByName 7 "testchar"] "char8"
checkSuccess [$cur getFieldByName 7 "testtext"] "text8"
checkSuccess [$cur getFieldByName 7 "testvarchar"] "varchar8"
checkSuccess [$cur getFieldByName 7 "testtinytext"] "tinytext8"
checkSuccess [$cur getFieldByName 7 "testmediumtext"] "mediumtext8"
checkSuccess [$cur getFieldByName 7 "testlongtext"] "longtext8"
puts ""

puts "FIELD LENGTHS BY NAME: "
checkSuccess [$cur getFieldLengthByName 0 "testtinyint"] 1
checkSuccess [$cur getFieldLengthByName 0 "testsmallint"] 1
checkSuccess [$cur getFieldLengthByName 0 "testmediumint"] 1
checkSuccess [$cur getFieldLengthByName 0 "testint"] 1
checkSuccess [$cur getFieldLengthByName 0 "testbigint"] 1
checkSuccess [$cur getFieldLengthByName 0 "testfloat"] 3
checkSuccess [$cur getFieldLengthByName 0 "testreal"] 3
checkSuccess [$cur getFieldLengthByName 0 "testdecimal"] 3
checkSuccess [$cur getFieldLengthByName 0 "testdate"] 10
checkSuccess [$cur getFieldLengthByName 0 "testtime"] 8
checkSuccess [$cur getFieldLengthByName 0 "testdatetime"] 19
checkSuccess [$cur getFieldLengthByName 0 "testyear"] 4
checkSuccess [$cur getFieldLengthByName 0 "testchar"] 5
checkSuccess [$cur getFieldLengthByName 0 "testtext"] 5
checkSuccess [$cur getFieldLengthByName 0 "testvarchar"] 8
checkSuccess [$cur getFieldLengthByName 0 "testtinytext"] 9
checkSuccess [$cur getFieldLengthByName 0 "testmediumtext"] 11
checkSuccess [$cur getFieldLengthByName 0 "testlongtext"] 9
puts ""
checkSuccess [$cur getFieldLengthByName 7 "testtinyint"] 1
checkSuccess [$cur getFieldLengthByName 7 "testsmallint"] 1
checkSuccess [$cur getFieldLengthByName 7 "testmediumint"] 1
checkSuccess [$cur getFieldLengthByName 7 "testint"] 1
checkSuccess [$cur getFieldLengthByName 7 "testbigint"] 1
checkSuccess [$cur getFieldLengthByName 7 "testfloat"] 3
checkSuccess [$cur getFieldLengthByName 7 "testreal"] 3
checkSuccess [$cur getFieldLengthByName 7 "testdecimal"] 3
checkSuccess [$cur getFieldLengthByName 7 "testdate"] 10
checkSuccess [$cur getFieldLengthByName 7 "testtime"] 8
checkSuccess [$cur getFieldLengthByName 7 "testdatetime"] 19
checkSuccess [$cur getFieldLengthByName 7 "testyear"] 4
checkSuccess [$cur getFieldLengthByName 7 "testchar"] 5
checkSuccess [$cur getFieldLengthByName 7 "testtext"] 5
checkSuccess [$cur getFieldLengthByName 7 "testvarchar"] 8
checkSuccess [$cur getFieldLengthByName 7 "testtinytext"] 9
checkSuccess [$cur getFieldLengthByName 7 "testmediumtext"] 11
checkSuccess [$cur getFieldLengthByName 7 "testlongtext"] 9
puts ""

puts "FIELDS BY ARRAY: "
set fields [$cur getRow 0]
checkSuccess [lindex $fields 0] 1
checkSuccess [lindex $fields 1] 1
checkSuccess [lindex $fields 2] 1
checkSuccess [lindex $fields 3] 1
checkSuccess [lindex $fields 4] 1
checkSuccess [lindex $fields 5] 1.1
checkSuccess [lindex $fields 6] 1.1
checkSuccess [lindex $fields 7] 1.1
checkSuccess [lindex $fields 8] "2001-01-01"
checkSuccess [lindex $fields 9] "01:00:00"
checkSuccess [lindex $fields 10] "2001-01-01 01:00:00"
checkSuccess [lindex $fields 11] 2001
checkSuccess [lindex $fields 12] "char1"
checkSuccess [lindex $fields 13] "text1"
checkSuccess [lindex $fields 14] "varchar1"
checkSuccess [lindex $fields 15] "tinytext1"
checkSuccess [lindex $fields 16] "mediumtext1"
checkSuccess [lindex $fields 17] "longtext1"
puts ""

puts "FIELD LENGTHS BY ARRAY: "
set fieldlens [$cur getRowLengths 0]
checkSuccess [lindex $fieldlens 0] 1
checkSuccess [lindex $fieldlens 1] 1
checkSuccess [lindex $fieldlens 2] 1
checkSuccess [lindex $fieldlens 3] 1
checkSuccess [lindex $fieldlens 4] 1
checkSuccess [lindex $fieldlens 5] 3
checkSuccess [lindex $fieldlens 6] 3
checkSuccess [lindex $fieldlens 7] 3
checkSuccess [lindex $fieldlens 8] 10
checkSuccess [lindex $fieldlens 9] 8
checkSuccess [lindex $fieldlens 10] 19
checkSuccess [lindex $fieldlens 11] 4
checkSuccess [lindex $fieldlens 12] 5
checkSuccess [lindex $fieldlens 13] 5
checkSuccess [lindex $fieldlens 14] 8
checkSuccess [lindex $fieldlens 15] 9
checkSuccess [lindex $fieldlens 16] 11
checkSuccess [lindex $fieldlens 17] 9
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
checkSuccess [$cur sendQuery "select * from testtable order by testtinyint"] 1
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
checkSuccess [$cur sendQuery "select * from testtable order by testtinyint"] 1
checkUndef [$cur getColumnName 0]
checkSuccess [$cur getColumnLengthByIndex 0] 0
checkUndef [$cur getColumnTypeByIndex 0]
puts ""
$cur getColumnInfo 
checkSuccess [$cur sendQuery "select * from testtable order by testtinyint"] 1
checkSuccess [$cur getColumnName 0] "testtinyint"
checkSuccess [$cur getColumnLengthByIndex 0] 1
checkSuccess [$cur getColumnTypeByIndex 0] "TINYINT"
puts ""

puts "SUSPENDED SESSION: "
checkSuccess [$cur sendQuery "select * from testtable order by testtinyint"] 1
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
checkSuccess [$cur sendQuery "select * from testtable order by testtinyint"] 1
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
checkSuccess [$cur sendQuery "select * from testtable order by testtinyint"] 1
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
checkSuccess [$cur sendQuery "select * from testtable order by testtinyint"] 1
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
checkSuccess [$cur sendQuery "select * from testtable order by testtinyint"] 1
set filename [$cur getCacheFileName]
checkSuccess $filename "cachefile1"
$cur cacheOff 
checkSuccess [$cur openCachedResultSet $filename] 1
checkSuccess [$cur getFieldByIndex 7 0] "8"
puts ""

puts "COLUMN COUNT FOR CACHED RESULT SET: "
checkSuccess [$cur colCount] 19
puts ""

puts "COLUMN NAMES FOR CACHED RESULT SET: "
checkSuccess [$cur getColumnName 0] "testtinyint"
checkSuccess [$cur getColumnName 1] "testsmallint"
checkSuccess [$cur getColumnName 2] "testmediumint"
checkSuccess [$cur getColumnName 3] "testint"
checkSuccess [$cur getColumnName 4] "testbigint"
checkSuccess [$cur getColumnName 5] "testfloat"
checkSuccess [$cur getColumnName 6] "testreal"
checkSuccess [$cur getColumnName 7] "testdecimal"
checkSuccess [$cur getColumnName 8] "testdate"
checkSuccess [$cur getColumnName 9] "testtime"
checkSuccess [$cur getColumnName 10] "testdatetime"
checkSuccess [$cur getColumnName 11] "testyear"
checkSuccess [$cur getColumnName 12] "testchar"
checkSuccess [$cur getColumnName 13] "testtext"
checkSuccess [$cur getColumnName 14] "testvarchar"
checkSuccess [$cur getColumnName 15] "testtinytext"
checkSuccess [$cur getColumnName 16] "testmediumtext"
checkSuccess [$cur getColumnName 17] "testlongtext"
set cols [$cur getColumnNames]
checkSuccess [lindex $cols 0] "testtinyint"
checkSuccess [lindex $cols 1] "testsmallint"
checkSuccess [lindex $cols 2] "testmediumint"
checkSuccess [lindex $cols 3] "testint"
checkSuccess [lindex $cols 4] "testbigint"
checkSuccess [lindex $cols 5] "testfloat"
checkSuccess [lindex $cols 6] "testreal"
checkSuccess [lindex $cols 7] "testdecimal"
checkSuccess [lindex $cols 8] "testdate"
checkSuccess [lindex $cols 9] "testtime"
checkSuccess [lindex $cols 10] "testdatetime"
checkSuccess [lindex $cols 11] "testyear"
checkSuccess [lindex $cols 12] "testchar"
checkSuccess [lindex $cols 13] "testtext"
checkSuccess [lindex $cols 14] "testvarchar"
checkSuccess [lindex $cols 15] "testtinytext"
checkSuccess [lindex $cols 16] "testmediumtext"
checkSuccess [lindex $cols 17] "testlongtext"
puts ""

puts "CACHED RESULT SET WITH RESULT SET BUFFER SIZE: "
$cur setResultSetBufferSize 2
$cur cacheToFile "cachefile1"
$cur setCacheTtl 200
checkSuccess [$cur sendQuery "select * from testtable order by testtinyint"] 1
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
checkSuccess [$cur sendQuery "select * from testtable order by testtinyint"] 1
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
set secondcon [sqlrcon -server [lindex $argv 0] -port [lindex $argv 1] -socket [lindex $argv 2] -user [lindex $argv 3] -password [lindex $argv 4] -retrytime 0 -tries 1]
set secondcur [$secondcon sqlrcur]
checkSuccess [$secondcur sendQuery "select count(*) from testtable"] 1
checkSuccess [$secondcur getFieldByIndex 0 0] "8"
checkSuccess [$con commit] 1
checkSuccess [$secondcur sendQuery "select count(*) from testtable"] 1
checkSuccess [$secondcur getFieldByIndex 0 0] "8"
checkSuccess [$con autoCommit 1] 1
checkSuccess [$cur sendQuery "insert into testdb.testtable values (10,10,10,10,10,10.1,10.1,10.1,'2010-01-01','10:00:00','2010-01-01,10:00:00','2010','char10','text10','varchar10','tinytext10','mediumtext10','longtext10',NULL)"] 1
checkSuccess [$secondcur sendQuery "select count(*) from testtable"] 1
checkSuccess [$secondcur getFieldByIndex 0 0] "9"
checkSuccess [$con autoCommit 0] 1
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

# invalid queries...
puts "INVALID QUERIES: "
catch {checkSuccess [$cur sendQuery "select * from testtable order by testtinyint"] 0}
catch {checkSuccess [$cur sendQuery "select * from testtable order by testtinyint"] 0}
catch {checkSuccess [$cur sendQuery "select * from testtable order by testtinyint"] 0}
catch {checkSuccess [$cur sendQuery "select * from testtable order by testtinyint"] 0}
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



