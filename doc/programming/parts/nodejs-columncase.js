var	sqlrelay=require("sqlrelay");

var	con=new sqlrelay.SQLRConnection("sqlrserver",9000,"/tmp/example.socket","user","password",0,1);
var	cur=new sqlrelay.SQLRCursor(con);

// column names will be forced to upper case
cur.upperCaseColumnNames();
cur.sendQuery("select * from my_table");
con.endSession();

for (var i=0; i&lt;cur.colCount(); i++) {
	console.log("Name: "+cur.getColumnName(i));
}

// column names will be forced to lower case
cur.lowerCaseColumnNames();
cur.sendQuery("select * from my_table");
con.endSession();

for (var i=0; i&lt;cur.colCount(); i++) {
	console.log("Name: "+cur.getColumnName(i));
}

// column names will be the same as they are in the database
cur.mixedCaseColumnNames();
cur.sendQuery("select * from my_table");
con.endSession();

for (var i=0; i&lt;cur.colCount(); i++) {
	console.log("Name: "+cur.getColumnName(i));
}
