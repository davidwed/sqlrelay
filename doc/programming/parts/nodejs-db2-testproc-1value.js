cur.prepareQuery("call testproc(?,?,?,?)");
cur.inputBind("1",1);
cur.inputBind("2",1.1,2,1);
cur.inputBind("3","hello");
cur.defineOutputBindInteger("4");
cur.executeQuery();
var   result=cur.getOutputBindInteger("4");
