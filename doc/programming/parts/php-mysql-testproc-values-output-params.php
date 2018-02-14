sqlrcur_sendQuery($cur,"set \@out1=0, \@out2=0.0, \@out3=''");
sqlrcur_sendQuery($cur,"call exampleproc(\@out1,\@out2,\@out3)");
sqlrcur_sendQuery($cur,"select \@out1,\@out2,\@out3");
var $out1=sqlrcur_getField($cur,0,0);
var $out2=sqlrcur_getField($cur,0,1);
var $out3=sqlrcur_getField($cur,0,2);
