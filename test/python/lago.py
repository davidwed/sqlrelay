#! /usr/bin/env python

# Copyright (c) 2001  David Muse
# See the file COPYING for more information.

from SQLRelay import PySQLRClient
import sys
import string

def checkSuccess(value,success):
	if value==success:
		print "success",
	else:
		print "failure"
		print "wanted", type(success), ":", success
		print "got   ", type(value), ":", value
		sys.exit(0)

def main():


	# usage...
	if len(sys.argv) < 5:
		print "usage: lago.py host port socket user password"
		sys.exit(0)


	# instantiation
	con=PySQLRClient.sqlrconnection(sys.argv[1],string.atoi(sys.argv[2]), 
					sys.argv[3],sys.argv[4],sys.argv[5])
	cur=PySQLRClient.sqlrcursor(con)

	# get database type
	print "IDENTIFY: "
	checkSuccess(con.identify(),"lago")
	print

	# ping
	print "PING: "
	checkSuccess(con.ping(),1)
	print

	# drop existing table
	cur.sendQuery("drop table testtable")

	print "CREATE TEMPTABLE: "
	checkSuccess(cur.sendQuery("create table testtable (testsmallint smallint, testint int, testfloat float, testdouble double, testdecimal decimal(1,1), testchar char(40), testvarchar varchar(40), testdate date, testtime time, testtimestamp timestamp)"),1)
	print

	print "INSERT: "
	checkSuccess(cur.sendQuery("insert into testtable (testsmallint, testint, testfloat, testdouble, testdecimal, testchar, testvarchar, testdate, testtime) values (1,1,1.1,1.1,1.1,'testchar1','testvarchar1','20010101','010000')"),1)
	checkSuccess(cur.sendQuery("insert into testtable (testsmallint, testint, testfloat, testdouble, testdecimal, testchar, testvarchar, testdate, testtime) values (2,2,2.1,2.1,2.1,'testchar2','testvarchar2','20020101','020000')"),1)
	checkSuccess(cur.sendQuery("insert into testtable (testsmallint, testint, testfloat, testdouble, testdecimal, testchar, testvarchar, testdate, testtime) values (3,3,3.1,3.1,3.1,'testchar3','testvarchar3','20030101','030000')"),1)
	checkSuccess(cur.sendQuery("insert into testtable (testsmallint, testint, testfloat, testdouble, testdecimal, testchar, testvarchar, testdate, testtime) values (4,4,4.1,4.1,4.1,'testchar4','testvarchar4','20040101','040000')"),1)
	print

	print "AFFECTED ROWS: "
	checkSuccess(cur.affectedRows(),-1)
	print

	print "BIND BY NAME: "
	cur.prepareQuery("insert into testtable (testsmallint, testint, testfloat, testdouble, testdecimal, testchar, testvarchar, testdate, testtime) values (:var1,:var2,:var3,:var4,:var5,:var6,:var7,:var8,:var9)")
	cur.inputBind("var1",5)
	cur.inputBind("var2",5)
	cur.inputBind("var3",5.1,2,1)
	cur.inputBind("var4",5.1,2,1)
	cur.inputBind("var5",5.1,2,1)
	cur.inputBind("var6","testchar5")
	cur.inputBind("var7","testvarchar5")
	cur.inputBind("var8","20050101")
	cur.inputBind("var9","050000")
	checkSuccess(cur.executeQuery(),1)
	cur.clearBinds()
	cur.inputBind("var1",6)
	cur.inputBind("var2",6)
	cur.inputBind("var3",6.1,2,1)
	cur.inputBind("var4",6.1,2,1)
	cur.inputBind("var5",6.1,2,1)
	cur.inputBind("var6","testchar6")
	cur.inputBind("var7","testvarchar6")
	cur.inputBind("var8","20060101")
	cur.inputBind("var9","060000")
	checkSuccess(cur.executeQuery(),1)
	print

	print "ARRAY OF BINDS BY NAME: "
	cur.clearBinds()
	cur.inputBinds(["var1","var2","var3","var4","var5","var6",
			"var7","var8","var9"],
		[7,7,7.7,7.7,7.7,"testchar7","testvarchar7",
			"20070101","070000"],
		[0,0,2,2,2,0,0,0,0],
		[0,0,1,1,1,0,0,0,0])
	checkSuccess(cur.executeQuery(),1)
	print

	print "BIND BY NAME WITH VALIDATION: "
	cur.clearBinds()
	cur.inputBind("var1",8)
	cur.inputBind("var2",8)
	cur.inputBind("var3",8.1,2,1)
	cur.inputBind("var4",8.1,2,1)
	cur.inputBind("var5",8.1,2,1)
	cur.inputBind("var6","testchar8")
	cur.inputBind("var7","testvarchar8")
	cur.inputBind("var8","20080101")
	cur.inputBind("var9","080000")
	cur.inputBind("var10","junkvalue")
	cur.validateBinds()
	checkSuccess(cur.executeQuery(),1)
	print

	print "SELECT: "
	checkSuccess(cur.sendQuery("select * from testtable"),1)
	print

	print "COLUMN COUNT: "
	checkSuccess(cur.colCount(),10)
	print

	print "COLUMN NAMES: "
	checkSuccess(cur.getColumnName(0),"testsmallint")
	checkSuccess(cur.getColumnName(1),"testint")
	checkSuccess(cur.getColumnName(2),"testfloat")
	checkSuccess(cur.getColumnName(3),"testdouble")
	checkSuccess(cur.getColumnName(4),"testdecimal")
	checkSuccess(cur.getColumnName(5),"testchar")
	checkSuccess(cur.getColumnName(6),"testvarchar")
	checkSuccess(cur.getColumnName(7),"testdate")
	checkSuccess(cur.getColumnName(8),"testtime")
	checkSuccess(cur.getColumnName(9),"testtimestamp")
	cols=cur.getColumnNames()
	checkSuccess(cols[0],"testsmallint")
	checkSuccess(cols[1],"testint")
	checkSuccess(cols[2],"testfloat")
	checkSuccess(cols[3],"testdouble")
	checkSuccess(cols[4],"testdecimal")
	checkSuccess(cols[5],"testchar")
	checkSuccess(cols[6],"testvarchar")
	checkSuccess(cols[7],"testdate")
	checkSuccess(cols[8],"testtime")
	checkSuccess(cols[9],"testtimestamp")
	print

	print "COLUMN TYPES: "
	checkSuccess(cur.getColumnType(0),"SMALLINT")
	checkSuccess(cur.getColumnType('testsmallint'),"SMALLINT")
	checkSuccess(cur.getColumnType(1),"INT")
	checkSuccess(cur.getColumnType('testint'),"INT")
	checkSuccess(cur.getColumnType(2),"FLOAT")
	checkSuccess(cur.getColumnType('testfloat'),"FLOAT")
	checkSuccess(cur.getColumnType(3),"DOUBLE")
	checkSuccess(cur.getColumnType('testdouble'),"DOUBLE")
	checkSuccess(cur.getColumnType(4),"DOUBLE")
	checkSuccess(cur.getColumnType('testdecimal'),"DOUBLE")
	checkSuccess(cur.getColumnType(5),"CHAR")
	checkSuccess(cur.getColumnType('testchar'),"CHAR")
	checkSuccess(cur.getColumnType(6),"VARCHAR")
	checkSuccess(cur.getColumnType('testvarchar'),"VARCHAR")
	checkSuccess(cur.getColumnType(7),"DATE")
	checkSuccess(cur.getColumnType('testdate'),"DATE")
	checkSuccess(cur.getColumnType(8),"TIME")
	checkSuccess(cur.getColumnType('testtime'),"TIME")
	checkSuccess(cur.getColumnType(9),"TIMESTAMP")
	checkSuccess(cur.getColumnType('testtimestamp'),"TIMESTAMP")
	print

	print "COLUMN LENGTH: "
	checkSuccess(cur.getColumnLength(0),2)
	checkSuccess(cur.getColumnLength('testsmallint'),2)
	checkSuccess(cur.getColumnLength(1),4)
	checkSuccess(cur.getColumnLength('testint'),4)
	checkSuccess(cur.getColumnLength(2),4)
	checkSuccess(cur.getColumnLength('testfloat'),4)
	checkSuccess(cur.getColumnLength(3),8)
	checkSuccess(cur.getColumnLength('testdouble'),8)
	checkSuccess(cur.getColumnLength(4),8)
	checkSuccess(cur.getColumnLength('testdecimal'),8)
	checkSuccess(cur.getColumnLength(5),40)
	checkSuccess(cur.getColumnLength('testchar'),40)
	checkSuccess(cur.getColumnLength(6),40)
	checkSuccess(cur.getColumnLength('testvarchar'),40)
	checkSuccess(cur.getColumnLength(7),4)
	checkSuccess(cur.getColumnLength('testdate'),4)
	checkSuccess(cur.getColumnLength(8),4)
	checkSuccess(cur.getColumnLength('testtime'),4)
	checkSuccess(cur.getColumnLength(9),4)
	checkSuccess(cur.getColumnLength('testtimestamp'),4)
	print

	print "LONGEST COLUMN: "
	checkSuccess(cur.getLongest(0),1)
	checkSuccess(cur.getLongest('testsmallint'),1)
	checkSuccess(cur.getLongest(1),1)
	checkSuccess(cur.getLongest('testint'),1)
	checkSuccess(cur.getLongest(2),3)
	checkSuccess(cur.getLongest('testfloat'),3)
	checkSuccess(cur.getLongest(3),3)
	checkSuccess(cur.getLongest('testdouble'),3)
	checkSuccess(cur.getLongest(4),3)
	checkSuccess(cur.getLongest('testdecimal'),3)
	checkSuccess(cur.getLongest(5),40)
	checkSuccess(cur.getLongest('testchar'),40)
	checkSuccess(cur.getLongest(6),12)
	checkSuccess(cur.getLongest('testvarchar'),12)
	checkSuccess(cur.getLongest(7),11)
	checkSuccess(cur.getLongest('testdate'),11)
	checkSuccess(cur.getLongest(8),8)
	checkSuccess(cur.getLongest('testtime'),8)
	print

	print "ROW COUNT: "
	checkSuccess(cur.rowCount(),8)
	print

	print "TOTAL ROWS: "
	checkSuccess(cur.totalRows(),8)
	print

	print "FIRST ROW INDEX: "
	checkSuccess(cur.firstRowIndex(),0)
	print

	print "END OF RESULT SET: "
	checkSuccess(cur.endOfResultSet(),1)
	print

	print "FIELDS BY INDEX: "
	checkSuccess(cur.getField(0,0),"1")
	checkSuccess(cur.getField(0,1),"1")
	checkSuccess(cur.getField(0,2),"1.1")
	checkSuccess(cur.getField(0,3),"1.1")
	checkSuccess(cur.getField(0,4),"1.1")
	checkSuccess(cur.getField(0,5),"testchar1                               ")
	checkSuccess(cur.getField(0,6),"testvarchar1")
	checkSuccess(cur.getField(0,7)," 1-Jan-2001")
	checkSuccess(cur.getField(0,8),"01:00:00")
	print
	checkSuccess(cur.getField(7,0),"8")
	checkSuccess(cur.getField(7,1),"8")
	checkSuccess(cur.getField(7,2),"8.1")
	checkSuccess(cur.getField(7,3),"8.1")
	checkSuccess(cur.getField(7,4),"8.1")
	checkSuccess(cur.getField(7,5),"testchar8                               ")
	checkSuccess(cur.getField(7,6),"testvarchar8")
	checkSuccess(cur.getField(7,7)," 1-Jan-2008")
	checkSuccess(cur.getField(7,8),"08:00:00")
	print

	print "FIELD LENGTHS BY INDEX: "
	checkSuccess(cur.getFieldLength(0,0),1)
	checkSuccess(cur.getFieldLength(0,1),1)
	checkSuccess(cur.getFieldLength(0,2),3)
	checkSuccess(cur.getFieldLength(0,3),3)
	checkSuccess(cur.getFieldLength(0,4),3)
	checkSuccess(cur.getFieldLength(0,5),40)
	checkSuccess(cur.getFieldLength(0,6),12)
	checkSuccess(cur.getFieldLength(0,7),11)
	checkSuccess(cur.getFieldLength(0,8),8)
	print
	checkSuccess(cur.getFieldLength(7,0),1)
	checkSuccess(cur.getFieldLength(7,1),1)
	checkSuccess(cur.getFieldLength(7,2),3)
	checkSuccess(cur.getFieldLength(7,3),3)
	checkSuccess(cur.getFieldLength(7,4),3)
	checkSuccess(cur.getFieldLength(7,5),40)
	checkSuccess(cur.getFieldLength(7,6),12)
	checkSuccess(cur.getFieldLength(7,7),11)
	checkSuccess(cur.getFieldLength(7,8),8)
	print

	print "FIELDS BY NAME: "
	checkSuccess(cur.getField(0,"testint"),"1")
	checkSuccess(cur.getField(0,"testsmallint"),"1")
	checkSuccess(cur.getField(0,"testfloat"),"1.1")
	checkSuccess(cur.getField(0,"testdouble"),"1.1")
	checkSuccess(cur.getField(0,"testdecimal"),"1.1")
	checkSuccess(cur.getField(0,"testchar"),"testchar1                               ")
	checkSuccess(cur.getField(0,"testvarchar"),"testvarchar1")
	checkSuccess(cur.getField(0,"testdate")," 1-Jan-2001")
	checkSuccess(cur.getField(0,"testtime"),"01:00:00")
	print
	checkSuccess(cur.getField(7,"testint"),"8")
	checkSuccess(cur.getField(7,"testsmallint"),"8")
	checkSuccess(cur.getField(7,"testfloat"),"8.1")
	checkSuccess(cur.getField(7,"testdouble"),"8.1")
	checkSuccess(cur.getField(7,"testdecimal"),"8.1")
	checkSuccess(cur.getField(7,"testchar"),"testchar8                               ")
	checkSuccess(cur.getField(7,"testvarchar"),"testvarchar8")
	checkSuccess(cur.getField(7,"testdate")," 1-Jan-2008")
	checkSuccess(cur.getField(7,"testtime"),"08:00:00")
	print

	print "FIELD LENGTHS BY NAME: "
	checkSuccess(cur.getFieldLength(0,"testint"),1)
	checkSuccess(cur.getFieldLength(0,"testsmallint"),1)
	checkSuccess(cur.getFieldLength(0,"testfloat"),3)
	checkSuccess(cur.getFieldLength(0,"testdouble"),3)
	checkSuccess(cur.getFieldLength(0,"testdecimal"),3)
	checkSuccess(cur.getFieldLength(0,"testchar"),40)
	checkSuccess(cur.getFieldLength(0,"testvarchar"),12)
	checkSuccess(cur.getFieldLength(0,"testdate"),11)
	checkSuccess(cur.getFieldLength(0,"testtime"),8)
	print
	checkSuccess(cur.getFieldLength(7,"testint"),1)
	checkSuccess(cur.getFieldLength(7,"testsmallint"),1)
	checkSuccess(cur.getFieldLength(7,"testfloat"),3)
	checkSuccess(cur.getFieldLength(7,"testdouble"),3)
	checkSuccess(cur.getFieldLength(7,"testdecimal"),3)
	checkSuccess(cur.getFieldLength(7,"testchar"),40)
	checkSuccess(cur.getFieldLength(7,"testvarchar"),12)
	checkSuccess(cur.getFieldLength(7,"testdate"),11)
	checkSuccess(cur.getFieldLength(7,"testtime"),8)
	print

	print "FIELDS BY ARRAY: "
	fields=cur.getRow(0)
	checkSuccess(fields[0],1)
	checkSuccess(fields[1],1)
	checkSuccess(fields[2],1.1)
	checkSuccess(fields[3],1.1)
	checkSuccess(fields[4],1.1)
	checkSuccess(fields[5],"testchar1                               ")
	checkSuccess(fields[6],"testvarchar1")
	checkSuccess(fields[7]," 1-Jan-2001")
	checkSuccess(fields[8],"01:00:00")
	print

	print "FIELD LENGTHS BY ARRAY: "
	fieldlens=cur.getRowLengths(0)
	checkSuccess(fieldlens[0],1)
	checkSuccess(fieldlens[1],1)
	checkSuccess(fieldlens[2],3)
	checkSuccess(fieldlens[3],3)
	checkSuccess(fieldlens[4],3)
	checkSuccess(fieldlens[5],40)
	checkSuccess(fieldlens[6],12)
	checkSuccess(fieldlens[7],11)
	checkSuccess(fieldlens[8],8)
	print

	print "FIELDS BY DICTIONARY: "
	fields=cur.getRowDictionary(0)
	checkSuccess(fields["testint"],1)
	checkSuccess(fields["testsmallint"],1)
	checkSuccess(fields["testfloat"],1.1)
	checkSuccess(fields["testdouble"],1.1)
	checkSuccess(fields["testdecimal"],1.1)
	checkSuccess(fields["testchar"],"testchar1                               ")
	checkSuccess(fields["testvarchar"],"testvarchar1")
	checkSuccess(fields["testdate"]," 1-Jan-2001")
	checkSuccess(fields["testtime"],"01:00:00")
	print
	fields=cur.getRowDictionary(7)
	checkSuccess(fields["testint"],8)
	checkSuccess(fields["testsmallint"],8)
	checkSuccess(fields["testfloat"],8.1)
	checkSuccess(fields["testdouble"],8.1)
	checkSuccess(fields["testdecimal"],8.1)
	checkSuccess(fields["testchar"],"testchar8                               ")
	checkSuccess(fields["testvarchar"],"testvarchar8")
	checkSuccess(fields["testdate"]," 1-Jan-2008")
	checkSuccess(fields["testtime"],"08:00:00")
	print

	print "FIELD LENGTHS BY DICTIONARY: "
	fieldlengths=cur.getRowLengthsDictionary(0)
	checkSuccess(fieldlengths["testint"],1)
	checkSuccess(fieldlengths["testsmallint"],1)
	checkSuccess(fieldlengths["testfloat"],3)
	checkSuccess(fieldlengths["testdouble"],3)
	checkSuccess(fieldlengths["testdecimal"],3)
	checkSuccess(fieldlengths["testchar"],40)
	checkSuccess(fieldlengths["testvarchar"],12)
	checkSuccess(fieldlengths["testdate"],11)
	checkSuccess(fieldlengths["testtime"],8)
	print
	fieldlengths=cur.getRowLengthsDictionary(7)
	checkSuccess(fieldlengths["testint"],1)
	checkSuccess(fieldlengths["testsmallint"],1)
	checkSuccess(fieldlengths["testfloat"],3)
	checkSuccess(fieldlengths["testdouble"],3)
	checkSuccess(fieldlengths["testdecimal"],3)
	checkSuccess(fieldlengths["testchar"],40)
	checkSuccess(fieldlengths["testvarchar"],12)
	checkSuccess(fieldlengths["testdate"],11)
	checkSuccess(fieldlengths["testtime"],8)
	print
	
	print "INDIVIDUAL SUBSTITUTIONS: "
	cur.sendQuery("drop table testtable1");
	checkSuccess(cur.sendQuery("create table testtable1 (col1 int, col2 varchar(40), col3 real)"),1)
	cur.prepareQuery("insert into testtable1 (col1, col2, col3) values ($(var1),'$(var2)',$(var3))")
	cur.substitution("var1",1)
	cur.substitution("var2","hello")
	cur.substitution("var3",10.5556,6,4)
	checkSuccess(cur.executeQuery(),1)
	print

	print "FIELDS: "
	checkSuccess(cur.sendQuery("select * from testtable1"),1)
	checkSuccess(cur.getField(0,0),"1")
	checkSuccess(cur.getField(0,1),"hello")
	checkSuccess(cur.getField(0,2),"10.5556")
	checkSuccess(cur.sendQuery("delete from testtable1"),1)
	print

	print "ARRAY SUBSTITUTIONS: "
	cur.prepareQuery("insert into testtable1 (col1, col2, col3) values ($(var1),'$(var2)',$(var3))")
	cur.substitutions(["var1","var2","var3"],
				[1,"hello",10.5556],[0,0,6],[0,0,4])
	checkSuccess(cur.executeQuery(),1)
	print

	print "FIELDS: "
	checkSuccess(cur.sendQuery("select * from testtable1"),1)
	checkSuccess(cur.getField(0,0),"1")
	checkSuccess(cur.getField(0,1),"hello")
	checkSuccess(cur.getField(0,2),"10.5556")
	checkSuccess(cur.sendQuery("delete from testtable1"),1)
	print

	print "NULLS as Nones: "
	cur.getNullsAsNone()
	checkSuccess(cur.sendQuery("insert into testtable1 (col1, col2, col3) values (1,NULL,NULL)"),1)
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
	checkSuccess(cur.sendQuery("select * from testtable"),1)
	checkSuccess(cur.getResultSetBufferSize(),2)
	print
	checkSuccess(cur.firstRowIndex(),0)
	checkSuccess(cur.endOfResultSet(),0)
	checkSuccess(cur.rowCount(),2)
	checkSuccess(cur.getField(0,0),"1")
	checkSuccess(cur.getField(1,0),"2")
	checkSuccess(cur.getField(2,0),"3")
	print
	checkSuccess(cur.firstRowIndex(),2)
	checkSuccess(cur.endOfResultSet(),0)
	checkSuccess(cur.rowCount(),4)
	checkSuccess(cur.getField(6,0),"7")
	checkSuccess(cur.getField(7,0),"8")
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
	checkSuccess(cur.sendQuery("select * from testtable"),1)
	checkSuccess(cur.getColumnName(0),None)
	checkSuccess(cur.getColumnLength(0),0)
	checkSuccess(cur.getColumnType(0),None)
	cur.getColumnInfo()
	checkSuccess(cur.sendQuery("select * from testtable"),1)
	checkSuccess(cur.getColumnName(0),"testsmallint")
	checkSuccess(cur.getColumnLength(0),2)
	checkSuccess(cur.getColumnType(0),"SMALLINT")
	print

	print "SUSPENDED SESSION: "
	checkSuccess(cur.sendQuery("select * from testtable"),1)
	cur.suspendResultSet()
	checkSuccess(con.suspendSession(),1)
	port=con.getConnectionPort()
	socket=con.getConnectionSocket()
	checkSuccess(con.resumeSession(port,socket),1)
	print
	checkSuccess(cur.getField(0,0),"1")
	checkSuccess(cur.getField(1,0),"2")
	checkSuccess(cur.getField(2,0),"3")
	checkSuccess(cur.getField(3,0),"4")
	checkSuccess(cur.getField(4,0),"5")
	checkSuccess(cur.getField(5,0),"6")
	checkSuccess(cur.getField(6,0),"7")
	checkSuccess(cur.getField(7,0),"8")
	print
	checkSuccess(cur.sendQuery("select * from testtable"),1)
	cur.suspendResultSet()
	checkSuccess(con.suspendSession(),1)
	port=con.getConnectionPort()
	socket=con.getConnectionSocket()
	checkSuccess(con.resumeSession(port,socket),1)
	print
	checkSuccess(cur.getField(0,0),"1")
	checkSuccess(cur.getField(1,0),"2")
	checkSuccess(cur.getField(2,0),"3")
	checkSuccess(cur.getField(3,0),"4")
	checkSuccess(cur.getField(4,0),"5")
	checkSuccess(cur.getField(5,0),"6")
	checkSuccess(cur.getField(6,0),"7")
	checkSuccess(cur.getField(7,0),"8")
	print
	checkSuccess(cur.sendQuery("select * from testtable"),1)
	cur.suspendResultSet()
	checkSuccess(con.suspendSession(),1)
	port=con.getConnectionPort()
	socket=con.getConnectionSocket()
	checkSuccess(con.resumeSession(port,socket),1)
	print
	checkSuccess(cur.getField(0,0),"1")
	checkSuccess(cur.getField(1,0),"2")
	checkSuccess(cur.getField(2,0),"3")
	checkSuccess(cur.getField(3,0),"4")
	checkSuccess(cur.getField(4,0),"5")
	checkSuccess(cur.getField(5,0),"6")
	checkSuccess(cur.getField(6,0),"7")
	checkSuccess(cur.getField(7,0),"8")
	print

	print "SUSPENDED RESULT SET: "
	cur.setResultSetBufferSize(2)
	checkSuccess(cur.sendQuery("select * from testtable"),1)
	checkSuccess(cur.getField(2,0),"3")
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
	checkSuccess(cur.getField(7,0),"8")
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
	checkSuccess(cur.sendQuery("select * from testtable"),1)
	filename=cur.getCacheFileName()
	checkSuccess(filename,"cachefile1")
	cur.cacheOff()
	checkSuccess(cur.openCachedResultSet(filename),1)
	checkSuccess(cur.getField(7,0),"8")
	print

	print "COLUMN COUNT FOR CACHED RESULT SET: "
	checkSuccess(cur.colCount(),10)
	print

	print "COLUMN NAMES FOR CACHED RESULT SET: "
	checkSuccess(cur.getColumnName(0),"testsmallint")
	checkSuccess(cur.getColumnName(1),"testint")
	checkSuccess(cur.getColumnName(2),"testfloat")
	checkSuccess(cur.getColumnName(3),"testdouble")
	checkSuccess(cur.getColumnName(4),"testdecimal")
	checkSuccess(cur.getColumnName(5),"testchar")
	checkSuccess(cur.getColumnName(6),"testvarchar")
	checkSuccess(cur.getColumnName(7),"testdate")
	checkSuccess(cur.getColumnName(8),"testtime")
	checkSuccess(cur.getColumnName(9),"testtimestamp")
	cols=cur.getColumnNames()
	checkSuccess(cols[0],"testsmallint")
	checkSuccess(cols[1],"testint")
	checkSuccess(cols[2],"testfloat")
	checkSuccess(cols[3],"testdouble")
	checkSuccess(cols[4],"testdecimal")
	checkSuccess(cols[5],"testchar")
	checkSuccess(cols[6],"testvarchar")
	checkSuccess(cols[7],"testdate")
	checkSuccess(cols[8],"testtime")
	checkSuccess(cols[9],"testtimestamp")
	print

	print "CACHED RESULT SET WITH RESULT SET BUFFER SIZE: "
	cur.setResultSetBufferSize(2)
	cur.cacheToFile("cachefile1")
	cur.setCacheTtl(200)
	checkSuccess(cur.sendQuery("select * from testtable"),1)
	filename=cur.getCacheFileName()
	checkSuccess(filename,"cachefile1")
	cur.cacheOff()
	checkSuccess(cur.openCachedResultSet(filename),1)
	checkSuccess(cur.getField(7,0),"8")
	checkSuccess(cur.getField(8,0),None)
	cur.setResultSetBufferSize(0)
	print

	print "FROM ONE CACHE FILE TO ANOTHER: "
	cur.cacheToFile("cachefile2")
	checkSuccess(cur.openCachedResultSet("cachefile1"),1)
	cur.cacheOff()
	checkSuccess(cur.openCachedResultSet("cachefile2"),1)
	checkSuccess(cur.getField(7,0),"8")
	checkSuccess(cur.getField(8,0),None)
	print

	print "FROM ONE CACHE FILE TO ANOTHER WITH RESULT SET BUFFER SIZE: "
	cur.setResultSetBufferSize(2)
	cur.cacheToFile("cachefile2")
	checkSuccess(cur.openCachedResultSet("cachefile1"),1)
	cur.cacheOff()
	checkSuccess(cur.openCachedResultSet("cachefile2"),1)
	checkSuccess(cur.getField(7,0),"8")
	checkSuccess(cur.getField(8,0),None)
	cur.setResultSetBufferSize(0)
	print

	print "CACHED RESULT SET WITH SUSPEND AND RESULT SET BUFFER SIZE: "
	cur.setResultSetBufferSize(2)
	cur.cacheToFile("cachefile1")
	cur.setCacheTtl(200)
	checkSuccess(cur.sendQuery("select * from testtable"),1)
	checkSuccess(cur.getField(2,0),"3")
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
	checkSuccess(cur.getField(7,0),"8")
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
	checkSuccess(cur.getField(7,0),"8")
	checkSuccess(cur.getField(8,0),None)
	cur.setResultSetBufferSize(0)
	print

	print "COMMIT AND ROLLBACK: "
	secondcon=PySQLRClient.sqlrconnection(sys.argv[1],
				string.atoi(sys.argv[2]), 
				sys.argv[3],sys.argv[4],sys.argv[5])
	secondcur=PySQLRClient.sqlrcursor(secondcon)
	checkSuccess(secondcur.sendQuery("select * from testtable"),1)
	checkSuccess(secondcur.getField(0,0),"1")
	checkSuccess(con.commit(),1)
	checkSuccess(secondcur.sendQuery("select * from testtable"),1)
	checkSuccess(secondcur.getField(0,0),"1")
	checkSuccess(con.autoCommitOn(),1)
	checkSuccess(cur.sendQuery("insert into testtable (testsmallint, testint, testfloat, testdouble, testdecimal, testchar, testvarchar, testdate, testtime) values (10,10,10.1,10.1,10.1,'testchar10','testvarchar10','20100101','100000')"),1)
	checkSuccess(secondcur.sendQuery("select * from testtable"),1)
	checkSuccess(secondcur.getField(8,0),"10")
	checkSuccess(con.autoCommitOff(),1)
	print

	print "ROW RANGE:"
	checkSuccess(cur.sendQuery("select * from testtable"),1)
	print
	rows=cur.getRowRange(0,5)
	checkSuccess(rows[0][0],1)
	checkSuccess(rows[0][1],1)
	checkSuccess(rows[0][2],1.1)
	checkSuccess(rows[0][3],1.1)
	checkSuccess(rows[0][4],1.1)
	checkSuccess(rows[0][5],"testchar1                               ")
	checkSuccess(rows[0][6],"testvarchar1")
	checkSuccess(rows[0][7]," 1-Jan-2001")
	checkSuccess(rows[0][8],"01:00:00")
	print
	checkSuccess(rows[1][0],2)
	checkSuccess(rows[1][1],2)
	checkSuccess(rows[1][2],2.1)
	checkSuccess(rows[1][3],2.1)
	checkSuccess(rows[1][4],2.1)
	checkSuccess(rows[1][5],"testchar2                               ")
	checkSuccess(rows[1][6],"testvarchar2")
	checkSuccess(rows[1][7]," 1-Jan-2002")
	checkSuccess(rows[1][8],"02:00:00")
	print
	checkSuccess(rows[2][0],3)
	checkSuccess(rows[2][1],3)
	checkSuccess(rows[2][2],3.1)
	checkSuccess(rows[2][3],3.1)
	checkSuccess(rows[2][4],3.1)
	checkSuccess(rows[2][5],"testchar3                               ")
	checkSuccess(rows[2][6],"testvarchar3")
	checkSuccess(rows[2][7]," 1-Jan-2003")
	checkSuccess(rows[2][8],"03:00:00")
	print
	checkSuccess(rows[3][0],4)
	checkSuccess(rows[3][1],4)
	checkSuccess(rows[3][2],4.1)
	checkSuccess(rows[3][3],4.1)
	checkSuccess(rows[3][4],4.1)
	checkSuccess(rows[3][5],"testchar4                               ")
	checkSuccess(rows[3][6],"testvarchar4")
	checkSuccess(rows[3][7]," 1-Jan-2004")
	checkSuccess(rows[3][8],"04:00:00")
	print
	checkSuccess(rows[4][0],5)
	checkSuccess(rows[4][1],5)
	checkSuccess(rows[4][2],5.1)
	checkSuccess(rows[4][3],5.1)
	checkSuccess(rows[4][4],5.1)
	checkSuccess(rows[4][5],"testchar5                               ")
	checkSuccess(rows[4][6],"testvarchar5")
	checkSuccess(rows[4][7]," 1-Jan-2005")
	checkSuccess(rows[4][8],"05:00:00")
	print
	checkSuccess(rows[5][0],6)
	checkSuccess(rows[5][1],6)
	checkSuccess(rows[5][2],6.1)
	checkSuccess(rows[5][3],6.1)
	checkSuccess(rows[5][4],6.1)
	checkSuccess(rows[5][5],"testchar6                               ")
	checkSuccess(rows[5][6],"testvarchar6")
	checkSuccess(rows[5][7]," 1-Jan-2006")
	checkSuccess(rows[5][8],"06:00:00")
	print

	print "FINISHED SUSPENDED SESSION: "
	checkSuccess(cur.sendQuery("select * from testtable"),1)
	checkSuccess(cur.getField(4,0),"5")
	checkSuccess(cur.getField(5,0),"6")
	checkSuccess(cur.getField(6,0),"7")
	checkSuccess(cur.getField(7,0),"8")
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
	cur.sendQuery("drop table testtable")

	# invalid queries...
	print "INVALID QUERIES: "
	checkSuccess(cur.sendQuery("select * from testtable"),0)
	checkSuccess(cur.sendQuery("select * from testtable"),0)
	checkSuccess(cur.sendQuery("select * from testtable"),0)
	checkSuccess(cur.sendQuery("select * from testtable"),0)
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
