from SQLRelay import PySQLRClient

con=PySQLRClient.sqlrconnection("sqlrserver",9000,"","user","password",0,1)
cur=PySQLRClient.sqlrcursor(con)

# column names will be forced to upper case
cur.upperCaseColumnNames()
cur.sendQuery("select * from my_table")
con.endSession()

for i in range(0,cur.colCount()-1):
        print "Name:  ", cur.getColumnName(i)

# column names will be forced to lower case
cur.lowerCaseColumnNames()
cur.sendQuery("select * from my_table")
con.endSession()

for i in range(0,cur.colCount()-1):
        print "Name:  ", cur.getColumnName(i)

# column names will be the same as they are in the database
cur.lowerCaseColumnNames()
cur.sendQuery("select * from my_table")
con.endSession()

for i in range(0,cur.colCount()-1):
        print "Name:  ", cur.getColumnName(i)
