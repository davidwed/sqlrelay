#include <sqlrelay/sqlrclient.h>
#include <iostream.h>

main() {

        sqlrconnection      *con=new sqlrconnection("sqlrserver",9000,"/tmp/example.socket","user","password",0,1);
        sqlrcursor          *cur=new sqlrcursor(con);

        if (!cur->sendQuery("select * from my_nonexistant_table")) {
                cout << cur->errorMessage() << endl;
        }

        delete cur;
        delete con;
}
