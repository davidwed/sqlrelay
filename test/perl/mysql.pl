#! /usr/bin/env perl

# Copyright (c) 1999-2018 David Muse
# See the file COPYING for more information.


use SQLRelay::Connection;
use SQLRelay::Cursor;

sub checkUndef {

	$value=shift(@_);

	if (!defined($value)) {
		print("success ");
	} else {
		print("failure ");
		exit(1);
	}
}

sub checkSuccess {

	$value=shift(@_);
	$success=shift(@_);

	if ($value==$success) {
		print("success ");
	} else {
		print("$value != $success ");
		print("failure ");
		exit(1);
	}
}

sub checkSuccessString {

	$value=shift(@_);
	$success=shift(@_);

	if ($value eq $success) {
		print("success ");
	} else {
		print("$value != $success ");
		print("failure ");
		exit(1);
	}
}


# instantiation
$con=SQLRelay::Connection->new("sqlrelay",9000,"/tmp/test.socket",
						"testuser","testpassword",0,1);
$cur=SQLRelay::Cursor->new($con);

# get database type
print("IDENTIFY: \n");
checkSuccessString($con->identify(),"mysql");
print("\n");

# get the db version
$dbversion=$con->dbVersion();
$majorversion=int(substr($dbversion,0,1));

# ping
print("PING: \n");
checkSuccess($con->ping(),1);
print("\n");

# drop existing table
$cur->sendQuery("drop table testtable");

# create a new table
print("CREATE TEMPTABLE: \n");
checkSuccess($cur->sendQuery("create table testtable (testtinyint tinyint, testsmallint smallint, testmediumint mediumint, testint int, testbigint bigint, testfloat float, testreal real, testdecimal decimal(2,1), testdate date, testtime time, testdatetime datetime, testyear year, testchar char(40), testtext text, testvarchar varchar(40), testtinytext tinytext, testmediumtext mediumtext, testlongtext longtext, testtimestamp timestamp)"),1);
print("\n");

print("INSERT: \n");
checkSuccess($cur->sendQuery("insert into testtable values (1,1,1,1,1,1.1,1.1,1.1,'2001-01-01','01:00:00','2001-01-01 01:00:00','2001','char1','text1','varchar1','tinytext1','mediumtext1','longtext1',NULL)"),1);
checkSuccess($cur->sendQuery("insert into testtable values (2,2,2,2,2,2.1,2.1,2.1,'2002-01-01','02:00:00','2002-01-01 02:00:00','2002','char2','text2','varchar2','tinytext2','mediumtext2','longtext2',NULL)"),1);
checkSuccess($cur->sendQuery("insert into testtable values (3,3,3,3,3,3.1,3.1,3.1,'2003-01-01','03:00:00','2003-01-01 03:00:00','2003','char3','text3','varchar3','tinytext3','mediumtext3','longtext3',NULL)"),1);
checkSuccess($cur->sendQuery("insert into testtable values (4,4,4,4,4,4.1,4.1,4.1,'2004-01-01','04:00:00','2004-01-01 04:00:00','2004','char4','text4','varchar4','tinytext4','mediumtext4','longtext4',NULL)"),1);
print("\n");

print("AFFECTED ROWS: \n");
checkSuccess($cur->affectedRows(),1);
print("\n");

print("BIND BY POSITION: \n");
$cur->prepareQuery("insert into testtable values (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,NULL)");
checkSuccess($cur->countBindVariables(),18);
$cur->inputBind("1",5);
$cur->inputBind("2",5);
$cur->inputBind("3",5);
$cur->inputBind("4",5);
$cur->inputBind("5",5);
$cur->inputBind("6",5.1,2,1);
$cur->inputBind("7",5.1,2,1);
$cur->inputBind("8",5.1,2,1);
$cur->inputBind("9","2005-01-01");
$cur->inputBind("10","05:00:00");
$cur->inputBind("11","2005-01-01 05:00:00");
$cur->inputBind("12","2005");
$cur->inputBind("13","char5");
$cur->inputBind("14","text5");
$cur->inputBind("15","varchar5");
$cur->inputBind("16","tinytext5");
$cur->inputBind("17","mediumtext5");
$cur->inputBind("18","longtext5");
checkSuccess($cur->executeQuery(),1);
$cur->clearBinds();
$cur->inputBind("1",6);
$cur->inputBind("2",6);
$cur->inputBind("3",6);
$cur->inputBind("4",6);
$cur->inputBind("5",6);
$cur->inputBind("6",6.1,2,1);
$cur->inputBind("7",6.1,2,1);
$cur->inputBind("8",6.1,2,1);
$cur->inputBind("9",'2006-01-01');
$cur->inputBind("10",'06:00:00');
$cur->inputBind("11",'2006-01-01 06:00:00');
$cur->inputBind("12",'2006');
$cur->inputBind("13",'char6');
$cur->inputBind("14",'text6');
$cur->inputBind("15",'varchar6');
$cur->inputBind("16",'tinytext6');
$cur->inputBind("17",'mediumtext6');
$cur->inputBind("18",'longtext6');
checkSuccess($cur->executeQuery(),1);
print("\n");

print("ARRAY OF BINDS BY POSITION: \n");
$cur->clearBinds();
@vars=("1","2","3","4","5","6",
		"7","8","9","10","11","12",
		"13","14","15",
		"16","17","18");
@vals=(7,7,7,7,7,7.1,7.1,7.1,'2007-01-01','07:00:00','2007-01-01 07:00:00','2007','char7','text7','varchar7','tinytext7','mediumtext7','longtext7');
@precs=(0,0,0,0,0,2,2,2,0,0,0,0,0,0,0,0,0,0);
@scales=(0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0);
$cur->inputBinds(\@vars,\@vals,\@precs,\@scales);
checkSuccess($cur->executeQuery(),1);
print("\n");

print("BIND BY POSITION WITH VALIDATION: \n");
$cur->clearBinds();
$cur->inputBind("1",8);
$cur->inputBind("2",8);
$cur->inputBind("3",8);
$cur->inputBind("4",8);
$cur->inputBind("5",8);
$cur->inputBind("6",8.1,2,1);
$cur->inputBind("7",8.1,2,1);
$cur->inputBind("8",8.1,2,1);
$cur->inputBind("9",'2008-01-01');
$cur->inputBind("10",'08:00:00');
$cur->inputBind("11",'2008-01-01 08:00:00');
$cur->inputBind("12",'2008');
$cur->inputBind("13",'char8');
$cur->inputBind("14",'text8');
$cur->inputBind("15",'varchar8');
$cur->inputBind("16",'tinytext8');
$cur->inputBind("17",'mediumtext8');
$cur->inputBind("18",'longtext8');
$cur->validateBinds();
checkSuccess($cur->executeQuery(),1);
print("\n");

