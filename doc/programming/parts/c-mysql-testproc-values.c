sqlrcur_prepareQuery(cur,"call testproc(?,?,?)");
sqlrcur_inputBindLong(cur,"1",1);
sqlrcur_inputBindDouble(cur,"2",1.1,4,2);
sqlrcur_inputBindString(cur,"3","hello");
sqlrcur_executeQuery(cur);
char    *out1=sqlrcur_getFieldByIndex(cur,0,0);
char    *out2=sqlrcur_getFieldByIndex(cur,0,1);
char    *out3=sqlrcur_getFieldByIndex(cur,0,2);
