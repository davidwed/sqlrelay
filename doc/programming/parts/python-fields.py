from SQLRelay import PySQLRClient

con=PySQLRClient.sqlrconnection('sqlrserver',9000,'/tmp/example.socket','user','password',0,1)
cur=PySQLRClient.sqlrcursor(con)

cur.sendQuery('select * from my_table')
con.endSession()

for row in range(0,cur.rowCount()-1):
        for col in range(0,cur.colCount()-1):
                print cur.getField(row,col), ',',
        print cur.getField(row,col)
