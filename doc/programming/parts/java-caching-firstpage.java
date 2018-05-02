import SQLRConnection;
import SQLRCursor;

public class MyClass {
	public static main() {

        	SQLRConnection      con=new SQLRConnection("sqlrserver",(short)9000,"/tmp/example.socket","user","password",0,1);
        	SQLRCursor          cur=new SQLRCursor(con);

		... generate a unique filename ...

        	cur.cacheToFile(filename);
        	cur.setCacheTtl(600);
        	cur.sendQuery("select * from my_table");
        	con.endSession();
        	cur.cacheOff();

        	... pass the filename to the next page ...

		cur.delete();
		con.delete();
	}
}
