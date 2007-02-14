#! /usr/bin/tclsh

# Copyright] c] 2001] David Muse
# See the file COPYING for more information.


load /usr/lib/sqlrelay/sqlrelay.so sqlrelay

proc checkUndef {value} {

	switch $value "" {
		puts -nonewline "success "
	} default {
		puts "failure "
		exit
	}
}

proc checkSuccess {value success} {

	if {$value==$success} {
		puts -nonewline "success "
	} else {
		puts "failure "
		exit
	}
}

# usage...
if {$argc<5} {
	puts "usage: sybase.tcl host port socket user password"
	exit
}

# instantiation
set con [sqlrcon -server [lindex $argv 0] -port [lindex $argv 1] -socket [lindex $argv 2] -user [lindex $argv 3] -password [lindex $argv 4] -retrytime 0 -tries 1]
set cur [$con sqlrcur]

# get database type
puts "IDENTIFY: "
checkSuccess [$con identify]  "sybase"
puts ""

# ping
puts "PING: "
checkSuccess [$con ping]  1
puts ""

# drop existing table
catch {$cur sendQuery "drop table testtable"}

puts "CREATE TEMPTABLE: "
checkSuccess [$cur sendQuery "create table testtable (testint int, testsmallint smallint, testtinyint tinyint, testreal real, testfloat float, testdecimal decimal(4,1),  testnumeric numeric(4,1),  testmoney money, testsmallmoney smallmoney, testdatetime datetime, testsmalldatetime smalldatetime, testchar char(40),  testvarchar varchar(40),  testbit bit)"] 1
puts ""

puts "BEGIN TRANSACTION: "
#checkSuccess [$cur sendQuery "begin tran"] 1
puts ""

puts "INSERT: "
checkSuccess [$cur sendQuery "insert into testtable values (1,1,1,1.1,1.1,1.1,1.1,1.00,1.00,'01-Jan-2001 01:00:00','01-Jan-2001 01:00:00','testchar1','testvarchar1',1)"] 1
puts ""

puts "AFFECTED ROWS: "
checkSuccess [$cur affectedRows]  1
puts ""

puts "BIND BY POSITION: "
$cur prepareQuery "insert into testtable values (\@var1,\@var2,\@var3,\@var4,\@var5,\@var6,\@var7,\@var8,\@var9,\@var10,\@var11,\@var12,\@var13,\@var14)"
checkSuccess [$cur countBindVariables]  14
$cur inputBind "1" 2
$cur inputBind "2" 2
$cur inputBind "3" 2
$cur inputBind "4" 2.2 2 1
$cur inputBind "5" 2.2 2 1
$cur inputBind "6" 2.2 2 1
$cur inputBind "7" 2.2 2 1
$cur inputBind "8" 2.00 3 2
$cur inputBind "9" 2.00 3 2
$cur inputBind "10" "01-Jan-2002 02:00:00"
$cur inputBind "11" "01-Jan-2002 02:00:00"
$cur inputBind "12" "testchar2"
$cur inputBind "13" "testvarchar2"
$cur inputBind "14" 1
checkSuccess [$cur executeQuery]  1
$cur clearBinds 
$cur inputBind "1" 3
$cur inputBind "2" 3
$cur inputBind "3" 3
$cur inputBind "4" 3.3 2 1
$cur inputBind "5" 3.3 2 1
$cur inputBind "6" 3.3 2 1
$cur inputBind "7" 3.3 2 1
$cur inputBind "8" 3.00 3 2
$cur inputBind "9" 3.00 3 2
$cur inputBind "10" "01-Jan-2003 03:00:00"
$cur inputBind "11" "01-Jan-2003 03:00:00"
$cur inputBind "12" "testchar3"
$cur inputBind "13" "testvarchar3"
$cur inputBind "14" 1
checkSuccess [$cur executeQuery]  1
puts ""

puts "ARRAY OF BINDS BY POSITION: "
$cur clearBinds 
$cur inputBinds {{"1" 4} {"2" 4} {"3" 4} {"4" 4.4 2 1} {"5" 4.4 2 1} {"6" 4.4 2 1} {"7" 4.4 2 1} {"8" 4.00 3 2} {"9" 4.00 3 2} {"10" "01-Jan-2004 04:00:00"} {"11" "01-Jan-2004 04:00:00"} {"12" "testchar4"} {"13" "testvarchar4"} {"14" 1}}
checkSuccess [$cur executeQuery]  1
puts ""

