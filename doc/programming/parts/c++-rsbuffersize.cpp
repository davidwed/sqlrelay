#include <sqlrelay/sqlrclient.h>
#include <iostream.h>

main() {

        sqlrconnection      *con=new sqlrconnection("sqlrserver",9000,"/tmp/example.socket","user","password",0,1);
        sqlrcursor          *cur=new sqlrcursor(con);

        cur->setResultSetBufferSize(5);

        cur->sendQuery("select * from my_table");

        int     done=0;
        int     row=0;
        char    *field;
        while (!done) {
                for (int col=0; col<cur->colCount(); col++) {
                        if (field=cur->getField(row,col)) {
                                cout << field << ",";
                        } else {
                                done=1;
                        }
                }
                cout << endl;
                row++;
        }

        cur->sendQuery("select * from my_other_table");

        ... process this query's result set in chunks also ...

        cur->setResultSetBufferSize(0);

        cur->sendQuery("select * from my_third_table");

        ... process this query's result set all at once ...

        con->endSession();

        delete cur;
        delete con;
}
