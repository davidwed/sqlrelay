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
		print "usage: oracle8.py host port socket user password"
		sys.exit(0)


	# instantiation
	con=PySQLRClient.sqlrconnection(sys.argv[1],string.atoi(sys.argv[2]), 
					sys.argv[3],sys.argv[4],sys.argv[5])
	cur=PySQLRClient.sqlrcursor(con)

	# get database type
	print "IDENTIFY: "
	checkSuccess(con.identify(),"oracle8")
	print

	# ping
	print "PING: "
	checkSuccess(con.ping(),1)
	print

	# drop existing table
	cur.sendQuery("drop table testtable")

	print "CREATE TEMPTABLE: "
	checkSuccess(cur.sendQuery("create table testtable (testnumber number, testchar char(40), testvarchar varchar2(40), testdate date, testlong long)"),1)
	print

	print "INSERT: "
	checkSuccess(cur.sendQuery("insert into testtable values (1,'testchar1','testvarchar1','01-JAN-2001','testlong1')"),1)
	print

	print "AFFECTED ROWS: "
	checkSuccess(cur.affectedRows(),1)
	print

	print "BIND BY POSITION: "
	cur.prepareQuery("insert into testtable values (:var1,:var2,:var3,:var4,:var5)")
	cur.inputBind("1",2)
	cur.inputBind("2","testchar2")
	cur.inputBind("3","testvarchar2")
	cur.inputBind("4","01-JAN-2002")
	cur.inputBind("5","testlong2")
	checkSuccess(cur.executeQuery(),1)
	cur.clearBinds()
	cur.inputBind("1",3)
	cur.inputBind("2","testchar3")
	cur.inputBind("3","testvarchar3")
	cur.inputBind("4","01-JAN-2003")
	cur.inputBind("5","testlong3")
	checkSuccess(cur.executeQuery(),1)
	print

	print "ARRAY OF BINDS BY POSITION: "
	cur.clearBinds()
	cur.inputBinds(["1","2","3","4","5"],
		[4,"testchar4","testvarchar4",
			"01-JAN-2004","testlong4"])
	checkSuccess(cur.executeQuery(),1)
	print

	print "BIND BY NAME: "
	cur.prepareQuery("insert into testtable values (:var1,:var2,:var3,:var4,:var5)")
	cur.inputBind("var1",5)
	cur.inputBind("var2","testchar5")
	cur.inputBind("var3","testvarchar5")
	cur.inputBind("var4","01-JAN-2005")
	cur.inputBind("var5","testlong5")
	checkSuccess(cur.executeQuery(),1)
	cur.clearBinds()
	cur.inputBind("var1",6)
	cur.inputBind("var2","testchar6")
	cur.inputBind("var3","testvarchar6")
	cur.inputBind("var4","01-JAN-2006")
	cur.inputBind("var5","testlong6")
	checkSuccess(cur.executeQuery(),1)
	print

	print "ARRAY OF BINDS BY NAME: "
	cur.clearBinds()
	cur.inputBinds(["var1","var2","var3","var4","var5"],
		[7,"testchar7","testvarchar7",
			"01-JAN-2007","testlong7"])
	checkSuccess(cur.executeQuery(),1)
	print

	print "BIND BY NAME WITH VALIDATION: "
	cur.clearBinds()
	cur.inputBind("var1",8)
	cur.inputBind("var2","testchar8")
	cur.inputBind("var3","testvarchar8")
	cur.inputBind("var4","01-JAN-2008")
	cur.inputBind("var5","testlong8")
	cur.inputBind("var6","junkvalue")
	cur.validateBinds()
	checkSuccess(cur.executeQuery(),1)
	print

	print "OUTPUT BIND BY NAME: "
	cur.prepareQuery("begin  :numvar:=1; :stringvar:='hello'; :floatvar:=2.5; end;")
	cur.defineOutputBind("numvar",10)
	cur.defineOutputBind("stringvar",10)
	cur.defineOutputBind("floatvar",10)
	checkSuccess(cur.executeQuery(),1)
	numvar=cur.getOutputBind("numvar")
	stringvar=cur.getOutputBind("stringvar")
	floatvar=cur.getOutputBind("floatvar")
	checkSuccess(numvar,'1')
	checkSuccess(stringvar,'hello')
	checkSuccess(floatvar,'2.5')
	print

	print "OUTPUT BIND BY NAME: "
	cur.clearBinds()
	cur.defineOutputBind("1",10)
	cur.defineOutputBind("2",10)
	cur.defineOutputBind("3",10)
	checkSuccess(cur.executeQuery(),1)
	numvar=cur.getOutputBind("1")
	stringvar=cur.getOutputBind("2")
	floatvar=cur.getOutputBind("3")
	checkSuccess(numvar,'1')
	checkSuccess(stringvar,'hello')
	checkSuccess(floatvar,'2.5')
	print

	print "OUTPUT BIND BY NAME WITH VALIDATION: "
	cur.clearBinds()
	cur.defineOutputBind("numvar",10)
	cur.defineOutputBind("stringvar",10)
	cur.defineOutputBind("floatvar",10)
	cur.defineOutputBind("dummyvar",10)
	cur.validateBinds()
	checkSuccess(cur.executeQuery(),1)
	numvar=cur.getOutputBind("numvar")
	stringvar=cur.getOutputBind("stringvar")
	floatvar=cur.getOutputBind("floatvar")
	checkSuccess(numvar,'1')
	checkSuccess(stringvar,'hello')
	checkSuccess(floatvar,'2.5')
	print

	print "SELECT: "
	checkSuccess(cur.sendQuery("select * from testtable order by testnumber"),1)
	print

	print "COLUMN COUNT: "
	checkSuccess(cur.colCount(),5)
	print

	print "COLUMN NAMES: "
	checkSuccess(cur.getColumnName(0),"TESTNUMBER")
	checkSuccess(cur.getColumnName(1),"TESTCHAR")
	checkSuccess(cur.getColumnName(2),"TESTVARCHAR")
	checkSuccess(cur.getColumnName(3),"TESTDATE")
	checkSuccess(cur.getColumnName(4),"TESTLONG")
	cols=cur.getColumnNames()
	checkSuccess(cols[0],"TESTNUMBER")
	checkSuccess(cols[1],"TESTCHAR")
	checkSuccess(cols[2],"TESTVARCHAR")
	checkSuccess(cols[3],"TESTDATE")
	checkSuccess(cols[4],"TESTLONG")
	print

	print "COLUMN TYPES: "
	checkSuccess(cur.getColumnType(0),"NUMBER")
	checkSuccess(cur.getColumnType('testnumber'),"NUMBER")
	checkSuccess(cur.getColumnType(1),"CHAR")
	checkSuccess(cur.getColumnType('testchar'),"CHAR")
	checkSuccess(cur.getColumnType(2),"VARCHAR2")
	checkSuccess(cur.getColumnType('testvarchar'),"VARCHAR2")
	checkSuccess(cur.getColumnType(3),"DATE")
	checkSuccess(cur.getColumnType('testdate'),"DATE")
	checkSuccess(cur.getColumnType(4),"LONG")
	checkSuccess(cur.getColumnType('testlong'),"LONG")
	print

	print "COLUMN LENGTH: "
	checkSuccess(cur.getColumnLength(0),22)
	checkSuccess(cur.getColumnLength('testnumber'),22)
	checkSuccess(cur.getColumnLength(1),40)
	checkSuccess(cur.getColumnLength('testchar'),40)
	checkSuccess(cur.getColumnLength(2),40)
	checkSuccess(cur.getColumnLength('testvarchar'),40)
	checkSuccess(cur.getColumnLength(3),7)
	checkSuccess(cur.getColumnLength('testdate'),7)
	checkSuccess(cur.getColumnLength(4),0)
	checkSuccess(cur.getColumnLength('testlong'),0)
	print

	print "LONGEST COLUMN: "
	checkSuccess(cur.getLongest(0),1)
	checkSuccess(cur.getLongest('testnumber'),1)
	checkSuccess(cur.getLongest(1),40)
	checkSuccess(cur.getLongest('testchar'),40)
	checkSuccess(cur.getLongest(2),12)
	checkSuccess(cur.getLongest('testvarchar'),12)
	checkSuccess(cur.getLongest(3),9)
	checkSuccess(cur.getLongest('testdate'),9)
	checkSuccess(cur.getLongest(4),9)
	checkSuccess(cur.getLongest('testlong'),9)
	print

	print "ROW COUNT: "
	checkSuccess(cur.rowCount(),8)
	print

	print "TOTAL ROWS: "
	checkSuccess(cur.totalRows(),-1)
	print

	print "FIRST ROW INDEX: "
	checkSuccess(cur.firstRowIndex(),0)
	print

	print "END OF RESULT SET: "
	checkSuccess(cur.endOfResultSet(),1)
	print

	print "FIELDS BY INDEX: "
	checkSuccess(cur.getField(0,0),"1")
	checkSuccess(cur.getField(0,1),"testchar1                               ")
	checkSuccess(cur.getField(0,2),"testvarchar1")
	checkSuccess(cur.getField(0,3),"01-JAN-01")
	checkSuccess(cur.getField(0,4),"testlong1")
	print
	checkSuccess(cur.getField(7,0),"8")
	checkSuccess(cur.getField(7,1),"testchar8                               ")
	checkSuccess(cur.getField(7,2),"testvarchar8")
	checkSuccess(cur.getField(7,3),"01-JAN-08")
	checkSuccess(cur.getField(7,4),"testlong8")
	print

	print "FIELD LENGTHS BY INDEX: "
	checkSuccess(cur.getFieldLength(0,0),1)
	checkSuccess(cur.getFieldLength(0,1),40)
	checkSuccess(cur.getFieldLength(0,2),12)
	checkSuccess(cur.getFieldLength(0,3),9)
	checkSuccess(cur.getFieldLength(0,4),9)
	print
	checkSuccess(cur.getFieldLength(7,0),1)
	checkSuccess(cur.getFieldLength(7,1),40)
	checkSuccess(cur.getFieldLength(7,2),12)
	checkSuccess(cur.getFieldLength(7,3),9)
	checkSuccess(cur.getFieldLength(7,4),9)
	print

	print "FIELDS BY NAME: "
	checkSuccess(cur.getField(0,"testnumber"),"1")
	checkSuccess(cur.getField(0,"testchar"),"testchar1                               ")
	checkSuccess(cur.getField(0,"testvarchar"),"testvarchar1")
	checkSuccess(cur.getField(0,"testdate"),"01-JAN-01")
	checkSuccess(cur.getField(0,"testlong"),"testlong1")
	print
	checkSuccess(cur.getField(7,"testnumber"),"8")
	checkSuccess(cur.getField(7,"testchar"),"testchar8                               ")
	checkSuccess(cur.getField(7,"testvarchar"),"testvarchar8")
	checkSuccess(cur.getField(7,"testdate"),"01-JAN-08")
	checkSuccess(cur.getField(7,"testlong"),"testlong8")
	print

	print "FIELD LENGTHS BY NAME: "
	checkSuccess(cur.getFieldLength(0,"testnumber"),1)
	checkSuccess(cur.getFieldLength(0,"testchar"),40)
	checkSuccess(cur.getFieldLength(0,"testvarchar"),12)
	checkSuccess(cur.getFieldLength(0,"testdate"),9)
	checkSuccess(cur.getFieldLength(0,"testlong"),9)
	print
	checkSuccess(cur.getFieldLength(7,"testnumber"),1)
	checkSuccess(cur.getFieldLength(7,"testchar"),40)
	checkSuccess(cur.getFieldLength(7,"testvarchar"),12)
	checkSuccess(cur.getFieldLength(7,"testdate"),9)
	checkSuccess(cur.getFieldLength(7,"testlong"),9)
	print

	print "FIELDS BY ARRAY: "
	fields=cur.getRow(0)
	checkSuccess(fields[0],1)
	checkSuccess(fields[1],"testchar1                               ")
	checkSuccess(fields[2],"testvarchar1")
	checkSuccess(fields[3],"01-JAN-01")
	checkSuccess(fields[4],"testlong1")
	print

	print "FIELD LENGTHS BY ARRAY: "
	fieldlens=cur.getRowLengths(0)
	checkSuccess(fieldlens[0],1)
	checkSuccess(fieldlens[1],40)
	checkSuccess(fieldlens[2],12)
	checkSuccess(fieldlens[3],9)
	checkSuccess(fieldlens[4],9)
	print

	print "FIELDS BY DICTIONARY: "
	fields=cur.getRowDictionary(0)
	checkSuccess(fields["TESTNUMBER"],1)
	checkSuccess(fields["TESTCHAR"],"testchar1                               ")
	checkSuccess(fields["TESTVARCHAR"],"testvarchar1")
	checkSuccess(fields["TESTDATE"],"01-JAN-01")
	checkSuccess(fields["TESTLONG"],"testlong1")
	print
	fields=cur.getRowDictionary(7)
	checkSuccess(fields["TESTNUMBER"],8)
	checkSuccess(fields["TESTCHAR"],"testchar8                               ")
	checkSuccess(fields["TESTVARCHAR"],"testvarchar8")
	checkSuccess(fields["TESTDATE"],"01-JAN-08")
	checkSuccess(fields["TESTLONG"],"testlong8")
	print

	print "FIELD LENGTHS BY DICTIONARY: "
	fieldlengths=cur.getRowLengthsDictionary(0)
	checkSuccess(fieldlengths["TESTNUMBER"],1)
	checkSuccess(fieldlengths["TESTCHAR"],40)
	checkSuccess(fieldlengths["TESTVARCHAR"],12)
	checkSuccess(fieldlengths["TESTDATE"],9)
	checkSuccess(fieldlengths["TESTLONG"],9)
	print
	fieldlengths=cur.getRowLengthsDictionary(7)
	checkSuccess(fieldlengths["TESTNUMBER"],1)
	checkSuccess(fieldlengths["TESTCHAR"],40)
	checkSuccess(fieldlengths["TESTVARCHAR"],12)
	checkSuccess(fieldlengths["TESTDATE"],9)
	checkSuccess(fieldlengths["TESTLONG"],9)
	print
	
	print "INDIVIDUAL SUBSTITUTIONS: "
	cur.prepareQuery("select $(var1),'$(var2)',$(var3) from dual")
	cur.substitution("var1",1)
	cur.substitution("var2","hello")
	cur.substitution("var3",10.5556,6,4)
	checkSuccess(cur.executeQuery(),1)
	print

	print "FIELDS: "
	checkSuccess(cur.getField(0,0),"1")
	checkSuccess(cur.getField(0,1),"hello")
	checkSuccess(cur.getField(0,2),"10.5556")
	print

	print "OUTPUT BIND: "
	cur.prepareQuery("begin :var1:='hello'; end;")
	cur.defineOutputBind("var1",10)
	checkSuccess(cur.executeQuery(),1)
	checkSuccess(cur.getOutputBind("var1"),"hello")
	print

	print "ARRAY SUBSTITUTIONS: "
	cur.prepareQuery("select $(var1),'$(var2)',$(var3) from dual")
	cur.substitutions(["var1","var2","var3"],
				[1,"hello",10.5556],[0,0,6],[0,0,4])
	checkSuccess(cur.executeQuery(),1)
	print

	print "FIELDS: "
	checkSuccess(cur.getField(0,0),"1")
	checkSuccess(cur.getField(0,1),"hello")
	checkSuccess(cur.getField(0,2),"10.5556")
	print

	print "NULLS as Nones: "
	cur.getNullsAsNone()
	checkSuccess(cur.sendQuery("select NULL,1,NULL from dual"),1)
	checkSuccess(cur.getField(0,0),None)
	checkSuccess(cur.getField(0,1),"1")
	checkSuccess(cur.getField(0,2),None)
	cur.getNullsAsEmptyStrings()
	checkSuccess(cur.sendQuery("select NULL,1,NULL from dual"),1)
	checkSuccess(cur.getField(0,0),"")
	checkSuccess(cur.getField(0,1),"1")
	checkSuccess(cur.getField(0,2),"")
	print

	print "RESULT SET BUFFER SIZE: "
	checkSuccess(cur.getResultSetBufferSize(),0)
	cur.setResultSetBufferSize(2)
	checkSuccess(cur.sendQuery("select * from testtable order by testnumber"),1)
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
	checkSuccess(cur.sendQuery("select * from testtable order by testnumber"),1)
	checkSuccess(cur.getColumnName(0),None)
	checkSuccess(cur.getColumnLength(0),0)
	checkSuccess(cur.getColumnType(0),None)
	cur.getColumnInfo()
	checkSuccess(cur.sendQuery("select * from testtable order by testnumber"),1)
	checkSuccess(cur.getColumnName(0),"TESTNUMBER")
	checkSuccess(cur.getColumnLength(0),22)
	checkSuccess(cur.getColumnType(0),"NUMBER")
	print

	print "SUSPENDED SESSION: "
	checkSuccess(cur.sendQuery("select * from testtable order by testnumber"),1)
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
	checkSuccess(cur.sendQuery("select * from testtable order by testnumber"),1)
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
	checkSuccess(cur.sendQuery("select * from testtable order by testnumber"),1)
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
	checkSuccess(cur.sendQuery("select * from testtable order by testnumber"),1)
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
	checkSuccess(cur.sendQuery("select * from testtable order by testnumber"),1)
	filename=cur.getCacheFileName()
	checkSuccess(filename,"cachefile1")
	cur.cacheOff()
	checkSuccess(cur.openCachedResultSet(filename),1)
	checkSuccess(cur.getField(7,0),"8")
	print

	print "COLUMN COUNT FOR CACHED RESULT SET: "
	checkSuccess(cur.colCount(),5)
	print

	print "COLUMN NAMES FOR CACHED RESULT SET: "
	checkSuccess(cur.getColumnName(0),"TESTNUMBER")
	checkSuccess(cur.getColumnName(1),"TESTCHAR")
	checkSuccess(cur.getColumnName(2),"TESTVARCHAR")
	checkSuccess(cur.getColumnName(3),"TESTDATE")
	checkSuccess(cur.getColumnName(4),"TESTLONG")
	cols=cur.getColumnNames()
	checkSuccess(cols[0],"TESTNUMBER")
	checkSuccess(cols[1],"TESTCHAR")
	checkSuccess(cols[2],"TESTVARCHAR")
	checkSuccess(cols[3],"TESTDATE")
	checkSuccess(cols[4],"TESTLONG")
	print

	print "CACHED RESULT SET WITH RESULT SET BUFFER SIZE: "
	cur.setResultSetBufferSize(2)
	cur.cacheToFile("cachefile1")
	cur.setCacheTtl(200)
	checkSuccess(cur.sendQuery("select * from testtable order by testnumber"),1)
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
	checkSuccess(cur.sendQuery("select * from testtable order by testnumber"),1)
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
	checkSuccess(secondcur.sendQuery("select count(*) from testtable"),1)
	checkSuccess(secondcur.getField(0,0),"0")
	checkSuccess(con.commit(),1)
	checkSuccess(secondcur.sendQuery("select count(*) from testtable"),1)
	checkSuccess(secondcur.getField(0,0),"8")
	checkSuccess(con.autoCommitOn(),1)
	checkSuccess(cur.sendQuery("insert into testtable values (10,'testchar10','testvarchar10','01-JAN-2010','testlong10')"),1)
	checkSuccess(secondcur.sendQuery("select count(*) from testtable"),1)
	checkSuccess(secondcur.getField(0,0),"9")
	checkSuccess(con.autoCommitOff(),1)
	print

	print "ROW RANGE:"
	checkSuccess(cur.sendQuery("select * from testtable order by testnumber"),1)
	print
	rows=cur.getRowRange(0,5)
	checkSuccess(rows[0][0],1);
	checkSuccess(rows[0][1],"testchar1                               ")
	checkSuccess(rows[0][2],"testvarchar1")
	checkSuccess(rows[0][3],"01-JAN-01")
	checkSuccess(rows[0][4],"testlong1")
	print
	checkSuccess(rows[1][0],2);
	checkSuccess(rows[1][1],"testchar2                               ")
	checkSuccess(rows[1][2],"testvarchar2")
	checkSuccess(rows[1][3],"01-JAN-02")
	checkSuccess(rows[1][4],"testlong2")
	print
	checkSuccess(rows[2][0],3);
	checkSuccess(rows[2][1],"testchar3                               ")
	checkSuccess(rows[2][2],"testvarchar3")
	checkSuccess(rows[2][3],"01-JAN-03")
	checkSuccess(rows[2][4],"testlong3")
	print
	checkSuccess(rows[3][0],4);
	checkSuccess(rows[3][1],"testchar4                               ")
	checkSuccess(rows[3][2],"testvarchar4")
	checkSuccess(rows[3][3],"01-JAN-04")
	checkSuccess(rows[3][4],"testlong4")
	print
	checkSuccess(rows[4][0],5);
	checkSuccess(rows[4][1],"testchar5                               ")
	checkSuccess(rows[4][2],"testvarchar5")
	checkSuccess(rows[4][3],"01-JAN-05")
	checkSuccess(rows[4][4],"testlong5")
	print
	checkSuccess(rows[5][0],6);
	checkSuccess(rows[5][1],"testchar6                               ")
	checkSuccess(rows[5][2],"testvarchar6")
	checkSuccess(rows[5][3],"01-JAN-06")
	checkSuccess(rows[5][4],"testlong6")
	print


	print "CLOB AND BLOB OUTPUT BIND: \n");
	cur.sendQuery("drop table testtable1");
	checkSuccess(cur.sendQuery("create table testtable1 (testclob clob, testblob blob)"),1);
	cur.prepareQuery("insert into testtable1 values ('hello',:var1)");
	cur.inputBindBlob("var1","hello",5);
	checkSuccess(cur.executeQuery(),1);
	cur.prepareQuery("begin select testclob into :clobvar from testtable1;  select testblob into :blobvar from testtable1; end;");
	cur.defineOutputBindClob("clobvar");
	cur.defineOutputBindBlob("blobvar");
	checkSuccess(cur.executeQuery(),1);
	clobvar=cur.getOutputBind("clobvar");
	clobvarlength=cur.getOutputBindLength("clobvar");
	blobvar=cur.getOutputBind("blobvar");
	blobvarlength=cur.getOutputBindLength("blobvar");
	checkSuccess(clobvar,"hello",5);
	checkSuccess(clobvarlength,5);
	checkSuccess(blobvar,"hello",5);
	checkSuccess(blobvarlength,5);
	cur.sendQuery("drop table testtable1");
	print

	print "NULL AND EMPTY CLOBS AND CLOBS: \n");
	cur.getNullsAsNulls();
	cur.sendQuery("create table testtable1 (testclob1 clob, testclob2 clob, testblob1 blob, testblob2 blob)");
	cur.prepareQuery("insert into testtable1 values (:var1,:var2,:var3,:var4)");
	cur.inputBindClob("var1","",0);
	cur.inputBindClob("var2",None,0);
	cur.inputBindBlob("var3","",0);
	cur.inputBindBlob("var4",None,0);
	checkSuccess(cur.executeQuery(),1);
	cur.sendQuery("select * from testtable1");
	checkSuccess(cur.getField(0,0),None);
	checkSuccess(cur.getField(0,1),None);
	checkSuccess(cur.getField(0,2),None);
	checkSuccess(cur.getField(0,3),None);
	cur.sendQuery("drop table testtable1");
	print

	print "CURSOR BINDS: \n");
	checkSuccess(cur.sendQuery("create or replace package types as type cursorType is ref cursor; end;"),1);
	checkSuccess(cur.sendQuery("create or replace function sp_testtable return types.cursortype as l_cursor    types.cursorType; begin open l_cursor for select * from testtable; return l_cursor; end;"),1);
	cur.prepareQuery("begin  :curs:=sp_testtable; end;");
	cur.defineOutputBindCursor("curs");
	checkSuccess(cur.executeQuery(),1);
	sqlrcursor	*bindcur=cur.getOutputBindCursor("curs");
	checkSuccess(bindcur.fetchFromBindCursor(),1);
	checkSuccess(bindcur.getField(0,0),"1");
	checkSuccess(bindcur.getField(1,0),"2");
	checkSuccess(bindcur.getField(2,0),"3");
	checkSuccess(bindcur.getField(3,0),"4");
	checkSuccess(bindcur.getField(4,0),"5");
	checkSuccess(bindcur.getField(5,0),"6");
	checkSuccess(bindcur.getField(6,0),"7");
	checkSuccess(bindcur.getField(7,0),"8");
	print

	print "LONG CLOB: \n");
	cur.sendQuery("drop table testtable2");
	cur.sendQuery("create table testtable2 (testclob clob)");
	cur.prepareQuery("insert into testtable2 values (:clobval)");
	char	clobval[8*1024+1];
	for (int i=0; i<8*1024; i++) {
		clobval[i]='C';
	}
	clobval[8*1024]=(char)NULL;
	cur.inputBindClob("clobval",clobval,8*1024);
	checkSuccess(cur.executeQuery(),1);
	cur.sendQuery("select testclob from testtable2");
	checkSuccess(clobval,cur.getField(0,"testclob"));
	cur.prepareQuery("begin select testclob into :clobbindval from testtable2; end;");
	cur.defineOutputBindClob("clobbindval");
	checkSuccess(cur.executeQuery(),1);
	char	*clobbindvar=cur.getOutputBind("clobbindval");
	checkSuccess(cur.getOutputBindLength("clobbindval"),8*1024);
	checkSuccess(clobval,clobbindvar);
	cur.sendQuery("drop table testtable2");
	print



	# drop existing table
	cur.sendQuery("drop table testtable")

	# invalid queries...
	print "INVALID QUERIES: "
	checkSuccess(cur.sendQuery("select * from testtable order by testnumber"),0)
	checkSuccess(cur.sendQuery("select * from testtable order by testnumber"),0)
	checkSuccess(cur.sendQuery("select * from testtable order by testnumber"),0)
	checkSuccess(cur.sendQuery("select * from testtable order by testnumber"),0)
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
