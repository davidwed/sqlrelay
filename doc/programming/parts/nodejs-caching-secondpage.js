var	sqlrelay=require("sqlrelay");

... get the filename from the previous page ...

... get the page to display from the previous page ...

var	con=new sqlrelay.SQLRConnection("sqlrserver",9000,"/tmp/example.socket","user","password",0,1);
var	cur=new sqlrelay.SQLRCursor(con);

cur.openCachedResultSet(filename);
con.endSession();

for (int row=pagetodisplay*20; row&lt;(pagetodisplay+1)*20; row++) {
	for (int col=0; col&lt;cur.colCount(); col++) {
		process.stdout.write(cur.getField(row,col) + ",");
	}
	process.stdout.write("\n");
}
