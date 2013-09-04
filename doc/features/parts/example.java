import com.firstworks.sqlrelay.SQLRConnection;
import com.firstworks.sqlrelay.SQLRCursor;

class example {

	public static void main(String[] args) {

		SQLRConnection	sqlrcon=new SQLRConnection(
						"examplehost",(short)9000,
						"/tmp/example.socket",
						"exampleuser",
						"examplepassword",0,1);
		SQLRCursor	sqlrcur=new SQLRCursor(sqlrcon);

		sqlrcur.sendQuery("select * from exampletable");
		for (long row=0; row<sqlrcur.rowCount(); row++) {
			for (long col=0; col<sqlrcur.colCount(); col++) {
				System.out.println(sqlrcur.getField(row,col)+",");
			}
			System.out.println();
		}
	}
}
