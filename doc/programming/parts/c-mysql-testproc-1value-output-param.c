sqlrcur_sendQuery(cur,"set @out1=0");
sqlrcur_sendQuery(cur,"call testproc(@out1)");
sqlrcur_sendQuery(cur,"select @out1");
char    *result=sqlrcur_getFieldByIndex(cur,0,0);
