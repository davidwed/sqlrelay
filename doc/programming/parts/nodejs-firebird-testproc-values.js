cur.prepareQuery("select * from exampleproc(?,?,?)");
cur.inputBind("1",1);
cur.inputBind("2",1.1,2,1);
cur.inputBind("3","hello");
cur.executeQuery();
var     out1=cur.getField(0,0);
var     out2=cur.getField(0,1);
var     out3=cur.getField(0,2);
