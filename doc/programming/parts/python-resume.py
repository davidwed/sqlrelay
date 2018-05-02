from SQLRelay import PySQLRClient

... get port and socket from previous page ...

con=PySQLRClient.sqlrconnection('sqlrserver',9000,'/tmp/example.socket','user','password',0,1)
cur=PySQLRClient.sqlrcursor(con)

con.resumeSession(port,socket)
cur.resumeResultSet(rs)
cur.sendQuery('commit')
con.endSession()
