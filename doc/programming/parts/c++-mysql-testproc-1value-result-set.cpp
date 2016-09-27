cur->sendQuery("select testproc()");
char    *result=cur->getFieldByIndex(0,0);
