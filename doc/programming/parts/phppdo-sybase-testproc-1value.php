$stmt=$dbh->prepare("exec exampleproc");
$stmt->bindValue("@in1",1);
$stmt->bindValue("@in2","1.1");
$stmt->bindValue("@in3","hello");
$out1=0;
$stmt->bindParam("@out1",$out1,PDO::PARAM_INT|PDO::PARAM_INPUT_OUTPUT);
$result=$stmt->execute();
