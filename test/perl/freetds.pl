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
							"test","test",0,1);
$cur=SQLRelay::Cursor->new($con);

# get database type
print("IDENTIFY: \n");
checkSuccessString($con->identify(),"freetds");
print("\n");

# ping
print("PING: \n");
checkSuccess($con->ping(),1);
print("\n");


# drop existing table
$cur->sendQuery("drop table testtable");


print("CREATE TEMPTABLE: \n");
checkSuccess($cur->sendQuery("create table testtable (testint int, testsmallint smallint, testtinyint tinyint, testreal real, testfloat float, testdecimal decimal(4,1), testnumeric numeric(4,1), testmoney money, testsmallmoney smallmoney, testdatetime datetime, testsmalldatetime smalldatetime, testchar char(40), testvarchar varchar(40), testbit bit)"),1);
print("\n");

print("BEGIN TRANSACTION: \n");
checkSuccess($cur->sendQuery("begin tran"),1);
print("\n");

print("INSERT: \n");
checkSuccess($cur->sendQuery("insert into testtable values (1,1,1,1.1,1.1,1.1,1.1,1.00,1.00,'01-Jan-2001 01:00:00','01-Jan-2001 01:00:00','testchar1','testvarchar1',1)"),1);
print("\n");

print("AFFECTED ROWS: \n");
checkSuccess($cur->affectedRows(),1);
print("\n");

print("BIND BY POSITION: \n");
$cur->prepareQuery("insert into testtable values (?,?,?,?,?,?,?,?,?,?,?,?,?,?)");
checkSuccess($cur->countBindVariables(),14);
$cur->inputBind("1",2);
$cur->inputBind("2",2);
$cur->inputBind("3",2);
$cur->inputBind("4",2.2,2,1);
$cur->inputBind("5",2.2,2,1);
$cur->inputBind("6",2.2,2,1);
$cur->inputBind("7",2.2,2,1);
$cur->inputBind("8",2.00,3,2);
$cur->inputBind("9",2.00,3,2);
$cur->inputBind("10","01-Jan-2002 02:00:00");
$cur->inputBind("11","01-Jan-2002 02:00:00");
$cur->inputBind("12","testchar2");
$cur->inputBind("13","testvarchar2");
$cur->inputBind("14",1);
checkSuccess($cur->executeQuery(),1);
$cur->clearBinds();
$cur->inputBind("1",3);
$cur->inputBind("2",3);
$cur->inputBind("3",3);
$cur->inputBind("4",3.3,2,1);
$cur->inputBind("5",3.3,2,1);
$cur->inputBind("6",3.3,2,1);
$cur->inputBind("7",3.3,2,1);
$cur->inputBind("8",3.00,3,2);
$cur->inputBind("9",3.00,3,2);
$cur->inputBind("10","01-Jan-2003 03:00:00");
$cur->inputBind("11","01-Jan-2003 03:00:00");
$cur->inputBind("12","testchar3");
$cur->inputBind("13","testvarchar3");
$cur->inputBind("14",1);
checkSuccess($cur->executeQuery(),1);
print("\n");

print("ARRAY OF BINDS BY POSITION: \n");
$cur->clearBinds();
@vars=("1","2","3","4","5","6","7","8","9","10","11","12","13","14");
@vals=(4,4,4,4.4,4.4,4.4,4.4,4.00,4.00,"01-Jan-2004 04:00:00",
	"01-Jan-2004 04:00:00","testchar4","testvarchar4",1);
@precs=(0,0,0,2,2,2,2,3,3,0,0,0,0,0);
@scales=(0,0,0,1,1,1,1,2,2,0,0,0,0,0);
$cur->inputBinds(\@vars,\@vals,\@precs,\@scales);
checkSuccess($cur->executeQuery(),1);
print("\n");

