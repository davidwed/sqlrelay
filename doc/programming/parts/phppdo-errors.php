$dbh=new PDO("sqlrelay:host=sqlrserver;port=9000;socket=/tmp/example.socket;tries=0;retrytime=1;debug=0","exampleuser","examplepassword");
if (!$dbh) {
	die("connection failed");
}

if (!$dbh->exec("insert into exampletable values (1,1.1,'hello')")) {
	die($dbh->errorCode().":".$dbh->errorInfo());
}

$stmt=$dbh->prepare("bad query");
if ($stmt->execute()) {
	die($stmt->errorCode().":".$stmt->errorInfo());
}
