var	sqlrelay=require("sqlrelay");

var	con=new sqlrelay.SQLRConnection("sqlrserver",9000,"/tmp/example.socket","user","password",0,1);
var	cur=new sqlrelay.SQLRCursor(con);

cur.prepareQuery("begin  :curs:=sp_mytable; end;");
cur.defineOutputBindCursor("curs");
cur.executeQuery();

var	bindcur=cur.getOutputBindCursor("curs");
bindcur.fetchFromBindCursor();

// print fields from table
for (var i=0; i&lt;bindcur.rowCount(); i++) {
	for (var j=0; j&lt;bindcur.colCount(); j++) {
		process.stdout.write(bindcur.getField(i,j)+", ");
	}
	process.stdout.write("\n");
}
