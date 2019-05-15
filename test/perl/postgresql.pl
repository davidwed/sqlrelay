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

print("IDENTIFY: \n");
checkSuccessString($con->identify(),"postgresql");
print("\n");

# ping
print("PING: \n");
checkSuccess($con->ping(),1);
print("\n");

# drop existing table
$cur->sendQuery("drop table testtable");

print("CREATE TEMPTABLE: \n");
checkSuccess($cur->sendQuery("create table testtable (testint int, testfloat float, testreal real, testsmallint smallint, testchar char(40), testvarchar varchar(40), testdate date, testtime time, testtimestamp timestamp)"),1);
print("\n");

print("BEGIN TRANSCTION: \n");
checkSuccess($cur->sendQuery("begin"),1);
print("\n");

print("INSERT: \n");
checkSuccess($cur->sendQuery("insert into testtable values (1,1.1,1.1,1,'testchar1','testvarchar1','01/01/2001','01:00:00',NULL)"),1);
checkSuccess($cur->sendQuery("insert into testtable values (2,2.2,2.2,2,'testchar2','testvarchar2','01/01/2002','02:00:00',NULL)"),1);
checkSuccess($cur->sendQuery("insert into testtable values (3,3.3,3.3,3,'testchar3','testvarchar3','01/01/2003','03:00:00',NULL)"),1);
checkSuccess($cur->sendQuery("insert into testtable values (4,4.4,4.4,4,'testchar4','testvarchar4','01/01/2004','04:00:00',NULL)"),1);
print("\n");

print("AFFECTED ROWS: \n");
checkSuccess($cur->affectedRows(),1);
print("\n");

print("BIND BY POSITION: \n");
$cur->prepareQuery("insert into testtable values (\$1,\$2,\$3,\$4,\$5,\$6,\$7,\$8)");
checkSuccess($cur->countBindVariables(),8);
$cur->inputBind("1",5);
$cur->inputBind("2",5.5,4,2);
$cur->inputBind("3",5.5,4,2);
$cur->inputBind("4",5);
$cur->inputBind("5","testchar5");
$cur->inputBind("6","testvarchar5");
$cur->inputBind("7","01/01/2005");
$cur->inputBind("8","05:00:00");
checkSuccess($cur->executeQuery(),1);
$cur->clearBinds();
$cur->inputBind("1",6);
$cur->inputBind("2",6.6,4,2);
$cur->inputBind("3",6.6,4,2);
$cur->inputBind("4",6);
$cur->inputBind("5","testchar6");
$cur->inputBind("6","testvarchar6");
$cur->inputBind("7","01/01/2006");
$cur->inputBind("8","06:00:00");
checkSuccess($cur->executeQuery(),1);
print("\n");

print("ARRAY OF BINDS BY POSITION: \n");
$cur->clearBinds();
@vars=("1","2","3","4","5","6","7","8");
@vals=(7,7.7,7.7,7,"testchar7","testvarchar7","01/01/2007","07:00:00");
@precs=(0,4,4,0,0,0,0,0);
@scales=(0,2,2,0,0,0,0,0);
$cur->inputBinds(\@vars,\@vals,\@precs,\@scales);
checkSuccess($cur->executeQuery(),1);
print("\n");

print("BIND BY POSITION WITH VALIDATION: \n");
$cur->clearBinds();
$cur->inputBind("1",8);
$cur->inputBind("2",8.8,4,2);
$cur->inputBind("3",8.8,4,2);
$cur->inputBind("4",8);
$cur->inputBind("5","testchar8");
$cur->inputBind("6","testvarchar8");
$cur->inputBind("7","01/01/2008");
$cur->inputBind("8","08:00:00");
$cur->validateBinds();
checkSuccess($cur->executeQuery(),1);
print("\n");

print("SELECT: \n");
checkSuccess($cur->sendQuery("select * from testtable order by testint"),1);
print("\n");

print("COLUMN COUNT: \n");
checkSuccess($cur->colCount(),9);
print("\n");

