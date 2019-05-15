#! /usr/bin/env ruby

# Copyright (c) 1999-2018 David Muse
# See the file COPYING for more information.



require 'rbconfig'
require 'sqlrelay'


def checkSuccess(value,success)
	if value==success
		print "success "
	else
		print value , " != " , success, " "
		print "failure "
		exit(1)
	end
end



# instantiation
con=SQLRConnection.new("sqlrelay",9000,"/tmp/test.socket",
						"test","test",0,1);
cur=SQLRCursor.new(con)

# get database type
print "IDENTIFY: \n"
checkSuccess(con.identify(),"sap")
print "\n"

# ping
print "PING: \n"
checkSuccess(con.ping(),1)
print "\n"

# drop existing table
cur.sendQuery("drop table testtable")

print "CREATE TEMPTABLE: \n"
checkSuccess(cur.sendQuery("create table testtable (testint int, testsmallint smallint, testtinyint tinyint, testreal real, testfloat float, testdecimal decimal(4,1), testnumeric numeric(4,1), testmoney money, testsmallmoney smallmoney, testdatetime datetime, testsmalldatetime smalldatetime, testchar char(40), testvarchar varchar(40), testbit bit)"),1)
print "\n"

print "BEGIN TRANSACTION: \n"
#checkSuccess(cur.sendQuery("begin tran"),1)
print "\n"

print "INSERT: \n"
checkSuccess(cur.sendQuery("insert into testtable values (1,1,1,1.1,1.1,1.1,1.1,1.00,1.00,'01-Jan-2001 01:00:00','01-Jan-2001 01:00:00','testchar1','testvarchar1',1)"),1)
print "\n"

print "AFFECTED ROWS: \n"
checkSuccess(cur.affectedRows(),1)
print "\n"

print "BIND BY POSITION: \n"
cur.prepareQuery("insert into testtable values (@var1,@var2,@var3,@var4,@var5,@var6,@var7,@var8,@var9,@var10,@var11,@var12,@var13,@var14)")
checkSuccess(cur.countBindVariables(),14)
cur.inputBind("1",2)
cur.inputBind("2",2)
cur.inputBind("3",2)
cur.inputBind("4",2.2,2,1)
cur.inputBind("5",2.2,2,1)
cur.inputBind("6",2.2,2,1)
cur.inputBind("7",2.2,2,1)
cur.inputBind("8",2.00,3,2)
cur.inputBind("9",2.00,3,2)
cur.inputBind("10","01-Jan-2002 02:00:00")
cur.inputBind("11","01-Jan-2002 02:00:00")
cur.inputBind("12","testchar2")
cur.inputBind("13","testvarchar2")
cur.inputBind("14",1)
checkSuccess(cur.executeQuery(),1)
cur.clearBinds()
cur.inputBind("1",3)
cur.inputBind("2",3)
cur.inputBind("3",3)
cur.inputBind("4",3.3,2,1)
cur.inputBind("5",3.3,2,1)
cur.inputBind("6",3.3,2,1)
cur.inputBind("7",3.3,2,1)
cur.inputBind("8",3.00,3,2)
cur.inputBind("9",3.00,3,2)
cur.inputBind("10","01-Jan-2003 03:00:00")
cur.inputBind("11","01-Jan-2003 03:00:00")
cur.inputBind("12","testchar3")
cur.inputBind("13","testvarchar3")
cur.inputBind("14",1)
checkSuccess(cur.executeQuery(),1)
print "\n"

print "ARRAY OF BINDS BY POSITION: \n"
cur.clearBinds()
cur.inputBinds(["1","2","3","4","5","6",
		"7","8","9","10","11","12",
		"13","14"],
	[4,4,4,4.4,4.4,4.4,4.4,4.00,4.00,
		"01-Jan-2004 04:00:00",
		"01-Jan-2004 04:00:00",
		"testchar4","testvarchar4",1],
	[0,0,0,2,2,2,2,3,3,0,0,0,0,0],
	[0,0,0,1,1,1,1,2,2,0,0,0,0,0])
checkSuccess(cur.executeQuery(),1)
print "\n"

