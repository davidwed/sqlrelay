#include <sqlrelay/sqlrclient.h>

main() {

        sqlrconnection      *con=new sqlrconnection("sqlrserver",9000,"/tmp/example.socket","user","password",0,1);
        sqlrcursor          *cur=new sqlrcursor(con);

        cur->executeQuery("create table images (image blob, description clob)");

        unsigned char   imagedata[40000];
        unsigned long   imagelength;

        ... read an image from a file into imagedata and the length of the
                file into imagelength ...

        unsigned char   description[40000];
        unsigned long   desclength;

        ... read a description from a file into description and the length of
                the file into desclength ...

        cur->prepareQuery("insert into images values (:image,:desc)");
        cur->inputBindBlob("image",imagedata,imagelength);
        cur->inputBindClob("desc",description,desclength);
        cur->executeQuery();

        delete cur;
        delete con;
}
