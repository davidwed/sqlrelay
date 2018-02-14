use SQLRelay::Connection;
use SQLRelay::Cursor;

$con=SQLRelay::Connection->new("sqlrserver",9000,"/tmp/example.socket","user","password",0,1);
$cur=SQLRelay::Cursor->new($con);

$cur->sendQuery("select * from my_table");
$con->endSession();

for ($i=0; $i<$cur->colCount(); $i++) {
        printf("Name:          %s\n",$cur->getColumnName($i));
        printf("Type:          %s\n",$cur->getColumnType($i));
        printf("Length:        %d\n",$cur->getColumnLength($i));
        printf("Precision:     %d\n",$cur->getColumnPrecision($i));
        printf("Scale:         %d\n",$cur->getColumnScale($i));
        printf("Longest Field: %d\n",$cur->getLongest($i));
        printf("Nullable:      %d\n",$cur->getColumnIsNullable($i));
        printf("Primary Key:   %d\n",$cur->getColumnIsPrimaryKey($i));
        printf("Unique:        %d\n",$cur->getColumnIsUnique($i));
        printf("Part Of Key:   %d\n",$cur->getColumnIsPartOfKey($i));
        printf("Unsigned:      %d\n",$cur->getColumnIsUnsigned($i));
        printf("Zero Filled:   %d\n",$cur->getColumnIsZeroFilled($i));
        printf("Binary:        %d\n",$cur->getColumnIsBinary($i));
        printf("Auto Increment:%d\n",$cur->getColumnIsAutoIncrement($i));
        printf("\n");
}
