#include <sqlrelay/sqlrclientwrapper.h>
#include <stdio.h>

main() {

        int        i;

        sqlrcon      con=sqlrcon_alloc("sqlrserver",9000,"/tmp/example.socket","user","password",0,1);
        sqlrcur      cur=sqlrcur_alloc(con);

        sqlrcur_sendQuery(cur,"select * from my_table");
        sqlrcon_endSession(con);

        for (i=0; i<sqlrcur_colCount(cur); i++) {
                printf("Name:          %s\n",sqlrcur_getColumnName(cur,i));
                printf("Type:          %s\n",sqlrcur_getColumnType(cur,i));
                printf("Length:        %d\n",sqlrcur_getColumnLength(cur,i));
                printf("Precision:     %d\n",sqlrcur_getColumnPrecision(cur,i));
                printf("Scale:         %d\n",sqlrcur_getColumnScale(cur,i));
                printf("Longest Field: %d\n",sqlrcur_getLongest(cur,i));
                printf("Nullable:      %d\n",sqlrcur_getColumnIsNullable(cur,i));
                printf("Primary Key:   %d\n",sqlrcur_getColumnIsPrimaryKey(cur,i));
                printf("Unique:        %d\n",sqlrcur_getColumnIsUnique(cur,i));
                printf("Part of Key:   %d\n",sqlrcur_getColumnIsPartOfKey(cur,i));
                printf("Unsigned:      %d\n",sqlrcur_getColumnIsUnsigned(cur,i));
                printf("Zero Filled:   %d\n",sqlrcur_getColumnIsZeroFilled(cur,i));
                printf("Binary:        %d\n",sqlrcur_getColumnIsBinary(cur,i));
                printf("Auth Increment:%d\n",sqlrcur_getColumnIsAutoIncrement(cur,i));
                printf("\n");
        }

        sqlrcur_free(cur);
        sqlrcon_free(con);
}
