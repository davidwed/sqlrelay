cur.prepareQuery("begin  :curs:=exampleproc; end;");
cur.defineOutputBindCursor("curs");
cur.executeQuery();
SQLRCursor bindcur=cur.getOutputBindCursor("curs");
bindcur.fetchFromBindCursor();
String field00=bindcur.getField(0, 0);
String field01=bindcur.getField(0, 1);
String field02=bindcur.getField(0, 2);
String field10=bindcur.getField(1, 0);
String field11=bindcur.getField(1, 1);
String field12=bindcur.getField(1, 2);
...
