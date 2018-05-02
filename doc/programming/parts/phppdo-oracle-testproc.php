$stmt=$dbh->prepare("begin exampleproc(:in1,:in2,:in3); end;");
$stmt->bindValue(":in1",1);
$stmt->bindValue(":in2","1.1");
$stmt->bindValue(":in3","hello");
$result=$stmt->execute();