print "BIND BY NAME: \n"
cur.clearBinds()
cur.inputBind("var1",5)
cur.inputBind("var2",5)
cur.inputBind("var3",5)
cur.inputBind("var4",5.5,2,1)
cur.inputBind("var5",5.5,2,1)
cur.inputBind("var6",5.5,2,1)
cur.inputBind("var7",5.5,2,1)
cur.inputBind("var8",5.00,3,2)
cur.inputBind("var9",5.00,3,2)
cur.inputBind("var10","01-Jan-2005 05:00:00")
cur.inputBind("var11","01-Jan-2005 05:00:00")
cur.inputBind("var12","testchar5")
cur.inputBind("var13","testvarchar5")
cur.inputBind("var14",1)
checkSuccess(cur.executeQuery(),1)
cur.clearBinds()
cur.inputBind("var1",6)
cur.inputBind("var2",6)
cur.inputBind("var3",6)
cur.inputBind("var4",6.6,2,1)
cur.inputBind("var5",6.6,2,1)
cur.inputBind("var6",6.6,2,1)
cur.inputBind("var7",6.6,2,1)
cur.inputBind("var8",6.00,3,2)
cur.inputBind("var9",6.00,3,2)
cur.inputBind("var10","01-Jan-2006 06:00:00")
cur.inputBind("var11","01-Jan-2006 06:00:00")
cur.inputBind("var12","testchar6")
cur.inputBind("var13","testvarchar6")
cur.inputBind("var14",1)
checkSuccess(cur.executeQuery(),1)
print "\n"

print "ARRAY OF BINDS BY NAME: \n"
cur.clearBinds()
cur.inputBinds(["var1","var2","var3","var4","var5","var6",
		"var7","var8","var9","var10","var11","var12",
		"var13","var14"],
	[7,7,7,7.7,7.7,7.7,7.7,7.00,7.00,
		"01-Jan-2007 07:00:00",
		"01-Jan-2007 07:00:00",
		"testchar7","testvarchar7",1],
	[0,0,0,2,2,2,2,3,3,0,0,0,0,0],
	[0,0,0,1,1,1,1,2,2,0,0,0,0,0])
checkSuccess(cur.executeQuery(),1)
print "\n"

print "BIND BY NAME WITH VALIDATION: \n"
cur.clearBinds()
cur.inputBind("var1",8)
cur.inputBind("var2",8)
cur.inputBind("var3",8)
cur.inputBind("var4",8.8,2,1)
cur.inputBind("var5",8.8,2,1)
cur.inputBind("var6",8.8,2,1)
cur.inputBind("var7",8.8,2,1)
cur.inputBind("var8",8.00,3,2)
cur.inputBind("var9",8.00,3,2)
cur.inputBind("var10","01-Jan-2008 08:00:00")
cur.inputBind("var11","01-Jan-2008 08:00:00")
cur.inputBind("var12","testchar8")
cur.inputBind("var13","testvarchar8")
cur.inputBind("var14",1)
cur.inputBind("var15","junkvalue")
cur.validateBinds()
checkSuccess(cur.executeQuery(),1)
print "\n"

print "SELECT: \n"
checkSuccess(cur.sendQuery("select * from testtable order by testint"),1)
print "\n"

print "COLUMN COUNT: \n"
checkSuccess(cur.colCount(),14)
print "\n"

print "COLUMN NAMES: \n"
checkSuccess(cur.getColumnName(0),"testint")
checkSuccess(cur.getColumnName(1),"testsmallint")
checkSuccess(cur.getColumnName(2),"testtinyint")
checkSuccess(cur.getColumnName(3),"testreal")
checkSuccess(cur.getColumnName(4),"testfloat")
checkSuccess(cur.getColumnName(5),"testdecimal")
checkSuccess(cur.getColumnName(6),"testnumeric")
checkSuccess(cur.getColumnName(7),"testmoney")
checkSuccess(cur.getColumnName(8),"testsmallmoney")
checkSuccess(cur.getColumnName(9),"testdatetime")
checkSuccess(cur.getColumnName(10),"testsmalldatetime")
checkSuccess(cur.getColumnName(11),"testchar")
checkSuccess(cur.getColumnName(12),"testvarchar")
checkSuccess(cur.getColumnName(13),"testbit")
cols=cur.getColumnNames()
checkSuccess(cols[0],"testint")
checkSuccess(cols[1],"testsmallint")
checkSuccess(cols[2],"testtinyint")
checkSuccess(cols[3],"testreal")
checkSuccess(cols[4],"testfloat")
checkSuccess(cols[5],"testdecimal")
checkSuccess(cols[6],"testnumeric")
checkSuccess(cols[7],"testmoney")
checkSuccess(cols[8],"testsmallmoney")
checkSuccess(cols[9],"testdatetime")
checkSuccess(cols[10],"testsmalldatetime")
checkSuccess(cols[11],"testchar")
checkSuccess(cols[12],"testvarchar")
checkSuccess(cols[13],"testbit")
print "\n"

