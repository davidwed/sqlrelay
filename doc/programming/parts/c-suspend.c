#include <sqlrelay/sqlrclientwrapper.h>
#include <stdio.h>

main() {

        int        port;
        char       *socket;
        int        rs;

        sqlrcon      con=sqlrcon_alloc("sqlrserver",9000,"/tmp/example.socket","user","password",0,1);
        sqlrcur      cur=sqlrcur_alloc(con);

        sqlrcur_sendQuery(cur,"insert into my_table values (1,2,3)");
	sqlrcur_suspendResultSet(cur);
        sqlrcon_suspendSession(con);
	rs=sqlrcur_getResultSetId(cur);
        port=getConnectionPort(cur);
        socket=getConnectionSocket(cur);

        ... pass the rs, port and socket to the next page ...

        sqlrcur_free(cur);
        sqlrcon_free(con);
}
