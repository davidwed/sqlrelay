import SQLRConnection;
import SQLRCursor;

public class MyClass {

        public static main() {

                SQLRConnection      con=new SQLRConnection("sqlrserver",(short)9000,"/tmp/example.socket","user","password",0,1);
                SQLRCursor          cur=new SQLRCursor(con);

                // column names will be forced to upper case
                cur.upperCaseColumnNames();
                cur.sendQuery("select * from my_table");
                con.endSession();

                for (int i=0; i<cur.colCount(); i++) {
                        System.out.println("Name: "+cur.getColumnName(i));
                }

                // column names will be forced to lower case
                cur.lowerCaseColumnNames();
                cur.sendQuery("select * from my_table");
                con.endSession();

                for (int i=0; i<cur.colCount(); i++) {
                        System.out.println("Name: "+cur.getColumnName(i));
                }

                // column names will be the same as they are in the database
                cur.mixedCaseColumnNames();
                cur.sendQuery("select * from my_table");
                con.endSession();

                for (int i=0; i<cur.colCount(); i++) {
                        System.out.println("Name: "+cur.getColumnName(i));
                }

                cur.delete();
                con.delete();
        }
}
