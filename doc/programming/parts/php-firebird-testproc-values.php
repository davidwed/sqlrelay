sqlrcur_prepareQuery($cur,"select * from exampleproc(?,?,?)");
sqlrcur_inputBind($cur,"1",1);
sqlrcur_inputBind($cur,"2",1.1,2,1);
sqlrcur_inputBind($cur,"3","hello");
sqlrcur_executeQuery($cur);
$out1=sqlrcur_getField($cur,0,0);
$out2=sqlrcur_getField($cur,0,1);
$out3=sqlrcur_getField($cur,0,2);
