#! /usr/bin/env perl

# Copyright (c) 2001  David Muse
# See the file COPYING for more information.


use Firstworks::SQLRConnection;
use Firstworks::SQLRCursor;

sub checkUndef {

	$value=shift(@_);

	if (!defined($value)) {
		print("success ");
	} else {
		print("failure ");
		exit;
	}
}

sub checkSuccess {

	$value=shift(@_);
	$success=shift(@_);

	if ($value==$success) {
		print("success ");
	} else {
		print("failure ");
		exit;
	}
}

sub checkSuccessString {

	$value=shift(@_);
	$success=shift(@_);

	if ($value eq $success) {
		print("success ");
	} else {
		print("failure ");
		exit;
	}
}


# usage...
if ($#ARGV+1<5) {
	print("usage: msql.pl host port socket user password");
	exit;
}


# instantiation
$con=Firstworks::SQLRConnection->new($ARGV[0],$ARGV[1],
		$ARGV[2],$ARGV[3],$ARGV[4],0,1);
$cur=Firstworks::SQLRCursor->new($con);

# get database type
print("IDENTIFY: \n");
checkSuccessString($con->identify(),"msql");
print("\n");

# ping
print("PING: \n");
checkSuccess($con->ping(),1);
print("\n");

# drop existing table
$cur->sendQuery("drop table testtable");

print("CREATE TEMPTABLE: \n");
checkSuccess($cur->sendQuery("create table testtable (testchar char(40), testdate date, testint int, testmoney money, testreal real, testtext text(40), testtime time, testuint uint)"),1);
print("\n");

print("INSERT: \n");
checkSuccess($cur->sendQuery("insert into testtable values ('char1','01-Jan-2001',1,1.00,1.1,'text1','01:00:00',1)"),1);
checkSuccess($cur->sendQuery("insert into testtable values ('char2','01-Jan-2002',2,2.00,2.1,'text2','02:00:00',2)"),1);
checkSuccess($cur->sendQuery("insert into testtable values ('char3','01-Jan-2003',3,3.00,3.1,'text3','03:00:00',3)"),1);
checkSuccess($cur->sendQuery("insert into testtable values ('char4','01-Jan-2004',4,4.00,4.1,'text4','04:00:00',4)"),1);
print("\n");

print("AFFECTED ROWS: \n");
checkSuccess($cur->affectedRows(),-1);
print("\n");

print("BIND BY NAME: \n");
$cur->prepareQuery("insert into testtable values (:var1,:var2,:var3,:var4,:var5,:var6,:var7,:var8)");
$cur->inputBind("var1","char5");
$cur->inputBind("var2","01-Jan-2005");
$cur->inputBind("var3",5);
$cur->inputBind("var4",5.00,3,2);
$cur->inputBind("var5",5.1,2,1);
$cur->inputBind("var6","text5");
$cur->inputBind("var7","05:00:00");
$cur->inputBind("var8",5);
checkSuccess($cur->executeQuery(),1);
$cur->clearBinds();
$cur->inputBind("var1","char6");
$cur->inputBind("var2","01-Jan-2006");
$cur->inputBind("var3",6);
$cur->inputBind("var4",6.00,3,2);
$cur->inputBind("var5",6.1,2,1);
$cur->inputBind("var6","text6");
$cur->inputBind("var7","06:00:00");
$cur->inputBind("var8",6);
checkSuccess($cur->executeQuery(),1);
print("\n");

print("ARRAY OF BINDS BY NAME: \n");
$cur->clearBinds();
@vars=("var1","var2","var3","var4","var5","var6","var7","var8");
@vals=("char7","01-Jan-2007",7,7.00,7.1,"text7","07:00:00",7);
@precs=(0,0,0,3,2,0,0,0);
@scales=(0,0,0,2,1,0,0,0);
$cur->inputBinds(\@vars,\@vals,\@precs,\@scales);
checkSuccess($cur->executeQuery(),1);
print("\n");

print("BIND BY NAME WITH VALIDATION: \n");
$cur->clearBinds();
$cur->inputBind("var1","char8");
$cur->inputBind("var2","01-Jan-2008");
$cur->inputBind("var3",8);
$cur->inputBind("var4",8.00,3,2);
$cur->inputBind("var5",8.1,2,1);
$cur->inputBind("var6","text8");
$cur->inputBind("var7","08:00:00");
$cur->inputBind("var8",8);
$cur->inputBind("var9","junkvalue");
$cur->validateBinds();
checkSuccess($cur->executeQuery(),1);
print("\n");

