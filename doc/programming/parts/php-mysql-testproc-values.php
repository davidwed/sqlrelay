sqlrcur_prepareQuery($cur,"call exampleproc(?,?,?)");
sqlrcur_inputBindLong($cur,"1",1);
sqlrcur_inputBindDouble($cur,"2",1.1,4,2);
sqlrcur_inputBindString($cur,"3","hello");
sqlrcur_executeQuery($cur);
var $out1=sqlrcur_getFieldByIndex($cur,0,0);
var $out2=sqlrcur_getFieldByIndex($cur,0,1);
var $out3=sqlrcur_getFieldByIndex($cur,0,2);
