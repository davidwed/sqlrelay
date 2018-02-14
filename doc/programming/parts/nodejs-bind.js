var	sqlrelay=require("sqlrelay");

var	con=new sqlrelay.SQLRConnection("sqlrserver",9000,"/tmp/example.socket","user","password",0,1);
var	cur=new sqlrelay.SQLRCursor(con);

cur.prepareQuery("select * from mytable $(whereclause)");
cur.substitution("whereclause","where stringcol=:stringval and integercol>:integerval and floatcol>floatval");
cur.inputBind("stringval","true");
cur.inputBind("integerval",10);
cur.inputBind("floatval",1.1,2,1);
cur.executeQuery();

... process the result set ...
