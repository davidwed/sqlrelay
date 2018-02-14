from SQLRelay import PySQLRClient

con=PySQLRClient.sqlrconnection('sqlrserver',9000,'/tmp/example.socket','user','password',0,1)
cur=PySQLRClient.sqlrcursor(con)

cur.prepareQuery('select * from mytable where mycolumn>:value')
cur.inputBind('value',1)
cur.executeQuery()

... process the result set ...

cur.clearBinds()
cur.inputBind('value',5)
cur.executeQuery()

... process the result set ...

cur.clearBinds()
cur.inputBind('value',10)
cur.executeQuery()

... process the result set ...
