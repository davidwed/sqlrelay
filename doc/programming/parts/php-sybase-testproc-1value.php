sqlrcur_prepareQuery($cur,"exec testproc");
sqlrcur_inputBind($cur,"in1",1);
sqlrcur_inputBind($cur,"in2",1.1,2,1);
sqlrcur_inputBind($cur,"in3","hello");
sqlrcur_defineOutputBind($cur,"out1",20);
sqlrcur_executeQuery($cur);
$result=sqlrcur_getOutputBind($cur,"out1");
