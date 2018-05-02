require('sqlrelay')

... get the filename from the previous page ...

... get the page to display from the previous page ...

con=SQLRConnection.new("sqlrserver",9000,"/tmp/example.socket","user","password",0,1)
cur=SQLRCursor.new(con)

cur.openCachedResultSet(filename)
con.endSession()

for row in pagetodisplay*20..((pagetodisplay+1)*20)-1 do
        for col in 0..cur.colCount()-1 do
                puts cur.getField(row,col)
                puts ","
        end
        puts "\n"
end
