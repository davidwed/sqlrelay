#! /usr/bin/env ruby

# Copyright (c) 2001  David Muse
# See the file COPYING for more information.



require 'sqlrelay'

def checkSuccess(value,success)
	if value==success
		print "success "
	else
		print "failure "
		exit(0);
	end
end




# usage...
if ARGV.length < 5
	print "usage: oracle7.rb host port socket user password\n"
	exit(0);
end


# instantiation
con=SQLRConnection.new(ARGV[0],ARGV[1].to_i, 
				ARGV[2],ARGV[3],ARGV[4],0,1)
cur=SQLRCursor.new(con)

# get database type
print "IDENTIFY: \n"
checkSuccess(con.identify(),"oracle7")
print "\n"

# ping
print "PING: \n"
checkSuccess(con.ping(),1)
print "\n"

# drop existing table
cur.sendQuery("drop table testtable")

print "CREATE TEMPTABLE: \n"
checkSuccess(cur.sendQuery("create table testtable (testnumber number, testchar char(40), testvarchar varchar2(40), testdate date, testlong long)"),1)
print "\n"

print "INSERT: \n"
checkSuccess(cur.sendQuery("insert into testtable values (1,'testchar1','testvarchar1','01-JAN-2001','testlong1')"),1)
print "\n"

print "AFFECTED ROWS: \n"
checkSuccess(cur.affectedRows(),1)
print "\n"

print "BIND BY POSITION: \n"
cur.prepareQuery("insert into testtable values (:1,:2,:3,:4,:5)")
checkSuccess(cur.countBindVariables(),5)
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
print "\n"

print "ARRAY OF BINDS BY POSITION: \n"
cur.clearBinds()
cur.inputBinds(["1","2","3","4","5"],
	[4,"testchar4","testvarchar4",
		"01-JAN-2004","testlong4"])
checkSuccess(cur.executeQuery(),1)
print "\n"

print "BIND BY NAME: \n"
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
print "\n"

print "ARRAY OF BINDS BY NAME: \n"
cur.clearBinds()
cur.inputBinds(["var1","var2","var3","var4","var5"],
	[7,"testchar7","testvarchar7",
		"01-JAN-2007","testlong7"])
checkSuccess(cur.executeQuery(),1)
print "\n"

print "BIND BY NAME WITH VALIDATION: \n"
cur.clearBinds()
cur.inputBind("var1",8)
cur.inputBind("var2","testchar8")
cur.inputBind("var3","testvarchar8")
cur.inputBind("var4","01-JAN-2008")
cur.inputBind("var5","testlong8")
cur.inputBind("var6","junkvalue")
cur.validateBinds()
checkSuccess(cur.executeQuery(),1)
print "\n"

print "OUTPUT BIND BY NAME: \n"
cur.prepareQuery("begin  :numvar:=1; :stringvar:='hello'; :floatvar:=2.5; end;")
cur.defineOutputBindInteger("numvar")
cur.defineOutputBindString("stringvar",10)
cur.defineOutputBindDouble("floatvar")
checkSuccess(cur.executeQuery(),1)
numvar=cur.getOutputBindInteger("numvar")
stringvar=cur.getOutputBindString("stringvar")
floatvar=cur.getOutputBindDouble("floatvar")
checkSuccess(numvar,1)
checkSuccess(stringvar,'hello')
checkSuccess(floatvar,2.5)
print "\n"

print "OUTPUT BIND BY NAME WITH VALIDATION: \n"
cur.clearBinds()
cur.defineOutputBindInteger("numvar")
cur.defineOutputBindString("stringvar",10)
cur.defineOutputBindDouble("floatvar")
cur.defineOutputBindString("dummyvar",10)
cur.validateBinds()
checkSuccess(cur.executeQuery(),1)
numvar=cur.getOutputBindInteger("numvar")
stringvar=cur.getOutputBindString("stringvar")
floatvar=cur.getOutputBindDouble("floatvar")
checkSuccess(numvar,1)
checkSuccess(stringvar,'hello')
checkSuccess(floatvar,2.5)
print "\n"