print("BIND BY NAME: \n");
$cur->clearBinds();
$cur->prepareQuery("insert into testtable values (\@var1,\@var2,\@var3,\@var4,\@var5,\@var6,\@var7,\@var8,\@var9,\@var10,\@var11,\@var12,\@var13,\@var14)");
$cur->inputBind("var1",5);
$cur->inputBind("var2",5);
$cur->inputBind("var3",5);
$cur->inputBind("var4",5.5,2,1);
$cur->inputBind("var5",5.5,2,1);
$cur->inputBind("var6",5.5,2,1);
$cur->inputBind("var7",5.5,2,1);
$cur->inputBind("var8",5.00,3,2);
$cur->inputBind("var9",5.00,3,2);
$cur->inputBind("var10","01-Jan-2005 05:00:00");
$cur->inputBind("var11","01-Jan-2005 05:00:00");
$cur->inputBind("var12","testchar5");
$cur->inputBind("var13","testvarchar5");
$cur->inputBind("var14",1);
checkSuccess($cur->executeQuery(),1);
$cur->clearBinds();
$cur->inputBind("var1",6);
$cur->inputBind("var2",6);
$cur->inputBind("var3",6);
$cur->inputBind("var4",6.6,2,1);
$cur->inputBind("var5",6.6,2,1);
$cur->inputBind("var6",6.6,2,1);
$cur->inputBind("var7",6.6,2,1);
$cur->inputBind("var8",6.00,3,2);
$cur->inputBind("var9",6.00,3,2);
$cur->inputBind("var10","01-Jan-2006 06:00:00");
$cur->inputBind("var11","01-Jan-2006 06:00:00");
$cur->inputBind("var12","testchar6");
$cur->inputBind("var13","testvarchar6");
$cur->inputBind("var14",1);
checkSuccess($cur->executeQuery(),1);
print("\n");

print("ARRAY OF BINDS BY NAME: \n");
$cur->clearBinds();
@vars=("var1","var2","var3","var4","var5","var6",
		"var7","var8","var9","var10","var11","var12","var13","var14");
@vals=(7,7,7,7.7,7.7,7.7,7.7,7.00,7.00,
	"01-Jan-2007 07:00:00","01-Jan-2007 07:00:00",
	"testchar7","testvarchar7",1);
@precs=(0,0,0,2,2,2,2,3,3,0,0,0,0,0);
@scales=(0,0,0,1,1,1,1,2,2,0,0,0,0,0);
$cur->inputBinds(\@vars,\@vals,\@precs,\@scales);
checkSuccess($cur->executeQuery(),1);
print("\n");

print("BIND BY NAME WITH VALIDATION: \n");
$cur->clearBinds();
$cur->inputBind("var1",8);
$cur->inputBind("var2",8);
$cur->inputBind("var3",8);
$cur->inputBind("var4",8.8,2,1);
$cur->inputBind("var5",8.8,2,1);
$cur->inputBind("var6",8.8,2,1);
$cur->inputBind("var7",8.8,2,1);
$cur->inputBind("var8",8.00,3,2);
$cur->inputBind("var9",8.00,3,2);
$cur->inputBind("var10","01-Jan-2008 08:00:00");
$cur->inputBind("var11","01-Jan-2008 08:00:00");
$cur->inputBind("var12","testchar8                               ");
$cur->inputBind("var13","testvarchar8");
$cur->inputBind("var14",1);
$cur->inputBind("var15","junkvalue");
$cur->validateBinds();
checkSuccess($cur->executeQuery(),1);
print("\n");

print("SELECT: \n");
checkSuccess($cur->sendQuery("select * from testtable order by testint"),1);
print("\n");

print("COLUMN COUNT: \n");
checkSuccess($cur->colCount(),14);
print("\n");