puts "BIND BY NAME: "
$cur clearBinds 
$cur inputBind "var1" 5
$cur inputBind "var2" 5
$cur inputBind "var3" 5
$cur inputBind "var4" 5.5 2 1
$cur inputBind "var5" 5.5 2 1
$cur inputBind "var6" 5.5 2 1
$cur inputBind "var7" 5.5 2 1
$cur inputBind "var8" 5.00 3 2
$cur inputBind "var9" 5.00 3 2
$cur inputBind "var10" "01-Jan-2005 05:00:00"
$cur inputBind "var11" "01-Jan-2005 05:00:00"
$cur inputBind "var12" "testchar5"
$cur inputBind "var13" "testvarchar5"
$cur inputBind "var14" 1
checkSuccess [$cur executeQuery]  1
$cur clearBinds 
$cur inputBind "var1" 6
$cur inputBind "var2" 6
$cur inputBind "var3" 6
$cur inputBind "var4" 6.6 2 1
$cur inputBind "var5" 6.6 2 1
$cur inputBind "var6" 6.6 2 1
$cur inputBind "var7" 6.6 2 1
$cur inputBind "var8" 6.00 3 2
$cur inputBind "var9" 6.00 3 2
$cur inputBind "var10" "01-Jan-2006 06:00:00"
$cur inputBind "var11" "01-Jan-2006 06:00:00"
$cur inputBind "var12" "testchar6"
$cur inputBind "var13" "testvarchar6"
$cur inputBind "var14" 1
checkSuccess [$cur executeQuery]  1
puts ""

puts "ARRAY OF BINDS BY NAME: "
$cur clearBinds 
$cur inputBinds {{"var1" 7} {"var2" 7} {"var3" 7} {"var4" 7.7 2 1} {"var5" 7.7 2 1} {"var6" 7.7 2 1} {"var7" 7.7 2 1} {"var8" 7.00 3 2} {"var9" 7.00 3 2} {"var10" "01-Jan-2007 07:00:00"} {"var11" "01-Jan-2007 07:00:00"} {"var12" "testchar7"} {"var13" "testvarchar7"} {"var14" 1}}
checkSuccess [$cur executeQuery]  1
puts ""

puts "BIND BY NAME WITH VALIDATION: "
$cur clearBinds 
$cur inputBind "var1" 8
$cur inputBind "var2" 8
$cur inputBind "var3" 8
$cur inputBind "var4" 8.8 2 1
$cur inputBind "var5" 8.8 2 1
$cur inputBind "var6" 8.8 2 1
$cur inputBind "var7" 8.8 2 1
$cur inputBind "var8" 8.00 3 2
$cur inputBind "var9" 8.00 3 2
$cur inputBind "var10" "01-Jan-2008 08:00:00"
$cur inputBind "var11" "01-Jan-2008 08:00:00"
$cur inputBind "var12" "testchar8"
$cur inputBind "var13" "testvarchar8"
$cur inputBind "var14" 1
$cur inputBind "var15" "junkvalue"
$cur validateBinds 
checkSuccess [$cur executeQuery]  1
puts ""

puts "SELECT: "
checkSuccess [$cur sendQuery "select * from testtable order by testint"] 1
puts ""

puts "COLUMN COUNT: "
checkSuccess [$cur colCount]  14
puts ""

puts "COLUMN NAMES: "
checkSuccess [$cur getColumnName 0] "testint"
checkSuccess [$cur getColumnName 1] "testsmallint"
checkSuccess [$cur getColumnName 2] "testtinyint"
checkSuccess [$cur getColumnName 3] "testreal"
checkSuccess [$cur getColumnName 4] "testfloat"
checkSuccess [$cur getColumnName 5] "testdecimal"
checkSuccess [$cur getColumnName 6] "testnumeric"
checkSuccess [$cur getColumnName 7] "testmoney"
checkSuccess [$cur getColumnName 8] "testsmallmoney"
checkSuccess [$cur getColumnName 9] "testdatetime"
checkSuccess [$cur getColumnName 10] "testsmalldatetime"
checkSuccess [$cur getColumnName 11] "testchar"
checkSuccess [$cur getColumnName 12] "testvarchar"
checkSuccess [$cur getColumnName 13] "testbit"
set cols [$cur getColumnNames]
checkSuccess [lindex $cols 0] "testint"
checkSuccess [lindex $cols 1] "testsmallint"
checkSuccess [lindex $cols 2] "testtinyint"
checkSuccess [lindex $cols 3] "testreal"
checkSuccess [lindex $cols 4] "testfloat"
checkSuccess [lindex $cols 5] "testdecimal"
checkSuccess [lindex $cols 6] "testnumeric"
checkSuccess [lindex $cols 7] "testmoney"
checkSuccess [lindex $cols 8] "testsmallmoney"
checkSuccess [lindex $cols 9] "testdatetime"
checkSuccess [lindex $cols 10] "testsmalldatetime"
checkSuccess [lindex $cols 11] "testchar"
checkSuccess [lindex $cols 12] "testvarchar"
checkSuccess [lindex $cols 13] "testbit"
puts ""