print "COLUMN TYPES: \n"
checkSuccess(cur.getColumnType(0),"INT")
checkSuccess(cur.getColumnType('testint'),"INT")
checkSuccess(cur.getColumnType(1),"SMALLINT")
checkSuccess(cur.getColumnType('testsmallint'),"SMALLINT")
checkSuccess(cur.getColumnType(2),"TINYINT")
checkSuccess(cur.getColumnType('testtinyint'),"TINYINT")
checkSuccess(cur.getColumnType(3),"REAL")
checkSuccess(cur.getColumnType('testreal'),"REAL")
checkSuccess(cur.getColumnType(4),"FLOAT")
checkSuccess(cur.getColumnType('testfloat'),"FLOAT")
checkSuccess(cur.getColumnType(5),"DECIMAL")
checkSuccess(cur.getColumnType('testdecimal'),"DECIMAL")
checkSuccess(cur.getColumnType(6),"NUMERIC")
checkSuccess(cur.getColumnType('testnumeric'),"NUMERIC")
checkSuccess(cur.getColumnType(7),"MONEY")
checkSuccess(cur.getColumnType('testmoney'),"MONEY")
checkSuccess(cur.getColumnType(8),"SMALLMONEY")
checkSuccess(cur.getColumnType('testsmallmoney'),"SMALLMONEY")
checkSuccess(cur.getColumnType(9),"DATETIME")
checkSuccess(cur.getColumnType('testdatetime'),"DATETIME")
checkSuccess(cur.getColumnType(10),"SMALLDATETIME")
checkSuccess(cur.getColumnType('testsmalldatetime'),"SMALLDATETIME")
checkSuccess(cur.getColumnType(11),"CHAR")
checkSuccess(cur.getColumnType('testchar'),"CHAR")
checkSuccess(cur.getColumnType(12),"CHAR")
checkSuccess(cur.getColumnType('testvarchar'),"CHAR")
checkSuccess(cur.getColumnType(13),"BIT")
checkSuccess(cur.getColumnType('testbit'),"BIT")
print "\n"

print "COLUMN LENGTH: \n"
checkSuccess(cur.getColumnLength(0),4)
checkSuccess(cur.getColumnLength('testint'),4)
checkSuccess(cur.getColumnLength(1),2)
checkSuccess(cur.getColumnLength('testsmallint'),2)
checkSuccess(cur.getColumnLength(2),1)
checkSuccess(cur.getColumnLength('testtinyint'),1)
checkSuccess(cur.getColumnLength(3),4)
checkSuccess(cur.getColumnLength('testreal'),4)
checkSuccess(cur.getColumnLength(4),8)
checkSuccess(cur.getColumnLength('testfloat'),8)
checkSuccess(cur.getColumnLength(5),35)
checkSuccess(cur.getColumnLength('testdecimal'),35)
checkSuccess(cur.getColumnLength(6),35)
checkSuccess(cur.getColumnLength('testnumeric'),35)
checkSuccess(cur.getColumnLength(7),8)
checkSuccess(cur.getColumnLength('testmoney'),8)
checkSuccess(cur.getColumnLength(8),4)
checkSuccess(cur.getColumnLength('testsmallmoney'),4)
checkSuccess(cur.getColumnLength(9),8)
checkSuccess(cur.getColumnLength('testdatetime'),8)
checkSuccess(cur.getColumnLength(10),4)
checkSuccess(cur.getColumnLength('testsmalldatetime'),4)
checkSuccess(cur.getColumnLength(11),40)
checkSuccess(cur.getColumnLength('testchar'),40)
checkSuccess(cur.getColumnLength(12),40)
checkSuccess(cur.getColumnLength('testvarchar'),40)
checkSuccess(cur.getColumnLength(13),1)
checkSuccess(cur.getColumnLength('testbit'),1)
print "\n"

