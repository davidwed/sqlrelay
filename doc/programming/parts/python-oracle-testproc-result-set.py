cur.prepareQuery("begin  :curs:=testproc; end;")
cur.defineOutputBindCursor("curs")
cur.executeQuery()
bindcur=cur.getOutputBindCursor("curs")
bindcur.fetchFromBindCursor()
field00=bindcur.getField(0,0)
field01=bindcur.getField(0,1)
field02=bindcur.getField(0,2)
field10=bindcur.getField(1,0)
field11=bindcur.getField(1,1)
field12=bindcur.getField(1,2)
