#! /usr/bin/env python

# Copyright (c) 2001  David Muse
# See the file COPYING for more information.

from SQLRelay import PySQLRClient
import sys
import string

def main():


	# usage...
	if len(sys.argv) < 5:
		print "usage: oracle8.py host port socket user password"
		sys.exit(0)

	loop=1

	while 1:

		# instantiation
		con=PySQLRClient.sqlrconnection(sys.argv[1],
					string.atoi(sys.argv[2]), 
					sys.argv[3],sys.argv[4],sys.argv[5])
		cur=PySQLRClient.sqlrcursor(con)
		con.debugOn()

		# get database type
		con.identify()

		# ping
		con.ping()

		# drop existing table
		cur.sendQuery("drop table testtable")

		# create table
		cur.sendQuery("create table testtable (testnumber number, testchar char(40), testvarchar varchar2(40), testdate date, testlong long)")

		# insert
		cur.sendQuery("insert into testtable values (1,'testchar1','testvarchar1','01-JAN-2001','testlong1')")

		# affected rows
		cur.affectedRows()

		# bind by position
		cur.prepareQuery("insert into testtable values (:var1,:var2,:var3,:var4,:var5)")
		cur.inputBind("1",2)
		cur.inputBind("2","testchar2")
		cur.inputBind("3","testvarchar2")
		cur.inputBind("4","01-JAN-2002")
		cur.inputBind("5","testlong2")
		cur.executeQuery()
		cur.clearBinds()
		cur.inputBind("1",3)
		cur.inputBind("2","testchar3")
		cur.inputBind("3","testvarchar3")
		cur.inputBind("4","01-JAN-2003")
		cur.inputBind("5","testlong3")
		cur.executeQuery()

		# array of binds by position
		cur.clearBinds()
		cur.inputBinds(["1","2","3","4","5"],
			[4,"testchar4","testvarchar4",
				"01-JAN-2004","testlong4"])
		cur.executeQuery()

		# bind by name
		cur.prepareQuery("insert into testtable values (:var1,:var2,:var3,:var4,:var5)")
		cur.inputBind("var1",5)
		cur.inputBind("var2","testchar5")
		cur.inputBind("var3","testvarchar5")
		cur.inputBind("var4","01-JAN-2005")
		cur.inputBind("var5","testlong5")
		cur.executeQuery()
		cur.clearBinds()
		cur.inputBind("var1",6)
		cur.inputBind("var2","testchar6")
		cur.inputBind("var3","testvarchar6")
		cur.inputBind("var4","01-JAN-2006")
		cur.inputBind("var5","testlong6")
		cur.executeQuery()

		# array of binds by name
		cur.clearBinds()
		cur.inputBinds(["var1","var2","var3","var4","var5"],
			[7,"testchar7","testvarchar7",
				"01-JAN-2007","testlong7"])
		cur.executeQuery()

		# bind by name with validation
		cur.clearBinds()
		cur.inputBind("var1",8)
		cur.inputBind("var2","testchar8")
		cur.inputBind("var3","testvarchar8")
		cur.inputBind("var4","01-JAN-2008")
		cur.inputBind("var5","testlong8")
		cur.inputBind("var6","junkvalue")
		cur.validateBinds()
		cur.executeQuery()

		# output bind by name
		cur.prepareQuery("begin  :numvar:=1; :stringvar:='hello'; :floatvar:=2.5; end;")
		cur.defineOutputBind("numvar",10)
		cur.defineOutputBind("stringvar",10)
		cur.defineOutputBind("floatvar",10)
		cur.executeQuery()
		numvar=cur.getOutputBind("numvar")
		stringvar=cur.getOutputBind("stringvar")
		floatvar=cur.getOutputBind("floatvar")

		# output bind by name
		cur.clearBinds()
		cur.defineOutputBind("1",10)
		cur.defineOutputBind("2",10)
		cur.defineOutputBind("3",10)
		cur.executeQuery()
		numvar=cur.getOutputBind("1")
		stringvar=cur.getOutputBind("2")
		floatvar=cur.getOutputBind("3")

		# output bind by name with validation
		cur.clearBinds()
		cur.defineOutputBind("numvar",10)
		cur.defineOutputBind("stringvar",10)
		cur.defineOutputBind("floatvar",10)
		cur.defineOutputBind("dummyvar",10)
		cur.validateBinds()
		cur.executeQuery()
		numvar=cur.getOutputBind("numvar")
		stringvar=cur.getOutputBind("stringvar")
		floatvar=cur.getOutputBind("floatvar")

		# select
		cur.sendQuery("select * from testtable order by testnumber")

		# column count
		cur.colCount()

		# column names
		cur.getColumnName(0)
		cur.getColumnName(1)
		cur.getColumnName(2)
		cur.getColumnName(3)
		cur.getColumnName(4)
		cols=cur.getColumnNames()

		# column types
		cur.getColumnType(0)
		cur.getColumnType('testnumber')
		cur.getColumnType(1)
		cur.getColumnType('testchar')
		cur.getColumnType(2)
		cur.getColumnType('testvarchar')
		cur.getColumnType(3)
		cur.getColumnType('testdate')
		cur.getColumnType(4)
		cur.getColumnType('testlong')

		# column length
		cur.getColumnLength(0)
		cur.getColumnLength('testnumber')
		cur.getColumnLength(1)
		cur.getColumnLength('testchar')
		cur.getColumnLength(2)
		cur.getColumnLength('testvarchar')
		cur.getColumnLength(3)
		cur.getColumnLength('testdate')
		cur.getColumnLength(4)
		cur.getColumnLength('testlong')

		# longest column
		cur.getLongest(0)
		cur.getLongest('testnumber')
		cur.getLongest(1)
		cur.getLongest('testchar')
		cur.getLongest(2)
		cur.getLongest('testvarchar')
		cur.getLongest(3)
		cur.getLongest('testdate')
		cur.getLongest(4)
		cur.getLongest('testlong')

		# row count
		cur.rowCount()

		# total rows
		cur.totalRows()

		# first row index
		cur.firstRowIndex()

		# end of result set
		cur.endOfResultSet()

		# fields by index
		cur.getField(0,0)
		cur.getField(0,1)
		cur.getField(0,2)
		cur.getField(0,3)
		cur.getField(0,4)
		cur.getField(7,0)
		cur.getField(7,1)
		cur.getField(7,2)
		cur.getField(7,3)
		cur.getField(7,4)

		# field lengths by index
		cur.getFieldLength(0,0)
		cur.getFieldLength(0,1)
		cur.getFieldLength(0,2)
		cur.getFieldLength(0,3)
		cur.getFieldLength(0,4)
		cur.getFieldLength(7,0)
		cur.getFieldLength(7,1)
		cur.getFieldLength(7,2)
		cur.getFieldLength(7,3)
		cur.getFieldLength(7,4)

		# fields by name
		cur.getField(0,"testnumber")
		cur.getField(0,"testchar")
		cur.getField(0,"testvarchar")
		cur.getField(0,"testdate")
		cur.getField(0,"testlong")
		cur.getField(7,"testnumber")
		cur.getField(7,"testchar")
		cur.getField(7,"testvarchar")
		cur.getField(7,"testdate")
		cur.getField(7,"testlong")

		# field lengths by name
		cur.getFieldLength(0,"testnumber")
		cur.getFieldLength(0,"testchar")
		cur.getFieldLength(0,"testvarchar")
		cur.getFieldLength(0,"testdate")
		cur.getFieldLength(0,"testlong")
		cur.getFieldLength(7,"testnumber")
		cur.getFieldLength(7,"testchar")
		cur.getFieldLength(7,"testvarchar")
		cur.getFieldLength(7,"testdate")
		cur.getFieldLength(7,"testlong")

		# fields by array
		fields=cur.getRow(0)

		# field lengths by array
		fieldlens=cur.getRowLengths(0)

		# fields by dictionary
		fields=cur.getRowDictionary(0)
		fields=cur.getRowDictionary(7)

		# field lengths by dictionary
		fieldlengths=cur.getRowLengthsDictionary(0)
		fieldlengths=cur.getRowLengthsDictionary(7)
		
		# individual substitutions
		cur.prepareQuery("select $(var1),'$(var2)',$(var3) from dual")
		cur.substitution("var1",1)
		cur.substitution("var2","hello")
		cur.substitution("var3",10.5556,6,4)
		cur.executeQuery()

		# fields
		cur.getField(0,0)
		cur.getField(0,1)
		cur.getField(0,2)

		# output bind
		cur.prepareQuery("begin :var1:='hello'; end;")
		cur.defineOutputBind("var1",10)
		cur.executeQuery()
		cur.getOutputBind("var1")

		# array substitutions
		cur.prepareQuery("select $(var1),'$(var2)',$(var3) from dual")
		cur.substitutions(["var1","var2","var3"],
					[1,"hello",10.5556],[0,0,6],[0,0,4])
		cur.executeQuery()

		# fields
		cur.getField(0,0)
		cur.getField(0,1)
		cur.getField(0,2)

		# NULLs as Nones
		cur.getNullsAsNone()
		cur.sendQuery("select NULL,1,NULL from dual")
		cur.getField(0,0)
		cur.getField(0,1)
		cur.getField(0,2)
		cur.getNullsAsEmptyStrings()
		cur.sendQuery("select NULL,1,NULL from dual")
		cur.getField(0,0)
		cur.getField(0,1)
		cur.getField(0,2)

		# result set buffer size
		cur.getResultSetBufferSize()
		cur.setResultSetBufferSize(2)
		cur.sendQuery("select * from testtable order by testnumber")
		cur.getResultSetBufferSize()
		cur.firstRowIndex()
		cur.endOfResultSet()
		cur.rowCount()
		cur.getField(0,0)
		cur.getField(1,0)
		cur.getField(2,0)
		cur.firstRowIndex()
		cur.endOfResultSet()
		cur.rowCount()
		cur.getField(6,0)
		cur.getField(7,0)
		cur.firstRowIndex()
		cur.endOfResultSet()
		cur.rowCount()
		cur.getField(8,0)
		cur.firstRowIndex()
		cur.endOfResultSet()
		cur.rowCount()

		# don't get column info
		cur.dontGetColumnInfo()
		cur.sendQuery("select * from testtable order by testnumber")
		cur.getColumnName(0)
		cur.getColumnLength(0)
		cur.getColumnType(0)
		cur.getColumnInfo()
		cur.sendQuery("select * from testtable order by testnumber")
		cur.getColumnName(0)
		cur.getColumnLength(0)
		cur.getColumnType(0)

		# suspended session
		cur.sendQuery("select * from testtable order by testnumber")
		con.suspendSession()
		port=con.getConnectionPort()
		socket=con.getConnectionSocket()
		con.resumeSession(port,socket)
		cur.getField(0,0)
		cur.getField(1,0)
		cur.getField(2,0)
		cur.getField(3,0)
		cur.getField(4,0)
		cur.getField(5,0)
		cur.getField(6,0)
		cur.getField(7,0)
		cur.sendQuery("select * from testtable order by testnumber")
		con.suspendSession()
		port=con.getConnectionPort()
		socket=con.getConnectionSocket()
		con.resumeSession(port,socket)
		cur.getField(0,0)
		cur.getField(1,0)
		cur.getField(2,0)
		cur.getField(3,0)
		cur.getField(4,0)
		cur.getField(5,0)
		cur.getField(6,0)
		cur.getField(7,0)
		cur.sendQuery("select * from testtable order by testnumber")
		con.suspendSession()
		port=con.getConnectionPort()
		socket=con.getConnectionSocket()
		con.resumeSession(port,socket)
		cur.getField(0,0)
		cur.getField(1,0)
		cur.getField(2,0)
		cur.getField(3,0)
		cur.getField(4,0)
		cur.getField(5,0)
		cur.getField(6,0)
		cur.getField(7,0)

		# suspended result set
		cur.setResultSetBufferSize(2)
		cur.sendQuery("select * from testtable order by testnumber")
		cur.getField(2,0)
		id=cur.getResultSetId()
		cur.suspendResultSet()
		con.suspendSession()
		port=con.getConnectionPort()
		socket=con.getConnectionSocket()
		con.resumeSession(port,socket)
		cur.resumeResultSet(id)
		cur.firstRowIndex()
		cur.endOfResultSet()
		cur.rowCount()
		cur.getField(7,0)
		cur.firstRowIndex()
		cur.endOfResultSet()
		cur.rowCount()
		cur.getField(8,0)
		cur.firstRowIndex()
		cur.endOfResultSet()
		cur.rowCount()
		cur.setResultSetBufferSize(0)

		# cached result set
		cur.cacheToFile("cachefile1")
		cur.setCacheTtl(200)
		cur.sendQuery("select * from testtable order by testnumber")
		filename=cur.getCacheFileName()
		cur.cacheOff()
		cur.openCachedResultSet(filename)
		cur.getField(7,0)

		# column count for cached result set
		cur.colCount()

		# column names for cached result set
		cur.getColumnName(0)
		cur.getColumnName(1)
		cur.getColumnName(2)
		cur.getColumnName(3)
		cur.getColumnName(4)
		cols=cur.getColumnNames()

		# cached result set with result set buffer size
		cur.setResultSetBufferSize(2)
		cur.cacheToFile("cachefile1")
		cur.setCacheTtl(200)
		cur.sendQuery("select * from testtable order by testnumber")
		filename=cur.getCacheFileName()
		cur.cacheOff()
		cur.openCachedResultSet(filename)
		cur.getField(7,0)
		cur.getField(8,0)
		cur.setResultSetBufferSize(0)

		# from one cache file to another
		cur.cacheToFile("cachefile2")
		cur.openCachedResultSet("cachefile1")
		cur.cacheOff()
		cur.openCachedResultSet("cachefile2")
		cur.getField(7,0)
		cur.getField(8,0)

		# from one cache file to another with result set buffer size
		cur.setResultSetBufferSize(2)
		cur.cacheToFile("cachefile2")
		cur.openCachedResultSet("cachefile1")
		cur.cacheOff()
		cur.openCachedResultSet("cachefile2")
		cur.getField(7,0)
		cur.getField(8,0)
		cur.setResultSetBufferSize(0)

		# cached result set with suspend and result set buffer size
		cur.setResultSetBufferSize(2)
		cur.cacheToFile("cachefile1")
		cur.setCacheTtl(200)
		cur.sendQuery("select * from testtable order by testnumber")
		cur.getField(2,0)
		filename=cur.getCacheFileName()
		id=cur.getResultSetId()
		cur.suspendResultSet()
		con.suspendSession()
		port=con.getConnectionPort()
		socket=con.getConnectionSocket()
		con.resumeSession(port,socket)
		cur.resumeCachedResultSet(id,filename)
		cur.firstRowIndex()
		cur.endOfResultSet()
		cur.rowCount()
		cur.getField(7,0)
		cur.firstRowIndex()
		cur.endOfResultSet()
		cur.rowCount()
		cur.getField(8,0)
		cur.firstRowIndex()
		cur.endOfResultSet()
		cur.rowCount()
		cur.cacheOff()
		cur.openCachedResultSet(filename)
		cur.getField(7,0)
		cur.getField(8,0)
		cur.setResultSetBufferSize(0)

		# commit and rollback
		secondcon=PySQLRClient.sqlrconnection(sys.argv[1],
					string.atoi(sys.argv[2]), 
					sys.argv[3],sys.argv[4],sys.argv[5])
		secondcur=PySQLRClient.sqlrcursor(secondcon)
		secondcur.sendQuery("select count(*) from testtable")
		secondcur.getField(0,0)
		con.commit()
		secondcur.sendQuery("select count(*) from testtable")
		secondcur.getField(0,0)
		con.autoCommitOn()
		cur.sendQuery("insert into testtable values (10,'testchar10','testvarchar10','01-JAN-2010','testlong10')")
		secondcur.sendQuery("select count(*) from testtable")
		secondcur.getField(0,0)
		con.autoCommitOff()

		# row range
		cur.sendQuery("select * from testtable order by testnumber")
		rows=cur.getRowRange(0,5)

		# drop existing table
		cur.sendQuery("drop table testtable")

		# invalid queries
		cur.sendQuery("select * from testtable order by testnumber")
		cur.sendQuery("insert into testtable values (1,2,3,4)")
		cur.sendQuery("create table testtable")

		print "loop: ", loop
		loop=loop+1

if __name__ == "__main__":
	main()