puts "COLUMN TYPES: "
checkSuccess [$cur getColumnTypeByIndex 0] "INT"
checkSuccess [$cur getColumnTypeByName "testint"] "INT"
checkSuccess [$cur getColumnTypeByIndex 1] "SMALLINT"
checkSuccess [$cur getColumnTypeByName "testsmallint"] "SMALLINT"
checkSuccess [$cur getColumnTypeByIndex 2] "TINYINT"
checkSuccess [$cur getColumnTypeByName "testtinyint"] "TINYINT"
checkSuccess [$cur getColumnTypeByIndex 3] "REAL"
checkSuccess [$cur getColumnTypeByName "testreal"] "REAL"
checkSuccess [$cur getColumnTypeByIndex 4] "FLOAT"
checkSuccess [$cur getColumnTypeByName "testfloat"] "FLOAT"
checkSuccess [$cur getColumnTypeByIndex 5] "DECIMAL"
checkSuccess [$cur getColumnTypeByName "testdecimal"] "DECIMAL"
checkSuccess [$cur getColumnTypeByIndex 6] "NUMERIC"
checkSuccess [$cur getColumnTypeByName "testnumeric"] "NUMERIC"
checkSuccess [$cur getColumnTypeByIndex 7] "MONEY"
checkSuccess [$cur getColumnTypeByName "testmoney"] "MONEY"
checkSuccess [$cur getColumnTypeByIndex 8] "SMALLMONEY"
checkSuccess [$cur getColumnTypeByName "testsmallmoney"] "SMALLMONEY"
checkSuccess [$cur getColumnTypeByIndex 9] "DATETIME"
checkSuccess [$cur getColumnTypeByName "testdatetime"] "DATETIME"
checkSuccess [$cur getColumnTypeByIndex 10] "SMALLDATETIME"
checkSuccess [$cur getColumnTypeByName "testsmalldatetime"] "SMALLDATETIME"
checkSuccess [$cur getColumnTypeByIndex 11] "LONGCHAR"
checkSuccess [$cur getColumnTypeByName "testchar"] "LONGCHAR"
checkSuccess [$cur getColumnTypeByIndex 12] "LONGCHAR"
checkSuccess [$cur getColumnTypeByName "testvarchar"] "LONGCHAR"
checkSuccess [$cur getColumnTypeByIndex 13] "BIT"
checkSuccess [$cur getColumnTypeByName "testbit"] "BIT"
puts ""

puts "COLUMN LENGTH: "
checkSuccess [$cur getColumnLengthByIndex 0] 4
checkSuccess [$cur getColumnLengthByName "testint"] 4
checkSuccess [$cur getColumnLengthByIndex 1] 2
checkSuccess [$cur getColumnLengthByName "testsmallint"] 2
checkSuccess [$cur getColumnLengthByIndex 2] 1
checkSuccess [$cur getColumnLengthByName "testtinyint"] 1
checkSuccess [$cur getColumnLengthByIndex 3] 4
checkSuccess [$cur getColumnLengthByName "testreal"] 4
checkSuccess [$cur getColumnLengthByIndex 4] 8
checkSuccess [$cur getColumnLengthByName "testfloat"] 8
checkSuccess [$cur getColumnLengthByIndex 5] 35
checkSuccess [$cur getColumnLengthByName "testdecimal"] 35
checkSuccess [$cur getColumnLengthByIndex 6] 35
checkSuccess [$cur getColumnLengthByName "testnumeric"] 35
checkSuccess [$cur getColumnLengthByIndex 7] 8
checkSuccess [$cur getColumnLengthByName "testmoney"] 8
checkSuccess [$cur getColumnLengthByIndex 8] 4
checkSuccess [$cur getColumnLengthByName "testsmallmoney"] 4
checkSuccess [$cur getColumnLengthByIndex 9] 8
checkSuccess [$cur getColumnLengthByName "testdatetime"] 8
checkSuccess [$cur getColumnLengthByIndex 10] 4
checkSuccess [$cur getColumnLengthByName "testsmalldatetime"] 4
checkSuccess [$cur getColumnLengthByIndex 11] 80
checkSuccess [$cur getColumnLengthByName "testchar"] 80
checkSuccess [$cur getColumnLengthByIndex 12] 80
checkSuccess [$cur getColumnLengthByName "testvarchar"] 80
checkSuccess [$cur getColumnLengthByIndex 13] 1
checkSuccess [$cur getColumnLengthByName "testbit"] 1
puts ""

