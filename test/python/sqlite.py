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
		print "usage: sqlite.py host port socket user password"
		sys.exit(0)

	# instantiation
	con=PySQLRClient.sqlrconnection(sys.argv[1],string.atoi(sys.argv[2]), 
					sys.argv[3],sys.argv[4],sys.argv[5])
	cur=PySQLRClient.sqlrcursor(con)

	# get database type
	print "IDENTIFY: "
	checkSuccess(con.identify(),"sqlite")
	print

	# ping
	print "PING: "
	checkSuccess(con.ping(),1)
	print

	# drop existing table
	cur.sendQuery("begin transaction")
	cur.sendQuery("drop table testtable")
	con.commit()

	# create a new table
	print "CREATE TEMPTABLE: "
	cur.sendQuery("begin transaction")
	checkSuccess(cur.sendQuery("create table testtable (testint int, testfloat float, testchar char(40), testvarchar varchar(40))"),1)
	con.commit()
	print

	print "INSERT: "
	cur.sendQuery("begin transaction")
	checkSuccess(cur.sendQuery("insert into testtable values (1,1.1,'testchar1','testvarchar1')"),1)
	checkSuccess(cur.sendQuery("insert into testtable values (2,2.2,'testchar2','testvarchar2')"),1)
	checkSuccess(cur.sendQuery("insert into testtable values (3,3.3,'testchar3','testvarchar3')"),1)
	checkSuccess(cur.sendQuery("insert into testtable values (4,4.4,'testchar4','testvarchar4')"),1)
	print

	print "AFFECTED ROWS: "
	checkSuccess(cur.affectedRows(),0)
	print

	print "BIND BY NAME: "
	cur.prepareQuery("insert into testtable values (:var1,:var2,:var3,:var4)")
	checkSuccess(cur.countBindVariables(),4)
	cur.inputBind("var1",5)
	cur.inputBind("var2",5.5,4,1)
	cur.inputBind("var3","testchar5")
	cur.inputBind("var4","testvarchar5")
	checkSuccess(cur.executeQuery(),1)
	cur.clearBinds()
	cur.inputBind("var1",6)
	cur.inputBind("var2",6.6,4,1)
	cur.inputBind("var3","testchar6")
	cur.inputBind("var4","testvarchar6")
	checkSuccess(cur.executeQuery(),1)
	print

	print "ARRAY OF BINDS BY NAME: "
	cur.clearBinds()
	cur.inputBinds(["var1","var2","var3","var4"],
			[7,7.7,"testchar7","testvarchar7"],[0,4,0,0],[0,1,0,0])
	checkSuccess(cur.executeQuery(),1)
	print

	print "BIND BY NAME WITH VALIDATION: "
	cur.clearBinds()
	cur.inputBind("var1",8)
	cur.inputBind("var2",8.8,4,1)
	cur.inputBind("var3","testchar8")
	cur.inputBind("var4","testvarchar8")
	cur.validateBinds()
	checkSuccess(cur.executeQuery(),1)
	print

	print "SELECT: "
	checkSuccess(cur.sendQuery("select * from testtable order by testint"),1)
	print

	print "COLUMN COUNT: "
	checkSuccess(cur.colCount(),4)
	print

	print "COLUMN NAMES: "
	checkSuccess(cur.getColumnName(0),"testint")
	checkSuccess(cur.getColumnName(1),"testfloat")
	checkSuccess(cur.getColumnName(2),"testchar")
	checkSuccess(cur.getColumnName(3),"testvarchar")
	cols=cur.getColumnNames()
	checkSuccess(cols[0],"testint")
	checkSuccess(cols[1],"testfloat")
	checkSuccess(cols[2],"testchar")
	checkSuccess(cols[3],"testvarchar")
	print

	print "COLUMN TYPES: "
	checkSuccess(cur.getColumnType(0),"UNKNOWN")
	checkSuccess(cur.getColumnType('testint'),"UNKNOWN")
	checkSuccess(cur.getColumnType(1),"UNKNOWN")
	checkSuccess(cur.getColumnType('testfloat'),"UNKNOWN")
	checkSuccess(cur.getColumnType(2),"UNKNOWN")
	checkSuccess(cur.getColumnType('testchar'),"UNKNOWN")
	checkSuccess(cur.getColumnType(3),"UNKNOWN")
	checkSuccess(cur.getColumnType('testvarchar'),"UNKNOWN")
	print

	print "COLUMN LENGTH: "
	checkSuccess(cur.getColumnLength(0),0)
	checkSuccess(cur.getColumnLength('testint'),0)
	checkSuccess(cur.getColumnLength(1),0)
	checkSuccess(cur.getColumnLength('testfloat'),0)
	checkSuccess(cur.getColumnLength(2),0)
	checkSuccess(cur.getColumnLength('testchar'),0)
	checkSuccess(cur.getColumnLength(3),0)
	checkSuccess(cur.getColumnLength('testvarchar'),0)
	print

	print "LONGEST COLUMN: "
	checkSuccess(cur.getLongest(0),1)
	checkSuccess(cur.getLongest('testint'),1)
	checkSuccess(cur.getLongest(1),3)
	checkSuccess(cur.getLongest('testfloat'),3)
	checkSuccess(cur.getLongest(2),9)
	checkSuccess(cur.getLongest('testchar'),9)
	checkSuccess(cur.getLongest(3),12)
	checkSuccess(cur.getLongest('testvarchar'),12)
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
	checkSuccess(cur.getField(0,1),"1.1")
	checkSuccess(cur.getField(0,2),"testchar1")
	checkSuccess(cur.getField(0,3),"testvarchar1")
	print
	checkSuccess(cur.getField(7,0),"8")
	checkSuccess(cur.getField(7,1),"8.8")
	checkSuccess(cur.getField(7,2),"testchar8")
	checkSuccess(cur.getField(7,3),"testvarchar8")
	print

	print "FIELD LENGTHS BY INDEX: "
	checkSuccess(cur.getFieldLength(0,0),1)
	checkSuccess(cur.getFieldLength(0,1),3)
	checkSuccess(cur.getFieldLength(0,2),9)
	checkSuccess(cur.getFieldLength(0,3),12)
	print
	checkSuccess(cur.getFieldLength(7,0),1)
	checkSuccess(cur.getFieldLength(7,1),3)
	checkSuccess(cur.getFieldLength(7,2),9)
	checkSuccess(cur.getFieldLength(7,3),12)
	print

	print "FIELDS BY NAME: "
	checkSuccess(cur.getField(0,"testint"),"1")
	checkSuccess(cur.getField(0,"testfloat"),"1.1")
	checkSuccess(cur.getField(0,"testchar"),"testchar1")
	checkSuccess(cur.getField(0,"testvarchar"),"testvarchar1")
	print
	checkSuccess(cur.getField(7,"testint"),"8")
	checkSuccess(cur.getField(7,"testfloat"),"8.8")
	checkSuccess(cur.getField(7,"testchar"),"testchar8")
	checkSuccess(cur.getField(7,"testvarchar"),"testvarchar8")
	print

	print "FIELD LENGTHS BY NAME: "
	checkSuccess(cur.getFieldLength(0,"testint"),1)
	checkSuccess(cur.getFieldLength(0,"testfloat"),3)
	checkSuccess(cur.getFieldLength(0,"testchar"),9)
	checkSuccess(cur.getFieldLength(0,"testvarchar"),12)
	print
	checkSuccess(cur.getFieldLength(7,"testint"),1)
	checkSuccess(cur.getFieldLength(7,"testfloat"),3)
	checkSuccess(cur.getFieldLength(7,"testchar"),9)
	checkSuccess(cur.getFieldLength(7,"testvarchar"),12)
	print

	print "FIELDS BY ARRAY: "
	fields=cur.getRow(0)
	checkSuccess(fields[0],"1")
	checkSuccess(fields[1],"1.1")
	checkSuccess(fields[2],"testchar1")
	checkSuccess(fields[3],"testvarchar1")
	print

	print "FIELD LENGTHS BY ARRAY: "
	fieldlens=cur.getRowLengths(0)
	checkSuccess(fieldlens[0],1)
	checkSuccess(fieldlens[1],3)
	checkSuccess(fieldlens[2],9)
	checkSuccess(fieldlens[3],12)
	print

	print "FIELDS BY DICTIONARY: "
	fields=cur.getRowDictionary(0)
	checkSuccess(fields["testint"],"1")
	checkSuccess(fields["testfloat"],"1.1")
	checkSuccess(fields["testchar"],"testchar1")
	checkSuccess(fields["testvarchar"],"testvarchar1")
	print
	fields=cur.getRowDictionary(7)
	checkSuccess(fields["testint"],"8")
	checkSuccess(fields["testfloat"],"8.8")
	checkSuccess(fields["testchar"],"testchar8")
	checkSuccess(fields["testvarchar"],"testvarchar8")
	print

	print "FIELD LENGTHS BY DICTIONARY: "
	fieldlengths=cur.getRowLengthsDictionary(0)
	checkSuccess(fieldlengths["testint"],1)
	checkSuccess(fieldlengths["testfloat"],3)
	checkSuccess(fieldlengths["testchar"],9)
	checkSuccess(fieldlengths["testvarchar"],12)
	print
	fieldlengths=cur.getRowLengthsDictionary(7)
	checkSuccess(fieldlengths["testint"],1)
	checkSuccess(fieldlengths["testfloat"],3)
	checkSuccess(fieldlengths["testchar"],9)
	checkSuccess(fieldlengths["testvarchar"],12)
	print
		
	print "INDIVIDUAL SUBSTITUTIONS: "
	cur.sendQuery("drop table testtable1")
	checkSuccess(cur.sendQuery("create table testtable1 (col1 int, col2 char, col3 float)"),1)
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
	checkSuccess(cur.sendQuery("select * from testtable order by testint"),1)
	checkSuccess(cur.getColumnName(0),None)
	checkSuccess(cur.getColumnLength(0),0)
	checkSuccess(cur.getColumnType(0),None)
	cur.getColumnInfo()
	checkSuccess(cur.sendQuery("select * from testtable order by testint"),1)
	checkSuccess(cur.getColumnName(0),"testint")
	checkSuccess(cur.getColumnLength(0),0)
	checkSuccess(cur.getColumnType(0),"UNKNOWN")
	print

	print "SUSPENDED SESSION: "
	checkSuccess(cur.sendQuery("select * from testtable order by testint"),1)
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
	checkSuccess(cur.sendQuery("select * from testtable order by testint"),1)
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
	checkSuccess(cur.sendQuery("select * from testtable order by testint"),1)
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
	checkSuccess(cur.sendQuery("select * from testtable order by testint"),1)
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
	checkSuccess(cur.sendQuery("select * from testtable order by testint"),1)
	filename=cur.getCacheFileName()
	checkSuccess(filename,"cachefile1")
	cur.cacheOff()
	checkSuccess(cur.openCachedResultSet(filename),1)
	checkSuccess(cur.getField(7,0),"8")
	print

	print "COLUMN COUNT FOR CACHED RESULT SET: "
	checkSuccess(cur.colCount(),4)
	print

	print "COLUMN NAMES FOR CACHED RESULT SET: "
	checkSuccess(cur.getColumnName(0),"testint")
	checkSuccess(cur.getColumnName(1),"testfloat")
	checkSuccess(cur.getColumnName(2),"testchar")
	checkSuccess(cur.getColumnName(3),"testvarchar")
	cols=cur.getColumnNames()
	checkSuccess(cols[0],"testint")
	checkSuccess(cols[1],"testfloat")
	checkSuccess(cols[2],"testchar")
	checkSuccess(cols[3],"testvarchar")
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
	checkSuccess(cur.sendQuery("select * from testtable order by testint"),1)
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

	print "ROW RANGE:"
	checkSuccess(cur.sendQuery("select * from testtable order by testint"),1)
	print
	rows=cur.getRowRange(0,5)
	checkSuccess(rows[0][0],"1")
	checkSuccess(rows[0][1],"1.1")
	checkSuccess(rows[0][2],"testchar1")
	checkSuccess(rows[0][3],"testvarchar1")
	print
	checkSuccess(rows[1][0],"2")
	checkSuccess(rows[1][1],"2.2")
	checkSuccess(rows[1][2],"testchar2")
	checkSuccess(rows[1][3],"testvarchar2")
	print
	checkSuccess(rows[2][0],"3")
	checkSuccess(rows[2][1],"3.3")
	checkSuccess(rows[2][2],"testchar3")
	checkSuccess(rows[2][3],"testvarchar3")
	print
	checkSuccess(rows[3][0],"4")
	checkSuccess(rows[3][1],"4.4")
	checkSuccess(rows[3][2],"testchar4")
	checkSuccess(rows[3][3],"testvarchar4")
	print
	checkSuccess(rows[4][0],"5")
	checkSuccess(rows[4][1],"5.5")
	checkSuccess(rows[4][2],"testchar5")
	checkSuccess(rows[4][3],"testvarchar5")
	print
	checkSuccess(rows[5][0],"6")
	checkSuccess(rows[5][1],"6.6")
	checkSuccess(rows[5][2],"testchar6")
	checkSuccess(rows[5][3],"testvarchar6")
	print

	print "COMMIT AND ROLLBACK: "
	secondcon=PySQLRClient.sqlrconnection(sys.argv[1],
				string.atoi(sys.argv[2]), 
				sys.argv[3],sys.argv[4],sys.argv[5],0,1)
	secondcur=PySQLRClient.sqlrcursor(secondcon)
	checkSuccess(secondcur.sendQuery("select count(*) from testtable"),0)
	checkSuccess(con.commit(),1)
	checkSuccess(secondcur.sendQuery("select count(*) from testtable"),1)
	checkSuccess(secondcur.getField(0,0),"8")
	checkSuccess(cur.sendQuery("insert into testtable values (10,10.1,'testchar10','testvarchar10')"),1)
	checkSuccess(secondcur.sendQuery("select count(*) from testtable"),1)
	checkSuccess(secondcur.getField(0,0),"9")
	print

	print "FINISHED SUSPENDED SESSION: "
	checkSuccess(cur.sendQuery("select * from testtable order by testint"),1)
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
