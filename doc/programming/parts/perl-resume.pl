use SQLRelay::Connection;
use SQLRelay::Cursor;

        ... get rs, port and socket from previous page ...

$con=SQLRelay::Connection->new("sqlrserver",9000,"/tmp/example.socket","user","password",0,1);
$cur=SQLRelay::Cursor->new($con);

$con->resumeSession(port,$socket);
$cur->resumeResultSet($rs);
$cur->sendQuery("commit");
$con->endSession();