print("COLUMN NAMES: \n");
checkSuccessString($cur->getColumnName(0),"testint");
checkSuccessString($cur->getColumnName(1),"testsmallint");
checkSuccessString($cur->getColumnName(2),"testtinyint");
checkSuccessString($cur->getColumnName(3),"testreal");
checkSuccessString($cur->getColumnName(4),"testfloat");
checkSuccessString($cur->getColumnName(5),"testdecimal");
checkSuccessString($cur->getColumnName(6),"testnumeric");
checkSuccessString($cur->getColumnName(7),"testmoney");
checkSuccessString($cur->getColumnName(8),"testsmallmoney");
checkSuccessString($cur->getColumnName(9),"testdatetime");
checkSuccessString($cur->getColumnName(10),"testsmalldatetime");
checkSuccessString($cur->getColumnName(11),"testchar");
checkSuccessString($cur->getColumnName(12),"testvarchar");
checkSuccessString($cur->getColumnName(13),"testbit");
@cols=$cur->getColumnNames();
checkSuccessString($cols[0],"testint");
checkSuccessString($cols[1],"testsmallint");
checkSuccessString($cols[2],"testtinyint");
checkSuccessString($cols[3],"testreal");
checkSuccessString($cols[4],"testfloat");
checkSuccessString($cols[5],"testdecimal");
checkSuccessString($cols[6],"testnumeric");
checkSuccessString($cols[7],"testmoney");
checkSuccessString($cols[8],"testsmallmoney");
checkSuccessString($cols[9],"testdatetime");
checkSuccessString($cols[10],"testsmalldatetime");
checkSuccessString($cols[11],"testchar");
checkSuccessString($cols[12],"testvarchar");
checkSuccessString($cols[13],"testbit");
print("\n");

print("COLUMN TYPES: \n");
checkSuccessString($cur->getColumnType(0),"INT");
checkSuccessString($cur->getColumnType('testint'),"INT");
checkSuccessString($cur->getColumnType(1),"SMALLINT");
checkSuccessString($cur->getColumnType('testsmallint'),"SMALLINT");
checkSuccessString($cur->getColumnType(2),"TINYINT");
checkSuccessString($cur->getColumnType('testtinyint'),"TINYINT");
checkSuccessString($cur->getColumnType(3),"REAL");
checkSuccessString($cur->getColumnType('testreal'),"REAL");
checkSuccessString($cur->getColumnType(4),"FLOAT");
checkSuccessString($cur->getColumnType('testfloat'),"FLOAT");
checkSuccessString($cur->getColumnType(5),"DECIMAL");
checkSuccessString($cur->getColumnType('testdecimal'),"DECIMAL");
checkSuccessString($cur->getColumnType(6),"NUMERIC");
checkSuccessString($cur->getColumnType('testnumeric'),"NUMERIC");
checkSuccessString($cur->getColumnType(7),"MONEY");
checkSuccessString($cur->getColumnType('testmoney'),"MONEY");
checkSuccessString($cur->getColumnType(8),"SMALLMONEY");
checkSuccessString($cur->getColumnType('testsmallmoney'),"SMALLMONEY");
checkSuccessString($cur->getColumnType(9),"DATETIME");
checkSuccessString($cur->getColumnType('testdatetime'),"DATETIME");
checkSuccessString($cur->getColumnType(10),"SMALLDATETIME");
checkSuccessString($cur->getColumnType('testsmalldatetime'),"SMALLDATETIME");
checkSuccessString($cur->getColumnType(11),"CHAR");
checkSuccessString($cur->getColumnType('testchar'),"CHAR");
checkSuccessString($cur->getColumnType(12),"CHAR");
checkSuccessString($cur->getColumnType('testvarchar'),"CHAR");
checkSuccessString($cur->getColumnType(13),"BIT");
checkSuccessString($cur->getColumnType('testbit'),"BIT");
print("\n");

print("COLUMN LENGTH: \n");
checkSuccess($cur->getColumnLength(0),4);
checkSuccess($cur->getColumnLength('testint'),4);
checkSuccess($cur->getColumnLength(1),2);
checkSuccess($cur->getColumnLength('testsmallint'),2);
checkSuccess($cur->getColumnLength(2),1);
checkSuccess($cur->getColumnLength('testtinyint'),1);
checkSuccess($cur->getColumnLength(3),4);
checkSuccess($cur->getColumnLength('testreal'),4);
checkSuccess($cur->getColumnLength(4),8);
checkSuccess($cur->getColumnLength('testfloat'),8);
# these seem to fluctuate with every freetds release
#checkSuccess($cur->getColumnLength(5),3);
#checkSuccess($cur->getColumnLength('testdecimal'),3);
#checkSuccess($cur->getColumnLength(6),3);
#checkSuccess($cur->getColumnLength('testnumeric'),3);
checkSuccess($cur->getColumnLength(7),8);
checkSuccess($cur->getColumnLength('testmoney'),8);
checkSuccess($cur->getColumnLength(8),4);
checkSuccess($cur->getColumnLength('testsmallmoney'),4);
checkSuccess($cur->getColumnLength(9),8);
checkSuccess($cur->getColumnLength('testdatetime'),8);
checkSuccess($cur->getColumnLength(10),4);
checkSuccess($cur->getColumnLength('testsmalldatetime'),4);
# these seem to fluctuate too
#checkSuccess($cur->getColumnLength(11),40);
#checkSuccess($cur->getColumnLength('testchar'),40);
#checkSuccess($cur->getColumnLength(12),40);
#checkSuccess($cur->getColumnLength('testvarchar'),40);
checkSuccess($cur->getColumnLength(13),1);
checkSuccess($cur->getColumnLength('testbit'),1);
print("\n");

