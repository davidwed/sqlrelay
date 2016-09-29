$stmt=$dbh->prepare("exec testproc");
$result=$stmt->execute();
