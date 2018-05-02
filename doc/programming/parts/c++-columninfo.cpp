#include <sqlrelay/sqlrclient.h>
#include <iostream.h>

main() {

        sqlrconnection      *con=new sqlrconnection("sqlrserver",9000,"/tmp/example.socket","user","password",0,1);
        sqlrcursor          *cur=new sqlrcursor(con);

        cur->sendQuery("select * from my_table");
        con->endSession();

        for (int i=0; i<cur->colCount(); i++) {
                cout << "Name:          " << cur->getColumnName(i) << endl;
                cout << "Type:          " << cur->getColumnType(i) << endl;
                cout << "Length:        " << cur->getColumnLength(i) << endl;
                cout << "Precision:     " << cur->getColumnPrecision(i) << endl;
                cout << "Scale:         " << cur->getColumnScale(i) << endl;
                cout << "Longest Field: " << cur->getLongest(i) << endl;
                cout << "Nullable:      " << cur->getColumnIsNullable(i) << endl;
                cout << "Primary Key:   " << cur->getColumnIsPrimaryKey(i) << endl;
                cout << "Unique:        " << cur->getColumnIsUnique(i) << endl;
                cout << "Part of Key:   " << cur->getColumnIsPartOfKey(i) << endl;
                cout << "Unsigned:      " << cur->getColumnIsUnsigned(i) << endl;
                cout << "Zero Filled:   " << cur->getColumnIsZeroFilled(i) << endl;
                cout << "Binary:        " << cur->getColumnIsBinary(i) << endl;
                cout << "Auto Increment:" << cur->getColumnIsAutoIncrement(i) << endl;
                cout << endl;
        }

        delete cur;
        delete con;
}
