#! /usr/bin/env python

# Copyright (c) 2001  David Muse
# See the file COPYING for more information.

from SQLRelay import PySQLRClient
from decimal import *
import sys
import string

def checkSuccess(value,success):
	if value==success:
		print "success",
	else:
		print "wanted", type(success), ":", success
		print "got   ", type(value), ":", value
		print "failure"
		sys.exit(0)

def main():

	PySQLRClient.getNumericFieldsAsNumbers()

	# instantiation
	con=PySQLRClient.sqlrconnection("sqlrserver",9000,"/tmp/test.socket",
								"test","test")
	cur=PySQLRClient.sqlrcursor(con)

	# get database type
	print "IDENTIFY: "
	checkSuccess(con.identify(),"db2")
	print

	# ping
	print "PING: "
	checkSuccess(con.ping(),1)
	print

	# drop existing table
	cur.sendQuery("drop table testtable")

	print "CREATE TEMPTABLE: "
	checkSuccess(cur.sendQuery("create table testtable (testsmallint smallint, testint integer, testbigint bigint, testdecimal decimal(10,2), testreal real, testdouble double, testchar char(40), testvarchar varchar(40), testdate date, testtime time, testtimestamp timestamp)"),1)
	print

	print "INSERT: "
	checkSuccess(cur.sendQuery("insert into testtable values (1,1,1,1.1,1.1,1.1,'testchar1','testvarchar1','01/01/2001','01:00:00',NULL)"),1)
	print

	print "BIND BY POSITION: "
	cur.prepareQuery("insert into testtable values (?,?,?,?,?,?,?,?,?,?,NULL)")
	checkSuccess(cur.countBindVariables(),10)
	cur.inputBind("1",2)
	cur.inputBind("2",2)
	cur.inputBind("3",2)
	cur.inputBind("4",2.2,4,2)
	cur.inputBind("5",2.2,4,2)
	cur.inputBind("6",2.2,4,2)
	cur.inputBind("7","testchar2")
	cur.inputBind("8","testvarchar2")
	cur.inputBind("9","01/01/2002")
	cur.inputBind("10","02:00:00")
	checkSuccess(cur.executeQuery(),1)
	cur.clearBinds()
	cur.inputBind("1",3)
	cur.inputBind("2",3)
	cur.inputBind("3",3)
	cur.inputBind("4",3.3,4,2)
	cur.inputBind("5",3.3,4,2)
	cur.inputBind("6",3.3,4,2)
	cur.inputBind("7","testchar3")
	cur.inputBind("8","testvarchar3")
	cur.inputBind("9","01/01/2003")
	cur.inputBind("10","03:00:00")
	checkSuccess(cur.executeQuery(),1)
	print

	print "ARRAY OF BINDS BY POSITION: "
	cur.clearBinds()
	cur.inputBinds(["1","2","3","4","5","6","7","8","9","10"],
		[4,4,4,4.4,4.4,4.4,"testchar4","testvarchar4",
			"01/01/2004","04:00:00"],
		[0,0,0,4,4,4,0,0,0,0],
		[0,0,0,2,2,2,0,0,0,0])
	checkSuccess(cur.executeQuery(),1)
	print

	print "INSERT: "
	checkSuccess(cur.sendQuery("insert into testtable values (5,5,5,5.5,5.5,5.5,'testchar5','testvarchar5','01/01/2005','05:00:00',NULL)"),1)
	checkSuccess(cur.sendQuery("insert into testtable values (6,6,6,6.6,6.6,6.6,'testchar6','testvarchar6','01/01/2006','06:00:00',NULL)"),1)
	checkSuccess(cur.sendQuery("insert into testtable values (7,7,7,7.7,7.7,7.7,'testchar7','testvarchar7','01/01/2007','07:00:00',NULL)"),1)
	checkSuccess(cur.sendQuery("insert into testtable values (8,8,8,8.8,8.8,8.8,'testchar8','testvarchar8','01/01/2008','08:00:00',NULL)"),1)
	print

	print "AFFECTED ROWS: "
	checkSuccess(cur.affectedRows(),1)
	print

	print "STORED PROCEDURE: "
	cur.sendQuery("drop procedure testproc");
	checkSuccess(cur.sendQuery("create procedure testproc(in invar int, out outvar int) language sql begin set outvar = invar; end"),1)
	cur.prepareQuery("call testproc(?,?)")
	cur.inputBind("1",5)
	cur.defineOutputBindString("2",10)
	checkSuccess(cur.executeQuery(),1)
	checkSuccess(cur.getOutputBindString("2"),"5")
	checkSuccess(cur.sendQuery("drop procedure testproc"),1)
	print


	print "SELECT: "
	checkSuccess(cur.sendQuery("select * from testtable order by testsmallint"),1)
	print

	print "COLUMN COUNT: "
	checkSuccess(cur.colCount(),11)
	print

	print "COLUMN NAMES: "
	checkSuccess(cur.getColumnName(0),"TESTSMALLINT")
	checkSuccess(cur.getColumnName(1),"TESTINT")
	checkSuccess(cur.getColumnName(2),"TESTBIGINT")
	checkSuccess(cur.getColumnName(3),"TESTDECIMAL")
	checkSuccess(cur.getColumnName(4),"TESTREAL")
	checkSuccess(cur.getColumnName(5),"TESTDOUBLE")
	checkSuccess(cur.getColumnName(6),"TESTCHAR")
	checkSuccess(cur.getColumnName(7),"TESTVARCHAR")
	checkSuccess(cur.getColumnName(8),"TESTDATE")
	checkSuccess(cur.getColumnName(9),"TESTTIME")
	checkSuccess(cur.getColumnName(10),"TESTTIMESTAMP")
	cols=cur.getColumnNames()
	checkSuccess(cols[0],"TESTSMALLINT")
	checkSuccess(cols[1],"TESTINT")
	checkSuccess(cols[2],"TESTBIGINT")
	checkSuccess(cols[3],"TESTDECIMAL")
	checkSuccess(cols[4],"TESTREAL")
	checkSuccess(cols[5],"TESTDOUBLE")
	checkSuccess(cols[6],"TESTCHAR")
	checkSuccess(cols[7],"TESTVARCHAR")
	checkSuccess(cols[8],"TESTDATE")
	checkSuccess(cols[9],"TESTTIME")
	checkSuccess(cols[10],"TESTTIMESTAMP")
	print

	print "COLUMN TYPES: "
	checkSuccess(cur.getColumnType(0),"SMALLINT")
	checkSuccess(cur.getColumnType('TESTSMALLINT'),"SMALLINT")
	checkSuccess(cur.getColumnType(1),"INTEGER")
	checkSuccess(cur.getColumnType('TESTINT'),"INTEGER")
	checkSuccess(cur.getColumnType(2),"BIGINT")
	checkSuccess(cur.getColumnType('TESTBIGINT'),"BIGINT")
	checkSuccess(cur.getColumnType(3),"DECIMAL")
	checkSuccess(cur.getColumnType('TESTDECIMAL'),"DECIMAL")
	checkSuccess(cur.getColumnType(4),"REAL")
	checkSuccess(cur.getColumnType('TESTREAL'),"REAL")
	checkSuccess(cur.getColumnType(5),"DOUBLE")
	checkSuccess(cur.getColumnType('TESTDOUBLE'),"DOUBLE")
	checkSuccess(cur.getColumnType(6),"CHAR")
	checkSuccess(cur.getColumnType('TESTCHAR'),"CHAR")
	checkSuccess(cur.getColumnType(7),"VARCHAR")
	checkSuccess(cur.getColumnType('TESTVARCHAR'),"VARCHAR")
	checkSuccess(cur.getColumnType(8),"DATE")
	checkSuccess(cur.getColumnType('TESTDATE'),"DATE")
	checkSuccess(cur.getColumnType(9),"TIME")
	checkSuccess(cur.getColumnType('TESTTIME'),"TIME")
	checkSuccess(cur.getColumnType(10),"TIMESTAMP")
	checkSuccess(cur.getColumnType('TESTTIMESTAMP'),"TIMESTAMP")
	print

	print "COLUMN LENGTH: "
	checkSuccess(cur.getColumnLength(0),2)
	checkSuccess(cur.getColumnLength('TESTSMALLINT'),2)
	checkSuccess(cur.getColumnLength(1),4)
	checkSuccess(cur.getColumnLength('TESTINT'),4)
	checkSuccess(cur.getColumnLength(2),8)
	checkSuccess(cur.getColumnLength('TESTBIGINT'),8)
	checkSuccess(cur.getColumnLength(3),12)
	checkSuccess(cur.getColumnLength('TESTDECIMAL'),12)
	checkSuccess(cur.getColumnLength(4),4)
	checkSuccess(cur.getColumnLength('TESTREAL'),4)
	checkSuccess(cur.getColumnLength(5),8)
	checkSuccess(cur.getColumnLength('TESTDOUBLE'),8)
	checkSuccess(cur.getColumnLength(6),40)
	checkSuccess(cur.getColumnLength('TESTCHAR'),40)
	checkSuccess(cur.getColumnLength(7),40)
	checkSuccess(cur.getColumnLength('TESTVARCHAR'),40)
	checkSuccess(cur.getColumnLength(8),6)
	checkSuccess(cur.getColumnLength('TESTDATE'),6)
	checkSuccess(cur.getColumnLength(9),6)
	checkSuccess(cur.getColumnLength('TESTTIME'),6)
	checkSuccess(cur.getColumnLength(10),16)
	checkSuccess(cur.getColumnLength('TESTTIMESTAMP'),16)
	print

	print "LONGEST COLUMN: "
	checkSuccess(cur.getLongest(0),1)
	checkSuccess(cur.getLongest('TESTSMALLINT'),1)
	checkSuccess(cur.getLongest(1),1)
	checkSuccess(cur.getLongest('TESTINT'),1)
	checkSuccess(cur.getLongest(2),1)
	checkSuccess(cur.getLongest('TESTBIGINT'),1)
	checkSuccess(cur.getLongest(3),4)
	checkSuccess(cur.getLongest('TESTDECIMAL'),4)
	#checkSuccess(cur.getLongest(4),3)
	#checkSuccess(cur.getLongest('TESTREAL'),3)
	#checkSuccess(cur.getLongest(5),3)
	#checkSuccess(cur.getLongest('TESTDOUBLE'),3)
	checkSuccess(cur.getLongest(6),40)
	checkSuccess(cur.getLongest('TESTCHAR'),40)
	checkSuccess(cur.getLongest(7),12)
	checkSuccess(cur.getLongest('TESTVARCHAR'),12)
	checkSuccess(cur.getLongest(8),10)
	checkSuccess(cur.getLongest('TESTDATE'),10)
	checkSuccess(cur.getLongest(9),8)
	checkSuccess(cur.getLongest('TESTTIME'),8)
	print

	print "ROW COUNT: "
	checkSuccess(cur.rowCount(),8)
	print

	print "TOTAL ROWS: "
	checkSuccess(cur.totalRows(),0)
	print

	print "FIRST ROW INDEX: "
	checkSuccess(cur.firstRowIndex(),0)
	print

	print "END OF RESULT SET: "
	checkSuccess(cur.endOfResultSet(),1)
	print

	print "FIELDS BY INDEX: "
	checkSuccess(cur.getField(0,0),1)
	checkSuccess(cur.getField(0,1),1)
	checkSuccess(cur.getField(0,2),1)
	checkSuccess(cur.getField(0,3),Decimal("1.10"))
	#checkSuccess(cur.getField(0,4),Decimal("1.1"))
	#checkSuccess(cur.getField(0,5),Decimal("1.1"))
	checkSuccess(cur.getField(0,6),"testchar1                               ")
	checkSuccess(cur.getField(0,7),"testvarchar1")
	checkSuccess(cur.getField(0,8),"2001-01-01")
	checkSuccess(cur.getField(0,9),"01:00:00")
	print
	checkSuccess(cur.getField(7,0),8)
	checkSuccess(cur.getField(7,1),8)
	checkSuccess(cur.getField(7,2),8)
	checkSuccess(cur.getField(7,3),Decimal("8.80"))
	#checkSuccess(cur.getField(7,4),Decimal("8.8"))
	#checkSuccess(cur.getField(7,5),Decimal("8.8"))
	checkSuccess(cur.getField(7,6),"testchar8                               ")
	checkSuccess(cur.getField(7,7),"testvarchar8")
	checkSuccess(cur.getField(7,8),"2008-01-01")
	checkSuccess(cur.getField(7,9),"08:00:00")
	print

	print "FIELD LENGTHS BY INDEX: "
	checkSuccess(cur.getFieldLength(0,0),1)
	checkSuccess(cur.getFieldLength(0,1),1)
	checkSuccess(cur.getFieldLength(0,2),1)
	checkSuccess(cur.getFieldLength(0,3),4)
	#checkSuccess(cur.getFieldLength(0,4),3)
	#checkSuccess(cur.getFieldLength(0,5),3)
	checkSuccess(cur.getFieldLength(0,6),40)
	checkSuccess(cur.getFieldLength(0,7),12)
	checkSuccess(cur.getFieldLength(0,8),10)
	checkSuccess(cur.getFieldLength(0,9),8)
	print
	checkSuccess(cur.getFieldLength(7,0),1)
	checkSuccess(cur.getFieldLength(7,1),1)
	checkSuccess(cur.getFieldLength(7,2),1)
	checkSuccess(cur.getFieldLength(7,3),4)
	#checkSuccess(cur.getFieldLength(7,4),3)
	#checkSuccess(cur.getFieldLength(7,5),3)
	checkSuccess(cur.getFieldLength(7,6),40)
	checkSuccess(cur.getFieldLength(7,7),12)
	checkSuccess(cur.getFieldLength(7,8),10)
	checkSuccess(cur.getFieldLength(7,9),8)
	print

	print "FIELDS BY NAME: "
	checkSuccess(cur.getField(0,"testsmallint"),1)
	checkSuccess(cur.getField(0,"testint"),1)
	checkSuccess(cur.getField(0,"testbigint"),1)
	checkSuccess(cur.getField(0,"testdecimal"),Decimal("1.10"))
	#checkSuccess(cur.getField(0,"testreal"),Decimal("1.1"))
	#checkSuccess(cur.getField(0,"testdouble"),Decimal("1.1"))
	checkSuccess(cur.getField(0,"testchar"),"testchar1                               ")
	checkSuccess(cur.getField(0,"testvarchar"),"testvarchar1")
	checkSuccess(cur.getField(0,"testdate"),"2001-01-01")
	checkSuccess(cur.getField(0,"testtime"),"01:00:00")
	print
	checkSuccess(cur.getField(7,"testsmallint"),8)
	checkSuccess(cur.getField(7,"testint"),8)
	checkSuccess(cur.getField(7,"testbigint"),8)
	checkSuccess(cur.getField(7,"testdecimal"),Decimal("8.80"))
	#checkSuccess(cur.getField(7,"testreal"),Decimal("8.8"))
	#checkSuccess(cur.getField(7,"testdouble"),Decimal("8.8"))
	checkSuccess(cur.getField(7,"testchar"),"testchar8                               ")
	checkSuccess(cur.getField(7,"testvarchar"),"testvarchar8")
	checkSuccess(cur.getField(7,"testdate"),"2008-01-01")
	checkSuccess(cur.getField(7,"testtime"),"08:00:00")
	print

	print "FIELD LENGTHS BY NAME: "
	checkSuccess(cur.getFieldLength(0,"testsmallint"),1)
	checkSuccess(cur.getFieldLength(0,"testint"),1)
	checkSuccess(cur.getFieldLength(0,"testbigint"),1)
	checkSuccess(cur.getFieldLength(0,"testdecimal"),4)
	#checkSuccess(cur.getFieldLength(0,"testreal"),3)
	#checkSuccess(cur.getFieldLength(0,"testdouble"),3)
	checkSuccess(cur.getFieldLength(0,"testchar"),40)
	checkSuccess(cur.getFieldLength(0,"testvarchar"),12)
	checkSuccess(cur.getFieldLength(0,"testdate"),10)
	checkSuccess(cur.getFieldLength(0,"testtime"),8)
	print
	checkSuccess(cur.getFieldLength(7,"testsmallint"),1)
	checkSuccess(cur.getFieldLength(7,"testint"),1)
	checkSuccess(cur.getFieldLength(7,"testbigint"),1)
	checkSuccess(cur.getFieldLength(7,"testdecimal"),4)
	#checkSuccess(cur.getFieldLength(7,"testreal"),3)
	#checkSuccess(cur.getFieldLength(7,"testdouble"),3)
	checkSuccess(cur.getFieldLength(7,"testchar"),40)
	checkSuccess(cur.getFieldLength(7,"testvarchar"),12)
	checkSuccess(cur.getFieldLength(7,"testdate"),10)
	checkSuccess(cur.getFieldLength(7,"testtime"),8)
	print

	print "FIELDS BY ARRAY: "
	fields=cur.getRow(0)
	checkSuccess(fields[0],1)
	checkSuccess(fields[1],1)
	checkSuccess(fields[2],1)
	checkSuccess(fields[3],Decimal("1.1"))
	checkSuccess(fields[4],Decimal("1.1"))
	checkSuccess(fields[5],Decimal("1.1"))
	checkSuccess(fields[6],"testchar1                               ")
	checkSuccess(fields[7],"testvarchar1")
	checkSuccess(fields[8],"2001-01-01")
	checkSuccess(fields[9],"01:00:00")
	print

	print "FIELD LENGTHS BY ARRAY: "
	fieldlens=cur.getRowLengths(0)
	checkSuccess(fieldlens[0],1)
	checkSuccess(fieldlens[1],1)
	checkSuccess(fieldlens[2],1)
	checkSuccess(fieldlens[3],4)
	#checkSuccess(fieldlens[4],3)
	#checkSuccess(fieldlens[5],3)
	checkSuccess(fieldlens[6],40)
	checkSuccess(fieldlens[7],12)
	checkSuccess(fieldlens[8],10)
	checkSuccess(fieldlens[9],8)
	print

	print "FIELDS BY DICTIONARY: "
	fields=cur.getRowDictionary(0)
	checkSuccess(fields["TESTSMALLINT"],1)
	checkSuccess(fields["TESTINT"],1)
	checkSuccess(fields["TESTBIGINT"],1)
	checkSuccess(fields["TESTDECIMAL"],Decimal("1.1"))
	#checkSuccess(fields["TESTREAL"],Decimal("1.1"))
	#checkSuccess(fields["TESTDOUBLE"],Decimal("1.1"))
	checkSuccess(fields["TESTCHAR"],"testchar1                               ")
	checkSuccess(fields["TESTVARCHAR"],"testvarchar1")
	checkSuccess(fields["TESTDATE"],"2001-01-01")
	checkSuccess(fields["TESTTIME"],"01:00:00")
	print
	fields=cur.getRowDictionary(7)
	checkSuccess(fields["TESTSMALLINT"],8)
	checkSuccess(fields["TESTINT"],8)
	checkSuccess(fields["TESTBIGINT"],8)
	checkSuccess(fields["TESTDECIMAL"],Decimal("8.8"))
	#checkSuccess(fields["TESTREAL"],Decimal("8.8"))
	#checkSuccess(fields["TESTDOUBLE"],Decimal("8.8"))
	checkSuccess(fields["TESTCHAR"],"testchar8                               ")
	checkSuccess(fields["TESTVARCHAR"],"testvarchar8")
	checkSuccess(fields["TESTDATE"],"2008-01-01")
	checkSuccess(fields["TESTTIME"],"08:00:00")
	print

	print "FIELD LENGTHS BY DICTIONARY: "
	fieldlengths=cur.getRowLengthsDictionary(0)
	checkSuccess(fieldlengths["TESTSMALLINT"],1)
	checkSuccess(fieldlengths["TESTINT"],1)
	checkSuccess(fieldlengths["TESTBIGINT"],1)
	checkSuccess(fieldlengths["TESTDECIMAL"],4)
	#checkSuccess(fieldlengths["TESTREAL"],3)
	#checkSuccess(fieldlengths["TESTDOUBLE"],1)
	checkSuccess(fieldlengths["TESTCHAR"],40)
	checkSuccess(fieldlengths["TESTVARCHAR"],12)
	checkSuccess(fieldlengths["TESTDATE"],10)
	checkSuccess(fieldlengths["TESTTIME"],8)
	print
	fieldlengths=cur.getRowLengthsDictionary(7)
	checkSuccess(fieldlengths["TESTSMALLINT"],1)
	checkSuccess(fieldlengths["TESTINT"],1)
	checkSuccess(fieldlengths["TESTBIGINT"],1)
	checkSuccess(fieldlengths["TESTDECIMAL"],4)
	#checkSuccess(fieldlengths["TESTREAL"],3)
	#checkSuccess(fieldlengths["TESTDOUBLE"],1)
	checkSuccess(fieldlengths["TESTCHAR"],40)
	checkSuccess(fieldlengths["TESTVARCHAR"],12)
	checkSuccess(fieldlengths["TESTDATE"],10)
	checkSuccess(fieldlengths["TESTTIME"],8)
	print

	print "INDIVIDUAL SUBSTITUTIONS: "
	cur.prepareQuery("values ($(var1),'$(var2)','$(var3)')")
	cur.substitution("var1",1)
	cur.substitution("var2","hello")
	cur.substitution("var3",10.5556,6,4)
	checkSuccess(cur.executeQuery(),1)
	print

	print "FIELDS: "
	checkSuccess(cur.getField(0,0),1)
	checkSuccess(cur.getField(0,1),"hello")
	checkSuccess(cur.getField(0,2),"10.5556")
	print

	print "ARRAY SUBSTITUTIONS: "
	cur.prepareQuery("values ($(var1),'$(var2)','$(var3)')")
	cur.substitutions(["var1","var2","var3"],
				[1,"hello",10.5556],[0,0,6],[0,0,4])
	checkSuccess(cur.executeQuery(),1)
	print

	print "FIELDS: "
	checkSuccess(cur.getField(0,0),1)
	checkSuccess(cur.getField(0,1),"hello")
	checkSuccess(cur.getField(0,2),"10.5556")
	print

	print "NULLS as Nones: "
	cur.getNullsAsNone()
	cur.sendQuery("drop table testtable1")
	checkSuccess(cur.sendQuery("create table testtable1 (col1 char(1), col2 char(1), col3 char(1))"),1)
	checkSuccess(cur.sendQuery("insert into testtable1 values ('1',NULL,NULL)"),1)
	checkSuccess(cur.sendQuery("select * from testtable1"),1)
	checkSuccess(cur.getField(0,0),"1")
	checkSuccess(cur.getField(0,1),None)
	checkSuccess(cur.getField(0,2),None)
	cur.getNullsAsEmptyStrings()
	checkSuccess(cur.sendQuery("select * from testtable1"),1)
	checkSuccess(cur.getField(0,0),"1")
	checkSuccess(cur.getField(0,1),"")
	checkSuccess(cur.getField(0,2),"")
	checkSuccess(cur.sendQuery("drop table testtable1"),1)
	cur.getNullsAsNone()
	print
	
	print "RESULT SET BUFFER SIZE: "
	checkSuccess(cur.getResultSetBufferSize(),0)
	cur.setResultSetBufferSize(2)
	checkSuccess(cur.sendQuery("select * from testtable order by testsmallint"),1)
	checkSuccess(cur.getResultSetBufferSize(),2)
	print
	checkSuccess(cur.firstRowIndex(),0)
	checkSuccess(cur.endOfResultSet(),0)
	checkSuccess(cur.rowCount(),2)
	checkSuccess(cur.getField(0,0),1)
	checkSuccess(cur.getField(1,0),2)
	checkSuccess(cur.getField(2,0),3)
	print
	checkSuccess(cur.firstRowIndex(),2)
	checkSuccess(cur.endOfResultSet(),0)
	checkSuccess(cur.rowCount(),4)
	checkSuccess(cur.getField(6,0),7)
	checkSuccess(cur.getField(7,0),8)
	print
	checkSuccess(cur.firstRowIndex(),6)
	checkSuccess(cur.endOfResultSet(),0)
	checkSuccess(cur.rowCount(),8)
	checkSuccess(cur.getField(8,0),None)
	print
	checkSuccess(cur.firstRowIndex(),8)
	checkSuccess(cur.endOfResultSet(),1)
	checkSuccess(cur.rowCount(),8)
	print

	print "DONT GET COLUMN INFO: "
	cur.dontGetColumnInfo()
	checkSuccess(cur.sendQuery("select * from testtable order by testsmallint"),1)
	checkSuccess(cur.getColumnName(0),None)
	checkSuccess(cur.getColumnLength(0),0)
	checkSuccess(cur.getColumnType(0),None)
	cur.getColumnInfo()
	checkSuccess(cur.sendQuery("select * from testtable order by testsmallint"),1)
	checkSuccess(cur.getColumnName(0),"TESTSMALLINT")
	checkSuccess(cur.getColumnLength(0),2)
	checkSuccess(cur.getColumnType(0),"SMALLINT")
	print

	print "SUSPENDED SESSION: "
	checkSuccess(cur.sendQuery("select * from testtable order by testsmallint"),1)
	cur.suspendResultSet()
	checkSuccess(con.suspendSession(),1)
	port=con.getConnectionPort()
	socket=con.getConnectionSocket()
	checkSuccess(con.resumeSession(port,socket),1)
	print
	checkSuccess(cur.getField(0,0),1)
	checkSuccess(cur.getField(1,0),2)
	checkSuccess(cur.getField(2,0),3)
	checkSuccess(cur.getField(3,0),4)
	checkSuccess(cur.getField(4,0),5)
	checkSuccess(cur.getField(5,0),6)
	checkSuccess(cur.getField(6,0),7)
	checkSuccess(cur.getField(7,0),8)
	print
	checkSuccess(cur.sendQuery("select * from testtable order by testsmallint"),1)
	cur.suspendResultSet()
	checkSuccess(con.suspendSession(),1)
	port=con.getConnectionPort()
	socket=con.getConnectionSocket()
	checkSuccess(con.resumeSession(port,socket),1)
	print
	checkSuccess(cur.getField(0,0),1)
	checkSuccess(cur.getField(1,0),2)
	checkSuccess(cur.getField(2,0),3)
	checkSuccess(cur.getField(3,0),4)
	checkSuccess(cur.getField(4,0),5)
	checkSuccess(cur.getField(5,0),6)
	checkSuccess(cur.getField(6,0),7)
	checkSuccess(cur.getField(7,0),8)
	print
	checkSuccess(cur.sendQuery("select * from testtable order by testsmallint"),1)
	cur.suspendResultSet()
	checkSuccess(con.suspendSession(),1)
	port=con.getConnectionPort()
	socket=con.getConnectionSocket()
	checkSuccess(con.resumeSession(port,socket),1)
	print
	checkSuccess(cur.getField(0,0),1)
	checkSuccess(cur.getField(1,0),2)
	checkSuccess(cur.getField(2,0),3)
	checkSuccess(cur.getField(3,0),4)
	checkSuccess(cur.getField(4,0),5)
	checkSuccess(cur.getField(5,0),6)
	checkSuccess(cur.getField(6,0),7)
	checkSuccess(cur.getField(7,0),8)
	print

	print "SUSPENDED RESULT SET: "
	cur.setResultSetBufferSize(2)
	checkSuccess(cur.sendQuery("select * from testtable order by testsmallint"),1)
	checkSuccess(cur.getField(2,0),3)
	id=cur.getResultSetId()
	cur.suspendResultSet()
	checkSuccess(con.suspendSession(),1)
	port=con.getConnectionPort()
	socket=con.getConnectionSocket()
	checkSuccess(con.resumeSession(port,socket),1)
	checkSuccess(cur.resumeResultSet(id),1)
	print
	checkSuccess(cur.firstRowIndex(),4)
	checkSuccess(cur.endOfResultSet(),0)
	checkSuccess(cur.rowCount(),6)
	checkSuccess(cur.getField(7,0),8)
	print
	checkSuccess(cur.firstRowIndex(),6)
	checkSuccess(cur.endOfResultSet(),0)
	checkSuccess(cur.rowCount(),8)
	checkSuccess(cur.getField(8,0),None)
	print
	checkSuccess(cur.firstRowIndex(),8)
	checkSuccess(cur.endOfResultSet(),1)
	checkSuccess(cur.rowCount(),8)
	cur.setResultSetBufferSize(0)
	print

	print "CACHED RESULT SET: "
	cur.cacheToFile("cachefile1")
	cur.setCacheTtl(200)
	checkSuccess(cur.sendQuery("select * from testtable order by testsmallint"),1)
	filename=cur.getCacheFileName()
	checkSuccess(filename,"cachefile1")
	cur.cacheOff()
	checkSuccess(cur.openCachedResultSet(filename),1)
	checkSuccess(cur.getField(7,0),8)
	print

	print "COLUMN COUNT FOR CACHED RESULT SET: "
	checkSuccess(cur.colCount(),11)
	print

	print "COLUMN NAMES FOR CACHED RESULT SET: "
	checkSuccess(cur.getColumnName(0),"TESTSMALLINT")
	checkSuccess(cur.getColumnName(1),"TESTINT")
	checkSuccess(cur.getColumnName(2),"TESTBIGINT")
	checkSuccess(cur.getColumnName(3),"TESTDECIMAL")
	checkSuccess(cur.getColumnName(4),"TESTREAL")
	checkSuccess(cur.getColumnName(5),"TESTDOUBLE")
	checkSuccess(cur.getColumnName(6),"TESTCHAR")
	checkSuccess(cur.getColumnName(7),"TESTVARCHAR")
	checkSuccess(cur.getColumnName(8),"TESTDATE")
	checkSuccess(cur.getColumnName(9),"TESTTIME")
	checkSuccess(cur.getColumnName(10),"TESTTIMESTAMP")
	cols=cur.getColumnNames()
	checkSuccess(cols[0],"TESTSMALLINT")
	checkSuccess(cols[1],"TESTINT")
	checkSuccess(cols[2],"TESTBIGINT")
	checkSuccess(cols[3],"TESTDECIMAL")
	checkSuccess(cols[4],"TESTREAL")
	checkSuccess(cols[5],"TESTDOUBLE")
	checkSuccess(cols[6],"TESTCHAR")
	checkSuccess(cols[7],"TESTVARCHAR")
	checkSuccess(cols[8],"TESTDATE")
	checkSuccess(cols[9],"TESTTIME")
	checkSuccess(cols[10],"TESTTIMESTAMP")
	print

	print "CACHED RESULT SET WITH RESULT SET BUFFER SIZE: "
	cur.setResultSetBufferSize(2)
	cur.cacheToFile("cachefile1")
	cur.setCacheTtl(200)
	checkSuccess(cur.sendQuery("select * from testtable order by testsmallint"),1)
	filename=cur.getCacheFileName()
	checkSuccess(filename,"cachefile1")
	cur.cacheOff()
	checkSuccess(cur.openCachedResultSet(filename),1)
	checkSuccess(cur.getField(7,0),8)
	checkSuccess(cur.getField(8,0),None)
	cur.setResultSetBufferSize(0)
	print

	print "FROM ONE CACHE FILE TO ANOTHER: "
	cur.cacheToFile("cachefile2")
	checkSuccess(cur.openCachedResultSet("cachefile1"),1)
	cur.cacheOff()
	checkSuccess(cur.openCachedResultSet("cachefile2"),1)
	checkSuccess(cur.getField(7,0),8)
	checkSuccess(cur.getField(8,0),None)
	print

	print "FROM ONE CACHE FILE TO ANOTHER WITH RESULT SET BUFFER SIZE: "
	cur.setResultSetBufferSize(2)
	cur.cacheToFile("cachefile2")
	checkSuccess(cur.openCachedResultSet("cachefile1"),1)
	cur.cacheOff()
	checkSuccess(cur.openCachedResultSet("cachefile2"),1)
	checkSuccess(cur.getField(7,0),8)
	checkSuccess(cur.getField(8,0),None)
	cur.setResultSetBufferSize(0)
	print

	print "CACHED RESULT SET WITH SUSPEND AND RESULT SET BUFFER SIZE: "
	cur.setResultSetBufferSize(2)
	cur.cacheToFile("cachefile1")
	cur.setCacheTtl(200)
	checkSuccess(cur.sendQuery("select * from testtable order by testsmallint"),1)
	checkSuccess(cur.getField(2,0),3)
	filename=cur.getCacheFileName()
	checkSuccess(filename,"cachefile1")
	id=cur.getResultSetId()
	cur.suspendResultSet()
	checkSuccess(con.suspendSession(),1)
	port=con.getConnectionPort()
	socket=con.getConnectionSocket()
	print
	checkSuccess(con.resumeSession(port,socket),1)
	checkSuccess(cur.resumeCachedResultSet(id,filename),1)
	print
	checkSuccess(cur.firstRowIndex(),4)
	checkSuccess(cur.endOfResultSet(),0)
	checkSuccess(cur.rowCount(),6)
	checkSuccess(cur.getField(7,0),8)
	print
	checkSuccess(cur.firstRowIndex(),6)
	checkSuccess(cur.endOfResultSet(),0)
	checkSuccess(cur.rowCount(),8)
	checkSuccess(cur.getField(8,0),None)
	print
	checkSuccess(cur.firstRowIndex(),8)
	checkSuccess(cur.endOfResultSet(),1)
	checkSuccess(cur.rowCount(),8)
	cur.cacheOff()
	print
	checkSuccess(cur.openCachedResultSet(filename),1)
	checkSuccess(cur.getField(7,0),8)
	checkSuccess(cur.getField(8,0),None)
	cur.setResultSetBufferSize(0)
	print

	print "ROW RANGE:"
	checkSuccess(cur.sendQuery("select * from testtable order by testsmallint"),1)
	print
	rows=cur.getRowRange(0,5)
	checkSuccess(rows[0][0],1);
	checkSuccess(rows[0][1],1);
	checkSuccess(rows[0][2],1);
	checkSuccess(rows[0][3],Decimal("1.1"));
	checkSuccess(rows[0][4],Decimal("1.1"));
	checkSuccess(rows[0][5],Decimal("1.1"));
	checkSuccess(rows[0][6],"testchar1                               ")
	checkSuccess(rows[0][7],"testvarchar1")
	checkSuccess(rows[0][8],"2001-01-01")
	checkSuccess(rows[0][9],"01:00:00")
	print
	checkSuccess(rows[1][0],2);
	checkSuccess(rows[1][1],2);
	checkSuccess(rows[1][2],2);
	checkSuccess(rows[1][3],Decimal("2.2"));
	checkSuccess(rows[1][4],Decimal("2.2"));
	checkSuccess(rows[1][5],Decimal("2.2"));
	checkSuccess(rows[1][6],"testchar2                               ")
	checkSuccess(rows[1][7],"testvarchar2")
	checkSuccess(rows[1][8],"2002-01-01")
	checkSuccess(rows[1][9],"02:00:00")
	print
	checkSuccess(rows[2][0],3);
	checkSuccess(rows[2][1],3);
	checkSuccess(rows[2][2],3);
	checkSuccess(rows[2][3],Decimal("3.3"));
	checkSuccess(rows[2][4],Decimal("3.3"));
	checkSuccess(rows[2][5],Decimal("3.3"));
	checkSuccess(rows[2][6],"testchar3                               ")
	checkSuccess(rows[2][7],"testvarchar3")
	checkSuccess(rows[2][8],"2003-01-01")
	checkSuccess(rows[2][9],"03:00:00")
	print
	checkSuccess(rows[3][0],4);
	checkSuccess(rows[3][1],4);
	checkSuccess(rows[3][2],4);
	checkSuccess(rows[3][3],Decimal("4.4"));
	checkSuccess(rows[3][4],Decimal("4.4"));
	checkSuccess(rows[3][5],Decimal("4.4"));
	checkSuccess(rows[3][6],"testchar4                               ")
	checkSuccess(rows[3][7],"testvarchar4")
	checkSuccess(rows[3][8],"2004-01-01")
	checkSuccess(rows[3][9],"04:00:00")
	print
	checkSuccess(rows[4][0],5);
	checkSuccess(rows[4][1],5);
	checkSuccess(rows[4][2],5);
	checkSuccess(rows[4][3],Decimal("5.5"));
	checkSuccess(rows[4][4],Decimal("5.5"));
	checkSuccess(rows[4][5],Decimal("5.5"));
	checkSuccess(rows[4][6],"testchar5                               ")
	checkSuccess(rows[4][7],"testvarchar5")
	checkSuccess(rows[4][8],"2005-01-01")
	checkSuccess(rows[4][9],"05:00:00")
	print
	checkSuccess(rows[5][0],6);
	checkSuccess(rows[5][1],6);
	checkSuccess(rows[5][2],6);
	checkSuccess(rows[5][3],Decimal("6.6"));
	checkSuccess(rows[5][4],Decimal("6.6"));
	checkSuccess(rows[5][5],Decimal("6.6"));
	checkSuccess(rows[5][6],"testchar6                               ")
	checkSuccess(rows[5][7],"testvarchar6")
	checkSuccess(rows[5][8],"2006-01-01")
	checkSuccess(rows[5][9],"06:00:00")
	print

	print "FINISHED SUSPENDED SESSION: "
	checkSuccess(cur.sendQuery("select * from testtable order by testint"),1)
	checkSuccess(cur.getField(4,0),5)
	checkSuccess(cur.getField(5,0),6)
	checkSuccess(cur.getField(6,0),7)
	checkSuccess(cur.getField(7,0),8)
	id=cur.getResultSetId()
	cur.suspendResultSet()
	checkSuccess(con.suspendSession(),1)
	port=con.getConnectionPort()
	socket=con.getConnectionSocket()
	checkSuccess(con.resumeSession(port,socket),1)
	checkSuccess(cur.resumeResultSet(id),1)
	checkSuccess(cur.getField(4,0),None)
	checkSuccess(cur.getField(5,0),None)
	checkSuccess(cur.getField(6,0),None)
	checkSuccess(cur.getField(7,0),None)
	print

	# drop existing table
	con.commit()
	cur.sendQuery("drop table testtable")
	print

	# invalid queries...
	print "INVALID QUERIES: "
	checkSuccess(cur.sendQuery("select * from testtable order by testsmallint"),0)
	checkSuccess(cur.sendQuery("select * from testtable order by testsmallint"),0)
	checkSuccess(cur.sendQuery("select * from testtable order by testsmallint"),0)
	checkSuccess(cur.sendQuery("select * from testtable order by testsmallint"),0)
	print
	checkSuccess(cur.sendQuery("insert into testtable values (1,2,3,4)"),0)
	checkSuccess(cur.sendQuery("insert into testtable values (1,2,3,4)"),0)
	checkSuccess(cur.sendQuery("insert into testtable values (1,2,3,4)"),0)
	checkSuccess(cur.sendQuery("insert into testtable values (1,2,3,4)"),0)
	print
	checkSuccess(cur.sendQuery("create table testtable"),0)
	checkSuccess(cur.sendQuery("create table testtable"),0)
	checkSuccess(cur.sendQuery("create table testtable"),0)
	checkSuccess(cur.sendQuery("create table testtable"),0)
	print

if __name__ == "__main__":
	main()
