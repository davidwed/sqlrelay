$stmt=$dbh->prepare("exec testproc");
$stmt->bindValue("@in1",1);
$stmt->bindValue("@in2","1.1");
$stmt->bindValue("@in3","hello");
$result=$stmt->execute();
