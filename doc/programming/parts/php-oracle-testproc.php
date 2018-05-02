sqlrcur_prepareQuery($cur,"begin exampleproc(:in1,:in2,:in3); end;");
sqlrcur_inputBind($cur,"in1",1);
sqlrcur_inputBind($cur,"in2",1.1,2,1);
sqlrcur_inputBind($cur,"in3","hello");
sqlrcur_executeQuery($cur);
