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
	print("usage: lago.pl host port socket user password");
	exit;
}


# instantiation
$con=Firstworks::SQLRConnection->new($ARGV[0],$ARGV[1],
		$ARGV[2],$ARGV[3],$ARGV[4],0,1);
$cur=Firstworks::SQLRCursor->new($con);

# get database type
print("IDENTIFY: \n");
checkSuccessString($con->identify(),"lago");
print("\n");

# ping
print("PING: \n");
checkSuccess($con->ping(),1);
print("\n");

# drop existing table
$cur->sendQuery("drop table testtable");

print("CREATE TEMPTABLE: \n");
checkSuccess($cur->sendQuery("create table testtable (testsmallint smallint, testint int, testfloat float, testdouble double, testdecimal decimal(1,1), testchar char(40), testvarchar varchar(40), testdate date, testtime time, testtimestamp timestamp)"),1);
print("\n");

print("INSERT: \n");
checkSuccess($cur->sendQuery("insert into testtable (testsmallint, testint, testfloat, testdouble, testdecimal, testchar, testvarchar, testdate, testtime) values (1,1,1.1,1.1,1.1,'testchar1','testvarchar1','20010101','010000')"),1);
checkSuccess($cur->sendQuery("insert into testtable (testsmallint, testint, testfloat, testdouble, testdecimal, testchar, testvarchar, testdate, testtime) values (2,2,2.1,2.1,2.1,'testchar2','testvarchar2','20020101','020000')"),1);
checkSuccess($cur->sendQuery("insert into testtable (testsmallint, testint, testfloat, testdouble, testdecimal, testchar, testvarchar, testdate, testtime) values (3,3,3.1,3.1,3.1,'testchar3','testvarchar3','20030101','030000')"),1);
checkSuccess($cur->sendQuery("insert into testtable (testsmallint, testint, testfloat, testdouble, testdecimal, testchar, testvarchar, testdate, testtime) values (4,4,4.1,4.1,4.1,'testchar4','testvarchar4','20040101','040000')"),1);
print("\n");

print("AFFECTED ROWS: \n");
checkSuccess($cur->affectedRows(),-1);
print("\n");

print("BIND BY NAME: \n");
$cur->prepareQuery("insert into testtable (testsmallint, testint, testfloat, testdouble, testdecimal, testchar, testvarchar, testdate, testtime) values (:var1,:var2,:var3,:var4,:var5,:var6,:var7,:var8,:var9)");
$cur->inputBind("var1",5);
$cur->inputBind("var2",5);
$cur->inputBind("var3",5.1,2,1);
$cur->inputBind("var4",5.1,2,1);
$cur->inputBind("var5",5.1,2,1);
$cur->inputBind("var6","testchar5");
$cur->inputBind("var7","testvarchar5");
$cur->inputBind("var8","20050101");
$cur->inputBind("var9","050000");
checkSuccess($cur->executeQuery(),1);
$cur->clearBinds();
$cur->inputBind("var1",6);
$cur->inputBind("var2",6);
$cur->inputBind("var3",6.1,2,1);
$cur->inputBind("var4",6.1,2,1);
$cur->inputBind("var5",6.1,2,1);
$cur->inputBind("var6","testchar6");
$cur->inputBind("var7","testvarchar6");
$cur->inputBind("var8","20060101");
$cur->inputBind("var9","060000");
checkSuccess($cur->executeQuery(),1);
print("\n");

print("ARRAY OF BINDS BY NAME: \n");
$cur->clearBinds();
@vars=("var1","var2","var3","var4","var5","var6","var7","var8","var9");
@vals=(7,7,7.7,7.7,7.7,"testchar7","testvarchar7","20070101","070000");
@precs=(0,0,2,2,2,0,0,0,0);
@scales=(0,0,1,1,1,0,0,0,0);
$cur->inputBinds(\@vars,\@vals,\@precs,\@scales);
checkSuccess($cur->executeQuery(),1);
print("\n");

