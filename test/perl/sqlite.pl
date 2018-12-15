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
checkSuccess($con->identify(),"sqlite");
print("\n");

# ping
print("PING: \n");
checkSuccess($con->ping(),1);
print("\n");

# drop existing table
$cur->sendQuery("begin transaction");
$cur->sendQuery("drop table testtable");
$con->commit();

# create a new table
print("CREATE TEMPTABLE: \n");
$cur->sendQuery("begin transaction");
checkSuccess($cur->sendQuery("create table testtable (testint int, testfloat float, testchar char(40), testvarchar varchar(40))"),1);
$con->commit();
print("\n");

print("INSERT: \n");
$cur->sendQuery("begin transaction");
checkSuccess($cur->sendQuery("insert into testtable values (1,1.1,'testchar1','testvarchar1')"),1);
checkSuccess($cur->sendQuery("insert into testtable values (2,2.2,'testchar2','testvarchar2')"),1);
checkSuccess($cur->sendQuery("insert into testtable values (3,3.3,'testchar3','testvarchar3')"),1);
checkSuccess($cur->sendQuery("insert into testtable values (4,4.4,'testchar4','testvarchar4')"),1);
print("\n");

print("AFFECTED ROWS: \n");
checkSuccess($cur->affectedRows(),0);
print("\n");

print("BIND BY NAME: \n");
$cur->prepareQuery("insert into testtable values (:var1,:var2,:var3,:var4)");
checkSuccess($cur->countBindVariables(),4);
$cur->inputBind("var1",5);
$cur->inputBind("var2",5.5,4,1);
$cur->inputBind("var3","testchar5");
$cur->inputBind("var4","testvarchar5");
checkSuccess($cur->executeQuery(),1);
$cur->clearBinds();
$cur->inputBind("var1",6);
$cur->inputBind("var2",6.6,4,1);
$cur->inputBind("var3","testchar6");
$cur->inputBind("var4","testvarchar6");
checkSuccess($cur->executeQuery(),1);
print("\n");

print("ARRAY OF BINDS BY NAME: \n");
$cur->clearBinds();
@vars=("var1","var2","var3","var4");
@vals=(7,7.7,"testchar7","testvarchar7");
@precs=(0,4,0,0);
@scales=(0,1,0,0);
$cur->inputBinds(\@vars,\@vals,\@precs,\@scales);
checkSuccess($cur->executeQuery(),1);
print("\n");

print("BIND BY NAME WITH VALIDATION: \n");
$cur->clearBinds();
$cur->inputBind("var1",8);
$cur->inputBind("var2",8.8,4,1);
$cur->inputBind("var3","testchar8");
$cur->inputBind("var4","testvarchar8");
$cur->validateBinds();
checkSuccess($cur->executeQuery(),1);
print("\n");

print("SELECT: \n");
checkSuccess($cur->sendQuery("select * from testtable order by testint"),1);
print("\n");

print("COLUMN COUNT: \n");
checkSuccess($cur->colCount(),4);
print("\n");

print("COLUMN NAMES: \n");
checkSuccess($cur->getColumnName(0),"testint");
checkSuccess($cur->getColumnName(1),"testfloat");
checkSuccess($cur->getColumnName(2),"testchar");
checkSuccess($cur->getColumnName(3),"testvarchar");
@cols=$cur->getColumnNames();
checkSuccess($cols[0],"testint");
checkSuccess($cols[1],"testfloat");
checkSuccess($cols[2],"testchar");
checkSuccess($cols[3],"testvarchar");
print("\n");

print("COLUMN TYPES: \n");
checkSuccess($cur->getColumnType(0),"INTEGER");
checkSuccess($cur->getColumnType('testint'),"INTEGER");
checkSuccess($cur->getColumnType(1),"FLOAT");
checkSuccess($cur->getColumnType('testfloat'),"FLOAT");
checkSuccess($cur->getColumnType(2),"STRING");
checkSuccess($cur->getColumnType('testchar'),"STRING");
checkSuccess($cur->getColumnType(3),"STRING");
checkSuccess($cur->getColumnType('testvarchar'),"STRING");
print("\n");

print("COLUMN LENGTH: \n");
checkSuccess($cur->getColumnLength(0),0);
checkSuccess($cur->getColumnLength('testint'),0);
checkSuccess($cur->getColumnLength(1),0);
checkSuccess($cur->getColumnLength('testfloat'),0);
checkSuccess($cur->getColumnLength(2),0);
checkSuccess($cur->getColumnLength('testchar'),0);
checkSuccess($cur->getColumnLength(3),0);
checkSuccess($cur->getColumnLength('testvarchar'),0);
print("\n");

