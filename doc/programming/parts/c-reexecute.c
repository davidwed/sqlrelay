#include <sqlrelay/sqlrclientwrapper.h>
#include <stdio.h>

main() {

        sqlrcon      con=sqlrcon_alloc("sqlrserver",9000,"/tmp/example.socket","user","password",0,1);
        sqlrcur      cur=sqlrcur_alloc(con);

        sqlrcur_prepareQuery(cur,"select * from mytable where mycolumn>:value");
        sqlrcur_inputBindLong(cur,"value",1);
        sqlrcur_executeQuery(cur);

        ... process the result set ...

        sqlrcur_clearBinds(cur);
        sqlrcur_inputBindLong(cur,"value",5);
        sqlrcur_executeQuery(cur);

        ... process the result set ...

        sqlrcur_clearBinds(cur);
        sqlrcur_inputBindLong(cur,"value",10);
        sqlrcur_executeQuery(cur);

        ... process the result set ...

        sqlrcur_free(cur);
        sqlrcon_free(con);
}
