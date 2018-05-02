import SQLRConnection;
import SQLRCursor;

public class MyClass {
	public static main() {

        	... get the filename from the previous page ...

        	... get the page to display from the previous page ...

        	SQLRConnection      con=new SQLRConnection("sqlrserver",(short)9000,"/tmp/example.socket","user","password",0,1);
        	SQLRCursor          cur=new SQLRCursor(con);

        	cur.openCachedResultSet(filename);
        	con.endSession();

        	for (int row=pagetodisplay*20; row<(pagetodisplay+1)*20; row++) {
                	for (int col=0; col<cur.colCount(); col++) {
                        	System.out.println(cur.getField(row,col) + ",");
                	}
                	System.out.println();
        	}

		cur.delete();
		con.delete();
	}
}
