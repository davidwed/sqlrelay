cur->sendQuery("set @out1=0");
cur->sendQuery("call testproc(@out1)");
cur->sendQuery("select @out1");
char    *result=cur->getFieldByIndex(0,0);