print "OUTPUT BIND BY POSITION: \n"
cur.prepareQuery("begin  :1:=1; :2:='hello'; :3:=2.5; end;")
cur.defineOutputBindInteger("1")
cur.defineOutputBindString("2",10)
cur.defineOutputBindDouble("3")
checkSuccess(cur.executeQuery(),1)
numvar=cur.getOutputBindInteger("1")
stringvar=cur.getOutputBindString("2")
floatvar=cur.getOutputBindDouble("3")
checkSuccess(numvar,1)
checkSuccess(stringvar,'hello')
checkSuccess(floatvar,2.5)
print "\n"

print "SELECT: \n"
checkSuccess(cur.sendQuery("select * from testtable order by testnumber"),1)
print "\n"

print "COLUMN COUNT: \n"
checkSuccess(cur.colCount(),5)
print "\n"

print "COLUMN NAMES: \n"
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
print "\n"

print "COLUMN TYPES: \n"
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
print "\n"

print "COLUMN LENGTH: \n"
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
print "\n"

print "LONGEST COLUMN: \n"
checkSuccess(cur.getLongest(0),1)
checkSuccess(cur.getLongest('testnumber'),1)
checkSuccess(cur.getLongest(1),40)
checkSuccess(cur.getLongest('testchar'),40)
checkSuccess(cur.getLongest(2),12)
checkSuccess(cur.getLongest('testvarchar'),12)
checkSuccess(cur.getLongest(3),9)
checkSuccess(cur.getLongest('testdate'),9)
print "\n"

print "ROW COUNT: \n"
checkSuccess(cur.rowCount(),8)
print "\n"

print "TOTAL ROWS: \n"
checkSuccess(cur.totalRows(),0)
print "\n"

print "FIRST ROW INDEX: \n"
checkSuccess(cur.firstRowIndex(),0)
print "\n"

print "END OF RESULT SET: \n"
checkSuccess(cur.endOfResultSet(),1)
print "\n"

print "FIELDS BY INDEX: \n"
checkSuccess(cur.getField(0,0),"1")
checkSuccess(cur.getField(0,1),"testchar1                               ")
checkSuccess(cur.getField(0,2),"testvarchar1")
checkSuccess(cur.getField(0,3),"01-JAN-01")
#checkSuccess(cur.getField(0,4),"testlong1")
print "\n"
checkSuccess(cur.getField(7,0),"8")
checkSuccess(cur.getField(7,1),"testchar8                               ")
checkSuccess(cur.getField(7,2),"testvarchar8")
checkSuccess(cur.getField(7,3),"01-JAN-08")
#checkSuccess(cur.getField(7,4),"testlong8")
print "\n"

print "FIELD LENGTHS BY INDEX: \n"
checkSuccess(cur.getFieldLength(0,0),1)
checkSuccess(cur.getFieldLength(0,1),40)
checkSuccess(cur.getFieldLength(0,2),12)
checkSuccess(cur.getFieldLength(0,3),9)
print "\n"
checkSuccess(cur.getFieldLength(7,0),1)
checkSuccess(cur.getFieldLength(7,1),40)
checkSuccess(cur.getFieldLength(7,2),12)
checkSuccess(cur.getFieldLength(7,3),9)
print "\n"

print "FIELDS BY NAME: \n"
checkSuccess(cur.getField(0,"testnumber"),"1")
checkSuccess(cur.getField(0,"testchar"),"testchar1                               ")
checkSuccess(cur.getField(0,"testvarchar"),"testvarchar1")
checkSuccess(cur.getField(0,"testdate"),"01-JAN-01")
#checkSuccess(cur.getField(0,"testlong"),"testlong1")
print "\n"
checkSuccess(cur.getField(7,"testnumber"),"8")
checkSuccess(cur.getField(7,"testchar"),"testchar8                               ")
checkSuccess(cur.getField(7,"testvarchar"),"testvarchar8")
checkSuccess(cur.getField(7,"testdate"),"01-JAN-08")
#checkSuccess(cur.getField(7,"testlong"),"testlong8")
print "\n"

