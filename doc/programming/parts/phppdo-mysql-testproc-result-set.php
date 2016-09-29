$stmt=$dbh->prepare("call testproc()");
$result=$stmt->execute();
