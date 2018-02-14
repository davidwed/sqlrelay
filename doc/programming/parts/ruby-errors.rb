require('sqlrelay')

con=SQLRConnection.new("sqlrserver",9000,"/tmp/example.socket","user","password",0,1)
cur=SQLRCursor.new(con)

if !cur.sendQuery("select * from my_nonexistant_table") then
        puts cur.errorMessage()
        puts "\n"
end
