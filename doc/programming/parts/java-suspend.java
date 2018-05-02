import SQLRConnection;
import SQLRCursor;

public class MyClass {
	public static main() {

        	SQLRConnection      con=new SQLRConnection("sqlrserver",(short)9000,"/tmp/example.socket","user","password",0,1);
        	SQLRCursor          cur=new SQLRCursor(con);

        	cur.sendQuery("insert into my_table values (1,2,3)");
        	cur.suspendResultSet();
        	con.suspendSession();
        	int     rs=cur.getResultSetId();
        	int     port=cur.getConnectionPort();
        	String  socket=cur.getConnectionSocket();

        	... pass the rs, port and socket to the next page ...

		cur.delete();
		con.delete();
	}
}
