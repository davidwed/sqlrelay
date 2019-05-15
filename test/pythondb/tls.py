#! /usr/bin/env python

# Copyright (c) 1999-2018 David Muse
# See the file COPYING for more information.

from SQLRelay import PySQLRDB
from SQLRelay import CSQLRelay
import sys
import string
import platform


def main():

	tlscert="/usr/local/firstworks/etc/sqlrelay.conf.d/client.pem"
	tlsca="/usr/local/firstworks/etc/sqlrelay.conf.d/ca.pem"
	if platform.system()=="Windows":
		tlscert="C:\\Program Files\\Firstworks\\etc\\sqlrelay.conf.d\\client.pfx"
		tlsca="C:\\Program Files\\Firstworks\\etc\\sqlrelay.conf.d\\ca.pfx"

	CSQLRelay.getNumericFieldsAsNumbers()

	# instantiation
	print("INSTANTIATION")
	con=PySQLRDB.connect("sqlrelay",9000,"/tmp/test.socket","","",0,1,tls='yes',tlscert=tlscert,tlsvalidate='ca',tlsca=tlsca)
	cur=con.cursor()
	print()
	print()


	# bind functions
	print("BIND FUNCTIONS")
	cur.execute("select :var1,:var2,:var3 from dual",{'var1':1,'var2':'hello','var3':1.1})
	print(cur.fetchone())
	print()
	print()

	
	# executemany
	print("BIND FUNCTIONS")

	try:
		cur.execute("drop table temptable")
	except (PySQLRDB.DatabaseError) as e:
		print(e)

	cur.execute("create table temptable (col1 number, col2 char(10), col3 number(2,1))")
	cur.execute("select * from temptable")
	print(cur.fetchmany(1))

	cur.executemany("insert into temptable values (:var1,:var2,:var3)",[{'var1':1,'var2':'hello','var3':1.1},{'var1':2,'var2':'hi','var3':2.2},{'var1':3,'var2':'bye','var3':3.3}])
	#cur.executemany("select :var1,:var2,:var3 from dual",[{'var1':1,'var2':'hello','var3':1.1},{'var1':2,'var2':'hi','var3':2.2},{'var1':3,'var2':'bye','var3':3.3}])
	cur.execute("select * from temptable")
	print(cur.fetchall())
	cur.execute("drop table temptable")
	print()
	print()

	# lots of rows
	print("LOTS OF ROWS")
	cur.execute("create table temptable (col1 number)")

	# empty result set
	cur.execute("select * from temptable")

	counter=0
	for counter in range(0,200):
		cur.execute("insert into temptable values (1)")

	cur.execute("select * from temptable")
	counter=0
	for counter in range(0,200):
		if cur.fetchone() == 0:
			break

	if counter == 199:
		print("success")
	else:
		print("failed counter = ")
		print(counter)

	# clean up
	cur.execute("drop table temptable")

	# callproc
	print("CALLPROC")
	cur.callproc("select :var1,:var2,:var3 from dual",{'var1':1,'var2':'hello','var3':1.1})
	print(cur.fetchone())
	print()
	print()

	cur.close()
	con.close()
	del cur
	del con

	# make sure we don't get a segfault
	try :
		cur.execute("select 1 from dual");
	except (UnboundLocalError) as e:
		print(e)

if __name__ == '__main__':
	main()