print("SELECT: \n");
checkSuccess($cur->sendQuery("select * from testtable order by testtinyint"),1);
print("\n");

print("COLUMN COUNT: \n");
checkSuccess($cur->colCount(),19);
print("\n");

print("COLUMN NAMES: \n");
checkSuccessString($cur->getColumnName(0),"testtinyint");
checkSuccessString($cur->getColumnName(1),"testsmallint");
checkSuccessString($cur->getColumnName(2),"testmediumint");
checkSuccessString($cur->getColumnName(3),"testint");
checkSuccessString($cur->getColumnName(4),"testbigint");
checkSuccessString($cur->getColumnName(5),"testfloat");
checkSuccessString($cur->getColumnName(6),"testreal");
checkSuccessString($cur->getColumnName(7),"testdecimal");
checkSuccessString($cur->getColumnName(8),"testdate");
checkSuccessString($cur->getColumnName(9),"testtime");
checkSuccessString($cur->getColumnName(10),"testdatetime");
checkSuccessString($cur->getColumnName(11),"testyear");
checkSuccessString($cur->getColumnName(12),"testchar");
checkSuccessString($cur->getColumnName(13),"testtext");
checkSuccessString($cur->getColumnName(14),"testvarchar");
checkSuccessString($cur->getColumnName(15),"testtinytext");
checkSuccessString($cur->getColumnName(16),"testmediumtext");
checkSuccessString($cur->getColumnName(17),"testlongtext");
checkSuccessString($cur->getColumnName(18),"testtimestamp");
@cols=$cur->getColumnNames();
checkSuccessString($cols[0],"testtinyint");
checkSuccessString($cols[1],"testsmallint");
checkSuccessString($cols[2],"testmediumint");
checkSuccessString($cols[3],"testint");
checkSuccessString($cols[4],"testbigint");
checkSuccessString($cols[5],"testfloat");
checkSuccessString($cols[6],"testreal");
checkSuccessString($cols[7],"testdecimal");
checkSuccessString($cols[8],"testdate");
checkSuccessString($cols[9],"testtime");
checkSuccessString($cols[10],"testdatetime");
checkSuccessString($cols[11],"testyear");
checkSuccessString($cols[12],"testchar");
checkSuccessString($cols[13],"testtext");
checkSuccessString($cols[14],"testvarchar");
checkSuccessString($cols[15],"testtinytext");
checkSuccessString($cols[16],"testmediumtext");
checkSuccessString($cols[17],"testlongtext");
checkSuccessString($cols[18],"testtimestamp");
print("\n");

print("COLUMN TYPES: \n");
checkSuccessString($cur->getColumnType(0),"TINYINT");
checkSuccessString($cur->getColumnType(1),"SMALLINT");
checkSuccessString($cur->getColumnType(2),"MEDIUMINT");
checkSuccessString($cur->getColumnType(3),"INT");
checkSuccessString($cur->getColumnType(4),"BIGINT");
checkSuccessString($cur->getColumnType(5),"FLOAT");
checkSuccessString($cur->getColumnType(6),"REAL");
checkSuccessString($cur->getColumnType(7),"DECIMAL");
checkSuccessString($cur->getColumnType(8),"DATE");
checkSuccessString($cur->getColumnType(9),"TIME");
checkSuccessString($cur->getColumnType(10),"DATETIME");
checkSuccessString($cur->getColumnType(11),"YEAR");
if ($majorversion==3) {
	checkSuccessString($cur->getColumnType(12),"VARSTRING");
} else {
	checkSuccessString($cur->getColumnType(12),"STRING");
}
checkSuccessString($cur->getColumnType(13),"BLOB");
checkSuccessString($cur->getColumnType(14),"VARSTRING");
checkSuccessString($cur->getColumnType(15),"TINYBLOB");
checkSuccessString($cur->getColumnType(16),"MEDIUMBLOB");
checkSuccessString($cur->getColumnType(17),"LONGBLOB");
checkSuccessString($cur->getColumnType(18),"TIMESTAMP");
checkSuccessString($cur->getColumnType("testtinyint"),"TINYINT");
checkSuccessString($cur->getColumnType("testsmallint"),"SMALLINT");
checkSuccessString($cur->getColumnType("testmediumint"),"MEDIUMINT");
checkSuccessString($cur->getColumnType("testint"),"INT");
checkSuccessString($cur->getColumnType("testbigint"),"BIGINT");
checkSuccessString($cur->getColumnType("testfloat"),"FLOAT");
checkSuccessString($cur->getColumnType("testreal"),"REAL");
checkSuccessString($cur->getColumnType("testdecimal"),"DECIMAL");
checkSuccessString($cur->getColumnType("testdate"),"DATE");
checkSuccessString($cur->getColumnType("testtime"),"TIME");
checkSuccessString($cur->getColumnType("testdatetime"),"DATETIME");
checkSuccessString($cur->getColumnType("testyear"),"YEAR");
if ($majorversion==3) {
	checkSuccessString($cur->getColumnType("testchar"),"VARSTRING");
} else {
	checkSuccessString($cur->getColumnType("testchar"),"STRING");
}
checkSuccessString($cur->getColumnType("testtext"),"BLOB");
checkSuccessString($cur->getColumnType("testvarchar"),"VARSTRING");
checkSuccessString($cur->getColumnType("testtinytext"),"TINYBLOB");
checkSuccessString($cur->getColumnType("testmediumtext"),"MEDIUMBLOB");
checkSuccessString($cur->getColumnType("testlongtext"),"LONGBLOB");
checkSuccessString($cur->getColumnType("testtimestamp"),"TIMESTAMP");
print("\n");

