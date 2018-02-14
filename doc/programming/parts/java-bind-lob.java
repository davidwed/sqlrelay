import SQLRConnection;
import SQLRCursor;

public class myclass {
        public static main() {

                SQLRConnection      con=new SQLRConnection("sqlrserver",(short)9000,"/tmp/example.socket","user","password",0,1);
                SQLRCursor          cur=new SQLRCursor(con);

                cur.executeQuery("create table images (image blob, description clob)");

                String     imagedata;
                long       imagelength;

                ... read an image from a file into imagedata and the length of the
                        file into imagelength ...

                String     description;
                long       desclength;

                ... read a description from a file into description and the length of
                        the file into desclength ...

                cur.prepareQuery("insert into images values (:image,:desc)");
                cur.inputBindBlob("image",imagedata,imagelength);
                cur.inputBindClob("desc",description,desclength);
                cur.executeQuery();
        }
}
