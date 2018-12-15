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
checkSuccessString($con->identify(),"db2");
print("\n");

# ping
print("PING: \n");
checkSuccess($con->ping(),1);
print("\n");

# drop existing table
$cur->sendQuery("drop table testtable");

print("CREATE TEMPTABLE: \n");
checkSuccess($cur->sendQuery("create table testtable (testsmallint smallint, testint integer, testbigint bigint, testdecimal decimal(10,2), testreal real, testdouble double, testchar char(40), testvarchar varchar(40), testdate date, testtime time, testtimestamp timestamp)"),1);
print("\n");

print("INSERT: \n");
checkSuccess($cur->sendQuery("insert into testtable values (1,1,1,1.1,1.1,1.1,'testchar1','testvarchar1','01/01/2001','01:00:00',NULL)"),1);
print("\n");

print("BIND BY POSITION: \n");
$cur->prepareQuery("insert into testtable values (?,?,?,?,?,?,?,?,?,?,NULL)");
checkSuccess($cur->countBindVariables(),10);
$cur->inputBind("1",2);
$cur->inputBind("2",2);
$cur->inputBind("3",2);
$cur->inputBind("4",2.2,4,2);
$cur->inputBind("5",2.2,4,2);
$cur->inputBind("6",2.2,4,2);
$cur->inputBind("7","testchar2");
$cur->inputBind("8","testvarchar2");
$cur->inputBind("9","01/01/2002");
$cur->inputBind("10","02:00:00");
checkSuccess($cur->executeQuery(),1);
$cur->clearBinds();
$cur->inputBind("1",3);
$cur->inputBind("2",3);
$cur->inputBind("3",3);
$cur->inputBind("4",3.3,4,2);
$cur->inputBind("5",3.3,4,2);
$cur->inputBind("6",3.3,4,2);
$cur->inputBind("7","testchar3");
$cur->inputBind("8","testvarchar3");
$cur->inputBind("9","01/01/2003");
$cur->inputBind("10","03:00:00");
checkSuccess($cur->executeQuery(),1);
print("\n");

print("ARRAY OF BINDS BY POSITION: \n");
$cur->clearBinds();
@vars=("1","2","3","4","5","6","7","8","9","10");
@vals=(4,4,4,4.4,4.4,4.4,"testchar4","testvarchar4","01/01/2004","04:00:00");
@precs=(0,0,0,4,4,4,0,0,0,0);
@scales=(0,0,0,2,2,2,0,0,0,0);
$cur->inputBinds(\@vars,\@vals,\@precs,\@scales);
checkSuccess($cur->executeQuery(),1);
print("\n");

print("INSERT: \n");
checkSuccess($cur->sendQuery("insert into testtable values (5,5,5,5.5,5.5,5.5,'testchar5','testvarchar5','01/01/2005','05:00:00',NULL)"),1);
checkSuccess($cur->sendQuery("insert into testtable values (6,6,6,6.6,6.6,6.6,'testchar6','testvarchar6','01/01/2006','06:00:00',NULL)"),1);
checkSuccess($cur->sendQuery("insert into testtable values (7,7,7,7.7,7.7,7.7,'testchar7','testvarchar7','01/01/2007','07:00:00',NULL)"),1);
checkSuccess($cur->sendQuery("insert into testtable values (8,8,8,8.8,8.8,8.8,'testchar8','testvarchar8','01/01/2008','08:00:00',NULL)"),1);
print("\n");

print("AFFECTED ROWS: \n");
checkSuccess($cur->affectedRows(),1);
print("\n");

print("STORED PROCEDURE: \n");
$cur->sendQuery("drop procedure testproc");
checkSuccess($cur->sendQuery("create procedure testproc(in invar int, out outvar int) language sql begin set outvar = invar; end"),1);
$cur->prepareQuery("call testproc(?,?)");
$cur->inputBind("1",5);
$cur->defineOutputBindString("2",10);
checkSuccess($cur->executeQuery(),1);
checkSuccess($cur->getOutputBindString("2"),"5");
checkSuccess($cur->sendQuery("drop procedure testproc"),1);
print("\n");

print("SELECT: \n");
checkSuccess($cur->sendQuery("select * from testtable order by testsmallint"),1);
print("\n");