print("COLUMN LENGTH: \n");
checkSuccess($cur->getColumnLength(0),1);
checkSuccess($cur->getColumnLength(1),2);
checkSuccess($cur->getColumnLength(2),3);
checkSuccess($cur->getColumnLength(3),4);
checkSuccess($cur->getColumnLength(4),8);
checkSuccess($cur->getColumnLength(5),4);
checkSuccess($cur->getColumnLength(6),8);
checkSuccess($cur->getColumnLength(7),6);
checkSuccess($cur->getColumnLength(8),3);
checkSuccess($cur->getColumnLength(9),3);
checkSuccess($cur->getColumnLength(10),8);
checkSuccess($cur->getColumnLength(11),1);
#checkSuccess($cur->getColumnLength(12),40);
checkSuccess($cur->getColumnLength(13),65535);
#checkSuccess($cur->getColumnLength(14),41);
checkSuccess($cur->getColumnLength(15),255);
checkSuccess($cur->getColumnLength(16),16777215);
checkSuccess($cur->getColumnLength(17),2147483647);
checkSuccess($cur->getColumnLength(18),4);
checkSuccess($cur->getColumnLength("testtinyint"),1);
checkSuccess($cur->getColumnLength("testsmallint"),2);
checkSuccess($cur->getColumnLength("testmediumint"),3);
checkSuccess($cur->getColumnLength("testint"),4);
checkSuccess($cur->getColumnLength("testbigint"),8);
checkSuccess($cur->getColumnLength("testfloat"),4);
checkSuccess($cur->getColumnLength("testreal"),8);
checkSuccess($cur->getColumnLength("testdecimal"),6);
checkSuccess($cur->getColumnLength("testdate"),3);
checkSuccess($cur->getColumnLength("testtime"),3);
checkSuccess($cur->getColumnLength("testdatetime"),8);
checkSuccess($cur->getColumnLength("testyear"),1);
#checkSuccess($cur->getColumnLength("testchar"),40);
checkSuccess($cur->getColumnLength("testtext"),65535);
#checkSuccess($cur->getColumnLength("testvarchar"),41);
checkSuccess($cur->getColumnLength("testtinytext"),255);
checkSuccess($cur->getColumnLength("testmediumtext"),16777215);
checkSuccess($cur->getColumnLength("testlongtext"),2147483647);
checkSuccess($cur->getColumnLength("testtimestamp"),4);
print("\n");

print("LONGEST COLUMN: \n");
checkSuccess($cur->getLongest(0),1);
checkSuccess($cur->getLongest(1),1);
checkSuccess($cur->getLongest(2),1);
checkSuccess($cur->getLongest(3),1);
checkSuccess($cur->getLongest(4),1);
#checkSuccess($cur->getLongest(5),3);
checkSuccess($cur->getLongest(6),3);
checkSuccess($cur->getLongest(7),3);
checkSuccess($cur->getLongest(8),10);
checkSuccess($cur->getLongest(9),8);
checkSuccess($cur->getLongest(10),19);
checkSuccess($cur->getLongest(11),4);
checkSuccess($cur->getLongest(12),5);
checkSuccess($cur->getLongest(13),5);
checkSuccess($cur->getLongest(14),8);
checkSuccess($cur->getLongest(15),9);
checkSuccess($cur->getLongest(16),11);
checkSuccess($cur->getLongest(17),9);
if ($majorversion==3) {
	checkSuccess($cur->getLongest(18),14);
} else {
	checkSuccess($cur->getLongest(18),19);
}
checkSuccess($cur->getLongest("testtinyint"),1);
checkSuccess($cur->getLongest("testsmallint"),1);
checkSuccess($cur->getLongest("testmediumint"),1);
checkSuccess($cur->getLongest("testint"),1);
checkSuccess($cur->getLongest("testbigint"),1);
#checkSuccess($cur->getLongest("testfloat"),3);
checkSuccess($cur->getLongest("testreal"),3);
checkSuccess($cur->getLongest("testdecimal"),3);
checkSuccess($cur->getLongest("testdate"),10);
checkSuccess($cur->getLongest("testtime"),8);
checkSuccess($cur->getLongest("testdatetime"),19);
checkSuccess($cur->getLongest("testyear"),4);
checkSuccess($cur->getLongest("testchar"),5);
checkSuccess($cur->getLongest("testtext"),5);
checkSuccess($cur->getLongest("testvarchar"),8);
checkSuccess($cur->getLongest("testtinytext"),9);
checkSuccess($cur->getLongest("testmediumtext"),11);
checkSuccess($cur->getLongest("testlongtext"),9);
if ($majorversion==3) {
	checkSuccess($cur->getLongest("testtimestamp"),14);
} else {
	checkSuccess($cur->getLongest("testtimestamp"),19);
}
print("\n");

print("ROW COUNT: \n");
checkSuccess($cur->rowCount(),8);
print("\n");

print("TOTAL ROWS: \n");
# older versions of mysql know this
#checkSuccess($cur->totalRows(),0);
print("\n");

print("FIRST ROW INDEX: \n");
checkSuccess($cur->firstRowIndex(),0);
print("\n");

print("END OF RESULT SET: \n");
checkSuccess($cur->endOfResultSet(),1);
print("\n");

print("FIELDS BY INDEX: \n");
checkSuccessString($cur->getField(0,0),"1");
checkSuccessString($cur->getField(0,1),"1");
checkSuccessString($cur->getField(0,2),"1");
checkSuccessString($cur->getField(0,3),"1");
checkSuccessString($cur->getField(0,4),"1");
#checkSuccessString($cur->getField(0,5),"1.1");
checkSuccessString($cur->getField(0,6),"1.1");
checkSuccessString($cur->getField(0,7),"1.1");
checkSuccessString($cur->getField(0,8),"2001-01-01");
checkSuccessString($cur->getField(0,9),"01:00:00");
checkSuccessString($cur->getField(0,10),"2001-01-01 01:00:00");
checkSuccessString($cur->getField(0,11),"2001");
checkSuccessString($cur->getField(0,12),"char1");
checkSuccessString($cur->getField(0,13),"text1");
checkSuccessString($cur->getField(0,14),"varchar1");
checkSuccessString($cur->getField(0,15),"tinytext1");
checkSuccessString($cur->getField(0,16),"mediumtext1");
checkSuccessString($cur->getField(0,17),"longtext1");
print("\n");
checkSuccessString($cur->getField(7,0),"8");
checkSuccessString($cur->getField(7,1),"8");
checkSuccessString($cur->getField(7,2),"8");
checkSuccessString($cur->getField(7,3),"8");
checkSuccessString($cur->getField(7,4),"8");
#checkSuccessString($cur->getField(7,5),"8.1");
checkSuccessString($cur->getField(7,6),"8.1");
checkSuccessString($cur->getField(7,7),"8.1");
checkSuccessString($cur->getField(7,8),"2008-01-01");
checkSuccessString($cur->getField(7,9),"08:00:00");
checkSuccessString($cur->getField(7,10),"2008-01-01 08:00:00");
checkSuccessString($cur->getField(7,11),"2008");
checkSuccessString($cur->getField(7,12),"char8");
checkSuccessString($cur->getField(7,13),"text8");
checkSuccessString($cur->getField(7,14),"varchar8");
checkSuccessString($cur->getField(7,15),"tinytext8");
checkSuccessString($cur->getField(7,16),"mediumtext8");
checkSuccessString($cur->getField(7,17),"longtext8");
print("\n");

