$dbh=new PDO("sqlrelay:host=sqlrserver;port=9000;socket=/tmp/example.socket;tries=0;retrytime=1;debug=0","exampleuser","examplepassword");
$stmt=$dbh->query("select * from exampletable");
$stmt->suspendResultSet();
$dbh->suspendSession();
$rs=$stmt->getResultSetId();
$port=$dbh->getConnectionPort();

... pass the rs, port and socket to the next page ...
