require('sqlrelay')

con=SQLRConnection.new("sqlrserver",9000,"/tmp/example.socket","user","password",0,1)
cur=SQLRCursor.new(con)

cur.prepareQuery("begin  :result1:=addTwoIntegers(:integer1,:integer2);  :result2=addTwoFloats(:float1,:float2);  :result3=convertToString(:integer3); end;")
cur.inputBind("integer1",10)
cur.inputBind("integer2",20)
cur.inputBind("float1",1.1,2,1)
cur.inputBind("float2",2.2,2,1)
cur.inputBind("integer3",30)
cur.defineOutputBindInteger("result1")
cur.defineOutputBindDouble("result2")
cur.defineOutputBindString("result3",100)
cur.executeQuery()
result1=cur.getOutputBindInteger("result1")
result2=cur.getOutputBindDouble("result2")
result3=cur.getOutputBindString("result3")
con.endSession()

... do something with the result ...
