sqlrcur_prepareQuery($cur,"begin exampleproc(:in1,:in2,:in3,:out1); end;");
sqlrcur_inputBind($cur,"in1",1);
sqlrcur_inputBind($cur,"in2",1.1,2,1);
sqlrcur_inputBind($cur,"in3","hello");
sqlrcur_defineOutputBind($cur,"out1",20);
sqlrcur_executeQuery($cur);
$result=sqlrcur_getOutputBind($cur,"out1");
