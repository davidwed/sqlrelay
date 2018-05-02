require('sqlrelay')

... get port and socket from previous page ...

con=SQLRConnection.new("sqlrserver",9000,"/tmp/example.socket","user","password",0,1)
cur=SQLRCursor.new(con)

con.resumeSession(port,socket)
cur.resumeResultSet(rs)
cur.sendQuery("commit")
con.endSession()