print("SELECT: \n");
checkSuccess($cur->sendQuery("select * from testtable order by testint"),1);
print("\n");

print("COLUMN COUNT: \n");
checkSuccess($cur->colCount(),8);
print("\n");

print("COLUMN NAMES: \n");
checkSuccessString($cur->getColumnName(0),"testchar");
checkSuccessString($cur->getColumnName(1),"testdate");
checkSuccessString($cur->getColumnName(2),"testint");
checkSuccessString($cur->getColumnName(3),"testmoney");
checkSuccessString($cur->getColumnName(4),"testreal");
checkSuccessString($cur->getColumnName(5),"testtext");
checkSuccessString($cur->getColumnName(6),"testtime");
checkSuccessString($cur->getColumnName(7),"testuint");
@cols=$cur->getColumnNames();
checkSuccessString($cols[0],"testchar");
checkSuccessString($cols[1],"testdate");
checkSuccessString($cols[2],"testint");
checkSuccessString($cols[3],"testmoney");
checkSuccessString($cols[4],"testreal");
checkSuccessString($cols[5],"testtext");
checkSuccessString($cols[6],"testtime");
checkSuccessString($cols[7],"testuint");
print("\n");

print("COLUMN TYPES: \n");
checkSuccessString($cur->getColumnType(0),"CHAR");
checkSuccessString($cur->getColumnType('testchar'),"CHAR");
checkSuccessString($cur->getColumnType(1),"DATE");
checkSuccessString($cur->getColumnType('testdate'),"DATE");
checkSuccessString($cur->getColumnType(2),"INT");
checkSuccessString($cur->getColumnType('testint'),"INT");
checkSuccessString($cur->getColumnType(3),"MONEY");
checkSuccessString($cur->getColumnType('testmoney'),"MONEY");
checkSuccessString($cur->getColumnType(4),"REAL");
checkSuccessString($cur->getColumnType('testreal'),"REAL");
checkSuccessString($cur->getColumnType(5),"TEXT");
checkSuccessString($cur->getColumnType('testtext'),"TEXT");
checkSuccessString($cur->getColumnType(6),"TIME");
checkSuccessString($cur->getColumnType('testtime'),"TIME");
checkSuccessString($cur->getColumnType(7),"UINT");
checkSuccessString($cur->getColumnType('testuint'),"UINT");
print("\n");

print("COLUMN LENGTH: \n");
checkSuccess($cur->getColumnLength(0),40);
checkSuccess($cur->getColumnLength('testchar'),40);
checkSuccess($cur->getColumnLength(1),4);
checkSuccess($cur->getColumnLength('testdate'),4);
checkSuccess($cur->getColumnLength(2),4);
checkSuccess($cur->getColumnLength('testint'),4);
checkSuccess($cur->getColumnLength(3),4);
checkSuccess($cur->getColumnLength('testmoney'),4);
checkSuccess($cur->getColumnLength(4),8);
checkSuccess($cur->getColumnLength('testreal'),8);
checkSuccess($cur->getColumnLength(5),40);
checkSuccess($cur->getColumnLength('testtext'),40);
checkSuccess($cur->getColumnLength(6),4);
checkSuccess($cur->getColumnLength('testtime'),4);
checkSuccess($cur->getColumnLength(7),4);
checkSuccess($cur->getColumnLength('testuint'),4);
print("\n");

print("LONGEST COLUMN: \n");
checkSuccess($cur->getLongest(0),5);
checkSuccess($cur->getLongest('testchar'),5);
checkSuccess($cur->getLongest(1),11);
checkSuccess($cur->getLongest('testdate'),11);
checkSuccess($cur->getLongest(2),1);
checkSuccess($cur->getLongest('testint'),1);
checkSuccess($cur->getLongest(3),4);
checkSuccess($cur->getLongest('testmoney'),4);
checkSuccess($cur->getLongest(4),3);
checkSuccess($cur->getLongest('testreal'),3);
checkSuccess($cur->getLongest(5),5);
checkSuccess($cur->getLongest('testtext'),5);
checkSuccess($cur->getLongest(6),8);
checkSuccess($cur->getLongest('testtime'),8);
checkSuccess($cur->getLongest(7),1);
checkSuccess($cur->getLongest('testuint'),1);
print("\n");

print("ROW COUNT: \n");
checkSuccess($cur->rowCount(),8);
print("\n");

