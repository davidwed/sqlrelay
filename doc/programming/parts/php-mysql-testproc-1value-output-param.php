sqlrcur_sendQuery($cur,"set @out1");
sqlrcur_sendQuery($cur,"call testproc()");
sqlrcur_sendQuery($cur,"select @out1");
var $result=sqlrcur_getFieldByIndex($cur,0,0);