puts "LONGEST COLUMN: "
checkSuccess [$cur getLongestByIndex 0] 1
checkSuccess [$cur getLongestByName "testint"] 1
checkSuccess [$cur getLongestByIndex 1] 1
checkSuccess [$cur getLongestByName "testsmallint"] 1
checkSuccess [$cur getLongestByIndex 2] 1
checkSuccess [$cur getLongestByName "testtinyint"] 1
checkSuccess [$cur getLongestByIndex 3] 18
checkSuccess [$cur getLongestByName "testreal"] 18
checkSuccess [$cur getLongestByIndex 4] 18
checkSuccess [$cur getLongestByName "testfloat"] 18
checkSuccess [$cur getLongestByIndex 5] 3
checkSuccess [$cur getLongestByName "testdecimal"] 3
checkSuccess [$cur getLongestByIndex 6] 3
checkSuccess [$cur getLongestByName "testnumeric"] 3
checkSuccess [$cur getLongestByIndex 7] 4
checkSuccess [$cur getLongestByName "testmoney"] 4
checkSuccess [$cur getLongestByIndex 8] 4
checkSuccess [$cur getLongestByName "testsmallmoney"] 4
checkSuccess [$cur getLongestByIndex 9] 19
checkSuccess [$cur getLongestByName "testdatetime"] 19
checkSuccess [$cur getLongestByIndex 10] 19
checkSuccess [$cur getLongestByName "testsmalldatetime"] 19
checkSuccess [$cur getLongestByIndex 11] 40
checkSuccess [$cur getLongestByName "testchar"] 40
checkSuccess [$cur getLongestByIndex 12] 12
checkSuccess [$cur getLongestByName "testvarchar"] 12
checkSuccess [$cur getLongestByIndex 13] 1
checkSuccess [$cur getLongestByName "testbit"] 1
puts ""

puts "ROW COUNT: "
checkSuccess [$cur rowCount]  8
puts ""

puts "TOTAL ROWS: "
checkSuccess [$cur totalRows]  0
puts ""

puts "FIRST ROW INDEX: "
checkSuccess [$cur firstRowIndex]  0
puts ""

puts "END OF RESULT SET: "
checkSuccess [$cur endOfResultSet]  1
puts ""

