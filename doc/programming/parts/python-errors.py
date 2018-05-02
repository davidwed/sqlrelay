from SQLRelay import PySQLRClient

con=PySQLRClient.sqlrconnection('sqlrserver',9000,'/tmp/example.socket','user','password',0,1)
cur=PySQLRClient.sqlrcursor(con)

if (!cur.sendQuery('select * from my_nonexistant_table')):
        print cur.errorMessage()
