import SQLRConnection;
import SQLRCursor;

public class myclass {
        public static main() {

                SQLRConnection      con=new SQLRConnection("sqlrserver",(short)9000,"/tmp/example.socket","user","password",0,1);
                SQLRCursor          cur=new SQLRCursor(con);

                cur.prepareQuery("begin  select image into :image from images;  select description into :desc from images;  end;");
                cur.defineOutputBindBlob("image");
                cur.defineOutputBindClob("desc");
                cur.executeQuery();

                String  image=cur.getOutputBindBlob("image");
                long    imagelength=cur.getOutputBindLength("image");

                String  desc=cur.getOutputBindClob("desc");
                String  desclength=cur.getOutputBindLength("desc");

                con.endSession();

                ... do something with image and desc ...

        }
}