print("BIND BY NAME WITH VALIDATION: \n");
$cur->clearBinds();
$cur->inputBind("var1",8);
$cur->inputBind("var2",8);
$cur->inputBind("var3",8.1,2,1);
$cur->inputBind("var4",8.1,2,1);
$cur->inputBind("var5",8.1,2,1);
$cur->inputBind("var6","testchar8");
$cur->inputBind("var7","testvarchar8");
$cur->inputBind("var8","20080101");
$cur->inputBind("var9","080000");
$cur->inputBind("var10","junkvalue");
$cur->validateBinds();
checkSuccess($cur->executeQuery(),1);
print("\n");

print("SELECT: \n");
checkSuccess($cur->sendQuery("select * from testtable"),1);
print("\n");

print("COLUMN COUNT: \n");
checkSuccess($cur->colCount(),10);
print("\n");

print("COLUMN NAMES: \n");
checkSuccessString($cur->getColumnName(0),"testsmallint");
checkSuccessString($cur->getColumnName(1),"testint");
checkSuccessString($cur->getColumnName(2),"testfloat");
checkSuccessString($cur->getColumnName(3),"testdouble");
checkSuccessString($cur->getColumnName(4),"testdecimal");
checkSuccessString($cur->getColumnName(5),"testchar");
checkSuccessString($cur->getColumnName(6),"testvarchar");
checkSuccessString($cur->getColumnName(7),"testdate");
checkSuccessString($cur->getColumnName(8),"testtime");
checkSuccessString($cur->getColumnName(9),"testtimestamp");
@cols=$cur->getColumnNames();
checkSuccessString($cols[0],"testsmallint");
checkSuccessString($cols[1],"testint");
checkSuccessString($cols[2],"testfloat");
checkSuccessString($cols[3],"testdouble");
checkSuccessString($cols[4],"testdecimal");
checkSuccessString($cols[5],"testchar");
checkSuccessString($cols[6],"testvarchar");
checkSuccessString($cols[7],"testdate");
checkSuccessString($cols[8],"testtime");
checkSuccessString($cols[9],"testtimestamp");
print("\n");

print("COLUMN TYPES: \n");
checkSuccessString($cur->getColumnType(0),"SMALLINT");
checkSuccessString($cur->getColumnType('testsmallint'),"SMALLINT");
checkSuccessString($cur->getColumnType(1),"INT");
checkSuccessString($cur->getColumnType('testint'),"INT");
checkSuccessString($cur->getColumnType(2),"FLOAT");
checkSuccessString($cur->getColumnType('testfloat'),"FLOAT");
checkSuccessString($cur->getColumnType(3),"DOUBLE");
checkSuccessString($cur->getColumnType('testdouble'),"DOUBLE");
checkSuccessString($cur->getColumnType(4),"DOUBLE");
checkSuccessString($cur->getColumnType('testdecimal'),"DOUBLE");
checkSuccessString($cur->getColumnType(5),"CHAR");
checkSuccessString($cur->getColumnType('testchar'),"CHAR");
checkSuccessString($cur->getColumnType(6),"VARCHAR");
checkSuccessString($cur->getColumnType('testvarchar'),"VARCHAR");
checkSuccessString($cur->getColumnType(7),"DATE");
checkSuccessString($cur->getColumnType('testdate'),"DATE");
checkSuccessString($cur->getColumnType(8),"TIME");
checkSuccessString($cur->getColumnType('testtime'),"TIME");
checkSuccessString($cur->getColumnType(9),"TIMESTAMP");
checkSuccessString($cur->getColumnType('testtimestamp'),"TIMESTAMP");
print("\n");

