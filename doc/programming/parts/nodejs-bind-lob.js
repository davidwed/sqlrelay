var	sqlrelay=require("sqlrelay");

var	con=new sqlrelay.SQLRConnection("sqlrserver",9000,"/tmp/example.socket","user","password",0,1);
var	cur=new sqlrelay.SQLRCursor(con);

cur.executeQuery("create table images (image blob, description clob)");

var	imagedata;
var	imagelength;

... read an image from a file into imagedata and the length of the file
into imagelength ...

var	description;
var	desclength;

... read a description from a file into description and the length of
the file into desclength ...

cur.prepareQuery("insert into images values (:image,:desc)");
cur.inputBindBlob("image",imagedata,imagelength);
cur.inputBindClob("desc",description,desclength);
cur.executeQuery();
