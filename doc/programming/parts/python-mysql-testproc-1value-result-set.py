cur.prepareQuery("select exampleproc()")
cur.executeQuery()
result=cur.getField(0,0)