print("TOTAL ROWS: \n");
checkSuccess($cur->totalRows(),8);
print("\n");

print("FIRST ROW INDEX: \n");
checkSuccess($cur->firstRowIndex(),0);
print("\n");

print("END OF RESULT SET: \n");
checkSuccess($cur->endOfResultSet(),1);
print("\n");

print("FIELDS BY INDEX: \n");
checkSuccessString($cur->getField(0,0),"char1");
checkSuccessString($cur->getField(0,1),"01-Jan-2001");
checkSuccessString($cur->getField(0,2),"1");
checkSuccessString($cur->getField(0,3),"1.00");
checkSuccessString($cur->getField(0,4),"1.1");
checkSuccessString($cur->getField(0,5),"text1");
checkSuccessString($cur->getField(0,6),"01:00:00");
checkSuccessString($cur->getField(0,7),"1");
print("\n");
checkSuccessString($cur->getField(7,0),"char8");
checkSuccessString($cur->getField(7,1),"01-Jan-2008");
checkSuccessString($cur->getField(7,2),"8");
checkSuccessString($cur->getField(7,3),"8.00");
checkSuccessString($cur->getField(7,4),"8.1");
checkSuccessString($cur->getField(7,5),"text8");
checkSuccessString($cur->getField(7,6),"08:00:00");
checkSuccessString($cur->getField(7,7),"8");
print("\n");

print("FIELD LENGTHS BY INDEX: \n");
checkSuccess($cur->getFieldLength(0,0),5);
checkSuccess($cur->getFieldLength(0,1),11);
checkSuccess($cur->getFieldLength(0,2),1);
checkSuccess($cur->getFieldLength(0,3),4);
checkSuccess($cur->getFieldLength(0,4),3);
checkSuccess($cur->getFieldLength(0,5),5);
checkSuccess($cur->getFieldLength(0,6),8);
checkSuccess($cur->getFieldLength(0,7),1);
print("\n");
checkSuccess($cur->getFieldLength(7,0),5);
checkSuccess($cur->getFieldLength(7,1),11);
checkSuccess($cur->getFieldLength(7,2),1);
checkSuccess($cur->getFieldLength(7,3),4);
checkSuccess($cur->getFieldLength(7,4),3);
checkSuccess($cur->getFieldLength(7,5),5);
checkSuccess($cur->getFieldLength(7,6),8);
checkSuccess($cur->getFieldLength(7,7),1);
print("\n");

print("FIELDS BY NAME: \n");
checkSuccessString($cur->getField(0,"testchar"),"char1");
checkSuccessString($cur->getField(0,"testdate"),"01-Jan-2001");
checkSuccessString($cur->getField(0,"testint"),"1");
checkSuccessString($cur->getField(0,"testmoney"),"1.00");
checkSuccessString($cur->getField(0,"testreal"),"1.1");
checkSuccessString($cur->getField(0,"testtext"),"text1");
checkSuccessString($cur->getField(0,"testtime"),"01:00:00");
checkSuccessString($cur->getField(0,"testuint"),"1");
print("\n");
checkSuccessString($cur->getField(7,"testchar"),"char8");
checkSuccessString($cur->getField(7,"testdate"),"01-Jan-2008");
checkSuccessString($cur->getField(7,"testint"),"8");
checkSuccessString($cur->getField(7,"testmoney"),"8.00");
checkSuccessString($cur->getField(7,"testreal"),"8.1");
checkSuccessString($cur->getField(7,"testtext"),"text8");
checkSuccessString($cur->getField(7,"testtime"),"08:00:00");
checkSuccessString($cur->getField(7,"testuint"),"8");
print("\n");

print("FIELD LENGTHS BY NAME: \n");
checkSuccess($cur->getFieldLength(0,"testchar"),5);
checkSuccess($cur->getFieldLength(0,"testdate"),11);
checkSuccess($cur->getFieldLength(0,"testint"),1);
checkSuccess($cur->getFieldLength(0,"testmoney"),4);
checkSuccess($cur->getFieldLength(0,"testreal"),3);
checkSuccess($cur->getFieldLength(0,"testtext"),5);
checkSuccess($cur->getFieldLength(0,"testtime"),8);
checkSuccess($cur->getFieldLength(0,"testuint"),1);
print("\n");
checkSuccess($cur->getFieldLength(7,"testchar"),5);
checkSuccess($cur->getFieldLength(7,"testdate"),11);
checkSuccess($cur->getFieldLength(7,"testint"),1);
checkSuccess($cur->getFieldLength(7,"testmoney"),4);
checkSuccess($cur->getFieldLength(7,"testreal"),3);
checkSuccess($cur->getFieldLength(7,"testtext"),5);
checkSuccess($cur->getFieldLength(7,"testtime"),8);
checkSuccess($cur->getFieldLength(7,"testuint"),1);
print("\n");

