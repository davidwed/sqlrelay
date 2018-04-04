cur.sendQuery("set @out1=0, @out2=0.0, @out3=''");
cur.sendQuery("call exampleproc(@out1,@out3,@out3)");
cur.sendQuery("select @out1,@out2,@out3");
String out1=cur.getFieldByIndex(0, 0);
String out2=cur.getFieldByIndex(0, 1);
String out3=cur.getFieldByIndex(0, 2);
