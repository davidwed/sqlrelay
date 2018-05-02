#include <sqlrelay/sqlrclientwrapper.h>
#include <stdio.h>

main() {

        int        row,col;

        sqlrcon      con=sqlrcon_alloc("sqlrserver",9000,"/tmp/example.socket","user","password",0,1);
        sqlrcur      cur=sqlrcur_alloc(con);

        sqlrcur_sendQuery(cur,"select * from my_table");
        sqlrcon_endSession(con);

        for (row=0; row<sqlrcur_rowCount(cur); row++) {
                for (col=0; col<sqlrcur_colCount(cur); col++) {
                        printf("%s,",sqlrcur_getFieldByIndex(cur,row,col));
                }
                printf("\n");
        }

        sqlrcur_free(cur);
        sqlrcon_free(con);
}
