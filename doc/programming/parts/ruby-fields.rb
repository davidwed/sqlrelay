require('sqlrelay')

con=SQLRConnection.new("sqlrserver",9000,"/tmp/example.socket","user","password",0,1)
cur=SQLRCursor.new(con)

cur.sendQuery("select * from my_table")
con.endSession()

for row in 0..cur.rowCount()-1 do
        for col in 0..cur.colCount()-1 do
                puts cur.getField(row,col)
                puts ","
        end
        puts "\n"
end