print("LONGEST COLUMN: \n");
checkSuccess($cur->getLongest(0),1);
checkSuccess($cur->getLongest('testint'),1);
checkSuccess($cur->getLongest(1),1);
checkSuccess($cur->getLongest('testsmallint'),1);
checkSuccess($cur->getLongest(2),1);
checkSuccess($cur->getLongest('testtinyint'),1);
#checkSuccess($cur->getLongest(3),3);
#checkSuccess($cur->getLongest('testreal'),3);
#checkSuccess($cur->getLongest(4),17);
#checkSuccess($cur->getLongest('testfloat'),17);
checkSuccess($cur->getLongest(5),3);
checkSuccess($cur->getLongest('testdecimal'),3);
checkSuccess($cur->getLongest(6),3);
checkSuccess($cur->getLongest('testnumeric'),3);
checkSuccess($cur->getLongest(7),4);
checkSuccess($cur->getLongest('testmoney'),4);
checkSuccess($cur->getLongest(8),4);
checkSuccess($cur->getLongest('testsmallmoney'),4);
#checkSuccess($cur->getLongest(9),26);
#checkSuccess($cur->getLongest('testdatetime'),26);
#checkSuccess($cur->getLongest(10),26);
#checkSuccess($cur->getLongest('testsmalldatetime'),26);
checkSuccess($cur->getLongest(11),40);
checkSuccess($cur->getLongest('testchar'),40);
checkSuccess($cur->getLongest(12),12);
checkSuccess($cur->getLongest('testvarchar'),12);
checkSuccess($cur->getLongest(13),1);
checkSuccess($cur->getLongest('testbit'),1);
print("\n");

print("ROW COUNT: \n");
checkSuccess($cur->rowCount(),8);
print("\n");

print("TOTAL ROWS: \n");
checkSuccess($cur->totalRows(),0);
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
#checkSuccessString($cur->getField(0,3),"1.1");
#checkSuccessString($cur->getField(0,4),"1.1");
checkSuccessString($cur->getField(0,5),"1.1");
checkSuccessString($cur->getField(0,6),"1.1");
checkSuccessString($cur->getField(0,7),"1.00");
checkSuccessString($cur->getField(0,8),"1.00");
#checkSuccessString($cur->getField(0,9),"Jan  1 2001 01:00:00:000AM");
#checkSuccessString($cur->getField(0,10),"Jan  1 2001 01:00:00:000AM");
checkSuccessString($cur->getField(0,11),"testchar1                               ");
checkSuccessString($cur->getField(0,12),"testvarchar1");
checkSuccessString($cur->getField(0,13),"1");
print("\n");
checkSuccessString($cur->getField(7,0),"8");
checkSuccessString($cur->getField(7,1),"8");
checkSuccessString($cur->getField(7,2),"8");
#checkSuccessString($cur->getField(7,3),"8.8");
#checkSuccessString($cur->getField(7,4),"8.8");
#checkSuccessString($cur->getField(7,5),"8.8");
#checkSuccessString($cur->getField(7,6),"8.8");
checkSuccessString($cur->getField(7,7),"8.00");
checkSuccessString($cur->getField(7,8),"8.00");
#checkSuccessString($cur->getField(7,9),"Jan  1 2008 08:00:00:000AM");
#checkSuccessString($cur->getField(7,10),"Jan  1 2008 08:00:00:000AM");
checkSuccessString($cur->getField(7,11),"testchar8                               ");
checkSuccessString($cur->getField(7,12),"testvarchar8");
checkSuccessString($cur->getField(7,13),"1");
print("\n");

