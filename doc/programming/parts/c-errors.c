#include <sqlrelay/sqlrclientwrapper.h>
#include <stdio.h>

main() {

        sqlrcon      con=sqlrcon_alloc("sqlrserver",9000,"/tmp/example.socket","user","password",0,1);
        sqlrcur      cur=sqlrcur_alloc(con);

        if (!sqlrcur_sendQuery(cur,"select * from my_nonexistant_table")) {
                printf("%s\n",sqlrcur_errorMessage(cur));
        }

        sqlrcur_free(cur);
        sqlrcon_free(con);
}
