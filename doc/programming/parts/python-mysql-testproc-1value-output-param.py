cur.prepareQuery("set @out1=0")
cur.prepareQuery("call exampleproc()")
cur.prepareQuery("select @out1")
cur.executeQuery()
result=cur.getField(0,0)
