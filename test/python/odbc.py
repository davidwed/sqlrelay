#! /usr/bin/env python

# Copyright (c) 1999-2018 David Muse
# See the file COPYING for more information.

from SQLRelay import PySQLRClient
import sys
import string

def checkSuccess(value,success):
	if value==success:
		print("success")
	else:
		print("wanted", type(success), ":", success)
		print("got   ", type(value), ":", value)
		print("failure")
		sys.exit(1)

def main():

	PySQLRClient.getNumericFieldsAsNumbers()

	# instantiation
	con=PySQLRClient.sqlrconnection("sqlrelay",9000,
						"/tmp/test.socket",
						"test","test")
	cur=PySQLRClient.sqlrcursor(con)

	# get database type
	print("IDENTIFY: ")
	checkSuccess(con.identify(),"odbc")

	# ping
	print("PING: ")
	checkSuccess(con.ping(),1)
	print()

	# drop existing table
	cur.sendQuery("drop table testtable")

	# create a new table
	print("CREATE TEMPTABLE: ")
	checkSuccess(cur.sendQuery("create table testtable (testint int, testchar char(40), testvarchar varchar(40), testdate date)"),1)
	print()

	print("INSERT: ")
	checkSuccess(cur.sendQuery("insert into testtable values (1,'testchar1','testvarchar1','01-JAN-2001')"),1)
	checkSuccess(cur.sendQuery("insert into testtable values (2,'testchar2','testvarchar2','02-JAN-2002')"),1)
	checkSuccess(cur.sendQuery("insert into testtable values (3,'testchar3','testvarchar3','03-JAN-2003')"),1)
	checkSuccess(cur.sendQuery("insert into testtable values (4,'testchar4','testvarchar4','04-JAN-2004')"),1)
	checkSuccess(cur.sendQuery("insert into testtable values (5,'testchar5','testvarchar5','05-JAN-2005')"),1)
	checkSuccess(cur.sendQuery("insert into testtable values (6,'testchar6','testvarchar6','06-JAN-2006')"),1)
	checkSuccess(cur.sendQuery("insert into testtable values (7,'testchar7','testvarchar7','07-JAN-2007')"),1)
	checkSuccess(cur.sendQuery("insert into testtable values (8,'testchar8','testvarchar8','08-JAN-2008')"),1)
	print()

	print("FINISHED SUSPENDED SESSION: ")
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
	print()

	# drop existing table
	cur.sendQuery("drop table testtable")

	# invalid queries...
	print("INVALID QUERIES: ")
	checkSuccess(cur.sendQuery("select * from testtable"),0)
	checkSuccess(cur.sendQuery("select * from testtable"),0)
	checkSuccess(cur.sendQuery("select * from testtable"),0)
	checkSuccess(cur.sendQuery("select * from testtable"),0)
	print()
	checkSuccess(cur.sendQuery("insert into testtable values (1,2,3,4)"),0)
	checkSuccess(cur.sendQuery("insert into testtable values (1,2,3,4)"),0)
	checkSuccess(cur.sendQuery("insert into testtable values (1,2,3,4)"),0)
	checkSuccess(cur.sendQuery("insert into testtable values (1,2,3,4)"),0)
	print()
	checkSuccess(cur.sendQuery("create table testtable"),0)
	checkSuccess(cur.sendQuery("create table testtable"),0)
	checkSuccess(cur.sendQuery("create table testtable"),0)
	checkSuccess(cur.sendQuery("create table testtable"),0)
	print()

if __name__ == "__main__":
	main()
