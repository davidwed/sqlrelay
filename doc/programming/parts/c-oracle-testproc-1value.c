sqlrcur_prepareQuery(cur,"select exampleproc(:in1,:in2,:in3) from dual");
sqlrcur_inputBindLong(cur,"in1",1);
sqlrcur_inputBindDouble(cur,"in2",1.1,2,1);
sqlrcur_inputBindString(cur,"in3","hello");
sqlrcur_executeQuery(cur);
char    *result=sqlrcur_getFieldByIndex(cur,0,0);