print("FIELD LENGTHS BY INDEX: \n");
checkSuccess($cur->getFieldLength(0,0),1);
checkSuccess($cur->getFieldLength(0,1),1);
checkSuccess($cur->getFieldLength(0,2),1);
#checkSuccess($cur->getFieldLength(0,3),3);
#checkSuccess($cur->getFieldLength(0,4),3);
checkSuccess($cur->getFieldLength(0,5),3);
checkSuccess($cur->getFieldLength(0,6),3);
checkSuccess($cur->getFieldLength(0,7),4);
checkSuccess($cur->getFieldLength(0,8),4);
#checkSuccess($cur->getFieldLength(0,9),26);
#checkSuccess($cur->getFieldLength(0,10),26);
checkSuccess($cur->getFieldLength(0,11),40);
checkSuccess($cur->getFieldLength(0,12),12);
checkSuccess($cur->getFieldLength(0,13),1);
print("\n");
checkSuccess($cur->getFieldLength(7,0),1);
checkSuccess($cur->getFieldLength(7,1),1);
checkSuccess($cur->getFieldLength(7,2),1);
#checkSuccess($cur->getFieldLength(7,3),3);
#checkSuccess($cur->getFieldLength(7,4),17);
checkSuccess($cur->getFieldLength(7,5),3);
checkSuccess($cur->getFieldLength(7,6),3);
checkSuccess($cur->getFieldLength(7,7),4);
checkSuccess($cur->getFieldLength(7,8),4);
#checkSuccess($cur->getFieldLength(7,9),26);
#checkSuccess($cur->getFieldLength(7,10),26);
checkSuccess($cur->getFieldLength(7,11),40);
checkSuccess($cur->getFieldLength(7,12),12);
checkSuccess($cur->getFieldLength(7,13),1);
print("\n");

print("FIELDS BY NAME: \n");
checkSuccessString($cur->getField(0,"testint"),"1");
checkSuccessString($cur->getField(0,"testsmallint"),"1");
checkSuccessString($cur->getField(0,"testtinyint"),"1");
#checkSuccessString($cur->getField(0,"testreal"),"1.1");
#checkSuccessString($cur->getField(0,"testfloat"),"1.1");
checkSuccessString($cur->getField(0,"testdecimal"),"1.1");
checkSuccessString($cur->getField(0,"testnumeric"),"1.1");
checkSuccessString($cur->getField(0,"testmoney"),"1.00");
checkSuccessString($cur->getField(0,"testsmallmoney"),"1.00");
#checkSuccessString($cur->getField(0,"testdatetime"),"Jan  1 2001 01:00:00:000AM");
#checkSuccessString($cur->getField(0,"testsmalldatetime"),"Jan  1 2001 01:00:00:000AM");
checkSuccessString($cur->getField(0,"testchar"),"testchar1                               ");
checkSuccessString($cur->getField(0,"testvarchar"),"testvarchar1");
checkSuccessString($cur->getField(0,"testbit"),"1");
print("\n");
checkSuccessString($cur->getField(7,"testint"),"8");
checkSuccessString($cur->getField(7,"testsmallint"),"8");
checkSuccessString($cur->getField(7,"testtinyint"),"8");
#checkSuccessString($cur->getField(7,"testreal"),"8.8");
#checkSuccessString($cur->getField(7,"testfloat"),"8.8");
#checkSuccessString($cur->getField(7,"testdecimal"),"8.8");
#checkSuccessString($cur->getField(7,"testnumeric"),"8.8");
checkSuccessString($cur->getField(7,"testmoney"),"8.00");
checkSuccessString($cur->getField(7,"testsmallmoney"),"8.00");
#checkSuccessString($cur->getField(7,"testdatetime"),"Jan  1 2008 08:00:00:000AM");
#checkSuccessString($cur->getField(7,"testsmalldatetime"),"Jan  1 2008 08:00:00:000AM");
checkSuccessString($cur->getField(7,"testchar"),"testchar8                               ");
checkSuccessString($cur->getField(7,"testvarchar"),"testvarchar8");
checkSuccessString($cur->getField(7,"testbit"),"1");
print("\n");

