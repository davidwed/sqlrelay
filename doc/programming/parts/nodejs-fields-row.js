var	sqlrelay=require("sqlrelay");

var	con=new sqlrelay.SQLRConnection("sqlrserver",9000,"/tmp/example.socket","user","password",0,1);
var	cur=new sqlrelay.SQLRCursor(con);

cur.sendQuery("select * from my_table");
con.endSession();

for (var row=0; row&lt;cur.rowCount(); row++) {
	String[]    rowarray=cur.getRow(row);
	for (var col=0; col&lt;cur.colCount(); col++) {
		process.stdout.write(rowarray[col] + ",");
	}
	process.stdout.write("\n");
}
