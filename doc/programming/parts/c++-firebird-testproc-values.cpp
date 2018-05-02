cur->prepareQuery("select * from exampleproc(?,?,?)");
cur->inputBind("1",1);
cur->inputBind("2",1.1,2,1);
cur->inputBind("3","hello");
cur->executeQuery();
char    *out1=cur->getField(0,0);
char    *out2=cur->getField(0,1);
char    *out3=cur->getField(0,2);
