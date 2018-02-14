use SQLRelay::Connection;
use SQLRelay::Cursor;

$con=SQLRelay::Connection->new("sqlrserver",9000,"/tmp/example.socket","user","password",0,1);
$cur=SQLRelay::Cursor->new($con);

... generate a unique file name ...

$cur->cacheToFile(filename);
$cur->setCacheTtl(600);
$cur->sendQuery("select * from my_table");
$con->endSession();
$cur->cacheOff();

... pass the filename to the next page ...
