$dbh=new PDO("sqlrelay:host=sqlrserver;port=9000;socket=/tmp/example.socket;tries=0;retrytime=1;debug=0","exampleuser","examplepassword");
if (!$dbh) {
	die("connection failed");
}

$dbh->beginTransaction();

... run some queries that we want to keep the results of ...

$dbh->commit();

$dbh->beginTransaction();

... run some queries that we don't want to keep the results of ...

$dbh->rollBack();
