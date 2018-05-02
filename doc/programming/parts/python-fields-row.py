from SQLRelay import PySQLRClient

con=PySQLRClient.sqlrconnection('sqlrserver',9000,'/tmp/example.socket','user','password',0,1)
cur=PySQLRClient.sqlrcursor(con)

cur.sendQuery('select * from my_table')
con.endSession()

for row in range(0,cur.rowCount()-1):
        rowarray=cur.getRow(row)
        for col in (0,cur.colCount()):
                print rowarray[col], ',',
        print
