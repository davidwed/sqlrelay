from SQLRelay import PySQLRClient

con=PySQLRClient.sqlrconnection('sqlrserver',9000,'/tmp/test.socket','user','password',0,1)
cur=PySQLRClient.sqlrcursor(con)

... execute some queries ...
