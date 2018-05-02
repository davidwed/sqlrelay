var	sqlrelay=require("sqlrelay");

var	con=new sqlrelay.SQLRConnection("sqlrserver",9000,"/tmp/example.socket","user","password",0,1);
var	cur=new sqlrelay.SQLRCursor(con);

cur.prepareQuery("select * from mytable where mycolumn&gt;:value");
cur.inputBind("value",1);
cur.executeQuery();

... process the result set ...

cur.clearBinds();
cur.inputBind("value",5);
cur.executeQuery();

... process the result set ...

cur.clearBinds();
cur.inputBind("value",10);
cur.executeQuery();

... process the result set ...
