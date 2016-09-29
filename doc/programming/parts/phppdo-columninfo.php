$dbh=new PDO("sqlrelay:host=sqlrserver;port=9000;socket=/tmp/test.socket;tries=0;retrytime=1;debug=0","testuser","testpassword");
if (!$dbh) {
	die("connection failed");
}

$stmt=$dbh->query("select * from testtable"));

for ($i=0; $i<$stmt->columnCount(); $i++) {

	$meta=$stmt->getColumnMeta($i);

	... do something with $meta ...
}