print("COLUMN COUNT: \n");
checkSuccess($cur->colCount(),11);
print("\n");

print("COLUMN NAMES: \n");
checkSuccessString($cur->getColumnName(0),"TESTSMALLINT");
checkSuccessString($cur->getColumnName(1),"TESTINT");
checkSuccessString($cur->getColumnName(2),"TESTBIGINT");
checkSuccessString($cur->getColumnName(3),"TESTDECIMAL");
checkSuccessString($cur->getColumnName(4),"TESTREAL");
checkSuccessString($cur->getColumnName(5),"TESTDOUBLE");
checkSuccessString($cur->getColumnName(6),"TESTCHAR");
checkSuccessString($cur->getColumnName(7),"TESTVARCHAR");
checkSuccessString($cur->getColumnName(8),"TESTDATE");
checkSuccessString($cur->getColumnName(9),"TESTTIME");
checkSuccessString($cur->getColumnName(10),"TESTTIMESTAMP");
@cols=$cur->getColumnNames();
checkSuccessString($cols[0],"TESTSMALLINT");
checkSuccessString($cols[1],"TESTINT");
checkSuccessString($cols[2],"TESTBIGINT");
checkSuccessString($cols[3],"TESTDECIMAL");
checkSuccessString($cols[4],"TESTREAL");
checkSuccessString($cols[5],"TESTDOUBLE");
checkSuccessString($cols[6],"TESTCHAR");
checkSuccessString($cols[7],"TESTVARCHAR");
checkSuccessString($cols[8],"TESTDATE");
checkSuccessString($cols[9],"TESTTIME");
checkSuccessString($cols[10],"TESTTIMESTAMP");
print("\n");

print("COLUMN TYPES: \n");
checkSuccessString($cur->getColumnType(0),"SMALLINT");
checkSuccessString($cur->getColumnType('TESTSMALLINT'),"SMALLINT");
checkSuccessString($cur->getColumnType(1),"INTEGER");
checkSuccessString($cur->getColumnType('TESTINT'),"INTEGER");
checkSuccessString($cur->getColumnType(2),"BIGINT");
checkSuccessString($cur->getColumnType('TESTBIGINT'),"BIGINT");
checkSuccessString($cur->getColumnType(3),"DECIMAL");
checkSuccessString($cur->getColumnType('TESTDECIMAL'),"DECIMAL");
checkSuccessString($cur->getColumnType(4),"REAL");
checkSuccessString($cur->getColumnType('TESTREAL'),"REAL");
checkSuccessString($cur->getColumnType(5),"DOUBLE");
checkSuccessString($cur->getColumnType('TESTDOUBLE'),"DOUBLE");
checkSuccessString($cur->getColumnType(6),"CHAR");
checkSuccessString($cur->getColumnType('TESTCHAR'),"CHAR");
checkSuccessString($cur->getColumnType(7),"VARCHAR");
checkSuccessString($cur->getColumnType('TESTVARCHAR'),"VARCHAR");
checkSuccessString($cur->getColumnType(8),"DATE");
checkSuccessString($cur->getColumnType('TESTDATE'),"DATE");
checkSuccessString($cur->getColumnType(9),"TIME");
checkSuccessString($cur->getColumnType('TESTTIME'),"TIME");
checkSuccessString($cur->getColumnType(10),"TIMESTAMP");
checkSuccessString($cur->getColumnType('TESTTIMESTAMP'),"TIMESTAMP");
print("\n");

print("COLUMN LENGTH: \n");
checkSuccess($cur->getColumnLength(0),2);
checkSuccess($cur->getColumnLength('TESTSMALLINT'),2);
checkSuccess($cur->getColumnLength(1),4);
checkSuccess($cur->getColumnLength('TESTINT'),4);
checkSuccess($cur->getColumnLength(2),8);
checkSuccess($cur->getColumnLength('TESTBIGINT'),8);
checkSuccess($cur->getColumnLength(3),12);
checkSuccess($cur->getColumnLength('TESTDECIMAL'),12);
checkSuccess($cur->getColumnLength(4),4);
checkSuccess($cur->getColumnLength('TESTREAL'),4);
checkSuccess($cur->getColumnLength(5),8);
checkSuccess($cur->getColumnLength('TESTDOUBLE'),8);
checkSuccess($cur->getColumnLength(6),40);
checkSuccess($cur->getColumnLength('TESTCHAR'),40);
checkSuccess($cur->getColumnLength(7),40);
checkSuccess($cur->getColumnLength('TESTVARCHAR'),40);
checkSuccess($cur->getColumnLength(8),6);
checkSuccess($cur->getColumnLength('TESTDATE'),6);
checkSuccess($cur->getColumnLength(9),6);
checkSuccess($cur->getColumnLength('TESTTIME'),6);
checkSuccess($cur->getColumnLength(10),16);
checkSuccess($cur->getColumnLength('TESTTIMESTAMP'),16);
print("\n");

