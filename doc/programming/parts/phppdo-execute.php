$dbh=new PDO("sqlrelay:host=sqlrserver;port=9000;socket=/tmp/test.socket;tries=0;retrytime=1;debug=0","testuser","testpassword");
if (!$dbh) {
	die("connection failed");
}

if (!$dbh->exec("insert into testtable values (1,'hello')")) {
	die("exec failed");
}
