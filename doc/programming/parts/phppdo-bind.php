$dbh=new PDO("sqlrelay:host=sqlrserver;port=9000;socket=/tmp/example.socket;tries=0;retrytime=1;debug=0","exampleuser","examplepassword");
if (!$dbh) {
	die("connection failed");
}

$stmt=$dbh->prepare("insert into exampletable values (:var1,:var2,:var3,:var4)");
$stmt->bindValue(":var",1,PDO::PARAM_INT);
$stmt->bindValue(":var","1.1",PDO::PARAM_STR);
$stmt->bindValue(":var","hello",PDO::PARAM_STR);
$stmt->bindValue(":var","hello",PDO::PARAM_LOB);
$stmt->execute();
