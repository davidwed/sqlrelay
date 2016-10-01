cur.prepareQuery("select testproc()")
cur.executeQuery()
result=cur.getField(0,0)
