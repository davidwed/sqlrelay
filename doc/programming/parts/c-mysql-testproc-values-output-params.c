sqlrcur_sendQuery(cur,"set @out1=0, @out2=0.0, @out3=''");
sqlrcur_sendQuery(cur,"call exampleproc(@out1,@out3,@out3)");
sqlrcur_sendQuery(cur,"select @out1,@out2,@out3");
char    *out1=sqlrcur_getFieldByIndex(cur,0,0);
char    *out2=sqlrcur_getFieldByIndex(cur,0,1);
char    *out3=sqlrcur_getFieldByIndex(cur,0,2);
