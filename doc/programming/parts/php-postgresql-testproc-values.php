sqlrcur_prepareQuery($cur,"select * from examplefunc($1,$2,$3) as (col1 int, col2 float, col3 char(20))");
sqlrcur_inputBind($cur,"1",1);
sqlrcur_inputBind($cur,"2",1.1,4,2);
sqlrcur_inputBind($cur,"3","hello");
sqlrcur_executeQuery($cur);
$out1=sqlrcur_getField($cur,0,0);
$out2=sqlrcur_getField($cur,0,1);
$out3=sqlrcur_getField($cur,0,2);
