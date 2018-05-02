<?
     $con=sqlrcon_alloc("sqlrserver",9000,"/tmp/example.socket","user","password",0,1);
     $cur=sqlrcur_alloc($con);

     sqlrcur_prepareQuery($cur,"begin  :result1:=addTwoIntegers(:integer1,:integer2);  :result2=addTwoFloats(:float1,:float2);  :result3=convertToString(:integer3); end;");
     sqlrcur_inputBind($cur,"integer1",10);
     sqlrcur_inputBind($cur,"integer2",20);
     sqlrcur_inputBind($cur,"float1",1.1,2,1);
     sqlrcur_inputBind($cur,"float2",2.2,2,1);
     sqlrcur_inputBind($cur,"integer3",30);
     sqlrcur_defineOutputBindInteger($cur,"result1");
     sqlrcur_defineOutputBindDouble($cur,"result2");
     sqlrcur_defineOutputBindString($cur,"result3",100);
     sqlrcur_executeQuery($cur);
     $result1=sqlrcur_getOutputBindInteger($cur,"result1");
     $result2=sqlrcur_getOutputBindDouble($cur,"result2");
     $result3=sqlrcur_getOutputBindString($cur,"result3");
     sqlrcon_endSession($con);

     ... do something with the result ...

     sqlrcur_free($cur);
     sqlrcon_free($con);
?>
