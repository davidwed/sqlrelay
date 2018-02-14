sqlrcur_prepareQuery($cur,"execute procedure exampleproc ?, ?, ?");
sqlrcur_inputBind($cur,"1",1);
sqlrcur_inputBind($cur,"2",1.1,2,1);
sqlrcur_inputBind($cur,"3","hello");
sqlrcur_defineOutputBind($cur,"1",20);
sqlrcur_executeQuery($cur);
$result=sqlrcur_getOutputBind($cur,"1");
