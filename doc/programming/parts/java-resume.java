import SQLRConnection;
import SQLRCursor;

public class MyClass {
	public static main() {

        	... get rs, port and socket from previous page ...

        	SQLRConnection      con=new SQLRConnection("sqlrserver",(short)9000,"/tmp/example.socket","user","password",0,1);
        	SQLRCursor          cur=new SQLRCursor(con);

        	con.resumeSession(port,socket);
        	cur.resumeResultSet(rs);
        	cur.sendQuery("commit");
        	con.endSession();

		cur.delete();
		con.delete();
	}
}
