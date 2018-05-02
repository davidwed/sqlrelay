... get the rs, port and socket from previous page ...

$dbh=new PDO("sqlrelay:host=sqlrserver;port=9000;socket=/tmp/example.socket;tries=0;retrytime=1;debug=0","exampleuser","examplepassword");
$stmt=$dbh->prepare(null);
$dbh->resumeSession($port,$socket);
$stmt->resumeResultSet($rs);

... run more queries in the same transaction ...
