#include <sqlrelay/sqlrclient.h>

main() {

        sqlrconnection      *con=new sqlrconnection("sqlrserver",9000,"/tmp/example.socket","user","password",0,1);
        sqlrcursor          *cur=new sqlrcursor(con);

        cur->sendQuery("insert into my_table values (1,2,3)");
        cur->suspendResultSet();
        con->suspendSession();
        int     rs=cur->getResultSetId();
        int     port=cur->getConnectionPort();
        char    *socket=cur->getConnectionSocket();

        ... pass the rs, port and socket to the next page ...

        delete cur;
        delete con;
}