print "LONGEST COLUMN: \n"
checkSuccess(cur.getLongest(0),1)
checkSuccess(cur.getLongest('testint'),1)
checkSuccess(cur.getLongest(1),1)
checkSuccess(cur.getLongest('testsmallint'),1)
checkSuccess(cur.getLongest(2),1)
checkSuccess(cur.getLongest('testtinyint'),1)
checkSuccess(cur.getLongest(3),18)
checkSuccess(cur.getLongest('testreal'),18)
checkSuccess(cur.getLongest(4),18)
checkSuccess(cur.getLongest('testfloat'),18)
checkSuccess(cur.getLongest(5),3)
checkSuccess(cur.getLongest('testdecimal'),3)
checkSuccess(cur.getLongest(6),3)
checkSuccess(cur.getLongest('testnumeric'),3)
checkSuccess(cur.getLongest(7),4)
checkSuccess(cur.getLongest('testmoney'),4)
checkSuccess(cur.getLongest(8),4)
checkSuccess(cur.getLongest('testsmallmoney'),4)
checkSuccess(cur.getLongest(9),19)
checkSuccess(cur.getLongest('testdatetime'),19)
checkSuccess(cur.getLongest(10),19)
checkSuccess(cur.getLongest('testsmalldatetime'),19)
checkSuccess(cur.getLongest(11),40)
checkSuccess(cur.getLongest('testchar'),40)
checkSuccess(cur.getLongest(12),12)
checkSuccess(cur.getLongest('testvarchar'),12)
checkSuccess(cur.getLongest(13),1)
checkSuccess(cur.getLongest('testbit'),1)
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
checkSuccess(cur.getField(0,1),"1")
checkSuccess(cur.getField(0,2),"1")
#checkSuccess(cur.getField(0,3),"1.1")
#checkSuccess(cur.getField(0,4),"1.1")
checkSuccess(cur.getField(0,5),"1.1")
checkSuccess(cur.getField(0,6),"1.1")
checkSuccess(cur.getField(0,7),"1.00")
checkSuccess(cur.getField(0,8),"1.00")
checkSuccess(cur.getField(0,9),"Jan  1 2001  1:00AM")
checkSuccess(cur.getField(0,10),"Jan  1 2001  1:00AM")
checkSuccess(cur.getField(0,11),"testchar1                               ")
checkSuccess(cur.getField(0,12),"testvarchar1")
checkSuccess(cur.getField(0,13),"1")
print "\n"
checkSuccess(cur.getField(7,0),"8")
checkSuccess(cur.getField(7,1),"8")
checkSuccess(cur.getField(7,2),"8")
#checkSuccess(cur.getField(7,3),"8.8")
#checkSuccess(cur.getField(7,4),"8.8")
checkSuccess(cur.getField(7,5),"8.8")
checkSuccess(cur.getField(7,6),"8.8")
checkSuccess(cur.getField(7,7),"8.00")
checkSuccess(cur.getField(7,8),"8.00")
checkSuccess(cur.getField(7,9),"Jan  1 2008  8:00AM")
checkSuccess(cur.getField(7,10),"Jan  1 2008  8:00AM")
checkSuccess(cur.getField(7,11),"testchar8                               ")
checkSuccess(cur.getField(7,12),"testvarchar8")
checkSuccess(cur.getField(7,13),"1")
print "\n"