puts "FIELDS BY INDEX: "
checkSuccess [$cur getFieldByIndex 0 0] "1"
checkSuccess [$cur getFieldByIndex 0 1] "1"
checkSuccess [$cur getFieldByIndex 0 2] "1"
#checkSuccess [$cur getFieldByIndex 0 3] "1.1"
#checkSuccess [$cur getFieldByIndex 0 4] "1.1"
checkSuccess [$cur getFieldByIndex 0 5] "1.1"
checkSuccess [$cur getFieldByIndex 0 6] "1.1"
checkSuccess [$cur getFieldByIndex 0 7] "1.00"
checkSuccess [$cur getFieldByIndex 0 8] "1.00"
checkSuccess [$cur getFieldByIndex 0 9] "Jan  1 2001  1:00AM"
checkSuccess [$cur getFieldByIndex 0 10] "Jan  1 2001  1:00AM"
checkSuccess [$cur getFieldByIndex 0 11] "testchar1                               "
checkSuccess [$cur getFieldByIndex 0 12] "testvarchar1"
checkSuccess [$cur getFieldByIndex 0 13] "1"
puts ""
checkSuccess [$cur getFieldByIndex 7 0] "8"
checkSuccess [$cur getFieldByIndex 7 1] "8"
checkSuccess [$cur getFieldByIndex 7 2] "8"
#checkSuccess [$cur getFieldByIndex 7 3] "8.8"
#checkSuccess [$cur getFieldByIndex 7 4] "8.8"
checkSuccess [$cur getFieldByIndex 7 5] "8.8"
checkSuccess [$cur getFieldByIndex 7 6] "8.8"
checkSuccess [$cur getFieldByIndex 7 7] "8.00"
checkSuccess [$cur getFieldByIndex 7 8] "8.00"
checkSuccess [$cur getFieldByIndex 7 9] "Jan  1 2008  8:00AM"
checkSuccess [$cur getFieldByIndex 7 10] "Jan  1 2008  8:00AM"
checkSuccess [$cur getFieldByIndex 7 11] "testchar8                               "
checkSuccess [$cur getFieldByIndex 7 12] "testvarchar8"
checkSuccess [$cur getFieldByIndex 7 13] "1"
puts ""

puts "FIELD LENGTHS BY INDEX: "
checkSuccess [$cur getFieldLengthByIndex 0 0] 1
checkSuccess [$cur getFieldLengthByIndex 0 1] 1
checkSuccess [$cur getFieldLengthByIndex 0 2] 1
checkSuccess [$cur getFieldLengthByIndex 0 3] 18
checkSuccess [$cur getFieldLengthByIndex 0 4] 18
checkSuccess [$cur getFieldLengthByIndex 0 5] 3
checkSuccess [$cur getFieldLengthByIndex 0 6] 3
checkSuccess [$cur getFieldLengthByIndex 0 7] 4
checkSuccess [$cur getFieldLengthByIndex 0 8] 4
checkSuccess [$cur getFieldLengthByIndex 0 9] 19
checkSuccess [$cur getFieldLengthByIndex 0 10] 19
checkSuccess [$cur getFieldLengthByIndex 0 11] 40
checkSuccess [$cur getFieldLengthByIndex 0 12] 12
checkSuccess [$cur getFieldLengthByIndex 0 13] 1
puts ""
checkSuccess [$cur getFieldLengthByIndex 7 0] 1
checkSuccess [$cur getFieldLengthByIndex 7 1] 1
checkSuccess [$cur getFieldLengthByIndex 7 2] 1
checkSuccess [$cur getFieldLengthByIndex 7 3] 18
checkSuccess [$cur getFieldLengthByIndex 7 4] 18
checkSuccess [$cur getFieldLengthByIndex 7 5] 3
checkSuccess [$cur getFieldLengthByIndex 7 6] 3
checkSuccess [$cur getFieldLengthByIndex 7 7] 4
checkSuccess [$cur getFieldLengthByIndex 7 8] 4
checkSuccess [$cur getFieldLengthByIndex 7 9] 19
checkSuccess [$cur getFieldLengthByIndex 7 10] 19
checkSuccess [$cur getFieldLengthByIndex 7 11] 40
checkSuccess [$cur getFieldLengthByIndex 7 12] 12
checkSuccess [$cur getFieldLengthByIndex 7 13] 1
puts ""

