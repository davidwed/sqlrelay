cur.prepareQuery("select examplefunc(?,?,?)");
cur.inputBind("1",1);
cur.inputBind("2",1.1,4,2);
cur.inputBind("3","hello");
cur.executeQuery();
String   result=cur.getField(0,0);