print "FIELD LENGTHS BY INDEX: \n"
checkSuccess(cur.getFieldLength(0,0),1)
checkSuccess(cur.getFieldLength(0,1),1)
checkSuccess(cur.getFieldLength(0,2),1)
checkSuccess(cur.getFieldLength(0,3),18)
checkSuccess(cur.getFieldLength(0,4),18)
checkSuccess(cur.getFieldLength(0,5),3)
checkSuccess(cur.getFieldLength(0,6),3)
checkSuccess(cur.getFieldLength(0,7),4)
checkSuccess(cur.getFieldLength(0,8),4)
checkSuccess(cur.getFieldLength(0,9),19)
checkSuccess(cur.getFieldLength(0,10),19)
checkSuccess(cur.getFieldLength(0,11),40)
checkSuccess(cur.getFieldLength(0,12),12)
checkSuccess(cur.getFieldLength(0,13),1)
print "\n"
checkSuccess(cur.getFieldLength(7,0),1)
checkSuccess(cur.getFieldLength(7,1),1)
checkSuccess(cur.getFieldLength(7,2),1)
checkSuccess(cur.getFieldLength(7,3),18)
checkSuccess(cur.getFieldLength(7,4),18)
checkSuccess(cur.getFieldLength(7,5),3)
checkSuccess(cur.getFieldLength(7,6),3)
checkSuccess(cur.getFieldLength(7,7),4)
checkSuccess(cur.getFieldLength(7,8),4)
checkSuccess(cur.getFieldLength(7,9),19)
checkSuccess(cur.getFieldLength(7,10),19)
checkSuccess(cur.getFieldLength(7,11),40)
checkSuccess(cur.getFieldLength(7,12),12)
checkSuccess(cur.getFieldLength(7,13),1)
print "\n"

print "FIELDS BY NAME: \n"
checkSuccess(cur.getField(0,"testint"),"1")
checkSuccess(cur.getField(0,"testsmallint"),"1")
checkSuccess(cur.getField(0,"testtinyint"),"1")
#checkSuccess(cur.getField(0,"testreal"),"1.1")
#checkSuccess(cur.getField(0,"testfloat"),"1.1")
checkSuccess(cur.getField(0,"testdecimal"),"1.1")
checkSuccess(cur.getField(0,"testnumeric"),"1.1")
checkSuccess(cur.getField(0,"testmoney"),"1.00")
checkSuccess(cur.getField(0,"testsmallmoney"),"1.00")
checkSuccess(cur.getField(0,"testdatetime"),"Jan  1 2001  1:00AM")
checkSuccess(cur.getField(0,"testsmalldatetime"),"Jan  1 2001  1:00AM")
checkSuccess(cur.getField(0,"testchar"),"testchar1                               ")
checkSuccess(cur.getField(0,"testvarchar"),"testvarchar1")
checkSuccess(cur.getField(0,"testbit"),"1")
print "\n"
checkSuccess(cur.getField(7,"testint"),"8")
checkSuccess(cur.getField(7,"testsmallint"),"8")
checkSuccess(cur.getField(7,"testtinyint"),"8")
#checkSuccess(cur.getField(7,"testreal"),"8.8")
#checkSuccess(cur.getField(7,"testfloat"),"8.8")
checkSuccess(cur.getField(7,"testdecimal"),"8.8")
checkSuccess(cur.getField(7,"testnumeric"),"8.8")
checkSuccess(cur.getField(7,"testmoney"),"8.00")
checkSuccess(cur.getField(7,"testsmallmoney"),"8.00")
checkSuccess(cur.getField(7,"testdatetime"),"Jan  1 2008  8:00AM")
checkSuccess(cur.getField(7,"testsmalldatetime"),"Jan  1 2008  8:00AM")
checkSuccess(cur.getField(7,"testchar"),"testchar8                               ")
checkSuccess(cur.getField(7,"testvarchar"),"testvarchar8")
checkSuccess(cur.getField(7,"testbit"),"1")
print "\n"