print("FIELD LENGTHS BY INDEX: \n");
checkSuccess($cur->getFieldLength(0,0),1);
checkSuccess($cur->getFieldLength(0,1),1);
checkSuccess($cur->getFieldLength(0,2),1);
checkSuccess($cur->getFieldLength(0,3),1);
checkSuccess($cur->getFieldLength(0,4),1);
#checkSuccess($cur->getFieldLength(0,5),3);
checkSuccess($cur->getFieldLength(0,6),3);
checkSuccess($cur->getFieldLength(0,7),3);
checkSuccess($cur->getFieldLength(0,8),10);
checkSuccess($cur->getFieldLength(0,9),8);
checkSuccess($cur->getFieldLength(0,10),19);
checkSuccess($cur->getFieldLength(0,11),4);
checkSuccess($cur->getFieldLength(0,12),5);
checkSuccess($cur->getFieldLength(0,13),5);
checkSuccess($cur->getFieldLength(0,14),8);
checkSuccess($cur->getFieldLength(0,15),9);
checkSuccess($cur->getFieldLength(0,16),11);
checkSuccess($cur->getFieldLength(0,17),9);
print("\n");
checkSuccess($cur->getFieldLength(7,0),1);
checkSuccess($cur->getFieldLength(7,1),1);
checkSuccess($cur->getFieldLength(7,2),1);
checkSuccess($cur->getFieldLength(7,3),1);
checkSuccess($cur->getFieldLength(7,4),1);
#checkSuccess($cur->getFieldLength(7,5),3);
checkSuccess($cur->getFieldLength(7,6),3);
checkSuccess($cur->getFieldLength(7,7),3);
checkSuccess($cur->getFieldLength(7,8),10);
checkSuccess($cur->getFieldLength(7,9),8);
checkSuccess($cur->getFieldLength(7,10),19);
checkSuccess($cur->getFieldLength(7,11),4);
checkSuccess($cur->getFieldLength(7,12),5);
checkSuccess($cur->getFieldLength(7,13),5);
checkSuccess($cur->getFieldLength(7,14),8);
checkSuccess($cur->getFieldLength(7,15),9);
checkSuccess($cur->getFieldLength(7,16),11);
checkSuccess($cur->getFieldLength(7,17),9);
print("\n");

print("FIELDS BY NAME: \n");
checkSuccessString($cur->getField(0,"testtinyint"),"1");
checkSuccessString($cur->getField(0,"testsmallint"),"1");
checkSuccessString($cur->getField(0,"testmediumint"),"1");
checkSuccessString($cur->getField(0,"testint"),"1");
checkSuccessString($cur->getField(0,"testbigint"),"1");
#checkSuccessString($cur->getField(0,"testfloat"),"1.1");
checkSuccessString($cur->getField(0,"testreal"),"1.1");
checkSuccessString($cur->getField(0,"testdecimal"),"1.1");
checkSuccessString($cur->getField(0,"testdate"),"2001-01-01");
checkSuccessString($cur->getField(0,"testtime"),"01:00:00");
checkSuccessString($cur->getField(0,"testdatetime"),"2001-01-01 01:00:00");
checkSuccessString($cur->getField(0,"testyear"),"2001");
checkSuccessString($cur->getField(0,"testchar"),"char1");
checkSuccessString($cur->getField(0,"testtext"),"text1");
checkSuccessString($cur->getField(0,"testvarchar"),"varchar1");
checkSuccessString($cur->getField(0,"testtinytext"),"tinytext1");
checkSuccessString($cur->getField(0,"testmediumtext"),"mediumtext1");
checkSuccessString($cur->getField(0,"testlongtext"),"longtext1");
print("\n");
checkSuccessString($cur->getField(7,"testtinyint"),"8");
checkSuccessString($cur->getField(7,"testsmallint"),"8");
checkSuccessString($cur->getField(7,"testmediumint"),"8");
checkSuccessString($cur->getField(7,"testint"),"8");
checkSuccessString($cur->getField(7,"testbigint"),"8");
#checkSuccessString($cur->getField(7,"testfloat"),"8.1");
checkSuccessString($cur->getField(7,"testreal"),"8.1");
checkSuccessString($cur->getField(7,"testdecimal"),"8.1");
checkSuccessString($cur->getField(7,"testdate"),"2008-01-01");
checkSuccessString($cur->getField(7,"testtime"),"08:00:00");
checkSuccessString($cur->getField(7,"testdatetime"),"2008-01-01 08:00:00");
checkSuccessString($cur->getField(7,"testyear"),"2008");
checkSuccessString($cur->getField(7,"testchar"),"char8");
checkSuccessString($cur->getField(7,"testtext"),"text8");
checkSuccessString($cur->getField(7,"testvarchar"),"varchar8");
checkSuccessString($cur->getField(7,"testtinytext"),"tinytext8");
checkSuccessString($cur->getField(7,"testmediumtext"),"mediumtext8");
checkSuccessString($cur->getField(7,"testlongtext"),"longtext8");
print("\n");

