var	sqlrelay=require("sqlrelay");

var	sqlrcon=new sqlrelay.SQLRConnection(
				"examplehost",9000,
				"/tmp/example.socket",
				"exampleuser",
				"examplepassword",0,1);
var	sqlrcur=new sqlrelay.SQLRCursor(sqlrcon);

sqlrcur.sendQuery("select * from exampletable");
for (var row=0; row<sqlrcur.rowCount(); row++) {
	for (var col=0; col<sqlrcur.colCount(); col++) {
		process.stdout.write(sqlrcur.getField(row,col)+",");
	}
	process.stdout.write("\n");
}