print "FIELD LENGTHS BY NAME: \n"
checkSuccess(cur.getFieldLength(0,"testnumber"),1)
checkSuccess(cur.getFieldLength(0,"testchar"),40)
checkSuccess(cur.getFieldLength(0,"testvarchar"),12)
checkSuccess(cur.getFieldLength(0,"testdate"),9)
print "\n"
checkSuccess(cur.getFieldLength(7,"testnumber"),1)
checkSuccess(cur.getFieldLength(7,"testchar"),40)
checkSuccess(cur.getFieldLength(7,"testvarchar"),12)
checkSuccess(cur.getFieldLength(7,"testdate"),9)
print "\n"

print "FIELDS BY ARRAY: \n"
fields=cur.getRow(0)
checkSuccess(fields[0],"1")
checkSuccess(fields[1],"testchar1                               ")
checkSuccess(fields[2],"testvarchar1")
checkSuccess(fields[3],"01-JAN-01")
#checkSuccess(fields[4],"testlong1")
print "\n"

print "FIELD LENGTHS BY ARRAY: \n"
fieldlens=cur.getRowLengths(0)
checkSuccess(fieldlens[0],1)
checkSuccess(fieldlens[1],40)
checkSuccess(fieldlens[2],12)
checkSuccess(fieldlens[3],9)
print "\n"

print "FIELDS BY HASH: \n"
fields=cur.getRowHash(0)
checkSuccess(fields["TESTNUMBER"],"1")
checkSuccess(fields["TESTCHAR"],"testchar1                               ")
checkSuccess(fields["TESTVARCHAR"],"testvarchar1")
checkSuccess(fields["TESTDATE"],"01-JAN-01")
#checkSuccess(fields["TESTLONG"],"testlong1")
print "\n"
fields=cur.getRowHash(7)
checkSuccess(fields["TESTNUMBER"],"8")
checkSuccess(fields["TESTCHAR"],"testchar8                               ")
checkSuccess(fields["TESTVARCHAR"],"testvarchar8")
checkSuccess(fields["TESTDATE"],"01-JAN-08")
#checkSuccess(fields["TESTLONG"],"testlong8")
print "\n"

print "FIELD LENGTHS BY HASH: \n"
fieldlengths=cur.getRowLengthsHash(0)
checkSuccess(fieldlengths["TESTNUMBER"],1)
checkSuccess(fieldlengths["TESTCHAR"],40)
checkSuccess(fieldlengths["TESTVARCHAR"],12)
checkSuccess(fieldlengths["TESTDATE"],9)
print "\n"
fieldlengths=cur.getRowLengthsHash(7)
checkSuccess(fieldlengths["TESTNUMBER"],1)
checkSuccess(fieldlengths["TESTCHAR"],40)
checkSuccess(fieldlengths["TESTVARCHAR"],12)
checkSuccess(fieldlengths["TESTDATE"],9)
print "\n"

print "INDIVIDUAL SUBSTITUTIONS: \n"
cur.prepareQuery("select $(var1),'$(var2)',$(var3) from dual")
cur.substitution("var1",1)
cur.substitution("var2","hello")
cur.substitution("var3",10.5556,6,4)
checkSuccess(cur.executeQuery(),1)
print "\n"

print "FIELDS: \n"
checkSuccess(cur.getField(0,0),"1")
checkSuccess(cur.getField(0,1),"hello")
checkSuccess(cur.getField(0,2),"10.5556")
print "\n"