puts "FIELDS BY NAME: "
checkSuccess [$cur getFieldByName 0 "testint"] "1"
checkSuccess [$cur getFieldByName 0 "testsmallint"] "1"
checkSuccess [$cur getFieldByName 0 "testtinyint"] "1"
#checkSuccess [$cur getFieldByName 0 "testreal"] "1.1"
#checkSuccess [$cur getFieldByName 0 "testfloat"] "1.1"
checkSuccess [$cur getFieldByName 0 "testdecimal"] "1.1"
checkSuccess [$cur getFieldByName 0 "testnumeric"] "1.1"
checkSuccess [$cur getFieldByName 0 "testmoney"] "1.00"
checkSuccess [$cur getFieldByName 0 "testsmallmoney"] "1.00"
checkSuccess [$cur getFieldByName 0 "testdatetime"] "Jan  1 2001  1:00AM"
checkSuccess [$cur getFieldByName 0 "testsmalldatetime"] "Jan  1 2001  1:00AM"
checkSuccess [$cur getFieldByName 0 "testchar"] "testchar1                               "
checkSuccess [$cur getFieldByName 0 "testvarchar"] "testvarchar1"
checkSuccess [$cur getFieldByName 0 "testbit"] "1"
puts ""
checkSuccess [$cur getFieldByName 7 "testint"] "8"
checkSuccess [$cur getFieldByName 7 "testsmallint"] "8"
checkSuccess [$cur getFieldByName 7 "testtinyint"] "8"
#checkSuccess [$cur getFieldByName 7 "testreal"] "8.8"
#checkSuccess [$cur getFieldByName 7 "testfloat"] "8.8"
checkSuccess [$cur getFieldByName 7 "testdecimal"] "8.8"
checkSuccess [$cur getFieldByName 7 "testnumeric"] "8.8"
checkSuccess [$cur getFieldByName 7 "testmoney"] "8.00"
checkSuccess [$cur getFieldByName 7 "testsmallmoney"] "8.00"
checkSuccess [$cur getFieldByName 7 "testdatetime"] "Jan  1 2008  8:00AM"
checkSuccess [$cur getFieldByName 7 "testsmalldatetime"] "Jan  1 2008  8:00AM"
checkSuccess [$cur getFieldByName 7 "testchar"] "testchar8                               "
checkSuccess [$cur getFieldByName 7 "testvarchar"] "testvarchar8"
checkSuccess [$cur getFieldByName 7 "testbit"] "1"
puts ""

puts "FIELD LENGTHS BY NAME: "
checkSuccess [$cur getFieldLengthByName 0 "testint"] 1
checkSuccess [$cur getFieldLengthByName 0 "testsmallint"] 1
checkSuccess [$cur getFieldLengthByName 0 "testtinyint"] 1
#checkSuccess [$cur getFieldLengthByName 0 "testreal"] 3
#checkSuccess [$cur getFieldLengthByName 0 "testfloat"] 3
checkSuccess [$cur getFieldLengthByName 0 "testdecimal"] 3
checkSuccess [$cur getFieldLengthByName 0 "testnumeric"] 3
checkSuccess [$cur getFieldLengthByName 0 "testmoney"] 4
checkSuccess [$cur getFieldLengthByName 0 "testsmallmoney"] 4
checkSuccess [$cur getFieldLengthByName 0 "testdatetime"] 19
checkSuccess [$cur getFieldLengthByName 0 "testsmalldatetime"] 19
checkSuccess [$cur getFieldLengthByName 0 "testchar"] 40
checkSuccess [$cur getFieldLengthByName 0 "testvarchar"] 12
checkSuccess [$cur getFieldLengthByName 0 "testbit"] 1
puts ""
checkSuccess [$cur getFieldLengthByName 7 "testint"] 1
checkSuccess [$cur getFieldLengthByName 7 "testsmallint"] 1
checkSuccess [$cur getFieldLengthByName 7 "testtinyint"] 1
#checkSuccess [$cur getFieldLengthByName 7 "testreal"] 3
#checkSuccess [$cur getFieldLengthByName 7 "testfloat"] 3
checkSuccess [$cur getFieldLengthByName 7 "testdecimal"] 3
checkSuccess [$cur getFieldLengthByName 7 "testnumeric"] 3
checkSuccess [$cur getFieldLengthByName 7 "testmoney"] 4
checkSuccess [$cur getFieldLengthByName 7 "testsmallmoney"] 4
checkSuccess [$cur getFieldLengthByName 7 "testdatetime"] 19
checkSuccess [$cur getFieldLengthByName 7 "testsmalldatetime"] 19
checkSuccess [$cur getFieldLengthByName 7 "testchar"] 40
checkSuccess [$cur getFieldLengthByName 7 "testvarchar"] 12
checkSuccess [$cur getFieldLengthByName 7 "testbit"] 1
puts ""

puts "FIELDS BY ARRAY: "
set fields [$cur getRow 0]
checkSuccess [lindex $fields 0] 1
checkSuccess [lindex $fields 1] 1
checkSuccess [lindex $fields 2] 1
#checkSuccess [lindex $fields 3] 1.1
#checkSuccess [lindex $fields 4] 1.1
checkSuccess [lindex $fields 5] 1.1
checkSuccess [lindex $fields 6] 1.1
checkSuccess [lindex $fields 7] 1.0
checkSuccess [lindex $fields 8] 1.0
checkSuccess [lindex $fields 9] "Jan  1 2001  1:00AM"
checkSuccess [lindex $fields 10] "Jan  1 2001  1:00AM"
checkSuccess [lindex $fields 11] "testchar1                               "
checkSuccess [lindex $fields 12] "testvarchar1"
checkSuccess [lindex $fields 13] 1
puts ""

