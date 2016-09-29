$dbh->query("set @out1=0");

$dbh->query("call testproc(@out1)");

$stmt=$dbh->prepare("select @out1");
$result=$stmt->execute();
