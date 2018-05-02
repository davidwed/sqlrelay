#include <sqlrelay/sqlrclientwrapper.h>

main() {

        sqlrcon      con;
        sqlrcur      cursor1;
        sqlrcur      cursor2;
        int          index;

        con=new sqlrcon_alloc("sqlrserver",9000,"/tmp/example.socket","user","password",0,1);
        cursor1=new sqlrcur_alloc(con);
        cursor2=new sqlrcur_alloc(con);

        sqlrcur_setResultSetBufferSize(cursor1,10);
        sqlrcur_sendQuery(cursor1,"select * from my_huge_table");

        index=0;
        while (!sqlrcur_endOfResultSet(cursor1)) {
                sqlrcur_prepareQuery(cursor2,"insert into my_other_table values (:1,:2,:3)");
                sqlrcur_inputBindString(cursor2,"1",sqlrcur_getFieldByIndex(cursor1,index,1));
                sqlrcur_inputBindString(cursor2,"2",sqlrcur_getFieldByIndex(cursor1,index,2));
                sqlrcur_inputBindString(cursor2,"3",sqlrcur_getFieldByIndex(cursor1,index,3));
                sqlrcur_executeQuery(cursor2);
        }

        sqlrcur_free(cursor2);
        sqlrcur_free(cursor1);
        sqlrcon_free(con);
}