print("FIELD LENGTHS BY NAME: \n");
checkSuccess($cur->getFieldLength(0,"testtinyint"),1);
checkSuccess($cur->getFieldLength(0,"testsmallint"),1);
checkSuccess($cur->getFieldLength(0,"testmediumint"),1);
checkSuccess($cur->getFieldLength(0,"testint"),1);
checkSuccess($cur->getFieldLength(0,"testbigint"),1);
#checkSuccess($cur->getFieldLength(0,"testfloat"),3);
checkSuccess($cur->getFieldLength(0,"testreal"),3);
checkSuccess($cur->getFieldLength(0,"testdecimal"),3);
checkSuccess($cur->getFieldLength(0,"testdate"),10);
checkSuccess($cur->getFieldLength(0,"testtime"),8);
checkSuccess($cur->getFieldLength(0,"testdatetime"),19);
checkSuccess($cur->getFieldLength(0,"testyear"),4);
checkSuccess($cur->getFieldLength(0,"testchar"),5);
checkSuccess($cur->getFieldLength(0,"testtext"),5);
checkSuccess($cur->getFieldLength(0,"testvarchar"),8);
checkSuccess($cur->getFieldLength(0,"testtinytext"),9);
checkSuccess($cur->getFieldLength(0,"testmediumtext"),11);
checkSuccess($cur->getFieldLength(0,"testlongtext"),9);
print("\n");
checkSuccess($cur->getFieldLength(7,"testtinyint"),1);
checkSuccess($cur->getFieldLength(7,"testsmallint"),1);
checkSuccess($cur->getFieldLength(7,"testmediumint"),1);
checkSuccess($cur->getFieldLength(7,"testint"),1);
checkSuccess($cur->getFieldLength(7,"testbigint"),1);
#checkSuccess($cur->getFieldLength(7,"testfloat"),3);
checkSuccess($cur->getFieldLength(7,"testreal"),3);
checkSuccess($cur->getFieldLength(7,"testdecimal"),3);
checkSuccess($cur->getFieldLength(7,"testdate"),10);
checkSuccess($cur->getFieldLength(7,"testtime"),8);
checkSuccess($cur->getFieldLength(7,"testdatetime"),19);
checkSuccess($cur->getFieldLength(7,"testyear"),4);
checkSuccess($cur->getFieldLength(7,"testchar"),5);
checkSuccess($cur->getFieldLength(7,"testtext"),5);
checkSuccess($cur->getFieldLength(7,"testvarchar"),8);
checkSuccess($cur->getFieldLength(7,"testtinytext"),9);
checkSuccess($cur->getFieldLength(7,"testmediumtext"),11);
checkSuccess($cur->getFieldLength(7,"testlongtext"),9);
print("\n");

print("FIELDS BY ARRAY: \n");
@fields=$cur->getRow(0);
checkSuccess($fields[0],1);
checkSuccess($fields[1],1);
checkSuccess($fields[2],1);
checkSuccess($fields[3],1);
checkSuccess($fields[4],1);
#checkSuccess($fields[5],1.1);
checkSuccess($fields[6],1.1);
checkSuccess($fields[7],1.1);
checkSuccessString($fields[8],"2001-01-01");
checkSuccessString($fields[9],"01:00:00");
checkSuccessString($fields[10],"2001-01-01 01:00:00");
checkSuccess($fields[11],2001);
checkSuccessString($fields[12],"char1");
checkSuccessString($fields[13],"text1");
checkSuccessString($fields[14],"varchar1");
checkSuccessString($fields[15],"tinytext1");
checkSuccessString($fields[16],"mediumtext1");
checkSuccessString($fields[17],"longtext1");
print("\n");

print("FIELD LENGTHS BY ARRAY: \n");
@fieldlens=$cur->getRowLengths(0);
checkSuccess($fieldlens[0],1);
checkSuccess($fieldlens[1],1);
checkSuccess($fieldlens[2],1);
checkSuccess($fieldlens[3],1);
checkSuccess($fieldlens[4],1);
#checkSuccess($fieldlens[5],3);
checkSuccess($fieldlens[6],3);
checkSuccess($fieldlens[7],3);
checkSuccess($fieldlens[8],10);
checkSuccess($fieldlens[9],8);
checkSuccess($fieldlens[10],19);
checkSuccess($fieldlens[11],4);
checkSuccess($fieldlens[12],5);
checkSuccess($fieldlens[13],5);
checkSuccess($fieldlens[14],8);
checkSuccess($fieldlens[15],9);
checkSuccess($fieldlens[16],11);
checkSuccess($fieldlens[17],9);
print("\n");

print("FIELDS BY HASH: \n");
%fields=$cur->getRowHash(0);
checkSuccess($fields{"testtinyint"},1);
checkSuccess($fields{"testsmallint"},1);
checkSuccess($fields{"testmediumint"},1);
checkSuccess($fields{"testint"},1);
checkSuccess($fields{"testbigint"},1);
#checkSuccess($fields{"testfloat"},1.1);
checkSuccess($fields{"testreal"},1.1);
checkSuccess($fields{"testdecimal"},1.1);
checkSuccessString($fields{"testdate"},"2001-01-01");
checkSuccessString($fields{"testtime"},"01:00:00");
checkSuccessString($fields{"testdatetime"},"2001-01-01 01:00:00");
checkSuccess($fields{"testyear"},2001);
checkSuccessString($fields{"testchar"},"char1");
checkSuccessString($fields{"testtext"},"text1");
checkSuccessString($fields{"testvarchar"},"varchar1");
checkSuccessString($fields{"testtinytext"},"tinytext1");
checkSuccessString($fields{"testmediumtext"},"mediumtext1");
checkSuccessString($fields{"testlongtext"},"longtext1");
print("\n");
%fields=$cur->getRowHash(7);
checkSuccess($fields{"testtinyint"},8);
checkSuccess($fields{"testsmallint"},8);
checkSuccess($fields{"testmediumint"},8);
checkSuccess($fields{"testint"},8);
checkSuccess($fields{"testbigint"},8);
#checkSuccess($fields{"testfloat"},8.1);
checkSuccess($fields{"testreal"},8.1);
checkSuccess($fields{"testdecimal"},8.1);
checkSuccessString($fields{"testdate"},"2008-01-01");
checkSuccessString($fields{"testtime"},"08:00:00");
checkSuccessString($fields{"testdatetime"},"2008-01-01 08:00:00");
checkSuccess($fields{"testyear"},2008);
checkSuccessString($fields{"testchar"},"char8");
checkSuccessString($fields{"testtext"},"text8");
checkSuccessString($fields{"testvarchar"},"varchar8");
checkSuccessString($fields{"testtinytext"},"tinytext8");
checkSuccessString($fields{"testmediumtext"},"mediumtext8");
checkSuccessString($fields{"testlongtext"},"longtext8");
print("\n");

