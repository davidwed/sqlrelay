sqlrcur_prepareQuery(cur,"select * from exampleproc(?,?,?)");
sqlrcur_inputBindLong(cur,"1",1);
sqlrcur_inputBindDouble(cur,"2",1.1,2,1);
sqlrcur_inputBindString(cur,"3","hello");
sqlrcur_executeQuery(cur);
char    *result=sqlrcur_getFieldByIndex(cur,0,0);