print("COLUMN LENGTH: \n");
checkSuccess($cur->getColumnLength(0),2);
checkSuccess($cur->getColumnLength('testsmallint'),2);
checkSuccess($cur->getColumnLength(1),4);
checkSuccess($cur->getColumnLength('testint'),4);
checkSuccess($cur->getColumnLength(2),4);
checkSuccess($cur->getColumnLength('testfloat'),4);
checkSuccess($cur->getColumnLength(3),8);
checkSuccess($cur->getColumnLength('testdouble'),8);
checkSuccess($cur->getColumnLength(4),8);
checkSuccess($cur->getColumnLength('testdecimal'),8);
checkSuccess($cur->getColumnLength(5),40);
checkSuccess($cur->getColumnLength('testchar'),40);
checkSuccess($cur->getColumnLength(6),40);
checkSuccess($cur->getColumnLength('testvarchar'),40);
checkSuccess($cur->getColumnLength(7),4);
checkSuccess($cur->getColumnLength('testdate'),4);
checkSuccess($cur->getColumnLength(8),4);
checkSuccess($cur->getColumnLength('testtime'),4);
checkSuccess($cur->getColumnLength(9),8);
checkSuccess($cur->getColumnLength('testtimestamp'),8);
print("\n");

print("LONGEST COLUMN: \n");
checkSuccess($cur->getLongest(0),1);
checkSuccess($cur->getLongest('testsmallint'),1);
checkSuccess($cur->getLongest(1),1);
checkSuccess($cur->getLongest('testint'),1);
checkSuccess($cur->getLongest(2),3);
checkSuccess($cur->getLongest('testfloat'),3);
checkSuccess($cur->getLongest(3),3);
checkSuccess($cur->getLongest('testdouble'),3);
checkSuccess($cur->getLongest(4),3);
checkSuccess($cur->getLongest('testdecimal'),3);
checkSuccess($cur->getLongest(5),40);
checkSuccess($cur->getLongest('testchar'),40);
checkSuccess($cur->getLongest(6),12);
checkSuccess($cur->getLongest('testvarchar'),12);
checkSuccess($cur->getLongest(7),11);
checkSuccess($cur->getLongest('testdate'),11);
checkSuccess($cur->getLongest(8),8);
checkSuccess($cur->getLongest('testtime'),8);
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
checkSuccessString($cur->getField(0,0),"1");
checkSuccessString($cur->getField(0,1),"1");
checkSuccessString($cur->getField(0,2),"1.1");
checkSuccessString($cur->getField(0,3),"1.1");
checkSuccessString($cur->getField(0,4),"1.1");
checkSuccessString($cur->getField(0,5),"testchar1                               ");
checkSuccessString($cur->getField(0,6),"testvarchar1");
checkSuccessString($cur->getField(0,7)," 1-Jan-2001");
checkSuccessString($cur->getField(0,8),"01:00:00");
print("\n");
checkSuccessString($cur->getField(7,0),"8");
checkSuccessString($cur->getField(7,1),"8");
checkSuccessString($cur->getField(7,2),"8.1");
checkSuccessString($cur->getField(7,3),"8.1");
checkSuccessString($cur->getField(7,4),"8.1");
checkSuccessString($cur->getField(7,5),"testchar8                               ");
checkSuccessString($cur->getField(7,6),"testvarchar8");
checkSuccessString($cur->getField(7,7)," 1-Jan-2008");
checkSuccessString($cur->getField(7,8),"08:00:00");
print("\n");

print("FIELD LENGTHS BY INDEX: \n");
checkSuccess($cur->getFieldLength(0,0),1);
checkSuccess($cur->getFieldLength(0,1),1);
checkSuccess($cur->getFieldLength(0,2),3);
checkSuccess($cur->getFieldLength(0,3),3);
checkSuccess($cur->getFieldLength(0,4),3);
checkSuccess($cur->getFieldLength(0,5),40);
checkSuccess($cur->getFieldLength(0,6),12);
checkSuccess($cur->getFieldLength(0,7),11);
checkSuccess($cur->getFieldLength(0,8),8);
print("\n");
checkSuccess($cur->getFieldLength(7,0),1);
checkSuccess($cur->getFieldLength(7,1),1);
checkSuccess($cur->getFieldLength(7,2),3);
checkSuccess($cur->getFieldLength(7,3),3);
checkSuccess($cur->getFieldLength(7,4),3);
checkSuccess($cur->getFieldLength(7,5),40);
checkSuccess($cur->getFieldLength(7,6),12);
checkSuccess($cur->getFieldLength(7,7),11);
checkSuccess($cur->getFieldLength(7,8),8);
print("\n");

