var	sqlrelay=require("sqlrelay");

var	con=new sqlrelay.SQLRConnection("sqlrserver",9000,"/tmp/example.socket","user","password",0,1);
var	cur=new sqlrelay.SQLRCursor(con);

if (!cur.sendQuery("select * from my_nonexistant_table")) {
	console.log(cur.errorMessage());
}