print("FIELD LENGTHS BY HASH: \n");
%fieldlengths=$cur->getRowLengthsHash(0);
checkSuccess($fieldlengths{"testtinyint"},1);
checkSuccess($fieldlengths{"testsmallint"},1);
checkSuccess($fieldlengths{"testmediumint"},1);
checkSuccess($fieldlengths{"testint"},1);
checkSuccess($fieldlengths{"testbigint"},1);
#checkSuccess($fieldlengths{"testfloat"},3);
checkSuccess($fieldlengths{"testreal"},3);
checkSuccess($fieldlengths{"testdecimal"},3);
checkSuccess($fieldlengths{"testdate"},10);
checkSuccess($fieldlengths{"testtime"},8);
checkSuccess($fieldlengths{"testdatetime"},19);
checkSuccess($fieldlengths{"testyear"},4);
checkSuccess($fieldlengths{"testchar"},5);
checkSuccess($fieldlengths{"testtext"},5);
checkSuccess($fieldlengths{"testvarchar"},8);
checkSuccess($fieldlengths{"testtinytext"},9);
checkSuccess($fieldlengths{"testmediumtext"},11);
checkSuccess($fieldlengths{"testlongtext"},9);
print("\n");
%fieldlengths=$cur->getRowLengthsHash(7);
checkSuccess($fieldlengths{"testtinyint"},1);
checkSuccess($fieldlengths{"testsmallint"},1);
checkSuccess($fieldlengths{"testmediumint"},1);
checkSuccess($fieldlengths{"testint"},1);
checkSuccess($fieldlengths{"testbigint"},1);
#checkSuccess($fieldlengths{"testfloat"},3);
checkSuccess($fieldlengths{"testreal"},3);
checkSuccess($fieldlengths{"testdecimal"},3);
checkSuccess($fieldlengths{"testdate"},10);
checkSuccess($fieldlengths{"testtime"},8);
checkSuccess($fieldlengths{"testdatetime"},19);
checkSuccess($fieldlengths{"testyear"},4);
checkSuccess($fieldlengths{"testchar"},5);
checkSuccess($fieldlengths{"testtext"},5);
checkSuccess($fieldlengths{"testvarchar"},8);
checkSuccess($fieldlengths{"testtinytext"},9);
checkSuccess($fieldlengths{"testmediumtext"},11);
checkSuccess($fieldlengths{"testlongtext"},9);
print("\n");

print("INDIVIDUAL SUBSTITUTIONS: \n");
$cur->prepareQuery("select \$(var1),'\$(var2)',\$(var3)");
$cur->substitution("var1",1);
$cur->substitution("var2","hello");
$cur->substitution("var3",10.5556,6,4);
checkSuccess($cur->executeQuery(),1);
print("\n");

print("FIELDS: \n");
checkSuccessString($cur->getField(0,0),"1");
checkSuccessString($cur->getField(0,1),"hello");
checkSuccessString($cur->getField(0,2),"10.5556");
print("\n");

print("ARRAY SUBSTITUTIONS: \n");
$cur->prepareQuery("select \$(var1),'\$(var2)',\$(var3)");
@vars=("var1","var2","var3");
@vals=(1,"hello",10.5556);
@precs=(0,0,6);
@scales=(0,0,4);
$cur->substitutions(\@vars,\@vals,\@precs,\@scales);
checkSuccess($cur->executeQuery(),1);
print("\n");

print("FIELDS: \n");
checkSuccessString($cur->getField(0,0),"1");
checkSuccessString($cur->getField(0,1),"hello");
checkSuccessString($cur->getField(0,2),"10.5556");
print("\n");

print("NULLS as Undef: \n");
$cur->getNullsAsUndefined();
checkSuccess($cur->sendQuery("select NULL,1,NULL"),1);
checkUndef($cur->getField(0,0));
checkSuccessString($cur->getField(0,1),"1");
checkUndef($cur->getField(0,2));
$cur->getNullsAsEmptyStrings();
checkSuccess($cur->sendQuery("select NULL,1,NULL"),1);
checkSuccessString($cur->getField(0,0),"");
checkSuccessString($cur->getField(0,1),"1");
checkSuccessString($cur->getField(0,2),"");
$cur->getNullsAsUndefined();
print("\n");

print("RESULT SET BUFFER SIZE: \n");
checkSuccess($cur->getResultSetBufferSize(),0);
$cur->setResultSetBufferSize(2);
checkSuccess($cur->sendQuery("select * from testtable order by testtinyint"),1);
checkSuccess($cur->getResultSetBufferSize(),2);
print("\n");
checkSuccess($cur->firstRowIndex(),0);
checkSuccess($cur->endOfResultSet(),0);
checkSuccess($cur->rowCount(),2);
checkSuccessString($cur->getField(0,0),"1");
checkSuccessString($cur->getField(1,0),"2");
checkSuccessString($cur->getField(2,0),"3");
print("\n");
checkSuccess($cur->firstRowIndex(),2);
checkSuccess($cur->endOfResultSet(),0);
checkSuccess($cur->rowCount(),4);
checkSuccessString($cur->getField(6,0),"7");
checkSuccessString($cur->getField(7,0),"8");
print("\n");
checkSuccess($cur->firstRowIndex(),6);
checkSuccess($cur->endOfResultSet(),0);
checkSuccess($cur->rowCount(),8);
checkUndef($cur->getField(8,0));
print("\n");
checkSuccess($cur->firstRowIndex(),8);
checkSuccess($cur->endOfResultSet(),1);
checkSuccess($cur->rowCount(),8);
print("\n");

print("DONT GET COLUMN INFO: \n");
$cur->dontGetColumnInfo();
checkSuccess($cur->sendQuery("select * from testtable order by testtinyint"),1);
checkUndef($cur->getColumnName(0));
checkSuccess($cur->getColumnLength(0),0);
checkUndef($cur->getColumnType(0));
print("\n");
$cur->getColumnInfo();
checkSuccess($cur->sendQuery("select * from testtable order by testtinyint"),1);
checkSuccessString($cur->getColumnName(0),"testtinyint");
checkSuccess($cur->getColumnLength(0),1);
checkSuccessString($cur->getColumnType(0),"TINYINT");
print("\n");

