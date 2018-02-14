<?
     $con=sqlrcon_alloc("sqlrserver",9000,"/tmp/example.socket","user","password",0,1);
     $cur=sqlrcur_alloc($con);

     sqlrcur_prepareQuery($cur,"select * from mytable $(whereclause)")
     sqlrcur_substitution($cur,"whereclause","where stringcol=:stringval and integercol>:integerval and floatcol>floatval");
     sqlrcur_inputBind($cur,"stringval","true");
     sqlrcur_inputBind($cur,"integerval",10);
     sqlrcur_inputBind($cur,"floatval",1.1,2,1);
     sqlrcur_executeQuery($cur);

     ... process the result set ...

     sqlrcur_free($cur);
     sqlrcon_free($con);
?>
