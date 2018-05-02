#include <sqlrelay/sqlrclient.h>

main() {

        sqlrconnection      *con=new sqlrconnection("sqlrserver",9000,"/tmp/example.socket","user","password",0,1);
        sqlrcursor          *cur=new sqlrcursor(con);

        cur->prepareQuery("select * from mytable where mycolumn>:value");
        cur->inputBind("value",1);
        cur->executeQuery();

        ... process the result set ...

        cur->clearBinds();
        cur->inputBind("value",5);
        cur->executeQuery();

        ... process the result set ...

        cur->clearBinds();
        cur->inputBind("value",10);
        cur->executeQuery();

        ... process the result set ...

        delete cur;
        delete con;
}