print("LONGEST COLUMN: \n");
checkSuccess($cur->getLongest(0),1);
checkSuccess($cur->getLongest('TESTSMALLINT'),1);
checkSuccess($cur->getLongest(1),1);
checkSuccess($cur->getLongest('TESTINT'),1);
checkSuccess($cur->getLongest(2),1);
checkSuccess($cur->getLongest('TESTBIGINT'),1);
checkSuccess($cur->getLongest(3),4);
checkSuccess($cur->getLongest('TESTDECIMAL'),4);
#checkSuccess($cur->getLongest(4),3);
#checkSuccess($cur->getLongest('TESTREAL'),3);
#checkSuccess($cur->getLongest(5),3);
#checkSuccess($cur->getLongest('TESTDOUBLE'),3);
checkSuccess($cur->getLongest(6),40);
checkSuccess($cur->getLongest('TESTCHAR'),40);
checkSuccess($cur->getLongest(7),12);
checkSuccess($cur->getLongest('TESTVARCHAR'),12);
checkSuccess($cur->getLongest(8),10);
checkSuccess($cur->getLongest('TESTDATE'),10);
checkSuccess($cur->getLongest(9),8);
checkSuccess($cur->getLongest('TESTTIME'),8);
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
checkSuccessString($cur->getField(0,3),"1.10");
#checkSuccessString($cur->getField(0,4),"1.1");
#checkSuccessString($cur->getField(0,5),"1.1");
checkSuccessString($cur->getField(0,6),"testchar1                               ");
checkSuccessString($cur->getField(0,7),"testvarchar1");
checkSuccessString($cur->getField(0,8),"2001-01-01");
checkSuccessString($cur->getField(0,9),"01:00:00");
print("\n");
checkSuccessString($cur->getField(7,0),"8");
checkSuccessString($cur->getField(7,1),"8");
checkSuccessString($cur->getField(7,2),"8");
checkSuccessString($cur->getField(7,3),"8.80");
#checkSuccessString($cur->getField(7,4),"8.8");
#checkSuccessString($cur->getField(7,5),"8.8");
checkSuccessString($cur->getField(7,6),"testchar8                               ");
checkSuccessString($cur->getField(7,7),"testvarchar8");
checkSuccessString($cur->getField(7,8),"2008-01-01");
checkSuccessString($cur->getField(7,9),"08:00:00");
print("\n");

print("FIELD LENGTHS BY INDEX: \n");
checkSuccess($cur->getFieldLength(0,0),1);
checkSuccess($cur->getFieldLength(0,1),1);
checkSuccess($cur->getFieldLength(0,2),1);
checkSuccess($cur->getFieldLength(0,3),4);
#checkSuccess($cur->getFieldLength(0,4),3);
#checkSuccess($cur->getFieldLength(0,5),3);
checkSuccess($cur->getFieldLength(0,6),40);
checkSuccess($cur->getFieldLength(0,7),12);
checkSuccess($cur->getFieldLength(0,8),10);
checkSuccess($cur->getFieldLength(0,9),8);
print("\n");
checkSuccess($cur->getFieldLength(7,0),1);
checkSuccess($cur->getFieldLength(7,1),1);
checkSuccess($cur->getFieldLength(7,2),1);
checkSuccess($cur->getFieldLength(7,3),4);
#checkSuccess($cur->getFieldLength(7,4),3);
#checkSuccess($cur->getFieldLength(7,5),3);
checkSuccess($cur->getFieldLength(7,6),40);
checkSuccess($cur->getFieldLength(7,7),12);
checkSuccess($cur->getFieldLength(7,8),10);
checkSuccess($cur->getFieldLength(7,9),8);
print("\n");

