<?
     $con=sqlrcon_alloc("sqlrserver",9000,"/tmp/example.socket","user","password",0,1);
     $cur=sqlrcur_alloc($con);

     sqlrcur_executeQuery($cur,"create table images (image blob, description clob)");

     $imagedata="";
     $imagelength=0;

     ... read an image from a file into imagedata and the length of the
             file into imagelength ...

     $description="";
     $desclength=0;

     ... read a description from a file into description and the length of
             the file into desclength ...

     sqlrcur_prepareQuery($cur,"insert into images values (:image,:desc)");
     sqlrcur_inputBindBlob($cur,"image",$imagedata,$imagelength);
     sqlrcur_inputBindClob($cur,"desc",$description,$desclength);
     sqlrcur_executeQuery($cur);

     sqlrcur_free($cur);
     sqlrcon_free($con);
?>
