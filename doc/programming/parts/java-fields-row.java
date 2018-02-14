import SQLRConnection;
import SQLRCursor;

public class MyClass {
	public static main() {

        	SQLRConnection      con=new SQLRConnection("sqlrserver",(short)9000,"/tmp/example.socket","user","password",0,1);
        	SQLRCursor          cur=new SQLRCursor(con);

        	cur.sendQuery("select * from my_table");
        	con.endSession();

        	for (int row=0; row<cur.rowCount(); row++) {
                	String[]    rowarray=cur.getRow(row);
                	for (int col=0; col<cur.colCount(); col++) {
                        	System.out.println(rowarray[col] + ",");
                	}
                	System.out.println();
        	}

		cur.delete();
		con.delete();
	}
}
