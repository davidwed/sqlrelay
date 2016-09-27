sqlrcur_prepareQuery(cur,"select * from testfunc(1,2,3) as (col1 int, col2 float, col3 char(20))");
sqlrcur_inputBindLong(cur,"1",1);
sqlrcur_inputBindDouble(cur,"2",1.1,4,2);
sqlrcur_inputBindString(cur,"3","hello");
sqlrcur_executeQuery(cur);
char    *out1=sqlrcur_getFieldByIndex(cur,0,0);
char    *out2=sqlrcur_getFieldByIndex(cur,0,1);
char    *out3=sqlrcur_getFieldByIndex(cur,0,2);
