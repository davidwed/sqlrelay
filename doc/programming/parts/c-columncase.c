#include <sqlrelay/sqlrclientwrapper.h>
#include <stdio.h>

main() {

        int        i;

        sqlrcon         con=sqlrcon_alloc("sqlrserver",9000,"/tmp/test.socket","user","password",0,1);
        sqlrcur         cur=sqlrcur_alloc(con);

        // column names will be forced to upper case
        sqlrcur_upperCaseColumnNames(cur);
        sqlrcur_endQuery(cur,"select * from my_table");
        sqlrcon_endSession(con);

        for (i=0; i<sqlrcur_colCount(cur); i++) {
                printf("Name:   %s\n",sqlrcur_getColumnName(cur,i));
        }

        // column names will be forced to lower case
        sqlrcur_lowerCaseColumnNames(cur);
        sqlrcur_endQuery(cur,"select * from my_table");
        sqlrcon_endSession(con);

        for (i=0; i<sqlrcur_colCount(cur); i++) {
                printf("Name:   %s\n",sqlrcur_getColumnName(cur,i));
        }

        // column names will be the same as they are in the database
        sqlrcur_mixedCaseColumnNames(cur);
        sqlrcur_endQuery(cur,"select * from my_table");
        sqlrcon_endSession(con);

        for (i=0; i<sqlrcur_colCount(cur); i++) {
                printf("Name:   %s\n",sqlrcur_getColumnName(cur,i));
        }

        sqlrcur_free(cur);
        sqlrcon_free(con);
}
