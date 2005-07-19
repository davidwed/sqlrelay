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
		print "usage: msql.py host port socket user password"
		sys.exit(0)


	# instantiation
	con=PySQLRClient.sqlrconnection(sys.argv[1],string.atoi(sys.argv[2]), 
					sys.argv[3],sys.argv[4],sys.argv[5])
	cur=PySQLRClient.sqlrcursor(con)

	# get database type
	print "IDENTIFY: "
	checkSuccess(con.identify(),"msql")
	print

	# ping
	print "PING: "
	checkSuccess(con.ping(),1)
	print

	# drop existing table
	cur.sendQuery("drop table testtable")

	print "CREATE TEMPTABLE: "
	checkSuccess(cur.sendQuery("create table testtable (testchar char(40), testdate date, testint int, testmoney money, testreal real, testtext text(40), testtime time, testuint uint)"),1)
	print

	print "INSERT: "
	checkSuccess(cur.sendQuery("insert into testtable values ('char1','01-Jan-2001',1,1.00,1.1,'text1','01:00:00',1)"),1)
	checkSuccess(cur.sendQuery("insert into testtable values ('char2','01-Jan-2002',2,2.00,2.1,'text2','02:00:00',2)"),1)
	checkSuccess(cur.sendQuery("insert into testtable values ('char3','01-Jan-2003',3,3.00,3.1,'text3','03:00:00',3)"),1)
	checkSuccess(cur.sendQuery("insert into testtable values ('char4','01-Jan-2004',4,4.00,4.1,'text4','04:00:00',4)"),1)
	print

	print "AFFECTED ROWS: "
	checkSuccess(cur.affectedRows(),0)
	print

	print "BIND BY NAME: "
	cur.prepareQuery("insert into testtable values (:var1,:var2,:var3,:var4,:var5,:var6,:var7,:var8)")
	checkSuccess(cur.countBindVariables(),8)
	cur.inputBind("var1","char5")
	cur.inputBind("var2","01-Jan-2005")
	cur.inputBind("var3",5)
	cur.inputBind("var4",5.00,3,2)
	cur.inputBind("var5",5.1,2,1)
	cur.inputBind("var6","text5")
	cur.inputBind("var7","05:00:00")
	cur.inputBind("var8",5)
	checkSuccess(cur.executeQuery(),1)
	cur.clearBinds()
	cur.inputBind("var1","char6")
	cur.inputBind("var2","01-Jan-2006")
	cur.inputBind("var3",6)
	cur.inputBind("var4",6.00,3,2)
	cur.inputBind("var5",6.1,2,1)
	cur.inputBind("var6","text6")
	cur.inputBind("var7","06:00:00")
	cur.inputBind("var8",6)
	checkSuccess(cur.executeQuery(),1)
	print

	print "ARRAY OF BINDS BY NAME: "
	cur.clearBinds()
	cur.inputBinds(["var1","var2","var3","var4","var5","var6",
			"var7","var8"],
		["char7","01-Jan-2007",7,7.00,7.1,"text7",
			"07:00:00",7],
		[0,0,0,3,2,0,0,0],
		[0,0,0,2,1,0,0,0])
	checkSuccess(cur.executeQuery(),1)
	print

	print "BIND BY NAME WITH VALIDATION: "
	cur.clearBinds()
	cur.inputBind("var1","char8")
	cur.inputBind("var2","01-Jan-2008")
	cur.inputBind("var3",8)
	cur.inputBind("var4",8.00,3,2)
	cur.inputBind("var5",8.1,2,1)
	cur.inputBind("var6","text8")
	cur.inputBind("var7","08:00:00")
	cur.inputBind("var8",8)
	cur.inputBind("var9","junkvalue")
	cur.validateBinds()
	checkSuccess(cur.executeQuery(),1)
	print

	print "SELECT: "
	checkSuccess(cur.sendQuery("select * from testtable order by testint"),1)
	print

	print "COLUMN COUNT: "
	checkSuccess(cur.colCount(),8)
	print

	print "COLUMN NAMES: "
	checkSuccess(cur.getColumnName(0),"testchar")
	checkSuccess(cur.getColumnName(1),"testdate")
	checkSuccess(cur.getColumnName(2),"testint")
	checkSuccess(cur.getColumnName(3),"testmoney")
	checkSuccess(cur.getColumnName(4),"testreal")
	checkSuccess(cur.getColumnName(5),"testtext")
	checkSuccess(cur.getColumnName(6),"testtime")
	checkSuccess(cur.getColumnName(7),"testuint")
	cols=cur.getColumnNames()
	checkSuccess(cols[0],"testchar")
	checkSuccess(cols[1],"testdate")
	checkSuccess(cols[2],"testint")
	checkSuccess(cols[3],"testmoney")
	checkSuccess(cols[4],"testreal")
	checkSuccess(cols[5],"testtext")
	checkSuccess(cols[6],"testtime")
	checkSuccess(cols[7],"testuint")
	print

	print "COLUMN TYPES: "
	checkSuccess(cur.getColumnType(0),"CHAR")
	checkSuccess(cur.getColumnType('testchar'),"CHAR")
	checkSuccess(cur.getColumnType(1),"DATE")
	checkSuccess(cur.getColumnType('testdate'),"DATE")
	checkSuccess(cur.getColumnType(2),"INT")
	checkSuccess(cur.getColumnType('testint'),"INT")
	checkSuccess(cur.getColumnType(3),"MONEY")
	checkSuccess(cur.getColumnType('testmoney'),"MONEY")
	checkSuccess(cur.getColumnType(4),"REAL")
	checkSuccess(cur.getColumnType('testreal'),"REAL")
	checkSuccess(cur.getColumnType(5),"TEXT")
	checkSuccess(cur.getColumnType('testtext'),"TEXT")
	checkSuccess(cur.getColumnType(6),"TIME")
	checkSuccess(cur.getColumnType('testtime'),"TIME")
	checkSuccess(cur.getColumnType(7),"UINT")
	checkSuccess(cur.getColumnType('testuint'),"UINT")
	print

	print "COLUMN LENGTH: "
	checkSuccess(cur.getColumnLength(0),40)
	checkSuccess(cur.getColumnLength('testchar'),40)
	checkSuccess(cur.getColumnLength(1),4)
	checkSuccess(cur.getColumnLength('testdate'),4)
	checkSuccess(cur.getColumnLength(2),4)
	checkSuccess(cur.getColumnLength('testint'),4)
	checkSuccess(cur.getColumnLength(3),4)
	checkSuccess(cur.getColumnLength('testmoney'),4)
	checkSuccess(cur.getColumnLength(4),8)
	checkSuccess(cur.getColumnLength('testreal'),8)
	checkSuccess(cur.getColumnLength(5),40)
	checkSuccess(cur.getColumnLength('testtext'),40)
	checkSuccess(cur.getColumnLength(6),4)
	checkSuccess(cur.getColumnLength('testtime'),4)
	checkSuccess(cur.getColumnLength(7),4)
	checkSuccess(cur.getColumnLength('testuint'),4)
	print

	print "LONGEST COLUMN: "
	checkSuccess(cur.getLongest(0),5)
	checkSuccess(cur.getLongest('testchar'),5)
	checkSuccess(cur.getLongest(1),11)
	checkSuccess(cur.getLongest('testdate'),11)
	checkSuccess(cur.getLongest(2),1)
	checkSuccess(cur.getLongest('testint'),1)
	checkSuccess(cur.getLongest(3),4)
	checkSuccess(cur.getLongest('testmoney'),4)
	checkSuccess(cur.getLongest(4),3)
	checkSuccess(cur.getLongest('testreal'),3)
	checkSuccess(cur.getLongest(5),5)
	checkSuccess(cur.getLongest('testtext'),5)
	checkSuccess(cur.getLongest(6),8)
	checkSuccess(cur.getLongest('testtime'),8)
	checkSuccess(cur.getLongest(7),1)
	checkSuccess(cur.getLongest('testuint'),1)
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
	checkSuccess(cur.getField(0,0),"char1")
	checkSuccess(cur.getField(0,1),"01-Jan-2001")
	checkSuccess(cur.getField(0,2),"1")
	checkSuccess(cur.getField(0,3),"1.00")
	checkSuccess(cur.getField(0,4),"1.1")
	checkSuccess(cur.getField(0,5),"text1")
	checkSuccess(cur.getField(0,6),"01:00:00")
	checkSuccess(cur.getField(0,7),"1")
	print
	checkSuccess(cur.getField(7,0),"char8")
	checkSuccess(cur.getField(7,1),"01-Jan-2008")
	checkSuccess(cur.getField(7,2),"8")
	checkSuccess(cur.getField(7,3),"8.00")
	checkSuccess(cur.getField(7,4),"8.1")
	checkSuccess(cur.getField(7,5),"text8")
	checkSuccess(cur.getField(7,6),"08:00:00")
	checkSuccess(cur.getField(7,7),"8")
	print

	print "FIELD LENGTHS BY INDEX: "
	checkSuccess(cur.getFieldLength(0,0),5)
	checkSuccess(cur.getFieldLength(0,1),11)
	checkSuccess(cur.getFieldLength(0,2),1)
	checkSuccess(cur.getFieldLength(0,3),4)
	checkSuccess(cur.getFieldLength(0,4),3)
	checkSuccess(cur.getFieldLength(0,5),5)
	checkSuccess(cur.getFieldLength(0,6),8)
	checkSuccess(cur.getFieldLength(0,7),1)
	print
	checkSuccess(cur.getFieldLength(7,0),5)
	checkSuccess(cur.getFieldLength(7,1),11)
	checkSuccess(cur.getFieldLength(7,2),1)
	checkSuccess(cur.getFieldLength(7,3),4)
	checkSuccess(cur.getFieldLength(7,4),3)
	checkSuccess(cur.getFieldLength(7,5),5)
	checkSuccess(cur.getFieldLength(7,6),8)
	checkSuccess(cur.getFieldLength(7,7),1)
	print

	print "FIELDS BY NAME: "
	checkSuccess(cur.getField(0,"testchar"),"char1")
	checkSuccess(cur.getField(0,"testdate"),"01-Jan-2001")
	checkSuccess(cur.getField(0,"testint"),"1")
	checkSuccess(cur.getField(0,"testmoney"),"1.00")
	checkSuccess(cur.getField(0,"testreal"),"1.1")
	checkSuccess(cur.getField(0,"testtext"),"text1")
	checkSuccess(cur.getField(0,"testtime"),"01:00:00")
	checkSuccess(cur.getField(0,"testuint"),"1")
	print
	checkSuccess(cur.getField(7,"testchar"),"char8")
	checkSuccess(cur.getField(7,"testdate"),"01-Jan-2008")
	checkSuccess(cur.getField(7,"testint"),"8")
	checkSuccess(cur.getField(7,"testmoney"),"8.00")
	checkSuccess(cur.getField(7,"testreal"),"8.1")
	checkSuccess(cur.getField(7,"testtext"),"text8")
	checkSuccess(cur.getField(7,"testtime"),"08:00:00")
	checkSuccess(cur.getField(7,"testuint"),"8")
	print

	print "FIELD LENGTHS BY NAME: "
	checkSuccess(cur.getFieldLength(0,"testchar"),5)
	checkSuccess(cur.getFieldLength(0,"testdate"),11)
	checkSuccess(cur.getFieldLength(0,"testint"),1)
	checkSuccess(cur.getFieldLength(0,"testmoney"),4)
	checkSuccess(cur.getFieldLength(0,"testreal"),3)
	checkSuccess(cur.getFieldLength(0,"testtext"),5)
	checkSuccess(cur.getFieldLength(0,"testtime"),8)
	checkSuccess(cur.getFieldLength(0,"testuint"),1)
	print
	checkSuccess(cur.getFieldLength(7,"testchar"),5)
	checkSuccess(cur.getFieldLength(7,"testdate"),11)
	checkSuccess(cur.getFieldLength(7,"testint"),1)
	checkSuccess(cur.getFieldLength(7,"testmoney"),4)
	checkSuccess(cur.getFieldLength(7,"testreal"),3)
	checkSuccess(cur.getFieldLength(7,"testtext"),5)
	checkSuccess(cur.getFieldLength(7,"testtime"),8)
	checkSuccess(cur.getFieldLength(7,"testuint"),1)
	print

	print "FIELDS BY ARRAY: "
	fields=cur.getRow(0)
	checkSuccess(fields[0],"char1")
	checkSuccess(fields[1],"01-Jan-2001")
	checkSuccess(fields[2],1)
	checkSuccess(fields[3],1.00)
	checkSuccess(fields[4],1.1)
	checkSuccess(fields[5],"text1")
	checkSuccess(fields[6],"01:00:00")
	checkSuccess(fields[7],1)
	print

	print "FIELD LENGTHS BY ARRAY: "
	fieldlens=cur.getRowLengths(0)
	checkSuccess(fieldlens[0],5)
	checkSuccess(fieldlens[1],11)
	checkSuccess(fieldlens[2],1)
	checkSuccess(fieldlens[3],4)
	checkSuccess(fieldlens[4],3)
	checkSuccess(fieldlens[5],5)
	checkSuccess(fieldlens[6],8)
	checkSuccess(fieldlens[7],1)
	print

	print "FIELDS BY DICTIONARY: "
	fields=cur.getRowDictionary(0)
	checkSuccess(fields["testchar"],"char1")
	checkSuccess(fields["testdate"],"01-Jan-2001")
	checkSuccess(fields["testint"],1)
	checkSuccess(fields["testmoney"],1.00)
	checkSuccess(fields["testreal"],1.1)
	checkSuccess(fields["testtext"],"text1")
	checkSuccess(fields["testtime"],"01:00:00")
	checkSuccess(fields["testuint"],1)
	print
	fields=cur.getRowDictionary(7)
	checkSuccess(fields["testchar"],"char8")
	checkSuccess(fields["testdate"],"01-Jan-2008")
	checkSuccess(fields["testint"],8)
	checkSuccess(fields["testmoney"],8.00)
	checkSuccess(fields["testreal"],8.1)
	checkSuccess(fields["testtext"],"text8")
	checkSuccess(fields["testtime"],"08:00:00")
	checkSuccess(fields["testuint"],8)
	print

	print "FIELD LENGTHS BY DICTIONARY: "
	fieldlengths=cur.getRowLengthsDictionary(0)
	checkSuccess(fieldlengths["testchar"],5)
	checkSuccess(fieldlengths["testdate"],11)
	checkSuccess(fieldlengths["testint"],1)
	checkSuccess(fieldlengths["testmoney"],4)
	checkSuccess(fieldlengths["testreal"],3)
	checkSuccess(fieldlengths["testtext"],5)
	checkSuccess(fieldlengths["testtime"],8)
	checkSuccess(fieldlengths["testuint"],1)
	print
	fieldlengths=cur.getRowLengthsDictionary(7)
	checkSuccess(fieldlengths["testchar"],5)
	checkSuccess(fieldlengths["testdate"],11)
	checkSuccess(fieldlengths["testint"],1)
	checkSuccess(fieldlengths["testmoney"],4)
	checkSuccess(fieldlengths["testreal"],3)
	checkSuccess(fieldlengths["testtext"],5)
	checkSuccess(fieldlengths["testtime"],8)
	checkSuccess(fieldlengths["testuint"],1)
	print
	
	print "INDIVIDUAL SUBSTITUTIONS: "
	cur.sendQuery("drop table testtable1");
	checkSuccess(cur.sendQuery("create table testtable1 (col1 int, col2 char(40), col3 real)"),1)
	cur.prepareQuery("insert into testtable1 values ($(var1),'$(var2)',$(var3))")
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
	cur.prepareQuery("insert into testtable1 values ($(var1),'$(var2)',$(var3))")
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
	checkSuccess(cur.sendQuery("insert into testtable1 values (1,NULL,NULL)"),1)
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
	checkSuccess(cur.sendQuery("select * from testtable order by testint"),1)
	checkSuccess(cur.getResultSetBufferSize(),2)
	print
	checkSuccess(cur.firstRowIndex(),0)
	checkSuccess(cur.endOfResultSet(),0)
	checkSuccess(cur.rowCount(),2)
	checkSuccess(cur.getField(0,0),"char1")
	checkSuccess(cur.getField(1,0),"char2")
	checkSuccess(cur.getField(2,0),"char3")
	print
	checkSuccess(cur.firstRowIndex(),2)
	checkSuccess(cur.endOfResultSet(),0)
	checkSuccess(cur.rowCount(),4)
	checkSuccess(cur.getField(6,0),"char7")
	checkSuccess(cur.getField(7,0),"char8")
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
	checkSuccess(cur.sendQuery("select * from testtable order by testint"),1)
	checkSuccess(cur.getColumnName(0),None)
	checkSuccess(cur.getColumnLength(0),0)
	checkSuccess(cur.getColumnType(0),None)
	cur.getColumnInfo()
	checkSuccess(cur.sendQuery("select * from testtable order by testint"),1)
	checkSuccess(cur.getColumnName(0),"testchar")
	checkSuccess(cur.getColumnLength(0),40)
	checkSuccess(cur.getColumnType(0),"CHAR")
	print

	print "SUSPENDED SESSION: "
	checkSuccess(cur.sendQuery("select * from testtable order by testint"),1)
	cur.suspendResultSet()
	checkSuccess(con.suspendSession(),1)
	port=con.getConnectionPort()
	socket=con.getConnectionSocket()
	checkSuccess(con.resumeSession(port,socket),1)
	print
	checkSuccess(cur.getField(0,0),"char1")
	checkSuccess(cur.getField(1,0),"char2")
	checkSuccess(cur.getField(2,0),"char3")
	checkSuccess(cur.getField(3,0),"char4")
	checkSuccess(cur.getField(4,0),"char5")
	checkSuccess(cur.getField(5,0),"char6")
	checkSuccess(cur.getField(6,0),"char7")
	checkSuccess(cur.getField(7,0),"char8")
	print
	checkSuccess(cur.sendQuery("select * from testtable order by testint"),1)
	cur.suspendResultSet()
	checkSuccess(con.suspendSession(),1)
	port=con.getConnectionPort()
	socket=con.getConnectionSocket()
	checkSuccess(con.resumeSession(port,socket),1)
	print
	checkSuccess(cur.getField(0,0),"char1")
	checkSuccess(cur.getField(1,0),"char2")
	checkSuccess(cur.getField(2,0),"char3")
	checkSuccess(cur.getField(3,0),"char4")
	checkSuccess(cur.getField(4,0),"char5")
	checkSuccess(cur.getField(5,0),"char6")
	checkSuccess(cur.getField(6,0),"char7")
	checkSuccess(cur.getField(7,0),"char8")
	print
	checkSuccess(cur.sendQuery("select * from testtable order by testint"),1)
	cur.suspendResultSet()
	checkSuccess(con.suspendSession(),1)
	port=con.getConnectionPort()
	socket=con.getConnectionSocket()
	checkSuccess(con.resumeSession(port,socket),1)
	print
	checkSuccess(cur.getField(0,0),"char1")
	checkSuccess(cur.getField(1,0),"char2")
	checkSuccess(cur.getField(2,0),"char3")
	checkSuccess(cur.getField(3,0),"char4")
	checkSuccess(cur.getField(4,0),"char5")
	checkSuccess(cur.getField(5,0),"char6")
	checkSuccess(cur.getField(6,0),"char7")
	checkSuccess(cur.getField(7,0),"char8")
	print

	print "SUSPENDED RESULT SET: "
	cur.setResultSetBufferSize(2)
	checkSuccess(cur.sendQuery("select * from testtable order by testint"),1)
	checkSuccess(cur.getField(2,0),"char3")
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
	checkSuccess(cur.getField(7,0),"char8")
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
	checkSuccess(cur.sendQuery("select * from testtable order by testint"),1)
	filename=cur.getCacheFileName()
	checkSuccess(filename,"cachefile1")
	cur.cacheOff()
	checkSuccess(cur.openCachedResultSet(filename),1)
	checkSuccess(cur.getField(7,0),"char8")
	print

	print "COLUMN COUNT FOR CACHED RESULT SET: "
	checkSuccess(cur.colCount(),8)
	print

	print "COLUMN NAMES FOR CACHED RESULT SET: "
	checkSuccess(cur.getColumnName(0),"testchar")
	checkSuccess(cur.getColumnName(1),"testdate")
	checkSuccess(cur.getColumnName(2),"testint")
	checkSuccess(cur.getColumnName(3),"testmoney")
	checkSuccess(cur.getColumnName(4),"testreal")
	checkSuccess(cur.getColumnName(5),"testtext")
	checkSuccess(cur.getColumnName(6),"testtime")
	checkSuccess(cur.getColumnName(7),"testuint")
	cols=cur.getColumnNames()
	checkSuccess(cols[0],"testchar")
	checkSuccess(cols[1],"testdate")
	checkSuccess(cols[2],"testint")
	checkSuccess(cols[3],"testmoney")
	checkSuccess(cols[4],"testreal")
	checkSuccess(cols[5],"testtext")
	checkSuccess(cols[6],"testtime")
	checkSuccess(cols[7],"testuint")
	print

	print "CACHED RESULT SET WITH RESULT SET BUFFER SIZE: "
	cur.setResultSetBufferSize(2)
	cur.cacheToFile("cachefile1")
	cur.setCacheTtl(200)
	checkSuccess(cur.sendQuery("select * from testtable order by testint"),1)
	filename=cur.getCacheFileName()
	checkSuccess(filename,"cachefile1")
	cur.cacheOff()
	checkSuccess(cur.openCachedResultSet(filename),1)
	checkSuccess(cur.getField(7,0),"char8")
	checkSuccess(cur.getField(8,0),None)
	cur.setResultSetBufferSize(0)
	print

	print "FROM ONE CACHE FILE TO ANOTHER: "
	cur.cacheToFile("cachefile2")
	checkSuccess(cur.openCachedResultSet("cachefile1"),1)
	cur.cacheOff()
	checkSuccess(cur.openCachedResultSet("cachefile2"),1)
	checkSuccess(cur.getField(7,0),"char8")
	checkSuccess(cur.getField(8,0),None)
	print

	print "FROM ONE CACHE FILE TO ANOTHER WITH RESULT SET BUFFER SIZE: "
	cur.setResultSetBufferSize(2)
	cur.cacheToFile("cachefile2")
	checkSuccess(cur.openCachedResultSet("cachefile1"),1)
	cur.cacheOff()
	checkSuccess(cur.openCachedResultSet("cachefile2"),1)
	checkSuccess(cur.getField(7,0),"char8")
	checkSuccess(cur.getField(8,0),None)
	cur.setResultSetBufferSize(0)
	print

	print "CACHED RESULT SET WITH SUSPEND AND RESULT SET BUFFER SIZE: "
	cur.setResultSetBufferSize(2)
	cur.cacheToFile("cachefile1")
	cur.setCacheTtl(200)
	checkSuccess(cur.sendQuery("select * from testtable order by testint"),1)
	checkSuccess(cur.getField(2,0),"char3")
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
	checkSuccess(cur.getField(7,0),"char8")
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
	checkSuccess(cur.getField(7,0),"char8")
	checkSuccess(cur.getField(8,0),None)
	cur.setResultSetBufferSize(0)
	print

	print "COMMIT AND ROLLBACK: "
	secondcon=PySQLRClient.sqlrconnection(sys.argv[1],
				string.atoi(sys.argv[2]), 
				sys.argv[3],sys.argv[4],sys.argv[5])
	secondcur=PySQLRClient.sqlrcursor(secondcon)
	checkSuccess(secondcur.sendQuery("select * from testtable order by testint"),1)
	checkSuccess(secondcur.getField(0,0),"char1")
	checkSuccess(con.commit(),1)
	checkSuccess(secondcur.sendQuery("select * from testtable order by testint"),1)
	checkSuccess(secondcur.getField(0,0),"char1")
	checkSuccess(con.autoCommitOn(),1)
	checkSuccess(cur.sendQuery("insert into testtable values ('char10','01-Jan-2010',10,10.00,10.1,'text10','10:00:00',10)"),1)
	checkSuccess(secondcur.sendQuery("select * from testtable order by testint"),1)
	checkSuccess(secondcur.getField(8,0),"char10")
	checkSuccess(con.autoCommitOff(),1)
	print

	print "ROW RANGE:"
	checkSuccess(cur.sendQuery("select * from testtable order by testint"),1)
	print
	rows=cur.getRowRange(0,5)
	checkSuccess(rows[0][0],"char1");
	checkSuccess(rows[0][1],"01-Jan-2001");
	checkSuccess(rows[0][2],1)
	checkSuccess(rows[0][3],1.00)
	checkSuccess(rows[0][4],1.1)
	checkSuccess(rows[0][5],"text1")
	checkSuccess(rows[0][6],"01:00:00")
	checkSuccess(rows[0][7],1)
	print
	checkSuccess(rows[1][0],"char2");
	checkSuccess(rows[1][1],"01-Jan-2002");
	checkSuccess(rows[1][2],2)
	checkSuccess(rows[1][3],2.00)
	checkSuccess(rows[1][4],2.1)
	checkSuccess(rows[1][5],"text2")
	checkSuccess(rows[1][6],"02:00:00")
	checkSuccess(rows[1][7],2)
	print
	checkSuccess(rows[2][0],"char3");
	checkSuccess(rows[2][1],"01-Jan-2003");
	checkSuccess(rows[2][2],3)
	checkSuccess(rows[2][3],3.00)
	checkSuccess(rows[2][4],3.1)
	checkSuccess(rows[2][5],"text3")
	checkSuccess(rows[2][6],"03:00:00")
	checkSuccess(rows[2][7],3)
	print
	checkSuccess(rows[3][0],"char4");
	checkSuccess(rows[3][1],"01-Jan-2004");
	checkSuccess(rows[3][2],4)
	checkSuccess(rows[3][3],4.00)
	checkSuccess(rows[3][4],4.1)
	checkSuccess(rows[3][5],"text4")
	checkSuccess(rows[3][6],"04:00:00")
	checkSuccess(rows[3][7],4)
	print
	checkSuccess(rows[4][0],"char5");
	checkSuccess(rows[4][1],"01-Jan-2005");
	checkSuccess(rows[4][2],5)
	checkSuccess(rows[4][3],5.00)
	checkSuccess(rows[4][4],5.1)
	checkSuccess(rows[4][5],"text5")
	checkSuccess(rows[4][6],"05:00:00")
	checkSuccess(rows[4][7],5)
	print
	checkSuccess(rows[5][0],"char6");
	checkSuccess(rows[5][1],"01-Jan-2006");
	checkSuccess(rows[5][2],6)
	checkSuccess(rows[5][3],6.00)
	checkSuccess(rows[5][4],6.1)
	checkSuccess(rows[5][5],"text6")
	checkSuccess(rows[5][6],"06:00:00")
	checkSuccess(rows[5][7],6)
	print

	print "FINISHED SUSPENDED SESSION: "
	checkSuccess(cur.sendQuery("select * from testtable order by testint"),1)
	checkSuccess(cur.getField(4,2),"5")
	checkSuccess(cur.getField(5,2),"6")
	checkSuccess(cur.getField(6,2),"7")
	checkSuccess(cur.getField(7,2),"8")
	id=cur.getResultSetId()
	cur.suspendResultSet()
	checkSuccess(con.suspendSession(),1)
	port=con.getConnectionPort()
	socket=con.getConnectionSocket()
	checkSuccess(con.resumeSession(port,socket),1)
	checkSuccess(cur.resumeResultSet(id),1)
	checkSuccess(cur.getField(4,2),None)
	checkSuccess(cur.getField(5,2),None)
	checkSuccess(cur.getField(6,2),None)
	checkSuccess(cur.getField(7,2),None)
	print

	# drop existing table
	cur.sendQuery("drop table testtable")

	# invalid queries...
	print "INVALID QUERIES: "
	checkSuccess(cur.sendQuery("select * from testtable order by testint"),0)
	checkSuccess(cur.sendQuery("select * from testtable order by testint"),0)
	checkSuccess(cur.sendQuery("select * from testtable order by testint"),0)
	checkSuccess(cur.sendQuery("select * from testtable order by testint"),0)
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
