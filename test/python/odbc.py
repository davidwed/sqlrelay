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
		print "usage: odbc.py host port socket user password"
		sys.exit(0)


	# instantiation
	con=PySQLRClient.sqlrconnection(sys.argv[1],string.atoi(sys.argv[2]), 
					sys.argv[3],sys.argv[4],sys.argv[5])
	cur=PySQLRClient.sqlrcursor(con)

	# get database type
	print "IDENTIFY: "
	checkSuccess(con.identify(),"odbc")

	# ping
	print "PING: "
	checkSuccess(con.ping(),1)
	print

	# drop existing table
	cur.sendQuery("drop table testtable")

	# create a new table
	print "CREATE TEMPTABLE: "
	checkSuccess(cur.sendQuery("create table testtable (testnumber number, testchar char(40), testvarchar varchar2(40), testdate date)"),1)

	print "INSERT: "
	checkSuccess(cur.sendQuery("insert into testtable values (1,'testchar1','testvarchar1','01-JAN-2001')"),1)

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