puts "FIELD LENGTHS BY ARRAY: "
set fieldlens [$cur getRowLengths 0]
checkSuccess [lindex $fieldlens 0] 1
checkSuccess [lindex $fieldlens 1] 1
checkSuccess [lindex $fieldlens 2] 1
#checkSuccess [lindex $fieldlens 3] 3
#checkSuccess [lindex $fieldlens 4] 3
checkSuccess [lindex $fieldlens 5] 3
checkSuccess [lindex $fieldlens 6] 3
checkSuccess [lindex $fieldlens 7] 4
checkSuccess [lindex $fieldlens 8] 4
checkSuccess [lindex $fieldlens 9] 19
checkSuccess [lindex $fieldlens 10] 19
checkSuccess [lindex $fieldlens 11] 40
checkSuccess [lindex $fieldlens 12] 12
checkSuccess [lindex $fieldlens 13] 1
puts ""

puts "INDIVIDUAL SUBSTITUTIONS: "
$cur prepareQuery "select \$(var1),'\$(var2)',\$(var3)"
$cur substitution "var1" 1
$cur substitution "var2" "hello"
$cur substitution "var3" 10.5556 6 4
checkSuccess [$cur executeQuery]  1
puts ""

puts "FIELDS: "
checkSuccess [$cur getFieldByIndex 0 0] "1"
checkSuccess [$cur getFieldByIndex 0 1] "hello"
checkSuccess [$cur getFieldByIndex 0 2] "10.5556"
puts ""

puts "ARRAY SUBSTITUTIONS: "
$cur prepareQuery "select \$(var1),'\$(var2)',\$(var3)"
$cur substitutions {{"var1" 1} {"var2" "hello"} {"var3" 10.5556 6 4}}
checkSuccess [$cur executeQuery]  1
puts ""

puts "FIELDS: "
checkSuccess [$cur getFieldByIndex 0 0] "1"
checkSuccess [$cur getFieldByIndex 0 1] "hello"
checkSuccess [$cur getFieldByIndex 0 2] "10.5556"
puts ""

puts "RESULT SET BUFFER SIZE: "
checkSuccess [$cur getResultSetBufferSize]  0
$cur setResultSetBufferSize 2
checkSuccess [$cur sendQuery "select * from testtable order by testint"] 1
checkSuccess [$cur getResultSetBufferSize]  2
puts ""
checkSuccess [$cur firstRowIndex]  0
checkSuccess [$cur endOfResultSet]  0
checkSuccess [$cur rowCount]  2
checkSuccess [$cur getFieldByIndex 0 0] "1"
checkSuccess [$cur getFieldByIndex 1 0] "2"
checkSuccess [$cur getFieldByIndex 2 0] "3"
puts ""
checkSuccess [$cur firstRowIndex]  2
checkSuccess [$cur endOfResultSet]  0
checkSuccess [$cur rowCount]  4
checkSuccess [$cur getFieldByIndex 6 0] "7"
checkSuccess [$cur getFieldByIndex 7 0] "8"
puts ""
checkSuccess [$cur firstRowIndex]  6
checkSuccess [$cur endOfResultSet]  0
checkSuccess [$cur rowCount]  8
checkUndef [$cur getFieldByIndex 8 0]
puts ""
checkSuccess [$cur firstRowIndex]  8
checkSuccess [$cur endOfResultSet]  1
checkSuccess [$cur rowCount]  8
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
checkSuccess [$cur getColumnTypeByIndex 0] "INT"
puts ""

