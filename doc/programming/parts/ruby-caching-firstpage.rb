require('sqlrelay')

con=SQLRConnection.new("sqlrserver",9000,"/tmp/test.socket","user","password",0,1)
cur=SQLRCursor.new(con)

... generate a unique file name ...

cur.cacheToFile(filename)
cur.setCacheTtl(600)
cur.sendQuery("select * from my_table")
con.endSession()
cur.cacheOff()

... pass the filename to the next page ...
