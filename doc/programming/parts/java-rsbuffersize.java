import SQLRConnection;
import SQLRCursor;

public class MyClass {
	public static main() {

        	SQLRConnection      con=new SQLRConnection("sqlrserver",(short)9000,"/tmp/example.socket","user","password",0,1);
        	SQLRCursor          cur=new SQLRCursor(con);

        	cur.setResultSetBufferSize(5);

        	cur.sendQuery("select * from my_table");

        	int     done=0;
        	int     row=0;
        	String  field;
        	while (!done) {
                	for (int col=0; col<cur.colCount(); col++) {
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

		cur.delete();
		con.delete();
	}
}
