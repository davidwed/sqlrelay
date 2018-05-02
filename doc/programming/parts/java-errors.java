import SQLRConnection;
import SQLRCursor;

public class MyClass {
	public static main() {

        	SQLRConnection      con=new SQLRConnection("sqlrserver",(short)9000,"/tmp/example.socket","user","password",0,1);
        	SQLRCursor          cur=new SQLRCursor(con);

        	if (!cur.sendQuery("select * from my_nonexistant_table")) {
                	System.out.println(cur.errorMessage());
        	}

		cur.delete();
		con.delete();
	}
}