print("COLUMN NAMES: \n");
checkSuccessString($cur->getColumnName(0),"testint");
checkSuccessString($cur->getColumnName(1),"testfloat");
checkSuccessString($cur->getColumnName(2),"testreal");
checkSuccessString($cur->getColumnName(3),"testsmallint");
checkSuccessString($cur->getColumnName(4),"testchar");
checkSuccessString($cur->getColumnName(5),"testvarchar");
checkSuccessString($cur->getColumnName(6),"testdate");
checkSuccessString($cur->getColumnName(7),"testtime");
checkSuccessString($cur->getColumnName(8),"testtimestamp");
@cols=$cur->getColumnNames();
checkSuccessString($cols[0],"testint");
checkSuccessString($cols[1],"testfloat");
checkSuccessString($cols[2],"testreal");
checkSuccessString($cols[3],"testsmallint");
checkSuccessString($cols[4],"testchar");
checkSuccessString($cols[5],"testvarchar");
checkSuccessString($cols[6],"testdate");
checkSuccessString($cols[7],"testtime");
checkSuccessString($cols[8],"testtimestamp");
print("\n");

print("COLUMN TYPES: \n");
checkSuccessString($cur->getColumnType(0),"int4");
checkSuccessString($cur->getColumnType('testint'),"int4");
checkSuccessString($cur->getColumnType(1),"float8");
checkSuccessString($cur->getColumnType('testfloat'),"float8");
checkSuccessString($cur->getColumnType(2),"float4");
checkSuccessString($cur->getColumnType('testreal'),"float4");
checkSuccessString($cur->getColumnType(3),"int2");
checkSuccessString($cur->getColumnType('testsmallint'),"int2");
checkSuccessString($cur->getColumnType(4),"bpchar");
checkSuccessString($cur->getColumnType('testchar'),"bpchar");
checkSuccessString($cur->getColumnType(5),"varchar");
checkSuccessString($cur->getColumnType('testvarchar'),"varchar");
checkSuccessString($cur->getColumnType(6),"date");
checkSuccessString($cur->getColumnType('testdate'),"date");
checkSuccessString($cur->getColumnType(7),"time");
checkSuccessString($cur->getColumnType('testtime'),"time");
checkSuccessString($cur->getColumnType(8),"timestamp");
checkSuccessString($cur->getColumnType('testtimestamp'),"timestamp");
print("\n");

print("COLUMN LENGTH: \n");
checkSuccess($cur->getColumnLength(0),4);
checkSuccess($cur->getColumnLength('testint'),4);
checkSuccess($cur->getColumnLength(1),8);
checkSuccess($cur->getColumnLength('testfloat'),8);
checkSuccess($cur->getColumnLength(2),4);
checkSuccess($cur->getColumnLength('testreal'),4);
checkSuccess($cur->getColumnLength(3),2);
checkSuccess($cur->getColumnLength('testsmallint'),2);
checkSuccess($cur->getColumnLength(4),44);
checkSuccess($cur->getColumnLength('testchar'),44);
checkSuccess($cur->getColumnLength(5),44);
checkSuccess($cur->getColumnLength('testvarchar'),44);
checkSuccess($cur->getColumnLength(6),4);
checkSuccess($cur->getColumnLength('testdate'),4);
checkSuccess($cur->getColumnLength(7),8);
checkSuccess($cur->getColumnLength('testtime'),8);
checkSuccess($cur->getColumnLength(8),8);
checkSuccess($cur->getColumnLength('testtimestamp'),8);
print("\n");

print("LONGEST COLUMN: \n");
checkSuccess($cur->getLongest(0),1);
checkSuccess($cur->getLongest('testint'),1);
checkSuccess($cur->getLongest(1),3);
checkSuccess($cur->getLongest('testfloat'),3);
checkSuccess($cur->getLongest(2),3);
checkSuccess($cur->getLongest('testreal'),3);
checkSuccess($cur->getLongest(3),1);
checkSuccess($cur->getLongest('testsmallint'),1);
checkSuccess($cur->getLongest(4),40);
checkSuccess($cur->getLongest('testchar'),40);
checkSuccess($cur->getLongest(5),12);
checkSuccess($cur->getLongest('testvarchar'),12);
checkSuccess($cur->getLongest(6),10);
checkSuccess($cur->getLongest('testdate'),10);
checkSuccess($cur->getLongest(7),8);
checkSuccess($cur->getLongest('testtime'),8);
print("\n");

print("ROW COUNT: \n");
checkSuccess($cur->rowCount(),8);
print("\n");

#print("TOTAL ROWS: \n");
#checkSuccess($cur->totalRows(),8);
#print("\n");

print("FIRST ROW INDEX: \n");
checkSuccess($cur->firstRowIndex(),0);
print("\n");

print("END OF RESULT SET: \n");
checkSuccess($cur->endOfResultSet(),1);
print("\n");

