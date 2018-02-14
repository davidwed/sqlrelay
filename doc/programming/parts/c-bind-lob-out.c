#include <sqlrelay/sqlrclientwrapper.h>

main() {

        sqlrcon      con=sqlrcon_alloc("sqlrserver",9000,"/tmp/example.socket","user","password",0,1);
        sqlrcur      cur=sqlrcur_alloc(con);

        sqlrcur_prepareQuery(cur,"begin  select image into :image from images;  select description into :desc from images;  end;");
        sqlrcur_defineOutputBindBlob(cur,"image");
        sqlrcur_defineOutputBindClob(cur,"desc");
        sqlrcur_executeQuery(cur);

        char    *image=sqlrcur_getOutputBindBlob(cur,"image");
        long    imagelength=sqlrcur_getOutputBindLength(cur,"image");

        char    *desc=sqlrcur_getOutputBindClob(cur,"desc");
        char    *desclength=sqlrcur_getOutputBindLength(cur,"desc");

        sqlrcon_endSession(con);

        ... do something with image and desc ...

        sqlrcur_free(cur);
        sqlrcon_free(con);
}
