import SQLRConnection;
import SQLRCursor;

public class MyClass {
	public static main() {

        	SQLRConnection      con=new SQLRConnection("sqlrserver",(short)9000,"/tmp/example.socket","user","password",0,1);
        	SQLRCursor          cur=new SQLRCursor(con);

        	cur.sendQuery("select * from my_table");
        	con.endSession();

        	for (int i=0; i<cur.colCount(); i++) {
                	System.out.println("Name:          " + cur.getColumnName(i));
                	System.out.println("Type:          " + cur.getColumnType(i));
                	System.out.print("Length:        ");
                	System.out.println(cur.getColumnLength(i));
                	System.out.print("Precision:     ");
                	System.out.println(cur.getColumnPrecision(i));
                	System.out.print("Scale:         ");
                	System.out.println(cur.getColumnScale(i));
                	System.out.print("Longest Field: ");
                	System.out.println(cur.getLongest(i));
                	System.out.print("Nullable:       ");
                	System.out.println(cur.getColumnIsNullable(i));
                	System.out.print("Primary Key:    ");
                	System.out.println(cur.getColumnIsPrimaryKey(i));
                	System.out.print("Unique:         ");
                	System.out.println(cur.getColumnIsUnique(i));
                	System.out.print("Part Of Key:    ");
                	System.out.println(cur.getColumnIsPartOfKey(i));
                	System.out.print("Unsigned:       ");
                	System.out.println(cur.getColumnIsUnsigned(i));
                	System.out.print("Zero Filled:    ");
                	System.out.println(cur.getColumnIsZeroFilled(i));
                	System.out.print("Binary:         ");
                	System.out.println(cur.getColumnIsBinary(i));
                	System.out.print("Auto Increment: ");
                	System.out.println(cur.getColumnIsAutoIncrement(i));
                	System.out.println();
        	}

		cur.delete();
		con.delete();
	}
}
