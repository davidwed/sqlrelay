#include <sqlrelay/sqlrclient.h>
#include <iostream.h>

main() {

        sqlrconnection      *con=new sqlrconnection("sqlrserver",9000,"/tmp/example.socket","user","password",0,1);
        sqlrcursor          *cur=new sqlrcursor(con);

        cur->sendQuery("select * from my_table");
        con->endSession();

        for (int row=0; row<cur->rowCount(); row++) {
                char    **rowarray=cur->getRow(row);
                for (int col=0; col<cur->colCount(); col++) {
                        cout << rowarray[col] << ",";
                }
                cout << endl;
        }

        delete cur;
        delete con;
}
