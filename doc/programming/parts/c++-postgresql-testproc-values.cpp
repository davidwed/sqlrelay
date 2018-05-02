cur->prepareQuery("select * from examplefunc(:in1,:in2,:in3) as (col1 int, col2 float, col3 char(20))");
cur->inputBind("in1",1);
cur->inputBind("in2",1.1,4,2);
cur->inputBind("in3","hello");
cur->executeQuery();
char    *out1=cur->getField(0,0);
char    *out2=cur->getField(0,1);
char    *out3=cur->getField(0,2);