print("FIELD LENGTHS BY NAME: \n");
checkSuccess($cur->getFieldLength(0,"testint"),1);
checkSuccess($cur->getFieldLength(0,"testsmallint"),1);
checkSuccess($cur->getFieldLength(0,"testtinyint"),1);
#checkSuccess($cur->getFieldLength(0,"testreal"),3);
#checkSuccess($cur->getFieldLength(0,"testfloat"),3);
checkSuccess($cur->getFieldLength(0,"testdecimal"),3);
checkSuccess($cur->getFieldLength(0,"testnumeric"),3);
checkSuccess($cur->getFieldLength(0,"testmoney"),4);
checkSuccess($cur->getFieldLength(0,"testsmallmoney"),4);
#checkSuccess($cur->getFieldLength(0,"testdatetime"),26);
#checkSuccess($cur->getFieldLength(0,"testsmalldatetime"),26);
checkSuccess($cur->getFieldLength(0,"testchar"),40);
checkSuccess($cur->getFieldLength(0,"testvarchar"),12);
checkSuccess($cur->getFieldLength(0,"testbit"),1);
print("\n");
checkSuccess($cur->getFieldLength(7,"testint"),1);
checkSuccess($cur->getFieldLength(7,"testsmallint"),1);
checkSuccess($cur->getFieldLength(7,"testtinyint"),1);
#checkSuccess($cur->getFieldLength(7,"testreal"),3);
#checkSuccess($cur->getFieldLength(7,"testfloat"),17);
checkSuccess($cur->getFieldLength(7,"testdecimal"),3);
checkSuccess($cur->getFieldLength(7,"testnumeric"),3);
checkSuccess($cur->getFieldLength(7,"testmoney"),4);
checkSuccess($cur->getFieldLength(7,"testsmallmoney"),4);
#checkSuccess($cur->getFieldLength(7,"testdatetime"),26);
#checkSuccess($cur->getFieldLength(7,"testsmalldatetime"),26);
checkSuccess($cur->getFieldLength(7,"testchar"),40);
checkSuccess($cur->getFieldLength(7,"testvarchar"),12);
checkSuccess($cur->getFieldLength(7,"testbit"),1);
print("\n");

print("FIELDS BY ARRAY: \n");
@fields=$cur->getRow(0);
checkSuccess($fields[0],1);
checkSuccess($fields[1],1);
checkSuccess($fields[2],1);
#checkSuccess($fields[3],1.1);
#checkSuccess($fields[4],1.1);
checkSuccess($fields[5],1.1);
checkSuccess($fields[6],1.1);
checkSuccess($fields[7],1.0);
checkSuccess($fields[8],1.0);
#checkSuccessString($fields[9],"Jan  1 2001 01:00:00:000AM");
#checkSuccessString($fields[10],"Jan  1 2001 01:00:00:000AM");
checkSuccessString($fields[11],"testchar1                               ");
checkSuccessString($fields[12],"testvarchar1");
checkSuccess($fields[13],1);
print("\n");

print("FIELD LENGTHS BY ARRAY: \n");
@fieldlens=$cur->getRowLengths(0);
checkSuccess($fieldlens[0],1);
checkSuccess($fieldlens[1],1);
checkSuccess($fieldlens[2],1);
#checkSuccess($fieldlens[3],3);
#checkSuccess($fieldlens[4],3);
checkSuccess($fieldlens[5],3);
checkSuccess($fieldlens[6],3);
checkSuccess($fieldlens[7],4);
checkSuccess($fieldlens[8],4);
#checkSuccess($fieldlens[9],26);
#checkSuccess($fieldlens[10],26);
checkSuccess($fieldlens[11],40);
checkSuccess($fieldlens[12],12);
checkSuccess($fieldlens[13],1);
print("\n");

