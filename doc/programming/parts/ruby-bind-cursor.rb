require('sqlrelay')

con=SQLRConnection.new("sqlrserver",9000,"/tmp/example.socket","user","password",0,1)
cur=SQLRCursor.new(con)

cur.prepareQuery("begin  :curs:=sp_mytable end;")
cur.defineOutputBindCursor("curs")
cur.executeQuery()

bindcur=cur.getOutputBindCursor("curs")
bindcur.fetchFromBindCursor()

# print fields from table
for i in 0..bindcur.rowCount()-1 do
        for j in 0..bindcur.colCount()-1 do
                puts bindcur.getField(i,j)
                puts ", "
        end
        puts "\n"
end
