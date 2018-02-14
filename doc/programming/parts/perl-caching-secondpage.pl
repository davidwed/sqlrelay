use SQLRelay::Connection;
use SQLRelay::Cursor;

        ... get the filename from the previous page ...

        ... get the page to display from the previous page ...

$con=SQLRelay::Connection->new("sqlrserver",9000,"/tmp/example.socket","user","password",0,1);
$cur=SQLRelay::Cursor->new($con);

$cur->openCachedResultSet(filename);
$con->endSession();

for ($row=pagetodisplay*20; $row<(pagetodisplay+1)*20; $row++) {
        for ($col=0; $col<$cur->colCount(); $col++) {
                printf("%s,",$cur->getField($row,$col));
        }
        printf("\n");
}
