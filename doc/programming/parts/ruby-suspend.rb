require('sqlrelay')

con=SQLRConnection.new("sqlrserver",9000,"/tmp/example.socket","user","password",0,1)
cur=SQLRCursor.new(con)

cur.sendQuery("insert into my_table values (1,2,3)")
cur.suspendResultSet()
con.suspendSession()
rs=cur.getResultSetId()
port=con.getConnectionPort()
socket=con.getConnectionSocket()

... pass the rs, port and socket to the next page ...