print("FIELDS BY NAME: \n");
checkSuccessString($cur->getField(0,"TESTSMALLINT"),"1");
checkSuccessString($cur->getField(0,"TESTINT"),"1");
checkSuccessString($cur->getField(0,"TESTBIGINT"),"1");
checkSuccessString($cur->getField(0,"TESTDECIMAL"),"1.10");
#checkSuccessString($cur->getField(0,"TESTREAL"),"1.1");
#checkSuccessString($cur->getField(0,"TESTDOUBLE"),"1.1");
checkSuccessString($cur->getField(0,"TESTCHAR"),"testchar1                               ");
checkSuccessString($cur->getField(0,"TESTVARCHAR"),"testvarchar1");
checkSuccessString($cur->getField(0,"TESTDATE"),"2001-01-01");
checkSuccessString($cur->getField(0,"TESTTIME"),"01:00:00");
print("\n");
checkSuccessString($cur->getField(7,"TESTSMALLINT"),"8");
checkSuccessString($cur->getField(7,"TESTINT"),"8");
checkSuccessString($cur->getField(7,"TESTBIGINT"),"8");
checkSuccessString($cur->getField(7,"TESTDECIMAL"),"8.80");
#checkSuccessString($cur->getField(7,"TESTREAL"),"8.8");
#checkSuccessString($cur->getField(7,"TESTDOUBLE"),"8.8");
checkSuccessString($cur->getField(7,"TESTCHAR"),"testchar8                               ");
checkSuccessString($cur->getField(7,"TESTVARCHAR"),"testvarchar8");
checkSuccessString($cur->getField(7,"TESTDATE"),"2008-01-01");
checkSuccessString($cur->getField(7,"TESTTIME"),"08:00:00");
print("\n");

print("FIELD LENGTHS BY NAME: \n");
checkSuccess($cur->getFieldLength(0,"TESTSMALLINT"),1);
checkSuccess($cur->getFieldLength(0,"TESTINT"),1);
checkSuccess($cur->getFieldLength(0,"TESTBIGINT"),1);
checkSuccess($cur->getFieldLength(0,"TESTDECIMAL"),4);
#checkSuccess($cur->getFieldLength(0,"TESTREAL"),3);
#checkSuccess($cur->getFieldLength(0,"TESTDOUBLE"),3);
checkSuccess($cur->getFieldLength(0,"TESTCHAR"),40);
checkSuccess($cur->getFieldLength(0,"TESTVARCHAR"),12);
checkSuccess($cur->getFieldLength(0,"TESTDATE"),10);
checkSuccess($cur->getFieldLength(0,"TESTTIME"),8);
print("\n");
checkSuccess($cur->getFieldLength(7,"TESTSMALLINT"),1);
checkSuccess($cur->getFieldLength(7,"TESTINT"),1);
checkSuccess($cur->getFieldLength(7,"TESTBIGINT"),1);
checkSuccess($cur->getFieldLength(7,"TESTDECIMAL"),4);
#checkSuccess($cur->getFieldLength(7,"TESTREAL"),3);
#checkSuccess($cur->getFieldLength(7,"TESTDOUBLE"),3);
checkSuccess($cur->getFieldLength(7,"TESTCHAR"),40);
checkSuccess($cur->getFieldLength(7,"TESTVARCHAR"),12);
checkSuccess($cur->getFieldLength(7,"TESTDATE"),10);
checkSuccess($cur->getFieldLength(7,"TESTTIME"),8);
print("\n");

print("FIELDS BY ARRAY: \n");
@fields=$cur->getRow(0);
checkSuccess($fields[0],1);
checkSuccess($fields[1],1);
checkSuccess($fields[2],1);
checkSuccess($fields[3],1.1);
checkSuccess($fields[4],1.1);
checkSuccess($fields[5],1.1);
checkSuccessString($fields[6],"testchar1                               ");
checkSuccessString($fields[7],"testvarchar1");
checkSuccessString($fields[8],"2001-01-01");
checkSuccessString($fields[9],"01:00:00");
print("\n");