print("LONGEST COLUMN: \n");
checkSuccess($cur->getLongest(0),1);
checkSuccess($cur->getLongest('testint'),1);
checkSuccess($cur->getLongest(1),3);
checkSuccess($cur->getLongest('testfloat'),3);
checkSuccess($cur->getLongest(2),9);
checkSuccess($cur->getLongest('testchar'),9);
checkSuccess($cur->getLongest(3),12);
checkSuccess($cur->getLongest('testvarchar'),12);
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
checkSuccess($cur->getField(0,0),"1");
checkSuccess($cur->getField(0,1),"1.1");
checkSuccess($cur->getField(0,2),"testchar1");
checkSuccess($cur->getField(0,3),"testvarchar1");
print("\n");
checkSuccess($cur->getField(7,0),"8");
checkSuccess($cur->getField(7,1),"8.8");
checkSuccess($cur->getField(7,2),"testchar8");
checkSuccess($cur->getField(7,3),"testvarchar8");
print("\n");

print("FIELD LENGTHS BY INDEX: \n");
checkSuccess($cur->getFieldLength(0,0),1);
checkSuccess($cur->getFieldLength(0,1),3);
checkSuccess($cur->getFieldLength(0,2),9);
checkSuccess($cur->getFieldLength(0,3),12);
print("\n");
checkSuccess($cur->getFieldLength(7,0),1);
checkSuccess($cur->getFieldLength(7,1),3);
checkSuccess($cur->getFieldLength(7,2),9);
checkSuccess($cur->getFieldLength(7,3),12);
print("\n");

print("FIELDS BY NAME: \n");
checkSuccess($cur->getField(0,"testint"),"1");
checkSuccess($cur->getField(0,"testfloat"),"1.1");
checkSuccess($cur->getField(0,"testchar"),"testchar1");
checkSuccess($cur->getField(0,"testvarchar"),"testvarchar1");
print("\n");
checkSuccess($cur->getField(7,"testint"),"8");
checkSuccess($cur->getField(7,"testfloat"),"8.8");
checkSuccess($cur->getField(7,"testchar"),"testchar8");
checkSuccess($cur->getField(7,"testvarchar"),"testvarchar8");
print("\n");

print("FIELD LENGTHS BY NAME: \n");
checkSuccess($cur->getFieldLength(0,"testint"),1);
checkSuccess($cur->getFieldLength(0,"testfloat"),3);
checkSuccess($cur->getFieldLength(0,"testchar"),9);
checkSuccess($cur->getFieldLength(0,"testvarchar"),12);
print("\n");
checkSuccess($cur->getFieldLength(7,"testint"),1);
checkSuccess($cur->getFieldLength(7,"testfloat"),3);
checkSuccess($cur->getFieldLength(7,"testchar"),9);
checkSuccess($cur->getFieldLength(7,"testvarchar"),12);
print("\n");

print("FIELDS BY ARRAY: \n");
@fields=$cur->getRow(0);
checkSuccess($fields[0],"1");
checkSuccess($fields[1],"1.1");
checkSuccess($fields[2],"testchar1");
checkSuccess($fields[3],"testvarchar1");
print("\n");

print("FIELD LENGTHS BY ARRAY: \n");
@fieldlens=$cur->getRowLengths(0);
checkSuccess($fieldlens[0],1);
checkSuccess($fieldlens[1],3);
checkSuccess($fieldlens[2],9);
checkSuccess($fieldlens[3],12);
print("\n");

print("FIELDS BY HASH: \n");
%fields=$cur->getRowHash(0);
checkSuccess($fields{"testint"},"1");
checkSuccess($fields{"testfloat"},"1.1");
checkSuccess($fields{"testchar"},"testchar1");
checkSuccess($fields{"testvarchar"},"testvarchar1");
print("\n");
%fields=$cur->getRowHash(7);
checkSuccess($fields{"testint"},"8");
checkSuccess($fields{"testfloat"},"8.8");
checkSuccess($fields{"testchar"},"testchar8");
checkSuccess($fields{"testvarchar"},"testvarchar8");
print("\n");

print("FIELD LENGTHS BY HASH: \n");
%fieldlengths=$cur->getRowLengthsHash(0);
checkSuccess($fieldlengths{"testint"},1);
checkSuccess($fieldlengths{"testfloat"},3);
checkSuccess($fieldlengths{"testchar"},9);
checkSuccess($fieldlengths{"testvarchar"},12);
print("\n");
%fieldlengths=$cur->getRowLengthsHash(7);
checkSuccess($fieldlengths{"testint"},1);
checkSuccess($fieldlengths{"testfloat"},3);
checkSuccess($fieldlengths{"testchar"},9);
checkSuccess($fieldlengths{"testvarchar"},12);
print("\n");
	
