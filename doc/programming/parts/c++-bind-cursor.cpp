#include <sqlrelay/sqlrclient.h>
#include <iostream.h>

main() {

        sqlrconnection      *con=new sqlrconnection("sqlrserver",9000,"/tmp/example.socket","user","password",0,1);
        sqlrcursor          *cur=new sqlrcursor(con);

        cur->prepareQuery("begin  :curs:=sp_mytable; end;");
        cur->defineOutputBindCursor("curs");
        cur->executeQuery();

        sqlrcursor      *bindcur=cur->getOutputBindCursor("curs");
        bindcur->fetchFromBindCursor();

        // print fields from table
        for (int i=0; i<bindcur->rowCount(); i++) {
                for (int j=0; j<bindcur->colCount(); j++) {
                        cout << bindcur->getField(i,j) << ", ";
                }
                cout << endl;
        }

        delete bindcur;

        delete cur;
        delete con;
}
