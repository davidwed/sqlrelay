require('sqlrelay')

con=SQLRConnection.new("sqlrserver",9000,"/tmp/example.socket","user","password",0,1)
cursor1=SQLRCursor.new(con)
cursor2=SQLRCursor.new(con)

cursor1.setResultSetBufferSize(10)
cursor1.sendQuery("select * from my_huge_table")

index=0
while cursor1.endOfResultSet()!=1 do
        cursor2.prepareQuery("insert into my_other_table values (:1,:2,:3)")
        cursor2.inputBind("1",cursor1.getField(index,1))
        cursor2.inputBind("2",cursor1.getField(index,2))
        cursor2.inputBind("3",cursor1.getField(index,3))
        cursor2.executeQuery()
end 