print("FIELD LENGTHS BY ARRAY: \n");
@fieldlens=$cur->getRowLengths(0);
checkSuccess($fieldlens[0],1);
checkSuccess($fieldlens[1],1);
checkSuccess($fieldlens[2],1);
checkSuccess($fieldlens[3],4);
#checkSuccess($fieldlens[4],3);
#checkSuccess($fieldlens[5],3);
checkSuccess($fieldlens[6],40);
checkSuccess($fieldlens[7],12);
checkSuccess($fieldlens[8],10);
checkSuccess($fieldlens[9],8);
print("\n");

print("FIELDS BY HASH: \n");
%fields=$cur->getRowHash(0);
checkSuccess($fields{"TESTSMALLINT"},1);
checkSuccess($fields{"TESTINT"},1);
checkSuccess($fields{"TESTBIGINT"},1);
checkSuccess($fields{"TESTDECIMAL"},1.1);
#checkSuccess($fields{"TESTREAL"},1.1);
#checkSuccess($fields{"TESTDOUBLE"},1.1);
checkSuccessString($fields{"TESTCHAR"},"testchar1                               ");
checkSuccessString($fields{"TESTVARCHAR"},"testvarchar1");
checkSuccessString($fields{"TESTDATE"},"2001-01-01");
checkSuccessString($fields{"TESTTIME"},"01:00:00");
print("\n");
%fields=$cur->getRowHash(7);
checkSuccess($fields{"TESTSMALLINT"},8);
checkSuccess($fields{"TESTINT"},8);
checkSuccess($fields{"TESTBIGINT"},8);
checkSuccess($fields{"TESTDECIMAL"},8.8);
#checkSuccess($fields{"TESTREAL"},8.8);
#checkSuccess($fields{"TESTDOUBLE"},8.8);
checkSuccessString($fields{"TESTCHAR"},"testchar8                               ");
checkSuccessString($fields{"TESTVARCHAR"},"testvarchar8");
checkSuccessString($fields{"TESTDATE"},"2008-01-01");
checkSuccessString($fields{"TESTTIME"},"08:00:00");
print("\n");

print("FIELD LENGTHS BY HASH: \n");
%fieldlengths=$cur->getRowLengthsHash(0);
checkSuccess($fieldlengths{"TESTSMALLINT"},1);
checkSuccess($fieldlengths{"TESTINT"},1);
checkSuccess($fieldlengths{"TESTBIGINT"},1);
checkSuccess($fieldlengths{"TESTDECIMAL"},4);
#checkSuccess($fieldlengths{"TESTREAL"},3);
#checkSuccess($fieldlengths{"TESTDOUBLE"},1);
checkSuccess($fieldlengths{"TESTCHAR"},40);
checkSuccess($fieldlengths{"TESTVARCHAR"},12);
checkSuccess($fieldlengths{"TESTDATE"},10);
checkSuccess($fieldlengths{"TESTTIME"},8);
print("\n");
%fieldlengths=$cur->getRowLengthsHash(7);
checkSuccess($fieldlengths{"TESTSMALLINT"},1);
checkSuccess($fieldlengths{"TESTINT"},1);
checkSuccess($fieldlengths{"TESTBIGINT"},1);
checkSuccess($fieldlengths{"TESTDECIMAL"},4);
#checkSuccess($fieldlengths{"TESTREAL"},3);
#checkSuccess($fieldlengths{"TESTDOUBLE"},1);
checkSuccess($fieldlengths{"TESTCHAR"},40);
checkSuccess($fieldlengths{"TESTVARCHAR"},12);
checkSuccess($fieldlengths{"TESTDATE"},10);
checkSuccess($fieldlengths{"TESTTIME"},8);
print("\n");

print("INDIVIDUAL SUBSTITUTIONS: \n");
$cur->prepareQuery("values (\$(var1),'\$(var2)','\$(var3)')");
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
$cur->prepareQuery("values (\$(var1),'\$(var2)','\$(var3)')");
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
$cur->sendQuery("drop table testtable1");
checkSuccess($cur->sendQuery("create table testtable1 (col1 char(1), col2 char(1), col3 char(1))"),1);
checkSuccess($cur->sendQuery("insert into testtable1 values ('1',NULL,NULL)"),1);
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
$cur->getNullsAsUndefined();
print("\n");

