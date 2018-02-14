require('sqlrelay')

con=SQLRConnection.new("sqlrserver",9000,"/tmp/example.socket","user","password",0,1)
cur=SQLRCursor.new(con)

# column names will be forced to upper case
cur.upperCaseColumnNames()
cur.sendQuery("select * from my_table")
con.endSession()

for i in 0..cur.colCount()-1 do
        puts "Name:  "
        puts cur.getColumnName(i)
        puts "\n"
end

# column names will be forced to lower case
cur.lowerCaseColumnNames()
cur.sendQuery("select * from my_table")
con.endSession()

for i in 0..cur.colCount()-1 do
        puts "Name:  "
        puts cur.getColumnName(i)
        puts "\n"
end

# column names will be the same as they are in the database
cur.mixedCaseColumnNames()
cur.sendQuery("select * from my_table")
con.endSession()

for i in 0..cur.colCount()-1 do
        puts "Name:  "
        puts cur.getColumnName(i)
        puts "\n"
end
