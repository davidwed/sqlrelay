require('sqlrelay')

con=SQLRConnection.new("sqlrserver",9000,"/tmp/example.socket","user","password",0,1)
cur=SQLRCursor.new(con)

cur.setResultSetBufferSize(5)

cur.sendQuery("select * from my_table")

done=0
row=0
while done!=0 do
        for col in 0..cur.colCount()-1 do
                if field=cur.getField(row,col) then
                        puts field
                        puts ","
                else
                        done=1
                end
        end
        puts "\n"
        row++
end

cur.sendQuery("select * from my_other_table")

... process this querys result set in chunks also ...

cur.setResultSetBufferSize(0)

cur.sendQuery("select * from my_third_table")

... process this querys result set all at once ...

con.endSession()
