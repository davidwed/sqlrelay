#include <sqlrelay/sqlrclient.h>
#include <iostream.h>

main() {

        sqlrconnection      *con=new sqlrconnection("sqlrserver",9000,"/tmp/example.socket","user","password",0,1);
        sqlrcursor          *cur=new sqlrcursor(con);

        // column names will be forced to upper case
        cur->upperCaseColumnNames();
        cur->sendQuery("select * from my_table");
        con->endSession();

        for (int i=0; i<cur->colCount(); i++) {
                cout << "Name:          " << cur->getColumnName(i) << endl;
                cout << endl;
        }

        // column names will be forced to lower case
        cur->lowerCaseColumnNames();
        cur->sendQuery("select * from my_table");
        con->endSession();

        for (int i=0; i<cur->colCount(); i++) {
                cout << "Name:          " << cur->getColumnName(i) << endl;
                cout << endl;
        }

        // column names will be the same as they are in the database
        cur->mixedCaseColumnNames();
        cur->sendQuery("select * from my_table");
        con->endSession();

        for (int i=0; i<cur->colCount(); i++) {
                cout << "Name:          " << cur->getColumnName(i) << endl;
                cout << endl;
        }

        delete cur;
        delete con;
}