print "OUTPUT BIND: \n"
cur.prepareQuery("begin :var1:='hello'; end;")
cur.defineOutputBindString("var1",10)
checkSuccess(cur.executeQuery(),1)
checkSuccess(cur.getOutputBindString("var1"),"hello")
print "\n"

print "ARRAY SUBSTITUTIONS: \n"
cur.prepareQuery("select $(var1),'$(var2)',$(var3) from dual")
cur.substitutions(["var1","var2","var3"],
			[1,"hello",10.5556],[0,0,6],[0,0,4])
checkSuccess(cur.executeQuery(),1)
print "\n"

print "FIELDS: \n"
checkSuccess(cur.getField(0,0),"1")
checkSuccess(cur.getField(0,1),"hello")
checkSuccess(cur.getField(0,2),"10.5556")
print "\n"

print "NULLS as nils: \n"
cur.getNullsAsNils()
checkSuccess(cur.sendQuery("select NULL,1,NULL from dual"),1)
checkSuccess(cur.getField(0,0),nil)
checkSuccess(cur.getField(0,1),"1")
checkSuccess(cur.getField(0,2),nil)
cur.getNullsAsEmptyStrings()
checkSuccess(cur.sendQuery("select NULL,1,NULL from dual"),1)
checkSuccess(cur.getField(0,0),"")
checkSuccess(cur.getField(0,1),"1")
checkSuccess(cur.getField(0,2),"")
cur.getNullsAsNils()
print "\n"

print "RESULT SET BUFFER SIZE: \n"
checkSuccess(cur.getResultSetBufferSize(),0)
cur.setResultSetBufferSize(2)
checkSuccess(cur.sendQuery("select * from testtable order by testnumber"),1)
checkSuccess(cur.getResultSetBufferSize(),2)
print "\n"
checkSuccess(cur.firstRowIndex(),0)
checkSuccess(cur.endOfResultSet(),0)
checkSuccess(cur.rowCount(),2)
checkSuccess(cur.getField(0,0),"1")
checkSuccess(cur.getField(1,0),"2")
checkSuccess(cur.getField(2,0),"3")
print "\n"
checkSuccess(cur.firstRowIndex(),2)
checkSuccess(cur.endOfResultSet(),0)
checkSuccess(cur.rowCount(),4)
checkSuccess(cur.getField(6,0),"7")
checkSuccess(cur.getField(7,0),"8")
print "\n"
checkSuccess(cur.firstRowIndex(),6)
checkSuccess(cur.endOfResultSet(),0)
checkSuccess(cur.rowCount(),8)
checkSuccess(cur.getField(8,0),nil)
print "\n"
checkSuccess(cur.firstRowIndex(),8)
checkSuccess(cur.endOfResultSet(),1)
checkSuccess(cur.rowCount(),8)
print "\n"

print "DONT GET COLUMN INFO: \n"
cur.dontGetColumnInfo()
checkSuccess(cur.sendQuery("select * from testtable order by testnumber"),1)
checkSuccess(cur.getColumnName(0),nil)
checkSuccess(cur.getColumnLength(0),0)
checkSuccess(cur.getColumnType(0),nil)
cur.getColumnInfo()
checkSuccess(cur.sendQuery("select * from testtable order by testnumber"),1)
checkSuccess(cur.getColumnName(0),"TESTNUMBER")
checkSuccess(cur.getColumnLength(0),22)
checkSuccess(cur.getColumnType(0),"NUMBER")
print "\n"

