<?
     $con=sqlrcon_alloc("sqlrserver",9000,"/tmp/example.socket","user","password",0,1);

     ... execute some queries ...

     sqlrcon_free($con);
?>
