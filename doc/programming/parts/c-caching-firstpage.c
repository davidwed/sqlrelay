#include <sqlrelay/sqlrclientwrapper.h>
#include <stdio.h>

main() {

        char       *filename;

        sqlrcon      con=sqlrcon_alloc("sqlrserver",9000,"/tmp/example.socket","user","password",0,1);
        sqlrcur      cur=sqlrcur_alloc(con);

	... generate a unique file name ...

        sqlrcur_cacheToFile(cur,filename);
        sqlrcur_setCacheTtl(cur,600);
        sqlrcur_sendQuery(cur,"select * from my_table");
        sqlrcon_endSession(con);
        sqlrcur_cacheOff(cur);

        ... pass the filename to the next page ...

        sqlrcur_free(cur);
        sqlrcon_free(con);
}