print "SUSPENDED SESSION: \n"
checkSuccess(cur.sendQuery("select * from testtable order by testnumber"),1)
cur.suspendResultSet()
checkSuccess(con.suspendSession(),1)
port=con.getConnectionPort()
socket=con.getConnectionSocket()
checkSuccess(con.resumeSession(port,socket),1)
print "\n"
checkSuccess(cur.getField(0,0),"1")
checkSuccess(cur.getField(1,0),"2")
checkSuccess(cur.getField(2,0),"3")
checkSuccess(cur.getField(3,0),"4")
checkSuccess(cur.getField(4,0),"5")
checkSuccess(cur.getField(5,0),"6")
checkSuccess(cur.getField(6,0),"7")
checkSuccess(cur.getField(7,0),"8")
print "\n"
checkSuccess(cur.sendQuery("select * from testtable order by testnumber"),1)
cur.suspendResultSet()
checkSuccess(con.suspendSession(),1)
port=con.getConnectionPort()
socket=con.getConnectionSocket()
checkSuccess(con.resumeSession(port,socket),1)
print "\n"
checkSuccess(cur.getField(0,0),"1")
checkSuccess(cur.getField(1,0),"2")
checkSuccess(cur.getField(2,0),"3")
checkSuccess(cur.getField(3,0),"4")
checkSuccess(cur.getField(4,0),"5")
checkSuccess(cur.getField(5,0),"6")
checkSuccess(cur.getField(6,0),"7")
checkSuccess(cur.getField(7,0),"8")
print "\n"
checkSuccess(cur.sendQuery("select * from testtable order by testnumber"),1)
cur.suspendResultSet()
checkSuccess(con.suspendSession(),1)
port=con.getConnectionPort()
socket=con.getConnectionSocket()
checkSuccess(con.resumeSession(port,socket),1)
print "\n"
checkSuccess(cur.getField(0,0),"1")
checkSuccess(cur.getField(1,0),"2")
checkSuccess(cur.getField(2,0),"3")
checkSuccess(cur.getField(3,0),"4")
checkSuccess(cur.getField(4,0),"5")
checkSuccess(cur.getField(5,0),"6")
checkSuccess(cur.getField(6,0),"7")
checkSuccess(cur.getField(7,0),"8")
print "\n"

print "SUSPENDED RESULT SET: \n"
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
print "\n"
checkSuccess(cur.firstRowIndex(),4)
checkSuccess(cur.endOfResultSet(),0)
checkSuccess(cur.rowCount(),6)
checkSuccess(cur.getField(7,0),"8")
print "\n"
checkSuccess(cur.firstRowIndex(),6)
checkSuccess(cur.endOfResultSet(),0)
checkSuccess(cur.rowCount(),8)
checkSuccess(cur.getField(8,0),nil)
print "\n"
checkSuccess(cur.firstRowIndex(),8)
checkSuccess(cur.endOfResultSet(),1)
checkSuccess(cur.rowCount(),8)
cur.setResultSetBufferSize(0)
print "\n"

print "CACHED RESULT SET: \n"
cur.cacheToFile("cachefile1")
cur.setCacheTtl(200)
checkSuccess(cur.sendQuery("select * from testtable order by testnumber"),1)
filename=cur.getCacheFileName()
checkSuccess(filename,"cachefile1")
cur.cacheOff()
checkSuccess(cur.openCachedResultSet(filename),1)
checkSuccess(cur.getField(7,0),"8")
print "\n"

print "COLUMN COUNT FOR CACHED RESULT SET: \n"
checkSuccess(cur.colCount(),5)
print "\n"

print "COLUMN NAMES FOR CACHED RESULT SET: \n"
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
print "\n"

print "CACHED RESULT SET WITH RESULT SET BUFFER SIZE: \n"
cur.setResultSetBufferSize(2)
cur.cacheToFile("cachefile1")
cur.setCacheTtl(200)
checkSuccess(cur.sendQuery("select * from testtable order by testnumber"),1)
filename=cur.getCacheFileName()
checkSuccess(filename,"cachefile1")
cur.cacheOff()
checkSuccess(cur.openCachedResultSet(filename),1)
checkSuccess(cur.getField(7,0),"8")
checkSuccess(cur.getField(8,0),nil)
cur.setResultSetBufferSize(0)
print "\n"

