var	sqlrelay=require("sqlrelay");

var	con=new sqlrelay.SQLRConnection("sqlrserver",9000,"/tmp/example.socket","user","password",0,1);
var	cur=new sqlrelay.SQLRCursor(con);

... generate a unique filename ...

cur.cacheToFile(filename);
cur.setCacheTtl(600);
cur.sendQuery("select * from my_table");
con.endSession();
cur.cacheOff();

... pass the filename to the next page ...
