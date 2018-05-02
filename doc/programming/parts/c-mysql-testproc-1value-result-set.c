sqlrcur_sendQuery(cur,"select exampleproc()");
char    *result=sqlrcur_getFieldByIndex(cur,0,0);