print("SUSPENDED SESSION: \n");
checkSuccess($cur->sendQuery("select * from testtable order by testtinyint"),1);
$cur->suspendResultSet();
checkSuccess($con->suspendSession(),1);
$port=$con->getConnectionPort();
$socket=$con->getConnectionSocket();
checkSuccess($con->resumeSession($port,$socket),1);
print("\n");
checkSuccessString($cur->getField(0,0),"1");
checkSuccessString($cur->getField(1,0),"2");
checkSuccessString($cur->getField(2,0),"3");
checkSuccessString($cur->getField(3,0),"4");
checkSuccessString($cur->getField(4,0),"5");
checkSuccessString($cur->getField(5,0),"6");
checkSuccessString($cur->getField(6,0),"7");
checkSuccessString($cur->getField(7,0),"8");
print("\n");
checkSuccess($cur->sendQuery("select * from testtable order by testtinyint"),1);
$cur->suspendResultSet();
checkSuccess($con->suspendSession(),1);
$port=$con->getConnectionPort();
$socket=$con->getConnectionSocket();
checkSuccess($con->resumeSession($port,$socket),1);
print("\n");
checkSuccessString($cur->getField(0,0),"1");
checkSuccessString($cur->getField(1,0),"2");
checkSuccessString($cur->getField(2,0),"3");
checkSuccessString($cur->getField(3,0),"4");
checkSuccessString($cur->getField(4,0),"5");
checkSuccessString($cur->getField(5,0),"6");
checkSuccessString($cur->getField(6,0),"7");
checkSuccessString($cur->getField(7,0),"8");
print("\n");
checkSuccess($cur->sendQuery("select * from testtable order by testtinyint"),1);
$cur->suspendResultSet();
checkSuccess($con->suspendSession(),1);
$port=$con->getConnectionPort();
$socket=$con->getConnectionSocket();
checkSuccess($con->resumeSession($port,$socket),1);
print("\n");
checkSuccessString($cur->getField(0,0),"1");
checkSuccessString($cur->getField(1,0),"2");
checkSuccessString($cur->getField(2,0),"3");
checkSuccessString($cur->getField(3,0),"4");
checkSuccessString($cur->getField(4,0),"5");
checkSuccessString($cur->getField(5,0),"6");
checkSuccessString($cur->getField(6,0),"7");
checkSuccessString($cur->getField(7,0),"8");
print("\n");

print("SUSPENDED RESULT SET: \n");
$cur->setResultSetBufferSize(2);
checkSuccess($cur->sendQuery("select * from testtable order by testtinyint"),1);
checkSuccessString($cur->getField(2,0),"3");
$id=$cur->getResultSetId();
$cur->suspendResultSet();
checkSuccess($con->suspendSession(),1);
$port=$con->getConnectionPort();
$socket=$con->getConnectionSocket();
checkSuccess($con->resumeSession($port,$socket),1);
checkSuccess($cur->resumeResultSet($id),1);
print("\n");
checkSuccess($cur->firstRowIndex(),4);
checkSuccess($cur->endOfResultSet(),0);
checkSuccess($cur->rowCount(),6);
checkSuccessString($cur->getField(7,0),"8");
print("\n");
checkSuccess($cur->firstRowIndex(),6);
checkSuccess($cur->endOfResultSet(),0);
checkSuccess($cur->rowCount(),8);
checkUndef($cur->getField(8,0));
print("\n");
checkSuccess($cur->firstRowIndex(),8);
checkSuccess($cur->endOfResultSet(),1);
checkSuccess($cur->rowCount(),8);
$cur->setResultSetBufferSize(0);
print("\n");

print("CACHED RESULT SET: \n");
$cur->cacheToFile("cachefile1");
$cur->setCacheTtl(200);
checkSuccess($cur->sendQuery("select * from testtable order by testtinyint"),1);
$filename=$cur->getCacheFileName();
checkSuccessString($filename,"cachefile1");
$cur->cacheOff();
checkSuccess($cur->openCachedResultSet($filename),1);
checkSuccessString($cur->getField(7,0),"8");
print("\n");

print("COLUMN COUNT FOR CACHED RESULT SET: \n");
checkSuccess($cur->colCount(),19);
print("\n");

print("COLUMN NAMES FOR CACHED RESULT SET: \n");
checkSuccessString($cur->getColumnName(0),"testtinyint");
checkSuccessString($cur->getColumnName(1),"testsmallint");
checkSuccessString($cur->getColumnName(2),"testmediumint");
checkSuccessString($cur->getColumnName(3),"testint");
checkSuccessString($cur->getColumnName(4),"testbigint");
checkSuccessString($cur->getColumnName(5),"testfloat");
checkSuccessString($cur->getColumnName(6),"testreal");
checkSuccessString($cur->getColumnName(7),"testdecimal");
checkSuccessString($cur->getColumnName(8),"testdate");
checkSuccessString($cur->getColumnName(9),"testtime");
checkSuccessString($cur->getColumnName(10),"testdatetime");
checkSuccessString($cur->getColumnName(11),"testyear");
checkSuccessString($cur->getColumnName(12),"testchar");
checkSuccessString($cur->getColumnName(13),"testtext");
checkSuccessString($cur->getColumnName(14),"testvarchar");
checkSuccessString($cur->getColumnName(15),"testtinytext");
checkSuccessString($cur->getColumnName(16),"testmediumtext");
checkSuccessString($cur->getColumnName(17),"testlongtext");
@cols=$cur->getColumnNames();
checkSuccessString($cols[0],"testtinyint");
checkSuccessString($cols[1],"testsmallint");
checkSuccessString($cols[2],"testmediumint");
checkSuccessString($cols[3],"testint");
checkSuccessString($cols[4],"testbigint");
checkSuccessString($cols[5],"testfloat");
checkSuccessString($cols[6],"testreal");
checkSuccessString($cols[7],"testdecimal");
checkSuccessString($cols[8],"testdate");
checkSuccessString($cols[9],"testtime");
checkSuccessString($cols[10],"testdatetime");
checkSuccessString($cols[11],"testyear");
checkSuccessString($cols[12],"testchar");
checkSuccessString($cols[13],"testtext");
checkSuccessString($cols[14],"testvarchar");
checkSuccessString($cols[15],"testtinytext");
checkSuccessString($cols[16],"testmediumtext");
checkSuccessString($cols[17],"testlongtext");
print("\n");

print("CACHED RESULT SET WITH RESULT SET BUFFER SIZE: \n");
$cur->setResultSetBufferSize(2);
$cur->cacheToFile("cachefile1");
$cur->setCacheTtl(200);
checkSuccess($cur->sendQuery("select * from testtable order by testtinyint"),1);
$filename=$cur->getCacheFileName();
checkSuccessString($filename,"cachefile1");
$cur->cacheOff();
checkSuccess($cur->openCachedResultSet($filename),1);
checkSuccessString($cur->getField(7,0),"8");
checkUndef($cur->getField(8,0));
$cur->setResultSetBufferSize(0);
print("\n");

