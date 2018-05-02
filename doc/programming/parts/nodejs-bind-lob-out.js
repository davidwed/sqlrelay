var	sqlrelay=require("sqlrelay");

var	con=new sqlrelay.SQLRConnection("sqlrserver",9000,"/tmp/example.socket","user","password",0,1);
var	cur=new sqlrelay.SQLRCursor(con);

cur.prepareQuery("begin  select image into :image from images;  select description into :desc from images;  end;");
cur.defineOutputBindBlob("image");
cur.defineOutputBindClob("desc");
cur.executeQuery();

var	image=cur.getOutputBindBlob("image");
var	imagelength=cur.getOutputBindLength("image");

var	desc=cur.getOutputBindClob("desc");
var	desclength=cur.getOutputBindLength("desc");

con.endSession();

... do something with image and desc ...