print("FIELDS BY INDEX: \n");
checkSuccessString($cur->getField(0,0),"1");
checkSuccessString($cur->getField(0,1),"1.1");
checkSuccessString($cur->getField(0,2),"1.1");
checkSuccessString($cur->getField(0,3),"1");
checkSuccessString($cur->getField(0,4),"testchar1                               ");
checkSuccessString($cur->getField(0,5),"testvarchar1");
checkSuccessString($cur->getField(0,6),"2001-01-01");
checkSuccessString($cur->getField(0,7),"01:00:00");
print("\n");
checkSuccessString($cur->getField(7,0),"8");
checkSuccessString($cur->getField(7,1),"8.8");
checkSuccessString($cur->getField(7,2),"8.8");
checkSuccessString($cur->getField(7,3),"8");
checkSuccessString($cur->getField(7,4),"testchar8                               ");
checkSuccessString($cur->getField(7,5),"testvarchar8");
checkSuccessString($cur->getField(7,6),"2008-01-01");
checkSuccessString($cur->getField(7,7),"08:00:00");
print("\n");

print("FIELD LENGTHS BY INDEX: \n");
checkSuccess($cur->getFieldLength(0,0),1);
checkSuccess($cur->getFieldLength(0,1),3);
checkSuccess($cur->getFieldLength(0,2),3);
checkSuccess($cur->getFieldLength(0,3),1);
checkSuccess($cur->getFieldLength(0,4),40);
checkSuccess($cur->getFieldLength(0,5),12);
checkSuccess($cur->getFieldLength(0,6),10);
checkSuccess($cur->getFieldLength(0,7),8);
print("\n");
checkSuccess($cur->getFieldLength(7,0),1);
checkSuccess($cur->getFieldLength(7,1),3);
checkSuccess($cur->getFieldLength(7,2),3);
checkSuccess($cur->getFieldLength(7,3),1);
checkSuccess($cur->getFieldLength(7,4),40);
checkSuccess($cur->getFieldLength(7,5),12);
checkSuccess($cur->getFieldLength(7,6),10);
checkSuccess($cur->getFieldLength(7,7),8);
print("\n");

print("FIELDS BY NAME: \n");
checkSuccessString($cur->getField(0,"testint"),"1");
checkSuccessString($cur->getField(0,"testfloat"),"1.1");
checkSuccessString($cur->getField(0,"testreal"),"1.1");
checkSuccessString($cur->getField(0,"testsmallint"),"1");
checkSuccessString($cur->getField(0,"testchar"),"testchar1                               ");
checkSuccessString($cur->getField(0,"testvarchar"),"testvarchar1");
checkSuccessString($cur->getField(0,"testdate"),"2001-01-01");
checkSuccessString($cur->getField(0,"testtime"),"01:00:00");
print("\n");
checkSuccessString($cur->getField(7,"testint"),"8");
checkSuccessString($cur->getField(7,"testfloat"),"8.8");
checkSuccessString($cur->getField(7,"testreal"),"8.8");
checkSuccessString($cur->getField(7,"testsmallint"),"8");
checkSuccessString($cur->getField(7,"testchar"),"testchar8                               ");
checkSuccessString($cur->getField(7,"testvarchar"),"testvarchar8");
checkSuccessString($cur->getField(7,"testdate"),"2008-01-01");
checkSuccessString($cur->getField(7,"testtime"),"08:00:00");
print("\n");

print("FIELD LENGTHS BY NAME: \n");
checkSuccess($cur->getFieldLength(0,"testint"),1);
checkSuccess($cur->getFieldLength(0,"testfloat"),3);
checkSuccess($cur->getFieldLength(0,"testreal"),3);
checkSuccess($cur->getFieldLength(0,"testsmallint"),1);
checkSuccess($cur->getFieldLength(0,"testchar"),40);
checkSuccess($cur->getFieldLength(0,"testvarchar"),12);
checkSuccess($cur->getFieldLength(0,"testdate"),10);
checkSuccess($cur->getFieldLength(0,"testtime"),8);
print("\n");
checkSuccess($cur->getFieldLength(7,"testint"),1);
checkSuccess($cur->getFieldLength(7,"testfloat"),3);
checkSuccess($cur->getFieldLength(7,"testreal"),3);
checkSuccess($cur->getFieldLength(7,"testsmallint"),1);
checkSuccess($cur->getFieldLength(7,"testchar"),40);
checkSuccess($cur->getFieldLength(7,"testvarchar"),12);
checkSuccess($cur->getFieldLength(7,"testdate"),10);
checkSuccess($cur->getFieldLength(7,"testtime"),8);
print("\n");

