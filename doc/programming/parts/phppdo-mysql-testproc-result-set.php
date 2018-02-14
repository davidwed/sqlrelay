$stmt=$dbh->prepare("call exampleproc()");
$result=$stmt->execute();
