sqlrcur_prepareQuery($cur,"select exampleproc(:in1,:in2,:in3) from dual");
sqlrcur_inputBind($cur,"in1",1);
sqlrcur_inputBind($cur,"in2",1.1,2,1);
sqlrcur_inputBind($cur,"in3","hello");
sqlrcur_executeQuery($cur);
$result=sqlrcur_getField($cur,0,0);
