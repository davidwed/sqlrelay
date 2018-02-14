use SQLRelay::Connection;
use SQLRelay::Cursor;

$con=SQLRelay::Connection->new("sqlrserver",9000,"/tmp/example.socket","user","password",0,1);
$cur=SQLRelay::Cursor->new($con);

$cur->sendQuery("select * from my_table");
$con->endSession();

for ($row=0; $row<$cur->rowCount(); $row++) {
        for ($col=0; $col<$cur->colCount(); $col++) {
                printf("%s,",$cur->getField($row,$col));
        }
        printf("\n");
}
