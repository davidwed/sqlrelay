<?
     ... get the filename from the previous page ...

     ... get the page to display from the previous page ...

     $con=sqlrcon_alloc("sqlrserver",9000,"/tmp/example.socket","user","password",0,1);
     $cur=sqlrcur_alloc($con);

     sqlrcur_openCachedResultSet($cur,filename);
     sqlrcon_endSession($con);

     for ($row=$pagetodisplay*20; $row<($pagetodisplay+1)*20; $row++) {
             for ($col=0; $col<sqlrcur_colCount($cur); $col++) {
                     echo sqlrcur_getField($cur,$row,$col);
                     echo ",";
             }
             echo "\n";
     }

     sqlrcur_free($cur);
     sqlrcon_free($con);
?>