print "FIELD LENGTHS BY NAME: \n"
checkSuccess(cur.getFieldLength(0,"testint"),1)
checkSuccess(cur.getFieldLength(0,"testsmallint"),1)
checkSuccess(cur.getFieldLength(0,"testtinyint"),1)
#checkSuccess(cur.getFieldLength(0,"testreal"),3)
#checkSuccess(cur.getFieldLength(0,"testfloat"),3)
checkSuccess(cur.getFieldLength(0,"testdecimal"),3)
checkSuccess(cur.getFieldLength(0,"testnumeric"),3)
checkSuccess(cur.getFieldLength(0,"testmoney"),4)
checkSuccess(cur.getFieldLength(0,"testsmallmoney"),4)
checkSuccess(cur.getFieldLength(0,"testdatetime"),19)
checkSuccess(cur.getFieldLength(0,"testsmalldatetime"),19)
checkSuccess(cur.getFieldLength(0,"testchar"),40)
checkSuccess(cur.getFieldLength(0,"testvarchar"),12)
checkSuccess(cur.getFieldLength(0,"testbit"),1)
print "\n"
checkSuccess(cur.getFieldLength(7,"testint"),1)
checkSuccess(cur.getFieldLength(7,"testsmallint"),1)
checkSuccess(cur.getFieldLength(7,"testtinyint"),1)
#checkSuccess(cur.getFieldLength(7,"testreal"),3)
#checkSuccess(cur.getFieldLength(7,"testfloat"),3)
checkSuccess(cur.getFieldLength(7,"testdecimal"),3)
checkSuccess(cur.getFieldLength(7,"testnumeric"),3)
checkSuccess(cur.getFieldLength(7,"testmoney"),4)
checkSuccess(cur.getFieldLength(7,"testsmallmoney"),4)
checkSuccess(cur.getFieldLength(7,"testdatetime"),19)
checkSuccess(cur.getFieldLength(7,"testsmalldatetime"),19)
checkSuccess(cur.getFieldLength(7,"testchar"),40)
checkSuccess(cur.getFieldLength(7,"testvarchar"),12)
checkSuccess(cur.getFieldLength(7,"testbit"),1)
print "\n"

print "FIELDS BY ARRAY: \n"
fields=cur.getRow(0)
checkSuccess(fields[0],"1")
checkSuccess(fields[1],"1")
checkSuccess(fields[2],"1")
#checkSuccess(fields[3],"1.1")
#checkSuccess(fields[4],"1.1")
checkSuccess(fields[5],"1.1")
checkSuccess(fields[6],"1.1")
checkSuccess(fields[7],"1.00")
checkSuccess(fields[8],"1.00")
checkSuccess(fields[9],"Jan  1 2001  1:00AM")
checkSuccess(fields[10],"Jan  1 2001  1:00AM")
checkSuccess(fields[11],"testchar1                               ")
checkSuccess(fields[12],"testvarchar1")
checkSuccess(fields[13],"1")
print "\n"

print "FIELD LENGTHS BY ARRAY: \n"
fieldlens=cur.getRowLengths(0)
checkSuccess(fieldlens[0],1)
checkSuccess(fieldlens[1],1)
checkSuccess(fieldlens[2],1)
#checkSuccess(fieldlens[3],3)
#checkSuccess(fieldlens[4],3)
checkSuccess(fieldlens[5],3)
checkSuccess(fieldlens[6],3)
checkSuccess(fieldlens[7],4)
checkSuccess(fieldlens[8],4)
checkSuccess(fieldlens[9],19)
checkSuccess(fieldlens[10],19)
checkSuccess(fieldlens[11],40)
checkSuccess(fieldlens[12],12)
checkSuccess(fieldlens[13],1)
print "\n"

print "FIELDS BY HASH: \n"
fields=cur.getRowHash(0)
checkSuccess(fields["testint"],"1")
checkSuccess(fields["testsmallint"],"1")
checkSuccess(fields["testtinyint"],"1")
#checkSuccess(fields["testreal"],"1.1")
#checkSuccess(fields["testfloat"],"1.1")
checkSuccess(fields["testdecimal"],"1.1")
checkSuccess(fields["testnumeric"],"1.1")
checkSuccess(fields["testmoney"],"1.00")
checkSuccess(fields["testsmallmoney"],"1.00")
checkSuccess(fields["testdatetime"],"Jan  1 2001  1:00AM")
checkSuccess(fields["testsmalldatetime"],"Jan  1 2001  1:00AM")
checkSuccess(fields["testchar"],"testchar1                               ")
checkSuccess(fields["testvarchar"],"testvarchar1")
checkSuccess(fields["testbit"],"1")
print "\n"
fields=cur.getRowHash(7)
checkSuccess(fields["testint"],"8")
checkSuccess(fields["testsmallint"],"8")
checkSuccess(fields["testtinyint"],"8")
#checkSuccess(fields["testreal"],"8.8")
#checkSuccess(fields["testfloat"],"8.8")
checkSuccess(fields["testdecimal"],"8.8")
checkSuccess(fields["testnumeric"],"8.8")
checkSuccess(fields["testmoney"],"8.00")
checkSuccess(fields["testsmallmoney"],"8.00")
checkSuccess(fields["testdatetime"],"Jan  1 2008  8:00AM")
checkSuccess(fields["testsmalldatetime"],"Jan  1 2008  8:00AM")
checkSuccess(fields["testchar"],"testchar8                               ")
checkSuccess(fields["testvarchar"],"testvarchar8")
checkSuccess(fields["testbit"],"1")
print "\n"

