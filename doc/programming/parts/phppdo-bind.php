$dbh=new PDO("sqlrelay:host=sqlrserver;port=9000;socket=/tmp/test.socket;tries=0;retrytime=1;debug=0","testuser","testpassword");
if (!$dbh) {
	die("connection failed");
}

$stmt=$dbh->prepare("insert into testtable values (:var1,:var2,:var3,:var4)");
$stmt->bindValue(":var",1,PDO::PARAM_INT);
$stmt->bindValue(":var","1.1",PDO::PARAM_STR);
$stmt->bindValue(":var","hello",PDO::PARAM_STR);
$stmt->bindValue(":var","hello",PDO::PARAM_LOB);
$stmt->execute();
