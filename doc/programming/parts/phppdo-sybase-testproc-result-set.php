$stmt=$dbh->prepare("exec exampleproc");
$result=$stmt->execute();
