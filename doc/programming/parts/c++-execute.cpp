#include <sqlrelay/sqlrclient.h>

main() {

        sqlrconnection      *con=new sqlrconnection("sqlrserver",9000,"/tmp/example.socket","user","password",0,1);
        sqlrcursor          *cur=new sqlrcursor(con);

        cur->sendQuery("select * from my_table");

        ... do some stuff that takes a short time ...

        cur->sendFileQuery("/usr/local/myprogram/sql","myquery.sql");
        con->endSession();

        ... do some stuff that takes a long time ...

        cur->sendQuery("select * from my_other_table");
        con->endSession();

        ... process the result set ...

        delete cur;
        delete con;
}