print("RESULT SET BUFFER SIZE: \n");
checkSuccess($cur->getResultSetBufferSize(),0);
$cur->setResultSetBufferSize(2);
checkSuccess($cur->sendQuery("select * from testtable order by testsmallint"),1);
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
checkSuccess($cur->sendQuery("select * from testtable order by testsmallint"),1);
checkUndef($cur->getColumnName(0));
checkSuccess($cur->getColumnLength(0),0);
checkUndef($cur->getColumnType(0));
$cur->getColumnInfo();
checkSuccess($cur->sendQuery("select * from testtable order by testsmallint"),1);
checkSuccessString($cur->getColumnName(0),"TESTSMALLINT");
checkSuccess($cur->getColumnLength(0),2);
checkSuccessString($cur->getColumnType(0),"SMALLINT");
print("\n");

print("SUSPENDED SESSION: \n");
checkSuccess($cur->sendQuery("select * from testtable order by testsmallint"),1);
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
checkSuccess($cur->sendQuery("select * from testtable order by testsmallint"),1);
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
checkSuccess($cur->sendQuery("select * from testtable order by testsmallint"),1);
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
checkSuccess($cur->sendQuery("select * from testtable order by testsmallint"),1);
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
checkSuccess($cur->sendQuery("select * from testtable order by testsmallint"),1);
$filename=$cur->getCacheFileName();
checkSuccessString($filename,"cachefile1");
$cur->cacheOff();
checkSuccess($cur->openCachedResultSet($filename),1);
checkSuccessString($cur->getField(7,0),"8");
print("\n");

print("COLUMN COUNT FOR CACHED RESULT SET: \n");
checkSuccess($cur->colCount(),11);
print("\n");

print("COLUMN NAMES FOR CACHED RESULT SET: \n");
checkSuccessString($cur->getColumnName(0),"TESTSMALLINT");
checkSuccessString($cur->getColumnName(1),"TESTINT");
checkSuccessString($cur->getColumnName(2),"TESTBIGINT");
checkSuccessString($cur->getColumnName(3),"TESTDECIMAL");
checkSuccessString($cur->getColumnName(4),"TESTREAL");
checkSuccessString($cur->getColumnName(5),"TESTDOUBLE");
checkSuccessString($cur->getColumnName(6),"TESTCHAR");
checkSuccessString($cur->getColumnName(7),"TESTVARCHAR");
checkSuccessString($cur->getColumnName(8),"TESTDATE");
checkSuccessString($cur->getColumnName(9),"TESTTIME");
checkSuccessString($cur->getColumnName(10),"TESTTIMESTAMP");
@cols=$cur->getColumnNames();
checkSuccessString($cols[0],"TESTSMALLINT");
checkSuccessString($cols[1],"TESTINT");
checkSuccessString($cols[2],"TESTBIGINT");
checkSuccessString($cols[3],"TESTDECIMAL");
checkSuccessString($cols[4],"TESTREAL");
checkSuccessString($cols[5],"TESTDOUBLE");
checkSuccessString($cols[6],"TESTCHAR");
checkSuccessString($cols[7],"TESTVARCHAR");
checkSuccessString($cols[8],"TESTDATE");
checkSuccessString($cols[9],"TESTTIME");
checkSuccessString($cols[10],"TESTTIMESTAMP");
print("\n");

print("CACHED RESULT SET WITH RESULT SET BUFFER SIZE: \n");
$cur->setResultSetBufferSize(2);
$cur->cacheToFile("cachefile1");
$cur->setCacheTtl(200);
checkSuccess($cur->sendQuery("select * from testtable order by testsmallint"),1);
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
checkSuccess($cur->sendQuery("select * from testtable order by testsmallint"),1);
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
$con->commit();
$cur->sendQuery("drop table testtable");
print("\n");

# invalid queries...
print("INVALID QUERIES: \n");
checkSuccess($cur->sendQuery("select * from testtable order by testsmallint"),0);
checkSuccess($cur->sendQuery("select * from testtable order by testsmallint"),0);
checkSuccess($cur->sendQuery("select * from testtable order by testsmallint"),0);
checkSuccess($cur->sendQuery("select * from testtable order by testsmallint"),0);
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



