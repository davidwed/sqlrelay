from SQLRelay import PySQLRDB

con=PySQLRDB.connect('sqlrserver',9000,'/tmp/example.socket','user','password',0,1)
cur=con.cursor()

... execute some queries ...
