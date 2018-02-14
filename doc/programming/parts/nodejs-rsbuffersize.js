var	sqlrelay=require("sqlrelay");

var	con=new sqlrelay.SQLRConnection("sqlrserver",9000,"/tmp/example.socket","user","password",0,1);
var	cur=new sqlrelay.SQLRCursor(con);

cur.setResultSetBufferSize(5);

cur.sendQuery("select * from my_table");

var	done=0;
var	row=0;
var	field;
while (!done) {
	for (var col=0; col&lt;cur.colCount(); col++) {
		if (field=cur.getField(row,col)) {
			System.out.println(field + ",");
		} else {
			done=1;
		}
	}
	System.out.println();
	row++;
}

cur.sendQuery("select * from my_other_table");

... process this query's result set in chunks also ...

cur.setResultSetBufferSize(0);

cur.sendQuery("select * from my_third_table");

... process this query's result set all at once ...

con.endSession();
