<?
     $con=sqlrcon_alloc("sqlrserver",9000,"/tmp/test.socket","user","password",0,1);
     $cur=sqlrcur_alloc($con);

     sqlrcur_prepareQuery($cur,"begin  select image into :image from images;  select description into :desc from images;  end;");
     sqlrcur_defineOutputBindBlob($cur,"image");
     sqlrcur_defineOutputBindClob($cur,"desc");
     sqlrcur_executeQuery($cur);

     $image=sqlrcur_getOutputBindBlob($cur,"image");
     $imagelength=sqlrcur_getOutputBindLength($cur,"image");

     $desc=sqlrcur_getOutputBindClob($cur,"desc");
     $desclength=sqlrcur_getOutputBindLength($cur,"desc");

     sqlrcon_endSession($con);

     ... do something with image and desc ...

     sqlrcur_free($cur);
     sqlrcon_free($con);
?>
