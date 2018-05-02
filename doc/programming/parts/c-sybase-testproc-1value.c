sqlrcur_prepareQuery(cur,"exec exampleproc");
sqlrcur_inputBindLong(cur,"in1",1);
sqlrcur_inputBindDouble(cur,"in2",1.1,2,1);
sqlrcur_inputBindString(cur,"in3","hello");
sqlrcur_defineOutputBindInteger(cur,"out1");
sqlrcur_executeQuery(cur);
int64_t    result=sqlrcur_getOutputBindInteger(cur,"out1");
