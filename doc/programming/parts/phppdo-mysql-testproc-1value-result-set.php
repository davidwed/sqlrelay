$stmt=$dbh->prepare("select * from testproc()");
$result=$stmt->execute();
