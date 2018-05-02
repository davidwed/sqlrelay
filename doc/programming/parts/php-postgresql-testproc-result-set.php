sqlrcur_sendQuery($cur,"select * from examplefunc() as (exampleint int, examplefloat float, examplechar char(40))");
$field00=sqlrcur_getField($cur,0,0);
$field01=sqlrcur_getField($cur,0,1);
$field02=sqlrcur_getField($cur,0,2);
$field10=sqlrcur_getField($cur,1,0);
$field11=sqlrcur_getField($cur,1,1);
$field12=sqlrcur_getField($cur,1,2);
