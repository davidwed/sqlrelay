require('sqlrelay')

con=SQLRConnection.new("sqlrserver",9000,"/tmp/test.socket","user","password",0,1)
cur=SQLRCursor.new(con)

... execute some queries ...