puts "SUSPENDED SESSION: "
checkSuccess [$cur sendQuery "select * from testtable order by testint"] 1
$cur suspendResultSet 
checkSuccess [$con suspendSession]  1
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
checkSuccess [$con suspendSession]  1
set port [$con getConnectionPort]
set socket [$con getConnectionSocket]
checkSuccess [$con resumeSession $port $socket] 1
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
checkSuccess [$con suspendSession]  1
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
checkSuccess [$con suspendSession]  1
set port [$con getConnectionPort]
set socket [$con getConnectionSocket]
checkSuccess [$con resumeSession $port $socket] 1
checkSuccess [$cur resumeResultSet $id] 1
puts ""
checkSuccess [$cur firstRowIndex]  4
checkSuccess [$cur endOfResultSet]  0
checkSuccess [$cur rowCount]  6
checkSuccess [$cur getFieldByIndex 7 0] "8"
puts ""
checkSuccess [$cur firstRowIndex]  6
checkSuccess [$cur endOfResultSet]  0
checkSuccess [$cur rowCount]  8
checkUndef [$cur getFieldByIndex 8 0]
puts ""
checkSuccess [$cur firstRowIndex]  8
checkSuccess [$cur endOfResultSet]  1
checkSuccess [$cur rowCount]  8
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
checkSuccess [$cur colCount]  14
puts ""

puts "COLUMN NAMES FOR CACHED RESULT SET: "
checkSuccess [$cur getColumnName 0] "testint"
checkSuccess [$cur getColumnName 1] "testsmallint"
checkSuccess [$cur getColumnName 2] "testtinyint"
checkSuccess [$cur getColumnName 3] "testreal"
checkSuccess [$cur getColumnName 4] "testfloat"
checkSuccess [$cur getColumnName 5] "testdecimal"
checkSuccess [$cur getColumnName 6] "testnumeric"
checkSuccess [$cur getColumnName 7] "testmoney"
checkSuccess [$cur getColumnName 8] "testsmallmoney"
checkSuccess [$cur getColumnName 9] "testdatetime"
checkSuccess [$cur getColumnName 10] "testsmalldatetime"
checkSuccess [$cur getColumnName 11] "testchar"
checkSuccess [$cur getColumnName 12] "testvarchar"
checkSuccess [$cur getColumnName 13] "testbit"
set cols [$cur getColumnNames]
checkSuccess [lindex $cols 0] "testint"
checkSuccess [lindex $cols 1] "testsmallint"
checkSuccess [lindex $cols 2] "testtinyint"
checkSuccess [lindex $cols 3] "testreal"
checkSuccess [lindex $cols 4] "testfloat"
checkSuccess [lindex $cols 5] "testdecimal"
checkSuccess [lindex $cols 6] "testnumeric"
checkSuccess [lindex $cols 7] "testmoney"
checkSuccess [lindex $cols 8] "testsmallmoney"
checkSuccess [lindex $cols 9] "testdatetime"
checkSuccess [lindex $cols 10] "testsmalldatetime"
checkSuccess [lindex $cols 11] "testchar"
checkSuccess [lindex $cols 12] "testvarchar"
checkSuccess [lindex $cols 13] "testbit"
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
checkSuccess [$con suspendSession]  1
set port [$con getConnectionPort]
set socket [$con getConnectionSocket]
puts ""
checkSuccess [$con resumeSession $port $socket] 1
checkSuccess [$cur resumeCachedResultSet $id $filename] 1
puts ""
checkSuccess [$cur firstRowIndex]  4
checkSuccess [$cur endOfResultSet]  0
checkSuccess [$cur rowCount]  6
checkSuccess [$cur getFieldByIndex 7 0] "8"
puts ""
checkSuccess [$cur firstRowIndex]  6
checkSuccess [$cur endOfResultSet]  0
checkSuccess [$cur rowCount]  8
checkUndef [$cur getFieldByIndex 8 0]
puts ""
checkSuccess [$cur firstRowIndex]  8
checkSuccess [$cur endOfResultSet]  1
checkSuccess [$cur rowCount]  8
$cur cacheOff 
puts ""
checkSuccess [$cur openCachedResultSet $filename] 1
checkSuccess [$cur getFieldByIndex 7 0] "8"
checkUndef [$cur getFieldByIndex 8 0]
$cur setResultSetBufferSize 0
puts ""

puts "FINISHED SUSPENDED SESSION: "
checkSuccess [$cur sendQuery "select * from testtable order by testint"] 1
checkSuccess [$cur getFieldByIndex 4 0] "5"
checkSuccess [$cur getFieldByIndex 5 0] "6"
checkSuccess [$cur getFieldByIndex 6 0] "7"
checkSuccess [$cur getFieldByIndex 7 0] "8"
set id [$cur getResultSetId]
$cur suspendResultSet 
checkSuccess [$con suspendSession]  1
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



