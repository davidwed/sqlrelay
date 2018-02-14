import SQLRConnection;
import SQLRCursor;

public class MyClass {
       public static main() {

               SQLRConnection      con=new SQLRConnection("sqlrserver",(short)9000,"/tmp/example.socket","user","password",0,1);
               SQLRCursor          cur=new SQLRCursor(con);

               cur.prepareQuery("select * from mytable where mycolumn>:value");
               cur.inputBind("value",1);
               cur.executeQuery();

               ... process the result set ...

               cur.clearBinds();
               cur.inputBind("value",5);
               cur.executeQuery();

               ... process the result set ...

               cur.clearBinds();
               cur.inputBind("value",10);
               cur.executeQuery();

               ... process the result set ...

	       cur.delete();
	       con.delete();
       }
}
