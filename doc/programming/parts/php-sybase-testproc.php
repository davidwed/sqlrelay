sqlrcur_prepareQuery($cur,"exec exampleproc");
sqlrcur_inputBind($cur,"in1",1);
sqlrcur_inputBind($cur,"in2",1.1,2,1);
sqlrcur_inputBind($cur,"in3","hello");
sqlrcur_executeQuery($cur);
