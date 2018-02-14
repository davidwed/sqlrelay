import SQLRConnection;

public class MyClass {
	public static main() {

       		SQLRConnection      con=new SQLRConnection("sqlrserver",(short)9000,"/tmp/example.socket","user","password",0,1);

       		... execute some queries ...

       		con.delete();
	}
}
