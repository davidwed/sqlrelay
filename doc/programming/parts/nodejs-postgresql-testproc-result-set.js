cur.sendQuery("select * from examplefunc() as (exampleint int, examplefloat float, examplechar char(40))");
var     field00=cur.getField(0,0);
var     field01=cur.getField(0,1);
var     field02=cur.getField(0,2);
var     field10=cur.getField(1,0);
var     field11=cur.getField(1,1);
var     field12=cur.getField(1,2);
