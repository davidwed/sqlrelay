#include <sqlrelay/sqlrclient.h>

main() {

        sqlrconnection      *con=new sqlrconnection("sqlrserver",9000,"/tmp/example.socket","user","password",0,1);
        sqlrcursor          *cur=new sqlrcursor(con);

        cur->prepareQuery("begin  select image into :image from images;  select description into :desc from images;  end;");
        cur->defineOutputBindBlob("image");
        cur->defineOutputBindClob("desc");
        cur->executeQuery();

        char    *image=cur->getOutputBindBlob("image");
        long    imagelength=cur->getOutputBindLength("image");

        char    *desc=cur->getOutputBindClob("desc");
        char    *desclength=cur->getOutputBindLength("desc");

        con->endSession();

        ... do something with image and desc ...

        delete cur;
        delete con;
}