print("FIELDS BY NAME: \n");
checkSuccessString($cur->getField(0,"testint"),"1");
checkSuccessString($cur->getField(0,"testsmallint"),"1");
checkSuccessString($cur->getField(0,"testfloat"),"1.1");
checkSuccessString($cur->getField(0,"testdouble"),"1.1");
checkSuccessString($cur->getField(0,"testdecimal"),"1.1");
checkSuccessString($cur->getField(0,"testchar"),"testchar1                               ");
checkSuccessString($cur->getField(0,"testvarchar"),"testvarchar1");
checkSuccessString($cur->getField(0,"testdate")," 1-Jan-2001");
checkSuccessString($cur->getField(0,"testtime"),"01:00:00");
print("\n");
checkSuccessString($cur->getField(7,"testint"),"8");
checkSuccessString($cur->getField(7,"testsmallint"),"8");
checkSuccessString($cur->getField(7,"testfloat"),"8.1");
checkSuccessString($cur->getField(7,"testdouble"),"8.1");
checkSuccessString($cur->getField(7,"testdecimal"),"8.1");
checkSuccessString($cur->getField(7,"testchar"),"testchar8                               ");
checkSuccessString($cur->getField(7,"testvarchar"),"testvarchar8");
checkSuccessString($cur->getField(7,"testdate")," 1-Jan-2008");
checkSuccessString($cur->getField(7,"testtime"),"08:00:00");
print("\n");

print("FIELD LENGTHS BY NAME: \n");
checkSuccess($cur->getFieldLength(0,"testint"),1);
checkSuccess($cur->getFieldLength(0,"testsmallint"),1);
checkSuccess($cur->getFieldLength(0,"testfloat"),3);
checkSuccess($cur->getFieldLength(0,"testdouble"),3);
checkSuccess($cur->getFieldLength(0,"testdecimal"),3);
checkSuccess($cur->getFieldLength(0,"testchar"),40);
checkSuccess($cur->getFieldLength(0,"testvarchar"),12);
checkSuccess($cur->getFieldLength(0,"testdate"),11);
checkSuccess($cur->getFieldLength(0,"testtime"),8);
print("\n");
checkSuccess($cur->getFieldLength(7,"testint"),1);
checkSuccess($cur->getFieldLength(7,"testsmallint"),1);
checkSuccess($cur->getFieldLength(7,"testfloat"),3);
checkSuccess($cur->getFieldLength(7,"testdouble"),3);
checkSuccess($cur->getFieldLength(7,"testdecimal"),3);
checkSuccess($cur->getFieldLength(7,"testchar"),40);
checkSuccess($cur->getFieldLength(7,"testvarchar"),12);
checkSuccess($cur->getFieldLength(7,"testdate"),11);
checkSuccess($cur->getFieldLength(7,"testtime"),8);
print("\n");

print("FIELDS BY ARRAY: \n");
@fields=$cur->getRow(0);
checkSuccess($fields[0],1);
checkSuccess($fields[1],1);
checkSuccess($fields[2],1.1);
checkSuccess($fields[3],1.1);
checkSuccess($fields[4],1.1);
checkSuccessString($fields[5],"testchar1                               ");
checkSuccessString($fields[6],"testvarchar1");
checkSuccessString($fields[7]," 1-Jan-2001");
checkSuccessString($fields[8],"01:00:00");
print("\n");

print("FIELD LENGTHS BY ARRAY: \n");
@fieldlens=$cur->getRowLengths(0);
checkSuccess($fieldlens[0],1);
checkSuccess($fieldlens[1],1);
checkSuccess($fieldlens[2],3);
checkSuccess($fieldlens[3],3);
checkSuccess($fieldlens[4],3);
checkSuccess($fieldlens[5],40);
checkSuccess($fieldlens[6],12);
checkSuccess($fieldlens[7],11);
checkSuccess($fieldlens[8],8);
print("\n");