print("FIELDS BY ARRAY: \n");
@fields=$cur->getRow(0);
checkSuccess($fields[0],1);
checkSuccess($fields[1],1.1);
checkSuccess($fields[2],1.1);
checkSuccess($fields[3],1);
checkSuccessString($fields[4],"testchar1                               ");
checkSuccessString($fields[5],"testvarchar1");
checkSuccessString($fields[6],"2001-01-01");
checkSuccessString($fields[7],"01:00:00");
print("\n");

print("FIELD LENGTHS BY ARRAY: \n");
@fieldlens=$cur->getRowLengths(0);
checkSuccess($fieldlens[0],1);
checkSuccess($fieldlens[1],3);
checkSuccess($fieldlens[2],3);
checkSuccess($fieldlens[3],1);
checkSuccess($fieldlens[4],40);
checkSuccess($fieldlens[5],12);
checkSuccess($fieldlens[6],10);
checkSuccess($fieldlens[7],8);
print("\n");

print("FIELDS BY HASH: \n");
%fields=$cur->getRowHash(0);
checkSuccess($fields{"testint"},1);
checkSuccess($fields{"testfloat"},1.1);
checkSuccess($fields{"testreal"},1.1);
checkSuccess($fields{"testsmallint"},1);
checkSuccessString($fields{"testchar"},"testchar1                               ");
checkSuccessString($fields{"testvarchar"},"testvarchar1");
checkSuccessString($fields{"testdate"},"2001-01-01");
checkSuccessString($fields{"testtime"},"01:00:00");
print("\n");
%fields=$cur->getRowHash(7);
checkSuccess($fields{"testint"},8);
checkSuccess($fields{"testfloat"},8.8);
checkSuccess($fields{"testreal"},8.8);
checkSuccess($fields{"testsmallint"},8);
checkSuccessString($fields{"testchar"},"testchar8                               ");
checkSuccessString($fields{"testvarchar"},"testvarchar8");
checkSuccessString($fields{"testdate"},"2008-01-01");
checkSuccessString($fields{"testtime"},"08:00:00");
print("\n");

print("FIELD LENGTHS BY HASH: \n");
%fieldlengths=$cur->getRowLengthsHash(0);
checkSuccess($fieldlengths{"testint"},1);
checkSuccess($fieldlengths{"testfloat"},3);
checkSuccess($fieldlengths{"testreal"},3);
checkSuccess($fieldlengths{"testsmallint"},1);
checkSuccess($fieldlengths{"testchar"},40);
checkSuccess($fieldlengths{"testvarchar"},12);
checkSuccess($fieldlengths{"testdate"},10);
checkSuccess($fieldlengths{"testtime"},8);
print("\n");
%fieldlengths=$cur->getRowLengthsHash(7);
checkSuccess($fieldlengths{"testint"},1);
checkSuccess($fieldlengths{"testfloat"},3);
checkSuccess($fieldlengths{"testreal"},3);
checkSuccess($fieldlengths{"testsmallint"},1);
checkSuccess($fieldlengths{"testchar"},40);
checkSuccess($fieldlengths{"testvarchar"},12);
checkSuccess($fieldlengths{"testdate"},10);
checkSuccess($fieldlengths{"testtime"},8);
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
@specs=(0,0,6);
@precs=(0,0,4);
$cur->substitutions(\@vars,\@vals,\@specs,\@precs);
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
checkSuccessString($cur->getColumnType(0),"int4");
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
checkSuccess($cur->colCount(),9);
print("\n");

print("COLUMN NAMES FOR CACHED RESULT SET: \n");
checkSuccessString($cur->getColumnName(0),"testint");
checkSuccessString($cur->getColumnName(1),"testfloat");
checkSuccessString($cur->getColumnName(2),"testreal");
checkSuccessString($cur->getColumnName(3),"testsmallint");
checkSuccessString($cur->getColumnName(4),"testchar");
checkSuccessString($cur->getColumnName(5),"testvarchar");
checkSuccessString($cur->getColumnName(6),"testdate");
checkSuccessString($cur->getColumnName(7),"testtime");
checkSuccessString($cur->getColumnName(8),"testtimestamp");
@cols=$cur->getColumnNames();
checkSuccessString($cols[0],"testint");
checkSuccessString($cols[1],"testfloat");
checkSuccessString($cols[2],"testreal");
checkSuccessString($cols[3],"testsmallint");
checkSuccessString($cols[4],"testchar");
checkSuccessString($cols[5],"testvarchar");
checkSuccessString($cols[6],"testdate");
checkSuccessString($cols[7],"testtime");
checkSuccessString($cols[8],"testtimestamp");
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

