<?
     $con=sqlrcon_alloc("sqlrserver",9000,"/tmp/example.socket","user","password",0,1);
     $cur=sqlrcur_alloc($con);

     if (!sqlrcur_sendQuery($cur,"select * from my_nonexistant_table")) {
             echo sqlrcur_errorMessage($cur);
             echo "\n";
     }

     sqlrcur_free($cur);
     sqlrcon_free($con);
?>
