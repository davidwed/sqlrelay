use SQLRelay::Connection;
use SQLRelay::Cursor;

$con=SQLRelay::Connection->new("sqlrserver",9000,"/tmp/example.socket","user","password",0,1);
$cur=SQLRelay::Cursor->new($con);

$cur->sendQuery("insert into my_table values (1,2,3)");
$cur->suspendResultSet();
$con->suspendSession();
$rs=$cur->getResultSetId();
$port=$cur->getConnectionPort();
$socket=$cur->getConnectionSocket();

... pass the rs, port and socket to the next page ...