print("FIELDS BY HASH: \n");
%fields=$cur->getRowHash(0);
checkSuccess($fields{"testint"},1);
checkSuccess($fields{"testsmallint"},1);
checkSuccess($fields{"testfloat"},1.1);
checkSuccess($fields{"testdouble"},1.1);
checkSuccess($fields{"testdecimal"},1.1);
checkSuccessString($fields{"testchar"},"testchar1                               ");
checkSuccessString($fields{"testvarchar"},"testvarchar1");
checkSuccessString($fields{"testdate"}," 1-Jan-2001");
checkSuccessString($fields{"testtime"},"01:00:00");
print("\n");
%fields=$cur->getRowHash(7);
checkSuccess($fields{"testint"},8);
checkSuccess($fields{"testsmallint"},8);
checkSuccess($fields{"testfloat"},8.1);
checkSuccess($fields{"testdouble"},8.1);
checkSuccess($fields{"testdecimal"},8.1);
checkSuccessString($fields{"testchar"},"testchar8                               ");
checkSuccessString($fields{"testvarchar"},"testvarchar8");
checkSuccessString($fields{"testdate"}," 1-Jan-2008");
checkSuccessString($fields{"testtime"},"08:00:00");
print("\n");

print("FIELD LENGTHS BY HASH: \n");
%fieldlengths=$cur->getRowLengthsHash(0);
checkSuccess($fieldlengths{"testint"},1);
checkSuccess($fieldlengths{"testsmallint"},1);
checkSuccess($fieldlengths{"testfloat"},3);
checkSuccess($fieldlengths{"testdouble"},3);
checkSuccess($fieldlengths{"testdecimal"},3);
checkSuccess($fieldlengths{"testchar"},40);
checkSuccess($fieldlengths{"testvarchar"},12);
checkSuccess($fieldlengths{"testdate"},11);
checkSuccess($fieldlengths{"testtime"},8);
print("\n");
%fieldlengths=$cur->getRowLengthsHash(7);
checkSuccess($fieldlengths{"testint"},1);
checkSuccess($fieldlengths{"testsmallint"},1);
checkSuccess($fieldlengths{"testfloat"},3);
checkSuccess($fieldlengths{"testdouble"},3);
checkSuccess($fieldlengths{"testdecimal"},3);
checkSuccess($fieldlengths{"testchar"},40);
checkSuccess($fieldlengths{"testvarchar"},12);
checkSuccess($fieldlengths{"testdate"},11);
checkSuccess($fieldlengths{"testtime"},8);
print("\n");

print("INDIVIDUAL SUBSTITUTIONS: \n");
$cur->sendQuery("drop table testtable1");
checkSuccess($cur->sendQuery("create table testtable1 (col1 int, col2 varchar(40), col3 real)"),1);
$cur->prepareQuery("insert into testtable1 (col1, col2, col3) values (\$(var1),'\$(var2)',\$(var3))");
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
$cur->prepareQuery("insert into testtable1 (col1, col2, col3) values (\$(var1),'\$(var2)',\$(var3))");
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
checkSuccess($cur->sendQuery("insert into testtable1 (col1, col2, col3) values (1,NULL,NULL)"),1);
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
checkSuccess($cur->sendQuery("select * from testtable"),1);
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
checkSuccess($cur->sendQuery("select * from testtable"),1);
checkUndef($cur->getColumnName(0));
checkSuccess($cur->getColumnLength(0),0);
checkUndef($cur->getColumnType(0));
$cur->getColumnInfo();
checkSuccess($cur->sendQuery("select * from testtable"),1);
checkSuccessString($cur->getColumnName(0),"testsmallint");
checkSuccess($cur->getColumnLength(0),2);
checkSuccessString($cur->getColumnType(0),"SMALLINT");
print("\n");