print "FIELD LENGTHS BY HASH: \n"
fieldlengths=cur.getRowLengthsHash(0)
checkSuccess(fieldlengths["testint"],1)
checkSuccess(fieldlengths["testsmallint"],1)
checkSuccess(fieldlengths["testtinyint"],1)
#checkSuccess(fieldlengths["testreal"],3)
#checkSuccess(fieldlengths["testfloat"],3)
checkSuccess(fieldlengths["testdecimal"],3)
checkSuccess(fieldlengths["testnumeric"],3)
checkSuccess(fieldlengths["testmoney"],4)
checkSuccess(fieldlengths["testsmallmoney"],4)
checkSuccess(fieldlengths["testdatetime"],19)
checkSuccess(fieldlengths["testsmalldatetime"],19)
checkSuccess(fieldlengths["testchar"],40)
checkSuccess(fieldlengths["testvarchar"],12)
checkSuccess(fieldlengths["testbit"],1)
print "\n"
fieldlengths=cur.getRowLengthsHash(7)
checkSuccess(fieldlengths["testsmallint"],1)
checkSuccess(fieldlengths["testtinyint"],1)
#checkSuccess(fieldlengths["testreal"],3)
#checkSuccess(fieldlengths["testfloat"],3)
checkSuccess(fieldlengths["testdecimal"],3)
checkSuccess(fieldlengths["testnumeric"],3)
checkSuccess(fieldlengths["testmoney"],4)
checkSuccess(fieldlengths["testsmallmoney"],4)
checkSuccess(fieldlengths["testdatetime"],19)
checkSuccess(fieldlengths["testsmalldatetime"],19)
checkSuccess(fieldlengths["testchar"],40)
checkSuccess(fieldlengths["testvarchar"],12)
checkSuccess(fieldlengths["testbit"],1)
print "\n"

print "INDIVIDUAL SUBSTITUTIONS: \n"
cur.prepareQuery("select $(var1),'$(var2)',$(var3)")
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

print "ARRAY SUBSTITUTIONS: \n"
cur.prepareQuery("select $(var1),'$(var2)',$(var3)")
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
checkSuccess(cur.sendQuery("select NULL,1,NULL"),1)
checkSuccess(cur.getField(0,0),nil)
checkSuccess(cur.getField(0,1),"1")
checkSuccess(cur.getField(0,2),nil)
cur.getNullsAsEmptyStrings()
checkSuccess(cur.sendQuery("select NULL,1,NULL"),1)
checkSuccess(cur.getField(0,0),"")
checkSuccess(cur.getField(0,1),"1")
checkSuccess(cur.getField(0,2),"")
cur.getNullsAsNils()
print "\n"

print "RESULT SET BUFFER SIZE: \n"
checkSuccess(cur.getResultSetBufferSize(),0)
cur.setResultSetBufferSize(2)
checkSuccess(cur.sendQuery("select * from testtable order by testint"),1)
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
checkSuccess(cur.sendQuery("select * from testtable order by testint"),1)
checkSuccess(cur.getColumnName(0),nil)
checkSuccess(cur.getColumnLength(0),0)
checkSuccess(cur.getColumnType(0),nil)
cur.getColumnInfo()
checkSuccess(cur.sendQuery("select * from testtable order by testint"),1)
checkSuccess(cur.getColumnName(0),"testint")
checkSuccess(cur.getColumnLength(0),4)
checkSuccess(cur.getColumnType(0),"INT")
print "\n"