print("FIELDS BY HASH: \n");
%fields=$cur->getRowHash(0);
checkSuccess($fields{"testint"},1);
checkSuccess($fields{"testsmallint"},1);
checkSuccess($fields{"testtinyint"},1);
#checkSuccess($fields{"testreal"},1.1);
#checkSuccess($fields{"testfloat"},1.1);
checkSuccess($fields{"testdecimal"},1.1);
checkSuccess($fields{"testnumeric"},1.1);
checkSuccess($fields{"testmoney"},1.0);
checkSuccess($fields{"testsmallmoney"},1.0);
#checkSuccessString($fields{"testdatetime"},"Jan  1 2001 01:00:00:000AM");
#checkSuccessString($fields{"testsmalldatetime"},"Jan  1 2001 01:00:00:000AM");
checkSuccessString($fields{"testchar"},"testchar1                               ");
checkSuccessString($fields{"testvarchar"},"testvarchar1");
checkSuccess($fields{"testbit"},1);
print("\n");
%fields=$cur->getRowHash(7);
checkSuccess($fields{"testint"},8);
checkSuccess($fields{"testsmallint"},8);
checkSuccess($fields{"testtinyint"},8);
#checkSuccess($fields{"testreal"},8.8);
#checkSuccess($fields{"testfloat"},8.8);
#checkSuccess($fields{"testdecimal"},8.8);
#checkSuccess($fields{"testnumeric"},8.8);
checkSuccess($fields{"testmoney"},8.0);
checkSuccess($fields{"testsmallmoney"},8.0);
#checkSuccessString($fields{"testdatetime"},"Jan  1 2008 08:00:00:000AM");
#checkSuccessString($fields{"testsmalldatetime"},"Jan  1 2008 08:00:00:000AM");
checkSuccessString($fields{"testchar"},"testchar8                               ");
checkSuccessString($fields{"testvarchar"},"testvarchar8");
checkSuccess($fields{"testbit"},1);
print("\n");

print("FIELD LENGTHS BY HASH: \n");
%fieldlengths=$cur->getRowLengthsHash(0);
checkSuccess($fieldlengths{"testint"},1);
checkSuccess($fieldlengths{"testsmallint"},1);
checkSuccess($fieldlengths{"testtinyint"},1);
#checkSuccess($fieldlengths{"testreal"},3);
#checkSuccess($fieldlengths{"testfloat"},3);
checkSuccess($fieldlengths{"testdecimal"},3);
checkSuccess($fieldlengths{"testnumeric"},3);
checkSuccess($fieldlengths{"testmoney"},4);
checkSuccess($fieldlengths{"testsmallmoney"},4);
#checkSuccess($fieldlengths{"testdatetime"},26);
#checkSuccess($fieldlengths{"testsmalldatetime"},26);
checkSuccess($fieldlengths{"testchar"},40);
checkSuccess($fieldlengths{"testvarchar"},12);
checkSuccess($fieldlengths{"testbit"},1);
print("\n");
%fieldlengths=$cur->getRowLengthsHash(7);
checkSuccess($fieldlengths{"testsmallint"},1);
checkSuccess($fieldlengths{"testtinyint"},1);
#checkSuccess($fieldlengths{"testreal"},3);
#checkSuccess($fieldlengths{"testfloat"},17);
checkSuccess($fieldlengths{"testdecimal"},3);
checkSuccess($fieldlengths{"testnumeric"},3);
checkSuccess($fieldlengths{"testmoney"},4);
checkSuccess($fieldlengths{"testsmallmoney"},4);
#checkSuccess($fieldlengths{"testdatetime"},26);
#checkSuccess($fieldlengths{"testsmalldatetime"},26);
checkSuccess($fieldlengths{"testchar"},40);
checkSuccess($fieldlengths{"testvarchar"},12);
checkSuccess($fieldlengths{"testbit"},1);
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
checkSuccess($cur->sendQuery("select * from testtable order by testint"),1);
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
checkSuccess($cur->sendQuery("select * from testtable order by testint"),1);
checkUndef($cur->getColumnName(0));
checkSuccess($cur->getColumnLength(0),0);
checkUndef($cur->getColumnType(0));
$cur->getColumnInfo();
checkSuccess($cur->sendQuery("select * from testtable order by testint"),1);
checkSuccessString($cur->getColumnName(0),"testint");
checkSuccess($cur->getColumnLength(0),4);
checkSuccessString($cur->getColumnType(0),"INT");
print("\n");

