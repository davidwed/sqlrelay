#include <sqlrelay/sqlrclientwrapper.h>
#include <stdio.h>

main() {

        int        row,col;

        ... get the filename from the previous page ...

        ... get the page to display from the previous page ...

        sqlrcon      con=sqlrcon_alloc("sqlrserver",9000,"/tmp/example.socket","user","password",0,1);
        sqlrcur      cur=sqlrcur_alloc(con);

        sqlrcur_openCachedResultSet(cur,filename);
        sqlrcon_endSession(con);

        for (row=pagetodisplay*20; row<(pagetodisplay+1)*20; row++) {
                for (col=0; col<sqlrcur_colCount(cur); col++) {
                        printf("%s,",sqlrcur_getFieldByIndex(cur,row,col));
                }
                printf("\n");
        }

        sqlrcur_free(cur);
        sqlrcon_free(con);
}
