from SQLRelay import PySQLRClient

con=PySQLRClient.sqlrconnection('sqlrserver',9000,'/tmp/example.socket','user','password',0,1)
cur=PySQLRClient.sqlrcursor(con)

cur.setResultSetBufferSize(5)

cur.sendQuery('select * from my_table')

done=0
row=0
while (!done):
        for col in range(0,cur.colCount()-1):
                if (field=cur.getField(row,col)):
                        print field, ',',
                else:
                        done=1
        print
        row++

cur.sendQuery('select * from my_other_table')

... process this querys result set in chunks also ...

cur.setResultSetBufferSize(0)

cur.sendQuery('select * from my_third_table')

... process this querys result set all at once ...

con.endSession()