print("SUSPENDED SESSION: \n");
checkSuccess($cur->sendQuery("select * from testtable"),1);
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
checkSuccess($cur->sendQuery("select * from testtable"),1);
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
checkSuccess($cur->sendQuery("select * from testtable"),1);
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
checkSuccess($cur->sendQuery("select * from testtable"),1);
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
checkSuccess($cur->sendQuery("select * from testtable"),1);
$filename=$cur->getCacheFileName();
checkSuccessString($filename,"cachefile1");
$cur->cacheOff();
checkSuccess($cur->openCachedResultSet($filename),1);
checkSuccessString($cur->getField(7,0),"8");
print("\n");

print("COLUMN COUNT FOR CACHED RESULT SET: \n");
checkSuccess($cur->colCount(),10);
print("\n");

print("COLUMN NAMES FOR CACHED RESULT SET: \n");
checkSuccessString($cur->getColumnName(0),"testsmallint");
checkSuccessString($cur->getColumnName(1),"testint");
checkSuccessString($cur->getColumnName(2),"testfloat");
checkSuccessString($cur->getColumnName(3),"testdouble");
checkSuccessString($cur->getColumnName(4),"testdecimal");
checkSuccessString($cur->getColumnName(5),"testchar");
checkSuccessString($cur->getColumnName(6),"testvarchar");
checkSuccessString($cur->getColumnName(7),"testdate");
checkSuccessString($cur->getColumnName(8),"testtime");
checkSuccessString($cur->getColumnName(9),"testtimestamp");
@cols=$cur->getColumnNames();
checkSuccessString($cols[0],"testsmallint");
checkSuccessString($cols[1],"testint");
checkSuccessString($cols[2],"testfloat");
checkSuccessString($cols[3],"testdouble");
checkSuccessString($cols[4],"testdecimal");
checkSuccessString($cols[5],"testchar");
checkSuccessString($cols[6],"testvarchar");
checkSuccessString($cols[7],"testdate");
checkSuccessString($cols[8],"testtime");
checkSuccessString($cols[9],"testtimestamp");
print("\n");

print("CACHED RESULT SET WITH RESULT SET BUFFER SIZE: \n");
$cur->setResultSetBufferSize(2);
$cur->cacheToFile("cachefile1");
$cur->setCacheTtl(200);
checkSuccess($cur->sendQuery("select * from testtable"),1);
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
checkSuccess($cur->sendQuery("select * from testtable"),1);
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
$secondcon=Firstworks::SQLRConnection->new($ARGV[0],
			$ARGV[1], 
		$ARGV[2],$ARGV[3],$ARGV[4],0,1);
$secondcur=Firstworks::SQLRCursor->new($secondcon);
checkSuccess($secondcur->sendQuery("select * from testtable"),1);
checkSuccessString($secondcur->getField(0,0),"1");
checkSuccess($con->commit(),1);
checkSuccess($secondcur->sendQuery("select * from testtable"),1);
checkSuccessString($secondcur->getField(0,0),"1");
checkSuccess($con->autoCommitOn(),1);
checkSuccess($cur->sendQuery("insert into testtable (testsmallint, testint, testfloat, testdouble, testdecimal, testchar, testvarchar, testdate, testtime) values (10,10,10.1,10.1,10.1,'testchar10','testvarchar10','20100101','100000')"),1);
checkSuccess($secondcur->sendQuery("select * from testtable"),1);
checkSuccessString($secondcur->getField(8,0),"10");
checkSuccess($con->autoCommitOff(),1);
print("\n");

# drop existing table
$cur->sendQuery("drop table testtable");

# invalid queries...
print("INVALID QUERIES: \n");
checkSuccess($cur->sendQuery("select * from testtable"),0);
checkSuccess($cur->sendQuery("select * from testtable"),0);
checkSuccess($cur->sendQuery("select * from testtable"),0);
checkSuccess($cur->sendQuery("select * from testtable"),0);
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


