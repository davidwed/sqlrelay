var	sqlrelay=require("sqlrelay");

var	con=new sqlrelay.SQLRConnection("sqlrserver",9000,"/tmp/example.socket","user","password",0,1);
var	cur=new sqlrelay.SQLRCursor(con);

cur.sendQuery("select * from my_table");
con.endSession();

for (var i=0; i&lt;cur.colCount(); i++) {
	process.stdout.write("Name:          " + cur.getColumnName(i) + "\n");
	process.stdout.write("Type:          " + cur.getColumnType(i) + "\n");
	process.stdout.write("Length:        ");
	process.stdout.write(cur.getColumnLength(i) + "\n");
	process.stdout.write("Precision:     ");
	process.stdout.write(cur.getColumnPrecision(i) + "\n");
	process.stdout.write("Scale:         ");
	process.stdout.write(cur.getColumnScale(i) + "\n");
	process.stdout.write("Longest Field: ");
	process.stdout.write(cur.getLongest(i) + "\n");
	process.stdout.write("Nullable:       ");
	process.stdout.write(cur.getColumnIsNullable(i) + "\n");
	process.stdout.write("Primary Key:    ");
	process.stdout.write(cur.getColumnIsPrimaryKey(i) + "\n");
	process.stdout.write("Unique:         ");
	process.stdout.write(cur.getColumnIsUnique(i) + "\n");
	process.stdout.write("Part Of Key:    ");
	process.stdout.write(cur.getColumnIsPartOfKey(i) + "\n");
	process.stdout.write("Unsigned:       ");
	process.stdout.write(cur.getColumnIsUnsigned(i) + "\n");
	process.stdout.write("Zero Filled:    ");
	process.stdout.write(cur.getColumnIsZeroFilled(i) + "\n");
	process.stdout.write("Binary:         ");
	process.stdout.write(cur.getColumnIsBinary(i) + "\n");
	process.stdout.write("Auto Increment: ");
	process.stdout.write(cur.getColumnIsAutoIncrement(i) + "\n");
	process.stdout.write("\n");
}
