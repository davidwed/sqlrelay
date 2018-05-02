use SQLRelay::Connection;
use SQLRelay::Cursor;

$con=SQLRelay::Connection->new("sqlrserver",9000,"/tmp/example.socket","user","password",0,1);
$cur=SQLRelay::Cursor->new($con);

$cur->setResultSetBufferSize(5);

$cur->sendQuery("select * from my_table");

$done=0;
$row=0;
while (!$done) {
        for ($col=0; $col<$cur->colCount(); $col++) {
                if ($field=getField($row,$col)) {
                        printf("%s,",$field);
                } else {
                        $done=1;
                }
        }
        printf("\n");
        row++;
}

$cur->sendQuery("select * from my_other_table");

... process this query's result set in chunks also ...

$cur->setResultSetBufferSize(0);

$cur->sendQuery("select * from my_third_table");

... process this query's result set all at once ...

$con->endSession();
