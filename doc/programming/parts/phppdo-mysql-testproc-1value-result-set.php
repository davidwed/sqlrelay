$stmt=$dbh->prepare("select * from exampleproc()");
$result=$stmt->execute();