print("FROM ONE CACHE FILE TO ANOTHER: \n");
$cur->cacheToFile("cachefile2");
checkSuccess($cur->openCachedResultSet("cachefile1"),1);
$cur->cacheOff();
checkSuccess($cur->openCachedResultSet("cachefile2"),1);
checkSuccessString($cur->getField(7,0),"8");
checkUndef($cur->getField(8,0));
print("\n");

print("FROM ONE CACHE FILE TO ANOTHER WITH RESULT SET BUFFER SIZE: \n");
$cur->setResultSetBufferSize(2);
$cur->cacheToFile("cachefile2");
checkSuccess($cur->openCachedResultSet("cachefile1"),1);
$cur->cacheOff();
checkSuccess($cur->openCachedResultSet("cachefile2"),1);
checkSuccessString($cur->getField(7,0),"8");
checkUndef($cur->getField(8,0));
$cur->setResultSetBufferSize(0);
print("\n");

print("CACHED RESULT SET WITH SUSPEND AND RESULT SET BUFFER SIZE: \n");
$cur->setResultSetBufferSize(2);
$cur->cacheToFile("cachefile1");
$cur->setCacheTtl(200);
checkSuccess($cur->sendQuery("select * from testtable order by testtinyint"),1);
checkSuccessString($cur->getField(2,0),"3");
$filename=$cur->getCacheFileName();
checkSuccessString($filename,"cachefile1");
$id=$cur->getResultSetId();
$cur->suspendResultSet();
checkSuccess($con->suspendSession(),1);
$port=$con->getConnectionPort();
$socket=$con->getConnectionSocket();
print("\n");
checkSuccess($con->resumeSession($port,$socket),1);
checkSuccess($cur->resumeCachedResultSet($id,$filename),1);
print("\n");
checkSuccess($cur->firstRowIndex(),4);
checkSuccess($cur->endOfResultSet(),0);
checkSuccess($cur->rowCount(),6);
checkSuccessString($cur->getField(7,0),"8");
print("\n");
checkSuccess($cur->firstRowIndex(),6);
checkSuccess($cur->endOfResultSet(),0);
checkSuccess($cur->rowCount(),8);
checkUndef($cur->getField(8,0));
print("\n");
checkSuccess($cur->firstRowIndex(),8);
checkSuccess($cur->endOfResultSet(),1);
checkSuccess($cur->rowCount(),8);
$cur->cacheOff();
print("\n");
checkSuccess($cur->openCachedResultSet($filename),1);
checkSuccessString($cur->getField(7,0),"8");
checkUndef($cur->getField(8,0));
$cur->setResultSetBufferSize(0);
print("\n");

print("COMMIT AND ROLLBACK: \n");
# Note: Mysql's default isolation level is repeatable-read,
# not read-committed like most other db's.  Both sessions must
# commit to see the changes that each other has made.
$secondcon=SQLRelay::Connection->new("sqlrelay",9000,"/tmp/test.socket",
						"testuser","testpassword",0,1);
$secondcur=SQLRelay::Cursor->new($secondcon);
checkSuccess($secondcur->sendQuery("select count(*) from testtable"),1);
if ($majorversion>3) {
	checkSuccessString($secondcur->getField(0,0),"0");
} else {
	checkSuccessString($secondcur->getField(0,0),"8");
}
checkSuccess($con->commit(),1);
checkSuccess($secondcon->commit(),1);
checkSuccess($secondcur->sendQuery("select count(*) from testtable"),1);
checkSuccessString($secondcur->getField(0,0),"8");
checkSuccess($con->autoCommitOn(),1);
checkSuccess($cur->sendQuery("insert into testtable values (10,10,10,10,10,10.1,10.1,1.1,'2010-01-01','10:00:00','2010-01-01 10:00:00','2010','char10','text10','varchar10','tinytext10','mediumtext10','longtext10',NULL)"),1);
checkSuccess($secondcon->commit(),1);
checkSuccess($secondcur->sendQuery("select count(*) from testtable"),1);
checkSuccessString($secondcur->getField(0,0),"9");
checkSuccess($con->autoCommitOff(),1);
$secondcon->commit();
print("\n");

print("FINISHED SUSPENDED SESSION: \n");
checkSuccess($cur->sendQuery("select * from testtable order by testint"),1);
checkSuccessString($cur->getField(4,0),"5");
checkSuccessString($cur->getField(5,0),"6");
checkSuccessString($cur->getField(6,0),"7");
checkSuccessString($cur->getField(7,0),"8");
$id=$cur->getResultSetId();
$cur->suspendResultSet();
checkSuccess($con->suspendSession(),1);
$port=$con->getConnectionPort();
$socket=$con->getConnectionSocket();
checkSuccess($con->resumeSession($port,$socket),1);
checkSuccess($cur->resumeResultSet($id),1);
checkUndef($cur->getField(4,0));
checkUndef($cur->getField(5,0));
checkUndef($cur->getField(6,0));
checkUndef($cur->getField(7,0));
print("\n");

# drop existing table
$cur->sendQuery("drop table testtable");

# invalid queries...
print("INVALID QUERIES: \n");
checkSuccess($cur->sendQuery("select * from testtable order by testtinyint"),0);
checkSuccess($cur->sendQuery("select * from testtable order by testtinyint"),0);
checkSuccess($cur->sendQuery("select * from testtable order by testtinyint"),0);
checkSuccess($cur->sendQuery("select * from testtable order by testtinyint"),0);
print("\n");
checkSuccess($cur->sendQuery("insert into testtable values (1,2,3,4)"),0);
checkSuccess($cur->sendQuery("insert into testtable values (1,2,3,4)"),0);
checkSuccess($cur->sendQuery("insert into testtable values (1,2,3,4)"),0);
checkSuccess($cur->sendQuery("insert into testtable values (1,2,3,4)"),0);
print("\n");
checkSuccess($cur->sendQuery("create table testtable"),0);
checkSuccess($cur->sendQuery("create table testtable"),0);
checkSuccess($cur->sendQuery("create table testtable"),0);
checkSuccess($cur->sendQuery("create table testtable"),0);
print("\n");

exit(0);
