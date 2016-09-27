sqlrcur_sendQuery(cur,"select testproc()");
char    *result=sqlrcur_getFieldByIndex(cur,0,0);