print("FIELDS BY ARRAY: \n");
@fields=$cur->getRow(0);
checkSuccessString($fields[0],"char1");
checkSuccessString($fields[1],"01-Jan-2001");
checkSuccess($fields[2],1);
checkSuccess($fields[3],1.00);
checkSuccess($fields[4],1.1);
checkSuccessString($fields[5],"text1");
checkSuccessString($fields[6],"01:00:00");
checkSuccess($fields[7],1);
print("\n");

print("FIELD LENGTHS BY ARRAY: \n");
@fieldlens=$cur->getRowLengths(0);
checkSuccess($fieldlens[0],5);
checkSuccess($fieldlens[1],11);
checkSuccess($fieldlens[2],1);
checkSuccess($fieldlens[3],4);
checkSuccess($fieldlens[4],3);
checkSuccess($fieldlens[5],5);
checkSuccess($fieldlens[6],8);
checkSuccess($fieldlens[7],1);
print("\n");

print("FIELDS BY HASH: \n");
%fields=$cur->getRowHash(0);
checkSuccessString($fields{"testchar"},"char1");
checkSuccessString($fields{"testdate"},"01-Jan-2001");
checkSuccess($fields{"testint"},1);
checkSuccess($fields{"testmoney"},1.00);
checkSuccess($fields{"testreal"},1.1);
checkSuccessString($fields{"testtext"},"text1");
checkSuccessString($fields{"testtime"},"01:00:00");
checkSuccess($fields{"testuint"},1);
print("\n");
%fields=$cur->getRowHash(7);
checkSuccessString($fields{"testchar"},"char8");
checkSuccessString($fields{"testdate"},"01-Jan-2008");
checkSuccess($fields{"testint"},8);
checkSuccess($fields{"testmoney"},8.00);
checkSuccess($fields{"testreal"},8.1);
checkSuccessString($fields{"testtext"},"text8");
checkSuccessString($fields{"testtime"},"08:00:00");
checkSuccess($fields{"testuint"},8);
print("\n");

print("FIELD LENGTHS BY HASH: \n");
%fieldlengths=$cur->getRowLengthsHash(0);
checkSuccess($fieldlengths{"testchar"},5);
checkSuccess($fieldlengths{"testdate"},11);
checkSuccess($fieldlengths{"testint"},1);
checkSuccess($fieldlengths{"testmoney"},4);
checkSuccess($fieldlengths{"testreal"},3);
checkSuccess($fieldlengths{"testtext"},5);
checkSuccess($fieldlengths{"testtime"},8);
checkSuccess($fieldlengths{"testuint"},1);
print("\n");
%fieldlengths=$cur->getRowLengthsHash(7);
checkSuccess($fieldlengths{"testchar"},5);
checkSuccess($fieldlengths{"testdate"},11);
checkSuccess($fieldlengths{"testint"},1);
checkSuccess($fieldlengths{"testmoney"},4);
checkSuccess($fieldlengths{"testreal"},3);
checkSuccess($fieldlengths{"testtext"},5);
checkSuccess($fieldlengths{"testtime"},8);
checkSuccess($fieldlengths{"testuint"},1);
print("\n");

print("INDIVIDUAL SUBSTITUTIONS: \n");
$cur->sendQuery("drop table testtable1");
checkSuccess($cur->sendQuery("create table testtable1 (col1 int, col2 char(40), col3 real)"),1);
$cur->prepareQuery("insert into testtable1 values (\$(var1),'\$(var2)',\$(var3))");
$cur->substitution("var1",1);
$cur->substitution("var2","hello");
$cur->substitution("var3",10.5556,6,4);
checkSuccess($cur->executeQuery(),1);
print("\n");

print("FIELDS: \n");
checkSuccess($cur->sendQuery("select * from testtable1"),1);
checkSuccessString($cur->getField(0,0),"1");
checkSuccessString($cur->getField(0,1),"hello");
checkSuccessString($cur->getField(0,2),"10.5556");
checkSuccess($cur->sendQuery("delete from testtable1"),1);
print("\n");

