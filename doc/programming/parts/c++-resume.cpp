#include <sqlrelay/sqlrclient.h>

main() {

        ... get rs, port and socket from previous page ...

        sqlrconnection      *con=new sqlrconnection("sqlrserver",9000,"/tmp/example.socket","user","password",0,1);
        sqlrcursor          *cur=new sqlrcursor(con);

        con->resumeSession(port,socket);
        cur->resumeResultSet(rs);
        cur->sendQuery("commit");
        con->endSession();

        delete cur;
        delete con;
}
