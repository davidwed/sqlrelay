#include <sqlrelay/sqlrclientwrapper.h>
#include <stdio.h>

main() {

        sqlrcon      con=sqlrcon_alloc("sqlrserver",9000,"/tmp/example.socket","user","password",0,1);
        sqlrcur      cur=sqlrcur_alloc(con);

        sqlrcur_sendQuery(cur,"select * from my_table");

        ... do some stuff that takes a short time ...

        sqlrcur_sendFileQuery(cur,"/usr/local/myprogram/sql","myquery.sql");
        sqlrcon_endSession(con);

        ... do some stuff that takes a long time ...

        sqlrcur_sendQuery(cur,"select * from my_other_table");
        sqlrcon_endSession(con);

        ... process the result set ...

        sqlrcur_free(cur);
        sqlrcon_free(con);
}
