#include <sqlrelay/sqlrclient.h>

main() {

        sqlrconnection      *con=new sqlrconnection("sqlrserver",9000,"/tmp/example.socket","user","password",0,1);
        sqlrcursor          *cur=new sqlrcursor(con);

	... generate a unique filename ...

        cur->cacheToFile(filename);
        cur->setCacheTtl(600);
        cur->sendQuery("select * from my_table");
        con->endSession();
        cur->cacheOff();

        ... pass the filename to the next page ...

        delete cur;
        delete con;
}
