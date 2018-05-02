#include <sqlrelay/sqlrclient.h>
#include <iostream.h>

main() {

        ... get the filename from the previous page ...

        ... get the page to display from the previous page ...

        sqlrconnection      *con=new sqlrconnection("sqlrserver",9000,"/tmp/example.socket","user","password",0,1);
        sqlrcursor          *cur=new sqlrcursor(con);

        cur->openCachedResultSet(filename);
        con->endSession();

        for (int row=pagetodisplay*20; row<(pagetodisplay+1)*20; row++) {
                for (int col=0; col<cur->colCount(); col++) {
                        cout << cur->getField(row,col) << ",";
                }
                cout << endl;
        }

        delete cur;
        delete con;
}
