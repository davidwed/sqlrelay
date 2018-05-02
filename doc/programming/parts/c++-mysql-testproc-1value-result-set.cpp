cur->sendQuery("select exampleproc()");
char    *result=cur->getFieldByIndex(0,0);
