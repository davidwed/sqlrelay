#! /usr/bin/env python

# Copyright (c) 2000-2001  David Muse
# See the file COPYING for more information.

from SQLRelay import PySQLRDB
import sys
import string


def main():

	# usage...
	if len(sys.argv) < 7:
		print "usage: pythondb.py host port socket user password query iterations"
		sys.exit(0)


	for loop in range(0,int(sys.argv[7])):

		# instantiation
		print "INSTANTIATION"
		con=PySQLRDB.connect(sys.argv[1],string.atoi(sys.argv[2]),
					sys.argv[3],sys.argv[4],sys.argv[5],0,1)
		cur=con.cursor()
		print
		print


		# query functions
		print "QUERY FUNCTIONS"
		cur.execute(sys.argv[6])
		print "Fetch One"
		print cur.fetchone()
		print
		print "Fetch Many"
		print cur.fetchmany(3)
		print
		print "Fetch All"
		print cur.fetchall()
		print
		print


		# bind functions
		print "BIND FUNCTIONS"
		cur.execute("select :var1,:var2,:var3 from dual",{'var1':1,'var2':'hello','var3':1.1})
		print cur.fetchone()
		print
		print
	
	
		# executemany
		print "BIND FUNCTIONS"
	
		try:
			cur.execute("drop table temptable")
		except PySQLRDB.DatabaseError, e:
			print e

		cur.execute("create table temptable (col1 number, col2 char(10), col3 number(2,1))")

		cur.executemany("insert into temptable values (:var1,:var2,:var3)",[{'var1':1,'var2':'hello','var3':1.1},{'var1':2,'var2':'hi','var3':2.2},{'var1':3,'var2':'bye','var3':3.3}])
		#cur.executemany("select :var1,:var2,:var3 from dual",[{'var1':1,'var2':'hello','var3':1.1},{'var1':2,'var2':'hi','var3':2.2},{'var1':3,'var2':'bye','var3':3.3}])
		cur.execute("select * from temptable")
		print cur.fetchall()
		cur.execute("drop table temptable")
		print
		print
	
	
		# callproc
		print "CALLPROC"
		cur.callproc("select :var1,:var2,:var3 from dual",{'var1':1,'var2':'hello','var3':1.1})
		print cur.fetchone()
		print
		print

		cur.close()
		con.close()
	
	
if __name__ == '__main__':
	main()
