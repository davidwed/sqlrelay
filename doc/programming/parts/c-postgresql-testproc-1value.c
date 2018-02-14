sqlrcur_prepareQuery(cur,"select * from examplefunc(1,2,3)");
sqlrcur_inputBindLong(cur,"1",1);
sqlrcur_inputBindDouble(cur,"2",1.1,4,2);
sqlrcur_inputBindString(cur,"3","hello");
sqlrcur_executeQuery(cur);
char    *result=sqlrcur_getFieldByIndex(cur,0,0);
