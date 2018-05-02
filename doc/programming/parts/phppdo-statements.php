$dbh=new PDO("sqlrelay:host=sqlrserver;port=9000;socket=/tmp/example.socket;tries=0;retrytime=1;debug=0","exampleuser","examplepassword");
if (!$dbh) {
	die("connection failed");
}

$stmt1=$dbh->prepare("select * from table1");
$stmt2=$dbh->prepare("insert into table2 values (:col1, :col2, :col3)");

$stmt1->execute();

while ($result=$stmt1->fetch()) {

	$stmt2->bindValue(":col1",$result[0],PDO::PARAM_INT);
	$stmt2->bindValue(":col2",$result[1],PDO::PARAM_STR);
	$stmt2->bindValue(":col3",$result[2],PDO::PARAM_STR);

	$stmt2->execute();
}