print("ARRAY SUBSTITUTIONS: \n");
$cur->prepareQuery("insert into testtable1 values (\$(var1),'\$(var2)',\$(var3))");
@vars=("var1","var2","var3");
@vals=(1,"hello",10.5556);
@precs=(0,0,6);
@scales=(0,0,4);
$cur->substitutions(\@vars,\@vals,\@precs,\@scales);
checkSuccess($cur->executeQuery(),1);
print("\n");

print("FIELDS: \n");
checkSuccess($cur->sendQuery("select * from testtable1"),1);
checkSuccessString($cur->getField(0,0),"1");
checkSuccessString($cur->getField(0,1),"hello");
checkSuccessString($cur->getField(0,2),"10.5556");
checkSuccess($cur->sendQuery("delete from testtable1"),1);
print("\n");

print("NULLS as Undef: \n");
$cur->getNullsAsUndefined();
checkSuccess($cur->sendQuery("insert into testtable1 values (1,NULL,NULL)"),1);
checkSuccess($cur->sendQuery("select * from testtable1"),1);
checkSuccessString($cur->getField(0,0),"1");
checkUndef($cur->getField(0,1));
checkUndef($cur->getField(0,2));
$cur->getNullsAsEmptyStrings();
checkSuccess($cur->sendQuery("select * from testtable1"),1);
checkSuccessString($cur->getField(0,0),"1");
checkSuccessString($cur->getField(0,1),"");
checkSuccessString($cur->getField(0,2),"");
checkSuccess($cur->sendQuery("drop table testtable1"),1);
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
checkSuccessString($cur->getField(0,0),"char1");
checkSuccessString($cur->getField(1,0),"char2");
checkSuccessString($cur->getField(2,0),"char3");
print("\n");
checkSuccess($cur->firstRowIndex(),2);
checkSuccess($cur->endOfResultSet(),0);
checkSuccess($cur->rowCount(),4);
checkSuccessString($cur->getField(6,0),"char7");
checkSuccessString($cur->getField(7,0),"char8");
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
checkSuccessString($cur->getColumnName(0),"testchar");
checkSuccess($cur->getColumnLength(0),40);
checkSuccessString($cur->getColumnType(0),"CHAR");
print("\n");

print("SUSPENDED SESSION: \n");
checkSuccess($cur->sendQuery("select * from testtable order by testint"),1);
$cur->suspendResultSet();
checkSuccess($con->suspendSession(),1);
$port=$con->getConnectionPort();
$socket=$con->getConnectionSocket();
checkSuccess($con->resumeSession($port,$socket),1);
print("\n");
checkSuccessString($cur->getField(0,0),"char1");
checkSuccessString($cur->getField(1,0),"char2");
checkSuccessString($cur->getField(2,0),"char3");
checkSuccessString($cur->getField(3,0),"char4");
checkSuccessString($cur->getField(4,0),"char5");
checkSuccessString($cur->getField(5,0),"char6");
checkSuccessString($cur->getField(6,0),"char7");
checkSuccessString($cur->getField(7,0),"char8");
print("\n");
checkSuccess($cur->sendQuery("select * from testtable order by testint"),1);
$cur->suspendResultSet();
checkSuccess($con->suspendSession(),1);
$port=$con->getConnectionPort();
$socket=$con->getConnectionSocket();
checkSuccess($con->resumeSession($port,$socket),1);
print("\n");
checkSuccessString($cur->getField(0,0),"char1");
checkSuccessString($cur->getField(1,0),"char2");
checkSuccessString($cur->getField(2,0),"char3");
checkSuccessString($cur->getField(3,0),"char4");
checkSuccessString($cur->getField(4,0),"char5");
checkSuccessString($cur->getField(5,0),"char6");
checkSuccessString($cur->getField(6,0),"char7");
checkSuccessString($cur->getField(7,0),"char8");
print("\n");
checkSuccess($cur->sendQuery("select * from testtable order by testint"),1);
$cur->suspendResultSet();
checkSuccess($con->suspendSession(),1);
$port=$con->getConnectionPort();
$socket=$con->getConnectionSocket();
checkSuccess($con->resumeSession($port,$socket),1);
print("\n");
checkSuccessString($cur->getField(0,0),"char1");
checkSuccessString($cur->getField(1,0),"char2");
checkSuccessString($cur->getField(2,0),"char3");
checkSuccessString($cur->getField(3,0),"char4");
checkSuccessString($cur->getField(4,0),"char5");
checkSuccessString($cur->getField(5,0),"char6");
checkSuccessString($cur->getField(6,0),"char7");
checkSuccessString($cur->getField(7,0),"char8");
print("\n");