print "SUSPENDED SESSION: \n"
checkSuccess(cur.sendQuery("select * from testtable order by testint"),1)
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
checkSuccess(cur.sendQuery("select * from testtable order by testint"),1)
cur.suspendResultSet()
checkSuccess(con.suspendSession(),1)
port=con.getConnectionPort()
socket=con.getConnectionSocket()
checkSuccess(con.resumeSession(port,socket),1)
checkSuccess(cur.getField(0,0),"1")
checkSuccess(cur.getField(1,0),"2")
checkSuccess(cur.getField(2,0),"3")
checkSuccess(cur.getField(3,0),"4")
checkSuccess(cur.getField(4,0),"5")
checkSuccess(cur.getField(5,0),"6")
checkSuccess(cur.getField(6,0),"7")
checkSuccess(cur.getField(7,0),"8")
print "\n"
checkSuccess(cur.sendQuery("select * from testtable order by testint"),1)
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
checkSuccess(cur.sendQuery("select * from testtable order by testint"),1)
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
checkSuccess(cur.sendQuery("select * from testtable order by testint"),1)
filename=cur.getCacheFileName()
checkSuccess(filename,"cachefile1")
cur.cacheOff()
checkSuccess(cur.openCachedResultSet(filename),1)
checkSuccess(cur.getField(7,0),"8")
print "\n"

print "COLUMN COUNT FOR CACHED RESULT SET: \n"
checkSuccess(cur.colCount(),14)
print "\n"

print "COLUMN NAMES FOR CACHED RESULT SET: \n"
checkSuccess(cur.getColumnName(0),"testint")
checkSuccess(cur.getColumnName(1),"testsmallint")
checkSuccess(cur.getColumnName(2),"testtinyint")
checkSuccess(cur.getColumnName(3),"testreal")
checkSuccess(cur.getColumnName(4),"testfloat")
checkSuccess(cur.getColumnName(5),"testdecimal")
checkSuccess(cur.getColumnName(6),"testnumeric")
checkSuccess(cur.getColumnName(7),"testmoney")
checkSuccess(cur.getColumnName(8),"testsmallmoney")
checkSuccess(cur.getColumnName(9),"testdatetime")
checkSuccess(cur.getColumnName(10),"testsmalldatetime")
checkSuccess(cur.getColumnName(11),"testchar")
checkSuccess(cur.getColumnName(12),"testvarchar")
checkSuccess(cur.getColumnName(13),"testbit")
cols=cur.getColumnNames()
checkSuccess(cols[0],"testint")
checkSuccess(cols[1],"testsmallint")
checkSuccess(cols[2],"testtinyint")
checkSuccess(cols[3],"testreal")
checkSuccess(cols[4],"testfloat")
checkSuccess(cols[5],"testdecimal")
checkSuccess(cols[6],"testnumeric")
checkSuccess(cols[7],"testmoney")
checkSuccess(cols[8],"testsmallmoney")
checkSuccess(cols[9],"testdatetime")
checkSuccess(cols[10],"testsmalldatetime")
checkSuccess(cols[11],"testchar")
checkSuccess(cols[12],"testvarchar")
checkSuccess(cols[13],"testbit")
print "\n"

print "CACHED RESULT SET WITH RESULT SET BUFFER SIZE: \n"
cur.setResultSetBufferSize(2)
cur.cacheToFile("cachefile1")
cur.setCacheTtl(200)
checkSuccess(cur.sendQuery("select * from testtable order by testint"),1)
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
checkSuccess(cur.sendQuery("select * from testtable order by testint"),1)
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

print "FINISHED SUSPENDED SESSION: \n"
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
checkSuccess(cur.getField(4,0),nil)
checkSuccess(cur.getField(5,0),nil)
checkSuccess(cur.getField(6,0),nil)
checkSuccess(cur.getField(7,0),nil)
print "\n"

# drop existing table
cur.sendQuery("drop table testtable")

# invalid queries...
print "INVALID QUERIES: \n"
checkSuccess(cur.sendQuery("select * from testtable order by testint"),0)
checkSuccess(cur.sendQuery("select * from testtable order by testint"),0)
checkSuccess(cur.sendQuery("select * from testtable order by testint"),0)
checkSuccess(cur.sendQuery("select * from testtable order by testint"),0)
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