print("SUSPENDED SESSION: \n");
checkSuccess($cur->sendQuery("select * from testtable order by testint"),1);
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
checkSuccess($cur->sendQuery("select * from testtable order by testint"),1);
$cur->suspendResultSet();
checkSuccess($con->suspendSession(),1);
$port=$con->getConnectionPort();
$socket=$con->getConnectionSocket();
checkSuccess($con->resumeSession($port,$socket),1);
checkSuccessString($cur->getField(0,0),"1");
checkSuccessString($cur->getField(1,0),"2");
checkSuccessString($cur->getField(2,0),"3");
checkSuccessString($cur->getField(3,0),"4");
checkSuccessString($cur->getField(4,0),"5");
checkSuccessString($cur->getField(5,0),"6");
checkSuccessString($cur->getField(6,0),"7");
checkSuccessString($cur->getField(7,0),"8");
print("\n");
checkSuccess($cur->sendQuery("select * from testtable order by testint"),1);
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
checkSuccess($cur->sendQuery("select * from testtable order by testint"),1);
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
checkSuccess($cur->sendQuery("select * from testtable order by testint"),1);
$filename=$cur->getCacheFileName();
checkSuccessString($filename,"cachefile1");
$cur->cacheOff();
checkSuccess($cur->openCachedResultSet($filename),1);
checkSuccessString($cur->getField(7,0),"8");
print("\n");

print("COLUMN COUNT FOR CACHED RESULT SET: \n");
checkSuccess($cur->colCount(),14);
print("\n");

print("COLUMN NAMES FOR CACHED RESULT SET: \n");
checkSuccessString($cur->getColumnName(0),"testint");
checkSuccessString($cur->getColumnName(1),"testsmallint");
checkSuccessString($cur->getColumnName(2),"testtinyint");
checkSuccessString($cur->getColumnName(3),"testreal");
checkSuccessString($cur->getColumnName(4),"testfloat");
checkSuccessString($cur->getColumnName(5),"testdecimal");
checkSuccessString($cur->getColumnName(6),"testnumeric");
checkSuccessString($cur->getColumnName(7),"testmoney");
checkSuccessString($cur->getColumnName(8),"testsmallmoney");
checkSuccessString($cur->getColumnName(9),"testdatetime");
checkSuccessString($cur->getColumnName(10),"testsmalldatetime");
checkSuccessString($cur->getColumnName(11),"testchar");
checkSuccessString($cur->getColumnName(12),"testvarchar");
checkSuccessString($cur->getColumnName(13),"testbit");
@cols=$cur->getColumnNames();
checkSuccessString($cols[0],"testint");
checkSuccessString($cols[1],"testsmallint");
checkSuccessString($cols[2],"testtinyint");
checkSuccessString($cols[3],"testreal");
checkSuccessString($cols[4],"testfloat");
checkSuccessString($cols[5],"testdecimal");
checkSuccessString($cols[6],"testnumeric");
checkSuccessString($cols[7],"testmoney");
checkSuccessString($cols[8],"testsmallmoney");
checkSuccessString($cols[9],"testdatetime");
checkSuccessString($cols[10],"testsmalldatetime");
checkSuccessString($cols[11],"testchar");
checkSuccessString($cols[12],"testvarchar");
checkSuccessString($cols[13],"testbit");
print("\n");

print("CACHED RESULT SET WITH RESULT SET BUFFER SIZE: \n");
$cur->setResultSetBufferSize(2);
$cur->cacheToFile("cachefile1");
$cur->setCacheTtl(200);
checkSuccess($cur->sendQuery("select * from testtable order by testint"),1);
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
checkSuccess($cur->sendQuery("select * from testtable order by testint"),1);
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
$cur->sendQuery("commit tran");
$cur->sendQuery("drop table testtable");

# invalid queries...
print("INVALID QUERIES: \n");
checkSuccess($cur->sendQuery("select * from testtable order by testint"),0);
checkSuccess($cur->sendQuery("select * from testtable order by testint"),0);
checkSuccess($cur->sendQuery("select * from testtable order by testint"),0);
checkSuccess($cur->sendQuery("select * from testtable order by testint"),0);
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



