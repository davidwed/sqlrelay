$dbh=new PDO("sqlrelay:host=sqlrserver;port=9000;socket=/tmp/example.socket;tries=0;retrytime=1;debug=0","exampleuser","examplepassword");
if (!$dbh) {
	die("connection failed");
}

$stmt=$dbh->query("select * from exampletable"));

for ($i=0; $i<$stmt->columnCount(); $i++) {

	$meta=$stmt->getColumnMeta($i);

	... do something with $meta ...
}