print("INDIVIDUAL SUBSTITUTIONS: \n");
$cur->sendQuery("drop table testtable1");
checkSuccess($cur->sendQuery("create table testtable1 (col1 int, col2 char, col3 float)"),1);
$cur->prepareQuery("insert into testtable1 values (\$(var1),'\$(var2)',\$(var3))");
$cur->substitution("var1",1);
$cur->substitution("var2","hello");
$cur->substitution("var3",10.5556,6,4);
checkSuccess($cur->executeQuery(),1);
print("\n");

print("FIELDS: \n");
checkSuccess($cur->sendQuery("select * from testtable1"),1);
checkSuccess($cur->getField(0,0),"1");
checkSuccess($cur->getField(0,1),"hello");
checkSuccess($cur->getField(0,2),"10.5556");
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
checkSuccess($cur->getField(0,0),"1");
checkSuccess($cur->getField(0,1),"hello");
checkSuccess($cur->getField(0,2),"10.5556");
checkSuccess($cur->sendQuery("delete from testtable1"),1);
print("\n");

print("NULLS as Undef: \n");
$cur->getNullsAsUndefined();
checkSuccess($cur->sendQuery("insert into testtable1 values (1,NULL,NULL)"),1);
checkSuccess($cur->sendQuery("select * from testtable1"),1);
checkSuccess($cur->getField(0,0),"1");
checkUndef($cur->getField(0,1));
checkUndef($cur->getField(0,2));
$cur->getNullsAsEmptyStrings();
checkSuccess($cur->sendQuery("select * from testtable1"),1);
checkSuccess($cur->getField(0,0),"1");
checkSuccess($cur->getField(0,1),"");
checkSuccess($cur->getField(0,2),"");
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
checkSuccess($cur->getField(0,0),"1");
checkSuccess($cur->getField(1,0),"2");
checkSuccess($cur->getField(2,0),"3");
print("\n");
checkSuccess($cur->firstRowIndex(),2);
checkSuccess($cur->endOfResultSet(),0);
checkSuccess($cur->rowCount(),4);
checkSuccess($cur->getField(6,0),"7");
checkSuccess($cur->getField(7,0),"8");
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
checkSuccess($cur->getColumnName(0),"testint");
checkSuccess($cur->getColumnLength(0),0);
checkSuccess($cur->getColumnType(0),"INTEGER");
print("\n");

print("SUSPENDED SESSION: \n");
checkSuccess($cur->sendQuery("select * from testtable order by testint"),1);
$cur->suspendResultSet();
checkSuccess($con->suspendSession(),1);
$port=$con->getConnectionPort();
$socket=$con->getConnectionSocket();
checkSuccess($con->resumeSession($port,$socket),1);
print("\n");
checkSuccess($cur->getField(0,0),"1");
checkSuccess($cur->getField(1,0),"2");
checkSuccess($cur->getField(2,0),"3");
checkSuccess($cur->getField(3,0),"4");
checkSuccess($cur->getField(4,0),"5");
checkSuccess($cur->getField(5,0),"6");
checkSuccess($cur->getField(6,0),"7");
checkSuccess($cur->getField(7,0),"8");
print("\n");
checkSuccess($cur->sendQuery("select * from testtable order by testint"),1);
$cur->suspendResultSet();
checkSuccess($con->suspendSession(),1);
$port=$con->getConnectionPort();
$socket=$con->getConnectionSocket();
checkSuccess($con->resumeSession($port,$socket),1);
print("\n");
checkSuccess($cur->getField(0,0),"1");
checkSuccess($cur->getField(1,0),"2");
checkSuccess($cur->getField(2,0),"3");
checkSuccess($cur->getField(3,0),"4");
checkSuccess($cur->getField(4,0),"5");
checkSuccess($cur->getField(5,0),"6");
checkSuccess($cur->getField(6,0),"7");
checkSuccess($cur->getField(7,0),"8");
print("\n");
checkSuccess($cur->sendQuery("select * from testtable order by testint"),1);
$cur->suspendResultSet();
checkSuccess($con->suspendSession(),1);
$port=$con->getConnectionPort();
$socket=$con->getConnectionSocket();
checkSuccess($con->resumeSession($port,$socket),1);
print("\n");
checkSuccess($cur->getField(0,0),"1");
checkSuccess($cur->getField(1,0),"2");
checkSuccess($cur->getField(2,0),"3");
checkSuccess($cur->getField(3,0),"4");
checkSuccess($cur->getField(4,0),"5");
checkSuccess($cur->getField(5,0),"6");
checkSuccess($cur->getField(6,0),"7");
checkSuccess($cur->getField(7,0),"8");
print("\n");

