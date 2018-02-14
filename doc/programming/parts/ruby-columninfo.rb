require('sqlrelay')

con=SQLRConnection.new("sqlrserver",9000,"/tmp/example.socket","user","password",0,1)
cur=SQLRCursor.new(con)

cur.sendQuery("select * from my_table")
con.endSession()

for i in 0..cur.colCount()-1 do
        puts "Name:          "
        puts cur.getColumnName(i)
        puts "\n"
        puts "Type:          "
        puts cur.getColumnType(i)
        puts "\n"
        puts "Length:        "
        puts cur.getColumnLength(i)
        puts "\n"
        puts "Precision:     "
        puts cur.getColumnPrecision(i)
        puts "\n"
        puts "Scale:         "
        puts cur.getColumnScale(i)
        puts "\n"
        puts "Longest Field: "
        puts cur.getLongest(i)
        puts "\n"
        puts "Nullable:      "
        puts cur.getColumnIsNullable(i)
        puts "\n"
        puts "Primary Key:   "
        puts cur.getColumnIsPrimaryKey(i)
        puts "\n"
        puts "Unique:        "
        puts cur.getColumnIsUnique(i)
        puts "\n"
        puts "Part Of Key:   "
        puts cur.getColumnIsPartOfKey(i)
        puts "\n"
        puts "Unsigned:      "
        puts cur.getColumnIsUnsigned(i)
        puts "\n"
        puts "Zero Filled:   "
        puts cur.getColumnIsZeroFilled(i)
        puts "\n"
        puts "Binary:        "
        puts cur.getColumnIsBinary(i)
        puts "\n"
        puts "Auto Increment:"
        puts cur.getColumnIsAutoIncrement(i)
        puts "\n"
end
