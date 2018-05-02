import SQLRConnection;
import SQLRCursor;

public class MyClass {

        public static main() {

                SQLRConnection      con=new SQLRConnection("sqlrserver",(short)9000,"/tmp/example.socket","user","password",0,1);
                SQLRCursor          cur=new SQLRCursor(con);

                cur.prepareQuery("begin  :curs:=sp_mytable; end;");
                cur.defineOutputBindCursor("curs");
                cur.executeQuery();

                SQLRCursor      bindcur=cur.getOutputBindCursor("curs");
                bindcur.fetchFromBindCursor();

                // print fields from table
                for (int i=0; i<bindcur.rowCount(); i++) {
                        for (int j=0; j<bindcur.colCount(); j++) {
                                System.out.print(bindcur.getField(i,j)+", ");
                        }
                        System.out.println();
                }

                bindcur.delete();

                cur.delete();
                con.delete();
        }
}