print "FROM ONE CACHE FILE TO ANOTHER: \n"
cur.cacheToFile("cachefile2")
checkSuccess(cur.openCachedResultSet("cachefile1"),1)
cur.cacheOff()
checkSuccess(cur.openCachedResultSet("cachefile2"),1)
checkSuccess(cur.getField(7,0),"8")
checkSuccess(cur.getField(8,0),nil)
print "\n"

print "FROM ONE CACHE FILE TO ANOTHER WITH RESULT SET BUFFER SIZE: \n"
cur.setResultSetBufferSize(2)
cur.cacheToFile("cachefile2")
checkSuccess(cur.openCachedResultSet("cachefile1"),1)
cur.cacheOff()
checkSuccess(cur.openCachedResultSet("cachefile2"),1)
checkSuccess(cur.getField(7,0),"8")
checkSuccess(cur.getField(8,0),nil)
cur.setResultSetBufferSize(0)
print "\n"

print "CACHED RESULT SET WITH SUSPEND AND RESULT SET BUFFER SIZE: \n"
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
print "\n"
checkSuccess(con.resumeSession(port,socket),1)
checkSuccess(cur.resumeCachedResultSet(id,filename),1)
print "\n"
checkSuccess(cur.firstRowIndex(),4)
checkSuccess(cur.endOfResultSet(),0)
checkSuccess(cur.rowCount(),6)
checkSuccess(cur.getField(7,0),"8")
print "\n"
checkSuccess(cur.firstRowIndex(),6)
checkSuccess(cur.endOfResultSet(),0)
checkSuccess(cur.rowCount(),8)
checkSuccess(cur.getField(8,0),nil)
print "\n"
checkSuccess(cur.firstRowIndex(),8)
checkSuccess(cur.endOfResultSet(),1)
checkSuccess(cur.rowCount(),8)
cur.cacheOff()
print "\n"
checkSuccess(cur.openCachedResultSet(filename),1)
checkSuccess(cur.getField(7,0),"8")
checkSuccess(cur.getField(8,0),nil)
cur.setResultSetBufferSize(0)
print "\n"

print "COMMIT AND ROLLBACK: \n"
secondcon=SQLRConnection.new(ARGV[0],
			ARGV[1].to_i, 
			ARGV[2],ARGV[3],ARGV[4],0,1)
secondcur=SQLRCursor.new(secondcon)
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
print "\n"

print "FINISHED SUSPENDED SESSION: \n"
checkSuccess(cur.sendQuery("select * from testtable order by testnumber"),1)
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
checkSuccess(cur.getField(4,0),nil)
checkSuccess(cur.getField(5,0),nil)
checkSuccess(cur.getField(6,0),nil)
checkSuccess(cur.getField(7,0),nil)
print "\n"

# drop existing table
cur.sendQuery("drop table testtable")

# invalid queries...
print "INVALID QUERIES: \n"
checkSuccess(cur.sendQuery("select * from testtable order by testnumber"),0)
checkSuccess(cur.sendQuery("select * from testtable order by testnumber"),0)
checkSuccess(cur.sendQuery("select * from testtable order by testnumber"),0)
checkSuccess(cur.sendQuery("select * from testtable order by testnumber"),0)
print "\n"
checkSuccess(cur.sendQuery("insert into testtable values (1,2,3,4)"),0)
checkSuccess(cur.sendQuery("insert into testtable values (1,2,3,4)"),0)
checkSuccess(cur.sendQuery("insert into testtable values (1,2,3,4)"),0)
checkSuccess(cur.sendQuery("insert into testtable values (1,2,3,4)"),0)
print "\n"
checkSuccess(cur.sendQuery("create table testtable"),0)
checkSuccess(cur.sendQuery("create table testtable"),0)
checkSuccess(cur.sendQuery("create table testtable"),0)
checkSuccess(cur.sendQuery("create table testtable"),0)
print "\n"