print("COMMIT AND ROLLBACK: \n");
$secondcon=SQLRelay::Connection->new("sqlrelay",9000,"/tmp/test.socket",
							"test","test",0,1);
$secondcur=SQLRelay::Cursor->new($secondcon);
checkSuccess($secondcur->sendQuery("select count(*) from testtable"),1);
checkSuccessString($secondcur->getField(0,0),"0");
checkSuccess($con->commit(),1);
checkSuccess($secondcur->sendQuery("select count(*) from testtable"),1);
checkSuccessString($secondcur->getField(0,0),"8");
#checkSuccess($con->autoCommitOn(),1);
checkSuccess($cur->sendQuery("insert into testtable values (10,10.1,10.1,10,'testchar10','testvarchar10','01/01/2010','10:00:00',NULL)"),1);
checkSuccess($secondcur->sendQuery("select count(*) from testtable"),1);
checkSuccessString($secondcur->getField(0,0),"9");
#checkSuccess($con->autoCommitOff(),1);
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

# stored procedures
print("STORED PROCEDURES: \n");
# return no values
$cur->sendQuery("drop function testfunc(int,float,char(20))");
checkSuccess($cur->sendQuery("create function testfunc(int,float,char(20)) returns void as ' declare in1 int; in2 float; in3 char(20); begin in1:=\$1; in2:=\$2; in3:=\$3; return; end;' language plpgsql"),1);
$cur->prepareQuery("select testfunc(\$1,\$2,\$3)");
$cur->inputBind("1",1);
$cur->inputBind("2",1.1,4,2);
$cur->inputBind("3","hello");
checkSuccess($cur->executeQuery(),1);
$cur->sendQuery("drop function testfunc(int,float,char(20))");
print("\n");
# return single value
$cur->sendQuery("drop function testfunc(int,float,char(20))");
checkSuccess($cur->sendQuery("create function testfunc(int,float,char(20)) returns int as ' begin return \$1; end;' language plpgsql"),1);
$cur->prepareQuery("select * from testfunc(\$1,\$2,\$3)");
$cur->inputBind("1",1);
$cur->inputBind("2",1.1,4,2);
$cur->inputBind("3","hello");
checkSuccess($cur->executeQuery(),1);
checkSuccess($cur->getField(0,0),"1");
$cur->sendQuery("drop function testfunc(int,float,char(20))");
print("\n");
# return multiple values
$cur->sendQuery("drop function testfunc(int,char(20))");
checkSuccess($cur->sendQuery("create function testfunc(int,float,char(20)) returns record as ' declare output record; begin select \$1,\$2,\$3 into output; return output; end;' language plpgsql"),1);
$cur->prepareQuery("select * from testfunc(\$1,\$2,\$3) as (col1 int, col2 float, col3 bpchar)");
$cur->inputBind("1",1);
$cur->inputBind("2",1.1,4,2);
$cur->inputBind("3","hello");
checkSuccess($cur->executeQuery(),1);
checkSuccess($cur->getField(0,0),"1");
checkSuccess($cur->getField(0,1),1.1);
checkSuccess($cur->getField(0,2),"hello");
$cur->sendQuery("drop function testfunc(int,float,char(20))");
print("\n");
# return result set
$cur->sendQuery("drop function testfunc()");
checkSuccess($cur->sendQuery("create function testfunc() returns setof record as ' declare output record; begin for output in select * from testtable loop return next output; end loop; return; end;' language plpgsql"),1);
checkSuccess($cur->sendQuery("select * from testfunc() as (testint int, testfloat float, testreal real, testsmallint smallint, testchar char(40), testvarchar varchar(40), testdate date, testtime time, testtimestamp timestamp)"),1);
checkSuccess($cur->getField(4,0),"5");
checkSuccess($cur->getField(5,0),"6");
checkSuccess($cur->getField(6,0),"7");
checkSuccess($cur->getField(7,0),"8");
$cur->sendQuery("drop function testfunc()");
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



