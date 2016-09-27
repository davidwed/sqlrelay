sqlrcur_sendQuery(cur,"select * from testfunc() as (testint int, testfloat float, testchar char(40))");
char    *field00=sqlrcur_getFieldByIndex(cur,0,0);
char    *field01=sqlrcur_getFieldByIndex(cur,0,1);
char    *field02=sqlrcur_getFieldByIndex(cur,0,2);
char    *field10=sqlrcur_getFieldByIndex(cur,1,0);
char    *field11=sqlrcur_getFieldByIndex(cur,1,1);
char    *field12=sqlrcur_getFieldByIndex(cur,1,2);