print("SUSPENDED RESULT SET: \n");
$cur->setResultSetBufferSize(2);
checkSuccess($cur->sendQuery("select * from testtable order by testint"),1);
checkSuccessString($cur->getField(2,0),"char3");
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
checkSuccessString($cur->getField(7,0),"char8");
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
checkSuccessString($cur->getField(7,0),"char8");
print("\n");

print("COLUMN COUNT FOR CACHED RESULT SET: \n");
checkSuccess($cur->colCount(),8);
print("\n");

print("COLUMN NAMES FOR CACHED RESULT SET: \n");
checkSuccessString($cur->getColumnName(0),"testchar");
checkSuccessString($cur->getColumnName(1),"testdate");
checkSuccessString($cur->getColumnName(2),"testint");
checkSuccessString($cur->getColumnName(3),"testmoney");
checkSuccessString($cur->getColumnName(4),"testreal");
checkSuccessString($cur->getColumnName(5),"testtext");
checkSuccessString($cur->getColumnName(6),"testtime");
checkSuccessString($cur->getColumnName(7),"testuint");
@cols=$cur->getColumnNames();
checkSuccessString($cols[0],"testchar");
checkSuccessString($cols[1],"testdate");
checkSuccessString($cols[2],"testint");
checkSuccessString($cols[3],"testmoney");
checkSuccessString($cols[4],"testreal");
checkSuccessString($cols[5],"testtext");
checkSuccessString($cols[6],"testtime");
checkSuccessString($cols[7],"testuint");
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
checkSuccessString($cur->getField(7,0),"char8");
checkUndef($cur->getField(8,0));
$cur->setResultSetBufferSize(0);
print("\n");

print("FROM ONE CACHE FILE TO ANOTHER: \n");
$cur->cacheToFile("cachefile2");
checkSuccess($cur->openCachedResultSet("cachefile1"),1);
$cur->cacheOff();
checkSuccess($cur->openCachedResultSet("cachefile2"),1);
checkSuccessString($cur->getField(7,0),"char8");
checkUndef($cur->getField(8,0));
print("\n");

print("FROM ONE CACHE FILE TO ANOTHER WITH RESULT SET BUFFER SIZE: \n");
$cur->setResultSetBufferSize(2);
$cur->cacheToFile("cachefile2");
checkSuccess($cur->openCachedResultSet("cachefile1"),1);
$cur->cacheOff();
checkSuccess($cur->openCachedResultSet("cachefile2"),1);
checkSuccessString($cur->getField(7,0),"char8");
checkUndef($cur->getField(8,0));
$cur->setResultSetBufferSize(0);
print("\n");

print("CACHED RESULT SET WITH SUSPEND AND RESULT SET BUFFER SIZE: \n");
$cur->setResultSetBufferSize(2);
$cur->cacheToFile("cachefile1");
$cur->setCacheTtl(200);
checkSuccess($cur->sendQuery("select * from testtable order by testint"),1);
checkSuccessString($cur->getField(2,0),"char3");
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
checkSuccessString($cur->getField(7,0),"char8");
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
checkSuccessString($cur->getField(7,0),"char8");
checkUndef($cur->getField(8,0));
$cur->setResultSetBufferSize(0);
print("\n");

print("COMMIT AND ROLLBACK: \n");
$secondcon=Firstworks::SQLRConnection->new($ARGV[0],
			$ARGV[1], 
		$ARGV[2],$ARGV[3],$ARGV[4],0,1);
$secondcur=Firstworks::SQLRCursor->new($secondcon);
checkSuccess($secondcur->sendQuery("select * from testtable order by testint"),1);
checkSuccessString($secondcur->getField(0,0),"char1");
checkSuccess($con->commit(),1);
checkSuccess($secondcur->sendQuery("select * from testtable order by testint"),1);
checkSuccessString($secondcur->getField(0,0),"char1");
checkSuccess($con->autoCommitOn(),1);
checkSuccess($cur->sendQuery("insert into testtable values ('char10','01-Jan-2010',10,10.00,10.1,'text10','10:00:00',10)"),1);
checkSuccess($secondcur->sendQuery("select * from testtable order by testint"),1);
checkSuccessString($secondcur->getField(8,0),"char10");
checkSuccess($con->autoCommitOff(),1);
print("\n");

# drop existing table
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