print("SUSPENDED RESULT SET: \n");
$cur->setResultSetBufferSize(2);
checkSuccess($cur->sendQuery("select * from testtable order by testint"),1);
checkSuccess($cur->getField(2,0),"3");
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
checkSuccess($cur->getField(7,0),"8");
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
checkSuccess($filename,"cachefile1");
$cur->cacheOff();
checkSuccess($cur->openCachedResultSet($filename),1);
checkSuccess($cur->getField(7,0),"8");
print("\n");

print("COLUMN COUNT FOR CACHED RESULT SET: \n");
checkSuccess($cur->colCount(),4);
print("\n");

print("COLUMN NAMES FOR CACHED RESULT SET: \n");
checkSuccess($cur->getColumnName(0),"testint");
checkSuccess($cur->getColumnName(1),"testfloat");
checkSuccess($cur->getColumnName(2),"testchar");
checkSuccess($cur->getColumnName(3),"testvarchar");
@cols=$cur->getColumnNames();
checkSuccess($cols[0],"testint");
checkSuccess($cols[1],"testfloat");
checkSuccess($cols[2],"testchar");
checkSuccess($cols[3],"testvarchar");
print("\n");

print("CACHED RESULT SET WITH RESULT SET BUFFER SIZE: \n");
$cur->setResultSetBufferSize(2);
$cur->cacheToFile("cachefile1");
$cur->setCacheTtl(200);
checkSuccess($cur->sendQuery("select * from testtable order by testint"),1);
$filename=$cur->getCacheFileName();
checkSuccess($filename,"cachefile1");
$cur->cacheOff();
checkSuccess($cur->openCachedResultSet($filename),1);
checkSuccess($cur->getField(7,0),"8");
checkUndef($cur->getField(8,0));
$cur->setResultSetBufferSize(0);
print("\n");

print("FROM ONE CACHE FILE TO ANOTHER: \n");
$cur->cacheToFile("cachefile2");
checkSuccess($cur->openCachedResultSet("cachefile1"),1);
$cur->cacheOff();
checkSuccess($cur->openCachedResultSet("cachefile2"),1);
checkSuccess($cur->getField(7,0),"8");
checkUndef($cur->getField(8,0));
print("\n");

print("FROM ONE CACHE FILE TO ANOTHER WITH RESULT SET BUFFER SIZE: \n");
$cur->setResultSetBufferSize(2);
$cur->cacheToFile("cachefile2");
checkSuccess($cur->openCachedResultSet("cachefile1"),1);
$cur->cacheOff();
checkSuccess($cur->openCachedResultSet("cachefile2"),1);
checkSuccess($cur->getField(7,0),"8");
checkUndef($cur->getField(8,0));
$cur->setResultSetBufferSize(0);
print("\n");

print("CACHED RESULT SET WITH SUSPEND AND RESULT SET BUFFER SIZE: \n");
$cur->setResultSetBufferSize(2);
$cur->cacheToFile("cachefile1");
$cur->setCacheTtl(200);
checkSuccess($cur->sendQuery("select * from testtable order by testint"),1);
checkSuccess($cur->getField(2,0),"3");
$filename=$cur->getCacheFileName();
checkSuccess($filename,"cachefile1");
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
checkSuccess($cur->getField(7,0),"8");
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
checkSuccess($cur->getField(7,0),"8");
checkUndef($cur->getField(8,0));
$cur->setResultSetBufferSize(0);
print("\n");

print("COMMIT AND ROLLBACK: \n");
$secondcon=SQLRelay::Connection->new("sqlrelay",9000,"/tmp/test.socket",
							"test","test",0,1);
$secondcur=SQLRelay::Cursor->new($secondcon);
checkSuccess($secondcur->sendQuery("select count(*) from testtable"),1);
checkSuccess($secondcur->getField(0,0),"0");
checkSuccess($con->commit(),1);
checkSuccess($secondcur->sendQuery("select count(*) from testtable"),1);
checkSuccess($secondcur->getField(0,0),"8");
checkSuccess($cur->sendQuery("insert into testtable values (10,10.1,'testchar10','testvarchar10')"),1);
checkSuccess($secondcur->sendQuery("select count(*) from testtable"),1);
checkSuccess($secondcur->getField(0,0),"9");
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
