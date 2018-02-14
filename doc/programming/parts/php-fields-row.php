<?
     $con=sqlrcon_alloc("sqlrserver",9000,"/tmp/example.socket","user","password",0,1);
     $cur=sqlrcur_alloc($con);

     sqlrcur_sendQuery($cur,"select * from my_table");
     sqlrcon_endSession($con);

     for ($row=0; $row<sqlrcur_rowCount($cur); $row++) {
             $rowarray=sqlrcur_getRow($cur,$row);
             for ($col=0; $col<sqlrcur_colCount($cur); $col++) {
                     echo rowarray[$col];
                     echo ",";
             }
             echo "\n";
     }

     sqlrcur_free($cur);
     sqlrcon_free($con);
?>
