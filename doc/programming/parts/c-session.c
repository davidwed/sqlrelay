#include <sqlrelay/sqlrclientwrapper.h>
#include <stdio.h>

main() {

        sqlrcon      con=sqlrcon_alloc("sqlrserver",9000,"/tmp/test.socket","user","password",0,1);

        ... execute some queries ...

        sqlrcon_free(con);
}
