<?
     ... get rs, port and socket from previous page ...

     $con=sqlrcon_alloc("sqlrserver",9000,"/tmp/example.socket","user","password",0,1);
     $cur=sqlrcur_alloc($con);

     sqlrcon_resumeSession($con,$port,$socket);
     sqlrcur_resumeResultSet($cur,$rs);
     sqlrcur_sendQuery($cur,"commit");
     sqlrcon_endSession($con);

     sqlrcur_free($cur);
     sqlrcon_free($con);
?>
