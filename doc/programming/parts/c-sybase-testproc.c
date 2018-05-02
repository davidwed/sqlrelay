sqlrcur_prepareQuery(cur,"exec exampleproc");
sqlrcur_inputBindLong(cur,"in1",1);
sqlrcur_inputBindDouble(cur,"in2",1.1,2,1);
sqlrcur_inputBindString(cur,"in3","hello");
sqlrcur_executeQuery(cur);
