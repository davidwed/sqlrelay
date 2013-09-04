#! /usr/bin/env python

from SQLRelay import PySQLRClient

def main():
	sqlrcon=PySQLRClient.sqlrconnection("examplehost",9000, \
						"/tmp/example.socket", \
						"exampleuser", \
						"examplepassword",0,1)
	sqlrcur=PySQLRClient.sqlrcursor(sqlrcon)

	sqlrcur.sendQuery("select * from exampletable")
	for row in range(0,sqlrcur.rowCount()):
		for col in range(0,sqlrcur.colCount()):
			print sqlrcur.getField(row,col)+",",
		print

if __name__ == "__main__":
	main()
